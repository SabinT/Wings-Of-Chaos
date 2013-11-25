#include <precompiled.h>

#include <CsGame.h>

#include <CsBulletManager.h>
#include <CsDebugDraw.h>
#include <CsPhysicsWorldObject.h>

#include <CsCameraManager.h>

#include <CsShipObject.h>
#include <CsShipManager.h>

#include <CsProjectileManager.h>
#include <CsExplosionManager.h>

#include <CsTerrainManager.h>
#include <CsGameLevel.h>

#include <CsGroundEnemyManager.h>
#include <CsGroundEnemy.h>

#include <CsGameLevel.h>
#include <CsObjectiveList.h>
#include <OgreBulletConverter.h>

#include <sstream>
std::stringstream shout;
//-------------------------------------
// test data

//--------------------------------------
CsGame* CsGame::pInstance = NULL;

CsGame::CsGame (	Ogre::Root* root,
					Ogre::RenderWindow* window,
					Ogre::Viewport* viewport,
					Ogre::SceneManager* sceneMgr)
{
	mRoot = root; mWindow = window; mViewport = viewport; mSceneMgr = sceneMgr;
	mShutdown = false;
	Ogre::LogManager::getSingletonPtr()->logMessage("*** CsGame Created ***");
	mBulletManager = NULL;
	mPausedTime = mCurTime = mPrevTime = 0;

	mGameState = PRE_GAME;
	mGamePaused = false;
}

CsGame::~CsGame()
{

}

//--------- some functions moved here to reduce dependencies
CsDiscreteDynamicsWorld* CsGame::GetDynamicsWorld() {
	return mBulletManager->GetDynamicsWorld();
}
//----------------------------------------------------------

void CsGame::Create (	Ogre::Root* root,
				Ogre::RenderWindow* window,
				Ogre::Viewport* viewport,
				Ogre::SceneManager* sceneMgr)
{
	if (!pInstance) pInstance = new CsGame(root, window, viewport, sceneMgr);
}

void CsGame::Destroy() {
	Ogre::LogManager::getSingletonPtr()->logMessage("*** CsGame Destroyed ***");
	if (pInstance) delete pInstance;
	pInstance = NULL;
}

// ----------------------------------------------------------------------
bool CsGame::Init() {
	// choose Scene Manager
    mSceneMgr = mRoot->createSceneManager(Ogre::ST_EXTERIOR_CLOSE, "CsSceneManager");
    // create viewport
    mDefaultCamera = mSceneMgr->createCamera("DummyCamera");
    mViewport = mWindow->addViewport(mDefaultCamera);

	//SetupBullet();
	//mEnableDebugDraw = true;
	mBulletManager = new CsBulletManager;
	mBulletManager->Init();
	mBulletManager->mEnableDebugDraw = false;

	// TODO: remove this
	mInputQueueLogic = new CsLockableQueue<CsInputEvent>;
	mInputQueueRender = new CsLockableQueue<CsInputEvent>;

	mInputManager = new CsInputManager;
	mInputManager->SetupInput(mWindow, &mRenderQueue, &mLogicTempQueue);

	// start the GUI
	mGameGUI = new CsGameGUI;
	mGameGUI->WriteToConsole("Welcome to the console!");

	mGameRunning = false;
	mGameState = PRE_GAME;
	mGamePaused = false;

	mBulletManager->mEnableDebugDraw = false;


	return true;
}

void CsGame::Cleanup(){

	delete mGameGUI;

	mInputManager->DestroyInput();
	delete mInputManager;
	delete mInputQueueLogic;
	delete mInputQueueRender;

	//DestroyBullet();
	mBulletManager->Cleanup();
	delete mBulletManager;

	mRoot->destroySceneManager(mSceneMgr);
	mWindow->removeAllViewports();
}

// bullet related ------------------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------------------------------

void CsGame::Logic() {
	mPrevTime = mCurTime = clock();

	mPausedTime = 0;

	mBulletManager->TweakParameters();

	while (!mShutdown) 
	{
		while(mGamePaused) {
			// but still do the time keeping!!
			mCurTime = clock();
			mPausedTime += mCurTime - mPrevTime;
			mPrevTime = mCurTime;
			boost::this_thread::yield();
		}
		
		mCurTime = clock();
	
		// calculate timeSinceLastFrame
		float timeSinceLastFrame = (mCurTime - mPrevTime)/(float)CLOCKS_PER_SEC;

		mBulletManager->GetDynamicsWorld()->stepSimulation(timeSinceLastFrame, 1);
		mBulletManager->ProcessCollisions();

		// synchronization placed here close to internal sync of stepSimulation
		// this one for the ones that dont have the luxury of motion states
		{
			boost::mutex::scoped_lock lock(mSyncMutex);
			mProjectileManager->Synchronize();
			mGameLevel->Synchronize();
			mCameraManager->UpdateSound();
		}

		ProcessLogicQueue();

		if (mGameState == RUNNING) 
			mGameLevel->Update();

		mProjectileManager->Update();

		// dispatch all the accumulated messages to Render
		{
			boost::mutex::scoped_lock lock(mRenderQueue.mMutex);

			while (!mRenderTempQueue.empty()) {
				mRenderQueue.push(mRenderTempQueue.front());
				mRenderTempQueue.pop();
			}
		}

		mPrevTime = mCurTime;
			
		if (mBulletManager->mEnableDebugDraw) 
		{
			{
				boost::mutex::scoped_lock lock(mBulletManager->mDebugDraw->mMutex);
				mBulletManager->mDebugPoints.clear();

				mBulletManager->mDynamicsWorld->debugDrawWorld();
			}
		} else
		{
			{
				boost::mutex::scoped_lock lock(mBulletManager->mDebugDraw->mMutex);
				mBulletManager->mDebugPoints.clear();
			}
		}
	}

}

// TODO: use frameRenderingqueued
// timesincelastframe in FrameEvent &evt
void CsGame::Render() {
	boost::this_thread::interruption_point();

	//ProcessInputRender();
	ProcessRenderQueue();
		
	{
		boost::mutex::scoped_lock lock(mSyncMutex);

		// TODO: do this for all game objects
		mGameLevel->UpdateSceneNodes();
		mProjectileManager->UpdateBillboards();
		mCameraManager->SynchronizeSound();
	}

	mCameraManager->Update();
	mExplosionManager->UpdateRender();

	if (mGameState == PRE_GAME) {
		mGameLevel->mObjectives->TourObjectives();
	}

	UpdateGUI();
	mGameGUI->Update();
	//boost::this_thread::interruption_point();

	{ // scope of disable interruption
		boost::this_thread::disable_interruption di;

			
		Ogre::ManualObject* debugLines;
		if (mBulletManager->mEnableDebugDraw) {
			debugLines = mRoot->getSceneManager("CsSceneManager")->createManualObject("DebugLines");
			// locking implemented in DrawDebugLines
			mBulletManager->DrawDebugLines(debugLines);
				
			mBulletManager->mDebugLinesNode->attachObject(debugLines);

			mRoot->renderOneFrame();

			mBulletManager->mDebugLinesNode->detachAllObjects();
			mRoot->getSceneManager("CsSceneManager")->destroyManualObject(debugLines);
		} else {
			mRoot->renderOneFrame();
		}

	} 
	// now enable interruption
}

void CsGame::UpdateGUI() {
	static Ogre::Vector3 sPos;

	shout.str("");
	btVector3 vel = mGameLevel->mPlayerShip->GetRigidBody()->getLinearVelocity();
	shout << "Player Velocity: " << vel.length() << std::endl;
	shout << "Player HP: " << mGameLevel->mPlayerShip->GetHP() << std::endl;

	if (mGameLevel->mPlayerShip->mTargetNode) {
		CsDestructibleObject *target = mGameLevel->mPlayerShip->mTarget;
		sPos = mCameraManager->ScreenPosition(target->GetNode()->_getDerivedPosition());
		if (sPos.z < 1.0f) {
			if (mGameLevel->mPlayerShip->GetTargetLocked()) {
				Ogre::OverlayManager::getSingleton().getByName("TargetLockedOverlay")->getChild("TargetLocked/Panel")->setPosition(sPos.x - 0.2, sPos.y - 0.2);
				Ogre::OverlayManager::getSingleton().getByName("TargetLockedOverlay")->show();
				Ogre::OverlayManager::getSingleton().getByName("TargetOverlay")->hide();
			} else {
				Ogre::OverlayManager::getSingleton().getByName("TargetOverlay")->getChild("Target/Panel")->setPosition(sPos.x - 0.2, sPos.y - 0.2);
				Ogre::OverlayManager::getSingleton().getByName("TargetOverlay")->show();
				Ogre::OverlayManager::getSingleton().getByName("TargetLockedOverlay")->hide();
			}
		} else {
			Ogre::OverlayManager::getSingleton().getByName("TargetOverlay")->hide();
		}

		shout << "Target Name: " << target->GetNode()->getName() << std::endl;
		shout << "Target Screen Position: " << sPos << std::endl;
		shout << "Target HP: " << target->GetHP() << std::endl;
		shout << "Target Armor: " << target->GetArmor() << std::endl;

		btVector3 center;	btScalar radius;
		//target->GetRigidBody()->getCollisionShape()->getBoundingSphere(center, radius);
		//shout << "Target Radius: " << radius << std::endl;
	} else {
		Ogre::OverlayManager::getSingleton().getByName("TargetOverlay")->hide();
		Ogre::OverlayManager::getSingleton().getByName("TargetLockedOverlay")->hide();
	}


	mGameGUI->ClearHistory();

	mGameGUI->WriteToConsole(shout.str().c_str());
}

void CsGame::SetupScene(const char* levelName) {
	// create the managers
	mProjectileManager = new CsProjectileManager;
	mExplosionManager = new CsExplosionManager;

	mProjectileManager->CreatePool();
	mExplosionManager->CreatePool();

	mGameLevel = new CsGameLevel;
	mGameLevel->Load(levelName);

	mCameraManager = new CsCameraManager(mSceneMgr, mViewport);
	//mCameraManager->Load(ogreScene);
	mCameraManager->Load("../../scenes/cameras.scene");
	mCameraManager->SetTarget(mGameLevel->mPlayerShip);
	mCameraManager->SetCamera("Orbit");
	mCameraManager->SetOrbitSpeed(0.785);

	mCameraManager->SetBackMusic(GameSounds::GAME_MUSIC);
	mCameraManager->PlayBackMusic();

}

void CsGame::DestroyScene() {
	mCameraManager->StopBackMusic();
	mCameraManager->UnLoad();
	mExplosionManager->Unload();
	mProjectileManager->Unload();
	
	mGameLevel->Unload();
	delete mGameLevel;
	
	mSceneMgr->clearScene();

	delete mCameraManager;
	delete mProjectileManager;
	delete mExplosionManager;
}


// ----------------------------- Input Processiing --------------------------------------------------
void CsGame::ProcessInputLogic(CsInputEvent evt) {
	static int xAbs = 0;
	static int yAbs = 0;
	static int zAbs = 0;
	static int wAbs = 0;

	switch (evt.type) {
	case Chaos::BUTTON_DOWN:
		switch (evt.info.buttonInfo.buttonId) {
		case Chaos::FORWARD:
			mGameLevel->mPlayerShip->SetThrottle();
			break;
		case Chaos::BACKWARD:
			mGameLevel->mPlayerShip->SetDamping();
			break;			
		case Chaos::UP:
			mGameLevel->mPlayerShip->Pitch(200);
			break;
		case Chaos::DOWN:
			mGameLevel->mPlayerShip->Pitch(-200);
			break;
		case Chaos::RIGHT:
			mGameLevel->mPlayerShip->Roll(200);
			break;
		case Chaos::LEFT:
			mGameLevel->mPlayerShip->Roll(-200);
			break;

		case Chaos::FIRE1:
			break;
		case Chaos::FIRE2:
			break;

		case Chaos::LOCK_TARGET:
			mGameLevel->mPlayerShip->ToggleTargetLock();
			break;
		case Chaos::DEBUG2:
			//mGameLevel->mShipManager->Create(std::string("BlueShip"));
			break;
		case Chaos::ESCAPE:
			//exit(0);
			InjectShutdown();
			break;
		}
		break;

	case Chaos::BUTTON_UP:
		switch (evt.info.buttonInfo.buttonId) {
		case Chaos::FORWARD:
			mGameLevel->mPlayerShip->ResetThrottle();
			break;
		case Chaos::BACKWARD:
			mGameLevel->mPlayerShip->ResetDamping();
			break;			
		case Chaos::UP:
			mGameLevel->mPlayerShip->Pitch(0);
			break;
		case Chaos::DOWN:
			mGameLevel->mPlayerShip->Pitch(0);
			break;
		case Chaos::RIGHT:
			mGameLevel->mPlayerShip->Roll(0);
			break;
		case Chaos::LEFT:
			mGameLevel->mPlayerShip->Roll(0);
			break;
		}
		break;

	case Chaos::AXIS_MOVED:
		switch (evt.info.axisInfo.axisId) {
		case Chaos::AXIS_X:
			xAbs += evt.info.axisInfo.axisValue;
			break;
		case Chaos::AXIS_Y:
			yAbs += evt.info.axisInfo.axisValue;
			break;
		case Chaos::AXIS_Z:
			zAbs += evt.info.axisInfo.axisValue;
			break;
		case Chaos::AXIS_W:
			wAbs = evt.info.axisInfo.axisValue;
			break;
		}


	default:
		break;
	}

	if (xAbs > 51) xAbs = 51; if (xAbs < -51) xAbs = -51;
	if (yAbs > 51) yAbs = 51; if (yAbs < -51) yAbs = -51;
	//mPlayerShip->Roll(xAbs);
	//mPlayerShip->Pitch(yAbs);

}

void CsGame::ProcessInputRender(CsInputEvent evt) {
	mCameraManager->InjectInput(evt);

	switch (evt.type) {
	case Chaos::BUTTON_DOWN:
		switch (evt.info.buttonInfo.buttonId) {
		case Chaos::FORWARD:
			break;
		case Chaos::BACKWARD:
			break;
		case Chaos::ESCAPE:
			break;
		case Chaos::SHOW_OBJECTIVES:
			mGameLevel->mObjectives->FlashObjectives();
			break;
		case Chaos::DEBUG2:
			if (mGameState == PRE_GAME) {
				mCameraManager->SetTarget(GetPlayerShip());
				mGameLevel->MakeHostile();
				mGameLevel->mPlayerShip->SetArmor(0.02);
				mCameraManager->SetCamera("Chase");
				mGameState = RUNNING;
			}
			break;
		}
		break;

	case Chaos::BUTTON_UP:
		switch (evt.info.buttonInfo.buttonId) {
		case Chaos::FORWARD:
			break;
		case Chaos::BACKWARD:
			break;
		}
		break;
	default:
		break;
	}
}

void CsGame::ProcessGUILogic(CsGUIEvent evt) {
	switch (evt.type) {
	case CsGUIEvent::DEBUG_TOGGLE_REQUEST:
		mBulletManager->mEnableDebugDraw = !mBulletManager->mEnableDebugDraw;
		break;
	case CsGUIEvent::CREATE_SHIP:
		mGameLevel->mShipManager->Create(std::string("RedShip"));
		break;
	case CsGUIEvent::QUIT_REQUEST:
		mShutdown = true;
		break;

	default:
		break;
	}
}

void CsGame::ProcessGameLogic(CsGameEvent evt) {
	switch(evt.type) {
	case CsGameEvent::OBJECT_DESTROYED:
		if (evt.info.destroyedObject->GetRigidBody()->isInWorld()) mBulletManager->GetDynamicsWorld()->removeRigidBody(evt.info.destroyedObject->GetRigidBody());
		//if(!evt.info.destroyedObject->IsActive()) {
			//evt.info.destroyedObject->GetRigidBody()->setActivationState(DISABLE_SIMULATION);
			//evt.info.destroyedObject->GetRigidBody()->setCollisionFlags(evt.info.destroyedObject->GetRigidBody()->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
		//}
		break;

	case CsGameEvent::MISSION_COMPLETE:
		mGameState = POST_GAME;
		// InjectShutdown();
		break;
	case CsGameEvent::MISSION_FAIL:
		// InjectShutdown();
		mGameState = POST_GAME;
		break;
	}
}

void CsGame::ProcessGameRender(CsGameEvent evt) {
	switch(evt.type) {
	case CsGameEvent::MISSION_COMPLETE:
		mGameGUI->ShowMessage(std::string("Congratulations!!"), std::string("You have rescued the earthlings from Santo's evil clutches! May God Bless You!! Say the magic word SSSHAABLAGOO!!"));
		break;
	case CsGameEvent::MISSION_FAIL:
		mGameGUI->ShowMessage(std::string("jESUS cHrIST yOu S*UCK!!"), std::string("Shit man.. what the hell have you done?? \n ... \nWell we TRUSTED you.. you Santo's biatch!!\n ...sigh"));
		break;
	}
}

void CsGame::ProcessLogicQueue () {
	boost::mutex::scoped_lock lock(mLogicQueue.mMutex);
	
	while (!mLogicQueue.empty()) {
		CsLogicMessage msg = mLogicQueue.front();
		mLogicQueue.pop();

		switch (msg.type) {
		case CsLogicMessage::INPUT_EVENT:
			// if (mGameState == RUNNING)
			ProcessInputLogic(msg.info.input);
			break;
		case CsLogicMessage::GUI_EVENT:
			ProcessGUILogic(msg.info.guiEvent);
			break;
		case CsLogicMessage::GAME_EVENT:
			ProcessGameLogic(msg.info.gameEvent);
		default:
			break;
		}
	}

	// extra input processing
	//if (mGameState == RUNNING) {
		if (mInputManager->GetButtonState(Chaos::FIRE1)) mGameLevel->mPlayerShip->FirePrimary();
		if (mInputManager->GetButtonState(Chaos::FIRE2)) mGameLevel->mPlayerShip->FireSecondary();
	//}
}

void CsGame::ProcessGUIRender(CsGUIEvent evt) {
	switch (evt.type) {
	case CsGUIEvent::PAUSE_REQUEST:
		mGamePaused = true;
		mInputManager->DisableInputLogic(true);
		break;

	case CsGUIEvent::RESUME_REQUEST:
		mGamePaused = false;
		mInputManager->DisableInputLogic(false);
		break;

	case CsGUIEvent::DEBUG_CLEAR:
		mGameGUI->ClearHistory();
		break;

	case CsGUIEvent::DEBUG_PRINT:
		mGameGUI->WriteToConsole(evt.info.str);
		break;
	case CsGUIEvent::SHIP_CAMERA_CHANGE:
		{
			CsShipObject* nextShip = mGameLevel->mShipManager->NextShip();
			if (nextShip) {
				mCameraManager->SetTarget(nextShip);
			}
		}
		break;
	default:
		break;
	}
}

void CsGame::ProcessRenderQueue () 
{

	boost::mutex::scoped_lock lock(mRenderQueue.mMutex);
	
	while (!mRenderQueue.empty()) {
		CsRenderMessage msg = mRenderQueue.front();
		mRenderQueue.pop();

		switch (msg.type) {
		case CsRenderMessage::INPUT_EVENT:
			ProcessInputRender(msg.info.input);
			break;

		case CsRenderMessage::GUI_EVENT:
			ProcessGUIRender(msg.info.guiEvent);
			break;

		case CsRenderMessage::CREATE_BILLBOARD:
			*(msg.info.bbInfo.bbPtr) = msg.info.bbInfo.bbSet->createBillboard(mCameraManager->GetCurrentCameraNode()->_getDerivedPosition());
			break;

		case CsRenderMessage::DESTROY_BILLBOARD:
			{
				Ogre::Billboard* tmp = *(msg.info.bbInfo.bbPtr);
				if (tmp->mParentSet)
					tmp->mParentSet->removeBillboard(tmp);	
			}
			break;

		case CsRenderMessage::SHOW_NODE:
			msg.info.sceneNode->setVisible(true);
			break;

		case CsRenderMessage::HIDE_NODE:
			msg.info.sceneNode->setVisible(false);
			break;

		case CsRenderMessage::ATTACH_NODE:
			if(!msg.info.nodeInfo.child->getParent())
				msg.info.nodeInfo.parent->addChild(msg.info.nodeInfo.child);
			break;

		case CsRenderMessage::DETACH_NODE:
			if (msg.info.nodeInfo.child->getParent()) 
				msg.info.nodeInfo.child->getParent()->removeChild(msg.info.nodeInfo.child);
			//msg.info.nodeInfo.parent->removeChild(msg.info.nodeInfo.child);
			break;

		case CsRenderMessage::ATTACH_OBJECT:
			msg.info.objectInfo.parent->attachObject(msg.info.objectInfo.obj);
			break;

		case CsRenderMessage::DETACH_OBJECT:
			msg.info.objectInfo.parent->detachObject(msg.info.objectInfo.obj);
			break;

		case CsRenderMessage::START_PARTICLE_FX:
			msg.info.particleFX->setEmitting(true);
			break;

		case CsRenderMessage::STOP_PARTICLE_FX:
			msg.info.particleFX->setEmitting(false);
			break;

		case CsRenderMessage::CREATE_PROJECTILE:
			mProjectileManager->CreateRender(msg.info.projectile);
			break;

		case CsRenderMessage::DESTROY_PROJECTILE:
			msg.info.projectile->DestroyRender();
			break;

		case CsRenderMessage::CREATE_EXPLOSION:
			mExplosionManager->CreateRender(msg.info.explosion.explosion, msg.info.explosion.x, msg.info.explosion.y, msg.info.explosion.z, msg.info.explosion.radius);
			break;

		//case CsRenderMessage::SHOW_DOT:
		//	msg.info.dot->show();
		//	break;

		//case CsRenderMessage::HIDE_DOT:
		//	msg.info.dot->hide();
		//	break;
		
		case CsRenderMessage::GAME_EVENT:
			ProcessGameRender(msg.info.gameEvent);
			break;

		default:
			break;
		}
	}
}

//----------------------------------------- Run function --------------------------------------------
void CsGame::InputControl() {
	  
	while(1) {
		mInputManager->CaptureInput();


		boost::this_thread::interruption_point();

		/*if (mLogicThread->get_id() == notAnyThread) {
			mRenderThread->interrupt();
			exit(0);
			break;
		}*/
		
    }
}

void CsGame::ThreadControl()  {
	mLogicThread->join();
	
	//mInputThread->interrupt();
	//mInputThread->join();
}

int CsGame::Run()
{
	Init();

	
	SetupScene("spawntest");
	
	//mRoot->renderOneFrame();
	// create threads
	
	mLogicThread  = new boost::thread(&CsGame::Logic, this);
	//mInputThread = new boost::thread(&CsGame::InputControl, this);
	mControlThread  = new boost::thread(&CsGame::ThreadControl, this);

	mBulletManager->GetDynamicsWorld()->stepSimulation(0.0);

	mGameLevel->Update();
	mGameLevel->Synchronize();
	mGameLevel->UpdateSceneNodes();


	//terminate the control thread 
	boost::thread::id notAny;
	while (1)  {
		Ogre::WindowEventUtilities::messagePump();

		// if logic thread quits then wait for the reaper
		if ( mLogicThread->get_id() == notAny)  {
				//mControlThread->interrupt();
				mControlThread->join();
				break;
		}

		mInputManager->CaptureInput();
		Render();
		
		// dispatch accumulated messages to the Logic part
		{
			boost::mutex::scoped_lock lock(mLogicQueue.mMutex);
			while (!mLogicTempQueue.empty()) {
				mLogicQueue.push(mLogicTempQueue.front());
				mLogicTempQueue.pop();
			}
		}

	}


	// TODO: termination of threads, exception handling etc
	// TODO: disableInterruption objects in thread functions
	// destroy threads

	delete mControlThread;
	delete mLogicThread;
	//delete mInputThread;

	DestroyScene();

	// ----------------------------------------------------------------------------------------------

	
	//------------------------------------------------------------------------------------------------

	Cleanup();
    return 17;
}

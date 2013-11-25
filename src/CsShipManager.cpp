#include <precompiled.h>

#include <CsShipManager.h>
#include <CsGame.h>
#include <CsRigidBody.h>
#include <CsShipObject.h>

#include <sstream>
#include <iomanip>
#include <ctime>

// some random numbers
#include <cstdlib>
#include <OgreBulletConverter.h>

#include <OgreMaterialManager.h>

std::stringstream ss;

CsShipManager::CsShipManager(void)
{
	mOgreScene =  NULL;
	mPhysicsScene = NULL;
	mPhysicsFile = NULL;

}


CsShipManager::~CsShipManager(void)
{

}

void CsShipManager::LoadTemplates(const char* ogreFilename, const char* bulletFilename, const char *prefix)  {
	mOgreScene = new OgreMax::OgreMaxScene;
	Ogre::SceneNode *ogreScene = CsGame::GetSingletonPtr()->GetSceneMgr()->createSceneNode("DummyShipNode");
	mOgreScene->Load(ogreFilename, NULL, 0, CsGame::GetSingletonPtr()->GetSceneMgr(), ogreScene, NULL, "General");

	mPhysicsScene = new CsBulletWorldImporter(NULL);
	mPhysicsFile = new bParse::btBulletFile(bulletFilename);
	mPhysicsScene->loadFileFromMemory(mPhysicsFile);

	LoadTemplates(ogreScene, mPhysicsScene, prefix);
}

btBoxShape* defaultShipShape;
void CsShipManager::LoadTemplates(Ogre::SceneNode *ogreScene, btBulletWorldImporter *bulletScene, const char *prefix)  {
	defaultShipShape = new btBoxShape(btVector3(10,1.5,5));

	Ogre::Node::ChildNodeIterator it = ogreScene->getChildIterator();

	std::vector<Ogre::SceneNode*> nodesToRemove;
	
	Ogre::SceneNode *tempNode;
	btRigidBody *tempBody;
	Ogre::String btString;


	while (it.hasMoreElements()) {
		tempNode = static_cast <Ogre::SceneNode*> (it.getNext());
		assert(tempNode);

		tempBody = NULL;

		Ogre::String name = tempNode->getName();

		// make sure this node is the perfect/main model
		if ((tempNode->getName().find(prefix) != std::string::npos) && 
			(tempNode->getName().find("damaged") == std::string::npos) &&
			(tempNode->getName().find("destroyed") == std::string::npos) &&
			(tempNode->getName().find("leftwing") == std::string::npos) &&
			(tempNode->getName().find("rightwing") == std::string::npos))
		{
			btString = "bt" + tempNode->getName();
			tempBody = bulletScene->getRigidBodyByName(btString.c_str());
			
			if (!tempBody) {
				btString = tempNode->getName();
				tempBody = bulletScene->getRigidBodyByName(btString.c_str());
			}

			if (tempBody) {
				CsShipObject *tempObj = new CsShipObject;


				Ogre::Vector3 pos = tempNode->getPosition();
				Ogre::Quaternion rot = tempNode->getOrientation();
				btTransform trans(btQuaternion(rot.x, rot.y, rot.z, rot.w), btVector3(pos.x, pos.y, pos.z));
				CsMotionState *motionState = new CsMotionState(tempObj, trans, tempBody->getCenterOfMassTransform());
				tempBody->setMotionState(motionState);

				tempObj->SetNode(CsGame::GetSingletonPtr()->GetSceneMgr()->createSceneNode(name + "_mainnode"));
				
				tempNode->setPosition(0,0,0);
				tempNode->setOrientation(1,0,0,0);
				tempObj->SetPerfectModel(tempNode);

				tempObj->SetRigidBody(tempBody);
				
				{
					Ogre::Node::ChildNodeIterator it2 = ogreScene->getChildIterator();

					Ogre::SceneNode *tempNode2;
					bool damagedFound = false, destroyedFound = false;
					bool rightFound = false, leftFound= false;

					Ogre::String damagedName = tempNode->getName() + "_damaged";
					Ogre::String destroyedName = tempNode->getName() + "_destroyed";
					Ogre::String rightName = tempNode->getName() + "_rightwing";
					Ogre::String leftName = tempNode->getName() + "_leftwing";

					while (it2.hasMoreElements()) {
						tempNode2 = static_cast <Ogre::SceneNode*> (it2.getNext());

						if ((tempNode2->getName() == damagedName)) {
							nodesToRemove.push_back(tempNode2);

							tempNode2->setPosition(0,0,0);
							tempNode2->setOrientation(1,0,0,0);
							tempObj->SetDamagedModel(tempNode2);
							damagedFound = true;
						}
						if ((tempNode2->getName() == destroyedName)) {
							nodesToRemove.push_back(tempNode2);

							tempNode2->setPosition(0,0,0);
							tempNode2->setOrientation(1,0,0,0);
							tempObj->SetDestroyedModel(tempNode2);
							destroyedFound = true;
						}
						// better do this from object extra data
						if ((tempNode2->getName() == rightName)) {
							nodesToRemove.push_back(tempNode2);
							tempNode2->detachAllObjects();

							tempObj->SetRightWing(tempNode2);
							rightFound = true;
						}
						if ((tempNode2->getName() == leftName)) {
							nodesToRemove.push_back(tempNode2);
							tempNode2->detachAllObjects();

							tempObj->SetLeftWing(tempNode2);
							leftFound = true;
						}
					}
					if (!damagedFound) tempObj->SetDamagedModel(tempNode);
					if (!destroyedFound) tempObj->SetDestroyedModel(tempNode);
					if (!rightFound) {
						tempObj->SetRightWing(CsGame::GetSingletonPtr()->GetSceneMgr()->createSceneNode(name + "_leftwing"));
						tempObj->GetRightWing()->translate(5.0,0,0);
					}
					if (!leftFound) {
						tempObj->SetLeftWing(CsGame::GetSingletonPtr()->GetSceneMgr()->createSceneNode(name + "_rightwing"));
						tempObj->GetLeftWing()->translate(-5.0,0,0);
					}
				}

				mShipMap[tempNode->getName()] = tempObj;
				mTemplates.push_back(tempObj);	

				// create particle effects done during CreatePool just after cloning
			}
			
			nodesToRemove.push_back(tempNode);
		}
	}

	std::vector<Ogre::SceneNode*>::iterator removeIt = nodesToRemove.begin();
	while (removeIt != nodesToRemove.end()) {
		ogreScene->removeChild(*removeIt);
		removeIt++;
	}

	// node consistency not needed for templates
}


void CsShipManager::CreatePool(int poolSize) {
	std::stringstream ss;

	// create equal no of ships of each type
	assert(mTemplates.size());
	int num = poolSize / mTemplates.size();

	std::map<std::string, CsShipObject*>::iterator it;
	for (it = mShipMap.begin(); it !=  mShipMap.end(); it++) {
		for (int j = 0; j < num; j++) {
			ss.str("");
			ss << it->first << std::setw(2) << std::setfill('0') << j;

			CsShipObject *newShip = it->second->Clone(ss.str());

			newShip->mTrackerNode = newShip->GetNode()->createChildSceneNode(newShip->GetNode()->getName() + "Tracker");
			newShip->mTrackerNode->setInheritOrientation(false);

			newShip->SetShipType(it->first);
			//newShip->SetShipStatus(CsShipObject::INACTIVE);

			newShip->mFSM = new CsStateMachine(newShip);

			newShip->mJetSound.LoadSource(GameSounds::JET_ENGINE);
			newShip->mJetSound.SetGain(5);
			newShip->mRicochet.LoadSource(GameSounds::RICOCHET);
			newShip->mRicochet.SetGain(5);
			newShip->mCollisionSound.LoadSource(GameSounds::COLLISION);
			newShip->mCollisionSound.SetGain(5);

			// particle effects
			newShip->mJetEngineFX = CsGame::GetSingletonPtr()->GetSceneMgr()->createParticleSystem(newShip->GetNode()->getName(), "JetEngine");
			newShip->mNode->attachObject(newShip->mJetEngineFX);

			// TODO, re-enable overlay
			// stimalsena: re-enabled
			//newShip->mDot = (Ogre::OverlayContainer*) Ogre::OverlayManager::getSingleton().createOverlayElement("Panel", newShip->GetNode()->getName() + "_dot");
			//newShip->mDot->setDimensions(0.225 / 32, .3 / 32.0);
			//newShip->mDot->setMaterialName("RedDot");
			//// Ogre::OverlayManager::getSingleton().getByName("MiniMapOverlay")->add2D(newShip->mDot);
			//newShip->mDot->hide();
			// -------------------------
			newShip->SetActive(false);
			push_back(newShip);
		}
	}
}

void CsShipManager::Unload() {
	// destroy the pool
	for (int i = 0; i < size(); i++) {
		(*this)[i]->Unload();
		delete (*this)[i];
	}

	// the templates are cleaned up by the file, but still need to clean up the motion state we created
	for (int i = 0; i < mTemplates.size(); i++) {
		delete mTemplates[i]->GetRigidBody()->getMotionState();
	}

	clear();

	delete defaultShipShape;

	if (mOgreScene) {
		mOgreScene->Destroy();
		delete mOgreScene;
	}
	
	if (mPhysicsScene) {
		mPhysicsScene->deletePhysicsData();
		delete mPhysicsScene;
	}
	
	if (mPhysicsFile) delete mPhysicsFile;
}

// -------------------------------- FACTORY METHOD -------------------------------------
CsShipObject* CsShipManager::Create(std::string &shipType) {
	if (mShipMap.count(shipType) == 0) return NULL;

	for (int index = 0; index < mSpawnPoints.size(); index++) 
	{
		SpawnPoint *spn = &mSpawnPoints[index];
		if (spn->mLastUsedTime + spn->mCoolDownTime > (CsGame::GetSingletonPtr()->GetGameTime())) continue; // SPAWN POINT NOT READY
		//if (spn->mLastUsedTime + spn->mCoolDownTime > (clock() / (float)CLOCKS_PER_SEC)) continue; // SPAWN POINT NOT READY
		else
		{
			// check if a ship of requested type is INACTIVE
			bool found = false;
			for (int i = 0; i < size(); i++) 
			{
				CsShipObject *ship = (*this)[i];
				if (ship->GetShipType() != shipType) continue;
				if (!ship->IsActive()) 
				{
					// recycle this ship

					// set position orientation from spawn point
					ship->GetRigidBody()->setWorldTransform(btTransform(spn->mOrientation, spn->mPosition));
					
					// eject with a velocity
					btQuaternion btQuat = ship->GetRigidBody()->getOrientation();

					Ogre::Quaternion quat(btQuat.w(), btQuat.x(), btQuat.y(), btQuat.z());
					Ogre::Vector3 ogreZ = quat.zAxis();
					btVector3 zAxis = btVector3(ogreZ.x, ogreZ.y, ogreZ.z);

					// tweak this eject velocity later
					ship->GetRigidBody()->setLinearVelocity(-zAxis * 20);
					

					if (!ship->mRigidBody->isInWorld()) CsGame::GetSingletonPtr()->GetDynamicsWorld()->addRigidBody(ship->GetRigidBody(), Chaos::SHIP_OBJECT, Chaos::SHIP_MASK);
					ship->SetCollidesWith(Chaos::SHIP_MASK);
					
					// grabity??
					ship->GetRigidBody()->setGravity(btVector3(0, 0,0));
					ship->GetRigidBody()->setActivationState(DISABLE_DEACTIVATION);

					ship->mSmokeFX->setEmitting(false);

					ship->Spawn();
					spn->mLastUsedTime = CsGame::GetSingletonPtr()->GetGameTime();
					//spn->mLastUsedTime = clock() / (float) CLOCKS_PER_SEC;

					return ship;
				}
			}
			// (ships all used)
			mPending.push_back(shipType);
			return NULL;	
		}

	}

	// (no spawn points ready)
	//add to queue
	mPending.push_back(shipType);
	return NULL;
}

void CsShipManager::LoadSpawnPoints (Ogre::SceneNode *ogreScene, const char *prefix) {
	Ogre::Node::ChildNodeIterator it = ogreScene->getChildIterator();
	
	Ogre::SceneNode *tempNode;
	Ogre::String btString;

	mEntryPos = btVector3(0,1000,2000);
	mEntryRot = btQuaternion();

	while (it.hasMoreElements()) {
		tempNode = static_cast <Ogre::SceneNode*> (it.getNext());
		assert(tempNode);

		if (tempNode->getName().find("Entry") != std::string::npos) {
			mEntryPos = OgretoBullet(tempNode->getPosition());
			mEntryRot = OgretoBullet(tempNode->getOrientation());
		}


		if ((tempNode->getName().find(prefix) != std::string::npos)) {
			Ogre::Vector3 pos = tempNode->getPosition();
			Ogre::Quaternion quat = tempNode->getOrientation();

			SpawnPoint spn;
			spn.mPosition = btVector3(pos.x, pos.y, pos.z);
			spn.mOrientation = btQuaternion(quat.x, quat.y, quat.z, quat.w);
			// get cooldown time from ogreScene extrainfo
			spn.mCoolDownTime = 4;
			spn.mLastUsedTime = -4;

			mSpawnPoints.push_back(spn);
		}
	}

	if (mSpawnPoints.size() == 0) {
		SpawnPoint spn;
		spn.mPosition = btVector3(-2000,500,-2000);
		spn.mOrientation = btQuaternion();
		// get cooldown time from ogreScene extrainfo
		spn.mCoolDownTime = 4;
		spn.mLastUsedTime = -4;

		mSpawnPoints.push_back(spn);
	}

}

//--------------------------------- update part ---------------------------------------------------------
void CsShipManager::Update() {
	// check the pending creation queue
	std::vector<std::string> tempQueue = mPending;
	mPending.clear();

	for (int i = 0; i < tempQueue.size(); i++) {
		Create(tempQueue[i]);
	}


	// update the ships
	for (int i = 0; i < size(); i++) {
		if ((*this)[i]->IsActive())
			(*this)[i]->Update();
	}
}

void CsShipManager::UpdateSceneNodes() {
	for (int i = 0; i < size(); i++) {
		if ((*this)[i]->IsActive())
			(*this)[i]->UpdateSceneNode();
	}
}

CsShipObject* CsShipManager::NextShip() {
	static int i = 0;

	do {
		i = (i + 1) % this->size();
	} while(!(*this)[i]->IsActive());

	if ((*this)[i]->IsActive())
		return (*this)[i];

	return NULL;
}
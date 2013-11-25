#include <precompiled.h>

#include "CsMenu.h"
#include <CsCameraManager.h>

CsMenu::~CsMenu()
{
    //dtor
}

int CsMenu::Run() {
	Init();

	ShowIntro();

	clock_t prevTime = clock();

	clock_t curTime = clock();
	float deltaTime = 0;

	mFader->SetMaterial("MenuBackMaterial");
	mFader->SetOperation(CsFader::Operation::FADEOUT, 1.0, 3.0, 0.0);	

	while (!mShutdown && mRunning) {
		mFader->Fade(deltaTime);
		Ogre::WindowEventUtilities::messagePump();
		mInputManager->CaptureInput();

		DoRender();

		curTime = clock();
		deltaTime = (curTime - prevTime)/(float)CLOCKS_PER_SEC;
		prevTime = curTime;
	}

	Cleanup();
    return mResult;
}

//---------------------- Input Listeners ----------------------------------------------------------------
// OIS::KeyListener
bool CsMenu::keyPressed( const OIS::KeyEvent &arg ) {
	switch (arg.key) {
	case OIS::KC_ESCAPE:
		mResult = CsMenuResult::MENU_EXIT;
		InjectShutdown();
		mRunning = false;
		break;
	case OIS::KC_RETURN:
		mResult = CsMenuResult::MENU_START_GAME;
		mRunning = false;
		break;
	default: 
		break;
	}
	return true;
}
bool CsMenu::keyReleased( const OIS::KeyEvent &arg ) {return true;}
// OIS::MouseListener
bool CsMenu::mouseMoved( const OIS::MouseEvent &arg ) {return true;}
bool CsMenu::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id ) {return true;}
bool CsMenu::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id ) {return true;}


//------------------------- Init-----------------
bool CsMenu::Init() {
	// setup scenemanagers
    mSceneMgr = mRoot->createSceneManager(Ogre::ST_EXTERIOR_CLOSE, "CsSceneManager");

    // create viewport
    mDefaultCamera = mSceneMgr->createCamera("DummyCamera");
    mViewport = mWindow->addViewport(mDefaultCamera);

	//lights
	Ogre::Light* directionalLight = mSceneMgr->createLight("directionalLight");
    directionalLight->setType(Ogre::Light::LT_DIRECTIONAL);
    directionalLight->setDiffuseColour(Ogre::ColourValue(0.7, 0.7, 0.7));
    directionalLight->setSpecularColour(Ogre::ColourValue(.25, .25, 0.1));
	directionalLight->setCastShadows(true);
 
    directionalLight->setDirection(Ogre::Vector3(-1, -1, -1)); 

	mSceneMgr->setAmbientLight(Ogre::ColourValue(0.35,0.35,0.35));

	mMenuScene = new OgreMax::OgreMaxScene;
	Ogre::SceneNode* menuNode = mSceneMgr->createSceneNode("MenuSceneNode");
	mMenuScene->Load("../../levels/menu/menu.scene",NULL, 0, mSceneMgr, menuNode, NULL, "MenuResources");

	//sky
	mSceneMgr->setSkyBox(true, "SpaceSky", 100.0f, true);

	mCameraManager = new CsCameraManager(mSceneMgr, mViewport);
	mCameraManager->Load(menuNode);
	//mCameraManager->Load(ogreScene);
	//mCameraManager->Load("../../scenes/cameras.scene");
	mCameraManager->SetTarget(mSceneMgr->createSceneNode("MenuCenter"));
	mCameraManager->SetCamera("Orbit");
	mCameraManager->SetOrbitSpeed(0.15);
	

	mInputManager = new CsInputManager;
	mInputManager->SetupInput(mWindow, this, this);

	mFader = new CsFader;

	// load the overlays, etc
	Ogre::OverlayManager::getSingleton().getByName("MenuOverlay")->show();

	mFader->SetAlpha("ChaosStudiosMaterial", 0.0);
	mFader->SetAlpha("OgreBulletMaterial", 0.0);
	mFader->SetAlpha("WingsOfChaosMaterial", 0.0);
	mFader->SetAlpha("LoadingMaterial", 0.0);
	mFader->SetAlpha("PressEnterMaterial", 0.0);
	mFader->SetAlpha("MenuBackMaterial", 1.0);

	// sound listenre
	mListener = new CsListener;
	mCameraManager->SetBackMusic(GameSounds::MENU_MUSIC);
	mCameraManager->PlayBackMusic();

	return true;
}

void CsMenu::Cleanup() {
	FadeOut("PressEnterMaterial", 0.5f, 0.0f);
	FadeOut("WingsOfChaosMaterial", 0.75f, 0.0f);
	
	if (mResult == CsMenuResult::MENU_START_GAME) FadeIn("LoadingMaterial", 1.0f, 0.1f);

	//Ogre::OverlayManager::getSingleton().getByName("MenuOverlay")->hide();


	delete mListener;

	delete mFader;

	mInputManager->DestroyInput();
	delete mInputManager;

	mCameraManager->DestroyCameras();
	mCameraManager->StopBackMusic();
	delete mCameraManager;

	mMenuScene->Destroy();
	delete mMenuScene;

	mRoot->destroySceneManager(mSceneMgr);
	mWindow->removeAllViewports();
}

//------------------------------------------
void CsMenu::ShowIntro() {

	FadeIn("ChaosStudiosMaterial", 1.0f, 3.0f);
	FadeOut("ChaosStudiosMaterial", 1.0f, 1.0f);

	FadeIn("OgreBulletMaterial", 1.0f, 1.0f);
	FadeOut("OgreBulletMaterial", 1.0f, 1.0f);

	FadeIn("WingsOfChaosMaterial", 1.0f, 1.0f);
	//FadeOut("WingsOfChaosMaterial", 1.0f);

	FadeIn("PressEnterMaterial", 1.0f, 1.0f);

	//FadeOut("ChaosStudiosMaterial", 1.0f);

	return;
}

void CsMenu::FadeOut(const char* materialName, float duration, float  wait, float sAlpha) {
	clock_t prevTime = clock();

	clock_t curTime = clock();
	float deltaTime = 0;

	mFader->SetMaterial(materialName);
	mFader->SetOperation(CsFader::Operation::FADEOUT, sAlpha, duration, wait);	

	while (mFader->Fade(deltaTime)) {
		DoRender();

		curTime = clock();
		deltaTime = (curTime - prevTime)/(float)CLOCKS_PER_SEC;
		prevTime = curTime;
	}
}

void CsMenu::FadeIn(const char* materialName, float duration, float wait, float sAlpha) {
	clock_t prevTime = clock();

	clock_t curTime = clock();
	float deltaTime = 0;

	mFader->SetMaterial(materialName);
	mFader->SetOperation(CsFader::Operation::FADEIN, sAlpha, duration, wait);	
	while (mFader->Fade(deltaTime)) {
		DoRender();

		curTime = clock();
		deltaTime = (curTime - prevTime)/(float)CLOCKS_PER_SEC;
		prevTime = curTime;
	}
}

void CsMenu::DoRender() {
	mRoot->renderOneFrame();
	mCameraManager->Update();
	mCameraManager->SynchronizeSound();
	mCameraManager->UpdateSound();
}
#include <precompiled.h>

#include <OgreMax\OgreMaxScene.hpp>

#include <CsGame.h>
#include <CsCameraManager.h>
#include <CsTerrainManager.h>
#include <CsShipObject.h>

#include <cassert>
#include <ctime>

const float NEAR_CLIP_DISTANCE = 0.1;
const float FAR_CLIP_DISTANCE = 5000;

const float MOVE_SPEED = 300;
const float LOOK_SPEED = 5;

const float ORBIT_CAM_DISTANCE = 2.5;
const float ORBIT_CAM_HEIGHT = 1.5;

CsCameraManager::CsCameraManager (Ogre::SceneManager* sceneMgr, Ogre::Viewport *viewport) {
	mSceneMgr = sceneMgr;
	mViewport = viewport;
	mTargetNode = NULL;
	mTargetObject = NULL;

	for (int i = 0; i < Chaos::NUM_BUTTONS; i++) mButtonState[i] = false;
	mCurrentCamera = 0;

	mOrbitSpeed = 0.785;
}

void CsCameraManager::Load (const char* filename) {
	mOgreScene = new OgreMax::OgreMaxScene;
	Ogre::SceneNode *ogreScene = mSceneMgr->createSceneNode("DummyCameraManagerNode");
	mOgreScene->Load(filename, NULL, 0, mSceneMgr, ogreScene, NULL, "General");
	
	Load (ogreScene);
}

void CsCameraManager::Load (Ogre::SceneNode* ogreScene) {
	UnLoad();
	SetupDefaultCamera();

	if (ogreScene == NULL) return;

	Ogre::Node::ChildNodeIterator it = ogreScene->getChildIterator();
	
	Ogre::SceneNode *tempNode;

	while (it.hasMoreElements()) {
		tempNode = static_cast <Ogre::SceneNode*> (it.getNext());
		assert(tempNode);

		if ((tempNode->getName().find("Camera")) != std::string::npos) {
			ogreScene->removeChild(tempNode);

			if (tempNode->getName().find("Free") != std::string::npos)
				AddCamera((Ogre::Camera*)tempNode->getAttachedObject(0), Chaos::FREE_CAMERA);
			else if (tempNode->getName().find("Track") != std::string::npos)
				AddCamera((Ogre::Camera*)tempNode->getAttachedObject(0), Chaos::TRACK_CAMERA);
			else if (tempNode->getName().find("Chase") != std::string::npos) {
				AddCamera((Ogre::Camera*)tempNode->getAttachedObject(0), Chaos::CHASE_CAMERA);
				if (tempNode->getName().find("001") != std::string::npos) {
					mCurrentCamera = mCameraList.size() - 1;
					ApplyCamera();
				}
			}
			else if (tempNode->getName().find("Cockpit") != std::string::npos)
				AddCamera((Ogre::Camera*)tempNode->getAttachedObject(0), Chaos::COCKPIT_CAMERA);
		}
	}

	mCurrentCamera = 0;
	ApplyCamera();
}

void CsCameraManager::UnLoad() {
	DestroyCameras();
}


void CsCameraManager::SetupDefaultCamera() {
	//for proper orientation
	Ogre::Quaternion q(Ogre::Radian(Ogre::Degree(0.0)), Ogre::Vector3::UNIT_Z);
	
	// Free movable Camera
	mDefaultCamNode  =  mSceneMgr->createSceneNode("DefaultCamNode");

	mDefaultCam = mSceneMgr->createCamera("DefaultCam");

	mDefaultCam->setNearClipDistance(0.1);
	mDefaultCam->setFarClipDistance(5000);

	mDefaultCam->setAspectRatio(Ogre::Real(mViewport->getActualWidth()) / Ogre::Real(mViewport->getActualHeight()));
	mDefaultCam->setPosition(0,0,0);
	mDefaultCam->setFixedYawAxis(false);

	mDefaultCamNode->translate(0, 200, -600);
	mDefaultCamNode->setFixedYawAxis(false);
	//mDefaultCamNode->lookAt(Ogre::Vector3(0,0,0), Ogre::Node::TS_LOCAL);

	mDefaultCamNode->attachObject(mDefaultCam);
	
	AddCamera(mDefaultCam, Chaos::FREE_CAMERA);
	mCurrentCamera = mCameraList.size() - 1;
	
	ApplyCamera();
	mViewport->setCamera(mCameraList[mCurrentCamera].first);

	Ogre::SceneNode *orbitNode = mSceneMgr->createSceneNode("OrbitCamera");
	Ogre::Camera *orbitCam = mSceneMgr->createCamera("OrbitCamera");
	orbitCam->setAspectRatio(Ogre::Real(mViewport->getActualWidth()) / Ogre::Real(mViewport->getActualHeight()));
	orbitCam->setFixedYawAxis(true);
	orbitNode->attachObject(orbitCam);
	orbitNode->setInheritOrientation(false);

	AddCamera(orbitCam, Chaos::ORBIT_CAMERA);

}

void CsCameraManager::DestroyCameras()  {
	if (mCameraList.empty()) return;

	
	std::vector<std::pair<Ogre::Camera*, Ogre::SceneNode*>>::iterator it;
	for (it = mCameraList.begin(); it != mCameraList.end(); it++) {
		mSceneMgr->destroyCamera((*it).first);

		mSceneMgr->destroySceneNode((*it).second);
	}

	mCameraList.clear();
	mCameraType.clear();

	mViewport->setCamera(mSceneMgr->getCamera("DummyCamera"));
}

void CsCameraManager::SetCamera (const char* name) {
	int count = 0;

	std::vector<std::pair<Ogre::Camera*, Ogre::SceneNode*>>::iterator it;

	for (it = mCameraList.begin(); it != mCameraList.end(); it++) {
		if ((*it).first->getName().find(name) != Ogre::String::npos)  {
			// found
			mCurrentCamera = count;
			ApplyCamera();
			return;
		}
		count++;
	}
}

void CsCameraManager::SetTarget (CsPhysicsWorldObject* targetObject) {
	if (targetObject) SetTarget(targetObject->GetNode());
	mTargetObject = targetObject;
}

void CsCameraManager::SetTarget (Ogre::SceneNode* targetNode) {
	if (targetNode == NULL) return;
	if (mTargetNode != NULL && mCameraType[mCurrentCamera] == Chaos::CHASE_CAMERA) {
		mTargetNode->removeChild(mCameraList[mCurrentCamera].second);
	}
	mTargetNode = targetNode; 
	ApplyCamera(); 

	mTargetObject = NULL;
}

void CsCameraManager::ApplyCamera() {
	if (mCurrentCamera > mCameraList.size()) return;

	// make sure the camnode is detached first
	// mSceneMgr->getRootSceneNode()->removeChild(mCameraList[mCurrentCamera].second);

	switch(mCameraType[mCurrentCamera]) {
	case Chaos::FREE_CAMERA:
		break;

	case Chaos::CHASE_CAMERA:
		if (mTargetNode) {
			if (mCameraList[mCurrentCamera].second->getParent()) mCameraList[mCurrentCamera].second->getParent()->removeChild(mCameraList[mCurrentCamera].second);
			mTargetNode->addChild(mCameraList[mCurrentCamera].second);
		}
		break;

	case Chaos::ORBIT_CAMERA:
		if (mTargetNode) {
			if (mCameraList[mCurrentCamera].second->getParent()) mCameraList[mCurrentCamera].second->getParent()->removeChild(mCameraList[mCurrentCamera].second);
			mTargetNode->addChild(mCameraList[mCurrentCamera].second);

			mCameraList[mCurrentCamera].second->setInheritOrientation(true);
			mCameraList[mCurrentCamera].second->setFixedYawAxis(true, mTargetNode->_getDerivedOrientation().zAxis());
			mCameraList[mCurrentCamera].second->setAutoTracking(true, mTargetNode);

			mCameraList[mCurrentCamera].first->setFixedYawAxis(false);
			mCameraList[mCurrentCamera].first->setAutoTracking(false);
		}
		Orbit(0.0);
		break;

	case Chaos::COCKPIT_CAMERA:
		break;

	case Chaos::TRACK_CAMERA:
		break;

	default:
		break;

	}

	// instead of setting the camera, make the default camera try to keep up with the current 
	// using interpolation
	// mViewport->setCamera(mCameraList[mCurrentCamera].first);
}


void CsCameraManager::AddCamera(Ogre::Camera *cam, Chaos::CsCameraType type) {
	// fix clip dist
	cam->setNearClipDistance(NEAR_CLIP_DISTANCE);
	cam->setFarClipDistance(FAR_CLIP_DISTANCE);


	Ogre::SceneNode* camNode = cam->getParentSceneNode();
	std::pair<Ogre::Camera*, Ogre::SceneNode*> camPair(cam, camNode);
	mCameraList.push_back(camPair);
	mCameraType.push_back(type);

	if (type == Chaos::FREE_CAMERA || type == Chaos::COCKPIT_CAMERA || type == Chaos::TRACK_CAMERA)
	{
		//mSceneMgr->getRootSceneNode()->addChild(camNode);
		//mTargetNode->addChild(camNode);
	}
}


// NOTE: the first camera at index 0 is used for animation purposes and is 
// not available
void CsCameraManager::PrevCamera() {
	// if we have only the default camera
	if (mCameraList.size() < 2) return;

	mCurrentCamera--;
	if (mCurrentCamera  < 1) mCurrentCamera = mCameraList.size() - 1;
	ApplyCamera();
} 

void CsCameraManager::NextCamera() {
	// if we have only the default camera
	if (mCameraList.size() < 2) return;

	if (++mCurrentCamera  >= mCameraList.size()) mCurrentCamera = 1;
	ApplyCamera();
} 

void CsCameraManager::InjectInput(CsInputEvent evt) {
	switch (evt.type) {
	case Chaos::BUTTON_DOWN:
		mButtonState[evt.info.buttonInfo.buttonId] = true;
		if (evt.info.buttonInfo.buttonId == Chaos::DEBUG2) {
			PrevCamera();
			// CsGame::GetSingletonPtr()->GetTerrainManager()->SetCamera(GetCurrentCamera());
		}
		break;
	case Chaos::BUTTON_UP:
		mButtonState[evt.info.buttonInfo.buttonId] = false;
		break;
	case Chaos::AXIS_MOVED:
		if (evt.info.axisInfo.axisId == Chaos::AXIS_X) xrel = evt.info.axisInfo.axisValue;
		else if (evt.info.axisInfo.axisId == Chaos::AXIS_Y) yrel = evt.info.axisInfo.axisValue;
		else if (evt.info.axisInfo.axisId == Chaos::AXIS_Z) zrel = evt.info.axisInfo.axisValue;
		break;
	default:
		break;
	}
}

void CsCameraManager::Update()  {
	static clock_t prevTime = clock();

	static Ogre::Vector3 lerpPos;
	static Ogre::Quaternion slerpRot;
	static float linLerpStep;
	static float angLerpStep;

	clock_t curTime = clock();
	float timeStep = (curTime - prevTime) / (float) CLOCKS_PER_SEC;
	
	Ogre::SceneNode* camNode = mCameraList[mCurrentCamera].second;
	
	// the interpolation ----------------------------------------
	// the current camera is not actually used, the default camera is used to approximate 
	// the current camera

	posA = mDefaultCamNode->_getDerivedPosition();
	rotA = mDefaultCamNode->_getDerivedOrientation();

	posB = camNode->_getDerivedPosition();
	rotB = camNode->_getDerivedOrientation();

	linLerpStep = 0.5 * timeStep / 0.12;	// reaches half i 0.3 seconds
	angLerpStep = 0.5 * timeStep / 0.12;

	// for fast moving targets
	if (mTargetObject) {
		float linear = mTargetObject->GetLinearVelocity().length();
		float angular = mTargetObject->GetAngularVelocity().length();

		if (linear > 200.0f) {
			linLerpStep += (linear - 150) / 300.0f;
			angLerpStep += (linear - 150) / 300.0f;
		}
		if (angular > 3.0f) {
			linLerpStep += (angular - 2.0f) / 8.0f;
			angLerpStep += (angular - 2.0f) / 8.0f;
		}
	}

	if (linLerpStep > 1.0f) linLerpStep = 0.90f;
	if (angLerpStep > 1.0f) angLerpStep = 0.90f;
	if (linLerpStep < 0.0f) linLerpStep = 0.1f;
	if (angLerpStep < 0.0f) angLerpStep = 0.1f;

	lerpPos = (posB - posA) * linLerpStep + posA;
	slerpRot = Ogre::Quaternion::Slerp(angLerpStep, rotA, rotB, true);

	mDefaultCamNode->setPosition(lerpPos);
	mDefaultCamNode->setOrientation(slerpRot);
	// ----------------------------------------------------------
	if (mCameraType[mCurrentCamera] == Chaos::ORBIT_CAMERA) 
	{
		Orbit(timeStep);
	}
	else if (mCameraType[mCurrentCamera] == Chaos::FREE_CAMERA) 
	{
		if ( mButtonState[Chaos::ARROW_UP]) camNode->translate(0,0,-timeStep * MOVE_SPEED, Ogre::Node::TS_LOCAL);
		if ( mButtonState[Chaos::ARROW_DOWN]) camNode->translate(0,0,timeStep * MOVE_SPEED, Ogre::Node::TS_LOCAL);
		if ( mButtonState[Chaos::ARROW_LEFT]) camNode->translate(-timeStep * MOVE_SPEED,0,0, Ogre::Node::TS_LOCAL);
		if ( mButtonState[Chaos::ARROW_RIGHT]) camNode->translate(timeStep * MOVE_SPEED,0,0, Ogre::Node::TS_LOCAL);

		if (xrel) camNode->yaw(Ogre::Radian(Ogre::Degree(-LOOK_SPEED * xrel * timeStep)), Ogre::Node::TS_WORLD);
		if (yrel) camNode->pitch(Ogre::Radian(Ogre::Degree(-LOOK_SPEED * yrel * timeStep)), Ogre::Node::TS_LOCAL);
	}


	xrel = yrel = zrel = 0;
	prevTime = curTime;
}

void CsCameraManager::SetOrbitSpeed(float speed) {
	mOrbitSpeed = speed;
}

void CsCameraManager::Orbit(float timeStep) {
	static float theta = 0;

	Ogre::SceneNode* camNode = mCameraList[mCurrentCamera].second;

	float radius = 100;
	if (mTargetObject) {
		btVector3 min, max;
		mTargetObject->GetRigidBody()->getAabb(min, max);
		radius = (max - min).length();
	}

	camNode->setPosition(0,0,0);
	//camNode->translate(cosf(theta) * 100.0f, 90.0f, sinf(theta) * 100.0f, Ogre::Node::TS_PARENT);
	camNode->translate(cosf(theta) * radius * ORBIT_CAM_DISTANCE, sinf(theta) * radius * ORBIT_CAM_DISTANCE, ORBIT_CAM_HEIGHT * radius, Ogre::Node::TS_PARENT);

	theta += mOrbitSpeed * timeStep;
}

Ogre::Vector3 CsCameraManager::ScreenPosition (const Ogre::Vector3 &pos) {
	// get the matrix
	Ogre::Matrix4 pm = mDefaultCam->getProjectionMatrix();
	Ogre::Matrix4 vm = mDefaultCam->getViewMatrix();

	Ogre::Vector3 sPos = pm * vm * pos;
	sPos.x = (sPos.x + 1) * 0.5;
	sPos.y = (1 - sPos.y) * 0.5;
	
	return sPos;
}

void CsCameraManager::SetBackMusic(GameSounds bufferNo) {
	mBackMusic.LoadSource(bufferNo);
}

void CsCameraManager::PlayBackMusic() {
	mBackMusic.Loop(true);
	mBackMusic.UpdateSound();
	mBackMusic.Play();
}

void CsCameraManager::StopBackMusic() {
	mBackMusic.Stop();
}

void CsCameraManager::UpdateSound() {
	listener.Align(syncPos, syncRot);
	mBackMusic.SetPosition(syncPos);

	if( CsGame::GetSingletonPtr()) {
		if (CsGame::GetSingletonPtr()->GetPlayerShip()) {
			Ogre::Vector3 pos = CsGame::GetSingletonPtr()->GetPlayerShip()->GetNode()->_getDerivedPosition();
			Ogre::Quaternion rot = CsGame::GetSingletonPtr()->GetPlayerShip()->GetNode()->_getDerivedOrientation();
			listener.Align(pos , rot);
			mBackMusic.SetPosition(pos);
			listener.SetVelocity(CsGame::GetSingletonPtr()->GetPlayerShip()->GetRigidBody()->getLinearVelocity());
		} else {
			listener.SetVelocity(btVector3());
		}
	}

	mBackMusic.UpdateSound();
	listener.UpdateListener();
}
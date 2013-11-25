#pragma once

#include <vector>
#include <utility>

#include <OgrePrerequisites.h>

#include <CsInputManager.h>
#include <CsListener.h>
#include <CsAudioManager.h>
#include <CsSource.h>

namespace Chaos {
	enum CsCameraType {
		FREE_CAMERA, CHASE_CAMERA, TRACK_CAMERA, COCKPIT_CAMERA, ORBIT_CAMERA
	};
};

/* Camera conventions
	when exporting form max, name cameras as:
	<TYPE>CameraXXX
*/


// TODO: camera control (UPDATE function)
// TODO: camera interpolation during change

class CsCameraManager
{
public:
	CsCameraManager(Ogre::SceneManager* sceneMgr, Ogre::Viewport *viewport);
	virtual ~CsCameraManager(void) {}

	void SetupDefaultCamera();
	void Load (Ogre::SceneNode* ogreScene);
	void Load (const char* filename);
	void UnLoad ();

	void DestroyCameras();

	void AddCamera (Ogre::Camera *cam, Chaos::CsCameraType type);

	void NextCamera();
	void PrevCamera();
	void SetCamera(const char* name);

	void InjectInput(CsInputEvent evt);

	void SetTarget ( Ogre::SceneNode* targetNode);
	void SetTarget (CsPhysicsWorldObject* targetObject);

	void Update();
	void SetOrbitSpeed(float speed);
	void Orbit(float timeStep); // for orbit cameras

	inline Ogre::Camera* GetCurrentCamera () { return mCameraList[mCurrentCamera].first; }
	inline Ogre::SceneNode* GetCurrentCameraNode () { return mCameraList[mCurrentCamera].second; }

	// in the range like 0 to 1
	Ogre::Vector3 ScreenPosition (const Ogre::Vector3 &pos);
	
	// called from Render
	inline void SynchronizeSound() {
		syncPos = posA; syncRot = rotA;
	}
	
	void SetBackMusic(GameSounds bufferNo);
	void PlayBackMusic();
	void StopBackMusic();

	void UpdateSound();

protected:
	bool mButtonState[Chaos::NUM_BUTTONS];
	int xrel, yrel, zrel;

	std::vector<std::pair<Ogre::Camera*, Ogre::SceneNode*>> mCameraList;
	int mCurrentCamera;
	std::vector<int> mCameraType;

	// this one used for animations ---------------------
	Ogre::Camera* mDefaultCam;
	Ogre::SceneNode* mDefaultCamNode;

	// interpolation recipe
	Ogre::Vector3 posA, posB; // previous transform
	Ogre::Quaternion rotA, rotB; // target transform
	// --------------------------------------------------

	Ogre::SceneNode *mTargetNode;
	CsPhysicsWorldObject *mTargetObject;

	Ogre::SceneManager *mSceneMgr;
	Ogre::Viewport *mViewport;

	void ApplyCamera();

	// to load from file
	OgreMax::OgreMaxScene *mOgreScene;

	CsSource mBackMusic;

	CsListener listener;
	// called in Render
	Ogre::Vector3 syncPos;
	Ogre::Quaternion syncRot;

	float mOrbitSpeed;
};


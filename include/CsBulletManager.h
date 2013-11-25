#pragma once

#include <CsCommons.h>
#include <BulletCommons.h>
#include <OgrePrerequisites.h>

class CsBulletManager
{
public:
	CsBulletManager(void);
	virtual ~CsBulletManager(void);

	inline CsDiscreteDynamicsWorld* GetDynamicsWorld() {return mDynamicsWorld;}

	void Init();
	void Cleanup();

	void ProcessCollisions ();

	void DrawDebugLines(Ogre::ManualObject* debugLines);

	// important!!
	void TweakParameters();

	// debug data here fo ease of access
	Ogre::SceneNode* mDebugLinesNode;
	CsDebugDraw* mDebugDraw;
	std::vector<Ogre::Vector3> mDebugPoints;
	std::vector<Ogre::ColourValue> mDebugColors;

	bool mEnableDebugDraw;
protected:
	// bullet init data
    CsDiscreteDynamicsWorld* mDynamicsWorld;
    btBroadphaseInterface* mBroadphase;
    btDefaultCollisionConfiguration* mCollisionConfiguration;
    btCollisionDispatcher* mDispatcher;
    btSequentialImpulseConstraintSolver* mSolver;

	//--------------------------------------------------------------------------
	friend class CsGame;
};


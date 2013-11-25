#pragma once

#include <LinearMath\btVector3.h>

#include <OgrePrerequisites.h>
#include <CsCommons.h>
#include <OgreVector3.h>

#include <LinearMath\btAlignedObjectArray.h>

#include <CsDestructibleObject.h>

#include <BulletCollision\CollisionDispatch\btCollisionWorld.h>
#include <CsAutoTracker.h>

// Projectile is NOT an object, it is a ray scene query
struct CsProjectile : public CsAutoTracker {
	enum ProjectileType {
		POINT = 1, ORIENTED, MISSILE
	};

	CsProjectile() { 
		mActive = false; 
		mPointBB = mOrientedBB = NULL;
		mMissileNode = NULL;
		mNode = NULL;
		mSmokeFX = NULL;
		mPending = false;
		mOwner = false;
	}

	// initialize parameters
	void ShootFrom (CsWeapon* weapon, Ogre::SceneNode* node, CsDestructibleObject *owner, CsDestructibleObject *target = NULL);
	void ShootFrom (CsWeapon* weapon, CsDestructibleObject *owner, CsDestructibleObject *target = NULL);

	bool TargetInSight(CsProjectile *owner, float angleRad);
	// check hits
	void Update();

	void Synchronize();
	// update the central sync data

	// update render
	void UpdateBillboard();

	// send messages to Render
	void Destroy(bool explode = true);
	void DestroyRender();
	//--------------------------------------------- members
	// physics part
	bool mActive;
	bool mPending;

	ProjectileType mType;
	float mImpulse;	
	float mStride;

	float mSmartness; // for missiles, degree of being able to auto track

	CsDestructibleObject* mOwner;

	btVector3 mPrevPos, mCurPos;
	btVector3 mVelocity;

	float mDistance;

	// sync part
	Ogre::Vector3 mPos;
	Ogre::Vector3 mDirection;
	Ogre::Quaternion mOrientation;

	// render part
	Ogre::Billboard *mPointBB;
	Ogre::Billboard *mOrientedBB;

	Ogre::SceneNode *mMissileNode;
	Ogre::ParticleSystem *mSmokeFX;

	Ogre::SceneNode *mNode;
	// to put it all together
};

// ----------------------- Closest Ray Result Callback -----------------------
struct CsClosestRayResultCallback : public btCollisionWorld::ClosestRayResultCallback 
{
		CsClosestRayResultCallback(const btVector3&	rayFromWorld,const btVector3&	rayToWorld, btCollisionObject* me = NULL)
		:btCollisionWorld::ClosestRayResultCallback(rayFromWorld, rayToWorld)
		{
			mMe = me;
		}

	btCollisionObject *mMe;

	virtual bool  needsCollision (btBroadphaseProxy *proxy0) const  
	{
		bool collides = btCollisionWorld::ClosestRayResultCallback::needsCollision(proxy0);
		if (((btCollisionObject *) proxy0->m_clientObject) == mMe) collides = false;
		if (((btCollisionObject *)proxy0->m_clientObject)->getCollisionFlags() == btCollisionObject::CF_NO_CONTACT_RESPONSE) collides = false;
		return collides;
	}
};

//------------------------------------------------------------------------

class CsProjectileManager
{
public:
	CsProjectileManager(void);
	virtual ~CsProjectileManager(void);

	void CreatePool();
	void Unload();

	void Update();
	void Synchronize();
	void UpdateBillboards();

	// factory method
	CsProjectile* Create(CsProjectile::ProjectileType type);
	void CreateRender(CsProjectile* slug);

	inline Ogre::BillboardSet* GetPointBBSet() {return mPointBBSet;}
	inline Ogre::BillboardSet* GetOrientedBBSet() {return mOrientedBBSet;}
	// get missile set
protected:
	// debugging
	int count;

	Ogre::BillboardSet *mPointBBSet;
	Ogre::BillboardSet *mOrientedBBSet;
	// TODO: missile pool with create() and destroy() methods

	btAlignedObjectArray <CsProjectile> mPointPool;
	btAlignedObjectArray <CsProjectile> mOrientedPool;
	btAlignedObjectArray <CsProjectile> mMissilePool;
};


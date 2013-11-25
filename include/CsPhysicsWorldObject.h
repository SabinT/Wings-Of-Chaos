#ifndef CSPHYSICSWORLDOBJECT_H
#define CSPHYSICSWORLDOBJECT_H

#include <CsWorldObject.h>
#include <BulletCommons.h>

// TODO: add functionality for COLLISION OBJECTS(SHAPES) that dont need rigid body dynamics

namespace Chaos {
	enum CsPhysicsType {PHYSICS_STATIC, PHYSICS_DYNAMIC, PHYSICS_KINEMATIC};
};

struct CsMotionState;

const float DEFAULT_MASS = 10;

// notes: default values of mPosition and mRotation are GARBAGE, valid only after first call to motionstate, no
// need to set manually at start

class CsPhysicsWorldObject : public CsWorldObject //, public CsLockable
{
public:
    CsPhysicsWorldObject();
    virtual ~CsPhysicsWorldObject();

	// inline void SetPhysicsType (Chaos::CsPhysicsType physicsType) {mPhysicsType = physicsType;}
	inline Chaos::CsPhysicsType GetPhysicsType () { return mPhysicsType; }
    
	void SetRigidBody (btRigidBody* rigidBody);
    inline btRigidBody* GetRigidBody () {return mRigidBody;}

	btTransform GetConvertedTrans();

	void SetCollidesWith (int mask);
	inline int GetCollidesWith () { return mCollidesWith; }

	// needed for Factory created Objects, NEVER call for file-loaded objects
	void Unload();

protected:
	Chaos::CsPhysicsType mPhysicsType;
	int mCollidesWith;

    // production of rigid bodies handled by factory

	btRigidBody* mRigidBody;
	// CsMotionState* mMotionState;
	// btCollisionShape* mCollisionShape;
};

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------


#endif // CSPHYSICSWORLDOBJECT_H

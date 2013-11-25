#include <precompiled.h>
#include <CsPhysicsWorldObject.h>
#include <CsMotionState.h>
#include <CsGame.h>


CsPhysicsWorldObject::CsPhysicsWorldObject()
{
    mRigidBody = NULL;
    mNode = NULL;
	mType = Chaos::PASSIVE_OBJECT;
	mNeedsUpdate = false;
}

CsPhysicsWorldObject::~CsPhysicsWorldObject()
{
    //dtor
}

btTransform CsPhysicsWorldObject::GetConvertedTrans() {
	return ((CsMotionState*)(mRigidBody->getMotionState()))->getConvertedTrans(mRigidBody->getWorldTransform());
}


void CsPhysicsWorldObject::SetRigidBody (btRigidBody* rigidBody) {
	assert (rigidBody);

	mRigidBody = rigidBody;
	
	if (mRigidBody->isStaticObject()) mPhysicsType = Chaos::PHYSICS_STATIC;
	else if (mRigidBody->isKinematicObject()) mPhysicsType = Chaos::PHYSICS_KINEMATIC;
	else mPhysicsType = Chaos::PHYSICS_DYNAMIC;

	mRigidBody->setUserPointer((void*)this);

}

void CsPhysicsWorldObject::SetCollidesWith (int mask) {
	mCollidesWith = mask;
	if (mRigidBody->getBroadphaseProxy()) mRigidBody->getBroadphaseProxy()->m_collisionFilterMask = mask;
}


void CsPhysicsWorldObject::Unload() {
	if (mRigidBody->isInWorld()) CsGame::GetSingletonPtr()->GetDynamicsWorld()->removeRigidBody(mRigidBody);
	// DONOT DELETE REUSED COLLISION SHAPES!
	// delete mRigidBody->getCollisionShape();
	delete mRigidBody->getMotionState();
	delete mRigidBody;
}
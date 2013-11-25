#include <CsMotionState.h>
#include <CsPhysicsWorldObject.h>
#include <btVector3.h>
#include <btQuaternion.h>
#include <BulletDynamics\Dynamics\btRigidBody.h>
#include <OgreQuaternion.h>
#include <OgreVector3.h>

CsMotionState::CsMotionState(CsPhysicsWorldObject* obj, const btTransform& worldTrans, const btTransform& physicsTrans)
{	
	mWorldTrans = worldTrans,
	mPhysicsTrans = physicsTrans,
	mObj = obj;

	mOffset = mPhysicsTrans.inverse() * mWorldTrans;
}

void CsMotionState::SetObject(CsPhysicsWorldObject* obj) {mObj = obj;}

void CsMotionState::getWorldTransform(btTransform& centerOfMassWorldTrans ) const 
{
	centerOfMassWorldTrans = 	mPhysicsTrans;
}

btTransform CsMotionState::getConvertedTrans(const btTransform& centerOfMassWorldTrans) const
{
	return centerOfMassWorldTrans * mOffset;
}

void CsMotionState::setWorldTransform(const btTransform& centerOfMassWorldTrans) {
	if(NULL == mObj) return; // silently return before we set a node
	
	mWorldTrans = centerOfMassWorldTrans * mOffset;

    btQuaternion rot = mWorldTrans.getRotation();
    mObj->SetRotation(Ogre::Quaternion(rot.w(), rot.x(), rot.y(), rot.z()));

    btVector3 pos = mWorldTrans.getOrigin();
    mObj->SetPosition(Ogre::Vector3(pos.x(), pos.y(), pos.z()));
	mObj->SetLinearVelocity(mObj->GetRigidBody()->getLinearVelocity());
	mObj->SetAngularVelocity(mObj->GetRigidBody()->getAngularVelocity());

	mObj->SetNeedsUpdate(true);
}

/*
struct	btDefaultMotionState : public btMotionState
{
	btTransform m_graphicsWorldTrans;
	btTransform	m_centerOfMassOffset;
	btTransform m_startWorldTrans;
	void*		m_userPointer;

	btDefaultMotionState(const btTransform& startTrans = btTransform::getIdentity(),const btTransform& centerOfMassOffset = btTransform::getIdentity())
		: m_graphicsWorldTrans(startTrans),
		m_centerOfMassOffset(centerOfMassOffset),
		m_startWorldTrans(startTrans),
		m_userPointer(0)

	{
	}

	///synchronizes world transform from user to physics
	virtual void	getWorldTransform(btTransform& centerOfMassWorldTrans ) const 
	{
			centerOfMassWorldTrans = 	m_centerOfMassOffset.inverse() * m_graphicsWorldTrans ;
	}

	///synchronizes world transform from physics to user
	///Bullet only calls the update of worldtransform for active objects
	virtual void	setWorldTransform(const btTransform& centerOfMassWorldTrans)
	{
			m_graphicsWorldTrans = centerOfMassWorldTrans * m_centerOfMassOffset ;
	}

	

};*/
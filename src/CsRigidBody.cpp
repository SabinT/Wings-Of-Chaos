#include <CsRigidBody.h>
#include <CsMotionState.h>

CsRigidBody::CsRigidBody(btScalar mass, btMotionState *motionState, btCollisionShape *collisionShape, const btVector3 &localInertia)
	: btRigidBody(mass, motionState, collisionShape, localInertia)
{
}


CsRigidBody::~CsRigidBody(void)
{
	//dtor
}

CsRigidBody* CsRigidBody::Clone() {
	btScalar cloneMass = btScalar(1.0) / getInvMass();
	btVector3 cloneInertia(btScalar(1.0)/getInvInertiaDiagLocal().x(), btScalar(1.0)/getInvInertiaDiagLocal().y(), btScalar(1.0) / getInvInertiaDiagLocal().z());
	
	CsMotionState *motionState =  NULL;
	
	if (getMotionState()) {
		CsMotionState *oldState = (CsMotionState*) getMotionState();
		motionState = new CsMotionState(*oldState);
		// set this from outer scope
		motionState->mObj = NULL;
	}


	CsRigidBody *cloneBody = new CsRigidBody(cloneMass, motionState, getCollisionShape(), cloneInertia);
	return cloneBody;
	
	// see test CCDphysics project

	
}
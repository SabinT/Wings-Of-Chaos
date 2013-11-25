#pragma once
#include <BulletDynamics\Dynamics\btRigidBody.h>

class CsRigidBody;

#include <btVector3.h>

class CsRigidBody : public btRigidBody
{
public:
	CsRigidBody(btScalar mass, btMotionState *motionState, btCollisionShape *collisionShape, const btVector3 &localInertia=btVector3(0, 0, 0));
	virtual ~CsRigidBody(void);
	
	CsRigidBody* Clone();
};


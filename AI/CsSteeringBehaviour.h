#pragma once
#include <LinearMath\btVector3.h>
#include <CsShipObject.h>

const float TOPSPEED  = 50.0f;

class CsShipObject;

class CsSteeringBehaviour
{
public:
	CsSteeringBehaviour(void);
	CsSteeringBehaviour(CsShipObject* owner);
	virtual ~CsSteeringBehaviour(void);

	btVector3 Seek(void);     //returns the required seek force required for  the movement

	void SetTarget(CsShipObject* target) { mTarget = target; }
	void Zero() { mSteeringForce.setZero(); }

protected:
	CsShipObject* mShipOwner;   //the owner of the steering behaviour
	btVector3 mSteeringForce; //the force to be applied
	CsShipObject* mTarget; //the target ie the player ship
};


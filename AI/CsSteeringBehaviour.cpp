#include "precompiled.h"
//#include <CsSteeringBehaviour.h>
//#include <CsShipObject.h>
//
//
//CsSteeringBehaviour::CsSteeringBehaviour(void)
//{
//}
//
//CsSteeringBehaviour::CsSteeringBehaviour(CsShipObject* owner) {
//	mShipOwner = owner;
//	mTarget = NULL;
//	mSteeringForce.Zero();
//}
//
//
//CsSteeringBehaviour::~CsSteeringBehaviour(void)
//{
//}
//
//btVector3 CsSteeringBehaviour::Seek(void) {
//	if (mTarget) {
//		btVector3 tpos = btVector3(mTarget->GetPositon().x,mTarget->GetPositon().y, mTarget->GetPositon().z) ;
//		btVector3 cpos = btVector3(mShipOwner->GetPositon().x,mShipOwner->GetPositon().y, mShipOwner->GetPositon().z);
//		Zero();
//		btVector3 reqvel = (tpos - cpos).normalize() * MAX_LINEAR_VEL;
//		btVector3 curvel = mShipOwner->GetRigidBody()->getLinearVelocity();
//		return reqvel - curvel;
//
//
//	}
//	else //no target...shit!!!
//		return btVector3(0.,0.0,0.0);
//}

#include "precompiled.h"
#include <csenemyshipstates.h>
#include <ctime>
#include <csshipobject.h>
#include <csstatemachine.h>
#include <csgame.h>
#include <CsGameLevel.h>
#include <CsObjectiveList.h>
#include <cmath>
//
CsIdle*  CsIdle::Instance()  {
	static CsIdle instanse;
	return &instanse;
}

void CsIdle::Enter (CsShipObject* villian)  {

}

void CsIdle::Execute (CsShipObject* villian)  {
	villian->mMachineGun.coolDownTime = 0.2f;
	villian->mRocketLauncher.coolDownTime = 3.0f;
		
	villian->GetFSM()->ChangeState(CsDefendBase::Instance());
}

void CsIdle::Exit (CsShipObject* villian)  {
	//have absolutely no idea at the moment
}


CsDefendBase*  CsDefendBase::Instance()  {
	static CsDefendBase instanse;
	return &instanse;
}

void CsDefendBase::Enter (CsShipObject* villian)  {
	villian->mTargetSpeed = 200.0;
	villian->mTargetPos = CsGame::GetSingletonPtr()->GetGameLevel()->mObjectives->mEnemyBasePos;
}

const float nearDist2 = 300.0f * 300.0f;
void CsDefendBase::Execute (CsShipObject* villian)  {
	btVector3 villianPos = villian->GetRigidBody()->getWorldTransform().getOrigin();
	btVector3 basePos =  CsGame::GetSingletonPtr()->GetGameLevel()->mObjectives->mEnemyBasePos;
	btVector3 playerPos = CsGame::GetSingletonPtr()->GetPlayerShip()->GetRigidBody()->getWorldTransform().getOrigin();

	btVector3 dist = villian->mTargetPos - villianPos;

	if (dist.length2() < nearDist2) {
		float angle = (rand() % 360) * 3.14159 / 180.0;
		float radius = (rand() % 1000);
		float y = (rand() % 400) + 300;
		
		villian->mTargetPos.setX(basePos.x() + radius * cosf(angle));
		villian->mTargetPos.setZ(basePos.z() + radius * sinf(angle));

		villian->mTargetPos.setY(y);
	}

	villian->CalculateSteering(villian, villian->mTargetPos);

	// apply results
	villian->mRigidBody->clearForces();
	villian->mRigidBody->applyCentralForce(villian->mSteeringForce);
	villian->mRigidBody->setAngularVelocity(villian->mSteeringW);

	dist = playerPos - villianPos;
	if (dist.length2() < TARGET_RANGE_FAR * TARGET_RANGE_FAR) {
		villian->GetFSM()->ChangeState(CsChase::Instance());
	}

	dist = playerPos - basePos;
	if (dist.length2() < TARGET_RANGE_FAR * TARGET_RANGE_FAR) {
		villian->GetFSM()->ChangeState(CsChase::Instance());
	}

	if (CsGame::GetSingletonPtr()->GetPlayerShip()->mTarget == CsGame::GetSingletonPtr()->GetGameLevel()->mObjectives->mEnemyBase)
		villian->GetFSM()->ChangeState(CsChase::Instance());
}

void CsDefendBase::Exit (CsShipObject* villian)  {

}

CsChase*  CsChase::Instance()  {
	static CsChase instanse;
	return &instanse;
}

void CsChase::Enter (CsShipObject* villian)  {
	villian->mRigidBody->setDamping(0.1, 0.0);

	villian->SetTarget(CsGame::GetSingletonPtr()->GetPlayerShip());

	villian->mTargetSpeed = 200.0;
}

void CsChase::Execute (CsShipObject* villian)  {
	btVector3 villianPos = villian->GetRigidBody()->getWorldTransform().getOrigin();
	btVector3 basePos =  CsGame::GetSingletonPtr()->GetGameLevel()->mObjectives->mEnemyBasePos;
	btVector3 playerPos = CsGame::GetSingletonPtr()->GetPlayerShip()->GetRigidBody()->getWorldTransform().getOrigin();
	
	btVector3 dist = playerPos - basePos;
	if (dist.length2() > TARGET_RANGE_FAR * TARGET_RANGE_FAR) {
		dist = playerPos - villianPos;
		if (dist.length2() > TARGET_RANGE_NEAR * TARGET_RANGE_NEAR) {
			villian->GetFSM()->ChangeState(CsDefendBase::Instance());
		}
	}

	villian->CalculateSteering(villian);

	//// apply results
	villian->mRigidBody->clearForces();
	villian->mRigidBody->applyCentralForce(villian->mSteeringForce);
	villian->mRigidBody->setAngularVelocity(villian->mSteeringW);

	if (villian->TargetInRange(villian, TARGET_RANGE_FAR)) {
		if (villian->TargetInSight(villian, TARGET_ANGLE_WIDE))
			villian->FireSecondary();
	}

	if (villian->TargetInRange(villian, 800.0)) villian->GetFSM()->ChangeState(CsShoot::Instance());

	if (villian->mCurrentHealth == Chaos::DESTROYED) {
		villian->GetFSM()->ChangeState(CsRetreat::Instance());
	} else if (villian->mCurrentHealth == Chaos::DAMAGED) {
		if (CsGame::GetSingletonPtr()->GetPlayerShip()->mTarget == villian && CsGame::GetSingletonPtr()->GetPlayerShip()->mTargetLocked == true) 
			villian->GetFSM()->ChangeState(CsRetreat::Instance());
	}
}

void CsChase::Exit (CsShipObject* villian)  {
	//
}
//
CsShoot*  CsShoot::Instance()  {
	static CsShoot instanse;
	return &instanse;
}

void CsShoot::Enter (CsShipObject* villian)  {
	villian->FirePrimary();
	villian->mTargetSpeed = 100.0;
}

const float brake = 0.6;
void CsShoot::Execute (CsShipObject* villian)  {
	//villian->FirePrimary();
	villian->CalculateSteering(villian);

	//// apply results
	villian->mRigidBody->clearForces();
	villian->mRigidBody->applyCentralForce(villian->mSteeringForce);
	villian->mRigidBody->setAngularVelocity(villian->mSteeringW);

	if (villian->TargetInRange(villian, 300.0)) {
		villian->mRigidBody->setDamping(brake, 0);
		if (villian->TargetInRange(villian, 100.0)) villian->mRigidBody->setDamping(0.8, 0);
		villian->mTargetSpeed = -50.0;
	}
	else {
		villian->mRigidBody->setDamping(0.2, 0);
		if (!villian->TargetInRange(villian, 500)) villian->mTargetSpeed = 100.0;
	}

	if (villian->TargetInRange(villian, 100.0)) {
		 if (villian->TargetInSight(villian, TARGET_ANGLE_WIDE)) villian->FirePrimary();
	}
	else if (villian->TargetInSight(villian, TARGET_ANGLE_MEDIUM)) villian->FirePrimary();

	if (!villian->TargetInRange(villian, 100.0)) {
		if (villian->TargetInSight(villian, TARGET_ANGLE_WIDE))
			villian->FireSecondary();
	}

	if (!villian->TargetInRange(villian, 800.0)) villian->GetFSM()->ChangeState(CsChase::Instance());

	if (villian->mCurrentHealth == Chaos::DESTROYED) {
		villian->GetFSM()->ChangeState(CsRetreat::Instance());
	} else if (villian->mCurrentHealth == Chaos::DAMAGED) {
		if (CsGame::GetSingletonPtr()->GetPlayerShip()->mTarget == villian && CsGame::GetSingletonPtr()->GetPlayerShip()->mTargetLocked == true) 
			villian->GetFSM()->ChangeState(CsRetreat::Instance());
	}
}

void CsShoot::Exit (CsShipObject* villian)  {
	
}

CsRetreat*  CsRetreat::Instance()  {
	static CsRetreat instanse;
	return &instanse;
}

void CsRetreat::Enter (CsShipObject* villian)  {
	villian->mTargetSpeed = 200.0;
	villian->mTargetPos = CsGame::GetSingletonPtr()->GetGameLevel()->mObjectives->mEnemyBasePos;
}

void CsRetreat::Execute (CsShipObject* villian)  {
	btVector3 villianPos = villian->GetRigidBody()->getWorldTransform().getOrigin();
	btVector3 basePos =  CsGame::GetSingletonPtr()->GetGameLevel()->mObjectives->mEnemyBasePos;
	btVector3 playerPos = CsGame::GetSingletonPtr()->GetPlayerShip()->GetRigidBody()->getWorldTransform().getOrigin();

	btVector3 dist = villian->mTargetPos - villianPos;

	if (dist.length2() < nearDist2) {
		float angle = (rand() % 360) * 3.14159 / 180.0;
		float radius = (rand() % 1000);
		float y = (rand() % 400) + 400;
		
		villian->mTargetPos.setX(basePos.x() + radius * cosf(angle));
		villian->mTargetPos.setZ(basePos.z() + radius * sinf(angle));

		villian->mTargetPos.setY(y);
	}

	villian->CalculateSteering(villian, villian->mTargetPos);

	// apply results
	villian->mRigidBody->clearForces();
	villian->mRigidBody->applyCentralForce(villian->mSteeringForce);
	villian->mRigidBody->setAngularVelocity(villian->mSteeringW);

	dist = villianPos - basePos;
	if (dist.length2() < nearDist2) {
		villian->GetFSM()->ChangeState(CsDefendBase::Instance());
	}

	if (CsGame::GetSingletonPtr()->GetPlayerShip()->mTarget == CsGame::GetSingletonPtr()->GetGameLevel()->mObjectives->mEnemyBase)
		villian->GetFSM()->ChangeState(CsChase::Instance());
}

void CsRetreat::Exit (CsShipObject* villian)  {

}
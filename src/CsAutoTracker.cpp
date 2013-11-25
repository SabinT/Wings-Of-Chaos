#include "precompiled.h"
#include <CsAutoTracker.h>

#include <CsDestructibleObjectManager.h>
#include <CsDestructibleObject.h>
#include <CsGroundEnemyManager.h>
#include <CsGroundEnemy.h>

#include <CsShipObject.h>
#include <CsShipManager.h>

#include <OgreBulletConverter.h>
#include <CsGame.h>


const float MAX_DIST2  = 2000 * 2000.0F;
const float MIN_COSINE = cosf(35 * Ogre::Math::PI / 180.0);

CsAutoTracker::CsAutoTracker(void)
{
	mTrackerNode = NULL;
	mTargetNode = NULL;
	mTarget = NULL;

	mTargetInRange = false;
	mTargetLocked = false;

	mSteeringW = btVector3(0,0,0);
	mSteeringForce = btVector3(0,0,0);
	mTargetSpeed = 0;
}


CsAutoTracker::~CsAutoTracker(void)
{
}

void CsAutoTracker::SetTarget (Ogre::SceneNode *target) {
	if (!target) return;

	// no particular object
	mTarget = NULL;

	mTargetNode = target;
	mTrackerNode->setAutoTracking(true, target);
}

void CsAutoTracker::SetTarget (CsDestructibleObject *target) {
	if (!target) {
		ResetTarget();
		return;
	}

	mTarget = target;
	mTargetNode = target->GetNode();

	Ogre::Vector3 offset = BullettoOgre(target->GetRigidBody()->getWorldTransform().getOrigin()) - mTargetNode->getPosition();
	mTrackerNode->setAutoTracking(true, mTargetNode, Ogre::Vector3::NEGATIVE_UNIT_Z, offset);
}

void CsAutoTracker::AlignNode (Ogre::SceneNode *node, float accuracy) {
	// if target has already been destroyed
	//if (mTarget) {
	//	if(!mTarget->IsActive()) ResetTarget();
	//	return;
	//}

	if (!mTargetNode) return;
	if (mTargetNode) node->setOrientation(Ogre::Quaternion::Slerp(accuracy, node->getOrientation(), mTrackerNode->getOrientation(), true));
}
 
void CsAutoTracker::FindNewTarget(CsDestructibleObject *owner) {
	if (mTargetLocked) {
		if (!TargetInRange(owner, TARGET_RANGE_FAR)) 
			mTargetLocked = false;

		return;
	}

	CsDestructibleObject *target = NULL;
	float targetDist2 = MAX_DIST2, targetCosine = MIN_COSINE;

	CsDestructibleObjectManager *dMan = CsGame::GetSingletonPtr()->GetDestructibleObjectManager();
	CsDestructibleObject *dObj;

	for (int i = 0; i < dMan->size(); i++) {
		dObj = dMan->at(i);
		if (dObj == owner || !dObj->IsActive()) continue;

		btVector3 posA = owner->GetRigidBody()->getWorldTransform().getOrigin();
		btVector3 posB = dObj->GetRigidBody()->getWorldTransform().getOrigin();

		btVector3 dirn = posB - posA;
		float dist2 = dirn.length2();
		dirn.normalize();

		btVector3 nZ = - OgretoBullet(BullettoOgre(owner->GetRigidBody()->getWorldTransform().getRotation()).zAxis());
		float cosine = nZ.dot(dirn);
		
		if (cosine < targetCosine || cosine < 0.0f) continue;

		if (dist2 < targetDist2) {
			target = dObj;
			targetDist2 = dist2;
			targetCosine = cosine;
		}
	}

	CsGroundEnemyManager *gMan = CsGame::GetSingletonPtr()->GetGroundEnemyManager();
	CsGroundEnemy *gObj;

	for (int i = 0; i < gMan->size(); i++) {
		gObj = gMan->at(i);
		if (gObj == owner || !gObj->IsActive()) continue;

		btVector3 posA = owner->GetRigidBody()->getWorldTransform().getOrigin();
		btVector3 posB = gObj->GetRigidBody()->getWorldTransform().getOrigin();

		btVector3 dirn = posB - posA;
		float dist2 = dirn.length2();
		dirn.normalize();

		btVector3 nZ = - OgretoBullet(BullettoOgre(owner->GetRigidBody()->getWorldTransform().getRotation()).zAxis());
		float cosine = nZ.dot(dirn);
		
		if (cosine < targetCosine || cosine < 0.0f) continue;

		if (dist2 < targetDist2) {
			target = gObj;
			targetDist2 = dist2;
			targetCosine = cosine;
		}
	}

	CsShipManager *sMan = CsGame::GetSingletonPtr()->GetShipManager();
	CsShipObject *sObj;

	for (int i = 0; i < sMan->size(); i++) {
		sObj = sMan->at(i);

		if (sObj == owner || !sObj->IsActive()) continue;

		btVector3 posA = owner->GetRigidBody()->getWorldTransform().getOrigin();
		btVector3 posB = sObj->GetRigidBody()->getWorldTransform().getOrigin();

		btVector3 dirn = posB - posA;
		float dist2 = dirn.length2();
		dirn.normalize();

		btVector3 nZ = - OgretoBullet(BullettoOgre(owner->GetRigidBody()->getWorldTransform().getRotation()).zAxis());
		float cosine = nZ.dot(dirn);
		
		if (cosine < targetCosine || cosine < 0.0f) continue;

		if (dist2 < targetDist2) {
			target = sObj;
			targetDist2 = dist2;
			targetCosine = cosine;
		}
	}

	if (target) SetTarget(target);
	else {
		mTarget = NULL;
		mTargetNode = NULL;
	}
}

bool CsAutoTracker::TargetInRange (CsDestructibleObject *owner, float range) {
	if (!mTarget) return false;
	btVector3 &posA = owner->GetRigidBody()->getWorldTransform().getOrigin();
	btVector3 &posB = mTarget->GetRigidBody()->getWorldTransform().getOrigin();
	return ( (posA - posB).length2() < range * range);
}

bool CsAutoTracker::TargetInSight(CsDestructibleObject *owner, float angleRad) {
	if (!mTarget) return false;
	btVector3 &posA = owner->GetRigidBody()->getWorldTransform().getOrigin();
	btVector3 &posB = mTarget->GetRigidBody()->getWorldTransform().getOrigin();

	btVector3 dirn = posB - posA;
	dirn.normalize();

	btVector3 nZ = - OgretoBullet(BullettoOgre(owner->GetRigidBody()->getWorldTransform().getRotation()).zAxis());
	float cosine = nZ.dot(dirn);

	if (cosine < 0.0f) return false;
	if (cosine < cosf(angleRad)) return false;
	return true;
}


// Steering ***************************************************
const float PI = 3.14159F;
const float k1 = 1.0;
const float k0 = 1.5;

const float k2 = 0.5;

void CsAutoTracker::CalculateSteering(CsDestructibleObject* owner) {
	if (!mTarget) {
		mSteeringW = btVector3(0,0,0);
		mSteeringForce = btVector3(0,0,0);
		return;
	}
	
	CalculateSteering(owner, mTarget->GetRigidBody()->getWorldTransform().getOrigin());
}

void CsAutoTracker::CalculateSteering(CsDestructibleObject* owner, const btVector3 &posB) {
	btVector3 posA = owner->GetRigidBody()->getWorldTransform().getOrigin();
	btVector3 vel = owner->GetRigidBody()->getLinearVelocity();

	btVector3 AB = (posB - posA).normalize();
	btVector3 dirn = - OgretoBullet(BullettoOgre(owner->GetConvertedTrans().getRotation()).zAxis());

	float costheta = dirn.dot(AB);
	
	btVector3 targetw = dirn.cross(AB);
	btVector3 w = owner->GetRigidBody()->getAngularVelocity();
	float cosphi = w.dot(targetw);

	if (fabs(costheta) < cosf(1 * PI / 180.0))
		mSteeringW = targetw * (k0 + k1 *(0.5 - 0.5 * costheta));
	else
		mSteeringW = targetw * k1 * (0.5 - 0.5 * costheta);

	mSteeringForce = (AB * mTargetSpeed - vel) * k2 / owner->GetRigidBody()->getInvMass();
}
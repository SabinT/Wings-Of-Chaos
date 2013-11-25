#include "precompiled.h"

#include <CsGroundEnemy.h>
#include <CsShipObject.h>
#include <CsGame.h>

// long awaited converter functions
#include <OgreBulletConverter.h>


CsGroundEnemy::CsGroundEnemy(void)
{
	mTurret = CsGame::GetSingletonPtr()->GetSceneMgr()->createSceneNode();
	mTurret->attachObject(CsGame::GetSingletonPtr()->GetSceneMgr()->createEntity("Turret.mesh"));

	mMachineGun.ammo = CsWeapon::UNLIMITED_AMMO;
	mMachineGun.coolDownTime = 0.33f;
	mMachineGun.speed = 600.0f;

	mRocketLauncher.ammo = 10.0f;
	mRocketLauncher.coolDownTime = 10.0f;
	mRocketLauncher.smartness = 0.2f;

	mType = Chaos::ENEMY_OBJECT;
}

CsGroundEnemy::~CsGroundEnemy(void)
{
}

void CsGroundEnemy::Spawn() {
	//assert(mNode);
	//if (!mPerfectModel->getParent()) mNode->addChild(mPerfectModel);
	//if (!mNode->getParent()) CsGame::GetSingletonPtr()->GetSceneMgr()->getRootSceneNode()->addChild(mNode);
	CsDestructibleObject::Spawn();

	if (!mTurret->getParent()) mNode->addChild(mTurret);
	mTurret->setInheritOrientation(false);
	//mTurret->setFixedYawAxis(true, mNode->getOrientation().yAxis());
}

bool CsGroundEnemy::TargetInRange (CsDestructibleObject *owner, float range) {
	if (!mTarget) return false;

	btVector3 posA = OgretoBullet(mTurret->_getDerivedPosition());
	btVector3 &posB = mTarget->GetRigidBody()->getWorldTransform().getOrigin();
	return ( (posA - posB).length2() < range * range);
}

bool CsGroundEnemy::TargetInSight(CsDestructibleObject *owner, float angleRad) {
	if (!mTarget) return false;

	btVector3 posA = OgretoBullet(mTurret->_getDerivedPosition());
	btVector3 &posB = mTarget->GetRigidBody()->getWorldTransform().getOrigin();

	btVector3 dirn = posB - posA;
	dirn.normalize();

	btVector3 nZ = - OgretoBullet(mTurret->_getDerivedOrientation().zAxis());
	float cosine = nZ.dot(dirn);

	if (cosine < 0.0f) return false;
	if (cosine < cosf(angleRad)) return false;
	return true;
}

void CsGroundEnemy::Update() {
	if (!mTarget) return;

	if (TargetInRange(this, TARGET_RANGE_FAR)) {
		mTargetInRange = true;

		if (TargetInSight(this, TARGET_ANGLE_WIDE)) {
			// good enough for missiles
			mRocketLauncher.Fire(this);

			if (TargetInRange(this, TARGET_RANGE_NEAR)) {
				if (TargetInSight(this, TARGET_ANGLE_NARROW)) {
					// close enough for bullets
					mMachineGun.Fire(this);
				}
			}
		}
	}

}

void CsGroundEnemy::Synchronize() {
	//mTurretDirection = mSyncDirection;
}

void CsGroundEnemy::UpdateSceneNode() {
	if (mTargetInRange) {
		AlignNode(mTurret, 0.2);
	}
	//UpdateTargetRender();

	CsDestructibleObject::UpdateSceneNode();
}

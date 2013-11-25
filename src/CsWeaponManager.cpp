#include <precompiled.h>
#include <CsWeaponManager.h>
#include <CsGame.h>
#include <CsProjectileManager.h>
#include <cassert>

#include <CsShipObject.h>
#include <CsGroundEnemy.h>
#include <CsDestructibleobject.h>
#include <CsDestructibleObjectManager.h>

// The weapons ---------------------------------------------------------------------
inline bool CsWeapon::IsReady () { return (lastUsedTime + coolDownTime < (CsGame::GetSingletonPtr()->GetGameTime())); }

void MachineGun::Fire(CsDestructibleObject *owner) {
	if (!IsReady() || ammo == 0) return;

	if (owner->GetType() == Chaos::SHIP_OBJECT) 
	{
		CsShipObject *ship = (CsShipObject*) owner;
		// need to fire two slugs
		CsProjectile *left = CsGame::GetSingletonPtr()->GetProjectileManager()->Create(CsProjectile::POINT);
		CsProjectile *right = CsGame::GetSingletonPtr()->GetProjectileManager()->Create(CsProjectile::POINT);
		
		if (left == NULL|| right == NULL) return;
		mFireSound.SetPosition(owner->GetRigidBody()->getWorldTransform().getOrigin());
		mFireSound.UpdateSound();
		mFireSound.Play();

		left->ShootFrom(this, ship->GetLeftWing(), owner);
		right->ShootFrom(this, ship->GetRightWing(), owner);

		lastUsedTime = CsGame::GetSingletonPtr()->GetGameTime();
		if (ammo != CsWeapon::UNLIMITED_AMMO) ammo--;
	}
	else if (owner->GetType() == Chaos::ENEMY_OBJECT)
	{
		CsProjectile* slug = CsGame::GetSingletonPtr()->GetProjectileManager()->Create(CsProjectile::POINT);

		if (!slug) return;
		mFireSound.SetPosition(owner->GetRigidBody()->getWorldTransform().getOrigin());
		mFireSound.UpdateSound();
		mFireSound.Play();

		slug->ShootFrom(this, ((CsGroundEnemy *)owner)->GetTurret(), owner);

		lastUsedTime = CsGame::GetSingletonPtr()->GetGameTime();
		if (ammo != CsWeapon::UNLIMITED_AMMO) ammo--;
	}
	else
	{
		CsProjectile* slug = CsGame::GetSingletonPtr()->GetProjectileManager()->Create(CsProjectile::POINT);

		if (!slug) return;
		mFireSound.SetPosition(owner->GetRigidBody()->getWorldTransform().getOrigin());
		mFireSound.UpdateSound();
		mFireSound.Play();

		slug->ShootFrom(this, owner);

		lastUsedTime = CsGame::GetSingletonPtr()->GetGameTime();
		if (ammo != CsWeapon::UNLIMITED_AMMO) ammo--;
	}
}

void RocketLauncher::Fire (CsDestructibleObject *owner) {
	if (!IsReady() || ammo == 0) return;
	
	CsProjectile* slug = CsGame::GetSingletonPtr()->GetProjectileManager()->Create(CsProjectile::MISSILE);

	if (!slug) return;
	mFireSound.SetPosition(owner->GetRigidBody()->getWorldTransform().getOrigin());
	mFireSound.UpdateSound();
	mFireSound.Play();

	if (owner->GetTargetLocked()) slug->ShootFrom(this, owner, owner->mTarget);
	else slug->ShootFrom(this, owner, NULL);

	if (ammo != CsWeapon::UNLIMITED_AMMO) ammo--;
	lastUsedTime = CsGame::GetSingletonPtr()->GetGameTime();
}

void FlareGun::Fire (CsDestructibleObject *owner) {
	CsProjectile* slug = CsGame::GetSingletonPtr()->GetProjectileManager()->Create(CsProjectile::ORIENTED);

	if (!slug) return;
	mFireSound.SetPosition(owner->GetRigidBody()->getWorldTransform().getOrigin());
	mFireSound.UpdateSound();
	mFireSound.Play();

	slug->ShootFrom(this, owner);

	lastUsedTime = CsGame::GetSingletonPtr()->GetGameTime();
	if (ammo != CsWeapon::UNLIMITED_AMMO) ammo--;
}
// ---------------------------------------------------------------------------------


CsWeaponManager::CsWeaponManager(void)
{
	mPrimary = &mFlareGun;
	mSecondary = &mRocketLauncher;
}


CsWeaponManager::~CsWeaponManager(void)
{
}

void CsWeaponManager::FirePrimary(CsDestructibleObject* owner) {
	mPrimary->Fire(owner);
}

void CsWeaponManager::FireSecondary(CsDestructibleObject* owner) {
	mSecondary->Fire(owner);
}

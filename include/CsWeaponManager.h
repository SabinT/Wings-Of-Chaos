#pragma once
#include <CsCommons.h>
#include <CsSource.h>

struct CsWeapon {
	enum {UNLIMITED_AMMO = -1};

	int ammo;
	float coolDownTime;
	float lastUsedTime;
	float damage;
	// proportional to impulse delivered
	float speed;
	// speed of slug

	float smartness;
	// can be upgraded, say 1 = very very smart

	CsSource mFireSound;
	
	// functions
	virtual void Fire(CsDestructibleObject *owner) = 0;

	inline bool IsReady ();
};

struct MachineGun : public CsWeapon {
	MachineGun() {
		ammo = UNLIMITED_AMMO;
		coolDownTime = 0.1;
		lastUsedTime = -0.2;
		damage = 2;
		speed = 1000.0f;
		mFireSound.LoadSource(GameSounds::FIRE_MACHINEGUN);
		mFireSound.SetGain(1.5);
		smartness = 0;
	}
	void Fire(CsDestructibleObject *owner);
};

struct FlareGun : public CsWeapon {
	FlareGun() {
		ammo = CsWeapon::UNLIMITED_AMMO;
		coolDownTime = 2.0;
		lastUsedTime = -2.0;
		damage = 50;
		speed = 400.0f;
		mFireSound.LoadSource(GameSounds::FIRE_MACHINEGUN);
		mFireSound.SetGain(1.5);
		smartness = 0.1;
	}
	void Fire(CsDestructibleObject *owner);
};

struct RocketLauncher : public CsWeapon {
	RocketLauncher() {
		ammo = CsWeapon::UNLIMITED_AMMO;
		coolDownTime = 2.0;
		lastUsedTime = -2.0;
		damage = 50;
		speed = 400.0f;
		mFireSound.LoadSource(GameSounds::FIRE_MISSILE);
		smartness = 0.3;
	}
	void Fire(CsDestructibleObject *owner);
};

class CsWeaponManager
{
public:
	CsWeaponManager(void);
	virtual ~CsWeaponManager(void);

	void FirePrimary(CsDestructibleObject *owner);

	void FireSecondary(CsDestructibleObject *owner);

protected:
	RocketLauncher mRocketLauncher;
	MachineGun mMachineGun;
	FlareGun mFlareGun;

	CsWeapon *mPrimary;
	CsWeapon *mSecondary;
};


#pragma once
#include <CsDestructibleObject.h>
#include <CsWeaponManager.h>

class CsGroundEnemy : public CsDestructibleObject
{
public:
	CsGroundEnemy(void);
	virtual ~CsGroundEnemy(void);

	inline Ogre::SceneNode* GetTurret() { return mTurret; }

	void Spawn(); // to establish node consistency etc, always call from Render!!
	 
	// overloaded here!!
	bool TargetInRange(CsDestructibleObject *owner, float range);
	bool TargetInSight(CsDestructibleObject *owner, float angleRad);

	// do the enemy tracking thing etc etc
	void Update();
	void Synchronize();
	virtual void UpdateSceneNode();

	// oh all this holy thread safety!!
	Ogre::Vector3 mSyncDirection;
	Ogre::Vector3 mTurretDirection;

private:
	Ogre::SceneNode* mTurret;
	
	MachineGun mMachineGun;
	RocketLauncher mRocketLauncher;
};


#pragma once
#include <CsDestructibleObject.h>
#include <CsWeaponManager.h>
#include <string>
#include <CsStateMachine.h>
#include <CsEnemyShipStates.h>

// temporary, get parameters for each ship from ship scene file (tinyXML parser)


class CsShipObject : public CsDestructibleObject
{
public:
	enum ShipStatus {
		ACTIVE, INACTIVE, DYING
	};

	CsShipObject(void);
	virtual ~CsShipObject(void);

	void Reset();

	CsShipObject* Clone(Ogre::String name);

	void SetPlayerShip(bool b);

	// overloaded to care for damping
	void SetRigidBody (btRigidBody* rigidBody);

	inline int GetShipStatus () {return mShipStatus;}
	inline void SetShipStatus (int status) {mShipStatus = status;}

	inline std::string GetShipType () {return mShipType;}
	inline void SetShipType (std::string type) {mShipType = type;}

	inline Ogre::SceneNode* GetLeftWing() {return mLeftWing;}
	void SetLeftWing(Ogre::SceneNode* leftWing);

	inline Ogre::SceneNode* GetRightWing() {return mRightWing; }
	void SetRightWing(Ogre::SceneNode* rightWing);

	// TODO: these manipulate nodes so should be called from the Render part
	void Spawn();
	void Destroy();

	void UpdateSceneNode();
	void Update(); 
	void UpdateAI();
	// logic related update

	// movement related
	void SetThrottle();
	void ResetThrottle();

	void SetDamping();
	void ResetDamping();

	// pass +1, -1 for direction, 0 for reset
	void Roll(int direction);
	void Pitch(int direction);

	// firing
	void FirePrimary();
	void FireSecondary();

	float mThrottleForce;
	float mRollTorque;
	float mPitchTorque;
		
	// ship parameters like throttle force etc
	float mLinearDamping, mAngularDamping;

	inline CsStateMachine* GetFSM() {return mFSM;}

protected:
	static CsShipObject* sPlayerShip;
	// calculated from ship model node and wing nodes
	Ogre::SceneNode* mLeftWing;
	Ogre::SceneNode* mRightWing;

	// Ogre::OverlayContainer* mDot;

	CsWeaponManager mWeaponManager;

	MachineGun mMachineGun;
	RocketLauncher mRocketLauncher;

	Ogre::ParticleSystem *mJetEngineFX;
	CsSource mJetSound;

	// ship active/ deactivated/ destroying
	int mShipStatus;
	std::string mShipType;

	bool mAIShip;

	CsStateMachine *mFSM;

	friend class CsShipManager;
	
	friend class CsDefendBase;
	friend class CsChase;
	friend class CsIdle;
	friend class CsShoot;
	friend class CsRetreat;
};


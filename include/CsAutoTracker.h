#pragma once

#include <OgrePrerequisites.h>

#include <CsCommons.h>

#define TARGET_SCAN_DELAY 5
#define TARGET_RANGE_NEAR 800
#define TARGET_RANGE_FAR 1500

// 35 degrees
#define TARGET_ANGLE_WIDE 0.610865F
// 15 degrees
#define TARGET_ANGLE_MEDIUM 0.261799F
// 5 degrees
#define TARGET_ANGLE_NARROW 0.087266F

// interface for autotracking objects like ship radars, missiles, ground enemies
class CsAutoTracker
{
public:
	CsAutoTracker(void);
	virtual ~CsAutoTracker(void);

	// setter, getter
	void SetTarget (CsDestructibleObject *target);
	void SetTarget (Ogre::SceneNode *target);
	inline void ResetTarget () {
		mTarget = NULL;
		mTargetNode = NULL;
		mTargetLocked = false;
	}
	inline void LockTarget(bool b) {mTargetLocked = b;}
	inline void ToggleTargetLock() {mTargetLocked = !mTargetLocked;}
	inline bool GetTargetLocked() {return mTargetLocked;}

	void AlignNode (Ogre::SceneNode *node, float accuracy);

	void FindNewTarget(CsDestructibleObject *owner);
	// do a convex cast OR do distance checks etc and find the closest target within some angle;

	virtual bool TargetInRange(CsDestructibleObject *owner, float range);
	virtual bool TargetInSight(CsDestructibleObject *owner, float angleRad);

	// to be updated each time, manually!! for your own use!!
	bool mTargetInRange;
	bool mTargetLocked;

	void UpdateTargetRender();

	Ogre::SceneNode *mTrackerNode;
	// the perfect tracker, always keeps up, just set its fixed yaw axis appropriately
	CsDestructibleObject *mTarget;
	Ogre::SceneNode *mTargetNode;

	// no target object
	btVector3 mTargetPos;

	// steering
	btVector3 mSteeringW;
	btVector3 mSteeringForce;
	btScalar mTargetSpeed;
	void CalculateSteering(CsDestructibleObject* owner);
	void CalculateSteering(CsDestructibleObject* owner, const btVector3 &targetPos);

	friend class CsChase;
	friend class CsIdle;
	friend class CsShoot;
};


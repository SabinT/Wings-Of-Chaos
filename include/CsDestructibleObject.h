#pragma once
#include <CsPhysicsWorldObject.h>
#include <CsDestructible.h>
#include <CsAutoTracker.h>
#include <CsSource.h>

class CsDestructibleObject : public CsPhysicsWorldObject, public CsDestructible, public CsAutoTracker
{
public:
	CsDestructibleObject(void);
	virtual ~CsDestructibleObject(void);

	inline void SetHP (float HP) { mHP = HP; mNeedsUpdate = true; }

	// to keep the nodes consistent etc.
	virtual void Spawn();
	inline bool IsActive () { return mActive; }
	inline void SetActive (bool b) {mActive = b;}

	virtual void UpdateSceneNode();
	virtual void TakeHit (float damage);
	// Unload not implemented in this class

	// ok this too public!!
	void InitParticleFX();
	Ogre::ParticleSystem *mSmokeFX;

	CsSource mRicochet, mCollisionSound;
private:

};


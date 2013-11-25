#pragma once

#include <OgrePrerequisites.h>

namespace Chaos {
	enum CsHealth {
		PERFECT = 100,
		DAMAGED = 50,
		DESTROYED = 10
	};
};

class CsDestructible
{
public:
	CsDestructible(void);
	virtual ~CsDestructible(void);

	inline float GetHP () { return mHP; }
	inline void SetHP (float HP) { mHP = HP; }

	inline void SetArmor (float armor) {mArmor = armor; }
	inline float GetArmor () { return mArmor; }

	inline Ogre::SceneNode* GetDamagedModel () { return mDamagedModel; }
	inline Ogre::SceneNode* GetDestroyedModel () { return mDestroyedModel; }
	inline Ogre::SceneNode* GetPerfectModel () { return mPerfectModel; }

	inline void SetDamagedModel (Ogre::SceneNode* damagedModel) { mDamagedModel = damagedModel; }
	inline void SetDestroyedModel (Ogre::SceneNode* destroyedModel) { mDestroyedModel = destroyedModel; }
	inline void SetPerfectModel (Ogre::SceneNode* perfectModel) { mPerfectModel = perfectModel; }

	inline bool IsActive() {return mActive; }


protected:
	float mHP;
	float mArmor; // the smaller the better

	// to know if this has alread been destroyed
	bool mActive;

	Chaos::CsHealth mCurrentHealth;
	Chaos::CsHealth mPrevHealth;
	Ogre::SceneNode* mDamagedModel;
	Ogre::SceneNode* mDestroyedModel;
	Ogre::SceneNode* mPerfectModel;
};


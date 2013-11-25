#include <precompiled.h>

#include <CsDestructible.h>


CsDestructible::CsDestructible(void)
{
	mHP = Chaos::PERFECT;
	mCurrentHealth = Chaos::PERFECT;
	mPrevHealth = Chaos::PERFECT;

	mArmor = 1;

	mDamagedModel = NULL;
	mDestroyedModel = NULL;
	mPerfectModel = NULL;

	mActive = true;
}


CsDestructible::~CsDestructible(void)
{
}

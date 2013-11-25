#include <precompiled.h>
#include <CsDestructibleObject.h>
#include <CsBulletManager.h>
#include <CsGame.h>
#include <CsShipObject.h>

#include <CsExplosionManager.h>

const float MINIMUM_POSSIBLE_DAMAGE = 0.5F;

CsDestructibleObject::CsDestructibleObject(void)
{
	mType = Chaos::DESTRUCTIBLE_OBJECT;
	mSmokeFX = NULL;

	// the smaller the better
}

void CsDestructibleObject::TakeHit(float damage) {
	//if (mArmor * damage < MINIMUM_POSSIBLE_DAMAGE) return;
	if (mHP >= 0.0) {
		float dHP =  mArmor * damage;
		mHP -= dHP;

		int state;
		alGetSourcei(mRicochet.mSource, AL_SOURCE_STATE, &state);
		if (state != AL_PLAYING) {
			mRicochet.SetPosition(mRigidBody->getWorldTransform().getOrigin());
			mRicochet.UpdateSound();
			//mRicochet.Play();
		}

	}

}

CsDestructibleObject::~CsDestructibleObject(void)
{
}

void CsDestructibleObject::Spawn() {
	assert(mNode);
	if (!mPerfectModel->getParent()) mNode->addChild(mPerfectModel);
	if (!mNode->getParent()) CsGame::GetSingletonPtr()->GetSceneMgr()->getRootSceneNode()->addChild(mNode);

	if (!mTrackerNode) mTrackerNode = mNode->createChildSceneNode(mNode->getName() + "Tracker");
	mTrackerNode->setInheritOrientation(false);
	mTrackerNode->setFixedYawAxis(true);
	//SetTarget(CsGame::GetSingletonPtr()->GetPlayerShip()->GetNode());

	mRicochet.LoadSource(GameSounds::RICOCHET);
	mRicochet.SetGain(5);
	mCollisionSound.LoadSource(GameSounds::COLLISION);
	mCollisionSound.SetGain(5);

	mActive = true;
}

void CsDestructibleObject::InitParticleFX() {
	mSmokeFX = CsGame::GetSingletonPtr()->GetSceneMgr()->createParticleSystem(mNode->getName() + "Smoke", "HeavySmoke");
	mSmokeFX->setEmitting(false);
	mSmokeFX->setKeepParticlesInLocalSpace(true);

	Ogre::SceneNode *smokeNode = mNode->createChildSceneNode();
	smokeNode->setInheritOrientation(false);

	smokeNode->attachObject(mSmokeFX);
}

void CsDestructibleObject::UpdateSceneNode() {
	if (!mActive) return;
	if (mHP > Chaos::DAMAGED) mCurrentHealth = Chaos::PERFECT;
	else if (mHP > Chaos::DESTROYED) mCurrentHealth = Chaos::DAMAGED;
	else mCurrentHealth = Chaos::DESTROYED;

	assert(mSmokeFX->getNumEmitters());

	if (mPrevHealth != mCurrentHealth) {
		if (mCurrentHealth == Chaos::PERFECT) {
			// remove current model
			//if(mPrevHealth == Chaos::PERFECT && mPerfectModel->getParent()) mNode->removeChild(mPerfectModel);
			//else if(mPrevHealth == Chaos::DAMAGED && mDamagedModel->getParent()) mNode->removeChild(mDamagedModel);
			//else if(mPrevHealth == Chaos::DESTROYED && mDestroyedModel->getParent()) mNode->removeChild(mDestroyedModel);
			
			mSmokeFX->setEmitting(false);

			// attach new model
			//if (!mPerfectModel->getParent())
			//	mNode->addChild(mPerfectModel);
		}
		if (mCurrentHealth == Chaos::DAMAGED) {
			//if(mPrevHealth == Chaos::PERFECT && mPerfectModel->getParent()) mNode->removeChild(mPerfectModel);
			//else if(mPrevHealth == Chaos::DAMAGED && mDamagedModel->getParent()) mNode->removeChild(mDamagedModel);
			//else if(mPrevHealth == Chaos::DESTROYED && mDestroyedModel->getParent()) mNode->removeChild(mDestroyedModel);
			//btVector3 min, max;
			//mRigidBody->getAabb(min, max);
			//float scale = (max - min).length() * 1.0;

			//mSmokeFX->setEmitting(true);
			//mSmokeFX->getEmitter(0)->setEmissionRate(12);

			//mSmokeFX->getParentSceneNode()->setScale(scale, scale, scale);

			//if (!mDamagedModel->getParent())
			//mNode->addChild(mDamagedModel);
		}
		if (mCurrentHealth == Chaos::DESTROYED) {
			//if(mPrevHealth == Chaos::PERFECT && mPerfectModel->getParent()) mNode->removeChild(mPerfectModel);
			//else if(mPrevHealth == Chaos::DAMAGED && mDamagedModel->getParent()) mNode->removeChild(mDamagedModel);
			//else if(mPrevHealth == Chaos::DESTROYED && mDestroyedModel->getParent()) mNode->removeChild(mDestroyedModel);
			//btVector3 min, max;
			//mRigidBody->getAabb(min, max);
			//float scale = (max - min).length() * scale;

			//mSmokeFX->setEmitting(true);
			//mSmokeFX->getEmitter(0)->setEmissionRate(25);

			//mSmokeFX->getParentSceneNode()->setScale(scale, scale, scale);

			//if (!mDestroyedModel->getParent())
			//mNode->addChild(mDestroyedModel);
			// NB: destroyed equals almost destroyed
		}

		mPrevHealth = mCurrentHealth;
	}

	if (mHP < 0) {
		//float radius = mNode->_getWorldAABB().getHalfSize().length();
		btVector3 min, max;
		mRigidBody->getAabb(min, max);
		float radius = (max - min).length();

		if (mNode->getParent()) mNode->getParent()->removeChild(mNode);
		CsGame::GetSingletonPtr()->GetExplosionManager()->Create(mNode, radius * 0.7);

		mActive = false;

		CsLogicMessage msg;
		msg.type = CsLogicMessage::GAME_EVENT;
		msg.info.gameEvent.type = CsGameEvent::OBJECT_DESTROYED;
		msg.info.gameEvent.info.destroyedObject = this;
		CsGame::GetSingletonPtr()->GetLogicTempQueue()->push(msg);
	}

	if (mType != Chaos::ENEMY_OBJECT)
		mTrackerNode->setFixedYawAxis(true, mNode->getOrientation().yAxis());

	CsWorldObject::UpdateSceneNode();
}
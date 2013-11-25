#include <precompiled.h>

#include <CsProjectileManager.h>
#include <CsExplosionManager.h>

#include <CsDestructibleObject.h>
#include <CsWeaponManager.h>
#include <CsGame.h>
#include <CsCameraManager.h>

#include <CsLockableQueue.h>

#include <OgreBulletConverter.h>

const float IMPULSE_PER_HP = 2000.0F;

const float MAX_BULLET_DISTANCE = 2000.0f;
const float MAX_MISSILE_DISTANCE = 3000.0F;

const float IMPULSE_LOW = 5.0F;
const float IMPULSE_MED = 50.0F;
const float IMPULSE_HI = 100.0F;

const float VELOCITY_LOW = 50.0F;
const float VELOCITY_MED = 2000.0F;
const float VELOCITY_HI = 10000.0F;

const int MAX_BULLETS = 1000;
const int MAX_FLARES = 500;
const int MAX_MISSILES = 50;

// point shape for raycasting--------------------------------
btSphereShape* pointShape;

// local function to set BBSet bounds
void SetBBSetBounds (Ogre::BillboardSet* bbSet) {

}

// ------------------------------ Projectile -------------------------------------
void CsProjectile::ShootFrom (CsWeapon* weapon, Ogre::SceneNode *node, CsDestructibleObject *owner, CsDestructibleObject *target) {
		mOwner = owner;

		mStride = weapon->speed;
		mImpulse = weapon->damage;

		mSmartness = weapon->smartness;

		// testing only
		// node = CsGame::GetSingletonPtr()->GetCameraManager()->GetCurrentCameraNode();
		
		Ogre::Vector3 pos = node->_getDerivedPosition();
		Ogre::Quaternion rot = node->_getDerivedOrientation();

		Ogre::Vector3 zAxis = rot.zAxis();

		btVector3 btPos(pos.x, pos.y, pos.z);
		btVector3 btZAxis(zAxis.x, zAxis.y, zAxis.z);

		mCurPos = mPrevPos = btPos;
		mVelocity = - mStride * btZAxis;
		mDistance = 0;

		mDirection = Ogre::Vector3(mVelocity.x(), mVelocity.y(), mVelocity.z());
		mDirection.normalise();

		if (mType == MISSILE) {
			mNode->setFixedYawAxis(false);

			mNode->setOrientation(rot);
			mNode->setPosition(pos);

			mMissileNode->setPosition(0,0,0);
			mMissileNode->setVisible(true);

			mOrientation = rot;

			if (target) SetTarget(target);
			else ResetTarget();
		}

		Synchronize();
}

void CsProjectile::ShootFrom (CsWeapon* weapon, CsDestructibleObject *owner, CsDestructibleObject *target) {
		mOwner = owner;

		mStride = weapon->speed;
		mImpulse = weapon->damage;

		mSmartness = weapon->smartness;
		
		btTransform btTrans = owner->GetConvertedTrans();
		btVector3 btPos = btTrans.getOrigin();

		btQuaternion btRot = btTrans.getRotation();
		Ogre::Quaternion rot(btRot.w(), btRot.x(), btRot.y(), btRot.z());
		Ogre::Vector3 zAxis = rot.zAxis();
		btVector3 btZAxis(zAxis.x, zAxis.y, zAxis.z);

		mCurPos = mPrevPos = btPos;
		mVelocity = - mStride * btZAxis;
		mDistance = 0;

		mDirection = Ogre::Vector3(mVelocity.x(), mVelocity.y(), mVelocity.z());
		mDirection.normalise();

		if (mType == MISSILE) {
			mNode->setFixedYawAxis(false);

			mNode->setOrientation(rot);
			mNode->setPosition(BullettoOgre(btPos));

			mMissileNode->setPosition(0,0,0);
			mMissileNode->setVisible(true);

			mOrientation = rot;
			if (target) SetTarget(target);
			else ResetTarget();
		}

		Synchronize();
}

bool CsProjectile::TargetInSight(CsProjectile *owner, float angleRad) {
	if (!mTarget) return false;

	btVector3 posA = OgretoBullet(owner->mNode->getPosition());
	btVector3 &posB = mTarget->GetRigidBody()->getWorldTransform().getOrigin();

	btVector3 dirn = posB - posA;
	dirn.normalize();

	btVector3 nZ = - OgretoBullet(owner->mNode->getOrientation().zAxis());
	float cosine = nZ.dot(dirn);

	if (cosine < 0.0f) return false;
	if (cosine < cosf(angleRad)) return false;
	return true;
}

void CsProjectile::Update() {
	if (mActive == false  || mPending == true) return;

	float timeSinceLastFrame = CsGame::GetSingletonPtr()->GetDeltaTime();

	// fix direction for missiles
	if (mType == MISSILE) {
		mVelocity = OgretoBullet(-mOrientation.zAxis() * mStride);
		if (TargetInSight(this, TARGET_ANGLE_WIDE)) mTargetInRange = true;
		else mTargetInRange = false;
	}


	mCurPos += mVelocity * timeSinceLastFrame;
	mDistance += mStride * timeSinceLastFrame;

	if (mType == MISSILE) {
		if (mDistance > MAX_MISSILE_DISTANCE) Destroy(false);
	} else {
		if (mDistance > MAX_BULLET_DISTANCE) Destroy(false);
	}

	btVector3 from = mPrevPos;
	btVector3 to = mCurPos;

	btDiscreteDynamicsWorld *dynamicsWorld = CsGame::GetSingletonPtr()->GetDynamicsWorld();

	//apply an impulse if hits an object
	if (dynamicsWorld)
	{
		CsClosestRayResultCallback rayCallback(from, to, mOwner->GetRigidBody());
		// rayCallback.m_collisionFilterMask = 0;

		dynamicsWorld->rayTest(from, to, rayCallback);
		if (rayCallback.hasHit())
		{
			btRigidBody* body = btRigidBody::upcast(rayCallback.m_collisionObject);
			if (body)
			{
				// deactivate or destroy bullet
				Destroy();

				// TODO: COLLISION PROCESSING (HIT POINT UPDATE etc), maybe send to EngineMessageQueue
				// if this is one of the game objects
				float armor = 1.0f;
				if (body->getUserPointer()) {
					CsObject *obj = (CsObject *) body->getUserPointer();
					if (obj->GetType() == Chaos::SHIP_OBJECT || obj->GetType() == Chaos::DESTRUCTIBLE_OBJECT || obj->GetType() == Chaos::ENEMY_OBJECT) {
						((CsDestructibleObject *) obj)->TakeHit(mImpulse);
						armor = ((CsDestructibleObject *) obj)->GetArmor();
					}
				}

				if (body->getActivationState() != DISABLE_DEACTIVATION && body->getActivationState() != DISABLE_SIMULATION)
					body->setActivationState(ACTIVE_TAG);

				// armor gives a measure of resistance
				btVector3 impulse = mVelocity.normalize() * mImpulse * IMPULSE_PER_HP * armor;

				btVector3 relPos = rayCallback.m_hitPointWorld - body->getCenterOfMassPosition();
				body->applyImpulse(impulse,relPos);
			}
		}
	}

	mPrevPos = mCurPos;
}

void CsProjectile::Synchronize() {
	if (mActive == false || mPending == true) return;
	mPos = BullettoOgre(mCurPos);

	//mDirection =  Ogre::Vector3(mVelocity.x(), mVelocity.y(), mVelocity.z());
	//mDirection.normalise();
	
	if (mType == MISSILE) {
		mOrientation = mNode->getOrientation();
	}
}

void CsProjectile::UpdateBillboard() {
	if (mActive == false || mPending == true) return;

	static Ogre::Vector3 dirn;
	if (mPointBB) {
		mPointBB->setPosition(mPos);
	}
	if (mOrientedBB) {
		mOrientedBB->setPosition(mPos);
		mOrientedBB->mDirection = mDirection;
	}
	if (mMissileNode) {
		if (mTargetNode && mTargetInRange) AlignNode(mNode, mSmartness);
		mNode->setPosition(mPos);
	}
}

void CsProjectile::Destroy(bool explode) {
	if (mActive == false) return;

	CsRenderMessage msg;
	msg.type = CsRenderMessage::DESTROY_PROJECTILE;
	msg.info.projectile = this;
	CsGame::GetSingletonPtr()->GetRenderTempQueue()->push(msg);

	if (mType == MISSILE) 
		CsGame::GetSingletonPtr()->GetExplosionManager()->Create(mCurPos, 2.0f);

	mPending = true;
}

void CsProjectile::DestroyRender() {
	if (mActive == false) return;

	if (mType == CsProjectile::POINT || mType == CsProjectile::ORIENTED) {
		if (mPointBB->mParentSet)
			mPointBB->mParentSet->removeBillboard(mPointBB);	
	} 

	if (mType == CsProjectile::ORIENTED) {
		if (mOrientedBB->mParentSet)
			mOrientedBB->mParentSet->removeBillboard(mOrientedBB);	
	}

	if (mType == CsProjectile::MISSILE) {
		mMissileNode->setVisible(false);
		mSmokeFX->setEmitting(false);

		//CsGame::GetSingletonPtr()->GetExplosionManager()->CreateRender(mNode, 5.0f);
	}

	// now clean
	mActive = false;
	mPending= false;
}
// ------------------------------- manager ---------------------------------------
CsProjectileManager::CsProjectileManager(void)
{
	count  =  0;
}


CsProjectileManager::~CsProjectileManager(void)
{
}

void CsProjectileManager::CreatePool() {
	// 500 point, 500 oriented, 100 missiles

	// point bullets
	for (int i = 0; i < MAX_BULLETS; i++) {
		CsProjectile tmp;
		tmp.mImpulse = IMPULSE_LOW;
		tmp.mType = CsProjectile::POINT;
		tmp.mStride = VELOCITY_LOW;
		mPointPool.push_back(tmp);
	}

	// oriented bullets
	for (int i = 0; i < MAX_FLARES; i++) {
		CsProjectile tmp;
		tmp.mImpulse = IMPULSE_MED;
		tmp.mType = CsProjectile::ORIENTED;
		tmp.mStride = VELOCITY_MED;
		mOrientedPool.push_back(tmp);
	}

	// missile
	for (int i = 0; i < MAX_MISSILES; i++) {
		CsProjectile tmp;
		tmp.mImpulse = IMPULSE_HI;
		tmp.mType = CsProjectile::MISSILE;
		tmp.mStride = VELOCITY_LOW;
		tmp.mSmartness = 1.0;

		// create the actual entities now to prevent in game cloning
		tmp.mNode = CsGame::GetSingletonPtr()->GetSceneMgr()->getRootSceneNode()->createChildSceneNode();
		tmp.mNode->setFixedYawAxis(false);

		tmp.mMissileNode = tmp.mNode->createChildSceneNode();
		Ogre::Entity* missile = CsGame::GetSingletonPtr()->GetSceneMgr()->createEntity("missile.mesh"); //haha
		tmp.mMissileNode->attachObject(missile);
		tmp.mMissileNode->setVisible(false);

		Ogre::String name = Ogre::String("MissileSmoke") + Ogre::StringConverter::toString(i);
		tmp.mSmokeFX = CsGame::GetSingletonPtr()->GetSceneMgr()->createParticleSystem(name, "LightSmoke");
		tmp.mSmokeFX->setEmitting(false);

		tmp.mNode->attachObject(tmp.mSmokeFX);

		tmp.mTrackerNode = tmp.mNode->createChildSceneNode(tmp.mNode->getName() + "Tracker");
		tmp.mTrackerNode->setInheritOrientation(false);
		tmp.mTrackerNode->setFixedYawAxis(true);

		mMissilePool.push_back(tmp);
	}

	// raycast point shape
	pointShape = new btSphereShape(0.);

	// WARNING: TEST ONLY!!!
	// return;

	// -------------------- create billboard pools ------------------------
	// bounds
	Ogre::AxisAlignedBox aabb(Ogre::AxisAlignedBox::Extent::EXTENT_INFINITE);
	float radius = Ogre::Math::boundingRadiusFromAABB(aabb);

	// point BB pool
	mPointBBSet = CsGame::GetSingletonPtr()->GetSceneMgr()->createBillboardSet("PointBullets", 1000);
	mPointBBSet->setBounds(aabb, radius);
	// texture coordinate arrays if necessary, maybe animated bullets, at least rotation in texture

	mPointBBSet->setMaterialName("Examples/Flare");
	mPointBBSet->setBillboardType(Ogre::BillboardType::BBT_POINT);

	mPointBBSet->setDefaultDimensions(5.0, 5.0);
	mPointBBSet->setCullIndividually(true);

	CsGame::GetSingletonPtr()->GetSceneMgr()->getRootSceneNode()->attachObject(mPointBBSet);

	// oriendetd BB pool
	mOrientedBBSet = CsGame::GetSingletonPtr()->GetSceneMgr()->createBillboardSet("OrientedBullets", 500);
	mOrientedBBSet->setBounds(aabb, radius);
	mOrientedBBSet->setBillboardOrigin(Ogre::BillboardOrigin::BBO_TOP_CENTER);

	mOrientedBBSet->setMaterialName("Examples/Flare2");
	mOrientedBBSet->setBillboardType(Ogre::BillboardType::BBT_ORIENTED_SELF);
	mOrientedBBSet->setDefaultDimensions(5.0, 5.0);
	mOrientedBBSet->setCullIndividually(true);

	CsGame::GetSingletonPtr()->GetSceneMgr()->getRootSceneNode()->attachObject(mOrientedBBSet);

	// missile pool not necessary, already in projectile
}

void CsProjectileManager::Unload() {
	// delete raycast point shape
	delete pointShape;

	// missiles cleaned by scenemanager
}

CsProjectile* CsProjectileManager::Create(CsProjectile::ProjectileType type) {
	// request render part to do things
	CsRenderMessage msg;

	if (type == CsProjectile::POINT) {
		for (int i = 0; i < mPointPool.size(); i++) {
			CsProjectile *tmp = &mPointPool[i];
			if (tmp->mActive == false && tmp->mPending == false) {
				tmp->mPending = true;

				msg.type = CsRenderMessage::CREATE_PROJECTILE;
				msg.info.projectile = tmp;
				CsGame::GetSingletonPtr()->GetRenderTempQueue()->push(msg);
				return tmp;
			}
		}

	}

	if (type == CsProjectile::ORIENTED) {
		for (int i = 0; i < mOrientedPool.size(); i++) {
			CsProjectile *tmp = &mOrientedPool[i];
			if (tmp->mActive == false && tmp->mPending == false) {
				tmp->mPending = true;

				msg.type = CsRenderMessage::CREATE_PROJECTILE;
				msg.info.projectile = tmp;
				CsGame::GetSingletonPtr()->GetRenderTempQueue()->push(msg);
				return tmp;
			}
		}

	}

	if (type == CsProjectile::MISSILE) {
		for (int i = 0; i < mMissilePool.size(); i++) {
			CsProjectile *tmp = &mMissilePool[i];
			if (tmp->mActive == false && tmp->mPending == false) {
				tmp->mPending = true;

				msg.type = CsRenderMessage::CREATE_PROJECTILE;
				msg.info.projectile = tmp;
				CsGame::GetSingletonPtr()->GetRenderTempQueue()->push(msg);
				return tmp;
			}
		}

	}

	return NULL;
}

// do the render creation part then activate
void CsProjectileManager::CreateRender(CsProjectile* slug) {
	// request render part to do things
	if(!slug) return;

	if (slug->mType == CsProjectile::POINT) {
		slug->mPointBB = mPointBBSet->createBillboard(0,0,0);
		slug->mPending = false;
		slug->mActive = true;
	}

	if (slug->mType == CsProjectile::ORIENTED) {
		//slug->mPointBB = mPointBBSet->createBillboard(0,0,0);
		slug->mOrientedBB = mOrientedBBSet->createBillboard(0,0,0);
		slug->mPending = false;
		slug->mActive = true;
	}

	if (slug->mType == CsProjectile::MISSILE) {
		slug->mMissileNode->setVisible(true);
		slug->mMissileNode->setPosition(0,0,0);
		slug->mSmokeFX->setEmitting(true);
		slug->mPending = false;
		slug->mActive = true;
	}

}


void CsProjectileManager::Update() {
	count = 0;

	for (int i = 0; i < mPointPool.size(); i++) {
		if (mPointPool[i].mActive) count++;
		mPointPool[i].Update();
	}

	for (int i = 0; i < mOrientedPool.size(); i++) {
		if (mOrientedPool[i].mActive) count++;
		mOrientedPool[i].Update();
	}

	for (int i = 0; i < mMissilePool.size(); i++) {
		if (mMissilePool[i].mActive) count++;
		mMissilePool[i].Update();
	}
}

void CsProjectileManager::Synchronize() {
	for (int i = 0; i < mPointPool.size(); i++)
		mPointPool[i].Synchronize();

	for (int i = 0; i < mOrientedPool.size(); i++)
		mOrientedPool[i].Synchronize();

	for (int i = 0; i < mMissilePool.size(); i++)
		mMissilePool[i].Synchronize();
}

void CsProjectileManager::UpdateBillboards() {
	for (int i = 0; i < mPointPool.size(); i++)
		mPointPool[i].UpdateBillboard();

	for (int i = 0; i < mOrientedPool.size(); i++)
		mOrientedPool[i].UpdateBillboard();

	for (int i = 0; i < mMissilePool.size(); i++)
		mMissilePool[i].UpdateBillboard();
}

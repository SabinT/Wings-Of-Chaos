#include <precompiled.h>

#include <CsBulletManager.h>
#include <CsGame.h>
#include <CsDebugDraw.h>
#include <CsDestructibleObject.h>

#define DAMAGE_PER_VELOCITY 1.0F / 300.0F
#define MIN_DAMAGE_VELOCITY 30.0F

const float	MAX_BODY_VELOCITY = 100.0f;
const float MAX_SHIP_VELOCITY = 300.0F;
const float NORMAL_RADIUS = 3.0F;
const float MINIMUM_ARMOR = 0.2F;

// to be applied incrementally and continuously
const float SPLASH_DAMAGE = 0.05f;

// motion clamping of bodies
void myTickCallback(btDynamicsWorld *world, btScalar timeStep) {
    btCollisionObjectArray objects = world->getCollisionObjectArray();

    for (int i = 0; i < objects.size(); i++) {
        btRigidBody *rigidBody = btRigidBody::upcast(objects[i]);
        if (!rigidBody) {
            continue;
        }

		if (rigidBody->getUserPointer()) {
			CsObject* obj = (CsObject*)rigidBody->getUserPointer();
			// let the ships sail!!!
			if (obj->GetType() == Chaos::SHIP_OBJECT) {
				float vel = rigidBody->getLinearVelocity().length();
				if (vel > MAX_SHIP_VELOCITY) {
					rigidBody->setLinearVelocity(rigidBody->getLinearVelocity() * MAX_SHIP_VELOCITY / vel);
				}
				continue;
			}
		}

		float vel = rigidBody->getLinearVelocity().length();
		if (vel > MAX_BODY_VELOCITY) {
			rigidBody->setLinearVelocity(rigidBody->getLinearVelocity() * MAX_BODY_VELOCITY / vel);
		}
    }

    return;
}



CsBulletManager::CsBulletManager(void)
{
	mEnableDebugDraw = false;
}


CsBulletManager::~CsBulletManager(void)
{
}

void CsBulletManager::Init() {
	mBroadphase = new btDbvtBroadphase();
    mCollisionConfiguration = new btDefaultCollisionConfiguration();
    mDispatcher = new btCollisionDispatcher(mCollisionConfiguration);
    mSolver = new btSequentialImpulseConstraintSolver;

    mDynamicsWorld = new CsDiscreteDynamicsWorld(mDispatcher,mBroadphase,mSolver,mCollisionConfiguration);
	mDynamicsWorld->SetSyncMutex(&CsGame::GetSingletonPtr()->mSyncMutex);
	mDynamicsWorld->setInternalTickCallback(myTickCallback);

	//btGImpactCollisionAlgorithm::registerAlgorithm((btCollisionDispatcher*)mDynamicsWorld->getDispatcher());

    mDynamicsWorld->setGravity(btVector3(0,-20.0,0));

	mDebugDraw = new CsDebugDraw;
	mDebugDraw->SetDebugLines(&mDebugPoints, &mDebugColors);
	mDynamicsWorld->setDebugDrawer(mDebugDraw);
	mDebugLinesNode = CsGame::GetSingletonPtr()->GetSceneMgr()->getRootSceneNode()->createChildSceneNode("BulletLinesNode");

	//mDebugDraw->setDebugMode(btIDebugDraw::DBG_DrawWireframe | btIDebugDraw::DBG_DrawAabb);
}

void CsBulletManager::Cleanup() {
	delete mDebugDraw;
    delete mDynamicsWorld;
    delete mSolver;
    delete mDispatcher;
    delete mCollisionConfiguration;
    delete mBroadphase;
}

void CsBulletManager::DrawDebugLines(Ogre::ManualObject* debugLines) {
	// lock debug points	
	boost::mutex::scoped_lock lock(mDebugDraw->mMutex);

	debugLines->begin("",Ogre::RenderOperation::OT_LINE_LIST);

	std::vector<Ogre::Vector3>::iterator it = mDebugPoints.begin();

	for (unsigned int i = 0; i < mDebugPoints.size();) {
		debugLines->position(mDebugPoints[i]);
		i++;

		if (i >= mDebugPoints.size()) break;

		debugLines->position(mDebugPoints[i]);
		i++;
	}
	debugLines->end();

}

void CsBulletManager::TweakParameters() {
    btCollisionObjectArray objects = mDynamicsWorld->getCollisionObjectArray();

    for (int i = 0; i < objects.size(); i++) {
        btRigidBody *rigidBody = btRigidBody::upcast(objects[i]);
        if (!rigidBody) {
            continue;
        }

		rigidBody->setDamping(0.3,0.3);

		// tweak the armor
		if (rigidBody->getUserPointer()) {
			CsObject* obj = (CsObject*)rigidBody->getUserPointer();
			if (obj->GetType() == Chaos::DESTRUCTIBLE_OBJECT || obj->GetType() == Chaos::ENEMY_OBJECT || obj->GetType() == Chaos::SHIP_OBJECT) {
				// get bounding radius
				btVector3 center;
				btScalar radius;
				rigidBody->getCollisionShape()->getBoundingSphere(center, radius);

				//float armor = NORMAL_RADIUS / radius + MINIMUM_ARMOR;
				float armor = 2.0 - radius / 500.0f * 1.5;
				// more than double damagenot allowed
				if (armor < 0.25) armor = 0.25f;
				((CsDestructibleObject*)obj)->SetArmor(armor);
			}
		}
    }

    return;
}

void CsBulletManager::ProcessCollisions () {
	//Assume world->stepSimulation or world->performDiscreteCollisionDetection has been called

	int numManifolds = mDynamicsWorld->getDispatcher()->getNumManifolds();
	for (int i=0;i<numManifolds;i++)
	{
		btPersistentManifold* contactManifold =  mDynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
		btRigidBody* obA = static_cast<btRigidBody*>(contactManifold->getBody0());
		btRigidBody* obB = static_cast<btRigidBody*>(contactManifold->getBody1());
	
		int numContacts = contactManifold->getNumContacts();
		if (numContacts == 0) continue;

		// calculate relative velocity
		float rVel = (obA->getLinearVelocity() - obB->getLinearVelocity()).length();
		if (rVel < MIN_DAMAGE_VELOCITY) continue;

		if (obA->getUserPointer()) {
			CsObject *obj = (CsObject *) obA->getUserPointer();
			if (obj->GetType() == Chaos::SHIP_OBJECT || obj->GetType() == Chaos::DESTRUCTIBLE_OBJECT || obj->GetType() == Chaos::ENEMY_OBJECT) {
				CsDestructibleObject *des =  (CsDestructibleObject*)obj;
				
				//if (obB->getUserPointer()) {
				//	if (((CsObject*)obB->getUserPointer())->GetType() == Chaos::EXPLOSION_OBJECT) des->TakeHit(SPLASH_DAMAGE);
				//	else des->TakeHit(rVel * DAMAGE_PER_VELOCITY);
				//}
				//else des->TakeHit(rVel * DAMAGE_PER_VELOCITY);
				

				int state;
				alGetSourcei(des->mCollisionSound.mSource, AL_SOURCE_STATE, &state);
				if (state != AL_PLAYING) {
					des->mCollisionSound.SetPosition(obB->getWorldTransform().getOrigin());
					des->mCollisionSound.UpdateSound();
					des->mCollisionSound.Play();
				}
			}
		}

		if (obB->getUserPointer()) {
			CsObject *obj = (CsObject *) obB->getUserPointer();
			if (obj->GetType() == Chaos::SHIP_OBJECT || obj->GetType() == Chaos::DESTRUCTIBLE_OBJECT || obj->GetType() == Chaos::ENEMY_OBJECT) {
				CsDestructibleObject *des =  (CsDestructibleObject*)obj;
				
				//if (obA->getUserPointer()) {
				//	if (((CsObject*)obA->getUserPointer())->GetType() == Chaos::EXPLOSION_OBJECT) des->TakeHit(SPLASH_DAMAGE);
				//	else des->TakeHit(rVel * DAMAGE_PER_VELOCITY);
				//}
				//else des->TakeHit(rVel * DAMAGE_PER_VELOCITY);

				int state;
				alGetSourcei(des->mCollisionSound.mSource, AL_SOURCE_STATE, &state);
				if (state != AL_PLAYING) {
					des->mCollisionSound.SetPosition(obB->getWorldTransform().getOrigin());
					des->mCollisionSound.UpdateSound();
					des->mCollisionSound.Play();
				}
			}
		}
	}

}
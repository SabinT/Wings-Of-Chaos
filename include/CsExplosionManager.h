#pragma once

#include <CsCommons.h>

#include <CsSource.h>

#include <BulletCollision\CollisionDispatch\btCollisionWorld.h>

// NB: there's nothing LOGIC about explosions, so do all the creating destroying updating in render
const int POOL_SIZE = 50;

const float EXPLOSION_TIME = 0.1F;
const float SMOKE_TIME = 0.5F;

struct CsExplosion {
	Ogre::SceneNode *node;
	Ogre::ParticleSystem *explosion;
	Ogre::ParticleSystem *smoke;
	Ogre::ParticleSystem *sparks;

	btRigidBody* splashBody;
	CsObject* explosionObject;

	Ogre::Light* explosionLight;

	CsSource mExplosionSound;
	bool active;
	bool exploding;
	bool smoking;
	bool pending;

	float startTime;

	// TODO: for pushing effect, splash damage etc
	float radius;

	CsExplosion () : node(NULL), explosion(NULL), smoke(NULL), sparks(NULL), active(false), startTime(0.0), radius(0.0),
					 exploding(false), smoking(false), pending(false), explosionObject(NULL)
	{ 
		//
	}

	// stop the fire/smoke/sparks and disable itself at the appropriate moment
	void UpdateRender ();
};

class CsExplosionManager
{
public:
	CsExplosionManager(void);
	~CsExplosionManager(void);

	void CreatePool();
	void Unload();

	// to be called by Logic Part
	void Create(Ogre::SceneNode* node, float radius = 5.0);
	void Create(btVector3 &pos, float radius = 5.0);

	void SplashEffect(btVector3 &pos, float radius = 100.0);

	void CreateRender(CsExplosion* e, Ogre::SceneNode* node, float radius = 5.0);
	void CreateRender(CsExplosion* e, float x, float y, float z, float radius = 5.0);
	
	// called from LOGIC, update all particles
	void UpdateRender();
private:
	CsExplosion explosions[POOL_SIZE];
};

struct CsConvexResultCallback : public btCollisionWorld::ConvexResultCallback {
	std::vector<btRigidBody*> hitList;
	btVector3 pos;

	CsConvexResultCallback(short group, short mask, btVector3 &_pos) :  btCollisionWorld::ConvexResultCallback() {
		m_collisionFilterMask = mask;
		m_collisionFilterGroup = group;
		pos = _pos;
	}

	virtual btScalar  addSingleResult(btCollisionWorld::LocalConvexResult& convexResult,bool normalInWorldSpace) {
		btRigidBody* hit = btRigidBody::upcast(convexResult.m_hitCollisionObject);
				//hitList.push_back(hit);
		if(hit) {
			//bool added = false;
			//for (int i = 0; i < hitList.size() && !added; i++) {
			//	if (hit == hitList.at(i)) added = true;
			//}
			//if (!added) {
				btVector3 relPos = hit->getCenterOfMassPosition() - pos;

				if (hit->getActivationState() != DISABLE_DEACTIVATION && hit->getActivationState() != DISABLE_SIMULATION)
				hit->setActivationState(ACTIVE_TAG);

				btVector3 impulse = relPos.normalize() * 50000 / (1 + relPos.length2());
				hit->applyCentralImpulse(btVector3(0,1000,0));
				//hitList.push_back(hit);
			//}
		}

		return convexResult.m_hitFraction;
	}

	virtual bool  needsCollision (btBroadphaseProxy *proxy0) const  {
		return true;
	}

	std::vector<btRigidBody*>& getHitList() {
		return hitList;
	}
};


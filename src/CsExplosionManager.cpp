#include <precompiled.h>
#include <CsExplosionManager.h>
#include <CsGame.h>
#include <CsSource.h>

#include <CsObject.h>
#include <CsDestructibleObject.h>
#include <OgreBulletConverter.h>

const int MAX_EXPLOSIONS = 15;

const float EXPLOSION_IMPULSE = 500;
const float EXPLOSION_IMPULSE_FACTOR = 500;

const float SPLASH_FACTOR = 100.0;

void CsExplosion::UpdateRender() {
	if (exploding) {
		if (CsGame::GetSingletonPtr()->GetGameTime() - startTime >  EXPLOSION_TIME) {
			explosion->setEmitting(false);
			sparks->setEmitting(false);
			exploding = false;

			smoke->setEmitting(true);
			smoking = true;

			splashBody->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
		}
	} else if (smoking) {
		if (CsGame::GetSingletonPtr()->GetGameTime() - startTime >  SMOKE_TIME + EXPLOSION_TIME) {
			smoke->setEmitting(false);
			smoking = false;

			explosionLight->setVisible(false);
		}
	}

	if (!exploding && !smoking) {
		// if the dust has settled
		if (smoke->getNumParticles() == 0) {
			CsGame::GetSingletonPtr()->GetSceneMgr()->getRootSceneNode()->removeChild(node);
			active = false;
			//explosionLight->setVisible(false);
		}
	}

}

/************************************************************************************************/

CsExplosionManager::CsExplosionManager(void)
{

}


CsExplosionManager::~CsExplosionManager(void)
{
	// nothing
}

void CsExplosionManager::CreatePool() {
	for (int i = 0; i < MAX_EXPLOSIONS; i++) {
		CsExplosion &e = explosions[i];
		e.node = CsGame::GetSingletonPtr()->GetSceneMgr()->createSceneNode("Explosion Node" + Ogre::StringConverter::toString(i));
		e.explosion = CsGame::GetSingletonPtr()->GetSceneMgr()->createParticleSystem("Explosion" + Ogre::StringConverter::toString(i), "Explosion");
		e.smoke = CsGame::GetSingletonPtr()->GetSceneMgr()->createParticleSystem("Smoke" + Ogre::StringConverter::toString(i), "Smoke");
		e.sparks = CsGame::GetSingletonPtr()->GetSceneMgr()->createParticleSystem("Sparks" + Ogre::StringConverter::toString(i), "Sparks");
		e.explosion->setKeepParticlesInLocalSpace(true);
		e.sparks->setKeepParticlesInLocalSpace(true);
		e.smoke->setKeepParticlesInLocalSpace(true);

		e.node->attachObject(e.explosion);
		e.node->attachObject(e.smoke);
		e.node->attachObject(e.sparks);
		
		e.mExplosionSound.LoadSource(GameSounds::EXPLOSION);
		alSourcef(e.mExplosionSound.mSource, AL_GAIN, 20.0);

		e.active = false;
		e.exploding = e.smoking = false;

		btSphereShape *sphere = new btSphereShape(SPLASH_FACTOR);
		e.splashBody = new btRigidBody(0, 0, sphere);
		e.splashBody->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
		e.explosionObject = new CsObject();
		e.explosionObject->SetType(Chaos::EXPLOSION_OBJECT);
		e.splashBody->setUserPointer(e.explosionObject);

		CsGame::GetSingletonPtr()->GetDynamicsWorld()->addRigidBody(e.splashBody);

		e.explosionLight = CsGame::GetSingletonPtr()->GetSceneMgr()->createLight();
		e.explosionLight->setType(Ogre::Light::LT_POINT);
		e.explosionLight->setCastShadows(false);
		e.explosionLight->setAttenuation(500, 1, 0, 0.0001);
		e.explosionLight->setDiffuseColour(Ogre::ColourValue(1,0.9,0.7));
		e.explosionLight->setSpecularColour(Ogre::ColourValue(1,0.9,0.7));
		e.explosionLight->setVisible(false);
	}
}

void CsExplosionManager::Unload() {
	for (int i = 0; i < MAX_EXPLOSIONS; i++) {
		CsGame::GetSingletonPtr()->GetDynamicsWorld()->removeRigidBody(explosions[i].splashBody);
		delete explosions[i].splashBody->getCollisionShape();
		delete explosions[i].splashBody;
		delete explosions[i].explosionObject;
	}
}

void CsExplosionManager::Create(Ogre::SceneNode* node, float radius) {
	btVector3 pos= OgretoBullet(node->_getDerivedPosition());
	Create(pos, radius);
	return;
	/*
	if (!node) return;

	for (int i = 0; i < POOL_SIZE; i++) {
		if (explosions[i].active == true || explosions[i].pending == true) continue;

		CsExplosion &e = explosions[i];

		e.active = false;
		e.pending = true;

		e.mExplosionSound.SetPosition(node->getPosition());
		//e.mExplosionSound.SetPosition(Ogre::Vector3(0,0,0));
		//e.mExplosionSound.SetVelocity(Ogre::Vector3());
		e.mExplosionSound.UpdateSound();
		e.mExplosionSound.Play();

		CsRenderMessage msg;
		msg.type = CsRenderMessage::CREATE_EXPLOSION;
		msg.info.explosion.node = node;
		msg.info.explosion.radius = radius;
		msg.info.explosion.explosion = &e;
		CsGame::GetSingletonPtr()->GetRenderTempQueue()->push(msg);

		return; // job done
	}
	*/

}

void CsExplosionManager::Create(btVector3 &pos, float radius) {
	for (int i = 0; i < POOL_SIZE; i++) {
		if (explosions[i].active == true || explosions[i].pending == true) continue;

		CsExplosion &e = explosions[i];

		e.active = false;
		e.pending = true;

		e.mExplosionSound.SetPosition(pos);
		e.mExplosionSound.SetGain(radius);
		//e.mExplosionSound.SetPosition(Ogre::Vector3(0,0,0));
		//e.mExplosionSound.SetVelocity(Ogre::Vector3());
		e.mExplosionSound.UpdateSound();
		e.mExplosionSound.Play();

		CsRenderMessage msg;
		msg.type = CsRenderMessage::CREATE_EXPLOSION;
		msg.info.explosion.x = pos.x();
		msg.info.explosion.y = pos.y();
		msg.info.explosion.z = pos.z();
		msg.info.explosion.radius = radius;
		msg.info.explosion.explosion = &e;
		CsGame::GetSingletonPtr()->GetRenderTempQueue()->push(msg);

		e.splashBody->setWorldTransform(btTransform(btQuaternion(), pos));
		e.splashBody->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT);

		return; // job done
	}


}

/*
void CsExplosionManager::SplashEffect(btVector3 &pos, float radius) {
	CsConvexResultCallback callback(0xffffffff, 0xffffffff, pos);

	btTransform from(btQuaternion(0,0,0,1), pos);
	btTransform to(btQuaternion(0,0,0,1), pos + btVector3(0.1,0,0));
	CsGame::GetSingletonPtr()->GetDynamicsWorld()->convexSweepTest(explosionSphere,from, to, callback);

	// now the other side
	from = btTransform(btQuaternion(0,0,0,1), pos);
	to = btTransform(btQuaternion(0,0,0,1), pos - btVector3(0.1,0,0));
	CsGame::GetSingletonPtr()->GetDynamicsWorld()->convexSweepTest(explosionSphere,from, to, callback);

	std::vector<btRigidBody*>& hitList = callback.getHitList();
		
	if (hitList.size() == 0) return;

	for (int i = 0; i < hitList.size(); i++) {
		btRigidBody* body = hitList.at(i); 
		btVector3 relPos = body->getCenterOfMassPosition() - pos;
		float length2 = relPos.length2();

		if (body->getActivationState() != DISABLE_DEACTIVATION && body->getActivationState() != DISABLE_SIMULATION)
			body->setActivationState(ACTIVE_TAG);

		btVector3 impulse = relPos.normalize() * 50000 / (1 + relPos.length2());
		body->applyCentralImpulse(impulse);

	}
		
}
*/

void CsExplosionManager::CreateRender(CsExplosion* e, Ogre::SceneNode* node, float radius) {
	Ogre::Vector3 pos = node->_getDerivedPosition();	
	CreateRender(e, pos.x, pos.y, pos.z, radius);
	return;

		//e->node->setPosition(node->_getDerivedPosition());

		//// set particle size according to radius
		//e->explosion->setDefaultDimensions(radius * 0.05, radius * 0.05);
		//e->smoke->setDefaultDimensions(radius * 2.0, radius * 2.0);
		//e->sparks->setDefaultDimensions(radius * 4.0, radius * 8.0);

		//e->explosion->setEmitting(true);
		//e->smoke->setEmitting(false);
		//e->sparks->setEmitting(true);

		//CsGame::GetSingletonPtr()->GetSceneMgr()->getRootSceneNode()->addChild(e->node);
		//
		//e->exploding = true;
		//e->smoking = false;
		//e->active = true;
		//e->pending = false;

		////set the timer
		//e->startTime = CsGame::GetSingletonPtr()->GetGameTime();
}

void CsExplosionManager::CreateRender(CsExplosion* e, float x, float y, float z, float radius) {
		e->node->setPosition(x,y,z);
		float scale = radius * 1.0;
		e->node->setScale(scale, scale, scale);

		// set particle size according to radius
		//e->explosion->setDefaultDimensions(1, 1);
		//e->smoke->setDefaultDimensions(1, 1);
		//e->sparks->setDefaultDimensions(0.5, 1.0);

		e->explosion->setEmitting(true);
		e->smoke->setEmitting(false);
		e->sparks->setEmitting(true);

		CsGame::GetSingletonPtr()->GetSceneMgr()->getRootSceneNode()->addChild(e->node);
		
		e->exploding = true;
		e->smoking = false;
		e->active = true;
		e->pending = false;

		e->explosionLight->setPosition(x,y,z);
		e->explosionLight->setVisible(true);

		//set the timer
		e->startTime = CsGame::GetSingletonPtr()->GetGameTime();
}


void CsExplosionManager::UpdateRender() {
	for (int i = 0; i < POOL_SIZE; i++) {
		if (explosions[i].active == true) explosions[i].UpdateRender();
	}
}
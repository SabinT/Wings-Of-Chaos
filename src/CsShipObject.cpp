#include <precompiled.h>
#include <CsShipObject.h>
#include <CsGame.h>
#include <CsRigidBody.h>
#include <CsBulletManager.h>
#include <CsGroundEnemyManager.h>
#include <CsGroundEnemy.h>
#include <cassert>

#include <OgreBulletConverter.h>

const float MAX_LINEAR_VEL = 300;
const float MAX_ANGULAR_VEL = 50;

const float THROTTLE_FORCE = 50000;
const float LINEAR_DAMPING = 0.7;
const float ANGULAR_DAMPING = 0.8;

const float ROLL_TORQUE = 100000;
const float PITCH_TORQUE = 50000;

const float DEFAULT_LINEAR_DAMPING = 0.1;
const float DEFAULT_ANGULAR_DAMPING = 0.7;

CsShipObject* CsShipObject::sPlayerShip = NULL;

CsShipObject::CsShipObject(void)
{
	Reset();

	mRightWing = NULL;
	mLeftWing = NULL;
	mType = Chaos::SHIP_OBJECT;

	mAIShip = true;
}

void CsShipObject::SetPlayerShip(bool b) {
	if (b == true) {
		mAIShip = false;
		sPlayerShip = this;
		//mDot->setMaterialName("GreenDot");
	} else {
		mAIShip = true;
		//mDot->setMaterialName("RedDot");
	}
}

void CsShipObject::Reset() 
{
	mType = Chaos::SHIP_OBJECT;

	mThrottleForce = 0;
	mPitchTorque = 0;
	mRollTorque = 0;

	mLinearDamping = DEFAULT_LINEAR_DAMPING;
	mAngularDamping = DEFAULT_ANGULAR_DAMPING;
	mShipStatus = INACTIVE;

	mJetEngineFX = NULL;
}

void CsShipObject::SetRigidBody (btRigidBody* rigidBody) {
	CsPhysicsWorldObject::SetRigidBody(rigidBody);
	
	ResetDamping();
	// and possibly other parameters too
}

CsShipObject::~CsShipObject(void)
{
}

void CsShipObject::SetLeftWing(Ogre::SceneNode* leftWing) {
	mLeftWing = leftWing;
	// if (mNode) mNode->addChild(mLeftWing);
}

void CsShipObject::SetRightWing(Ogre::SceneNode* rightWing) {
	mRightWing = rightWing;
	// if (mNode) mNode->addChild(mRightWing);
}

CsShipObject* CsShipObject::Clone(Ogre::String name) {
	CsShipObject *newShip = new CsShipObject;

	// this copies all non-pointer data
	*newShip = *this;

	Ogre::Entity *tempEntity;
	Ogre::SceneNode *tempNode;

	// new main node to hold it all together
	newShip->SetNode(CsGame::GetSingletonPtr()->GetSceneMgr()->createSceneNode(name + "_mainnode"));

	// clone models and fix the default model
	if (mPerfectModel) {
		tempEntity = ((Ogre::Entity*)mPerfectModel->getAttachedObject(0))->clone(name + "_perfect");
		tempNode = CsGame::GetSingletonPtr()->GetSceneMgr()->createSceneNode(name + "_perfect");
		tempNode->attachObject(tempEntity);
		newShip->SetPerfectModel(tempNode);
	} else {
		newShip->SetPerfectModel(CsGame::GetSingletonPtr()->GetSceneMgr()->createSceneNode(name + "_perfect"));
		newShip->GetPerfectModel()->attachObject(CsGame::GetSingletonPtr()->GetSceneMgr()->createEntity("ogrehead.mesh"));
	}

	if (mDamagedModel) {
		tempNode = CsGame::GetSingletonPtr()->GetSceneMgr()->createSceneNode(name + "_damaged");
		tempEntity = ((Ogre::Entity*)mDamagedModel->getAttachedObject(0))->clone(name + "_damaged");
		tempNode->attachObject(tempEntity);
		newShip->SetDamagedModel(tempNode);
	} else {
		newShip->SetDamagedModel(CsGame::GetSingletonPtr()->GetSceneMgr()->createSceneNode(name + "_damaged"));
		newShip->GetDamagedModel()->attachObject(CsGame::GetSingletonPtr()->GetSceneMgr()->createEntity("penguin.mesh"));
	}

	if (mDestroyedModel) {
		tempNode = CsGame::GetSingletonPtr()->GetSceneMgr()->createSceneNode(name + "_destroyed");
		tempEntity = ((Ogre::Entity*)mDestroyedModel->getAttachedObject(0))->clone(name + "_destroyed");
		tempNode->attachObject(tempEntity);
		newShip->SetDestroyedModel(tempNode);
	} else {
		newShip->SetDestroyedModel(CsGame::GetSingletonPtr()->GetSceneMgr()->createSceneNode(name + "_destroyed"));
		newShip->GetDestroyedModel()->attachObject(CsGame::GetSingletonPtr()->GetSceneMgr()->createEntity("woodpallet.mesh"));
	}

	assert(mRightWing);
	if (mRightWing) {
		tempNode = CsGame::GetSingletonPtr()->GetSceneMgr()->createSceneNode(name + "_rightwing");
		newShip->SetRightWing(tempNode);
		tempNode->setPosition(mRightWing->getPosition());
		tempNode->setOrientation(mRightWing->getOrientation());
	} else {
		newShip->SetRightWing(CsGame::GetSingletonPtr()->GetSceneMgr()->createSceneNode(name + "_rightwing"));
		newShip->GetRightWing()->translate(5,0,0);
	}

	assert(mLeftWing);
	if (mLeftWing) {
		tempNode = CsGame::GetSingletonPtr()->GetSceneMgr()->createSceneNode(name + "_leftwing");
		newShip->SetLeftWing(tempNode);
		tempNode->setPosition(mLeftWing->getPosition());
		tempNode->setOrientation(mLeftWing->getOrientation());
	} else {
		newShip->SetLeftWing(CsGame::GetSingletonPtr()->GetSceneMgr()->createSceneNode(name + "_leftwing"));
		newShip->GetLeftWing()->translate(-5,0,0);
	}

	// position orientation etc
	newShip->mNode->setPosition(mNode->getPosition());
	newShip->mNode->setOrientation(mNode->getOrientation());
	newShip->mNode->setScale(mNode->getScale());

	newShip->mPerfectModel->setPosition(mPerfectModel->getPosition());
	newShip->mPerfectModel->setOrientation(mPerfectModel->getOrientation());
	newShip->mPerfectModel->setScale(mPerfectModel->getScale());

	newShip->mDamagedModel->setPosition(mDamagedModel->getPosition());
	newShip->mDamagedModel->setOrientation(mDamagedModel->getOrientation());
	newShip->mDamagedModel->setScale(mDamagedModel->getScale());

	newShip->mDestroyedModel->setPosition(mDestroyedModel->getPosition());
	newShip->mDestroyedModel->setOrientation(mDestroyedModel->getOrientation());
	newShip->mDestroyedModel->setScale(mDestroyedModel->getScale());

	// duplicate the rigid body
	btRigidBody* tempBody = ((CsRigidBody*) mRigidBody)->Clone();
	((CsMotionState* )tempBody->getMotionState())->SetObject(newShip);
	newShip->SetRigidBody(tempBody);
	
	// particle effects
	newShip->InitParticleFX();

	// TODO: clone weapon manager
	return newShip;

}
//=====================================================================================

void CsShipObject::Spawn() {
	assert(mNode);
	//if(!mRightWing->getParentSceneNode()) mNode->addChild(mRightWing);
	//if(!mLeftWing->getParentSceneNode()) mNode->addChild(mLeftWing);

	//if(!mNode->getParent()) CsGame::GetSingletonPtr()->GetSceneMgr()->getRootSceneNode()->addChild(mNode);
	CsRenderMessage msg;
	msg.type = CsRenderMessage::ATTACH_NODE;
	msg.info.nodeInfo.parent = CsGame::GetSingletonPtr()->GetSceneMgr()->getRootSceneNode();
	msg.info.nodeInfo.child = mNode;
	CsGame::GetSingletonPtr()->GetRenderTempQueue()->push(msg);

	msg.info.nodeInfo.parent = mNode;
	msg.info.nodeInfo.child = mPerfectModel;
	CsGame::GetSingletonPtr()->GetRenderTempQueue()->push(msg);

	msg.info.nodeInfo.child = mRightWing;
	CsGame::GetSingletonPtr()->GetRenderTempQueue()->push(msg);

	msg.info.nodeInfo.child = mLeftWing;
	CsGame::GetSingletonPtr()->GetRenderTempQueue()->push(msg);

	// stimalsena: re-enabled
	//msg.type = CsRenderMessage::SHOW_DOT;
	//msg.info.dot = mDot;
	//CsGame::GetSingletonPtr()->GetRenderTempQueue()->push(msg);
	//--------------------
	// now start the physics part
	
	if (!mRigidBody->isInWorld()) CsGame::GetSingletonPtr()->GetBulletManager()->GetDynamicsWorld()->addRigidBody(mRigidBody, Chaos::SHIP_OBJECT, Chaos::SHIP_MASK);
	//mRigidBody->setActivationState(ACTIVE_TAG);

	mShipStatus = ACTIVE;
	mHP = Chaos::PERFECT;
	mPrevHealth = mCurrentHealth = Chaos::PERFECT;

	if (mAIShip) {
		mFSM->SetCurrentState(CsIdle::Instance());
		//mFSM->ChangeState(CsChase::Instance());
	}

	SetDamping();
	ResetDamping();
	ResetThrottle();
	mActive = true;
	
	// sounds
	mJetSound.Loop(true);
	mJetSound.Play();
}

// dont need this function
void CsShipObject::Destroy() {
	mJetSound.Stop();

	// stimalsena: re-enabled
	//CsRenderMessage msg;
	//msg.type = CsRenderMessage::HIDE_DOT;
	//msg.info.dot = mDot;
	//CsGame::GetSingletonPtr()->GetRenderTempQueue()->push(msg);
	// ---------------------
}

void CsShipObject::Update() {
	btVector3 vel = mRigidBody->getLinearVelocity();
	float linVel = vel.length();
	// jet sound
	mJetSound.SetPosition(mRigidBody->getWorldTransform().getOrigin());
	mJetSound.SetVelocity(vel);
	mJetSound.UpdateSound();

	float pitchf = 1 + (linVel - 150) / 300.0f;
	alSourcef(mJetSound.mSource, AL_PITCH, pitchf);

	if (mAIShip == true) {
		UpdateAI();
		return;
	}

	// perform radar upadates (target autolocking)
	if (((int)CsGame::GetSingletonPtr()->GetGameTime()) % TARGET_SCAN_DELAY == 0) FindNewTarget(this);
	if (mTarget) {
		if (!mTarget->IsActive()) ResetTarget();
	}
	//SetTarget(CsGame::GetSingletonPtr()->GetGroundEnemyManager()->at(0)->GetNode());
	
	// clear and set forces (and torques as well)
	mRigidBody->clearForces();

	btQuaternion btQuat = GetConvertedTrans().getRotation();

	Ogre::Quaternion quat(btQuat.w(), btQuat.x(), btQuat.y(), btQuat.z());
	Ogre::Vector3 ogreZ = quat.zAxis();
	btVector3 zAxis = btVector3(ogreZ.x, ogreZ.y, ogreZ.z);

	Ogre::Vector3 ogreX = quat.xAxis();
	btVector3 xAxis = btVector3(ogreX.x, ogreX.y, ogreX.z);

	btVector3 yAxis = OgretoBullet(quat.yAxis());

	mRigidBody->applyCentralForce(-zAxis * mThrottleForce);

	// tweak velocity
	//float vel = mRigidBody->getLinearVelocity().length();
	mRigidBody->setLinearVelocity( -zAxis * 0.05 * linVel + 0.95 * mRigidBody->getLinearVelocity() );


	// roll;
	if (mRollTorque != 0) {
		mRigidBody->applyTorque(zAxis * mRollTorque);
		//mRigidBody->applyTorque(yAxis * mRollTorque / 2.0);
	}
	//if (mRollTorque != 0) mRigidBody->applyTorque(zAxis * mRollTorque / 100 * mass);

	// pitch
	//if (mPitchTorque != 0) mRigidBody->applyTorque(xAxis * mPitchTorque / 100 * mass);
	if (mPitchTorque != 0) mRigidBody->applyTorque(xAxis * mPitchTorque);
}

void CsShipObject::UpdateSceneNode() {
	CsDestructibleObject::UpdateSceneNode();

	// TODO: re-enable overlay
	// stimalsena: re-enabled --------------------------------
//	Ogre::Vector3 pos = mNode->getPosition();
//	Ogre::OverlayContainer* miniMap = Ogre::OverlayManager::getSingleton().getByName("MiniMapOverlay")->getChild("MiniMapPanel");
//	float w = miniMap->getWidth();
//	float h = miniMap->getHeight();
////
//	float mx = w * 0.5 * (pos.x / 7500.0 + 1);
//	float my = h * 0.5 * (pos.z / 7500.0 + 1);
//
//	mDot->setPosition(miniMap->getLeft() + mx - mDot->getWidth() / 2, miniMap->getTop() + my - mDot->getHeight() / 2);
//
//	if (mHP < 0) mDot->hide();
	// stimalsena: re-enabled ---------------------------------------
	//UpdateTargetRender();
}
//---------------------- movement -------------------------
void CsShipObject::SetThrottle() {
	mThrottleForce = THROTTLE_FORCE;
}

void CsShipObject::ResetThrottle() {
	mThrottleForce = 0.;
}

void CsShipObject::SetDamping() {
	//mRigidBody->setDamping(0.8, 0.8);
	mRigidBody->setDamping(LINEAR_DAMPING, ANGULAR_DAMPING);
}

void CsShipObject::ResetDamping() {
	mRigidBody->setDamping(DEFAULT_LINEAR_DAMPING, DEFAULT_ANGULAR_DAMPING);
}

void CsShipObject::Roll(int axisValue) {
	static int prevState = 0;
	
	int curState = 0;
	if (axisValue > 50) curState = 1;
	else if (axisValue < -50) curState = -1;

	if (curState == prevState) return;

	btQuaternion btQuat = mRigidBody->getOrientation();

	Ogre::Quaternion quat(btQuat.w(), btQuat.x(), btQuat.y(), btQuat.z());
	Ogre::Vector3 ogreZ = quat.zAxis();
	btVector3 zAxis = btVector3(ogreZ.x, ogreZ.y, ogreZ.z);

	if (curState > 0) {
		// rotate CW about +ve Z axis
		//mAngularDamping =
		mRollTorque = - ROLL_TORQUE;
		//mRigidBody->setAngularVelocity(mRigidBody->getAngularVelocity() - zAxis * TURN_VELOCITY);
	} 
	else if (curState < 0) {
		// rotate CCw
		mRollTorque = ROLL_TORQUE;
		//mRigidBody->setAngularVelocity(mRigidBody->getAngularVelocity() + zAxis * TURN_VELOCITY);
	} 
	else if (curState == 0) {
		mRollTorque = 0;
		//btVector3 angVel = mRigidBody->getAngularVelocity();
		//btVector3 newVel = mRigidBody->getAngularVelocity() - (angVel.dot(zAxis) * zAxis);
		//// nullify the roll component
		//mRigidBody->setAngularVelocity(newVel);
	}

	prevState = curState;
}

void CsShipObject::Pitch(int axisValue) {
	static int prevState = 0;
	
	int curState = 0;
	if (axisValue > 50) curState = 1;
	else if (axisValue < -50) curState = -1;

	if (curState == prevState) return;

	btQuaternion btQuat = mRigidBody->getOrientation();

	Ogre::Quaternion quat(btQuat.w(), btQuat.x(), btQuat.y(), btQuat.z());
	Ogre::Vector3 ogreX = quat.xAxis();
	btVector3 xAxis = btVector3(ogreX.x, ogreX.y, ogreX.z);

	if (curState < 0) {
		// rotate CW about +ve X axis
		mPitchTorque = -PITCH_TORQUE;
		// mRigidBody->setAngularVelocity(mRigidBody->getAngularVelocity() - xAxis * TURN_VELOCITY);
	} 
	else if (curState > 0) {
		// rotate CCw
		mPitchTorque = PITCH_TORQUE;
		//mRigidBody->setAngularVelocity(mRigidBody->getAngularVelocity() + xAxis * TURN_VELOCITY);
	} 
	else {
		mPitchTorque = -0.0;
		//btVector3 angVel = mRigidBody->getAngularVelocity();
		//btVector3 newVel = mRigidBody->getAngularVelocity() - (angVel.dot(xAxis) * xAxis);
		//// nullify the pitch component
		//mRigidBody->setAngularVelocity(newVel);
	}

	prevState = curState;
}

void CsShipObject::FirePrimary() {
	if (!(mRightWing->getParentSceneNode())) mNode->addChild(mRightWing);
	if (!(mLeftWing->getParentSceneNode())) mNode->addChild(mLeftWing);

	mMachineGun.Fire(this);
}

void CsShipObject::FireSecondary() {
	if (!(mRightWing->getParentSceneNode())) mNode->addChild(mRightWing);
	if (!(mLeftWing->getParentSceneNode())) mNode->addChild(mLeftWing);

	mRocketLauncher.Fire(this);
}

void CsShipObject::UpdateAI() {
	// santo
	//CalculateSteering(this);

	//// apply results
	//mRigidBody->clearForces();
	//mRigidBody->applyCentralForce(mSteeringForce);
	//mRigidBody->setAngularVelocity(mSteeringW);

	mFSM->Update();
}
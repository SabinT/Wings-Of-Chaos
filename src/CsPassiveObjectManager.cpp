#include <precompiled.h>

#include <CsPhysicsWorldObject.h>
#include <CsPassiveObjectManager.h>
#include <CsGame.h>
#include <cassert>
#include <CsMotionState.h>

CsPassiveObjectManager::CsPassiveObjectManager(void)
{
}


CsPassiveObjectManager::~CsPassiveObjectManager(void)
{
}

void CsPassiveObjectManager::Load(Ogre::SceneNode *ogreScene, CsBulletWorldImporter *bulletScene, const char *prefix) {
	Ogre::Node::ChildNodeIterator it = ogreScene->getChildIterator();
	
	std::vector<Ogre::SceneNode*> nodesToRemove;

	Ogre::SceneNode *tempNode;
	btRigidBody *tempBody;
	Ogre::String btString;

	while (it.hasMoreElements()) {
		tempNode = static_cast <Ogre::SceneNode*> (it.getNext());
		assert(tempNode);

		tempBody = NULL;
		if ((tempNode->getName().find(prefix) != std::string::npos)) {
			if (tempNode->getName().find("bt") != std::string::npos) continue;

			btString = "bt" + tempNode->getName();
			tempBody = bulletScene->getRigidBodyByName(btString.c_str());
			if (!tempBody) {
				btString = tempNode->getName();
				tempBody = bulletScene->getRigidBodyByName(btString.c_str());
			}

			if (tempBody) {
				CsPhysicsWorldObject *tempObj = new CsPhysicsWorldObject;

				Ogre::Vector3 pos = tempNode->getPosition();
				Ogre::Quaternion rot = tempNode->getOrientation();
				btTransform trans(btQuaternion(rot.x, rot.y, rot.z, rot.w), btVector3(pos.x, pos.y, pos.z));
				CsMotionState *motionState = new CsMotionState(tempObj, trans, tempBody->getCenterOfMassTransform());
				tempBody->setMotionState(motionState);

				tempBody->setActivationState(DISABLE_DEACTIVATION);

				tempObj->SetNode(tempNode);
				

				tempObj->SetRigidBody(tempBody);
				CsGame::GetSingletonPtr()->GetDynamicsWorld()->addRigidBody(tempObj->GetRigidBody(), Chaos::PASSIVE_OBJECT, Chaos::PASSIVE_MASK);
				tempObj->SetCollidesWith(Chaos::PASSIVE_MASK);			
				
				push_back(tempObj);
			}

			nodesToRemove.push_back(tempNode);
		}
	}

	std::vector<Ogre::SceneNode*>::iterator removeIt = nodesToRemove.begin();
	while (removeIt != nodesToRemove.end()) {
		ogreScene->removeChild(*removeIt);
		CsGame::GetSingletonPtr()->GetSceneMgr()->getRootSceneNode()->addChild(*removeIt);
		removeIt++;
	}

	//// get extra data description from entity 'i_entity' in scene 'm_scene'
	//OgreMax::OgreMaxScene* ogreMaxFile = CsGame::GetSingletonPtr()->GetOgreMaxSceneFile();
	//Ogre::MovableObject* userEntity = CsGame::GetSingletonPtr()->GetSceneMgr()->getSceneNode("Passive003")->getAttachedObject(0);
	//OgreMax::Types::ObjectExtraData *objDesc = ogreMaxFile->GetObjectExtraData(userEntity);

	//if ((objDesc!=NULL) && (objDesc->HasUserData()))
	//{
	//	// get extra data
	//	Ogre::String userObjData = objDesc->userData;
	//	if (!userObjData.empty())
	//	{
	//		Ogre::LogManager::getSingleton().logMessage(userObjData);
	//	}
	//}
}

void CsPassiveObjectManager::Unload() {
	CsPhysicsWorldObject* tempObj;

	while (!empty()) {
		tempObj = back();
		pop_back();
		//delete only the motionstate that we created
		//CsGame::GetSingletonPtr()->GetDynamicsWorld()->removeRigidBody(tempObj->GetRigidBody());
		delete tempObj;
	}
		
}

void CsPassiveObjectManager::UpdateAllObjects() {
	iterator it;
	for (it = begin(); it != end(); it++)
		(*it)->UpdateSceneNode();
}
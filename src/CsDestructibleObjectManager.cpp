#include <precompiled.h>

#include <CsDestructibleObjectManager.h>
#include <CsGame.h>
#include <CsMotionState.h>
#include <CsDestructibleObject.h>
#include <CsObjectiveList.h>
#include <CsGameLevel.h>

CsDestructibleObjectManager::CsDestructibleObjectManager(void)
{
}

CsDestructibleObjectManager::~CsDestructibleObjectManager(void)
{
}

void CsDestructibleObjectManager::Load(Ogre::SceneNode *ogreScene, btBulletWorldImporter *bulletScene, const char *prefix) {
	Ogre::Node::ChildNodeIterator it = ogreScene->getChildIterator();
	
	std::vector<Ogre::SceneNode*> nodesToRemove;

	Ogre::SceneNode *tempNode;
	btRigidBody *tempBody;
	Ogre::String btString;

	while (it.hasMoreElements()) {
		tempNode = static_cast <Ogre::SceneNode*> (it.getNext());
		assert(tempNode);

		tempBody = NULL;

		// make sure this node is an instance, not a template
		if ((tempNode->getName().find(prefix) != std::string::npos) && 
			(tempNode->getName().find("damaged") == std::string::npos) &&
			(tempNode->getName().find("destroyed") == std::string::npos)) 
		{
			if (tempNode->getName().find("bt") != std::string::npos) continue;

			btString = "bt" + tempNode->getName();
			tempBody = bulletScene->getRigidBodyByName(btString.c_str());
			
			if (!tempBody) {
				btString = tempNode->getName();
				tempBody = bulletScene->getRigidBodyByName(btString.c_str());
			}
			if (!tempBody) continue;

			if (tempBody) {
				CsDestructibleObject *tempObj = new CsDestructibleObject;

				Ogre::Vector3 pos = tempNode->getPosition();
				Ogre::Quaternion rot = tempNode->getOrientation();
				btTransform trans(btQuaternion(rot.x, rot.y, rot.z, rot.w), btVector3(pos.x, pos.y, pos.z));
				CsMotionState *motionState = new CsMotionState(tempObj, trans, tempBody->getCenterOfMassTransform());
				tempBody->setMotionState(motionState);
				tempBody->setActivationState(DISABLE_DEACTIVATION);

				// new node to hold it all together, no worries about transform since it will be pumped by physics each frame
				tempObj->SetNode(CsGame::GetSingletonPtr()->GetSceneMgr()->getRootSceneNode()->createChildSceneNode());
				
				tempNode->setPosition(0,0,0);
				tempNode->setOrientation(1,0,0,0);

				tempObj->SetPerfectModel(tempNode);

				tempObj->SetRigidBody(tempBody);
				CsGame::GetSingletonPtr()->GetDynamicsWorld()->addRigidBody(tempBody, Chaos::DESTRUCTIBLE_OBJECT, Chaos::DESTRUCTIBLE_MASK);
				tempObj->SetCollidesWith(Chaos::DESTRUCTIBLE_MASK);
				
				{
					Ogre::Node::ChildNodeIterator it2 = ogreScene->getChildIterator();
					Ogre::SceneNode *tempNode2;
					bool damagedFound = false, destroyedFound = false;

					Ogre::String damagedName = tempNode->getName().substr(0, tempNode->getName().length() - 3) + "_damaged";
					Ogre::String destroyedName = tempNode->getName().substr(0, tempNode->getName().length() - 3) + "_destroyed";

					while (it2.hasMoreElements()) {
						tempNode2 = static_cast <Ogre::SceneNode*> (it2.getNext());

						if ((tempNode2->getName() == damagedName)) {
							// duplication
							//Ogre::SceneNode *duplicateNode = CsGame::GetSingletonPtr()->GetSceneMgr()->createSceneNode();
							//Ogre::Entity *duplicateEntity = ((Ogre::Entity*) tempNode2->getAttachedObject(0))->clone(tempNode->getName() + "_damaged");
							//duplicateNode->attachObject(duplicateEntity);

							//duplicateNode->setPosition(0,0,0);
							//duplicateNode->setOrientation(1,0,0,0);
							tempObj->SetDamagedModel(tempNode);
							damagedFound = true;
						}
						if ((tempNode2->getName() == destroyedName)) {
							// duplication
							//Ogre::SceneNode *duplicateNode = CsGame::GetSingletonPtr()->GetSceneMgr()->createSceneNode();
							//Ogre::Entity *duplicateEntity = ((Ogre::Entity*) tempNode2->getAttachedObject(0))->clone(tempNode->getName() + "_destroyed");
							//duplicateNode->attachObject(duplicateEntity);

							//duplicateNode->setPosition(0,0,0);
							//duplicateNode->setOrientation(1,0,0,0);
							tempObj->SetDestroyedModel(tempNode);
							destroyedFound = true;
						}
					}
					if (!damagedFound) tempObj->SetDamagedModel(tempNode);
					if (!destroyedFound) tempObj->SetDestroyedModel(tempNode);

					// create the particle effects
					tempObj->InitParticleFX();

					//Ogre::MovableObject* userEntity = dObj->GetPerfectModel()->getAttachedObject(0);
					OgreMax::Types::ObjectExtraData *objDesc = CsGame::GetSingletonPtr()->GetGameLevel()->GetOgreMap()->GetObjectExtraData(tempNode);

					if ((objDesc!=NULL) && (objDesc->HasUserData()))
					{
						// get extra data
						Ogre::String userObjData = objDesc->userData;
						if (!userObjData.empty())
						{
							CsGame::GetSingletonPtr()->GetGameGUI()->WriteToConsole(userObjData.c_str());

							// create xml document
							TiXmlDocument doc;
							doc.Parse(userObjData.c_str());
							TiXmlHandle hnd(&doc);
							TiXmlElement* elm = hnd.FirstChild("Objective").ToElement();
							TiXmlText* text = hnd.FirstChild("Objective").FirstChild().ToText();
							if (elm) {
								// create an objective
								CsObjective objective;

								if (std::string("Protect") == elm->Attribute("type"))
									objective.type = CsObjective::PROTECT;
								else
									objective.type = CsObjective::DESTROY;
					
								if(text) { 
									objective.description = text->Value();
								}

								objective.object = tempObj;
								CsGame::GetSingletonPtr()->GetGameLevel()->mObjectives->Add(objective);
							}
						}
					}

					push_back(tempObj);
				}
			}

			nodesToRemove.push_back(tempNode);
			//ogreScene->removeChild(tempNode);
		}
	}

	std::vector<Ogre::SceneNode*>::iterator removeIt = nodesToRemove.begin();
	while (removeIt != nodesToRemove.end()) {
		ogreScene->removeChild(*removeIt);
		removeIt++;
	}
	
	std::vector<CsDestructibleObject*>::iterator spawnObj = begin();
	while (spawnObj != end()) {
		(*spawnObj)->Spawn();
		spawnObj++;
	}
}

void CsDestructibleObjectManager::Unload() {
	iterator it;
	for (it = begin(); it != end(); it++) {
		delete (*it);
	}
}

void CsDestructibleObjectManager::UpdateAllObjects() {
	iterator it;
	for (it = begin(); it != end(); it++) {
		if ((*it)->IsActive()) (*it)->UpdateSceneNode();
	}
}
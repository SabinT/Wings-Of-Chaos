#include "precompiled.h"
#include <CsGameLevel.h>

#include <CsPassiveObjectManager.h>
#include <CsDestructibleObjectManager.h>
#include <CsGroundEnemyManager.h>
#include <CsGroundEnemy.h>

#include <CsShipObject.h>
#include <CsShipManager.h>
#include <CsTerrainManager.h>

#include <CsObjectiveList.h>

#include <CsGame.h>

#include <OgreMax/tinyxml/tinyxml.h>

CsGameLevel::CsGameLevel()
{
	mPlayerShip = NULL;
	mPhysicsMapFile = NULL;
	mPhysicsMap = NULL;

	mOgreScene = NULL;
	mOgreMap = NULL;

	mPassiveObjectManager = NULL;
	mDestructibleObjectManager = NULL;
	mGroundEnemyManager = NULL;
	
	mShipManager = NULL;
	mPlayerShip = NULL;

	mTerrainManager = NULL;
	
	mGroundPlaneRigidBody = NULL;
	mObjectives = NULL;
}


CsGameLevel::~CsGameLevel(void)
{
}

void CsGameLevel::Load (const char *levelName) {
	Ogre::SceneManager* sceneMgr = CsGame::GetSingletonPtr()->GetSceneMgr();

	mName = levelName;

	std::string levelPath = LEVELS_DIR + mName + "/";
	std::string physicsFilename = LEVELS_DIR + mName + "/" + levelName + ".bullet";
	std::string ogreFilename = LEVELS_DIR + mName + "/" + levelName + ".scene";

	// set up the Lights and the shadows
	sceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
	sceneMgr->setShadowDirectionalLightExtrusionDistance(1000); 
	sceneMgr->setShadowFarDistance(5000);
	
	Ogre::Light* directionalLight = CsGame::GetSingletonPtr()->GetSceneMgr()->createLight("directionalLight");
    directionalLight->setType(Ogre::Light::LT_DIRECTIONAL);
    directionalLight->setDiffuseColour(Ogre::ColourValue(0.7, 0.7, 0.7));
    directionalLight->setSpecularColour(Ogre::ColourValue(.25, .25, 0.1));
	directionalLight->setCastShadows(true);
 
    directionalLight->setDirection(Ogre::Vector3(-1, -1, -1)); 

	sceneMgr->setAmbientLight(Ogre::ColourValue(0.35,0.35,0.35));

	mObjectives = new CsObjectiveList;
	mShipManager = new CsShipManager;
	mPassiveObjectManager = new CsPassiveObjectManager;
	mDestructibleObjectManager = new CsDestructibleObjectManager;
	mGroundEnemyManager = new CsGroundEnemyManager;
	mTerrainManager = new CsTerrainManager;


	// open files ******************************************************************************************************
	mPhysicsMap = new CsBulletWorldImporter(NULL);
	mPhysicsMapFile = new bParse::btBulletFile(physicsFilename.c_str());
	mPhysicsMap->loadFileFromMemory(mPhysicsMapFile);
	// *****************************************************************************************************************

	mOgreMap = new OgreMax::OgreMaxScene;
	mOgreScene = CsGame::GetSingletonPtr()->GetSceneMgr()->createSceneNode("DummyMapNode");
	mOgreMap->Load(ogreFilename, NULL, 0, CsGame::GetSingletonPtr()->GetSceneMgr(), mOgreScene, NULL, "LevelResources");

	mShipManager->LoadTemplates("../../scenes/ships/ships.scene", "../../scenes/ships/ships.bullet");
	mShipManager->LoadSpawnPoints(mOgreScene);
	mShipManager->CreatePool(20);
	
	mPlayerShip = mShipManager->Create(std::string("BlueShip"));
	for (int i = 0; i < 40; i++)
		mShipManager->Create(std::string("RedShip"));

	assert(mPlayerShip);
	btQuaternion q = CsGame::GetSingletonPtr()->GetShipManager()->mEntryRot;
	btVector3  v =   CsGame::GetSingletonPtr()->GetShipManager()->mEntryPos;
	mPlayerShip->SetPlayerShip(true);
	mPlayerShip->GetRigidBody()->setWorldTransform(btTransform(q, v));

	LoadStaticGeometry(mOgreScene);

	// UNCOMMENT THESE!!
	mPassiveObjectManager->Load(mOgreScene, mPhysicsMap);
	mDestructibleObjectManager->Load(mOgreScene, mPhysicsMap);
	mGroundEnemyManager->Load(mOgreScene, mPhysicsMap);

	// stimalsena 10-13, hoping to re-enable terrain
	CsGame::GetSingletonPtr()->GetSceneMgr()->setFog(Ogre::FOG_LINEAR, Ogre::ColourValue( 0.6745098 ,  0.72941176,  0.74117647), 0, 9500, 10000);
	mTerrainManager->Load(levelPath + "terrain/terrain.cfg");	
	mTerrainManager->LoadPhysicsData(levelPath + "terrain/terrain.cfg");

	LoadExtraData();

	// set up a few objectives
	// protect our ship!!
	CsObjective  protectShip(CsObjective::PROTECT, mPlayerShip);
	protectShip.description = "Do not die!!";
	mObjectives->push_back(protectShip);
}

void CsGameLevel::Unload() {
	mPassiveObjectManager->Unload();
	mDestructibleObjectManager->Unload();
	mGroundEnemyManager->Unload();
	mShipManager->Unload();

	DestroyStaticGeometry();

	// close files
	mOgreMap->Destroy();
	delete mOgreMap;

	mPhysicsMap->deletePhysicsData();
	delete mPhysicsMapFile;
	delete mPhysicsMap;
	
	// TESTING: no trees now so no need to destroy trees
	// stimalsena 10-13
	mTerrainManager->Unload();

	// destroy the managers
	delete mShipManager;
	delete mTerrainManager;
	delete mPassiveObjectManager;
	delete mDestructibleObjectManager;
	delete mGroundEnemyManager;

	//Ogre::ResourceGroupManager::getSingleton().unloadResourceGroup("LevelResources");
	Ogre::ResourceGroupManager::getSingleton().destroyResourceGroup("LevelResources");
}

void CsGameLevel::LoadStaticGeometry(Ogre::SceneNode *ogreScene) {
	Ogre::Node::ChildNodeIterator it = ogreScene->getChildIterator();

	std::vector<Ogre::SceneNode*> nodesToRemove;

	Ogre::SceneNode *tempNode;
	btRigidBody *tempBody;
	Ogre::String btString;

	while (it.hasMoreElements()) {
		tempNode = static_cast <Ogre::SceneNode*> (it.getNext());	
		tempBody = NULL;
		if (tempNode->getName().find("Static") != std::string::npos) {
			btString = "bt" + tempNode->getName();

			/// maybe first look for the 'bt' model
			tempBody = mPhysicsMap->getRigidBodyByName(tempNode->getName().c_str());
			if (!tempBody) mPhysicsMap->getRigidBodyByName(btString.c_str());
			if (tempBody) CsGame::GetSingletonPtr()->GetDynamicsWorld()->addRigidBody(tempBody, Chaos::ENVIRONMENT_OBJECT, Chaos::ENVIRONMENT_MASK);

			if (tempNode->getName().find("bt") == std::string::npos)
				nodesToRemove.push_back(tempNode);
		}
	}

	std::vector<Ogre::SceneNode*>::iterator removeIt = nodesToRemove.begin();
	while (removeIt != nodesToRemove.end()) {
		ogreScene->removeChild(*removeIt);
		CsGame::GetSingletonPtr()->GetSceneMgr()->getRootSceneNode()->addChild(*removeIt);
		removeIt++;
	}

	// TODO: read all static configuration from external file
	// TODO: do this from map folder
	CsGame::GetSingletonPtr()->GetSceneMgr()->setSkyBox(true, "spawntestSky", 100.0f, true);
}

void CsGameLevel::DestroyStaticGeometry() {


}

void CsGameLevel::Update() {
	mShipManager->Update();
	mGroundEnemyManager->Update();
	mObjectives->Update();
}

void CsGameLevel::Synchronize() {
	mGroundEnemyManager->Synchronize();
}

void CsGameLevel::UpdateSceneNodes() {
	mPassiveObjectManager->UpdateAllObjects();
	mDestructibleObjectManager->UpdateAllObjects();
	mGroundEnemyManager->UpdateSceneNodes();
	mShipManager->UpdateSceneNodes();
}

void CsGameLevel::LoadExtraData() {
	CsGroundEnemy* gObj;
	CsGroundEnemyManager* gMan = CsGame::GetSingletonPtr()->GetGroundEnemyManager();

	for (int i = 0; i < gMan->size(); i++) {
		gObj = gMan->at(i);

		OgreMax::Types::ObjectExtraData *objDesc = CsGame::GetSingletonPtr()->GetGameLevel()->GetOgreMap()->GetObjectExtraData(gObj->GetNode());

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
					// if (std::string("Destroy") == elm->Attribute("type"))
					objective.type = CsObjective::DESTROY;
					
					if(text) { 
						objective.description = text->Value();
					}

					objective.object = gObj;
					mObjectives->Add(objective);
				}

				elm = hnd.FirstChild("Destructible").ToElement();
				if (elm) {
					float hp, armor;
					elm->QueryFloatAttribute("HP", &hp);
					elm->QueryFloatAttribute("armor", &armor);
					CsGame::GetSingletonPtr()->GetGameGUI()->WriteToConsole(Ogre::StringConverter::toString(hp * armor).c_str());
				}
			}
		}

	}
}

void CsGameLevel::MakeHostile () {
	CsGroundEnemy *gObj;
	for (int i = 0; i < mGroundEnemyManager->size(); i++) {
		gObj = mGroundEnemyManager->at(i);
		gObj->SetTarget(mPlayerShip);
	}

	CsShipObject *sObj;
	for (int i = 0; i < mShipManager->size(); i++) {
		sObj = mShipManager->at(i);
		if (sObj == mPlayerShip) continue;
		sObj->SetTarget(mPlayerShip);
	}
}
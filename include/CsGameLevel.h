#pragma once

#include <CsCommons.h>

#define LEVELS_DIR "../../levels/"

class CsGameLevel
{
public:
	CsGameLevel();
	virtual ~CsGameLevel(void);

	void Update();
	void Synchronize();
	void UpdateSceneNodes();

	void Load(const char* levelName);
	void Unload();

	inline OgreMax::OgreMaxScene* GetOgreMap() { return mOgreMap; }

	// everything targets your ship!!
	void MakeHostile();

	CsObjectiveList *mObjectives;

protected:
	// and objectives of course!!
	void LoadExtraData();

	void LoadStaticGeometry(Ogre::SceneNode *ogreScene);
	void DestroyStaticGeometry();

	std::string mName;

	bParse::btBulletFile *mPhysicsMapFile;
	CsBulletWorldImporter *mPhysicsMap;

	Ogre::SceneNode* mOgreScene;
	OgreMax::OgreMaxScene *mOgreMap;

	//Managers
	CsPassiveObjectManager* mPassiveObjectManager;
	CsDestructibleObjectManager* mDestructibleObjectManager;
	CsGroundEnemyManager *mGroundEnemyManager;
	
	CsShipManager* mShipManager;
	CsShipObject* mPlayerShip;

	CsTerrainManager* mTerrainManager;


	btRigidBody *mGroundPlaneRigidBody;


	friend class CsGame;
};


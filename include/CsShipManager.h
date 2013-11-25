#pragma once

#include <CsCommons.h>
#include <vector>
#include <LinearMath\btAlignedObjectArray.h>
#include <BulletCommons.h>

//conventions in scene file
// ShipXXX (XXX = type) for perfect model
// ShipXXX_damaged, _destroyed
// ShipXXX_leftwing, _rightwing

//convention for spawn points
// SpawnXXX
// attach a dummy entity to the spawn point nodes with extra data

struct SpawnPoint {
	btVector3 mPosition;
	btQuaternion mOrientation;
	float mCoolDownTime;
	float mLastUsedTime;
};

class CsShipManager : public std::vector<CsShipObject*>
{
public:
	CsShipManager(void);
	virtual ~CsShipManager(void);

	void LoadTemplates(Ogre::SceneNode *ogreScene, btBulletWorldImporter *bulletScene, const char *prefix = "Ship");
	void LoadTemplates(const char* ogreFilename, const char* bulletFilename, const char *prefix = "Ship");
	void LoadTemplates_Test();

	void LoadSpawnPoints(Ogre::SceneNode *ogreScene, const char *prefix = "Spawn");

	void Unload();

	void CreatePool(int poolSize);
	
	// shipType string is looked up in the shipMap to locate appropriate template
	CsShipObject* Create(std::string &shipType);

	void Update();
	void UpdateSceneNodes();

	CsShipObject* NextShip();

	btVector3 mEntryPos;
	btQuaternion mEntryRot; 
protected:
	std::map<std::string, CsShipObject*> mShipMap;
	std::vector<CsShipObject*> mTemplates;

	std::vector<std::string> mPending;

	btAlignedObjectArray<SpawnPoint> mSpawnPoints;

	// to store the files needed for loading
	CsBulletWorldImporter *mPhysicsScene;
	OgreMax::OgreMaxScene *mOgreScene;
	bParse::btBulletFile *mPhysicsFile;
};


#pragma once

#include <vector>

#include <CsCommons.h>
#include <BulletCommons.h>
#include <OgrePrerequisites.h>
// conventions
// XXEnemyXXX
// physics: either XXEnemyXXX or btXXEnemyXXX

class CsGroundEnemyManager : public std::vector<CsGroundEnemy *>
{
public:
	CsGroundEnemyManager(void);
	virtual ~CsGroundEnemyManager(void);

	void Load(Ogre::SceneNode *ogreScene, btBulletWorldImporter *bulletScene, const char *prefix = "Enemy");
	
	// Unload done from the scene files,doesnt use factory, just free the memory for objects
	void Unload();

	void Update();
	void Synchronize();
	void UpdateSceneNodes();
};


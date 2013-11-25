#pragma once

#include <vector>

#include <CsCommons.h>
#include <BulletCommons.h>
#include <OgrePrerequisites.h>
// scene conventions
// physics: btXXDestructibleXXX
// regular model: XXDestructibleXXX
// damaged: XXDestructible_damaged
// destroyed: XXDestructible_destroyed
// first two digits = type of destructible

// see CsDestructible.h

class CsDestructibleObjectManager : public std::vector<CsDestructibleObject*>
{
public:
	CsDestructibleObjectManager(void);
	virtual ~CsDestructibleObjectManager(void);

	void Load(Ogre::SceneNode *ogreScene, btBulletWorldImporter *bulletScene, const char *prefix = "Destructible");
	
	// Unload done from the scene files,doesnt use factory, just free the memory for objects
	void Unload();

	void UpdateAllObjects();
};



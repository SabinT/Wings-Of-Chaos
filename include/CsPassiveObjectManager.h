#pragma once
#include <vector>

#include <BulletCommons.h>
#include <CsCommons.h>

// Convention: PassiveXXX

class CsPassiveObjectManager : public std::vector<CsPhysicsWorldObject*>
{
public:
	CsPassiveObjectManager(void);
	virtual ~CsPassiveObjectManager(void);

	void Load(Ogre::SceneNode *ogreScene, CsBulletWorldImporter *bulletScene, const char *prefix = "Passive");
	// Unload done from the scene files, doesnt use factory

	// only delete memory allocated for objects, not the actual meshes or rigidbodies (handled by fileLoaders)
	void Unload();

	void UpdateAllObjects();

};


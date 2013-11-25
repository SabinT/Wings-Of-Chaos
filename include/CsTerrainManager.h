#pragma once


//to calculate the height parameters for the paging objects


//to be added at the Ogre Initialization for the proper execution
//ResourceGroupManager::getSingleton().createResourceGroup("Impostors");

//TODO
//add the camera member from the CsGame class and use singleton to access it

#include <OgrePrerequisites.h>

class btHeightfieldTerrainShape;

class CsTerrainManager
{
public:
	CsTerrainManager(void);
	virtual ~CsTerrainManager(void);

	void Load ( Ogre::String &terrainFile);

	void LoadPhysicsData(Ogre::String &terrainFileStr);
	void Unload();

	float getY (float x, float z);  

protected:

	//bullet part of the terrain
	btRigidBody *mTerrainRigidBody;
	Ogre::Image heightmap;
};


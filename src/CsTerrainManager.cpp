#include <precompiled.h>

#include "CsTerrainManager.h"
#include <CsGame.h>
#include <CsCameraManager.h>
#include <CsObject.h>

#include "HeightFunction.h"

#include <BulletCollision\CollisionShapes\btHeightfieldTerrainShape.h>

// Notes: SetCamera fuction currently called in CsGame::Init() maybe load static geometry part 
// and CsCameraManager::NextCamera etc currently commented out

// some globals
unsigned char* heightfieldData;
//Ogre::Image* heightmap;

CsTerrainManager::CsTerrainManager(void)
{
}


CsTerrainManager::~CsTerrainManager(void)
{

}


void CsTerrainManager::Load(Ogre::String &terrainFile) {
	CsGame::GetSingletonPtr()->GetSceneMgr()->setWorldGeometry(terrainFile);
}

void CsTerrainManager::Unload()  {

	CsGame::GetSingletonPtr()->GetDynamicsWorld()->removeRigidBody(mTerrainRigidBody);
	delete mTerrainRigidBody->getCollisionShape();
	delete mTerrainRigidBody->getMotionState();
	delete mTerrainRigidBody;

	delete heightfieldData;
	//delete heightmap;
}

//-------------academic function

float CsTerrainManager::getY (float x, float z)  {
		return HeightFunction::getTerrainHeight(x, z);
}

//Ogre::Image heightmap;

void CsTerrainManager::LoadPhysicsData(Ogre::String &terrainFileStr)  {
// terrainFileStr is a string, representing the file name of *.cfg file to  be used by TerrainSceneManager

// Load the *.cfg file into a ConfigFile object
Ogre::DataStreamPtr configStream = Ogre::ResourceGroupManager::getSingleton().openResource(terrainFileStr, Ogre::ResourceGroupManager::getSingleton().getWorldResourceGroupName());
Ogre::ConfigFile config;
config.load(configStream);

/* Load the various settings we need.
* These are:
* width:  Saved in the terrainFile as PageSize.  We pass width to Bullet twice, as both width and height,
*     because heightfields in Ogre are square.
* imgFileStr: the string representing the image from which we want to load the heightmap data.
* heightmap: an Ogre::Image object, which contains the data itself.  A reference to this needs to be kept
*     somewhere, because just keeps a pointer to the data, and the data itself will be deleted when the
*     Image is deleted at the end of this block.  I found this out the hard way >.>
* maxHeight: the heightmap will be scaled to go from zero to maxHeight
* heightScale: this is how much each number in the heightmap (0 - 255) will translate to in Ogre/Bullet units.
*    This number is obtained by dividing the maxHeight by 256 which is the total number of different steps in
*    the heightmap before interpolation.
*/
Ogre::String widthStr = config.getSetting("PageSize");
int width = atoi(widthStr.c_str());

Ogre::String imgFile = config.getSetting("Heightmap.image");
heightmap.load(imgFile, Ogre::ResourceGroupManager::getSingleton().getWorldResourceGroupName());

Ogre::String maxHeightStr = config.getSetting("MaxHeight");
btScalar maxHeight = atof(maxHeightStr.c_str());
btScalar heightScale = maxHeight / 256;

	// This code for the localScaling is taken directly from the TerrainSceneManager, adapted
	//      to a btVector3
	//EDIT: Thanks to Ripgiblet for pointing out and fixing some mistakes in this section.
	btVector3 localScaling(1, 1, 1);
	btScalar vScaleX = atof(config.getSetting("PageWorldX").c_str());
	vScaleX /= (float) width - 1;
	btScalar vScaleZ = atof(config.getSetting("PageWorldZ").c_str());
	vScaleZ /= (float) width - 1;
	localScaling.setX(vScaleX);
	localScaling.setZ(vScaleZ);


	heightfieldData = new unsigned char[513 * 513];
	{
		for (int i=0;i<513*513;i++)
		{
			heightfieldData[i]=i;
		}
	}
	// And now, we actually call Bullet.
	// I've actually used btHeightfieldTerrainShape's legacy constructor, but really, it works so well
	//      with the way Ogre used Heightmaps.

	// heightmap.getData()
	// heightfieldData
	// btHeightfieldTerrainShape* terrainShape = new btHeightfieldTerrainShape(width, width, heightmap.getData(), heightScale, 0, maxHeight, 1, PHY_UCHAR, false);
	btHeightfieldTerrainShape* terrainShape = new btHeightfieldTerrainShape(width, width, heightmap.getData(), heightScale, 0, maxHeight, 1, PHY_UCHAR, false);
	terrainShape->setLocalScaling(localScaling);

	// All thats left is to line up the btHeightfieldTerrainShape with the Ogre::SceneNode
	// We have to do it this way because of differences in how Bullet and Ogre orient the shapes.
	// In Ogre, the terrain's top left corner is at (0, 0, 0) in the local TransformSpace
	// In Bullet, not only is the center of the btHeighfield terrrain shape at (0, 0, 0), but from
	//      what I can tell, its immovable.

	btVector3 min, max;
	terrainShape->getAabb(btTransform::getIdentity(), min, max);
	float yOffset = (max - min).y() / 2;
	
	Ogre::SceneNode* sceneNode = CsGame::GetSingletonPtr()->GetSceneMgr()->getSceneNode("Terrain");

	sceneNode->setPosition(Ogre::Vector3(min.x(), /*min.y() + yOffset*/ 0 , min.z()));
	

	// Finally, create your btMotionState, and btRigidBody, and all the rigid body to the physics world.

	// BtOgre::RigidBodyState* state = new BtOgre::RigidBodyState(sceneNode, btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));
	//btMotionState * state = new btDefaultMotionState();
	btMotionState * state = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1), btVector3(0, + yOffset, 0)));

	mTerrainRigidBody = new btRigidBody(0, state, terrainShape);

	// CAREFUUL: RESTORE THIS!!!
	// stimalsena 10/13, restored terrain physics data
	CsGame::GetSingletonPtr()->GetDynamicsWorld()->addRigidBody(mTerrainRigidBody, Chaos::ENVIRONMENT_OBJECT, Chaos::ENVIRONMENT_MASK);

	//rigidBody = new btRigidBody(0, state, collisionShape, btVector3(0, 0, 0));
	//rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);
	//discreteDynamicsWorld->addRigidBody(rigidBody);

}

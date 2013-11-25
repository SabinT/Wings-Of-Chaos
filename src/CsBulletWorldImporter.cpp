#include <precompiled.h>

#include <CsBulletWorldImporter.h>

#include <Serialize\BulletFileLoader\btBulletFile.h>

#include "btBulletDynamicsCommon.h"
#include "BulletCollision/Gimpact/btGImpactShape.h"

#include <CsGame.h>

#include <cstdio>
#include <cassert>


//#define USE_INTERNAL_EDGE_UTILITY
#ifdef USE_INTERNAL_EDGE_UTILITY
#include "BulletCollision/CollisionDispatch/btInternalEdgeUtility.h"
#endif //USE_INTERNAL_EDGE_UTILITY

CsBulletWorldImporter::~CsBulletWorldImporter(void)
{
}

void CsBulletWorldImporter::LoadFileIntoMemory(const char *filename) 
{ 
	int size = 0;
	FILE *f;
	fopen_s(&f, filename, "rb");
	if (f == NULL) 
	{ 
		mFileBuffer = NULL;
		return; // -1 means file opening fail 
	} 
	fseek(f, 0, SEEK_END);
	size = ftell(f);
	fseek(f, 0, SEEK_SET);
	mFileBuffer = (char *)malloc(size+1);
	if (size != fread(mFileBuffer , sizeof(char), size, f)) 
	{ 
		free(mFileBuffer);
		mFileBuffer = NULL;
		return; // -2 means file reading fail
	} 
	fclose(f);
	(mFileBuffer)[size] = 0;
	mFileLength =  size;

	loadFileFromMemory(mFileBuffer, mFileLength);
}

void CsBulletWorldImporter::deletePhysicsData() {
	int i;
	for (i=0;i<m_allocatedRigidBodies.size();i++)
	{
		if (btRigidBody::upcast(m_allocatedRigidBodies[i])->isInWorld() == false) continue;
		CsGame::GetSingletonPtr()->GetDynamicsWorld()->removeRigidBody(btRigidBody::upcast(m_allocatedRigidBodies[i]));
	}

	for (i=0;i<m_allocatedCollisionShapes.size();i++)
	{
		delete m_allocatedCollisionShapes[i];
	}
	m_allocatedCollisionShapes.clear();

	for (i=0;i<m_allocatedRigidBodies.size();i++)
	{
		delete m_allocatedRigidBodies[i];
	}
	m_allocatedRigidBodies.clear();

	for (i=0;i<m_allocatedConstraints.size();i++)
	{
		delete m_allocatedConstraints[i];
	}
	m_allocatedConstraints.clear();

	for (i=0;i<m_allocatedBvhs.size();i++)
	{
		delete m_allocatedBvhs[i];
	}
	m_allocatedBvhs.clear();
	
	for (i=0;i<m_allocatedTriangleInfoMaps.size();i++)
	{
		delete m_allocatedTriangleInfoMaps[i];
	}
	m_allocatedTriangleInfoMaps.clear();
	for (i=0;i<m_allocatedTriangleIndexArrays.size();i++)
	{
		delete m_allocatedTriangleIndexArrays[i];
	}
	m_allocatedTriangleIndexArrays.clear();
	for (i=0;i<m_allocatedNames.size();i++)
	{
		delete m_allocatedNames[i];
	}
	m_allocatedNames.clear();

	delete mFileBuffer;
	mFileBuffer = NULL;
	mFileLength = 0;

}
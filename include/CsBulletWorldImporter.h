#pragma once

#include "btbulletworldimporter.h"
class CsBulletWorldImporter :
	public btBulletWorldImporter
{
public:
	CsBulletWorldImporter(btDynamicsWorld* world=0) : btBulletWorldImporter(world) 
	{
		mFileBuffer = NULL;
		mFileLength = 0;
	}
	~CsBulletWorldImporter(void);

	void deletePhysicsData ();
	void LoadFileIntoMemory(const char *filename);

protected:
	char* mFileBuffer;
	int mFileLength;
};


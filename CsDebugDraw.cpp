#include <precompiled.h>

#include <CsDebugDraw.h>

#include <OgreVector3.h>
#include <OgreColourValue.h>

#include <CsGame.h>
#include <CsCameraManager.h>

CsDebugDraw::CsDebugDraw(void)
{
	mDebugColors = NULL;
	mDebugPoints = NULL;
}


CsDebugDraw::~CsDebugDraw(void)
{

}

void CsDebugDraw::drawLine(const btVector3& from,const btVector3& to,const btVector3& color) {
	if (!mDebugPoints) return;
	if ( mDebugPoints->size() > mDebugPoints->max_size() - 100) return;
	// fill color array
	//mDebugColors->push_back(Ogre::ColourValue(color.x(), color.y(), color.z(), 1.0f));
	//Ogre::SceneNode* camNode = CsGame::GetSingletonPtr()->GetCameraManager()->GetCurrentCameraNode();
	//Ogre::Vector3 camPos = camNode->_getDerivedPosition();
	//Ogre::Vector3 ogreFrom(from.x(), from.y(), from.z());

	//if ((ogreFrom - camPos).length() < 100) return;

	if (mDebugPoints->size() > 20,000) return;

	mDebugPoints->push_back(Ogre::Vector3(from.x(), from.y(), from.z()));
	mDebugPoints->push_back(Ogre::Vector3(to.x(), to.y(), to.z()));


}


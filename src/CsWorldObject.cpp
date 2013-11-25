#include <precompiled.h>
#include "CsWorldObject.h"

CsWorldObject::CsWorldObject (Ogre::SceneNode* node) {
    SetNode(node);
}

void CsWorldObject::SetNode (Ogre::SceneNode* node) {
    mNode = node;
    //mPosition = mNode->getPosition();
    //mRotation = mNode->getOrientation();
}

void CsWorldObject::UpdateSceneNode () {
	if (!mNode) return;
	if (!mNeedsUpdate) return;

	// TODO: visibility test and set

    mNode->setPosition(mPosition);
    mNode->setOrientation(mRotation);

	mNeedsUpdate = false;
}

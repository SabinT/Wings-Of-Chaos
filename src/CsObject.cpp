#include <precompiled.h>
#include "CsObject.h"

CsObject::CsObject()
{
    mType = Chaos::NULL_OBJECT;
	mPosition = Ogre::Vector3(0,0,0);
	mRotation = Ogre::Quaternion();
}

CsObject::~CsObject()
{
    //dtor

}

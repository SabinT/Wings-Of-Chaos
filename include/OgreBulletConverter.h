#pragma once

#include <OgreVector3.h>
#include <OgreQuaternion.h>
#include <btVector3.h>
#include <btQuaternion.h>

inline Ogre::Vector3 BullettoOgre(btVector3 &bv) {
	return Ogre::Vector3(bv.x(), bv.y(), bv.z());
}

inline btVector3 OgretoBullet (Ogre::Vector3 &o) {
	return btVector3(o.x, o.y, o.z);
}

inline btVector3 OgretoBullet (const Ogre::Vector3 &o) {
	return btVector3(o.x, o.y, o.z);
}

inline Ogre::Quaternion BullettoOgre (btQuaternion &bq) {
	return Ogre::Quaternion(bq.w(), bq.x(), bq.y(), bq.z());
}

inline btQuaternion OgretoBullet (const Ogre::Quaternion &oq) {
	return btQuaternion(oq.x, oq.y, oq.z, oq.w);
}
#ifndef CSOBJECT_H
#define CSOBJECT_H

#include <OgreVector3.h>
#include <OgreQuaternion.h>

#include <CsCommons.h>

/// any object that can be placed in the world
namespace Chaos {
	enum CsObjectType {
		NULL_OBJECT = 0,
		PASSIVE_OBJECT = 1,
		DESTRUCTIBLE_OBJECT = 2,
		// custom queries for powerups
		POWERUP_OBJECT = 4,
		ENEMY_OBJECT = 8,
		SHIP_OBJECT = 16,
		// WEAPONs dont have collision shape
		WEAPON_OBJECT = 32,
		ENVIRONMENT_OBJECT = 64,
		EXPLOSION_OBJECT = 128
	};

	//to make collision enabled..two way confirmatory required
	enum CsDefaultCollisionMask {
		ALL_MASK = 0xFFFFFFFF,
		PASSIVE_MASK =	ALL_MASK ^ POWERUP_OBJECT,
		// TODO Remove environment from destructible mask
		DESTRUCTIBLE_MASK = ALL_MASK ^ POWERUP_OBJECT,
		// 
		POWERUP_MASK = SHIP_OBJECT,
		ENEMY_MASK = ALL_MASK ^ (ENEMY_OBJECT | POWERUP_OBJECT | ENVIRONMENT_OBJECT),
		SHIP_MASK = ALL_MASK,
		ENVIRONMENT_MASK = PASSIVE_OBJECT | SHIP_OBJECT | DESTRUCTIBLE_OBJECT | ENEMY_OBJECT
	};
};

class CsObject
{
public:
    CsObject();
    virtual ~CsObject();

    void SetPosition (Ogre::Vector3 position) {mPosition = position;}
    void SetPosition (float x, float y, float z) {
        mPosition.x = x;
        mPosition.y = y;
        mPosition.z = z;
    }
    virtual Ogre::Vector3 GetPositon() {return mPosition;}

    virtual void SetRotation (Ogre::Quaternion rotation) {mRotation = rotation;}
    virtual void SetRotation (float x, float y, float z, float w) {
        mRotation.x = x;
        mRotation.y = y;
        mRotation.z = z;
        mRotation.w = w;
    }

	inline void SetLinearVelocity (const btVector3& vel) {mLinearVelocity = vel;}
	inline void SetAngularVelocity (const btVector3& vel) {mAngularVelocity = vel;}
	inline btVector3 GetLinearVelocity() {return mLinearVelocity; }
	inline btVector3 GetAngularVelocity() {return mAngularVelocity; }
	
    virtual Ogre::Quaternion GetRotation() {return mRotation;}

	inline void SetType (int type) {mType = type;}
	inline int GetType () {return mType;}

protected:
    Ogre::Vector3 mPosition;
    Ogre::Quaternion mRotation;

	btVector3 mLinearVelocity;
	btVector3 mAngularVelocity;
	
	int mType;
};

//class CsDestructibleObject;

//class CsMovableObject;
// velocity force etc

#endif // CSOBJECT_H

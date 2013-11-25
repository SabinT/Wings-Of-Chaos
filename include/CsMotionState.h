#ifndef CSMOTIONSTATE_H
#define CSMOTIONSTATE_H

#include <BulletCommons.h>
#include <CsCommons.h>

#include <btTransform.h>

#include <LinearMath\btMotionState.h>
struct CsMotionState: public btMotionState {
	btTransform mWorldTrans;
	btTransform	mPhysicsTrans;
	btTransform mOffset;
    CsPhysicsWorldObject* mObj;

	// worldTrans = trans of SceneNode, physicsTrans = trans of rigid body (those two are skewed, so need to calculate offset)
	CsMotionState(CsPhysicsWorldObject* obj, const btTransform& worldTrans = btTransform::getIdentity(),const btTransform& physicsTrans = btTransform::getIdentity());

    void SetObject(CsPhysicsWorldObject* obj);

	virtual void getWorldTransform(btTransform& centerOfMassWorldTrans ) const;

    virtual void setWorldTransform(const btTransform &worldTrans);

	btTransform getConvertedTrans(const btTransform& centerOfMassWorldTrans) const;
};


#endif
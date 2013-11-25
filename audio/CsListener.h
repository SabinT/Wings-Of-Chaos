#pragma once

#include <al.h>
#include <alut.h>
#include <alc.h>


//ready for interfacing

class CsListener   {
public:
	CsListener(void);
	virtual ~CsListener(void);

	void SetVelocity( ALfloat *vel);
	void SetPosition( ALfloat *pos);
	void SetOrientation ( ALfloat *ori);

	void SetVelocity(btVector3 &vel);
	void SetVelocity(const btVector3 &vel);
	void Align(Ogre::Vector3 &pos, Ogre::Quaternion &ori);
	void UpdateListener();

protected:
	ALfloat mPosition[3];
	ALfloat mVelocity[3];
	ALfloat mOrientation[6];   //first three are AT next three UP
};


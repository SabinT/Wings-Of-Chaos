#include <precompiled.h>

#include <CsListener.h>


CsListener::CsListener(void)
{

	for (int i = 0; i < 3; i++) {
		mPosition[i] = 0.0f;
		mVelocity[i] = 0.0f;
		mOrientation[i] = 0.0f;
		mOrientation[i + 3] = 0.0f;
	}
	mOrientation[2] = -1.0f;
	mOrientation[4] = 1.0f;
}


CsListener::~CsListener(void)
{
}

void CsListener::SetVelocity(ALfloat *vel)  {
	if (!vel) return;
	for (int i = 0; i < 3; i++) 
		mVelocity[i] = vel[i];
}

void CsListener::SetPosition(ALfloat *pos)  {
	if (!pos) return;
	for (int i = 0; i < 3; i++) 
		mPosition[i] = pos[i];
}

void CsListener::SetOrientation(ALfloat *ori)  {
	if (!ori) return;
	for (int i = 0; i < 6; i++) 
		mOrientation[i] = ori[i];
}

void CsListener::SetVelocity(btVector3 &vel)  {
	mVelocity[0] = vel.x();
	mVelocity[1] = vel.y();
	mVelocity[2] = vel.z();
}

void CsListener::SetVelocity(const btVector3 &vel)  {
	mVelocity[0] = vel.x();
	mVelocity[1] = vel.y();
	mVelocity[2] = vel.z();
}

void CsListener::Align(Ogre::Vector3 &pos, Ogre::Quaternion &ori) {
	mPosition[0] = pos.x;
	mPosition[1] = pos.y;
	mPosition[2] = pos.z;
	Ogre::Vector3 nz = -ori.zAxis();
	Ogre::Vector3 py =  ori.yAxis();
	mOrientation[0] = nz.x + pos.x;
	mOrientation[1] = nz.y + pos.y;
	mOrientation[2] = nz.z + pos.z;
	mOrientation[3] = py.x;
	mOrientation[4] = py.y;
	mOrientation[5] = py.z;
}

void CsListener::UpdateListener()  {
	alListenerfv(AL_POSITION, mPosition);
    alListenerfv(AL_VELOCITY,  mVelocity);
    alListenerfv(AL_ORIENTATION, mOrientation);
}

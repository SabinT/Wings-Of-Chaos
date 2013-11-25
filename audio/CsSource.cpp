#include <precompiled.h>

#include <CsSource.h>


CsSource::CsSource(void)
{
	for (int i = 0; i < 3; i++) {
		mVelocity[i] = 0.;
		mPosition[i] = 0.;
	}
}


CsSource::~CsSource(void)
{
	UnloadSource();
}


void CsSource::LoadSource(GameSounds bufferno)  {
	//function to load the source with given buffer
	mSource = 0;
	alGenSources(1,&mSource);

	alSourcei (mSource, AL_BUFFER,(CsAudioManager::GetSingletonPtr()->GetBuffer())[bufferno]);
	if (alGetError() != AL_NO_ERROR) return;

    alSourcef (mSource, AL_PITCH,1.0f);
    alSourcef (mSource, AL_GAIN,1.0f);
    alSourcefv(mSource, AL_POSITION, mPosition);
    alSourcefv(mSource, AL_VELOCITY, mVelocity);

    alSourcei (mSource, AL_LOOPING,  AL_FALSE);
	CsAudioManager::GetSingletonPtr()->RegisterSource(this);
}

void CsSource::UnloadSource()  {
	if (mSource)
		alDeleteSources(1, &mSource);
}

void CsSource::SetGain(float gain) {
	alSourcef(mSource, AL_GAIN, gain);
}

void CsSource::SetVelocity(ALfloat *vel)  {
	if (!vel) return;
	for (int i = 0 ; i < 3; i++)  {
		mVelocity[i] = vel[i];
	}

}

void CsSource::SetPosition(const btVector3 &pos)  {
	mPosition[0] = pos.x();
	mPosition[1] = pos.y();
	mPosition[2] = pos.z();
}

void CsSource::SetPosition(const Ogre::Vector3 &pos)  {
	mPosition[0] = pos.x;
	mPosition[1] = pos.y;
	mPosition[2] = pos.z;
}

void CsSource::SetPosition(ALfloat *pos)  {
	if (!pos) return;
	for (int i = 0 ; i < 3; i++)  {
		mPosition[i] = pos[i];
	}
}

void CsSource::SetVelocity(const btVector3 &vel)  {
	mVelocity[0] = vel.x();
	mVelocity[1] = vel.y();
	mVelocity[2] = vel.z();
}

void CsSource::SetVelocity(const Ogre::Vector3 &vel)  {
	mVelocity[0] = vel.x;
	mVelocity[1] = vel.y;
	mVelocity[2] = vel.z;
}

void CsSource::UpdateSound() {
	alSourcefv(mSource, AL_POSITION, mPosition);
    alSourcefv(mSource, AL_VELOCITY, mVelocity);
//	alSourcef(mSource, AL_PITCH, p);
}

void CsSource::Play()  {
	alSourceStop(mSource);
	alSourcePlay(mSource);
}

void CsSource::Pause()  {
	alSourcePause(mSource);
}

void CsSource::Resume() {
	alSourcePlay(mSource);
}

void CsSource::Stop() {
	alSourceStop(mSource);
}

void CsSource::Loop(bool b) {
	alSourcei(mSource, AL_LOOPING, b);
}

ALint CsSource::GetState() {
	int a;
	alGetSourcei(mSource, AL_SOURCE_STATE, &a);
	return a;
}

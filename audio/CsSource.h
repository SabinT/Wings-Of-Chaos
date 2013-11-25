#pragma once

#include <al.h>
#include <alut.h>
#include <alc.h>
#include "CsAudioManager.h"

//ready for interfacing
class CsSource   {
public:
	//constructor and destructors
	//enum { PLAY = 0, PAUSE, STOP };
	CsSource(void);
	virtual ~CsSource(void);

	void LoadSource(GameSounds bufferNo);   //loads the source with the needed data
	void UnloadSource();
	void SetVelocity(ALfloat *vel);
	void SetPosition(ALfloat *pos);

	void SetPosition(const Ogre::Vector3 &pos);
	void SetPosition(const btVector3 &pos);
	void SetVelocity(const Ogre::Vector3 &vel);
	void SetVelocity(const btVector3 &vel);

	void SetGain(float gain);

	void UpdateSound();


	void Play();
	void Pause();
	void Resume();
	void Stop();
	void Loop(bool loop);

	ALint GetState();
	ALuint mSource;

protected:
	ALfloat mPosition[3];
	ALfloat mVelocity[3];
};


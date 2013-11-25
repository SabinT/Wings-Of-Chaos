#pragma once
#include <al.h>
#include <alc.h>
#include <alut.h>
#include <vector>

class CsSource;

enum GameSounds {
	BACKGROUND_MUSIC, JET_ENGINE, EXPLOSION, COLLISION, RICOCHET, FIRE_MISSILE,FIRE_MACHINEGUN,
	GAME_MUSIC, MENU_MUSIC,
	NUM_SOUNDS
};

class CsAudioManager    {
public:

	static void Create();
	static void Destroy();


	virtual ~CsAudioManager(void);

	static CsAudioManager* GetSingletonPtr() { if (pInstance) return pInstance; else return NULL;}

	//initialize the buffers
	bool Init(ALCchar *deviceName = "DirectSound3D");
	void LoadBuffer (ALbyte* fileName, int no);

	//obtain the buffer for sources
	inline ALuint* GetBuffer() { return Buffer; }

	//properly close the buffers
	void Cleanup();

	//the global functions
	void PauseAll();
	void StopAll();
	void ResumeAll();

	//to add the sources
	void RegisterSource ( CsSource *source);

protected:
	CsAudioManager(void);
	static CsAudioManager* pInstance;

	// Buffers hold sound data.
	ALuint Buffer[NUM_SOUNDS];
	//to store the sources
	std::vector<CsSource*> mSourceList;
};


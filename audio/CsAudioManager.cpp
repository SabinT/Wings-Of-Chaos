#include <precompiled.h>
#include <CsAudioManager.h>
#include <CsSource.h>


CsAudioManager* CsAudioManager::pInstance = NULL;

void CsAudioManager::Create() {
	if (!pInstance) pInstance = new CsAudioManager();
}

void CsAudioManager::Destroy() {
	if (pInstance) delete pInstance;
	pInstance = NULL;
}


CsAudioManager::CsAudioManager(void)
{
}


CsAudioManager::~CsAudioManager(void)
{
	//don't do anything here because of the Q command
}

void CsAudioManager::LoadBuffer(ALbyte* fileName, int no) {
    ALenum format;
    ALsizei size;
    ALvoid* data;
    ALsizei freq;
    ALboolean loop;
    // Load wav data into a buffer.
    if (alGetError() != AL_NO_ERROR)
        return;

    alutLoadWAVFile(fileName, &format, &data, &size, &freq, &loop);
    alBufferData(Buffer[no], format, data, size, freq);
    alutUnloadWAV(format, data, size, freq);
}

bool CsAudioManager::Init(ALCchar *deviceName)  {
	//initialize the audio library
    ALCdevice* pDevice;
    ALCcontext* pContext;
    const ALCchar* deviceSpecifier;
    // Get handle to device.
    pDevice = alcOpenDevice(deviceName);
    // Get the device specifier.
    deviceSpecifier = alcGetString(pDevice, ALC_DEVICE_SPECIFIER);
    // Create audio context.
    pContext = alcCreateContext(pDevice, NULL);

    // Set active context.
    alcMakeContextCurrent(pContext);

    // Check for an error.
    if (alcGetError(pDevice) != ALC_NO_ERROR)
        return false;

    alGenBuffers(NUM_SOUNDS, Buffer);

	LoadBuffer("../../sounds/0explosion.wav", EXPLOSION);
	LoadBuffer("../../sounds/0FIREMISSILE.wav",FIRE_MISSILE);
	LoadBuffer("../../sounds/0FIREMACHINEGUN.wav",FIRE_MACHINEGUN);
	LoadBuffer("../../sounds/0JETENGINE.wav", JET_ENGINE);
	LoadBuffer("../../sounds/0COLLISION.wav", COLLISION);
	LoadBuffer("../../sounds/0RICOCHET.wav", RICOCHET);
	LoadBuffer("../../sounds/game_music.wav", GAME_MUSIC);
	LoadBuffer("../../sounds/menu_music.wav", MENU_MUSIC);
	//LoadBuffer("wavdata/stereo.wav", MENU_SOUND);
	//LoadBuffer("wavdata/stereo.wav", EXPLOSION_SOUND);

	return true;
}

void CsAudioManager::Cleanup()  {

	//for (int i = 0; i < mSourceList.size(); i++) {
	//	mSourceList[i]->UnloadSource();
	//}
	mSourceList.clear();
    alDeleteBuffers(NUM_SOUNDS, Buffer);
    ALCcontext* pCurContext;
    ALCdevice* pCurDevice;

    // Get the current context.
    pCurContext = alcGetCurrentContext();

    // Get the device used by that context.
    pCurDevice = alcGetContextsDevice(pCurContext);

    // Reset the current context to NULL.
    alcMakeContextCurrent(0);

    // Release the context and the device.
    alcDestroyContext(pCurContext);
    alcCloseDevice(pCurDevice);

}

void CsAudioManager::RegisterSource ( CsSource *source) {
	mSourceList.push_back(source);
}


void CsAudioManager::PauseAll()  {
	for (int i = 0; i < mSourceList.size(); i++) 
		if (mSourceList[i]) mSourceList[i]->Pause();
}

void CsAudioManager::ResumeAll() {
	for (int i = 0; i < mSourceList.size(); i++) 
		if (mSourceList[i])   mSourceList[i]->Resume();
}


void CsAudioManager::StopAll()  {
	for (int i = 0; i < mSourceList.size(); i++)
		if (mSourceList[i])   mSourceList[i]->Stop();
}
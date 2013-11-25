#ifndef CSGAMEINTERFACE_H
#define CSGAMEINTERFACE_H

#include <OgrePrerequisites.h>

#include <CsCommons.h>

// TODO: separate input into separate class, which handles controls
#include <CsLockable.h>
class CsGameInterface: public CsLockable
{
public:
    CsGameInterface() : mRoot(0), mWindow(0), mViewport(0), mSceneMgr(0),
						mShutdown(false)
	{
		for (int i = 0; i < Chaos::NUM_BUTTONS; i++) mButtonState[i]= false;
	}

    virtual ~CsGameInterface();

    virtual int Run() = 0;

    virtual void InjectShutdown();
protected:
	//--------------------------------------------------------------------------------
	virtual bool Init() = 0;
	virtual void Cleanup() = 0;

	// input manager
	CsInputManager *mInputManager;
	bool mButtonState[Chaos::NUM_BUTTONS];

	CsLockableQueue<CsInputEvent> *mInputQueueLogic;
	CsLockableQueue<CsInputEvent> *mInputQueueRender;

	// Ogre root data
	Ogre::Root* mRoot;
	Ogre::RenderWindow* mWindow;
	Ogre::Viewport* mViewport;
	Ogre::SceneManager* mSceneMgr;
    Ogre::Camera* mDefaultCamera;

	// lock for game interface status variables provided by Lockable interface
	bool mShutdown;
};

#endif // CSGAMEINTERFACE_H

#pragma once

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

#include <OgrePrerequisites.h>

#include <CsLockableQueue.h>

// NOTE: all queue and event types defined in CsLockableQueue.h (including input events)

//------------------------------------------------------------------------------------------

class CsInputManager : public OIS::KeyListener, public OIS::MouseListener
{
public:
	CsInputManager(void);
	virtual ~CsInputManager(void);

	virtual void SetupInput(Ogre::RenderWindow *window, 
							CsLockableQueue<CsRenderMessage> *renderQueue = NULL,
							CsLockableQueue<CsLogicMessage> *logicQueue = NULL);

	virtual void SetupInput(Ogre::RenderWindow *window, OIS::MouseListener* mLis, OIS::KeyListener* kLis);

	virtual void DestroyInput();

	void CaptureInput () {
		mKeyboard->capture();
		mMouse->capture();
	}

	inline bool GetButtonState(Chaos::CsButtonId id) { return mButtonState[id]; }

	inline void DisableInputLogic (bool disable) { mDisableInputLogic = disable; }
	inline bool GetDisableInputLogic () { return mDisableInputLogic; }
	
protected:
    // OIS Input devices
    OIS::InputManager* mInputManager;
    OIS::Mouse* mMouse;
    OIS::Keyboard* mKeyboard;

	// on a different thread, but pass the temp queue to avoid locking
	CsLockableQueue<CsLogicMessage> *mLogicQueue;
	// on the same thread
	CsLockableQueue<CsRenderMessage> *mRenderQueue;

	bool mDisableInputLogic;

	// TODO: remove dependence on mButtonState
	bool mButtonState[Chaos::NUM_BUTTONS];

	// Input related ----------------------------------------------------------------------
    virtual bool keyPressed( const OIS::KeyEvent &arg );
    virtual bool keyReleased( const OIS::KeyEvent &arg );

    virtual bool mouseMoved( const OIS::MouseEvent &arg );
    virtual bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
    virtual bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
	//--------------------------------------------------------------------------------------

};


#ifndef CSMENU_H
#define CSMENU_H

#include <CsGameInterface.h>
#include <CsFader.h>

enum CsMenuResult {MENU_START_GAME, MENU_CREDITS, MENU_DEMO, MENU_EXIT};

class CsMenu: public CsGameInterface, OIS::KeyListener, OIS::MouseListener
{
public:
	CsMenu (	Ogre::Root* root,
				Ogre::RenderWindow* window,
				Ogre::Viewport* viewport,
				Ogre::SceneManager* sceneMgr)
	{
		mRoot = root; mWindow = window; mViewport = viewport; mSceneMgr = sceneMgr;
		mShutdown = false;
		mRunning = true;
		mFader = NULL;
	}
    virtual ~CsMenu();
    int Run();

protected:
	bool Init();
	void Cleanup ();
	
	// fade in/out a few overlays
	void ShowIntro();
	// OIS::KeyListener
    virtual bool keyPressed( const OIS::KeyEvent &arg );
    virtual bool keyReleased( const OIS::KeyEvent &arg );
    // OIS::MouseListener
    virtual bool mouseMoved( const OIS::MouseEvent &arg );
    virtual bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
    virtual bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );

	void FadeIn(const char* materialName, float duration, float wait, float sAlpha = 0.0);
	void FadeOut(const char* materialName, float duration, float wait, float sAlpha = 1.0);

	void DoRender();

	CsMenuResult mResult;
	bool mRunning;

	CsListener* mListener;
	CsCameraManager* mCameraManager;

	OgreMax::OgreMaxScene* mMenuScene;

	CsFader* mFader;
};

#endif // CSMENU_H

#ifndef CSBASEAPPLICATION_H
#define CSBASEAPPLICATION_H

// Ogre forward declarations
#include <OgrePrerequisites.h>

// Chaos forward declarations
#include <CsCommons.h>
#include <CEGUIForwardRefs.h>
#include <RendererModules/Ogre/CEGUIOgreRenderer.h>

#include <CsAudioManager.h>

#include <OgreWindowEventUtilities.h>
class CsBaseApplication: public Ogre::WindowEventListener
{
public:
    CsBaseApplication();
    virtual ~CsBaseApplication();

    bool Run();
protected:
    bool Init();
    void Cleanup();

	void CsBaseApplication::SetupResources(void);

    // window event listener, only for debug
    virtual void windowClosed(Ogre::RenderWindow* rw);
    bool mShutdown;

    CsGameInterface* mCurrentInterface;
    inline void ChangeInterface(CsGameInterface* newInterface);
    CsGame* mGame;
    CsMenu* mMenu;
    // credits demo etc

    //ogre initilization data
    Ogre::Root* mRoot;
    //Ogre::Camera* mDefaultCamera;
    Ogre::RenderWindow* mWindow;
    Ogre::SceneManager* mSceneMgr;
	Ogre::Viewport* mViewport;

	// Now the CEGUI portion
	// GUI created and destroyed only once during the program execution (destruction automatic not explicit)
	CEGUI::OgreRenderer* mRenderer;

	// SOUNDS
	CsAudioManager *mAudioManager;
};

#endif // CSBASEAPPLICATION_H

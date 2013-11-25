#include <precompiled.h>

#pragma comment(lib,"user32.lib")

#include "CsBaseApplication.h"
#include <CsMenu.h>
#include <CsGame.h>
#include <CsGameInterface.h>
#include <OgreConfigFile.h>

#include <CEGUI.h>


CsBaseApplication::CsBaseApplication()
  : mRoot(0),
    //mDefaultCamera(0),
    mSceneMgr(0),
    mWindow(0),
    mShutdown(false),
	mViewport(0),
    mCurrentInterface(0)
{ }

CsBaseApplication::~CsBaseApplication()
{
    //Ogre::LogManager::getSingletonPtr()->logMessage("*** CsApplication Destroyed ***");

    //dtor
}

void CsBaseApplication::ChangeInterface(CsGameInterface* newInterface) {
    mCurrentInterface = newInterface;
}

bool CsBaseApplication::Run() {
    if (!Init()) return false;

    while (!mShutdown) {
        mMenu = new CsMenu(mRoot, mWindow, mViewport, mSceneMgr);

        ChangeInterface(mMenu);
        int result = mMenu->Run();
        delete mMenu; mMenu = NULL;

        switch(result) {
            case MENU_START_GAME:
                CsGame::Create(mRoot, mWindow, mViewport, mSceneMgr);

				mGame = CsGame::GetSingletonPtr();
                ChangeInterface(mGame);

				assert(mGame);

                mGame->Run();

                CsGame::Destroy(); 
				mGame = NULL;
                break;
            case MENU_CREDITS:
            case MENU_DEMO:
                break;
            case MENU_EXIT:
                mShutdown = true;
                break;
            default:
                break;
        };
        Ogre::WindowEventUtilities::messagePump();

		//break;
		// for test purposes, run only once
    }

    Cleanup();

    return true;
}

bool CsBaseApplication::Init() {
	Ogre::String pluginsCfg;
#ifdef _DEBUG
    pluginsCfg = "plugins_d.cfg";
#else
    pluginsCfg = "plugins.cfg";
#endif

    //create ogre root
    mRoot = new Ogre::Root(pluginsCfg);

	SetupResources();
    Ogre::ResourceGroupManager *rgm = Ogre::ResourceGroupManager::getSingletonPtr();

    // set a render system
    Ogre::RenderSystem* rSys;
    rSys = mRoot->getRenderSystemByName("OpenGL Rendering Subsystem");
    mRoot->setRenderSystem(rSys);
    // end gracelessly if the preferred renderer is not available
    if (mRoot->getRenderSystem() == NULL) {
        delete mRoot;
        return false;
    }

    // create a window
    mRoot->initialise(false);
    mWindow = mRoot->createRenderWindow(
        "CsWindow",   // window name
        1366,                    // window width, in pixels
        768,                    // window height, in pixels
        false,                  // fullscreen or not
        0);                     // use defaults for all other values
    Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);

    // extra init
    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

	// Startup the GUI section *******************************************************
	try {
		mRenderer = &CEGUI::OgreRenderer::bootstrapSystem(*mRoot->getRenderTarget("CsWindow"));
	} catch ( CEGUI::Exception& e) {
		MessageBoxA( NULL, e.getMessage().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
		return false;
	}
	//CEGUI::OgreRenderer::bootstrapSystem();

	CEGUI::Imageset::setDefaultResourceGroup("Imagesets");
	CEGUI::Font::setDefaultResourceGroup("Fonts");
	CEGUI::Scheme::setDefaultResourceGroup("Schemes");
	CEGUI::WidgetLookManager::setDefaultResourceGroup("LookNFeel");
	CEGUI::WindowManager::setDefaultResourceGroup("Layouts");
	
	CEGUI::SchemeManager::getSingleton().create("TaharezLook.scheme");
	CEGUI::System::getSingleton().setDefaultMouseCursor("TaharezLook", "MouseArrow");
	// *******************************************************************************

	rgm->initialiseAllResourceGroups(); 
	// SceneManager and viewports created in the GameInterface component

	CsAudioManager::Create();
	mAudioManager = CsAudioManager::GetSingletonPtr();
	mAudioManager->Init();

    return true;
}

// window is destroyed after this function returns
void CsBaseApplication::windowClosed(Ogre::RenderWindow* rw) {
    mShutdown = true;
    mCurrentInterface->InjectShutdown();
}

void CsBaseApplication::Cleanup() {
    // ogre shutdown
    // unload plugins done by OGRE itself, do ont repeat!!!

	mAudioManager->Cleanup();
	CsAudioManager::Destroy();

    Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
    delete mRoot;
}

void CsBaseApplication::SetupResources(void)
{
	Ogre::String resourcesCfg;
#ifdef _DEBUG
    resourcesCfg = "resources_d.cfg";
#else
    resourcesCfg = "resources.cfg";
#endif
    // Load resource paths from config file
    Ogre::ConfigFile cf;
    cf.load(resourcesCfg);

    // Go through all sections & settings in the file
    Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

    Ogre::String secName, typeName, archName;
    while (seci.hasMoreElements())
    {
        secName = seci.peekNextKey();
        Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i)
        {
            typeName = i->first;
            archName = i->second;
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
                archName, typeName, secName);
        }
    }
}






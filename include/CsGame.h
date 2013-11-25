#ifndef CSGAME_H
#define CSGAME_H

#include <OgrePrerequisites.h>
#include <BulletCommons.h>
#include <CsCommons.h>

class boost::thread;
class boost::mutex;
namespace OgreMax {
	class OgreMaxScene;
};
class bParse::btBulletFile;

#include <vector>

#include <ctime>
#include <CsLockableQueue.h>

#include <CsGameGUI.h>
#include <CsGameLevel.h>

#include <CsGameInterface.h>
class CsGame: public CsGameInterface
{
public:
	enum GameState {
		PRE_GAME,	RUNNING,	POST_GAME
	};

	static void Create (	Ogre::Root* root,
					Ogre::RenderWindow* window,
					Ogre::Viewport* viewport,
					Ogre::SceneManager* sceneMgr);

	static void Destroy ();

	static CsGame* GetSingletonPtr() { if (pInstance) return pInstance; else return NULL;}

	//get functions for anywhere access
	Ogre::Root* GetRoot()  { return mRoot; }
	Ogre::RenderWindow* GetRenderWindow() { return mWindow; }
	Ogre::Viewport* GetViewport()  { return mViewport; }
	Ogre::SceneManager* GetSceneMgr()  { return mSceneMgr; }

	CsBulletManager* GetBulletManager() { return mBulletManager; }
	// implemented in .cpp file
	CsDiscreteDynamicsWorld* GetDynamicsWorld();

	inline CsPassiveObjectManager* GetPassiveObjectManager() {return mGameLevel->mPassiveObjectManager;}
	inline CsDestructibleObjectManager* GetDestructibleObjectManager() {return mGameLevel->mDestructibleObjectManager;}
	inline CsGroundEnemyManager * GetGroundEnemyManager () {return mGameLevel->mGroundEnemyManager; }
	inline CsShipManager* GetShipManager() {return mGameLevel->mShipManager;}
	inline CsShipObject* GetPlayerShip() {return mGameLevel->mPlayerShip;}

	inline CsProjectileManager* GetProjectileManager() {return mProjectileManager;}
	inline CsExplosionManager* GetExplosionManager() {return mExplosionManager;}

	inline CsGameLevel* GetGameLevel() {return mGameLevel;}

	inline CsCameraManager* GetCameraManager() {return mCameraManager;}
	inline CsTerrainManager* GetTerrainManager() {return mGameLevel->mTerrainManager;}
	inline CsGameGUI* GetGameGUI() {return mGameGUI;}

	// writing messages all done to the TEMP queue first, which is later dumped all at once into the actual queue
	CsLockableQueue<CsRenderMessage> *GetRenderTempQueue() {return &mRenderTempQueue;}
	CsLockableQueue<CsLogicMessage> *GetLogicTempQueue() {return &mLogicTempQueue;}

	CsLockableQueue<CsRenderMessage> *GetRenderQueue() {return &mRenderQueue;}
	CsLockableQueue<CsLogicMessage> *GetLogicQueue() {return &mLogicQueue;}

	//for test purposes only!!!! remove later
	//OgreMax::OgreMaxScene* GetOgreMaxSceneFile() { return mOgreMap; }

	inline float GetGameTime() { return (clock() - mPausedTime) / (float) CLOCKS_PER_SEC; }
	inline float GetDeltaTime() {return (mCurTime - mPrevTime)/(float)CLOCKS_PER_SEC; }

	// Run the game!!!
    int Run();

	bool mGameRunning;
	bool mGamePaused;
	GameState mGameState;

protected:
	CsGame (	Ogre::Root* root,
				Ogre::RenderWindow* window,
				Ogre::Viewport* viewport,
				Ogre::SceneManager* sceneMgr);

	CsGame (CsGame& c) {}
	void operator = (CsGame& c) {}

	~CsGame ();

	
	static CsGame* pInstance;

	// starts and cleans itself, never bothers you
	CsGameGUI *mGameGUI;

	//----------------------------------------------------------
	bool Init();
	void Cleanup();

	// thread objects
	void ThreadControl();
	void InputControl();

	boost::thread *mControlThread;
	//boost::thread *mRenderThread;
	boost::thread *mLogicThread;
	boost::thread *mInputThread;

	// subfunctions-------------------------------------------------
	boost::mutex mSyncMutex;

	CsLockableQueue<CsRenderMessage> mRenderQueue;
	CsLockableQueue<CsLogicMessage> mLogicQueue;

	// used for intermediate message storage to avoid excessive locking
	// NOTE: locking NOT done in the following queues
	CsLockableQueue<CsRenderMessage> mRenderTempQueue;	// logic thread writes to this
	CsLockableQueue<CsLogicMessage> mLogicTempQueue;	// render thread writes to this

	void Render();
	void ProcessRenderQueue();
	void UpdateSceneNodes();		// game data
	void UpdateGUI();
	// with hit points, ammo etc everything, target locked notices etc

	void Logic();	
	//void ProcessCollisions();
	void ProcessLogicQueue();

	// Input related (see CsGameInterface) --------------------------------------
	void ProcessInputLogic(CsInputEvent evt); // to be used by logic thread
	void ProcessInputRender(CsInputEvent evt); // to be used by render thread
	void ProcessGUILogic(CsGUIEvent evt);
	void ProcessGUIRender(CsGUIEvent evt);
	void ProcessGameLogic(CsGameEvent evt);
	void ProcessGameRender(CsGameEvent evt);

	// bullet section -----------------------------------------------------------
	CsBulletManager* mBulletManager;
	// --------------------------------------------------------------------------

	// scene related ------------------------------------------------------------
	void SetupScene(const char* levelName);
	void DestroyScene();

	// CameraManager object
	CsCameraManager* mCameraManager;

	CsGameLevel *mGameLevel;

	CsProjectileManager* mProjectileManager;
	CsExplosionManager* mExplosionManager;

	// timer section
	clock_t mPausedTime, mCurTime, mPrevTime;

	// test data
	friend class CsBulletManager;
};

#endif // CSGAME_H

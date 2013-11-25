#ifndef CSCOMMONS_H_INCLUDED
#define CSCOMMONS_H_INCLUDED

// TODO: management of pointers in each class, setting NULL to unused, initialization to NULL;

class CsBaseApplication;

class CsGame;
class CsMenu;
class CsGameInterface;

struct CsInputEvent;
class CsInputManager;

class CsLockable;
template <class T>
class CsLockableQueue;

struct CsRenderMessage;
struct CsLogicMessage;

class CsBulletManager;

class CsDiscreteDynamicsWorld;
class CsBulletWorldImporter;
class CsDebugDraw;
struct CsMotionState;
class CsRigidBody;

class CsAutoTracker;

class CsDestructible;
class CsDestructibleObject;
class CsEnemyObject;
class CsObject;
class CsPhysicsWorldObject;
class CsShipObject;
class CsWorldObject;

class CsCameraManager;
class CsDestructibleObjectManager;
class CsInputManager;
class CsPassiveObjectManager;

class CsGroundEnemy;
class CsGroundEnemyManager;

struct CsProjectile;
class CsProjectileManager;
class CsParticleManager;

struct CsExplosion;
class CsExplosionManager;

class CsGameLevel;

struct CsWeapon;
struct PrimaryWeapon;
struct SecondaryWeapon;
class CsWeaponManager;

class CsShipManager;

class CsTerrainManager;

struct CsObjective;
class CsObjectiveList;

// Interface part
class CsGameGUI;
class CsMenuGUI;

//Sound parts
class CsSource;
class CsListener;
class CsAudioManager;

// STL headers many already included in bullet and ogre

#endif // CSCOMMONS_H_INCLUDED

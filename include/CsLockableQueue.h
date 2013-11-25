#pragma once

#include <queue>
#include <CsLockable.h>

template <class T>
class CsLockableQueue : public CsLockable, public std::queue<T> {
public:
	CsLockableQueue () {}
	virtual ~CsLockableQueue() {}
};

// ------------------------------- Input Packets -------------------------
namespace Chaos {
	enum CsInputEventType {
		BUTTON_DOWN,
		BUTTON_UP,
		AXIS_MOVED,
		KEY_UP,
		KEY_DOWN
	};
	enum CsButtonId {
		FORWARD = 0, BACKWARD, LEFT, RIGHT, UP, DOWN, FIRE1, FIRE2,
		NEXTWEAPON, PREVWEAPON, LOCK_TARGET, SHOW_OBJECTIVES,
		ESCAPE, 
		ARROW_UP, ARROW_DOWN, ARROW_LEFT, ARROW_RIGHT, 
		DEBUG2,
		NUM_BUTTONS
	};
	enum CsAxisId {
		AXIS_X, AXIS_Y, AXIS_Z, AXIS_W
	};
};

struct CsInputEvent {
	int type;
	union {
		struct ButtonInfoType {
			int buttonId;
		} buttonInfo;
		OIS::KeyCode key;
		struct AxisInfoType {
			int axisId;
			int axisValue;
		} axisInfo;
	} info;
	// MAYBE THE ACTUAL MOUSE AND KEYBOARD DATA AS WELL
};

//namespace CsGUIEvent {
//	enum GUIEventTypes {
//		// In game Events
//		START_REQUEST = 1,
//		PAUSE_REQUEST,
//		RESUME_REQUEST,
//		DEBUG_TOGGLE_REQUEST,
//		QUIT_REQUEST
//	};
//};

struct CsGameEvent {
	enum GameEventType {
		OBJECT_DESTROYED,

		MISSION_COMPLETE,
		MISSION_FAIL,
	};
	
	GameEventType type;
	union {
		CsObjective* objective;
		CsDestructibleObject* destroyedObject;
	} info;
};
//-------------------------------- Render Messages ------------------------
// -------------- INFO PACKETS -------------------------------
struct BillboardInfo {
	Ogre::Billboard **bbPtr;
	Ogre::BillboardSet *bbSet;
};

struct NodeInfo {
	Ogre::SceneNode *parent, *child;
};

struct ObjectInfo {
	Ogre::SceneNode *parent;
	Ogre::MovableObject *obj;
};

struct ExplosionInfo {
	Ogre::SceneNode* node;
	float x,y,z;
	CsExplosion* explosion;
	float radius;
};

struct CsGUIEvent {
	enum GUIEventType {
		// In game Events
		START_REQUEST = 1,
		PAUSE_REQUEST,
		RESUME_REQUEST,
		CREATE_SHIP,
		SHIP_CAMERA_CHANGE,
		DEBUG_TOGGLE_REQUEST,
		QUIT_REQUEST,

		// only for Render
		DEBUG_PRINT,
		DEBUG_CLEAR
	};

	GUIEventType type;

	union {
		// careful with this!!
		char *str;
	} info;
};
//---------------------- MESSAGE PACKET ----------------------
struct CsRenderMessage {
	// message types
	enum RenderMessageTypes {
		INPUT_EVENT,
		GUI_EVENT,
		GAME_EVENT,
		CREATE_BILLBOARD,
		DESTROY_BILLBOARD,
		SHOW_NODE,
		HIDE_NODE,
		DETACH_NODE,
		ATTACH_NODE,
		DETACH_OBJECT,
		ATTACH_OBJECT,
		START_PARTICLE_FX,
		STOP_PARTICLE_FX,
		CREATE_PROJECTILE,
		DESTROY_PROJECTILE,
		CREATE_EXPLOSION,
		SHOW_DOT,
		HIDE_DOT
	};

	int type;
	union {
		//CsGUIEvent::GUIEventTypes guiEventInfo;
		CsInputEvent input;
		CsGameEvent gameEvent;
		CsGUIEvent guiEvent;

		BillboardInfo bbInfo;
		Ogre::SceneNode *sceneNode;
		NodeInfo nodeInfo;
		ObjectInfo objectInfo;
		Ogre::ParticleSystem *particleFX;
		CsProjectile* projectile;
		ExplosionInfo explosion;
		Ogre::OverlayContainer* dot;
		//CsExplosion* explosion;
	} info;
};

//-------------- now the  messages to Logic, including input, render to logic events, game events etc
//---------------------------------------------------------------------------------------------------

struct CsLogicMessage {
	// message types
	enum LogicMessageTypes {
		INPUT_EVENT,
		GUI_EVENT,
		GAME_EVENT
	};

	int type;
	union {
		//CsGUIEvent::GUIEventTypes guiEventInfo;
		CsInputEvent input;
		CsGameEvent gameEvent;
		CsGUIEvent guiEvent;
	} info;
};
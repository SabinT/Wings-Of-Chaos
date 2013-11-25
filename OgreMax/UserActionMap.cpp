/*
 * OgreMax Sample Viewer and Scene Loader - Ogre3D-based viewer and code for loading and displaying .scene files
 * Copyright 2010 AND Entertainment
 *
 * This code is available under the OgreMax Free License:
 *   -You may use this code for any purpose, commercial or non-commercial.
 *   -If distributing derived works (that use this source code) in binary or source code form, 
 *    you must give the following credit in your work's end-user documentation: 
 *        "Portions of this work provided by OgreMax (www.ogremax.com)"
 *
 * AND Entertainment assumes no responsibility for any harm caused by using this code.
 * 
 * The OgreMax Sample Viewer and Scene Loader were released at www.ogremax.com 
 */


//Includes---------------------------------------------------------------------
#include "UserActionMap.hpp"

using namespace Ogre;
using namespace OgreMax;


//Implementation---------------------------------------------------------------
UserActionMap::UserActionMap()
{
    this->mappings.reserve(50);
}

void UserActionMap::Initialize(const Ogre::String& fileName, InputSource::DeviceClass deviceClass)
{
    if (LoadSaveActionMap::LoadFile(fileName))
        SetPrivateDefaults(deviceClass);
    else
        SetDefaults(deviceClass);
}

void UserActionMap::SetDefaults(InputSource::DeviceClass deviceClass)
{
    //Keyboard
    if (deviceClass & InputSource::KEYBOARD)
    {
        AddActionMapping(InputSource::FromKey(OIS::KC_W), UserActionID::MOVE_FORWARD);
        AddActionMapping(InputSource::FromKey(OIS::KC_A), UserActionID::MOVE_LEFT);
        AddActionMapping(InputSource::FromKey(OIS::KC_D), UserActionID::MOVE_RIGHT);
        AddActionMapping(InputSource::FromKey(OIS::KC_S), UserActionID::MOVE_BACKWARD);

        AddActionMapping(InputSource::FromKey(OIS::KC_UP), UserActionID::MOVE_FORWARD);
        AddActionMapping(InputSource::FromKey(OIS::KC_LEFT), UserActionID::MOVE_LEFT);
        AddActionMapping(InputSource::FromKey(OIS::KC_RIGHT), UserActionID::MOVE_RIGHT);
        AddActionMapping(InputSource::FromKey(OIS::KC_DOWN), UserActionID::MOVE_BACKWARD);

        AddActionMapping(InputSource::FromKey(OIS::KC_PGUP), UserActionID::MOVE_UP);
        AddActionMapping(InputSource::FromKey(OIS::KC_PGDOWN), UserActionID::MOVE_DOWN);

        AddActionMapping(InputSource::FromKey(OIS::KC_SYSRQ), UserActionID::SCREENSHOT, PRESSED);

        AddActionMapping(InputSource::FromKey(OIS::KC_1), UserActionID::SET_CAMERA1, PRESSED);
        AddActionMapping(InputSource::FromKey(OIS::KC_2), UserActionID::SET_CAMERA2, PRESSED);
        AddActionMapping(InputSource::FromKey(OIS::KC_3), UserActionID::SET_CAMERA3, PRESSED);
        AddActionMapping(InputSource::FromKey(OIS::KC_4), UserActionID::SET_CAMERA4, PRESSED);
        AddActionMapping(InputSource::FromKey(OIS::KC_5), UserActionID::SET_CAMERA5, PRESSED);
        AddActionMapping(InputSource::FromKey(OIS::KC_6), UserActionID::SET_CAMERA6, PRESSED);
        AddActionMapping(InputSource::FromKey(OIS::KC_7), UserActionID::SET_CAMERA7, PRESSED);
        AddActionMapping(InputSource::FromKey(OIS::KC_8), UserActionID::SET_CAMERA8, PRESSED);
        AddActionMapping(InputSource::FromKey(OIS::KC_9), UserActionID::SET_CAMERA9, PRESSED);

        AddActionMapping(InputSource::FromKey(OIS::KC_SPACE), UserActionID::TOGGLE_VIEW_CAMERA_LOCK, PRESSED);
    }

    //Mouse
    if (deviceClass & InputSource::MOUSE)
    {
        SetMouseSensitivity(0, (Real).0025);
        SetMouseSensitivity(1, (Real).0025);
        SetMouseSensitivity(2, (Real).5);

        AddActionMapping(InputSource::FromMouseAxis(0, -1), UserActionID::LOOK_LEFT);
        AddActionMapping(InputSource::FromMouseAxis(0, 1), UserActionID::LOOK_RIGHT);
        AddActionMapping(InputSource::FromMouseAxis(1, -1), UserActionID::LOOK_UP);
        AddActionMapping(InputSource::FromMouseAxis(1, 1), UserActionID::LOOK_DOWN);
        AddActionMapping(InputSource::FromMouseAxis(2, 1), UserActionID::MOVE_UP);
        AddActionMapping(InputSource::FromMouseAxis(2, -1), UserActionID::MOVE_DOWN);

        AddActionMapping(InputSource::FromMouseButton(2), UserActionID::MOVE_FORWARD);
    }

    //Joystick
    if (deviceClass & InputSource::JOYSTICK)
    {
        SetJoystickAxisSensitivity(2, (Real).005);
        SetJoystickAxisSensitivity(3, (Real).005);

        AddActionMapping(InputSource::FromJoystickAxis(0, -1), UserActionID::MOVE_FORWARD);
        AddActionMapping(InputSource::FromJoystickAxis(0, 1), UserActionID::MOVE_BACKWARD);
        AddActionMapping(InputSource::FromJoystickAxis(1, -1), UserActionID::MOVE_LEFT);
        AddActionMapping(InputSource::FromJoystickAxis(1, 1), UserActionID::MOVE_RIGHT);
        AddActionMapping(InputSource::FromJoystickAxis(2, -1), UserActionID::LOOK_UP);
        AddActionMapping(InputSource::FromJoystickAxis(2, 1), UserActionID::LOOK_DOWN);
        AddActionMapping(InputSource::FromJoystickAxis(3, -1), UserActionID::LOOK_LEFT);
        AddActionMapping(InputSource::FromJoystickAxis(3, 1), UserActionID::LOOK_RIGHT);
        AddActionMapping(InputSource::FromJoystickAxis(4, 1), UserActionID::MOVE_DOWN);
        AddActionMapping(InputSource::FromJoystickAxis(5, 1), UserActionID::MOVE_UP);

        AddActionMapping(InputSource::FromJoystickButton(0), UserActionID::SCREENSHOT, PRESSED); //"Start" on the Xbox 360 controller
        AddActionMapping(InputSource::FromJoystickButton(4), UserActionID::TOGGLE_VIEW_CAMERA_LOCK, PRESSED);
        AddActionMapping(InputSource::FromJoystickButton(5), UserActionID::TOGGLE_VIEW_CAMERA_LOCK, PRESSED);

        AddActionMapping(InputSource::FromJoystickPov(0, OIS::Pov::West), UserActionID::PREVIOUS_CAMERA, PRESSED); //Left on Xbox 360 dpad
        AddActionMapping(InputSource::FromJoystickPov(0, OIS::Pov::East), UserActionID::NEXT_CAMERA, PRESSED); //Right on Xbox 360 dpad
    }

    SetPrivateDefaults(deviceClass);
}

void UserActionMap::SetPrivateDefaults(InputSource::DeviceClass deviceClass)
{
    //Keyboard
    if (deviceClass & InputSource::KEYBOARD)
        AddActionMapping(InputSource::FromKey(OIS::KC_ESCAPE), UserActionID::ESCAPE, PRESSED);

    //Joystick
    if (deviceClass & InputSource::JOYSTICK)
        AddActionMapping(InputSource::FromJoystickButton(1), UserActionID::EXIT, PRESSED); //"Back" on the Xbox 360 controller
}

void UserActionMap::AddEvent
    (
    UserEvents& events,
    UserActionID::Type actionID,
    InputSource::DeviceType deviceType,
    TriggerFlags trigger,
    Real amount
    )
{
    switch (actionID)
    {
        case UserActionID::MOVE_FORWARD:
            events.AddEvent(UserEventID::MOVE);
            events.move.y += amount;
            break;
        case UserActionID::MOVE_BACKWARD:
            events.AddEvent(UserEventID::MOVE);
            events.move.y -= amount;
            break;
        case UserActionID::MOVE_LEFT:
            events.AddEvent(UserEventID::MOVE);
            events.move.x -= amount;
            break;
        case UserActionID::MOVE_RIGHT:
            events.AddEvent(UserEventID::MOVE);
            events.move.x += amount;
            break;
        case UserActionID::MOVE_UP:
            events.AddEvent(UserEventID::MOVE);
            events.move.z += amount;
            break;
        case UserActionID::MOVE_DOWN:
            events.AddEvent(UserEventID::MOVE);
            events.move.z -= amount;
            break;
        case UserActionID::LOOK_UP:
            events.AddEvent(UserEventID::LOOK);
            events.look.y += amount;
            break;
        case UserActionID::LOOK_DOWN:
            events.AddEvent(UserEventID::LOOK);
            events.look.y -= amount;
            break;
        case UserActionID::LOOK_LEFT:
            events.AddEvent(UserEventID::LOOK);
            events.look.x -= amount;
            break;
        case UserActionID::LOOK_RIGHT:
            events.AddEvent(UserEventID::LOOK);
            events.look.x += amount;
            break;
        case UserActionID::PREVIOUS_CAMERA:
            events.AddEvent(UserEventID::CHANGE_CAMERA);
            events.cameraIndexDirection = -1;
            break;
        case UserActionID::NEXT_CAMERA:
            events.AddEvent(UserEventID::CHANGE_CAMERA);
            events.cameraIndexDirection = 1;
            break;
        case UserActionID::SET_CAMERA1:
        case UserActionID::SET_CAMERA2:
        case UserActionID::SET_CAMERA3:
        case UserActionID::SET_CAMERA4:
        case UserActionID::SET_CAMERA5:
        case UserActionID::SET_CAMERA6:
        case UserActionID::SET_CAMERA7:
        case UserActionID::SET_CAMERA8:
        case UserActionID::SET_CAMERA9:
            events.AddEvent(UserEventID::CHANGE_CAMERA);
            events.cameraIndex = (int)actionID - (int)UserActionID::SET_CAMERA1;
            break;
        case UserActionID::TOGGLE_VIEW_CAMERA_LOCK:
            events.AddEvent(UserEventID::TOGGLE_VIEW_CAMERA_LOCK);
            break;
        case UserActionID::SCREENSHOT: events.AddEvent(UserEventID::SCREENSHOT); break;
        case UserActionID::ESCAPE: events.AddEvent(UserEventID::ESCAPE); break;
        case UserActionID::EXIT: events.AddEvent(UserEventID::EXIT); break;
    }
}

const UserActionMap::ActionInfo* UserActionMap::GetActionInfo() const
{
    static const ActionInfo actionInfo[] =
    {
        ActionInfo("MoveForward", UserActionID::MOVE_FORWARD),
        ActionInfo("MoveBackward", UserActionID::MOVE_BACKWARD),
        ActionInfo("MoveLeft", UserActionID::MOVE_LEFT),
        ActionInfo("MoveRight", UserActionID::MOVE_RIGHT),
        ActionInfo("MoveUp", UserActionID::MOVE_UP),
        ActionInfo("MoveDown", UserActionID::MOVE_DOWN),
        ActionInfo("LookUp", UserActionID::LOOK_UP),
        ActionInfo("LookDown", UserActionID::LOOK_DOWN),
        ActionInfo("LookLeft", UserActionID::LOOK_LEFT),
        ActionInfo("LookRight", UserActionID::LOOK_RIGHT),
        ActionInfo("PreviousCamera", UserActionID::PREVIOUS_CAMERA),
        ActionInfo("NextCamera", UserActionID::NEXT_CAMERA),
        ActionInfo("SetCamera1", UserActionID::SET_CAMERA1),
        ActionInfo("SetCamera2", UserActionID::SET_CAMERA2),
        ActionInfo("SetCamera3", UserActionID::SET_CAMERA3),
        ActionInfo("SetCamera4", UserActionID::SET_CAMERA4),
        ActionInfo("SetCamera5", UserActionID::SET_CAMERA5),
        ActionInfo("SetCamera6", UserActionID::SET_CAMERA6),
        ActionInfo("SetCamera7", UserActionID::SET_CAMERA7),
        ActionInfo("SetCamera8", UserActionID::SET_CAMERA8),
        ActionInfo("SetCamera9", UserActionID::SET_CAMERA9),
        ActionInfo("ToggleViewCameraLock", UserActionID::TOGGLE_VIEW_CAMERA_LOCK),
        ActionInfo("Screenshot", UserActionID::SCREENSHOT),
        ActionInfo("Escape", UserActionID::ESCAPE, ActionInfo::PRIVATE),
        ActionInfo("Exit", UserActionID::EXIT, ActionInfo::PRIVATE),
        ActionInfo()
    };

    return actionInfo;
}

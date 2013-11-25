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
#include <OIS.h>
#include "InputSource.hpp"

using namespace OgreMax;
using namespace Ogre;


//Local variables--------------------------------------------------------------
static const String KeyboardKeyText = "KeyboardKey";
static const String MouseButtonText = "MouseButton";
static const String MouseAxisText = "MouseAxis";
static const String JoystickButtonText = "JoystickButton";
static const String JoystickAxisText = "JoystickAxis";
static const String JoystickPovText = "JoystickPov";


//Implementation---------------------------------------------------------------
InputSource::InputSource()
{
    Reset();
}

void InputSource::Reset()
{
    this->deviceType = NO_TYPE;
    this->button = 0;
    this->direction = 0;
    this->deviceIndex = 0;
}

InputSource InputSource::FromKey(OIS::KeyCode keyCode)
{
    InputSource source;
    source.deviceType = KEYBOARD_KEY;
    source.keyCode = keyCode;
    return source;
}

InputSource InputSource::FromMouseButton(int button)
{
    InputSource source;
    source.deviceType = MOUSE_BUTTON;
    source.button = button;
    return source;
}

InputSource InputSource::FromMouseAxis(int axis, int direction)
{
    InputSource source;
    source.deviceType = MOUSE_AXIS;
    source.axis = axis;
    source.direction = direction;
    return source;
}

InputSource InputSource::FromJoystickButton(int button, int joystickIndex)
{
    InputSource source;
    source.deviceType = JOYSTICK_BUTTON;
    source.deviceIndex = joystickIndex;
    source.button = button;
    return source;
}

InputSource InputSource::FromJoystickAxis(int axis, int direction, int joystickIndex)
{
    InputSource source;
    source.deviceType = JOYSTICK_AXIS;
    source.deviceIndex = joystickIndex;
    source.axis = axis;
    source.direction = direction;
    return source;
}

InputSource InputSource::FromJoystickPov(int povIndex, int direction, int joystickIndex)
{
    InputSource source;
    source.deviceType = JOYSTICK_POV;
    source.deviceIndex = joystickIndex;
    source.povIndex = povIndex;
    source.direction = direction;
    return source;
}

const String& InputSource::GetDeviceTypeString() const
{
    return DeviceTypeToString(this->deviceType);
}

const String& InputSource::DeviceTypeToString(DeviceType deviceType)
{
    switch (deviceType)
    {
        case KEYBOARD_KEY: return KeyboardKeyText;
        case MOUSE_BUTTON: return MouseButtonText;
        case MOUSE_AXIS: return MouseAxisText;
        case JOYSTICK_BUTTON: return JoystickButtonText;
        case JOYSTICK_AXIS: return JoystickAxisText;
        case JOYSTICK_POV: return JoystickPovText;
        default: return StringUtil::BLANK;
    }
}

const InputSource::DeviceType InputSource::DeviceTypeFromString(const String& deviceType)
{
    if (deviceType == KeyboardKeyText) return KEYBOARD_KEY;
    else if (deviceType == MouseButtonText) return MOUSE_BUTTON;
    else if (deviceType == MouseAxisText) return MOUSE_AXIS;
    else if (deviceType == JoystickButtonText) return JOYSTICK_BUTTON;
    else if (deviceType == JoystickAxisText) return JOYSTICK_AXIS;
    else if (deviceType == JoystickPovText) return JOYSTICK_POV;
    else return NO_TYPE;
}

InputSource::DeviceClass InputSource::GetDeviceClass() const
{
    switch (this->deviceType)
    {
        case KEYBOARD_KEY: return KEYBOARD;
        case MOUSE_BUTTON:
        case MOUSE_AXIS: return MOUSE;
        case JOYSTICK_BUTTON:
        case JOYSTICK_AXIS:
        case JOYSTICK_POV: return JOYSTICK;
        default: return NO_CLASS;
    }
}

InputSource::DeviceComponent InputSource::GetDeviceComponent() const
{
    switch (this->deviceType)
    {
        case KEYBOARD_KEY: return KEY;
        case MOUSE_BUTTON:
        case JOYSTICK_BUTTON: return BUTTON;
        case MOUSE_AXIS:
        case JOYSTICK_AXIS: return AXIS;
        case JOYSTICK_POV: return POV;
        default: return NO_COMPONENT;
    }
}

String InputSource::ToString() const
{
    StringUtil::StrStreamType stringBuilder;

    switch (this->deviceType)
    {
        case KEYBOARD_KEY: stringBuilder << "Device: Keyboard, Key: " << (int)this->keyCode; break;
        case MOUSE_BUTTON: stringBuilder << "Device: Mouse, Button: " << this->button; break;
        case JOYSTICK_BUTTON: stringBuilder << "Device: Joystick, Button: " << this->button; break;
        case MOUSE_AXIS: stringBuilder << "Device: Mouse, Axis: " << this->axis << ", Direction: " << this->direction; break;
        case JOYSTICK_AXIS: stringBuilder << "Device: Joystick " << this->deviceIndex << ", Axis: " << this->axis << ", Direction: " << this->direction; break;
        case JOYSTICK_POV: stringBuilder << "Device: Joystick " << this->deviceIndex << ", POV index: " << this->povIndex << ", Direction:" << this->direction; break;
        default: stringBuilder << "Device: None"; break;
    }

    return stringBuilder.str();
}

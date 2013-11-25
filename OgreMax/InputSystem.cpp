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
#include <OgreLogManager.h>
#include <OgreStringConverter.h>
#include "InputSystem.hpp"

using namespace OgreMax;
using namespace Ogre;


//Local classes----------------------------------------------------------------

/**
 * Checks devices for the "pressed" event
 */
class PressedDeviceChecker : public InputSystem::DeviceChecker
{
public:
    OIS::KeyCode GetKeyboardKey(const Keyboard& keyboard)
    {
        return keyboard.GetPressedKey();
    }

    int GetMouseButton(const Mouse& mouse)
    {
        return mouse.GetPressedButton();
    }

    bool GetMouseAxisIndexAndDirection(const Mouse& mouse, int& axisIndex, int& direction)
    {
        return mouse.GetPressedAxisIndexAndDirection(axisIndex, direction);
    }

    int GetJoystickButton(const Joystick& joystick)
    {
        return joystick.GetPressedButton();
    }

    bool GetJoystickAxisIndexAndDirection(const Joystick& joystick, int& axisIndex, int& direction)
    {
        return joystick.GetPressedAxisIndexAndDirection(axisIndex, direction);
    }

    bool GetJoystickPovIndexAndDirection(const Joystick& joystick, int& povIndex, int& direction)
    {
        return joystick.GetPressedPovIndexAndDirection(povIndex, direction);
    }

    void GetKeyboardKeys(InputSourceVector& inputSources, const Keyboard& keyboard)
    {
        keyboard.GetPressedKeys(inputSources);
    }

    void GetMouseButtons(InputSourceVector& inputSources, const Mouse& mouse)
    {
        mouse.GetPressedButtons(inputSources);
    }

    void GetMouseAxisIndicesAndDirections(InputSourceVector& inputSources, const Mouse& mouse)
    {
        mouse.GetPressedAxisIndicesAndDirections(inputSources);
    }

    void GetJoystickButtons(InputSourceVector& inputSources, const Joystick& joystick)
    {
        joystick.GetPressedButtons(inputSources);
    }

    void GetJoystickAxisIndicesAndDirections(InputSourceVector& inputSources, const Joystick& joystick)
    {
        joystick.GetPressedAxisIndicesAndDirections(inputSources);
    }

    void GetJoystickPovIndicesAndDirections(InputSourceVector& inputSources, const Joystick& joystick)
    {
        joystick.GetPressedPovIndicesAndDirections(inputSources);
    }

    static PressedDeviceChecker instance;
};

/**
 * Checks devices for the "pressing" event
 */
class PressingDeviceChecker : public InputSystem::DeviceChecker
{
public:
    OIS::KeyCode GetKeyboardKey(const Keyboard& keyboard)
    {
        return keyboard.GetPressingKey();
    }

    int GetMouseButton(const Mouse& mouse)
    {
        return mouse.GetPressingButton();
    }

    bool GetMouseAxisIndexAndDirection(const Mouse& mouse, int& axisIndex, int& direction)
    {
        return mouse.GetPressingAxisIndexAndDirection(axisIndex, direction);
    }

    int GetJoystickButton(const Joystick& joystick)
    {
        return joystick.GetPressingButton();
    }

    bool GetJoystickAxisIndexAndDirection(const Joystick& joystick, int& axisIndex, int& direction)
    {
        return joystick.GetPressingAxisIndexAndDirection(axisIndex, direction);
    }

    bool GetJoystickPovIndexAndDirection(const Joystick& joystick, int& povIndex, int& direction)
    {
        return joystick.GetPressingPovIndexAndDirection(povIndex, direction);
    }

    void GetKeyboardKeys(InputSourceVector& inputSources, const Keyboard& keyboard)
    {
        keyboard.GetPressingKeys(inputSources);
    }

    void GetMouseButtons(InputSourceVector& inputSources, const Mouse& mouse)
    {
        mouse.GetPressingButtons(inputSources);
    }

    void GetMouseAxisIndicesAndDirections(InputSourceVector& inputSources, const Mouse& mouse)
    {
        mouse.GetPressingAxisIndicesAndDirections(inputSources);
    }

    void GetJoystickButtons(InputSourceVector& inputSources, const Joystick& joystick)
    {
        joystick.GetPressingButtons(inputSources);
    }

    void GetJoystickAxisIndicesAndDirections(InputSourceVector& inputSources, const Joystick& joystick)
    {
        joystick.GetPressingAxisIndicesAndDirections(inputSources);
    }

    void GetJoystickPovIndicesAndDirections(InputSourceVector& inputSources, const Joystick& joystick)
    {
        joystick.GetPressingPovIndicesAndDirections(inputSources);
    }

    static PressingDeviceChecker instance;
};

/**
 * Checks devices for the "released" event
 */
class ReleasedDeviceChecker : public InputSystem::DeviceChecker
{
public:
    OIS::KeyCode GetKeyboardKey(const Keyboard& keyboard)
    {
        return keyboard.GetReleasedKey();
    }

    int GetMouseButton(const Mouse& mouse)
    {
        return mouse.GetReleasedButton();
    }

    bool GetMouseAxisIndexAndDirection(const Mouse& mouse, int& axisIndex, int& direction)
    {
        return mouse.GetReleasedAxisIndexAndDirection(axisIndex, direction);
    }

    int GetJoystickButton(const Joystick& joystick)
    {
        return joystick.GetReleasedButton();
    }

    bool GetJoystickAxisIndexAndDirection(const Joystick& joystick, int& axisIndex, int& direction)
    {
        return joystick.GetReleasedAxisIndexAndDirection(axisIndex, direction);
    }

    bool GetJoystickPovIndexAndDirection(const Joystick& joystick, int& povIndex, int& direction)
    {
        return joystick.GetReleasedPovIndexAndDirection(povIndex, direction);
    }

    void GetKeyboardKeys(InputSourceVector& inputSources, const Keyboard& keyboard)
    {
        keyboard.GetReleasedKeys(inputSources);
    }

    void GetMouseButtons(InputSourceVector& inputSources, const Mouse& mouse)
    {
        mouse.GetReleasedButtons(inputSources);
    }

    void GetMouseAxisIndicesAndDirections(InputSourceVector& inputSources, const Mouse& mouse)
    {
        mouse.GetReleasedAxisIndicesAndDirections(inputSources);
    }

    void GetJoystickButtons(InputSourceVector& inputSources, const Joystick& joystick)
    {
        joystick.GetReleasedButtons(inputSources);
    }

    void GetJoystickAxisIndicesAndDirections(InputSourceVector& inputSources, const Joystick& joystick)
    {
        joystick.GetReleasedAxisIndicesAndDirections(inputSources);
    }

    void GetJoystickPovIndicesAndDirections(InputSourceVector& inputSources, const Joystick& joystick)
    {
        joystick.GetReleasedPovIndicesAndDirections(inputSources);
    }

    static ReleasedDeviceChecker instance;
};


//Static initialization--------------------------------------------------------
InputSystem InputSystem::instance;
PressedDeviceChecker PressedDeviceChecker::instance;
PressingDeviceChecker PressingDeviceChecker::instance;
ReleasedDeviceChecker ReleasedDeviceChecker::instance;


//Implementation---------------------------------------------------------------
InputSystem::InputSystem()
{
    this->inputManager = 0;
    this->joystickCount = 0;
    this->updateID = 0;
}

void InputSystem::Initialize(size_t windowHandle, bool exclusive, InputSource::DeviceClass deviceClass)
{
    Initialize(windowHandle, deviceClass, !exclusive ? NONEXCLUSIVE : NO_FLAGS);
}

void InputSystem::Initialize(size_t windowHandle, InputSource::DeviceClass deviceClass, Flags flags)
{
    //Exit early if the input system is already initialized
    if (this->inputManager != 0)
        return;

    this->loggingEnabled = (flags & LOGGING_ENABLED) != NO_FLAGS;

    if (this->loggingEnabled)
        LogManager::getSingleton().logMessage("Initializing input system");

    //Initialize input manager parameters
    OIS::ParamList paramList;
    String windowKey("WINDOW");
    paramList.insert(std::make_pair(windowKey, Ogre::StringConverter::toString(windowHandle)));
    if ((deviceClass & InputSource::KEYBOARD) && (flags & NONEXCLUSIVE_KEYBOARD))
    {
        paramList.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND")));
        paramList.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE")));
    }
    if ((deviceClass & InputSource::MOUSE) && (flags & NONEXCLUSIVE_MOUSE))
    {
        paramList.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND")));
        paramList.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
    }
    if ((deviceClass & InputSource::JOYSTICK) && (flags & NONEXCLUSIVE_JOYSTICK))
    {
        paramList.insert(std::make_pair(std::string("w32_joystick"), std::string("DISCL_FOREGROUND")));
        paramList.insert(std::make_pair(std::string("w32_joystick"), std::string("DISCL_NONEXCLUSIVE")));
    }

    //Initialize input manager
    this->inputManager = OIS::InputManager::createInputSystem(paramList);

	//Initialize all devices
    if ((deviceClass & InputSource::KEYBOARD) && this->inputManager->getNumberOfDevices(OIS::OISKeyboard) > 0)
    {
        try
        {
            OIS::Keyboard* keyboardDevice = (OIS::Keyboard*)this->inputManager->createInputObject(OIS::OISKeyboard, false);
            this->Keyboard.Set(keyboardDevice);
        }
        catch (...)
        {
            if (this->loggingEnabled)
                LogManager::getSingleton().logMessage("Error initializing keyboard");
        }
    }

    if ((deviceClass & InputSource::MOUSE) && this->inputManager->getNumberOfDevices(OIS::OISMouse) > 0)
    {
        try
        {
	        OIS::Mouse* mouseDevice = (OIS::Mouse*)this->inputManager->createInputObject(OIS::OISMouse, false);
            this->Mouse.Set(mouseDevice);
        }
        catch (...)
        {
            if (this->loggingEnabled)
                LogManager::getSingleton().logMessage("Error initializing mouse");
        }
    }

    int joystickCount = this->inputManager->getNumberOfDevices(OIS::OISJoyStick);
    if ((deviceClass & InputSource::JOYSTICK) && joystickCount > 0)
    {
        try
        {
            if (joystickCount > MAX_JOYSTICKS)
                joystickCount = MAX_JOYSTICKS;

            for (int joystickIndex = 0; joystickIndex < joystickCount; joystickIndex++)
            {
		        OIS::JoyStick* joystickDevice = (OIS::JoyStick*)this->inputManager->createInputObject(OIS::OISJoyStick, false);
                this->joysticks[this->joystickCount++].Set(joystickDevice);
            }
	    }
        catch (...)
        {
            if (this->loggingEnabled)
                LogManager::getSingleton().logMessage("Error initializing joystick");
	    }
    }
}

void InputSystem::Shutdown()
{
    if (this->inputManager != 0)
	{
        if (!this->Keyboard.IsNull())
        {
		    this->inputManager->destroyInputObject(this->Keyboard.keyboard);
            this->Keyboard.Set(0);
        }

        if (!this->Mouse.IsNull())
        {
		    this->inputManager->destroyInputObject(this->Mouse.mouse);
            this->Mouse.Set(0);
        }

        for (size_t joystickIndex = 0; joystickIndex < this->joystickCount; joystickIndex++)
        {
            this->inputManager->destroyInputObject(this->joysticks[joystickIndex].joystick);
            this->joysticks[joystickIndex].Set(0);
        }
        this->joystickCount = 0;

		OIS::InputManager::destroyInputSystem(this->inputManager);
		this->inputManager = 0;
	}
}

bool InputSystem::PressedAny(InputSource::DeviceClass deviceClass, InputSource::DeviceComponent deviceComponent) const
{
    InputSource value;
    return GetAnyPressedInputSource(value, deviceClass, deviceComponent);
}

bool InputSystem::PressingAny(InputSource::DeviceClass deviceClass, InputSource::DeviceComponent deviceComponent) const
{
    InputSource value;
    return GetAnyPressingInputSource(value, deviceClass, deviceComponent);
}

bool InputSystem::ReleasedAny(InputSource::DeviceClass deviceClass, InputSource::DeviceComponent deviceComponent) const
{
    InputSource value;
    return GetAnyReleasedInputSource(value, deviceClass, deviceComponent);
}

bool InputSystem::GetAnyPressedInputSource(InputSource& inputSource, InputSource::DeviceClass deviceClass, InputSource::DeviceComponent deviceComponent) const
{
    return GetAnyActiveInputSource(PressedDeviceChecker::instance, inputSource, deviceClass, deviceComponent);
}

bool InputSystem::GetAnyPressingInputSource(InputSource& inputSource, InputSource::DeviceClass deviceClass, InputSource::DeviceComponent deviceComponent) const
{
    return GetAnyActiveInputSource(PressingDeviceChecker::instance, inputSource, deviceClass, deviceComponent);
}

bool InputSystem::GetAnyReleasedInputSource(InputSource& inputSource, InputSource::DeviceClass deviceClass, InputSource::DeviceComponent deviceComponent) const
{
    return GetAnyActiveInputSource(ReleasedDeviceChecker::instance, inputSource, deviceClass, deviceComponent);
}

bool InputSystem::GetAllPressedInputSources(InputSourceVector& inputSources, InputSource::DeviceClass deviceClass, InputSource::DeviceComponent deviceComponent) const
{
    return GetAllActiveInputSources(PressedDeviceChecker::instance, inputSources, deviceClass, deviceComponent);
}

bool InputSystem::GetAllPressingInputSources(InputSourceVector& inputSources, InputSource::DeviceClass deviceClass, InputSource::DeviceComponent deviceComponent) const
{
    return GetAllActiveInputSources(PressingDeviceChecker::instance, inputSources, deviceClass, deviceComponent);
}

bool InputSystem::GetAllReleasedInputSources(InputSourceVector& inputSources, InputSource::DeviceClass deviceClass, InputSource::DeviceComponent deviceComponent) const
{
    return GetAllActiveInputSources(ReleasedDeviceChecker::instance, inputSources, deviceClass, deviceComponent);
}

void InputSystem::Update()
{
    Update(0, true);
}

bool InputSystem::Update(size_t updateID, bool forceUpdate)
{
    bool result = false;

    //Guard against redundant updates
    if (forceUpdate || updateID != this->updateID)
    {
        //Store update ID for next call
        this->updateID = updateID;

        //Update input devices
        this->Keyboard.Update();
        this->Mouse.Update();
        for (size_t joystickIndex = 0; joystickIndex < this->joystickCount; joystickIndex++)
	        this->joysticks[joystickIndex].Update();

        result = true;
    }

    return result;
}

size_t InputSystem::GetJoystickCount() const
{
    return (int)this->joystickCount;
}

Joystick& InputSystem::GetJoystick(size_t index)
{
    if (index < (int)this->joystickCount)
        return this->joysticks[index];
    else
        return this->nullJoystick;
}

InputSystem& InputSystem::GetInstance()
{
    return instance;
}

bool InputSystem::GetAnyActiveInputSource
    (
    DeviceChecker& checker,
    InputSource& inputSource,
    InputSource::DeviceClass deviceClass,
    InputSource::DeviceComponent deviceComponent
    ) const
{
    //Clear output value
    inputSource.Reset();

    //Check keyboard
    if (inputSource.deviceType == InputSource::NO_TYPE &&
        ((deviceClass & InputSource::KEYBOARD) || (deviceComponent & InputSource::KEY)))
    {
        OIS::KeyCode key = checker.GetKeyboardKey(this->Keyboard);
        if (key != OIS::KC_UNASSIGNED)
        {
            inputSource.deviceType = InputSource::KEYBOARD_KEY;
            inputSource.keyCode = key;
        }
    }

    //Check mouse buttons
    if (inputSource.deviceType == InputSource::NO_TYPE &&
        ((deviceClass & InputSource::MOUSE) || (deviceComponent & InputSource::BUTTON)))
    {
        int buttonIndex = checker.GetMouseButton(this->Mouse);
        if (buttonIndex != -1)
        {
            inputSource.deviceType = InputSource::MOUSE_BUTTON;
            inputSource.button = buttonIndex;
        }
    }

    //Check mouse axes
    if (inputSource.deviceType == InputSource::NO_TYPE &&
        ((deviceClass & InputSource::MOUSE) || (deviceComponent & InputSource::AXIS)))
    {
        if (checker.GetMouseAxisIndexAndDirection(this->Mouse, inputSource.axis, inputSource.direction))
            inputSource.deviceType = InputSource::MOUSE_AXIS;
    }

    //Check joystick buttons
    if (inputSource.deviceType == InputSource::NO_TYPE &&
        ((deviceClass & InputSource::JOYSTICK) || (deviceComponent & InputSource::BUTTON)))
    {
        int buttonIndex;
        for (size_t joystickIndex = 0; joystickIndex < this->joystickCount; joystickIndex++)
        {
            buttonIndex = checker.GetJoystickButton(this->joysticks[joystickIndex]);
            if (buttonIndex != -1)
            {
                inputSource.deviceType = InputSource::JOYSTICK_BUTTON;
                inputSource.button = buttonIndex;
                inputSource.deviceIndex = (int)joystickIndex;
                break;
            }
        }
    }

    //Check joystick axes
    if (inputSource.deviceType == InputSource::NO_TYPE &&
        ((deviceClass & InputSource::JOYSTICK) || (deviceComponent & InputSource::AXIS)))
    {
        for (size_t joystickIndex = 0; joystickIndex < this->joystickCount; joystickIndex++)
        {
            if (checker.GetJoystickAxisIndexAndDirection(this->joysticks[joystickIndex], inputSource.axis, inputSource.direction))
            {
                inputSource.deviceType = InputSource::JOYSTICK_AXIS;
                inputSource.deviceIndex = (int)joystickIndex;
                break;
            }
        }
    }

    //Check joystick POV
    if (inputSource.deviceType == InputSource::NO_TYPE &&
        ((deviceClass & InputSource::JOYSTICK) || (deviceComponent & InputSource::POV)))
    {
        for (size_t joystickIndex = 0; joystickIndex < this->joystickCount; joystickIndex++)
        {
            if (checker.GetJoystickPovIndexAndDirection(this->joysticks[joystickIndex], inputSource.povIndex, inputSource.direction))
            {
                inputSource.deviceType = InputSource::JOYSTICK_POV;
                inputSource.deviceIndex = (int)joystickIndex;
                break;
            }
        }
    }

    return inputSource.deviceType != InputSource::NO_TYPE;
}

bool InputSystem::GetAllActiveInputSources
    (
    DeviceChecker& checker,
    InputSourceVector& inputSources,
    InputSource::DeviceClass deviceClass,
    InputSource::DeviceComponent deviceComponent
    ) const
{
    //Clear output value
    inputSources.clear();

    //Check keyboard
    if ((deviceClass & InputSource::KEYBOARD) || (deviceComponent & InputSource::KEY))
        checker.GetKeyboardKeys(inputSources, this->Keyboard);

    //Check mouse buttons
    if ((deviceClass & InputSource::MOUSE) || (deviceComponent & InputSource::BUTTON))
        checker.GetMouseButtons(inputSources, this->Mouse);

    //Check mouse axes
    if ((deviceClass & InputSource::MOUSE) || (deviceComponent & InputSource::AXIS))
        checker.GetMouseAxisIndicesAndDirections(inputSources, this->Mouse);

    //Check joystick buttons
    if ((deviceClass & InputSource::JOYSTICK) || (deviceComponent & InputSource::BUTTON))
    {
        for (size_t joystickIndex = 0; joystickIndex < this->joystickCount; joystickIndex++)
        {
            size_t inputSourceOffset = inputSources.size();
            checker.GetJoystickButtons(inputSources, this->joysticks[joystickIndex]);
            for (; inputSourceOffset < inputSources.size(); inputSourceOffset++)
                inputSources[inputSourceOffset].deviceIndex = (int)joystickIndex;
        }
    }

    //Check joystick axes
    if ((deviceClass & InputSource::JOYSTICK) || (deviceComponent & InputSource::AXIS))
    {
        for (size_t joystickIndex = 0; joystickIndex < this->joystickCount; joystickIndex++)
        {
            size_t inputSourceOffset = inputSources.size();
            checker.GetJoystickAxisIndicesAndDirections(inputSources, this->joysticks[joystickIndex]);
            for (; inputSourceOffset < inputSources.size(); inputSourceOffset++)
                inputSources[inputSourceOffset].deviceIndex = (int)joystickIndex;
        }
    }

    //Check joystick POV
    if ((deviceClass & InputSource::JOYSTICK) || (deviceComponent & InputSource::POV))
    {
        for (size_t joystickIndex = 0; joystickIndex < this->joystickCount; joystickIndex++)
        {
            size_t inputSourceOffset = inputSources.size();
            checker.GetJoystickPovIndicesAndDirections(inputSources, this->joysticks[joystickIndex]);
            for (; inputSourceOffset < inputSources.size(); inputSourceOffset++)
                inputSources[inputSourceOffset].deviceIndex = (int)joystickIndex;
        }
    }

    return !inputSources.empty();
}

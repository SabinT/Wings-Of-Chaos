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


#ifndef OgreMax_InputSystem_INCLUDED
#define OgreMax_InputSystem_INCLUDED


//Includes---------------------------------------------------------------------
#include "Keyboard.hpp"
#include "Mouse.hpp"
#include "Joystick.hpp"
#include "InputSource.hpp"


//Classes----------------------------------------------------------------------
namespace OgreMax
{
    /**
     * Manages all the input devices
     *
     * For the input devices to return the correct values, be sure to call Update()
     * at least once per frame.
     *
     * For all of the input-related classes there is the notion of "pressed", "pressing"
     * and "released" states. They are explained as follows:
     *   -Pressed: Anything that the user was not pressing in the previous update, but
     *             is pressing in the current update, has become "pressed". For example,
     *             the moment the user presses down on the space bar is when a "pressed"
     *             state occurs for the space bar. Holding down on the space bar does not
     *             represent a "pressed" state
     *   -Pressing: The "pressing" state does not take the previous state into account.
     *              So, regardless of whether the user has just pressed the space bar or
     *              they continue to press down on the space bar, the space bar is always
     *              in a "pressing" state
     *   -Released: Anything that the user was pressing in the previous update, but
     *              is not pressing in the current update, has become "released". For example,
     *              the moment the user lets up on the space bar is when a "released"
     *              state occurs for the space bar.
     */
    class InputSystem
    {
    public:
        /**
         * Initializes the input system
         * @param windowHandle [in] - Value containing a pointer to the native window handle
         * @param exclusive [in] - Indicates whether devices should be initialized in exclusive mode
         * This should only be true if the application is running in exclusive full screen mode
         * @param deviceClass [in] - Devices to initialize
         */
        void Initialize
            (
            size_t windowHandle,
            bool exclusive = true,
            InputSource::DeviceClass deviceClass = InputSource::ALL_DEVICE_CLASSES
            );

        typedef int Flags;
        enum
        {
            NO_FLAGS = 0,
            NONEXCLUSIVE_KEYBOARD = 0x1,
            NONEXCLUSIVE_MOUSE = 0x2,
            NONEXCLUSIVE_JOYSTICK = 0x4,
            NONEXCLUSIVE = NONEXCLUSIVE_KEYBOARD | NONEXCLUSIVE_MOUSE | NONEXCLUSIVE_JOYSTICK,
            LOGGING_ENABLED = 0x8
        };

        /**
         * Initializes the input system
         * @param windowHandle [in] - Value containing a pointer to the native window handle
         * @param deviceClass [in] - Devices to initialize
         * @param flags [in] - Flags indicating additional information about the initialization
         */
        void Initialize
            (
            size_t windowHandle,
            InputSource::DeviceClass deviceClass,
            Flags flags = NO_FLAGS
            );

        /** Shuts down the input system */
        void Shutdown();

        /**
         * Indicates whether any input device has become pressed.
         * This will be true when nothing was being pressed in the previous update
         * but at least device is pressed in the current update
         */
        bool PressedAny(InputSource::DeviceClass deviceClass, InputSource::DeviceComponent deviceComponent) const;

        /** Indicates whether any input device has is currently pressed */
        bool PressingAny(InputSource::DeviceClass deviceClass, InputSource::DeviceComponent deviceComponent) const;

        /**
         * Indicates whether any input device has become released.
         * This will be true when at least one device was being pressed in the previous update
         * but none are pressed in the current update
         */
        bool ReleasedAny(InputSource::DeviceClass deviceClass, InputSource::DeviceComponent deviceComponent) const;

        /**
         * Gets information about the input source that was pressed
         * This can be called in place of PressedAny()
         * @param inputSource [out] - The pressed input source. Only valid if the return value is true
         * @param deviceClass [in] - Flags indicating which device classes should be checked
         * @param deviceComponent [in] - Flags indicating which device components should be checked
         * @return Returns true if a value was retrieved, false otherwise
         */
        bool GetAnyPressedInputSource(InputSource& inputSource, InputSource::DeviceClass deviceClass, InputSource::DeviceComponent deviceComponent) const;

        /**
         * Gets information about the input source that is being pressed
         * This can be called in place of PressingAny()
         * @param inputSource [out] - The input source being pressed. Only valid if the return value is true
         * @param deviceClass [in] - Flags indicating which device classes should be checked
         * @param deviceComponent [in] - Flags indicating which device components should be checked
         * @return Returns true if a value was retrieved, false otherwise
         */
        bool GetAnyPressingInputSource(InputSource& inputSource, InputSource::DeviceClass deviceClass, InputSource::DeviceComponent deviceComponent) const;

        /**
         * Gets information about the input source that was released
         * This can be called in place of ReleasedAny()
         * @param inputSource [out] - The released input source. Only valid if the return value is true
         * @param deviceClass [in] - Flags indicating which device classes should be checked
         * @param deviceComponent [in] - Flags indicating which device components should be checked
         * @return Returns true if a value was retrieved, false otherwise
         */
        bool GetAnyReleasedInputSource(InputSource& inputSource, InputSource::DeviceClass deviceClass, InputSource::DeviceComponent deviceComponent) const;

        /**
         * Gets information about the input sources that were pressed
         * @param inputSource [out] - The pressed input sources. Only valid if the return value is true
         * @param deviceClass [in] - Flags indicating which device classes should be checked
         * @param deviceComponent [in] - Flags indicating which device components should be checked
         * @return Returns true if a value was retrieved, false otherwise
         */
        bool GetAllPressedInputSources(InputSourceVector& inputSources, InputSource::DeviceClass deviceClass, InputSource::DeviceComponent deviceComponent) const;

        /**
         * Gets information about the input sources that are being pressed
         * @param inputSource [out] - The input sources being pressed. Only valid if the return value is true
         * @param deviceClass [in] - Flags indicating which device classes should be checked
         * @param deviceComponent [in] - Flags indicating which device components should be checked
         * @return Returns true if a value was retrieved, false otherwise
         */
        bool GetAllPressingInputSources(InputSourceVector& inputSources, InputSource::DeviceClass deviceClass, InputSource::DeviceComponent deviceComponent) const;

        /**
         * Gets information about the input sources that were released
         * @param inputSources [out] - The released input sources. Only valid if the return value is true
         * @param deviceClass [in] - Flags indicating which device classes should be checked
         * @param deviceComponent [in] - Flags indicating which device components should be checked
         * @return Returns true if a value was retrieved, false otherwise
         */
        bool GetAllReleasedInputSources(InputSourceVector& inputSources, InputSource::DeviceClass deviceClass, InputSource::DeviceComponent deviceComponent) const;

        /**
         * Updates the state of all devices
         * Should be called at least once per game update
         */
        void Update();

        /**
         * Updates the state of all devices
         * Should be called at least once per game update
         * @param updateID [in] - The update identifier. This must be different
         * from a previous update identifier in order for the update to actually
         * occur. This is used to guard against redundant updates
         * @param forceUpdate [in] - Indicates whether the update should be
         * forced to occur, regardless of whether the updateID is the same
         * @return Returns true if the input system was updated - because the update ID
         * differed from the current ID or forceUpdate was true - otherwise, false is
         * returned
         */
        bool Update(size_t updateID, bool forceUpdate = false);

        /** Gets the number of joysticks */
        size_t GetJoystickCount() const;

        /**
         * Gets the joystick at the specified index.
         * A null joystick (it has no device) will be returned if the index is invalid
         */
        Joystick& GetJoystick(size_t index);

        /**
         * Gets the one and only instance of the InputSystem class
         */
        static InputSystem& GetInstance();

        /**
         * Checks for a class of events on the specified devices
         * For internal use only
         */
        class DeviceChecker
        {
        public:
            virtual ~DeviceChecker() {}

            virtual OIS::KeyCode GetKeyboardKey(const Keyboard& keyboard) = 0;
            virtual int GetMouseButton(const Mouse& mouse) = 0;
            virtual bool GetMouseAxisIndexAndDirection(const Mouse& mouse, int& axisIndex, int& direction) = 0;
            virtual int GetJoystickButton(const Joystick& joystick) = 0;
            virtual bool GetJoystickAxisIndexAndDirection(const Joystick& joystick, int& axisIndex, int& direction) = 0;
            virtual bool GetJoystickPovIndexAndDirection(const Joystick& joystick, int& povIndex, int& direction) = 0;

            virtual void GetKeyboardKeys(InputSourceVector& inputSources, const Keyboard& keyboard) = 0;
            virtual void GetMouseButtons(InputSourceVector& inputSources, const Mouse& mouse) = 0;
            virtual void GetMouseAxisIndicesAndDirections(InputSourceVector& inputSources, const Mouse& mouse) = 0;
            virtual void GetJoystickButtons(InputSourceVector& inputSources, const Joystick& joystick) = 0;
            virtual void GetJoystickAxisIndicesAndDirections(InputSourceVector& inputSources, const Joystick& joystick) = 0;
            virtual void GetJoystickPovIndicesAndDirections(InputSourceVector& inputSources, const Joystick& joystick) = 0;
        };

    private:
        /** Disable creation of new InputSystem instances */
        InputSystem();

        /**
         * Gets information about the active input source, as indicated by the device checker
         * @param checker [in] - Performs the logic of checking the devices
         * @param inputSource [out] - The active input source. Only valid if the return value is true
         * @param deviceClass [in] - Flags indicating which device classes should be checked
         * @param deviceComponent [in] - Flags indicating which device components should be checked
         * @return Returns true if a value was retrieved, false otherwise
         */
        bool GetAnyActiveInputSource
            (
            DeviceChecker& checker,
            InputSource& inputSource,
            InputSource::DeviceClass deviceClass,
            InputSource::DeviceComponent deviceComponent
            ) const;

        /**
         * Gets information about all the active input sources, as indicated by the device checker
         * @param checker [in] - Performs the logic of checking the devices
         * @param inputSources [out] - The active input sources. Only valid if the return value is true
         * @param deviceClass [in] - Flags indicating which device classes should be checked
         * @param deviceComponent [in] - Flags indicating which device components should be checked
         * @return Returns true if a value was retrieved, false otherwise
         */
        bool GetAllActiveInputSources
            (
            DeviceChecker& checker,
            InputSourceVector& inputSources,
            InputSource::DeviceClass deviceClass,
            InputSource::DeviceComponent deviceComponent
            ) const;

    public:
        /** Keyboard */
        Keyboard Keyboard;

        /** Mouse */
        Mouse Mouse;

    private:
        /** The underlying device manager */
        OIS::InputManager* inputManager;

        /** The maximum number of joysticks */
        enum {MAX_JOYSTICKS = 4};

        /** The number of joysticks */
        size_t joystickCount;

        /** The joysticks, one for each joystick */
        Joystick joysticks[MAX_JOYSTICKS];

        /** Returned by GetJoystick when the index is invalid */
        Joystick nullJoystick;

        /**
         * The last updateID passed to Update()
         * This is used to guard against redundant updates
         */
        size_t updateID;

        /** Indicates whether message logging is enabled */
        bool loggingEnabled;

        static InputSystem instance;
    };

}


#endif
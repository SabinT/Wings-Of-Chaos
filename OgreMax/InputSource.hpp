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


#ifndef OgreMax_InputSource_INCLUDED
#define OgreMax_InputSource_INCLUDED


//Includes---------------------------------------------------------------------
#include <OgreString.h>


//Classes----------------------------------------------------------------------
namespace OgreMax
{
    /**
     * A single component of an input device, such as a keyboard key or joystick button
     * You should not attempt to construct an instance of this class manually. Instead,
     * always use the "From" functions
     */
    class InputSource
    {
    public:
        /** Device types */
        enum DeviceType
        {
            NO_TYPE = 0,
            KEYBOARD_KEY,
            MOUSE_BUTTON,
            MOUSE_AXIS,
            JOYSTICK_BUTTON,
            JOYSTICK_AXIS,
            JOYSTICK_POV
        };

        /** Device classes. These can be bitwise OR'ed together */
        enum DeviceClass
        {
            NO_CLASS = 0,
            KEYBOARD = 0x1,
            MOUSE = 0x2,
            JOYSTICK = 0x4,
            ALL_DEVICE_CLASSES = KEYBOARD | MOUSE | JOYSTICK
        };

        /** Device components. These can be bitwise OR'ed together */
        enum DeviceComponent
        {
            NO_COMPONENT = 0,
            KEY = 0x8,
            BUTTON = 0x10,
            AXIS = 0x20,
            POV = 0x40,
            KEY_OR_BUTTON = KEY | BUTTON,
            POV_OR_AXIS = POV | AXIS,
            ALL_COMPONENT_CLASSES = KEY | BUTTON | AXIS | POV
        };

        InputSource();

        void Reset();

        static InputSource FromKey(OIS::KeyCode keyCode);
        static InputSource FromMouseButton(int button);
        static InputSource FromMouseAxis(int axis, int direction);
        static InputSource FromJoystickButton(int button, int joystickIndex = 0);
        static InputSource FromJoystickAxis(int axis, int direction, int joystickIndex = 0);
        static InputSource FromJoystickPov(int povIndex, int direction, int joystickIndex = 0);

        /** Converts the specified device type to a string */
        static const Ogre::String& DeviceTypeToString(DeviceType deviceType);

        /** Converts the specified string to a device type */
        static const DeviceType DeviceTypeFromString(const Ogre::String& deviceType);

        /** Gets the device type as a string */
        const Ogre::String& GetDeviceTypeString() const;

        /** Gets the device class stored in the device type flags */
        DeviceClass GetDeviceClass() const;

        /** Gets the device component type stored in the device type flags */
        DeviceComponent GetDeviceComponent() const;

        /** Gets a string containing the input source information. Useful for testing */
        Ogre::String ToString() const;

    public:
        /** Device type */
        DeviceType deviceType;

        /* The device component */
        union
        {
            /** Keyboard key */
            OIS::KeyCode keyCode;

            /** Mouse or joystick button */
            int button;

            /** Mouse or joystick axis */
            int axis;

            /** Joystick POV index */
            int povIndex;
        };

        /** Mouse axis direction, joystick axis direction, or joystick pov direction */
        int direction;

        /** Index of the device. This is only used for a joystick device */
        int deviceIndex;
    };

    class InputSourceVector
    {
    public:
        InputSourceVector()
        {
            this->count = 0;
        }

        enum {MAX_INPUT_SOURCES = 16};

        bool empty() const
        {
            return this->count == 0;
        }

        size_t size() const
        {
            return this->count;
        }

        void clear()
        {
            this->count = 0;
        }

        InputSource& operator [] (size_t i)
        {
            return this->sources[i];
        }

        const InputSource& operator [] (size_t i) const
        {
            return this->sources[i];
        }

        void push_back(const InputSource& source)
        {
            assert(this->count < MAX_INPUT_SOURCES - 1);
            if (this->count < MAX_INPUT_SOURCES - 1)
                this->sources[this->count++] = source;
        }

    private:
        InputSource sources[MAX_INPUT_SOURCES];
        size_t count;
    };
}


#endif
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


#ifndef OgreMax_Keyboard_INCLUDED
#define OgreMax_Keyboard_INCLUDED


//Includes---------------------------------------------------------------------
#include <OIS.h>
#include "InputSource.hpp"


//Classes----------------------------------------------------------------------
namespace OgreMax
{
    /**
     * Wrapper for a keyboard device
     * All public methods are safe to call with a null device
     */
    class Keyboard
    {
        friend class InputSystem;

    public:
        const OIS::Keyboard* GetKeyboard() const
        {
            return this->keyboard;
        }

        bool IsNull() const
        {
            return this->keyboard == 0;
        }

        bool PressedKey(OIS::KeyCode keyCode) const
        {
            return
                this->keyboard != 0 &&
                !this->previousKeys[(int)keyCode] &&
                this->keys[(int)keyCode];
        }

        bool PressingKey(OIS::KeyCode keyCode) const
        {
            return
                this->keyboard != 0 &&
                this->keys[(int)keyCode];
        }

        bool ReleasedKey(OIS::KeyCode keyCode) const
        {
            return
                this->keyboard != 0 &&
                this->previousKeys[(int)keyCode] &&
                !this->keys[(int)keyCode];
        }

        bool PressingAnyKey() const
        {
            return
                this->keyboard != 0 &&
                CheckPressingAnyKey(this->keys);
        }

        enum
        {
            ALT_MODIFIER = 0x1,
            CTRL_MODIFIER = 0x2,
            SHIFT_MODIFIER = 0x4
        };

        OIS::KeyCode GetPressedKey(int* modifierFlags = 0) const
        {
            OIS::KeyCode returnKeyCode = OIS::KC_UNASSIGNED;
            OIS::KeyCode keyCode;

            if (modifierFlags != 0)
            {
                *modifierFlags = 0;

                if (PressingKey(OIS::KC_LMENU) || PressingKey(OIS::KC_RMENU))
                    *modifierFlags |= ALT_MODIFIER;
                if (PressingKey(OIS::KC_LCONTROL) || PressingKey(OIS::KC_RCONTROL))
                    *modifierFlags |= CTRL_MODIFIER;
                if (PressingKey(OIS::KC_LSHIFT) || PressingKey(OIS::KC_RSHIFT))
                    *modifierFlags |= SHIFT_MODIFIER;

                for (int keyIndex = 0; keyIndex < KEY_COUNT; keyIndex++)
                {
                    keyCode = OIS::KeyCode(OIS::KC_UNASSIGNED + keyIndex);
                    if (PressedKey(keyCode) && !IsModifierKey(keyCode))
                    {
                        returnKeyCode = keyCode;
                        break;
                    }
                }

                return returnKeyCode;
            }
            else
            {
                for (int keyIndex = 0; keyIndex < KEY_COUNT; keyIndex++)
                {
                    keyCode = OIS::KeyCode(OIS::KC_UNASSIGNED + keyIndex);
                    if (PressedKey(keyCode))
                    {
                        returnKeyCode = keyCode;
                        break;
                    }
                }
            }

            return returnKeyCode;
        }

        OIS::KeyCode GetPressingKey(int* modifierFlags = 0) const
        {
            OIS::KeyCode returnKeyCode = OIS::KC_UNASSIGNED;
            OIS::KeyCode keyCode;

            if (modifierFlags != 0)
            {
                *modifierFlags = 0;

                if (PressingKey(OIS::KC_LMENU) || PressingKey(OIS::KC_RMENU))
                    *modifierFlags |= ALT_MODIFIER;
                if (PressingKey(OIS::KC_LCONTROL) || PressingKey(OIS::KC_RCONTROL))
                    *modifierFlags |= CTRL_MODIFIER;
                if (PressingKey(OIS::KC_LSHIFT) || PressingKey(OIS::KC_RSHIFT))
                    *modifierFlags |= SHIFT_MODIFIER;

                for (int keyIndex = 0; keyIndex < KEY_COUNT; keyIndex++)
                {
                    keyCode = OIS::KeyCode(OIS::KC_UNASSIGNED + keyIndex);
                    if (PressingKey(keyCode) && !IsModifierKey(keyCode))
                    {
                        returnKeyCode = keyCode;
                        break;
                    }
                }

                return returnKeyCode;
            }
            else
            {
                for (int keyIndex = 0; keyIndex < KEY_COUNT; keyIndex++)
                {
                    keyCode = OIS::KeyCode(OIS::KC_UNASSIGNED + keyIndex);
                    if (PressingKey(keyCode))
                    {
                        returnKeyCode = keyCode;
                        break;
                    }
                }
            }

            return returnKeyCode;
        }

        OIS::KeyCode GetReleasedKey() const
        {
            OIS::KeyCode returnKeyCode = OIS::KC_UNASSIGNED;

            OIS::KeyCode keyCode;
            for (int keyIndex = 0; keyIndex < KEY_COUNT; keyIndex++)
            {
                keyCode = OIS::KeyCode(OIS::KC_UNASSIGNED + keyIndex);
                if (ReleasedKey(keyCode))
                {
                    returnKeyCode = keyCode;
                    break;
                }
            }

            return returnKeyCode;
        }

        void GetPressedKeys(InputSourceVector& inputSources) const
        {
            InputSource inputSource;
            inputSource.deviceType = InputSource::KEYBOARD_KEY;
            for (int keyIndex = 0; keyIndex < KEY_COUNT; keyIndex++)
            {
                inputSource.keyCode = OIS::KeyCode(OIS::KC_UNASSIGNED + keyIndex);
                if (PressedKey(inputSource.keyCode))
                    inputSources.push_back(inputSource);
            }
        }

        void GetPressingKeys(InputSourceVector& inputSources) const
        {
            InputSource inputSource;
            inputSource.deviceType = InputSource::KEYBOARD_KEY;
            for (int keyIndex = 0; keyIndex < KEY_COUNT; keyIndex++)
            {
                inputSource.keyCode = OIS::KeyCode(OIS::KC_UNASSIGNED + keyIndex);
                if (PressingKey(inputSource.keyCode))
                    inputSources.push_back(inputSource);
            }
        }

        void GetReleasedKeys(InputSourceVector& inputSources) const
        {
            InputSource inputSource;
            inputSource.deviceType = InputSource::KEYBOARD_KEY;
            for (int keyIndex = 0; keyIndex < KEY_COUNT; keyIndex++)
            {
                inputSource.keyCode = OIS::KeyCode(OIS::KC_UNASSIGNED + keyIndex);
                if (ReleasedKey(inputSource.keyCode))
                    inputSources.push_back(inputSource);
            }
        }

        static bool IsAltKey(OIS::KeyCode keyCode)
        {
            return keyCode == OIS::KC_LMENU || keyCode == OIS::KC_RMENU;
        }

        static bool IsCtrlKey(OIS::KeyCode keyCode)
        {
            return keyCode == OIS::KC_LCONTROL || keyCode == OIS::KC_RCONTROL;
        }

        static bool IsShiftKey(OIS::KeyCode keyCode)
        {
            return keyCode == OIS::KC_LSHIFT || keyCode == OIS::KC_RSHIFT;
        }

        static bool IsModifierKey(OIS::KeyCode keyCode)
        {
            return IsAltKey(keyCode) || IsCtrlKey(keyCode) || IsShiftKey(keyCode);
        }

        static bool IsFunctionKey(OIS::KeyCode keyCode)
        {
            switch (keyCode)
            {
                case OIS::KC_F1:
                case OIS::KC_F2:
                case OIS::KC_F3:
                case OIS::KC_F4:
                case OIS::KC_F5:
                case OIS::KC_F6:
                case OIS::KC_F7:
                case OIS::KC_F8:
                case OIS::KC_F9:
                case OIS::KC_F10:
                case OIS::KC_F11:
                case OIS::KC_F12:
                case OIS::KC_F13:
                case OIS::KC_F14:
                case OIS::KC_F15: return true;
                default: return false;
            }
        }

        static bool IsNumberKey(OIS::KeyCode keyCode)
        {
            return ToNumber(keyCode) != -1;
        }

        static int ToNumber(OIS::KeyCode keyCode)
        {
            switch (keyCode)
            {
                case OIS::KC_0: return 0;
                case OIS::KC_1: return 1;
                case OIS::KC_2: return 2;
                case OIS::KC_3: return 3;
                case OIS::KC_4: return 4;
                case OIS::KC_5: return 5;
                case OIS::KC_6: return 6;
                case OIS::KC_7: return 7;
                case OIS::KC_8: return 8;
                case OIS::KC_9: return 9;
                case OIS::KC_NUMPAD0: return 0;
                case OIS::KC_NUMPAD1: return 1;
                case OIS::KC_NUMPAD2: return 2;
                case OIS::KC_NUMPAD3: return 3;
                case OIS::KC_NUMPAD4: return 4;
                case OIS::KC_NUMPAD5: return 5;
                case OIS::KC_NUMPAD6: return 6;
                case OIS::KC_NUMPAD7: return 7;
                case OIS::KC_NUMPAD8: return 8;
                case OIS::KC_NUMPAD9: return 9;
                default: return -1;
            }
        }

    private:
        Keyboard()
        {
            this->keyboard = 0;
            this->updatedOnce = false;

            memset(this->keyBuffers, 0, KEY_BUFFER_BYTE_SIZE);

            SetKeyBuffers(0);
        }

        void Set(OIS::Keyboard* keyboard)
        {
            this->keyboard = keyboard;
            this->updatedOnce = false;
        }

        void Update()
        {
            if (this->keyboard != 0)
            {
                //Switch buffers
                SetKeyBuffers(this->keyBufferIndex ^ 1);

                //Get the current keys
                this->keyboard->capture();
                this->keyboard->copyKeyStates(this->keys);

                //Copy the current keys to the previous if this is the first update
                if (!this->updatedOnce)
                {
                    memcpy(this->previousKeys, this->keys, KEY_COUNT);
                    this->updatedOnce = true;
                }
            }
        }

        void SetKeyBuffers(int currentKeyBufferIndex)
        {
            this->keyBufferIndex = currentKeyBufferIndex;
            this->keys = this->keyBuffers[this->keyBufferIndex];
            this->previousKeys = this->keyBuffers[this->keyBufferIndex ^ 1];
        }

        static bool CheckPressingAnyKey(const char* keys)
        {
            const long* longKeys = (long*)keys;
            const int longKeyCount = KEY_COUNT / sizeof(long);

            for (int i = 0; i < longKeyCount; i++)
            {
                if (longKeys[i] != 0)
                    return true;
            }

            return false;
        }

    private:
        OIS::Keyboard* keyboard;

        bool updatedOnce;

        enum {KEY_COUNT = 256};
        enum {KEY_BUFFER_BYTE_SIZE = 2 * KEY_COUNT};

        /** The key buffers. There are two: one for the current update, one for the previous update */
        char keyBuffers[2][KEY_COUNT];

        /** Index of the current buffer */
        int keyBufferIndex;

        /** Pointer to the key buffer containing the keys from the most recent update */
        char* keys;

        /** Pointer to the key buffer containing the keys from the previous update */
        char* previousKeys;
    };

}

#endif
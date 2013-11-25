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


#ifndef OgreMax_Joystick_INCLUDED
#define OgreMax_Joystick_INCLUDED


//Includes---------------------------------------------------------------------
#include <OIS.h>
#include "InputSource.hpp"


//Classes----------------------------------------------------------------------
namespace OgreMax
{
    /**
     * Wrapper for a joystick device
     * All public methods are safe to call with a null device
     */
    class Joystick
    {
        friend class InputSystem;

    public:
        enum {DEFAULT_DEAD_ZONE = 6000};

        const OIS::JoyStick* GetJoystick() const
        {
            return this->joystick;
        }

        bool IsNull() const
        {
            return this->joystick == 0;
        }

        bool PressedButton(int button) const
        {
            return
                this->joystick != 0 &&
                button < (int)this->previousJoystickState.mButtons.size() &&
                !this->previousJoystickState.mButtons[button] &&
                this->joystick->getJoyStickState().mButtons[button];
        }

        bool PressingButton(int button) const
        {
            return
                this->joystick != 0 &&
                button < (int)this->previousJoystickState.mButtons.size() &&
                this->joystick->getJoyStickState().mButtons[button];
        }

        bool ReleasedButton(int button) const
        {
            return
                this->joystick != 0 &&
                button < (int)this->previousJoystickState.mButtons.size() &&
                this->previousJoystickState.mButtons[button] &&
                !this->joystick->getJoyStickState().mButtons[button];
        }

        bool PressedAnyButton() const
        {
            for (int buttonIndex = 0; buttonIndex < BUTTON_COUNT; buttonIndex++)
            {
                if (PressedButton(buttonIndex))
                    return true;
            }
            return false;
        }

        bool PressingAnyButton() const
        {
            if (this->joystick != 0)
            {
                const OIS::JoyStickState& joystickState = this->joystick->getJoyStickState();
                for (int buttonIndex = 0; buttonIndex < (int)joystickState.mButtons.size(); buttonIndex++)
                {
                    if (joystickState.mButtons[buttonIndex])
                        return true;
                }
            }
            return false;
        }

        bool ReleasedAnyButton() const
        {
            for (int buttonIndex = 0; buttonIndex < BUTTON_COUNT; buttonIndex++)
            {
                if (ReleasedButton(buttonIndex))
                    return true;
            }
            return false;
        }

        bool PressedAxis(int axis, int direction, int deadZone) const
        {
            return
                this->joystick != 0 &&
                axis < (int)this->joystick->getJoyStickState().mAxes.size() &&
                !AxisActive(this->previousJoystickState.mAxes[axis].abs, direction, deadZone) &&
                AxisActive(this->joystick->getJoyStickState().mAxes[axis].abs, direction, deadZone);
        }

        bool PressingAxis(int axis, int direction, int deadZone) const
        {
            return
                this->joystick != 0 &&
                axis < (int)this->joystick->getJoyStickState().mAxes.size() &&
                AxisActive(this->joystick->getJoyStickState().mAxes[axis].abs, direction, deadZone);
        }

        bool ReleasedAxis(int axis, int direction, int deadZone) const
        {
            return
                this->joystick != 0 &&
                axis < (int)this->joystick->getJoyStickState().mAxes.size() &&
                AxisActive(this->previousJoystickState.mAxes[axis].abs, direction, deadZone) &&
                !AxisActive(this->joystick->getJoyStickState().mAxes[axis].abs, direction, deadZone);
        }

        bool PressedAnyAxis() const
        {
            if (this->joystick != 0)
            {
                const OIS::JoyStickState& joystickState = this->joystick->getJoyStickState();

                for (size_t axisIndex = 0; axisIndex < joystickState.mAxes.size(); axisIndex++)
                {
                    if (!AxisActive(joystickState.mAxes[axisIndex].abs, DEFAULT_DEAD_ZONE) &&
                        AxisActive(joystickState.mAxes[axisIndex].abs, DEFAULT_DEAD_ZONE))
                    {
                        return true;
                    }
                }
            }

            return false;
        }

        bool PressingAnyAxis() const
        {
            if (this->joystick != 0)
            {
                const OIS::JoyStickState& joystickState = this->joystick->getJoyStickState();

                for (size_t axisIndex = 0; axisIndex < joystickState.mAxes.size(); axisIndex++)
                {
                    if (AxisActive(joystickState.mAxes[axisIndex].abs, DEFAULT_DEAD_ZONE))
                        return true;
                }
            }

            return false;
        }

        bool ReleasedAnyAxis() const
        {
            if (this->joystick != 0)
            {
                const OIS::JoyStickState& joystickState = this->joystick->getJoyStickState();

                for (size_t axisIndex = 0; axisIndex < joystickState.mAxes.size(); axisIndex++)
                {
                    if (AxisActive(joystickState.mAxes[axisIndex].abs, DEFAULT_DEAD_ZONE) &&
                        !AxisActive(joystickState.mAxes[axisIndex].abs, DEFAULT_DEAD_ZONE))
                    {
                        return true;
                    }
                }
            }

            return false;
        }

        bool PressedPov(int povIndex, int povDirection, bool strict = false) const
        {
            bool result = false;
            if (this->joystick != 0 && povIndex < POV_COUNT)
            {
                if (strict)
                {
                    result =
                        this->previousJoystickState.mPOV[povIndex].direction != povDirection &&
                        this->joystick->getJoyStickState().mPOV[povIndex].direction == povDirection;
                }
                else
                {
                    result =
                        !(this->previousJoystickState.mPOV[povIndex].direction & povDirection) &&
                        this->joystick->getJoyStickState().mPOV[povIndex].direction & povDirection;
                }
            }
            return result;
        }

        bool PressingPov(int povIndex, int povDirection, bool strict = false) const
        {
            bool result = false;
            if (this->joystick != 0 && povIndex < POV_COUNT)
            {
                if (strict)
                    result = this->joystick->getJoyStickState().mPOV[povIndex].direction == povDirection;
                else
                    result = (this->joystick->getJoyStickState().mPOV[povIndex].direction & povDirection) != 0;
            }
            return result;
        }

        bool ReleasedPov(int povIndex, int povDirection, bool strict = false) const
        {
            bool result = false;
            if (this->joystick != 0 && povIndex < POV_COUNT)
            {
                if (strict)
                {
                    result =
                        this->previousJoystickState.mPOV[povIndex].direction == povDirection &&
                        this->joystick->getJoyStickState().mPOV[povIndex].direction != povDirection;
                }
                else
                {
                    result =
                        (this->previousJoystickState.mPOV[povIndex].direction & povDirection) &&
                        !(this->joystick->getJoyStickState().mPOV[povIndex].direction & povDirection);
                }
            }
            return result;
        }

        bool PressedAnyPov() const
        {
            if (this->joystick != 0)
            {
                const OIS::JoyStickState& joystickState = this->joystick->getJoyStickState();

                for (size_t povIndex = 0; povIndex < POV_COUNT; povIndex++)
                {
                    if ((this->previousJoystickState.mPOV[povIndex].direction == 0 || this->previousJoystickState.mPOV[povIndex].direction != joystickState.mPOV[povIndex].direction) &&
                        joystickState.mPOV[povIndex].direction != 0)
                    {
                        return true;
                    }
                }
            }

            return false;
        }

        bool PressingAnyPov() const
        {
            if (this->joystick != 0)
            {
                const OIS::JoyStickState& joystickState = this->joystick->getJoyStickState();

                for (size_t povIndex = 0; povIndex < POV_COUNT; povIndex++)
                {
                    if (joystickState.mPOV[povIndex].direction != 0)
                        return true;
                }
            }

            return false;
        }

        bool ReleasedAnyPov() const
        {
            if (this->joystick != 0)
            {
                const OIS::JoyStickState& joystickState = this->joystick->getJoyStickState();

                for (size_t povIndex = 0; povIndex < POV_COUNT; povIndex++)
                {
                    if ((joystickState.mPOV[povIndex].direction == 0 || this->previousJoystickState.mPOV[povIndex].direction != joystickState.mPOV[povIndex].direction) &&
                        this->previousJoystickState.mPOV[povIndex].direction != 0)
                    {
                        return true;
                    }
                }
            }

            return false;
        }

        int GetPressedButton() const
        {
            for (int buttonIndex = 0; buttonIndex < BUTTON_COUNT; buttonIndex++)
            {
                if (PressedButton(buttonIndex))
                    return buttonIndex;
            }

            return -1;
        }

        int GetPressingButton() const
        {
            for (int buttonIndex = 0; buttonIndex < BUTTON_COUNT; buttonIndex++)
            {
                if (PressingButton(buttonIndex))
                    return buttonIndex;
            }

            return -1;
        }

        int GetReleasedButton() const
        {
            for (int buttonIndex = 0; buttonIndex < BUTTON_COUNT; buttonIndex++)
            {
                if (ReleasedButton(buttonIndex))
                    return buttonIndex;
            }

            return -1;
        }

        bool GetPressedAxisIndexAndDirection(int& index, int& direction) const
        {
            if (this->joystick != 0)
            {
                const OIS::JoyStickState& joystickState = this->joystick->getJoyStickState();

                for (int axisIndex = 0; axisIndex < (int)joystickState.mAxes.size(); axisIndex++)
                {
                    if (PressedAxis(axisIndex, 0, Joystick::DEFAULT_DEAD_ZONE))
                    {
                        index = axisIndex;
                        direction = (joystickState.mAxes[axisIndex].abs < 0) ? -1 : 1;
                        return true;
                    }
                }
            }

            return false;
        }

        bool GetPressingAxisIndexAndDirection(int& index, int& direction) const
        {
            if (this->joystick != 0)
            {
                const OIS::JoyStickState& joystickState = this->joystick->getJoyStickState();

                for (int axisIndex = 0; axisIndex < (int)joystickState.mAxes.size(); axisIndex++)
                {
                    if (PressingAxis(axisIndex, 0, Joystick::DEFAULT_DEAD_ZONE))
                    {
                        index = axisIndex;
                        direction = (joystickState.mAxes[axisIndex].abs < 0) ? -1 : 1;
                        return true;
                    }
                }
            }

            return false;
        }

        bool GetReleasedAxisIndexAndDirection(int& index, int& direction) const
        {
            if (this->joystick != 0)
            {
                const OIS::JoyStickState& joystickState = this->joystick->getJoyStickState();

                for (int axisIndex = 0; axisIndex < (int)joystickState.mAxes.size(); axisIndex++)
                {
                    if (ReleasedAxis(axisIndex, 0, Joystick::DEFAULT_DEAD_ZONE))
                    {
                        index = axisIndex;
                        direction = (joystickState.mAxes[axisIndex].abs < 0) ? -1 : 1;
                        return true;
                    }
                }
            }

            return false;
        }

        bool GetPressedPovIndexAndDirection(int& index, int& direction) const
        {
            if (this->joystick != 0)
            {
                const OIS::JoyStickState& joystickState = this->joystick->getJoyStickState();

                for (int povIndex = 0; povIndex < POV_COUNT; povIndex++)
                {
                    if ((this->previousJoystickState.mPOV[povIndex].direction == 0 || this->previousJoystickState.mPOV[povIndex].direction != joystickState.mPOV[povIndex].direction) &&
                        joystickState.mPOV[povIndex].direction != 0)
                    {
                        index = povIndex;
                        direction = joystickState.mPOV[povIndex].direction;
                        return true;
                    }
                }
            }

            return false;
        }

        bool GetPressingPovIndexAndDirection(int& index, int& direction) const
        {
            if (this->joystick != 0)
            {
                const OIS::JoyStickState& joystickState = this->joystick->getJoyStickState();

                for (int povIndex = 0; povIndex < POV_COUNT; povIndex++)
                {
                    if (joystickState.mPOV[povIndex].direction != 0)
                    {
                        index = povIndex;
                        direction = joystickState.mPOV[povIndex].direction;
                        return true;
                    }
                }
            }

            return false;
        }

        bool GetReleasedPovIndexAndDirection(int& index, int& direction) const
        {
            if (this->joystick != 0)
            {
                const OIS::JoyStickState& joystickState = this->joystick->getJoyStickState();

                for (int povIndex = 0; povIndex < POV_COUNT; povIndex++)
                {
                    if ((joystickState.mPOV[povIndex].direction == 0 || this->previousJoystickState.mPOV[povIndex].direction != joystickState.mPOV[povIndex].direction) &&
                        this->previousJoystickState.mPOV[povIndex].direction != 0)
                    {
                        index = povIndex;
                        direction = joystickState.mPOV[povIndex].direction;
                        return true;
                    }
                }
            }

            return false;
        }

        void GetPressedButtons(InputSourceVector& inputSources) const
        {
            InputSource inputSource;
            inputSource.deviceType = InputSource::JOYSTICK_BUTTON;
            for (inputSource.button = 0; inputSource.button < BUTTON_COUNT; inputSource.button++)
            {
                if (PressedButton(inputSource.button))
                    inputSources.push_back(inputSource);
            }
        }

        void GetPressingButtons(InputSourceVector& inputSources) const
        {
            InputSource inputSource;
            inputSource.deviceType = InputSource::JOYSTICK_BUTTON;
            for (inputSource.button = 0; inputSource.button < BUTTON_COUNT; inputSource.button++)
            {
                if (PressingButton(inputSource.button))
                    inputSources.push_back(inputSource);
            }
        }

        void GetReleasedButtons(InputSourceVector& inputSources) const
        {
            InputSource inputSource;
            inputSource.deviceType = InputSource::JOYSTICK_BUTTON;
            for (inputSource.button = 0; inputSource.button < BUTTON_COUNT; inputSource.button++)
            {
                if (ReleasedButton(inputSource.button))
                    inputSources.push_back(inputSource);
            }
        }

        void GetPressedAxisIndicesAndDirections(InputSourceVector& inputSources) const
        {
            if (this->joystick != 0)
            {
                const OIS::JoyStickState& joystickState = this->joystick->getJoyStickState();
                InputSource inputSource;
                inputSource.deviceType = InputSource::JOYSTICK_AXIS;
                for (inputSource.axis = 0; inputSource.axis < (int)joystickState.mAxes.size(); inputSource.axis++)
                {
                    if (PressedAxis(inputSource.axis, 0, Joystick::DEFAULT_DEAD_ZONE))
                    {
                        inputSource.direction = (joystickState.mAxes[inputSource.axis].abs < 0) ? -1 : 1;
                        inputSources.push_back(inputSource);
                    }
                }
            }
        }

        void GetPressingAxisIndicesAndDirections(InputSourceVector& inputSources) const
        {
            if (this->joystick != 0)
            {
                const OIS::JoyStickState& joystickState = this->joystick->getJoyStickState();
                InputSource inputSource;
                inputSource.deviceType = InputSource::JOYSTICK_AXIS;
                for (inputSource.axis = 0; inputSource.axis < (int)joystickState.mAxes.size(); inputSource.axis++)
                {
                    if (PressingAxis(inputSource.axis, 0, Joystick::DEFAULT_DEAD_ZONE))
                    {
                        inputSource.direction = (joystickState.mAxes[inputSource.axis].abs < 0) ? -1 : 1;
                        inputSources.push_back(inputSource);
                    }
                }
            }
        }

        void GetReleasedAxisIndicesAndDirections(InputSourceVector& inputSources) const
        {
            if (this->joystick != 0)
            {
                const OIS::JoyStickState& joystickState = this->joystick->getJoyStickState();
                InputSource inputSource;
                inputSource.deviceType = InputSource::JOYSTICK_AXIS;
                for (inputSource.axis = 0; inputSource.axis < (int)joystickState.mAxes.size(); inputSource.axis++)
                {
                    if (ReleasedAxis(inputSource.axis, 0, Joystick::DEFAULT_DEAD_ZONE))
                    {
                        inputSource.direction = (joystickState.mAxes[inputSource.axis].abs < 0) ? -1 : 1;
                        inputSources.push_back(inputSource);
                    }
                }
            }
        }

        void GetPressedPovIndicesAndDirections(InputSourceVector& inputSources) const
        {
            if (this->joystick != 0)
            {
                const OIS::JoyStickState& joystickState = this->joystick->getJoyStickState();
                InputSource inputSource;
                inputSource.deviceType = InputSource::JOYSTICK_POV;
                for (inputSource.povIndex = 0; inputSource.povIndex < POV_COUNT; inputSource.povIndex++)
                {
                    if ((this->previousJoystickState.mPOV[inputSource.povIndex].direction == 0 || this->previousJoystickState.mPOV[inputSource.povIndex].direction != joystickState.mPOV[inputSource.povIndex].direction) &&
                        joystickState.mPOV[inputSource.povIndex].direction != 0)
                    {
                        inputSource.direction = joystickState.mPOV[inputSource.povIndex].direction;
                        inputSources.push_back(inputSource);
                    }
                }
            }
        }

        void GetPressingPovIndicesAndDirections(InputSourceVector& inputSources) const
        {
            if (this->joystick != 0)
            {
                const OIS::JoyStickState& joystickState = this->joystick->getJoyStickState();
                InputSource inputSource;
                inputSource.deviceType = InputSource::JOYSTICK_POV;
                for (inputSource.povIndex = 0; inputSource.povIndex < POV_COUNT; inputSource.povIndex++)
                {
                    if ((this->previousJoystickState.mPOV[inputSource.povIndex].direction == 0 || this->previousJoystickState.mPOV[inputSource.povIndex].direction != joystickState.mPOV[inputSource.povIndex].direction) &&
                        joystickState.mPOV[inputSource.povIndex].direction != 0)
                    {
                        inputSource.direction = joystickState.mPOV[inputSource.povIndex].direction;
                        inputSources.push_back(inputSource);
                    }
                }
            }
        }

        void GetReleasedPovIndicesAndDirections(InputSourceVector& inputSources) const
        {
            if (this->joystick != 0)
            {
                const OIS::JoyStickState& joystickState = this->joystick->getJoyStickState();
                InputSource inputSource;
                inputSource.deviceType = InputSource::JOYSTICK_POV;
                for (inputSource.povIndex = 0; inputSource.povIndex < POV_COUNT; inputSource.povIndex++)
                {
                    if ((this->previousJoystickState.mPOV[inputSource.povIndex].direction == 0 || this->previousJoystickState.mPOV[inputSource.povIndex].direction != joystickState.mPOV[inputSource.povIndex].direction) &&
                        joystickState.mPOV[inputSource.povIndex].direction != 0)
                    {
                        inputSource.direction = joystickState.mPOV[inputSource.povIndex].direction;
                        inputSources.push_back(inputSource);
                    }
                }
            }
        }

    private:
        Joystick()
        {
            this->joystick = 0;
            this->updatedOnce = false;
        }

        void Set(OIS::JoyStick* joystick)
        {
            this->joystick = joystick;
            this->updatedOnce = false;
        }

        void Update()
        {
            if (this->joystick != 0)
            {
                //Save current state as the previous state
                this->previousJoystickState = this->joystick->getJoyStickState();

                //Get new state
                this->joystick->capture();

                //Copy current state to previous state if this is the first update
                if (!this->updatedOnce)
                {
                    this->previousJoystickState = this->joystick->getJoyStickState();
                    this->updatedOnce = true;
                }
            }
        }

        static bool AxisActive(int value, int direction, int deadZone)
        {
            if (direction == 0)
                return std::abs(value) > deadZone;
            else
                return value * direction > deadZone;
        }

        static bool AxisActive(int value, int deadZone)
        {
            return std::abs(value) > deadZone;
        }

        static const OIS::Axis& GetJoystickAxis(const OIS::JoyStickState& joystickState, int axis)
        {
            static OIS::Axis nullAxis;

            if (axis < (int)joystickState.mAxes.size())
                return joystickState.mAxes[axis];
            else
                return nullAxis;
        }

    private:
        enum {BUTTON_COUNT = 16};
        enum {POV_COUNT = 4};

        OIS::JoyStick* joystick;
        OIS::JoyStickState previousJoystickState;
        bool updatedOnce;
    };

}

#endif
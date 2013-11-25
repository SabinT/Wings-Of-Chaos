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


#ifndef OgreMax_Mouse_INCLUDED
#define OgreMax_Mouse_INCLUDED


//Includes---------------------------------------------------------------------
#include <OIS.h>
#include "InputSource.hpp"


//Classes----------------------------------------------------------------------
namespace OgreMax
{
    /**
     * Wrapper for a mouse device
     * All public methods are safe to call with a null device
     */
    class Mouse
    {
        friend class InputSystem;

    public:
        enum {AXIS_COUNT = 3};

        const OIS::Mouse* GetMouse() const
        {
            return this->mouse;
        }

        bool IsNull() const
        {
            return this->mouse == 0;
        }

        bool PressedButton(OIS::MouseButtonID button) const
        {
            return
                this->mouse != 0 &&
                !this->previousMouseState.buttonDown(button) &&
                this->mouse->getMouseState().buttonDown(button);
        }

        bool PressingButton(OIS::MouseButtonID button) const
        {
            return
                this->mouse != 0 &&
                this->mouse->getMouseState().buttonDown(button);
        }

        bool ReleasedButton(OIS::MouseButtonID button) const
        {
            return
                this->mouse != 0 &&
                this->previousMouseState.buttonDown(button) &&
                !this->mouse->getMouseState().buttonDown(button);
        }

        bool PressedAnyButton() const
        {
            for (int buttonIndex = 0; buttonIndex < BUTTON_COUNT; buttonIndex++)
            {
                if (PressedButton((OIS::MouseButtonID)buttonIndex))
                    return true;
            }
            return false;
        }

        bool PressingAnyButton() const
        {
            return
                this->mouse != 0 &&
                this->mouse->getMouseState().buttons != 0;
        }

        bool ReleasedAnyButton() const
        {
            for (int buttonIndex = 0; buttonIndex < BUTTON_COUNT; buttonIndex++)
            {
                if (ReleasedButton((OIS::MouseButtonID)buttonIndex))
                    return true;
            }
            return false;
        }

        bool PressedAxis(int axis, int direction = 0) const
        {
            return
                this->mouse != 0 &&
                axis < AXIS_COUNT &&
                !AxisActive(GetMouseAxis(this->previousMouseState, axis).rel, direction) &&
                AxisActive(GetMouseAxis(this->mouse->getMouseState(), axis).rel, direction);
        }

        bool PressingAxis(int axis, int direction = 0) const
        {
            return
                this->mouse != 0 &&
                axis < AXIS_COUNT &&
                AxisActive(GetMouseAxis(this->mouse->getMouseState(), axis).rel, direction);
        }

        bool ReleasedAxis(int axis, int direction = 0) const
        {
            return
                this->mouse != 0 &&
                axis < AXIS_COUNT &&
                AxisActive(GetMouseAxis(this->previousMouseState, axis).rel, direction) &&
                !AxisActive(GetMouseAxis(this->mouse->getMouseState(), axis).rel, direction);
        }

        bool PressedAnyAxis() const
        {
            bool pressed = false;

            if (this->mouse != 0)
            {
                const OIS::MouseState& mouseState = this->mouse->getMouseState();
                pressed =
                    (!AxisActive(this->previousMouseState.X.rel) && AxisActive(mouseState.X.rel)) ||
                    (!AxisActive(this->previousMouseState.Y.rel) && AxisActive(mouseState.Y.rel)) ||
                    (!AxisActive(this->previousMouseState.Z.rel) && AxisActive(mouseState.Z.rel));
            }

            return pressed;
        }

        bool PressingAnyAxis() const
        {
            bool pressing = false;

            if (this->mouse != 0)
            {
                const OIS::MouseState& mouseState = this->mouse->getMouseState();
                pressing =
                    AxisActive(mouseState.X.rel) ||
                    AxisActive(mouseState.Y.rel) ||
                    AxisActive(mouseState.Z.rel);
            }

            return pressing;
        }

        bool ReleasedAnyAxis() const
        {
            bool released = false;

            if (this->mouse != 0)
            {
                const OIS::MouseState& mouseState = this->mouse->getMouseState();
                released =
                    (AxisActive(this->previousMouseState.X.rel) && !AxisActive(mouseState.X.rel)) ||
                    (AxisActive(this->previousMouseState.Y.rel) && !AxisActive(mouseState.Y.rel)) ||
                    (AxisActive(this->previousMouseState.Z.rel) && !AxisActive(mouseState.Z.rel));
            }

            return released;
        }

        int GetPressedButton() const
        {
            for (int buttonIndex = 0; buttonIndex < BUTTON_COUNT; buttonIndex++)
            {
                if (PressedButton((OIS::MouseButtonID)buttonIndex))
                    return buttonIndex;
            }

            return -1;
        }

        int GetPressingButton() const
        {
            for (int buttonIndex = 0; buttonIndex < BUTTON_COUNT; buttonIndex++)
            {
                if (PressingButton((OIS::MouseButtonID)buttonIndex))
                    return buttonIndex;
            }

            return -1;
        }

        int GetReleasedButton() const
        {
            for (int buttonIndex = 0; buttonIndex < BUTTON_COUNT; buttonIndex++)
            {
                if (ReleasedButton((OIS::MouseButtonID)buttonIndex))
                    return buttonIndex;
            }

            return -1;
        }

        bool GetPressedAxisIndexAndDirection(int& index, int& direction) const
        {
            if (this->mouse != 0)
            {
                for (int axisIndex = 0; axisIndex < AXIS_COUNT; axisIndex++)
                {
                    if (PressedAxis(axisIndex))
                    {
                        index = axisIndex;
                        direction = (GetAxis(axisIndex).rel < 0) ? -1 : 1;
                        return true;
                    }
                }
            }

            return false;
        }

        bool GetPressingAxisIndexAndDirection(int& index, int& direction) const
        {
            if (this->mouse != 0)
            {
                for (int axisIndex = 0; axisIndex < AXIS_COUNT; axisIndex++)
                {
                    if (PressingAxis(axisIndex))
                    {
                        index = axisIndex;
                        direction = (GetAxis(axisIndex).rel < 0) ? -1 : 1;
                        return true;
                    }
                }
            }

            return false;
        }

        bool GetReleasedAxisIndexAndDirection(int& index, int& direction) const
        {
            if (this->mouse != 0)
            {
                for (int axisIndex = 0; axisIndex < AXIS_COUNT; axisIndex++)
                {
                    if (ReleasedAxis(axisIndex))
                    {
                        index = axisIndex;
                        direction = (GetAxis(axisIndex).rel < 0) ? -1 : 1;
                        return true;
                    }
                }
            }

            return false;
        }

        void GetPressedButtons(InputSourceVector& inputSources) const
        {
            InputSource inputSource;
            inputSource.deviceType = InputSource::MOUSE_BUTTON;
            for (inputSource.button = 0; inputSource.button < BUTTON_COUNT; inputSource.button++)
            {
                if (PressedButton((OIS::MouseButtonID)inputSource.button))
                    inputSources.push_back(inputSource);
            }
        }

        void GetPressingButtons(InputSourceVector& inputSources) const
        {
            InputSource inputSource;
            inputSource.deviceType = InputSource::MOUSE_BUTTON;
            for (inputSource.button = 0; inputSource.button < BUTTON_COUNT; inputSource.button++)
            {
                if (PressingButton((OIS::MouseButtonID)inputSource.button))
                    inputSources.push_back(inputSource);
            }
        }

        void GetReleasedButtons(InputSourceVector& inputSources) const
        {
            InputSource inputSource;
            inputSource.deviceType = InputSource::MOUSE_BUTTON;
            for (inputSource.button = 0; inputSource.button < BUTTON_COUNT; inputSource.button++)
            {
                if (ReleasedButton((OIS::MouseButtonID)inputSource.button))
                    inputSources.push_back(inputSource);
            }
        }

        void GetPressedAxisIndicesAndDirections(InputSourceVector& inputSources) const
        {
            if (this->mouse != 0)
            {
                InputSource inputSource;
                inputSource.deviceType = InputSource::MOUSE_AXIS;
                for (inputSource.axis = 0; inputSource.axis < AXIS_COUNT; inputSource.axis++)
                {
                    if (PressedAxis(inputSource.axis))
                    {
                        inputSource.direction = (GetAxis(inputSource.axis).rel < 0) ? -1 : 1;
                        inputSources.push_back(inputSource);
                    }
                }
            }
        }

        void GetPressingAxisIndicesAndDirections(InputSourceVector& inputSources) const
        {
            if (this->mouse != 0)
            {
                InputSource inputSource;
                inputSource.deviceType = InputSource::MOUSE_AXIS;
                for (inputSource.axis = 0; inputSource.axis < AXIS_COUNT; inputSource.axis++)
                {
                    if (PressingAxis(inputSource.axis))
                    {
                        inputSource.direction = (GetAxis(inputSource.axis).rel < 0) ? -1 : 1;
                        inputSources.push_back(inputSource);
                    }
                }
            }
        }

        void GetReleasedAxisIndicesAndDirections(InputSourceVector& inputSources) const
        {
            if (this->mouse != 0)
            {
                InputSource inputSource;
                inputSource.deviceType = InputSource::MOUSE_AXIS;
                for (inputSource.axis = 0; inputSource.axis < AXIS_COUNT; inputSource.axis++)
                {
                    if (ReleasedAxis(inputSource.axis))
                    {
                        inputSource.direction = (GetAxis(inputSource.axis).rel < 0) ? -1 : 1;
                        inputSources.push_back(inputSource);
                    }
                }
            }
        }

        /**
         * Determines if the mouse has moved since the last Update(), using the
         * conventional notion of the X/Y axes indicating mouse movement
         */
        bool Moved() const
        {
            return
                this->mouse != 0 &&
                (AxisActive(this->mouse->getMouseState().X.rel) || AxisActive(this->mouse->getMouseState().Y.rel));
        }

        /** Gets the specified axis */
        const OIS::Axis& GetAxis(int axis) const
        {
            static OIS::Axis nullAxis;

            if (this->mouse != 0)
                return GetMouseAxis(this->mouse->getMouseState(), axis);
            else
                return nullAxis;
        }

    private:
        Mouse()
        {
            this->mouse = 0;
            this->updatedOnce = false;
        }

        void Set(OIS::Mouse* mouse)
        {
            this->mouse = mouse;
            this->updatedOnce = false;
        }

        void Update()
        {
            if (this->mouse != 0)
            {
                //Save current state as the previous state
                this->previousMouseState = this->mouse->getMouseState();

                //Get new state
                this->mouse->capture();

                //Copy current state to previous state if this is the first update
                if (!this->updatedOnce)
                {
                    this->previousMouseState = this->mouse->getMouseState();
                    this->updatedOnce = true;
                }
            }
        }

        static const OIS::Axis& GetMouseAxis(const OIS::MouseState& mouseState, int axis)
        {
            static OIS::Axis nullAxis;

            switch (axis)
            {
                case 0: return mouseState.X;
                case 1: return mouseState.Y;
                case 2: return mouseState.Z;
                default: return nullAxis;
            }
        }

        static bool AxisActive(int value, int direction = 0, int deadZone = 0)
        {
            if (direction == 0)
                return std::abs(value) > deadZone;
            else
                return value * direction > deadZone;
        }


    private:
        enum {BUTTON_COUNT = 8};

        OIS::Mouse* mouse;
        OIS::MouseState previousMouseState;
        bool updatedOnce;
    };

}

#endif
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


#ifndef OgreMax_ActionMap_INCLUDED
#define OgreMax_ActionMap_INCLUDED


//Includes---------------------------------------------------------------------
#include "InputSystem.hpp"


//Classes----------------------------------------------------------------------
namespace OgreMax
{
    /**
     * Template action map base class.
     *
     * This class operates by translating inputs to higher level application-defined actions.
     * The translation occurs in two stages. The first stage turns an input into an ActionID.
     * In the second stage, the ActionID is then turned into an EventID, which is added to a collection of Events.
     * For example, some ActionID values might be: MOVE_UP, MOVE_DOWN, MOVE_LEFT, MOVE_RIGHT. Those
     * might be translated to a single MOVE EventID, which may also have additional data for tracking
     * the direction of the move. In the process of performing the translation, depending on the
     * input source type, values may also be generated as well. This is the case for mouse and joystick inputs
     *
     * This class has one virtual method that must be overridden in a derived class:
     *   virtual void AddEvent(Events& events, ActionID action, InputSource::DeviceType deviceType, TriggerFlags trigger, Ogre::Real amount);
     *
     * The Events type is never used directly. It can be whatever you use in your AddEvent()
     * implementation. It will typically provide some facility for retaining the event information
     * that is passed to AddEvent().
     */
    template <class ActionID, class Events>
    class ActionMap
    {
    public:
        enum TriggerFlags
        {
            NO_FLAGS = 0,

            /** The event should only be triggered when the input is pressed  */
            PRESSED = 0x1,

            /** The event can be triggered continuously */
            PRESSING = 0x2,

            /** The event should only be triggered when the input is released */
            RELEASED = 0x4,

            /**
             * The event should be triggered with an ALT key pressed, in addition to the regular key
             * This only applies for a keyboard input source
             */
            ALT_MODIFIER = 0x8,

            /**
             * The event should be triggered with a CTRL key pressed, in addition to the regular key
             * This only applies for a keyboard input source
             */
            CTRL_MODIFIER = 0x10,

            /**
             * The event should be triggered with a SHIFT key pressed, in addition to the regular key
             * This only applies for a keyboard input source
             */
            SHIFT_MODIFIER = 0x20,

            //These NO modifiers indicate that the specified modifier should NOT be present in order
            //for the action to be triggered. This is useful in the rare case that you would like to
            //have separate actions for the same key, depending on which modifiers are pressed
            NO_ALT_MODIFIER = 0x40,
            NO_CTRL_MODIFIER = 0x80,
            NO_SHIFT_MODIFIER = 0x100,

            /**
             * Diagonal POV directions are considered to be part horizontal, part vertical
             * If this is specified, do not specify POV_STRONG
             */
            POV_WEAK = 0x200,

            /**
             * Each POV direction is considered to be distinct
             * If this is specified, do not specify POV_WEAK
             */
            POV_STRONG = 0x400
        };

        struct ActionMapping
        {
            /** The source input device for the action */
            InputSource inputSource;

            /** The action */
            ActionID action;

            /** Flags indicating when action is triggered */
            TriggerFlags triggerFlags;
        };

        /** Constructor/destructor */
        ActionMap() :
            defaultMouseSensitivity((Real).025, (Real).025, (Real).5),
            defaultJoystickAxisDeadZone(Joystick::DEFAULT_DEAD_ZONE),
            defaultJoystickAxisSensitivity(1)

        {
            this->mouseSensitivity = this->defaultMouseSensitivity;

            this->joystickAxisDeadZone.resize(MAX_JOYSTICK_AXES);
            this->joystickAxisSensitivity.resize(MAX_JOYSTICK_AXES);
            for (size_t index = 0; index < MAX_JOYSTICK_AXES; index++)
            {
                this->joystickAxisDeadZone[index] = this->defaultJoystickAxisDeadZone;
                this->joystickAxisSensitivity[index] = this->defaultJoystickAxisSensitivity;
            }
        }

        virtual ~ActionMap()
        {
        }

        /** Gets the number of mappings */
        size_t GetMappingCount() const
        {
            return this->mappings.size();
        }

        /** Gets the specified mapping */
        const ActionMapping& GetMapping(size_t index) const
        {
            return this->mappings[index];
        }

        /**
         * Gets all of the events currently present in the input devices.
         * Before calling this, make sure that Update has been called on the InputSystem
         */
        void GetEvents(Events& events)
        {
            InputSystem& inputSystem = InputSystem::GetInstance();

            //Process all action mappings
            for (size_t index = 0; index < this->mappings.size(); index++)
            {
                const ActionMapping& mapping = this->mappings[index];
                switch (mapping.inputSource.deviceType)
                {
                    case InputSource::KEYBOARD_KEY:
                    {
                        ProcessKeyboardKeyMapping(mapping, inputSystem.Keyboard, events);
                        break;
                    }
                    case InputSource::MOUSE_BUTTON:
                    {
                        ProcessMouseButtonMapping(mapping, inputSystem.Mouse, events);
                        break;
                    }
                    case InputSource::MOUSE_AXIS:
                    {
                        ProcessMouseAxisMapping(mapping, inputSystem.Mouse, events);
                        break;
                    }
                    case InputSource::JOYSTICK_BUTTON:
                    {
                        ProcessJoystickButtonMapping(mapping, inputSystem.GetJoystick(mapping.inputSource.deviceIndex), events);
                        break;
                    }
                    case InputSource::JOYSTICK_AXIS:
                    {
                        ProcessJoystickAxisMapping(mapping, inputSystem.GetJoystick(mapping.inputSource.deviceIndex), events);
                        break;
                    }
                    case InputSource::JOYSTICK_POV:
                    {
                        ProcessJoystickPovMapping(mapping, inputSystem.GetJoystick(mapping.inputSource.deviceIndex), events);
                        break;
                    }
                }
            }
        }

        /**
         * Checks the action map item for the specified action and input method.
         * @param deviceType [in] - The device type. This is needed since there may be multiple mappings
         * for the same action, each with a different device type.
         * Should be one of the following: InputSource::KEYBOARD_KEY, InputSource::MOUSE_BUTTON,
         * InputSource::MOUSE_AXIS, InputSource::JOYSTICK_BUTTON, InputSource::JOYSTICK_AXIS, InputSource::JOYSTICK_POV
         * @param action [in] - The action identifier
         * @return If the specified action mapping is found, true is returned. Otherwise, false is returned
         */
        bool HasActionMapping(InputSource::DeviceType deviceType, ActionID action) const
        {
            for (size_t index = 0; index < this->mappings.size(); index++)
            {
                const ActionMapping& mapping = this->mappings[index];
                if (mapping.action == action && mapping.inputSource.deviceType == deviceType)
                {
                    return true;
                }
            }

            return false;
        }

        /**
         * Gets the action map item for the specified action and input method.
         * @param deviceType [in] - The device type. This is needed since there may be multiple mappings
         * for the same action, each with a different device type.
         * Should be one of the following: InputSource::KEYBOARD_KEY, InputSource::MOUSE_BUTTON,
         * InputSource::MOUSE_AXIS, InputSource::JOYSTICK_BUTTON, InputSource::JOYSTICK_AXIS, InputSource::JOYSTICK_POV
         * @param action [in] - The action identifier
         * @param actionMapping [out] - The found action mapping
         * @return If the specified action mapping is found, true is returned. Otherwise, false is returned
         */
        bool GetActionMapping(InputSource::DeviceType deviceType, ActionID action, ActionMapping& actionMapping) const
        {
            for (size_t index = 0; index < this->mappings.size(); index++)
            {
                const ActionMapping& mapping = this->mappings[index];
                if (mapping.inputSource.deviceType == deviceType && mapping.action == action)
                {
                    actionMapping = mapping;
                    return true;
                }
            }

            return false;
        }

        /**
         * Adds a new action mapping or modifies an existing one.
         */
        void SetActionMapping(const ActionMapping& actionMapping)
        {
            //Try to set the mapping into an existing location
            for (size_t index = 0; index < this->mappings.size(); index++)
            {
                ActionMapping& mapping = this->mappings[index];
                if (mapping.inputSource.deviceType == actionMapping.inputSource.deviceType &&
                    mapping.action == actionMapping.action)
                {
                    mapping = actionMapping;
                    return;
                }
            }

            //If we're here, no such mapping exists. Add it
            AddActionMapping(actionMapping.inputSource, actionMapping.action, actionMapping.triggerFlags);
        }

        //Gets/sets the mouse sensitivity
        Ogre::Real GetMouseSensitivity(int axis) const
        {
            return this->mouseSensitivity[axis];
        }
        void SetMouseSensitivity(int axis, Ogre::Real sensitivity)
        {
            this->mouseSensitivity[axis] = sensitivity;
        }

        //Gets/sets the joystick dead zone
        int GetJoystickDeadZone(int axis) const
        {
            return this->joystickAxisDeadZone[axis];
        }
        void SetJoystickDeadZone(int axis, int deadZone)
        {
            this->joystickAxisDeadZone[axis] = deadZone;
        }

        //Gets/sets the sensitivity for a joystick axis
        Ogre::Real GetJoystickAxisSensitivity(int axis)
        {
            return this->joystickAxisSensitivity[axis];
        }
        void SetJoystickAxisSensitivity(int axis, Ogre::Real sensitivity)
        {
            this->joystickAxisSensitivity[axis] = sensitivity;
        }

    protected:
        /**
         * Translates an action to an event
         * @param events [out] - The container for the generated event
         * @param action [in] - The action identifier
         * @param deviceType [in] - The type of device that triggered the action to occur.
         * This is useful in rare situations where logic is needed for a particular device type
         * @param trigger [in] - The type of trigger that caused the event to occur. Will be
         * PRESSING, PRESSED, or RELEASED
         * @param amount [in] - This will vary depending on the input device mapped to
         * the action. Digital devices will generate 0 or 1, analog devices will generate
         * a value in [-1, 1]
         */
        virtual void AddEvent
            (
            Events& events,
            ActionID action,
            InputSource::DeviceType deviceType,
            TriggerFlags trigger,
            Ogre::Real amount
            ) = 0;

        /**
         * Adds an action mapping
         * @param inputSource [in] - The input source that generates the action
         * @param action [in] - The action identifier
         * @param triggerFlags [in] - Flags that affect the triggering of the action.
         */
        void AddActionMapping(InputSource inputSource, ActionID action, int triggerFlags)
        {
            AddActionMapping(inputSource, action, (TriggerFlags)triggerFlags);
        }

        /**
         * Adds an action mapping
         * @param inputSource [in] - The input source that generates the action
         * @param action [in] - The action identifier
         * @param triggerFlags [in] - Flags that affect the triggering of the action.
         */
        void AddActionMapping(InputSource inputSource, ActionID action, TriggerFlags triggerFlags = PRESSING)
        {
            ActionMapping mapping;
            mapping.inputSource = inputSource;
            mapping.action = action;
            mapping.triggerFlags = triggerFlags;
            this->mappings.push_back(mapping);
        }

    private:
        void ProcessKeyboardKeyMapping(const ActionMapping& mapping, const Keyboard& keyboard, Events& events)
        {
            if (!keyboard.IsNull())
            {
                //Check modifier keys if the mapping specifies a continuous or single press
                size_t modifierConditions = 0; //If this ends up as 0, the conditions are satisfied
                if ((mapping.triggerFlags & PRESSING) || (mapping.triggerFlags & PRESSED))
                {
                    if (mapping.triggerFlags & ALT_MODIFIER)
                    {
                        //Alt key needed
                        modifierConditions ^= ALT_MODIFIER;

                        if (keyboard.PressingKey(OIS::KC_LMENU) || keyboard.PressingKey(OIS::KC_RMENU))
                            modifierConditions ^= ALT_MODIFIER;
                    }
                    else if (mapping.triggerFlags & NO_ALT_MODIFIER)
                    {
                        //Alt key not wanted
                        if (keyboard.PressingKey(OIS::KC_LMENU) || keyboard.PressingKey(OIS::KC_RMENU))
                            modifierConditions ^= ALT_MODIFIER;
                    }

                    if (mapping.triggerFlags & CTRL_MODIFIER)
                    {
                        //Ctrl key needed
                        modifierConditions ^= CTRL_MODIFIER;

                        if (keyboard.PressingKey(OIS::KC_LCONTROL) || keyboard.PressingKey(OIS::KC_RCONTROL))
                            modifierConditions ^= CTRL_MODIFIER;
                    }
                    else if (mapping.triggerFlags & NO_CTRL_MODIFIER)
                    {
                        //Ctrl key not wanted
                        if (keyboard.PressingKey(OIS::KC_LCONTROL) || keyboard.PressingKey(OIS::KC_RCONTROL))
                            modifierConditions ^= CTRL_MODIFIER;
                    }

                    if (mapping.triggerFlags & SHIFT_MODIFIER)
                    {
                        //Shift key needed
                        modifierConditions ^= SHIFT_MODIFIER;

                        if (keyboard.PressingKey(OIS::KC_LSHIFT) || keyboard.PressingKey(OIS::KC_RSHIFT))
                            modifierConditions ^= SHIFT_MODIFIER;
                    }
                    else if (mapping.triggerFlags & NO_SHIFT_MODIFIER)
                    {
                        //Shift key not wanted
                        if (keyboard.PressingKey(OIS::KC_LSHIFT) || keyboard.PressingKey(OIS::KC_RSHIFT))
                            modifierConditions ^= SHIFT_MODIFIER;
                    }
                }

                if (modifierConditions == NO_FLAGS)
                {
                    if ((mapping.triggerFlags & PRESSED) && keyboard.PressedKey(mapping.inputSource.keyCode))
                        AddEvent(events, mapping.action, InputSource::KEYBOARD_KEY, PRESSED, 1);
                    else if ((mapping.triggerFlags & PRESSING) && keyboard.PressingKey(mapping.inputSource.keyCode))
                        AddEvent(events, mapping.action, InputSource::KEYBOARD_KEY, PRESSING, 1);
                    else if ((mapping.triggerFlags & RELEASED) && keyboard.ReleasedKey(mapping.inputSource.keyCode))
                        AddEvent(events, mapping.action, InputSource::KEYBOARD_KEY, RELEASED, 1);
                }
            }
        }

        void ProcessMouseButtonMapping(const ActionMapping& mapping, const Mouse& mouse, Events& events)
        {
            if (!mouse.IsNull())
            {
                if ((mapping.triggerFlags & PRESSED) && mouse.PressedButton((OIS::MouseButtonID)mapping.inputSource.button))
                    AddEvent(events, mapping.action, InputSource::MOUSE_BUTTON, PRESSED, 1);
                else if ((mapping.triggerFlags & PRESSING) && mouse.PressingButton((OIS::MouseButtonID)mapping.inputSource.button))
                    AddEvent(events, mapping.action, InputSource::MOUSE_BUTTON, PRESSING, 1);
                else if ((mapping.triggerFlags & RELEASED) && mouse.ReleasedButton((OIS::MouseButtonID)mapping.inputSource.button))
                    AddEvent(events, mapping.action, InputSource::MOUSE_BUTTON, RELEASED, 1);
            }
        }

        void ProcessMouseAxisMapping(const ActionMapping& mapping, const Mouse& mouse, Events& events)
        {
            if (!mouse.IsNull() && mapping.inputSource.axis < Mouse::AXIS_COUNT)
            {
                int value = mouse.GetAxis(mapping.inputSource.axis).rel;
                Ogre::Real amount =
                    value *
                    mapping.inputSource.direction *
                    this->mouseSensitivity[mapping.inputSource.axis];

                if ((mapping.triggerFlags & PRESSED) && mouse.PressedAxis(mapping.inputSource.axis, mapping.inputSource.direction))
                    AddEvent(events, mapping.action, InputSource::MOUSE_AXIS, PRESSED, amount);
                else if ((mapping.triggerFlags & PRESSING) && mouse.PressingAxis(mapping.inputSource.axis, mapping.inputSource.direction))
                    AddEvent(events, mapping.action, InputSource::MOUSE_AXIS, PRESSING, amount);
                else if ((mapping.triggerFlags & RELEASED) && mouse.ReleasedAxis(mapping.inputSource.axis, mapping.inputSource.direction))
                    AddEvent(events, mapping.action, InputSource::MOUSE_AXIS, RELEASED, amount);
            }
        }

        void ProcessJoystickButtonMapping(const ActionMapping& mapping, const Joystick& joystick, Events& events)
        {
            if (!joystick.IsNull())
            {
                if ((mapping.triggerFlags & PRESSED) && joystick.PressedButton(mapping.inputSource.button))
                    AddEvent(events, mapping.action, InputSource::JOYSTICK_BUTTON, PRESSED, 1);
                else if ((mapping.triggerFlags & PRESSING) && joystick.PressingButton(mapping.inputSource.button))
                    AddEvent(events, mapping.action, InputSource::JOYSTICK_BUTTON, PRESSING, 1);
                else if ((mapping.triggerFlags & RELEASED) && joystick.ReleasedButton(mapping.inputSource.button))
                    AddEvent(events, mapping.action, InputSource::JOYSTICK_BUTTON, RELEASED, 1);
            }
        }

        void ProcessJoystickAxisMapping(const ActionMapping& mapping, const Joystick& joystick, Events& events)
        {
            if (!joystick.IsNull())
            {
                const OIS::JoyStickState& joystickState = joystick.GetJoystick()->getJoyStickState();
                if (mapping.inputSource.axis < (int)joystickState.mAxes.size())
                {
                    int deadZone = this->joystickAxisDeadZone[mapping.inputSource.axis];
                    int value = joystickState.mAxes[mapping.inputSource.axis].abs;
                    Ogre::Real amount =
                        NormalizeJoystickAxis(value, deadZone) *
                        mapping.inputSource.direction *
                        this->joystickAxisSensitivity[mapping.inputSource.axis];

                    if ((mapping.triggerFlags & PRESSED) && joystick.PressedAxis(mapping.inputSource.axis, mapping.inputSource.direction, deadZone))
                        AddEvent(events, mapping.action, InputSource::JOYSTICK_AXIS, PRESSED, amount);
                    else if ((mapping.triggerFlags & PRESSING) && joystick.PressingAxis(mapping.inputSource.axis, mapping.inputSource.direction, deadZone))
                        AddEvent(events, mapping.action, InputSource::JOYSTICK_AXIS, PRESSING, amount);
                    else if ((mapping.triggerFlags & RELEASED) && joystick.ReleasedAxis(mapping.inputSource.axis, mapping.inputSource.direction, deadZone))
                        AddEvent(events, mapping.action, InputSource::JOYSTICK_AXIS, RELEASED, amount);
                }
            }
        }

        void ProcessJoystickPovMapping(const ActionMapping& mapping, const Joystick& joystick, Events& events)
        {
            if (!joystick.IsNull())
            {
                if ((mapping.triggerFlags & PRESSED) && joystick.PressedPov(mapping.inputSource.povIndex, mapping.inputSource.direction, (mapping.triggerFlags & POV_STRONG) != 0))
                    AddEvent(events, mapping.action, InputSource::JOYSTICK_POV, PRESSED, 1);
                else if ((mapping.triggerFlags & PRESSING) && joystick.PressingPov(mapping.inputSource.povIndex, mapping.inputSource.direction, (mapping.triggerFlags & POV_STRONG) != 0))
                    AddEvent(events, mapping.action, InputSource::JOYSTICK_POV, PRESSING, 1);
                else if ((mapping.triggerFlags & RELEASED) && joystick.ReleasedPov(mapping.inputSource.povIndex, mapping.inputSource.direction, (mapping.triggerFlags & POV_STRONG) != 0))
                    AddEvent(events, mapping.action, InputSource::JOYSTICK_POV, RELEASED, 1);
            }
        }

    protected:
        /**
         * Normalizes the input joystick offset value to the [-1, 1] range. This function assumes
         * that the joystick value is not within the dead zone
         * @param value [in] - The joystick value. In [-32768, 32767]
         * @param deadZone [in] - The joystick value dead zone. This value should be positive
         */
        Ogre::Real NormalizeJoystickAxis(int value, int deadZone)
        {
            int range;
            int nearValue;
            if (value < 0)
            {
                nearValue = -deadZone;
                range = 32768;
            }
            else
            {
                nearValue = deadZone;
                range = 32767;
            }

            return Ogre::Real(value - nearValue) / range;
        }

    protected:
        /** All the action mappings */
        std::vector<ActionMapping> mappings;

        /** The default sensitivities for each of the mouse axes */
        const Ogre::Vector3 defaultMouseSensitivity;

        /** The sensitivities for each of the mouse axes */
        Ogre::Vector3 mouseSensitivity;

        /** The maximum number of joystick axes */
        enum {MAX_JOYSTICK_AXES = 16};

        /** The dead zone for each of the joystick axes */
        const int defaultJoystickAxisDeadZone;

        /** The dead zones for each of the joystick axes */
        std::vector<int> joystickAxisDeadZone;

        /** The dead zone for each of the joystick axes */
        const Ogre::Real defaultJoystickAxisSensitivity;

        /** The sensitivities for each of the joystick axes */
        std::vector<Ogre::Real> joystickAxisSensitivity;
    };

}


#endif
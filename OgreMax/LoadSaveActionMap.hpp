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


#ifndef OgreMax_LoadSaveActionMap_INCLUDED
#define OgreMax_LoadSaveActionMap_INCLUDED


//Includes---------------------------------------------------------------------
#include "tinyxml/tinyxml.h"
#include "ActionMap.hpp"


//Classes----------------------------------------------------------------------
namespace OgreMax
{
    /**
     * ActionMap-derived class that provides the ability to load and save
     * the action map settings.
     *
     * Note that ActionID must be able to evaluate to false/true. It will
     * evaluate to false when the value returned by ParseAction is invalid
     */
    template <class ActionID, class EventCollection>
    class LoadSaveActionMap : public ActionMap<ActionID, EventCollection>
    {
    public:

        /** Describes an action */
        class ActionInfo
        {
        public:
            /** Flags */
            enum
            {
                NO_FLAGS = 0,

                /** Indicates that the action is private and should not be loaded or saved */
                PRIVATE = 0x1
            };

            /** Constructor used to initialize the null action info */
            ActionInfo()
            {
                this->flags = NO_FLAGS;
                this->isNull = true;
            }

            /** The most commonly used constructor */
            ActionInfo(const char* actionName, ActionID action, int flags = NO_FLAGS) : name(actionName)
            {
                this->action = action;
                this->flags = flags;
                this->isNull = false;
            }

            /** Determines if the action is public */
            bool IsPublic() const
            {
                return this->name != 0 && this->name[0] != 0 && !(this->flags & PRIVATE);
            }

        public:
            /**
             * The name of the action.
             * This will be used when loading and saving mappings, so it's best if
             * the name doesn't contain whitespace
             */
            const char* name;

            /** The action identifier */
            ActionID action;

            /** Indicates whether the info represents the 'null' action */
            bool isNull;

        private:
            /** Flags */
            int flags;
        };

        /**
         * Loads the action map settings from the specified file
         * @param fileName [in] - Name of the file to laod
         * @return Returns true if the file was successfully loaded, false otherwise
         */
        virtual bool LoadFile(const Ogre::String& fileName)
        {
            TiXmlDocument xmlDocument;
            if (!xmlDocument.LoadFile(fileName.c_str()))
                return false;

            TiXmlElement* rootXmlElement = xmlDocument.RootElement();

            //Settings
            const TiXmlElement* settingsXmlElement = (const TiXmlElement*)rootXmlElement->FirstChild("settings");
            LoadSettings(settingsXmlElement);

            //Mappings
            const TiXmlElement* mappingsXmlElement = (const TiXmlElement*)rootXmlElement->FirstChild("mappings");
            LoadMappings(mappingsXmlElement);

            return true;
        }

        /** Saves the action map settings to the specified file */
        virtual void SaveFile(const Ogre::String& fileName)
        {
            TiXmlDocument xmlDocument;
            TiXmlElement* rootXmlElement = (TiXmlElement*)xmlDocument.LinkEndChild(new TiXmlElement("actionMap"));

            SaveSettings(rootXmlElement);
            SaveMappings(rootXmlElement);

            xmlDocument.SaveFile(fileName.c_str());
        }

    protected:
        /**
         * Gets a pointer to an array containing information about all the actions
         * The final element in the array must be a 'null' ActionInfo
         */
        virtual const ActionInfo* GetActionInfo() const = 0;

    private:
        /** Gets information about an action, using an action identifier. */
        const ActionInfo& GetActionInfo(ActionID action) const
        {
            const ActionInfo* actionInfo = GetActionInfo();
            int index = 0;
            for (; !actionInfo[index].isNull; index++)
            {
                if (action == actionInfo[index].action)
                    return actionInfo[index];
            }

            //Return null ActionInfo
            return actionInfo[index];
        }

        /** Gets information about an action, using an action name. */
        const ActionInfo& GetActionInfo(const Ogre::String& action) const
        {
            const ActionInfo* actionInfo = GetActionInfo();
            int index = 0;
            for (; !actionInfo[index].isNull; index++)
            {
                if (action == actionInfo[index].name)
                    return actionInfo[index];
            }

            //Return null ActionInfo
            return actionInfo[index];
        }

        /** Loads data from the 'settings' element */
        void LoadSettings(const TiXmlElement* settingsXmlElement)
        {
            if (settingsXmlElement == 0)
                return;

            const TiXmlElement* mouseXmlElement = (const TiXmlElement*)settingsXmlElement->FirstChild("mouse");
            LoadMouseSettings(mouseXmlElement);

            const TiXmlElement* joystickXmlElement = (const TiXmlElement*)settingsXmlElement->FirstChild("joystick");
            LoadJoystickSettings(joystickXmlElement);
        }

        /** Loads data from the 'mappings' element */
        void LoadMappings(const TiXmlElement* mappingsXmlElement)
        {
            if (mappingsXmlElement == 0)
                return;

            Ogre::String actionName;
            Ogre::String sourceName;

            const TiXmlNode* childXmlNode = 0;
            while ((childXmlNode = mappingsXmlElement->IterateChildren("mapping", childXmlNode)))
            {
	            if (childXmlNode->Type() == TiXmlNode::ELEMENT)
	            {
		            const TiXmlElement* mappingXmlElement = (const TiXmlElement*)childXmlNode;

                    //Action name
                    actionName = GetStringAttribute(mappingXmlElement, "action");

                    //Load the mapping if its action is public
                    const ActionInfo& actionInfo = GetActionInfo(actionName);
                    if (actionInfo.IsPublic())
                    {
                        //Input source
                        InputSource inputSource;
                        LoadInputSource(mappingXmlElement, inputSource);

                        //Trigger flags
                        TriggerFlags triggerFlags = LoadTriggerFlags(mappingXmlElement);

                        //Add the action mapping
                        AddActionMapping(inputSource, actionInfo.action, triggerFlags);
                    }
                }
            }
        }

        /** Loads data from the 'mouse' element */
        void LoadMouseSettings(const TiXmlElement* element)
        {
            if (element == 0)
                return;

            const TiXmlNode* childXmlNode = 0;
            while ((childXmlNode = element->IterateChildren("setting", childXmlNode)))
            {
	            if (childXmlNode->Type() == TiXmlNode::ELEMENT)
	            {
		            const TiXmlElement* settingXmlElement = (const TiXmlElement*)childXmlNode;

                    //Axis
                    int axis;
                    if (settingXmlElement->Attribute("axis", &axis) && axis >= 0 && axis < Mouse::AXIS_COUNT)
                    {
                        //Sensitivity
                        double sensitivity;
                        if (settingXmlElement->Attribute("sensitivity", &sensitivity))
                            this->mouseSensitivity[axis] = (Ogre::Real)sensitivity;
                    }
	            }
            }
        }

        /** Loads data from the 'joystick' element */
        void LoadJoystickSettings(const TiXmlElement* element)
        {
            if (element == 0)
                return;

            const TiXmlNode* childXmlNode = 0;
            while ((childXmlNode = element->IterateChildren("setting", childXmlNode)))
            {
	            if (childXmlNode->Type() == TiXmlNode::ELEMENT)
	            {
		            const TiXmlElement* settingXmlElement = (const TiXmlElement*)childXmlNode;

                    //Axis
                    int axis;
                    if (settingXmlElement->Attribute("axis", &axis) && axis >= 0)
                    {
                        if (axis >= (int)this->joystickAxisSensitivity.size())
                            this->joystickAxisSensitivity.resize(axis + 1);
                        if (axis >= (int)this->joystickAxisDeadZone.size())
                            this->joystickAxisDeadZone.resize(axis + 1);

                        //Sensitivity
                        double sensitivity;
                        if (settingXmlElement->Attribute("sensitivity", &sensitivity))
                            this->joystickAxisSensitivity[axis] = (Ogre::Real)sensitivity;

                        //Dead zone
                        int deadZone;
                        if (settingXmlElement->Attribute("deadZone", &deadZone))
                            this->joystickAxisDeadZone[axis] = deadZone;
                    }
	            }
            }
        }

        /** Saves the mappings the 'settings' element */
        void SaveSettings(TiXmlElement* rootXmlElement)
        {
            //Determine which settings have changed from their defaults
            bool modifiedMouseSensitivity = this->mouseSensitivity != this->defaultMouseSensitivity;
            bool modifiedJoystickAxisDeadZones =
                std::find_if
                    (
                    this->joystickAxisDeadZone.begin(),
                    this->joystickAxisDeadZone.end(),
                    std::bind2nd(std::not_equal_to<int>(), this->defaultJoystickAxisDeadZone)
                    ) != this->joystickAxisDeadZone.end();
            bool modifiedJoystickAxisSensitivities =
                std::find_if
                    (
                    this->joystickAxisSensitivity.begin(),
                    this->joystickAxisSensitivity.end(),
                    std::bind2nd(std::not_equal_to<Ogre::Real>(), this->defaultJoystickAxisSensitivity)
                    ) != this->joystickAxisSensitivity.end();

            //Only save if some settings have changed
            if (modifiedMouseSensitivity || modifiedJoystickAxisDeadZones || modifiedJoystickAxisSensitivities)
            {
                TiXmlElement* settingsXmlElement =
                    (TiXmlElement*)rootXmlElement->LinkEndChild(new TiXmlElement("settings"));

                //Mouse sensitivities
                if (modifiedMouseSensitivity)
                {
                    TiXmlElement* mouseXmlElement =
                        (TiXmlElement*)settingsXmlElement->LinkEndChild(new TiXmlElement("mouse"));
                    for (int axisIndex = 0; axisIndex < Mouse::AXIS_COUNT; axisIndex++)
                    {
                        if (this->mouseSensitivity[axisIndex] != this->defaultMouseSensitivity[axisIndex])
                        {
                            TiXmlElement* settingXmlElement =
                                (TiXmlElement*)mouseXmlElement->LinkEndChild(new TiXmlElement("setting"));

                            //Axis index
                            settingXmlElement->SetAttribute("axis", axisIndex);

                            //Sensitivity
                            settingXmlElement->SetDoubleAttribute("sensitivity", this->mouseSensitivity[axisIndex]);
                        }
                    }
                }

                //Joystick axis dead zones and sensitivities
                if (modifiedJoystickAxisDeadZones || modifiedJoystickAxisSensitivities)
                {
                    TiXmlElement* joystickXmlElement =
                        (TiXmlElement*)settingsXmlElement->LinkEndChild(new TiXmlElement("joystick"));

                    size_t maxAxisCount = (int)std::max(this->joystickAxisDeadZone.size(), this->joystickAxisSensitivity.size());
                    for (size_t axisIndex = 0; axisIndex < maxAxisCount; axisIndex++)
                    {
                        if ((axisIndex < this->joystickAxisDeadZone.size() && this->joystickAxisDeadZone[axisIndex] != this->defaultJoystickAxisDeadZone) ||
                            (axisIndex < this->joystickAxisSensitivity.size() && this->joystickAxisSensitivity[axisIndex] != this->defaultJoystickAxisSensitivity))
                        {
                            TiXmlElement* settingXmlElement =
                                (TiXmlElement*)joystickXmlElement->LinkEndChild(new TiXmlElement("setting"));

                            //Axis index
                            settingXmlElement->SetAttribute("axis", (int)axisIndex);

                            //Dead zone
                            if (axisIndex < this->joystickAxisDeadZone.size() && this->joystickAxisDeadZone[axisIndex] != this->defaultJoystickAxisDeadZone)
                                settingXmlElement->SetAttribute("deadZone", this->joystickAxisDeadZone[axisIndex]);

                            //Sensitivity
                            if (axisIndex < this->joystickAxisSensitivity.size() && this->joystickAxisSensitivity[axisIndex] != this->defaultJoystickAxisSensitivity)
                                settingXmlElement->SetDoubleAttribute("sensitivity", this->joystickAxisSensitivity[axisIndex]);
                        }
                    }
                }
            }
        }

        /** Saves the mappings a 'mappings' element */
        void SaveMappings(TiXmlElement* rootXmlElement)
        {
            TiXmlElement* mappingsXmlElement = (TiXmlElement*)rootXmlElement->LinkEndChild(new TiXmlElement("mappings"));
            for (size_t mappingIndex = 0; mappingIndex < this->mappings.size(); mappingIndex++)
            {
                ActionMapping& mapping = this->mappings[mappingIndex];

                //Save the mapping if its action is public
                const ActionInfo& actionInfo = GetActionInfo(mapping.action);
                if (actionInfo.IsPublic())
                {
                    TiXmlElement* mappingXmlElement =
                        (TiXmlElement*)mappingsXmlElement->LinkEndChild(new TiXmlElement("mapping"));

                    //Action name
                    mappingXmlElement->SetAttribute("action", actionInfo.name);

                    //Input source
                    SaveInputSource(mappingXmlElement, mapping.inputSource);

                    //Trigger flags
                    SaveTriggerFlags(mappingXmlElement, mapping.triggerFlags);
                }
            }
        }

        /** Loads an InputSource from the specified element's attributes */
        static void LoadInputSource(const TiXmlElement* element, InputSource& inputSource)
        {
            Ogre::String sourceName = GetStringAttribute(element, "source");
            inputSource.deviceType = InputSource::DeviceTypeFromString(sourceName);

            switch (inputSource.deviceType)
            {
                case InputSource::KEYBOARD_KEY:
                {
                    int key;
                    if (element->Attribute("key", &key))
                        inputSource.keyCode = (OIS::KeyCode)key;
                    break;
                }
                case InputSource::MOUSE_BUTTON:
                {
                    element->Attribute("button", &inputSource.button);
                    break;
                }
                case InputSource::MOUSE_AXIS:
                {
                    element->Attribute("axis", &inputSource.axis);
                    element->Attribute("direction", &inputSource.direction);
                    break;
                }
                case InputSource::JOYSTICK_BUTTON:
                {
                    element->Attribute("device", &inputSource.deviceIndex);
                    element->Attribute("button", &inputSource.button);
                    break;
                }
                case InputSource::JOYSTICK_AXIS:
                {
                    element->Attribute("device", &inputSource.deviceIndex);
                    element->Attribute("axis", &inputSource.axis);
                    element->Attribute("direction", &inputSource.direction);
                    break;
                }
                case InputSource::JOYSTICK_POV:
                {
                    element->Attribute("device", &inputSource.deviceIndex);
                    element->Attribute("index", &inputSource.povIndex);
                    element->Attribute("direction", &inputSource.direction);
                    break;
                }
            }
        }

        /** Saves an InputSource to the specified element's attributes */
        static void SaveInputSource(TiXmlElement* element, const InputSource& inputSource)
        {
            element->SetAttribute("source", inputSource.GetDeviceTypeString());
            switch (inputSource.deviceType)
            {
                case InputSource::KEYBOARD_KEY:
                    element->SetAttribute("key", (int)inputSource.keyCode);
                    break;
                case InputSource::MOUSE_BUTTON:
                    element->SetAttribute("button", inputSource.button);
                    break;
                case InputSource::MOUSE_AXIS:
                    element->SetAttribute("axis", inputSource.axis);
                    element->SetAttribute("direction", inputSource.direction);
                    break;
                case InputSource::JOYSTICK_BUTTON:
                    element->SetAttribute("device", inputSource.deviceIndex);
                    element->SetAttribute("button", inputSource.button);
                    break;
                case InputSource::JOYSTICK_AXIS:
                    element->SetAttribute("device", inputSource.deviceIndex);
                    element->SetAttribute("axis", inputSource.axis);
                    element->SetAttribute("direction", inputSource.direction);
                    break;
                case InputSource::JOYSTICK_POV:
                    element->SetAttribute("device", inputSource.deviceIndex);
                    element->SetAttribute("index", inputSource.povIndex);
                    element->SetAttribute("direction", inputSource.direction);
                    break;
            }
        }

        /** Loads TriggerFlags from the specified element's attributes */
        static TriggerFlags LoadTriggerFlags(const TiXmlElement* element)
        {
            TriggerFlags triggerFlags = NO_FLAGS;
            if (element->Attribute("pressed"))
                triggerFlags = TriggerFlags(triggerFlags | PRESSED);
            if (element->Attribute("pressing"))
                triggerFlags = TriggerFlags(triggerFlags | PRESSING);
            if (element->Attribute("released"))
                triggerFlags = TriggerFlags(triggerFlags | RELEASED);
            if (element->Attribute("alt"))
                triggerFlags = TriggerFlags(triggerFlags | ALT_MODIFIER);
            if (element->Attribute("ctrl"))
                triggerFlags = TriggerFlags(triggerFlags | CTRL_MODIFIER);
            if (element->Attribute("shift"))
                triggerFlags = TriggerFlags(triggerFlags | SHIFT_MODIFIER);
            if (element->Attribute("noAlt"))
                triggerFlags = TriggerFlags(triggerFlags | NO_ALT_MODIFIER);
            if (element->Attribute("noCtrl"))
                triggerFlags = TriggerFlags(triggerFlags | NO_CTRL_MODIFIER);
            if (element->Attribute("noShift"))
                triggerFlags = TriggerFlags(triggerFlags | NO_SHIFT_MODIFIER);
            if (element->Attribute("povWeak"))
                triggerFlags = TriggerFlags(triggerFlags | POV_WEAK);
            if (element->Attribute("povStrong"))
                triggerFlags = TriggerFlags(triggerFlags | POV_STRONG);
            return triggerFlags;
        }

        /** Saves TriggerFlags to the specified element's attributes */
        static void SaveTriggerFlags(TiXmlElement* element, TriggerFlags triggerFlags)
        {
            if (triggerFlags & PRESSED)
                element->SetAttribute("pressed", "true");
            if (triggerFlags & PRESSING)
                element->SetAttribute("pressing", "true");
            if (triggerFlags & RELEASED)
                element->SetAttribute("released", "true");
            if (triggerFlags & ALT_MODIFIER)
                element->SetAttribute("alt", "true");
            if (triggerFlags & CTRL_MODIFIER)
                element->SetAttribute("ctrl", "true");
            if (triggerFlags & SHIFT_MODIFIER)
                element->SetAttribute("shift", "true");
            if (triggerFlags & NO_ALT_MODIFIER)
                element->SetAttribute("noAlt", "true");
            if (triggerFlags & NO_CTRL_MODIFIER)
                element->SetAttribute("noCtrl", "true");
            if (triggerFlags & NO_SHIFT_MODIFIER)
                element->SetAttribute("noShift", "true");
            if (triggerFlags & POV_WEAK)
                element->SetAttribute("povWeak", "true");
            if (triggerFlags & POV_STRONG)
                element->SetAttribute("povStrong", "true");
        }

        static Ogre::String GetStringAttribute(const TiXmlElement* xmlElement, const char* name)
        {
            const char* value = xmlElement->Attribute(name);
            if (value != 0)
                return value;
            else
                return Ogre::StringUtil::BLANK;
        }
    };

}


#endif
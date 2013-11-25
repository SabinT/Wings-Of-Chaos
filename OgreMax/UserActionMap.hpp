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


#ifndef OgreMax_UserActionMap_INCLUDED
#define OgreMax_UserActionMap_INCLUDED


//Includes---------------------------------------------------------------------
#include "LoadSaveActionMap.hpp"
#include "EventCollection.hpp"
#include <OgreVector2.h>
#include <OgreVector3.h>


//Classes----------------------------------------------------------------------
namespace OgreMax
{
    class UserEventID
    {
    public:
        enum Type
        {
            MOVE,
            LOOK,
            CHANGE_CAMERA,
            TOGGLE_VIEW_CAMERA_LOCK,
            SCREENSHOT,
            ESCAPE,
            EXIT
        };
    };

    class UserEvents : public EventCollection<UserEventID::Type>
    {
    public:
        UserEvents() : EventCollection<UserEventID::Type>()
        {
            ClearEvents();
        }

        void ClearEvents()
        {
            this->events = 0;

            this->move = Ogre::Vector3::ZERO;
            this->look = Ogre::Vector2::ZERO;
            this->cameraIndexDirection = 0;
            this->cameraIndex = -1;
        }

        void AddEvent(UserEventID::Type eventID)
        {
            if (this->canAddEvents)
                this->events |= (1 << eventID);
        }

        void RemoveEvent(UserEventID::Type eventID)
        {
            size_t eventFlag = (1 << eventID);
            this->events &= ~eventFlag;
        }

        bool HasAnyEvent() const
        {
            return this->events != 0;
        }

        bool HasEvent(UserEventID::Type eventID) const
        {
            size_t eventFlag = (1 << eventID);
            return (this->events & eventFlag) != 0;
        }

    public:
        /** The event flags. */
        size_t events;

        /**
         * The components represent the amount of movement,
         * where +x is right, -x is left, +y is forward, -y is backward, +z is up, -z is down
         * The MOVE event must be set for this to be used
         */
        Ogre::Vector3 move;

        /**
         * The components represent the amount of rotation,
         * where +x is turn right, -x is turn left, +y is tilt upward, -y is tilt downward
         * The LOOK event must be set for this to be used
         */
        Ogre::Vector2 look;

        /**
         * Indicates that the user wishes to switch to the previous or next camera
         * 0 means no change, -1 is previous camera, 1 is next camera
         * A CHANGE_CAMERA event must be set for this to be used
         */
        int cameraIndexDirection;

        /**
         * Indicates that the user wishes to switch to a particular camera
         * -1 means no change, >= 0 indicates which camera to switch to
         * A CHANGE_CAMERA event must be set for this to be used
         */
        int cameraIndex;
    };

    class UserActionID
    {
    public:
        enum Type
        {
            MOVE_FORWARD,
            MOVE_BACKWARD,
            MOVE_LEFT,
            MOVE_RIGHT,
            MOVE_UP,
            MOVE_DOWN,
            LOOK_UP,
            LOOK_DOWN,
            LOOK_LEFT,
            LOOK_RIGHT,
            PREVIOUS_CAMERA,
            NEXT_CAMERA,
            SET_CAMERA1, SET_CAMERA2, SET_CAMERA3, SET_CAMERA4, SET_CAMERA5, SET_CAMERA6, SET_CAMERA7, SET_CAMERA8, SET_CAMERA9,
            TOGGLE_VIEW_CAMERA_LOCK,
            SCREENSHOT,
            ESCAPE,
            EXIT
        };
    };

    class UserActionMap : public LoadSaveActionMap<UserActionID::Type, UserEvents>
    {
    public:
        UserActionMap();

        void Initialize(const Ogre::String& fileName, InputSource::DeviceClass deviceClass = InputSource::ALL_DEVICE_CLASSES);

    protected:
        void AddEvent
            (
            UserEvents& events,
            UserActionID::Type actionID,
            InputSource::DeviceType deviceType,
            TriggerFlags trigger,
            Ogre::Real amount
            );

        const ActionInfo* GetActionInfo() const;

    private:
        /** Sets all public and private settings to their defaults. Called during initialization*/
        void SetDefaults(InputSource::DeviceClass deviceClass);

        /** Sets all private settings to their defaults. Called during initialization*/
        void SetPrivateDefaults(InputSource::DeviceClass deviceClass);

    };
}


#endif
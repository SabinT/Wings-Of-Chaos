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


#ifndef OgreMax_EventCollection_INCLUDED
#define OgreMax_EventCollection_INCLUDED


//Includes---------------------------------------------------------------------
#include <OgreVector3.h>


//Classes----------------------------------------------------------------------
namespace OgreMax
{
    /**
     * An abstract collection of events, with additional (and optional) time-based
     * event throttling, which is useful if you wish to space out event generation.
     * Note that this throttling is not enforced in any way - it is up to derived classes
     * to enforce a throttling policy
     *
     * In addition to overriding the required methods, derived classes may wish
     * to implement a ClearEvents() method that allows all events and their associated
     * state to be reseet. This is typically useful during development, when not all
     * events are checked/removed during an update - in this case ClearEvents() would
     * be called after the limited event checking code
     */
    template <class EventID, class TimeValue = Ogre::Real>
    class EventCollection
    {
    public:
        EventCollection()
        {
            this->timeAccumulator = 0;
            this->updateInterval = 0;

            this->canAddEvents = true;
        }

        virtual ~EventCollection()
        {
        }

        /**
         * Performs a time-based update to determine if the events can be added
         * Calling this is not necessary if the time-throttling is not needed
         */
        virtual bool UpdateThrottle(TimeValue elapsedTime)
        {
            this->canAddEvents = true;

            if (this->updateInterval > 0)
            {
                this->timeAccumulator += elapsedTime;
                if (this->timeAccumulator >= this->updateInterval)
                {
                    do
                    {
                        this->timeAccumulator -= this->updateInterval;
                    }while (this->timeAccumulator >= this->updateInterval);
                }
                else
                    this->canAddEvents = false;
            }

            return this->canAddEvents;
        }

        /**
         * Adds an event to the collection.
         * The implementation should take care to record the event only once.
         */
        virtual void AddEvent(EventID eventID) = 0;

        /** Removes an event from the collection */
        virtual void RemoveEvent(EventID eventID) = 0;

        /** Determines if the collection has any events */
        virtual bool HasAnyEvent() const = 0;

        /** Determines if the collection has the specified event */
        virtual bool HasEvent(EventID eventID) const = 0;

        /**
         * Removes the event if present, and returns whether the event was removed
         * Most of the time this will be called to simultaneously check and clear
         * an event.
         */
        virtual bool ConsumeEvent(EventID eventID)
        {
            bool hasEvent = HasEvent(eventID);
            if (hasEvent)
                RemoveEvent(eventID);
            return hasEvent;
        }

        TimeValue GetUpdateInterval() const
        {
            return this->updateInterval;
        }

        void SetUpdateInterval(TimeValue updateInterval)
        {
            this->updateInterval = updateInterval;
        }

        /** Returns a value, from 0 to 1, indicating how far across the time interval the current time is */
        Ogre::Real GetInterpolator() const
        {
            if (this->updateInterval == 0)
                return 0;
            else
                return (Ogre::Real)this->timeAccumulator / (Ogre::Real)this->updateInterval;
        }

    protected:
        //Time values used for event throttling
        TimeValue timeAccumulator;
        TimeValue updateInterval;

        /** Indicates whether events can be added. Use of this during AddEvent() is optional */
        bool canAddEvents;
    };

}


#endif
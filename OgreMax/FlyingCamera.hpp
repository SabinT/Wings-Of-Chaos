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


#ifndef OgreMax_FlyingCamera_INCLUDED
#define OgreMax_FlyingCamera_INCLUDED


//Includes---------------------------------------------------------------------
#include "OgreMaxTypes.hpp"


//Classes----------------------------------------------------------------------
namespace OgreMax
{
    /**
     * Manages a camera position and rotation that allows the user to control
     * the camera and fly through a scene.
     */
    class FlyingCamera
    {
    public:
        FlyingCamera();
        virtual ~FlyingCamera();

        /** Sets the flying camera's up axis and, optionally, its rotation. */
        void SetUpAxis(Types::UpAxis upAxis, bool setDefaultRotation = false);

        /** Gets the current flying camera position. */
        const Ogre::Vector3& GetPosition() const;

        /** Sets the current flying camera position. */
        void SetPosition(const Ogre::Vector3& position);

        /** Gets the current flying camera rotation. */
        const Ogre::Quaternion& GetRotation();

        /** Sets the current flying camera rotation. */
        void SetRotation(const Ogre::Quaternion& rotation);

        /**
         * Sets the camera to turn into a 'flying camera'.
         * @param camera [in] - Pointer to the camera
         * @param useParentNodeIfAvailable [in] - If true, indicatets that the camera's parent node,
         * if available, should be used, rather than the camera object itself. If false, the camera
         * is used directly.
         * @param copySettings [in] - Indicates whether the camera's settings should be copied
         * This allows the camera object to be changed while maintaining the previous position
         * and orientation (for copySettings = false).
         */
        void SetCamera(Ogre::Camera* camera, bool useParentNodeIfAvailable = true, bool copySettings = true);

        /**
         * Sets the node to turn into a 'flying camera'. The node is used directly and doesn't
         * need to contain a camera
         * @param copySettings [in] - Indicates whether the camera's settings should be copied
         * This allows the camera object to be changed while maintaining the previous position
         * and orientation (for copySettings = false).
         */
        void SetCameraNode(Ogre::Node* cameraNode, bool copySettings = true);

        /**
         * Moves the flying camera relative to its current position, along its local directions
         * @param move [in] - The amount to move in each direction
         *    move.x = left/right movement
         *    move.y = forward/backward movement
         *    move.z = up/down movement
         * @param elapsedTime [in] - The amount of time that passed for the last frame, in seconds
         */
        void Move(const Ogre::Vector3& move, Ogre::Real elapsedTime = 1);

        /**
         * Rotates the flying camera relative to its rotation
         * @param rotate [in] - The amount to rotate
         *    rotate.x = "turn". This is the amount to rotate around up axis
         *    rotate.y = "tilt". This is the amount to rotate backward (toward sky, +y values) or
         *               forward (toward ground, -y values)
         */
        void Rotate(const Ogre::Vector2& rotate);

        /** Updates the camera with the current current position and rotation. */
        void UpdateCamera();

        /** Gets the speed of the camera movement */
        float GetMoveSpeed() const {return this->moveSpeed;}

        /** Sets the speed of the camera movement */
        void SetMoveSpeed(Ogre::Real speed) {this->moveSpeed = speed;}

    protected:
        /** Gets the direction vectors for the current rotation */
        void GetDirections(Ogre::Vector3& rightDirection, Ogre::Vector3& forwardDirection, Ogre::Vector3& upDirection);

        void Initialize();

    protected:
        /** Indicates which way is up in the world */
        Types::UpAxis upAxis;

        /** The node that is acting as a flying camera. If this is set, camera is null. */
        Ogre::Node* cameraNode;

        /** The camera that is acting as a flying camera. If this is set, cameraNode is null. */
        Ogre::Camera* camera;

        /** Intermediate position. Call UpdateCamera() to commit this value to the camera. */
        Ogre::Vector3 position;

        /** Intermediate rotation. Call UpdateCamera() to commit this value to the camera. */
        Ogre::Quaternion rotation;

        /** The speed of the motion, in units per second */
        Ogre::Real moveSpeed;
    };

}


#endif
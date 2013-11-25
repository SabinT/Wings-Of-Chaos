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


//Includes---------------------------------------------------------------------
#include "FlyingCamera.hpp"

using namespace Ogre;
using namespace OgreMax;
using namespace OgreMax::Types;


//Implementation---------------------------------------------------------------
FlyingCamera::FlyingCamera()
{
    this->upAxis = UP_AXIS_Y;
    this->cameraNode = 0;
    this->camera = 0;
    this->moveSpeed = 100;
}

FlyingCamera::~FlyingCamera()
{
}

void FlyingCamera::Initialize()
{
    if (this->cameraNode != 0)
    {
        this->position = this->cameraNode->_getDerivedPosition();
        this->rotation = this->cameraNode->_getDerivedOrientation();
    }
    else if (this->camera != 0)
    {
        this->position = this->camera->getRealPosition();
        this->rotation = this->camera->getRealOrientation();
    }
    else
    {
        this->position = Vector3::ZERO;
        this->rotation = Quaternion::IDENTITY;
    }
}

void FlyingCamera::SetUpAxis(UpAxis upAxis, bool setDefaultRotation)
{
    this->upAxis = upAxis;

    if (setDefaultRotation)
    {
        if (this->upAxis == UP_AXIS_Y)
            this->rotation = Quaternion(1, 0, 0, 0);
        else
            this->rotation = Quaternion((Real)0.707107, (Real)0.707107, (Real)0, (Real)0);
    }
}

const Vector3& FlyingCamera::GetPosition() const
{
    return this->position;
}

void FlyingCamera::SetPosition(const Vector3& position)
{
    this->position = position;
}

const Quaternion& FlyingCamera::GetRotation()
{
    return this->rotation;
}

void FlyingCamera::SetRotation(const Quaternion& rotation)
{
    this->rotation = rotation;
}

void FlyingCamera::SetCamera(Camera* camera, bool useParentNodeIfAvailable, bool copySettings)
{
    Node* node = camera->getParentNode();
    if (useParentNodeIfAvailable && node != 0)
        SetCameraNode(node, copySettings);
    else
    {
        this->camera = camera;
        this->cameraNode = 0;

        if (copySettings)
            Initialize();
    }
}

void FlyingCamera::SetCameraNode(Node* cameraNode, bool copySettings)
{
    this->cameraNode = cameraNode;
    this->camera = 0;

    if (copySettings)
        Initialize();
}

void FlyingCamera::Move(const Vector3& move, Real elapsedTime)
{
    if (move != Vector3::ZERO)
    {
        //Get camera directions
        Vector3 rightDirection, forwardDirection, upDirection;
        GetDirections(rightDirection, forwardDirection, upDirection);

        Vector3 offset = move * (this->moveSpeed * elapsedTime);

        //Move current position
        this->position +=
            rightDirection * offset.x +
            forwardDirection * offset.y +
            upDirection * offset.z;
    }
}

void FlyingCamera::Rotate(const Vector2& rotate)
{
    if (rotate != Vector2::ZERO)
    {
        Real tilt = rotate.y;
        Real turn = -rotate.x;

        //Create tilt/turn quaternions
        Quaternion tiltRotationQuat(Radian(tilt), Vector3::UNIT_X);
        Quaternion turnRotationQuat;
        if (this->upAxis == UP_AXIS_Y)
            turnRotationQuat = Quaternion(Radian(turn), Vector3::UNIT_Y);
        else
            turnRotationQuat = Quaternion(Radian(turn), Vector3::UNIT_Z);

        //Calculate new orientation, tilting first (in local space), then turning (in world space)
        this->rotation = turnRotationQuat * this->rotation * tiltRotationQuat;
    }
}

void FlyingCamera::UpdateCamera()
{
    if (this->cameraNode != 0)
    {
        this->cameraNode->setPosition(this->position);
        this->cameraNode->setOrientation(this->rotation);
    }
    else if (this->camera != 0)
    {
        this->camera->setPosition(this->position);
        this->camera->setOrientation(this->rotation);
    }
}

void FlyingCamera::GetDirections(Vector3& rightDirection, Vector3& forwardDirection, Vector3& upDirection)
{
    //Convert rotation to matrix
    Matrix3 rotationMatrix;
    this->rotation.ToRotationMatrix(rotationMatrix);

    //Get appropriate column vectors
    rightDirection = rotationMatrix.GetColumn(0);
    forwardDirection = -rotationMatrix.GetColumn(2);

    //The up direction is special since it is in world space
    if (this->upAxis == UP_AXIS_Y)
        upDirection = Vector3::UNIT_Y;
    else
        upDirection = Vector3::UNIT_Z;
}


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
#include "ApplicationUtilities.hpp"

using namespace Ogre;
using namespace OgreMax;


//Implementation---------------------------------------------------------------
void ApplicationUtilities::CopyCameraSettings(Camera* destinationCamera, const Camera* sourceCamera)
{
    destinationCamera->setPosition(sourceCamera->getRealPosition());
    destinationCamera->setOrientation(sourceCamera->getRealOrientation());
    destinationCamera->setNearClipDistance(sourceCamera->getNearClipDistance());
    destinationCamera->setFarClipDistance(sourceCamera->getFarClipDistance());
    destinationCamera->setProjectionType(sourceCamera->getProjectionType());
    destinationCamera->setFOVy(sourceCamera->getFOVy());
}

const RenderSystemList& ApplicationUtilities::GetRenderSystems()
{
#if OGRE_VERSION_MAJOR == 1 && OGRE_VERSION_MINOR < 7
    return *Root::getSingleton().getAvailableRenderers();
#else
    return Root::getSingleton().getAvailableRenderers();
#endif
}
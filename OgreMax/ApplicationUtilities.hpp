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


#ifndef OgreMax_ApplicationUtilities_INCLUDED
#define OgreMax_ApplicationUtilities_INCLUDED


//Includes---------------------------------------------------------------------
#include <OgreCamera.h>
#include <OgreString.h>
#include <OgrePixelFormat.h>
#include <OgreRoot.h>


//Classes----------------------------------------------------------------------
namespace OgreMax
{
    /**
     * Utility functions implemented in a way that's specific to how the
     * application operates
     */
    class ApplicationUtilities
    {
    public:
        /** Copies the relevant camera settings from one camera to another */
        static void CopyCameraSettings
            (
            Ogre::Camera* destinationCamera,
            const Ogre::Camera* sourceCamera
            );

        static const Ogre::RenderSystemList& GetRenderSystems();
    };
}

#endif
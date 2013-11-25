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


#ifndef OgreMax_ViewerConstants_INCLUDED
#define OgreMax_ViewerConstants_INCLUDED


//Includes---------------------------------------------------------------------
#include <OgreString.h>


//Classes----------------------------------------------------------------------
namespace OgreMax
{
    class ViewerConstants
    {
    public:
        static const Ogre::String USER_CONTROLS_FILE_NAME;
        static const Ogre::String MESH_FILE_EXTENSION;
        static const Ogre::String LUA_SCRIPT_MODULE_NAME;
        static const Ogre::String LUA_SCRIPT_ACTIVATING_FUNCTION;
        static const Ogre::String LUA_SCRIPT_DEACTIVATING_FUNCTION;
        static const Ogre::String LUA_SCRIPT_UPDATE_FUNCTION;
    };
}

#endif
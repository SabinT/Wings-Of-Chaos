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


#ifndef OgreMax_ApplicationLuaTypes_INCLUDED
#define OgreMax_ApplicationLuaTypes_INCLUDED

#ifndef OGREMAX_VIEWER_NO_LUA


//Includes---------------------------------------------------------------------
#include "LuaScript.hpp"


//Classes----------------------------------------------------------------------
namespace OgreMax
{
    class LuaScript;

    class ApplicationLuaTypes
    {
    public:
        static void Register(LuaScript& script, const Ogre::String& moduleName);
    };

}

#endif

#endif
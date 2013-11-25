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
#include "OgrePlugins.hpp"
#include <OgreRoot.h>
#ifdef OGRE_STATIC_LIB
    #include <OgreCgPlugin.h>
    #include <OgreParticleFXPlugin.h>
    #include <OgreOctreePlugin.h>
    #include <OgreGLPlugin.h>
    #if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
        #include <OgreD3D9Plugin.h>
    #endif
#endif

using namespace Ogre;
using namespace OgreMax;


//Implementation---------------------------------------------------------------
OgrePlugins::~OgrePlugins()
{
    Destroy();
}

void OgrePlugins::Create()
{
#ifdef OGRE_STATIC_LIB
    #if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
        InstallStaticPlugin(new D3D9Plugin);
    #endif

    InstallStaticPlugin(new GLPlugin);
    InstallStaticPlugin(new CgPlugin);
    InstallStaticPlugin(new ParticleFXPlugin);
    InstallStaticPlugin(new OctreePlugin);
#endif
}

void OgrePlugins::Destroy()
{
#ifdef OGRE_STATIC_LIB
    for (Plugins::reverse_iterator pluginIterator = this->plugins.rbegin();
        pluginIterator != this->plugins.rend();
        ++pluginIterator)
    {
        delete *pluginIterator;
    }
    this->plugins.clear();
#endif
}

void OgrePlugins::InstallStaticPlugin(Ogre::Plugin* plugin)
{
    Root::getSingleton().installPlugin(plugin);
    this->plugins.push_back(plugin);
}
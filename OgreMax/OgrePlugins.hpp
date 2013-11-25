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


#ifndef OgreMax_OgrePlugins_INCLUDED
#define OgreMax_OgrePlugins_INCLUDED


//Includes---------------------------------------------------------------------
#include <OgrePlugin.h>


//Classes----------------------------------------------------------------------
namespace OgreMax
{
    /** Manages the creation and destruction of Ogre plugins */
    class OgrePlugins
    {
    public:
        ~OgrePlugins();

        void Create();
        void Destroy();

    private:
        void InstallStaticPlugin(Ogre::Plugin* plugin);

    private:
        typedef std::list<Ogre::Plugin*> Plugins;
        Plugins plugins;
    };
}

#endif
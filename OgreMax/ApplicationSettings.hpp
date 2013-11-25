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


#ifndef OgreMax_ApplicationSettings_INCLUDED
#define OgreMax_ApplicationSettings_INCLUDED


//Includes---------------------------------------------------------------------
#include "CommandLineArgs.hpp"
#include "OgreMaxTypes.hpp"


//Classes----------------------------------------------------------------------
namespace OgreMax
{
    class ApplicationSettings
    {
    public:
        ApplicationSettings();
        ApplicationSettings(const CommandLineArgs& commandLineArgs);
        
        /** Called when the command line has been parsed and the settings should be initialized */
        virtual void Initialize(const CommandLineArgs& commandLine);
        
        Ogre::RenderSystem* GetConfiguredRenderSystem() const;
        Ogre::RenderSystem* ConfigureRenderSystem(Ogre::RenderSystem* renderSystem = 0);

        enum RenderingSubsystem
        {
            DIRECT3D,
            OPENGL
        };

        /** Sets the currently configured rendering subsystem*/
        void SetRenderingSubsystem(RenderingSubsystem subsystem);

    protected:
        /**
         * Determines if te specified rendering subsystem, given by name,
         * matches the currently configured subsystem. This method takes into
         * account the current platform
         */
        bool IsConfiguredRenderingSubsystem(const Ogre::String& name) const;

        RenderingSubsystem GetAllowedRenderingSubsystem() const;

    protected:
        Ogre::String renderingSubsystemName;

        /** The selected rendering subsystem */
        RenderingSubsystem renderingSubsystem;

    public:
        bool fullScreen;
        bool vsync;
        bool enableNvidiaPerformanceHUD;
        int screenWidth;
        int screenHeight;
        int fullScreenRefreshRate;

        /** Empty if none specified. If not empty, will always have a trailing path separator */
        Ogre::String pluginsDirectory;

        /** Base file name for screenshots. Will be empty if none are specified */
        Ogre::String screenshotBaseName;

        /** Screenshot file name extension. Includes the leading '.' */
        Ogre::String screenshotExtension;

        Ogre::String fileName;
        Ogre::String resourcesConfigurationFileName;
        Ogre::String pluginsConfigurationFileName;

        struct ResourceLocation
        {
            ResourceLocation()
            {
                this->recursive = false;
            }

            Ogre::String type;
            Ogre::String name;
            bool recursive;
        };
        typedef std::list<ResourceLocation> ResourceLocations;
        ResourceLocations resourceLocations;

        bool lockViewCameraToSceneCamera;
        bool writeUserControlsFile;
        bool keepLogFile;

        Ogre::Real cameraMoveSpeed;

        /** Scene up axis. Used only if a mesh is being viewed */
        Types::UpAxis upAxis;

        /**
         * Indicates whether scene information should be displayed
         * Used by the OgreMax WinViewer to determine whether tree view is shown/hidden on startup
         */
        bool showInfo;

        /**
         * Indicates whether a file dialog should be presented if a file was passed in on startup.
         * Used by the OgreMax WinViewer to show the "Open File" dialog on startup when a scene or mesh file
         * is specified as a startup setting.
         */
        bool useStartupFileDialog;
    };

}

#endif
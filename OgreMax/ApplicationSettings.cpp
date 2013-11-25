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
#include "ApplicationSettings.hpp"
#include "ApplicationUtilities.hpp"
#include "OgreMaxUtilities.hpp"
#include <OgreConfigFile.h>
#include <OgreRoot.h>
#include <OgreRenderSystem.h>

using namespace OgreMax;
using namespace OgreMax::Types;
using namespace Ogre;


//Implementation---------------------------------------------------------------
ApplicationSettings::ApplicationSettings()
{
}

ApplicationSettings::ApplicationSettings(const CommandLineArgs& commandLineArgs)
{
    Initialize(commandLineArgs);
}

void ApplicationSettings::Initialize(const CommandLineArgs& commandLine)
{
    this->fullScreen = false;
    this->vsync = false;
    this->enableNvidiaPerformanceHUD = false;
    this->screenWidth = 0;
    this->screenHeight = 0;
    this->fullScreenRefreshRate = 60;
    this->lockViewCameraToSceneCamera = false;
    this->writeUserControlsFile = false;
    this->keepLogFile = false;
    this->cameraMoveSpeed = 100;
    this->upAxis = UP_AXIS_Y;
    this->showInfo = true;
    this->useStartupFileDialog = true;

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    this->renderingSubsystem = DIRECT3D;
#else
    this->renderingSubsystem = OPENGL;
#endif

    if (commandLine.GetLength() > 0)
    {
        String argLower;
        for (int index = 0; index < commandLine.GetLength(); index++)
        {
            argLower = commandLine[index];
            StringUtil::toLowerCase(argLower);

            if (argLower == "-rendersystem" && index < commandLine.GetLength() - 1)
            {
                this->renderingSubsystemName = commandLine[index + 1];
                index++;
            }
            else if (argLower == "-direct3d" || argLower == "-d3d")
                this->renderingSubsystem = DIRECT3D;
            else if (argLower == "-opengl" || argLower == "-ogl")
                this->renderingSubsystem = OPENGL;
            else if (argLower == "-fullscreen")
                this->fullScreen = true;
            else if (argLower == "-vsync")
                this->vsync = true;
            else if (argLower == "-width" && index < commandLine.GetLength() - 1)
            {
                this->screenWidth = StringConverter::parseInt(commandLine[index + 1]);
                index++;
            }
            else if (argLower == "-height" && index < commandLine.GetLength() - 1)
            {
                this->screenHeight = StringConverter::parseInt(commandLine[index + 1]);
                index++;
            }
            else if (argLower == "-plugins" && index < commandLine.GetLength() - 1)
            {
                this->pluginsDirectory = commandLine[index + 1];
                OgreMaxUtilities::EnsureTrailingPathSeparator(this->pluginsDirectory);
                index++;
            }
            else if (argLower == "-screenshotbasename" && index < commandLine.GetLength() - 1)
            {
                this->screenshotBaseName = commandLine[index + 1];
                index++;
            }
            else if (argLower == "-screenshotextension" && index < commandLine.GetLength() - 1)
            {
                const String& ext = commandLine[index + 1];
                if (!ext.empty())
                {
                    //Make sure extension starts with a '.'
                    if (ext[0] == '.')
                        this->screenshotExtension = ext;
                    else
                    {
                        this->screenshotExtension = '.';
                        this->screenshotExtension += ext;
                    }
                }
                index++;
            }
            else if ((argLower == "-r" || argLower == "-resourcesconfigfile") && index < commandLine.GetLength() - 1)
            {
                this->resourcesConfigurationFileName = commandLine[index + 1];
                OgreMaxUtilities::MakeFullPath(this->resourcesConfigurationFileName);
                index++;
            }
            else if ((argLower == "-p" || argLower == "-pluginsconfigfile") && index < commandLine.GetLength() - 1)
            {
                this->pluginsConfigurationFileName = commandLine[index + 1];
                OgreMaxUtilities::MakeFullPath(this->pluginsConfigurationFileName);
                index++;
            }
            else if ((argLower == "-z" || argLower == "-zip") && index < commandLine.GetLength() - 1)
            {
                ResourceLocation resourceLocation;
                resourceLocation.type = "Zip";
                resourceLocation.name = commandLine[index + 1];
                OgreMaxUtilities::MakeFullPath(resourceLocation.name);
                this->resourceLocations.push_back(resourceLocation);
                index++;
            }
            else if ((argLower == "-d" || argLower == "-dir" || argLower == "-directory") && index < commandLine.GetLength() - 1)
            {
                ResourceLocation resourceLocation;
                resourceLocation.type = "FileSystem";
                resourceLocation.name = commandLine[index + 1];
                OgreMaxUtilities::MakeFullPath(resourceLocation.name);
                OgreMaxUtilities::EnsureTrailingPathSeparator(resourceLocation.name);
                this->resourceLocations.push_back(resourceLocation);
                index++;
            }
            else if ((argLower == "-rd" || argLower == "-rdir" || argLower == "-rdirectory") && index < commandLine.GetLength() - 1)
            {
                ResourceLocation resourceLocation;
                resourceLocation.type = "FileSystem";
                resourceLocation.name = commandLine[index + 1];
                OgreMaxUtilities::MakeFullPath(resourceLocation.name);
                OgreMaxUtilities::EnsureTrailingPathSeparator(resourceLocation.name);
                resourceLocation.recursive = true;
                this->resourceLocations.push_back(resourceLocation);
                index++;
            }
            else if (argLower == "-lockviewcamera")
                this->lockViewCameraToSceneCamera = true;
            else if (argLower == "-writeusercontrolsfile")
                this->writeUserControlsFile = true;
            else if (argLower == "-keeplogfile")
                this->keepLogFile = true;
            else if (argLower == "-cameramovespeed" && index < commandLine.GetLength() - 1)
            {
                this->cameraMoveSpeed = StringConverter::parseReal(commandLine[index + 1]);
                index++;
            }
            else if (argLower == "-zup")
                this->upAxis = UP_AXIS_Z;
            else if (argLower == "-nvperfhud")
                this->enableNvidiaPerformanceHUD = true;
            else if (argLower == "-noinfo")
                this->showInfo = false;
            else if (argLower == "-nostartupfiledialog")
                this->useStartupFileDialog = false;
            else if (index == commandLine.GetLength() - 1)
                this->fileName = commandLine[index];
        }
    }

    if (this->screenWidth == 0 || this->screenHeight == 0)
    {
        this->screenWidth = 800;
        this->screenHeight = 600;
    }

    if (this->screenshotBaseName.empty())
        this->screenshotBaseName = "Screenshot";
    if (this->screenshotExtension.empty())
        this->screenshotExtension = ".jpg";
}

ApplicationSettings::RenderingSubsystem ApplicationSettings::GetAllowedRenderingSubsystem() const
{
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    return this->renderingSubsystem;
#else
    return OPENGL;
#endif
}

void ApplicationSettings::SetRenderingSubsystem(RenderingSubsystem subsystem)
{
    this->renderingSubsystem = subsystem;
}

bool ApplicationSettings::IsConfiguredRenderingSubsystem(const String& name) const
{
    if (!this->renderingSubsystemName.empty())
        return name == renderingSubsystemName;
    else
    {
        return
            (GetAllowedRenderingSubsystem() == DIRECT3D && name.find("Direct3D") != String::npos) ||
            (GetAllowedRenderingSubsystem() == OPENGL && name.find("OpenGL") != String::npos);
    }
}

RenderSystem* ApplicationSettings::GetConfiguredRenderSystem() const
{
    const RenderSystemList& renderSystems = ApplicationUtilities::GetRenderSystems();
    for (RenderSystemList::const_iterator renderSystem = renderSystems.begin();
        renderSystem != renderSystems.end();
        ++renderSystem)
    {
        const String& name = (*renderSystem)->getName();
        if (IsConfiguredRenderingSubsystem(name))
            return *renderSystem;
    }

    return 0;
}

RenderSystem* ApplicationSettings::ConfigureRenderSystem(RenderSystem* renderSystem)
{
    const size_t bitDepth = 32;

    //Set the rendering system
    if (renderSystem == 0)
        renderSystem = GetConfiguredRenderSystem();
	if (renderSystem == 0)
        renderSystem = ApplicationUtilities::GetRenderSystems().front();
    Root::getSingleton().setRenderSystem(renderSystem);

    //Configure the render system
    ConfigOptionMap configOptions = renderSystem->getConfigOptions();
    for (ConfigOptionMap::iterator configItem = configOptions.begin();
        configItem != configOptions.end();
        ++configItem)
    {
        String name = configItem->first;
        if (name == "Video Mode")
        {
            StringUtil::StrStreamType videoMode;
            videoMode
                << this->screenWidth << " x " << this->screenHeight
                << " @ " << bitDepth << "-bit colour" ;

            String value = videoMode.str();
            renderSystem->setConfigOption(name, value);
        }
        else if (name == "Colour Depth")
        {
            String value = StringConverter::toString(bitDepth);
            renderSystem->setConfigOption(name, value);
        }
        else if (name == "Full Screen")
        {
            renderSystem->setConfigOption(name, this->fullScreen ? "Yes" : "No");
        }
        else if (name == "VSync")
        {
            renderSystem->setConfigOption(name, this->vsync ? "Yes" : "No");
        }
        else if (name == "Display Frequency")
        {
            if (this->fullScreen)
            {
                String value = StringConverter::toString(this->fullScreenRefreshRate);
                renderSystem->setConfigOption(name, value);
            }
        }
        else if (name == "Allow NVPerfHUD")
        {
            renderSystem->setConfigOption(name, this->enableNvidiaPerformanceHUD ? "Yes" : "No");
        }
    }

    String message = renderSystem->validateConfigOptions();
    if (!message.empty())
    {
        StringUtil::StrStreamType errorMessage;
        errorMessage << "Unable to validate configuration options: " << message;

        OGRE_EXCEPT
            (
            Exception::ERR_INVALIDPARAMS,
	        errorMessage.str(),
	        "ApplicationSettings::ConfigureRenderSystem"
            );
    }

    return renderSystem;
}

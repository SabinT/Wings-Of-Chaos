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
#ifndef OGREMAX_VIEWER_NO_LUA

#include "LuaScript.hpp"
#include <OgreLogManager.h>
#include <OgreResourceGroupManager.h>
#include <luabind/luabind.hpp>

using namespace Ogre;
using namespace OgreMax;


//Implementation---------------------------------------------------------------
LuaScript::LuaScript()
{
    this->L = 0;
    this->defaultResourceGroupName = ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME;

    Initialize();
}

LuaScript::~LuaScript()
{
    Shutdown();
}

void LuaScript::Initialize()
{
    //Exit early if script has already been initialized
    if (this->L != 0)
        return;

    using namespace luabind;

    //Open Lua
    this->L = lua_open();

    //Note that this opens all of the libraries, including some that are "unsafe"
    //since they allow he scripts to access the local file system
    luaL_openlibs(this->L);

    //Set up Luabind error callback
    set_pcall_callback(AddFileAndLineNumber);

    //Open Luabind
    open(L);

    //Set some common bindings
    module(this->L)
	[
		def("PrintLog", &PrintLog),
        def("PrintDebug", &PrintLog),
        class_<LuaScript>("Script")
            .def("LoadModule", &LuaScript::LoadLuaModule)
    ];

    //Set the script object as a global
    luabind::object globalTable = luabind::globals(this->L);
    globalTable["script"] = this;

    //Perform some final setup on the script state
    ExecuteString
        (
        //Set up a custom loader
        "package.loaders[2] = function(name) return script:LoadModule(name) end\n"

        //Disable the DLL and all-in-one package loaders
        "package.loaders[3] = nil\n"
        "package.loaders[4] = nil\n"

        //Disable loadlib
        "package.loadlib = function() end\n"

        //Disable I/O functionality
        "for k, v in pairs(io) do\n"
            "if type(v) == \"function\" then\n"
                "io[k] = function() end\n"
            "end\n"
        "end\n"

        //Disable some OS functionality
        "local osOriginal = os\n"

        "os = \n"
        "{\n"
            "date = os.date,\n"
            "time = os.time, \n"
            "setlocale = os.setlocale,\n"
            "clock = os.clock, \n"
            "difftime = os.difftime,\n"
        "}\n"

        "for k, v in pairs(osOriginal) do\n"
            "if not os[k] and type(v) == \"function\" then\n"
                "os[k] = function() end\n"
            "end\n"
        "end\n"

        //Customize the print function
        "function print(...)\n"
        "   local result = \"\"\n"
        "   for i, v in ipairs{...} do\n"
        "       result = result .. v .. ' '\n"
        "   end\n"
        "   PrintLog('Script message: ' .. result)\n"
        "end\n"
        );
}

void LuaScript::Shutdown()
{
    if (this->L != 0)
    {
        lua_close(this->L);
        this->L = 0;
    }
}

LuaScript::operator lua_State* ()
{
    return this->L;
}

const String& LuaScript::GetDefaultResourceGroupName() const
{
    return this->defaultResourceGroupName;
}

void LuaScript::SetDefaultResourceGroupName(const Ogre::String& name)
{
    this->defaultResourceGroupName = name;
}

bool LuaScript::ExecuteFile(const String& fileName)
{
    //Load the file and execute it
    bool returnValue = false;
    String text;
    if (LoadTextFile(fileName, text, this->defaultResourceGroupName))
    {
        //Load the text into the Lua state and then execute it
        if (luaL_loadbuffer(L, text.c_str(), text.size(), fileName.c_str()) ||
            lua_pcall(L, 0, 0, 0))
        {
            //An error occurred. Log it
            const char* errorMessage = lua_tostring(L, -1);
            LogManager::getSingleton().logMessage(errorMessage);

            //Remove the error message from the stack
            lua_pop(L, 1);
        }
        else
        {
            //Success
            returnValue = true;
        }
    }

    return returnValue;
}

bool LuaScript::ExecuteString(const char* str)
{
    //Load the string and then execute it.

    //The "string" parameter doesn't matter. It can be called anything

	if (luaL_loadbuffer(L, str, std::strlen(str), "string") ||
        lua_pcall(L, 0, 0, 0))
	{
        //An error occurred. Log it
        const char* errorMessage = lua_tostring(L, -1);
        LogManager::getSingleton().logMessage(errorMessage);

        //Remove the error message from the stack
        lua_pop(L, 1);

		return false;
	}

	return true;
}

bool LuaScript::HasGlobal(const char* name, int luaType)
{
    return HasGlobal(String(name), luaType);
}

bool LuaScript::HasGlobal(const String& name, int luaType)
{
    luabind::object globalTable = luabind::globals(this->L);
    luabind::object global = luabind::rawget(globalTable, name);
    int globalType = luabind::type(global);
    return (luaType > LUA_TNIL) ? globalType == luaType : globalType > LUA_TNIL;
}

int LuaScript::LoadLuaModule(const String& moduleName)
{
    //Turn the module name into a file name
    String fileName = moduleName + ".lua";

    //Get the resource group name for the file
    String resourceGroupName = this->defaultResourceGroupName;

    //Load the file
    int returnValue = 0;
    String text;
    if (LoadTextFile(fileName, text, resourceGroupName))
    {
        if (luaL_loadbuffer(this->L, text.c_str(), text.size(), moduleName.c_str()))
        {
            //Error
        }
        else
        {
            //Success
            returnValue = 1;
        }
    }

    return returnValue;
}

int LuaScript::AddFileAndLineNumber(lua_State* L)
{
    lua_Debug d;
    lua_getstack(L, 1, &d);
    lua_getinfo(L, "Sln", &d);
    std::string err = lua_tostring(L, -1);
    lua_pop(L, 1);
    std::stringstream msg;
    msg << "Script error: " << d.short_src << ":" << d.currentline;

    if (d.name != 0)
        msg << " (" << d.namewhat << " " << d.name << ")";

    msg << " " << err;
    lua_pushstring(L, msg.str().c_str());

    return 1;
}

void LuaScript::PrintLog(const char* text)
{
    //Prints the specified text to the log
    if (LogManager::getSingletonPtr() != 0)
        LogManager::getSingleton().logMessage(text);
}

bool LuaScript::LoadTextFile
    (
    const String& fileName,
    String& text,
    const String& resourceGroupName
    )
{
    //Open the file
    try
    {
        DataStreamPtr stream = ResourceGroupManager::getSingleton().openResource(fileName, resourceGroupName);
        if (stream.isNull())
            return false;

        //Get the text file contents
        text = stream->getAsString();
        return true;
    }
    catch (...)
    {
        return false;
    }
}

#endif
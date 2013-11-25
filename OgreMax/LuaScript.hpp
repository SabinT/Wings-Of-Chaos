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


#ifndef OgreMax_LuaScript_INCLUDED
#define OgreMax_LuaScript_INCLUDED

#ifndef OGREMAX_VIEWER_NO_LUA


//Includes---------------------------------------------------------------------
#include <OgreString.h>
extern "C"
{
    #include <lua.h>
    #include <lauxlib.h>
    #include <lualib.h>
}


//Classes----------------------------------------------------------------------
namespace OgreMax
{
    /**
     * Manages the initialization of a Lua script, and provides some runtime
     * facilities such as module loading, core Ogre math type export, and
     * of course script text and file execution
     */
    class LuaScript
    {
    public:
        LuaScript();

        virtual ~LuaScript();

        /**
         * Initializes the script state
         * This is called automatically when the LuaScript is created
         */
        void Initialize();

        /** Shuts down the script state */
        void Shutdown();

        /** Gets the internal lua_State */
        operator lua_State* ();

        /** Gets the default resource group name. This is used to locate script files */
        const Ogre::String& GetDefaultResourceGroupName() const;

        /** Sets the default resource group name. This is used to locate script files */
        void SetDefaultResourceGroupName(const Ogre::String& name);

        /**
         * Loads the specified script file into the current state and runs it
         * The file must be located in the script's default resource group
         */
        bool ExecuteFile(const Ogre::String& fileName);

        /** Loads the specified script text into the current state and runs it */
        bool ExecuteString(const char* str);

        /**
         * Determines whether the Lua script contains the specified global
         * @param name [in] - The name of the global to search for
         * @param luaType [in] - Optional Lua type that the global, if present, must match
         */
        bool HasGlobal(const char* name, int luaType = LUA_TNONE);

        /**
         * Determines whether the Lua script contains the specified global
         * @param name [in] - The name of the global to search for
         * @param luaType [in] - Optional Lua type that the global, if present, must match
         */
        bool HasGlobal(const Ogre::String& name, int luaType = LUA_TNONE);

    private:
        /**
         * Loads the specified Lua module. This is called by Lua code
         */
        int LoadLuaModule(const Ogre::String& moduleName);

        /** Lua error callback that inserts additional debugging information into the error string */
        static int AddFileAndLineNumber(lua_State* L);

        /** Prints the specified text to the global log */
        static void PrintLog(const char* text);

        /**
         * Loads the specified file name from the configured Ogre resources
         * @param fileName [in] - The file name
         * @param text [out] - The text loaded from the file
         * @param resourceGroupName [in] - The resource group name
         * @return Returns true if the file was loaded, false otherwise
         */
        static bool LoadTextFile
            (
            const Ogre::String& fileName,
            Ogre::String& text,
            const Ogre::String& resourceGroupName
            );

    protected:
        /** The script state */
        lua_State* L;

        /** The default resource group name */
        Ogre::String defaultResourceGroupName;
    };

}

#endif

#endif
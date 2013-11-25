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


#ifndef OgreMax_CommandLineArgs_INCLUDED
#define OgreMax_CommandLineArgs_INCLUDED


//Includes---------------------------------------------------------------------
#include <OgreString.h>


//Classes----------------------------------------------------------------------
namespace OgreMax
{
    /**
     * Command line argument parser/container.
     * This class unifies the command line parsing regardless of platform being
     * used, whether it's Windows with it's single string command line, or
     * other platforms with the argc/argv commands.
     */
    class CommandLineArgs
    {
    public:
	    /** Initializes with a Windows-style command line string */
	    CommandLineArgs(const Ogre::String& commandLine);

        /**
         * Initializes with an argc/argv command line string array
         * @param argc [in] - The number of arguments in argv
         * @param argv [in] - Array of arguments
         * @param ignoreFirst [in] - Indicates whether the first argument in argv should
         * be ignored. Since the first argument is the program name, this will usually be
         * true, which is the default value.
         */
        CommandLineArgs(int argc, char* argv[], bool ignoreFirst = true);
	    virtual ~CommandLineArgs();

        /** Gets the number of parsed command line arguments */
        int GetLength() const;

        /** Gets the specified argument */
        const Ogre::String& GetArg(int index) const;

        /** Gets the specified argument */
        const Ogre::String& operator [] (int index) const;

    protected:
        void Parse(const Ogre::String& commandLine);

	    static bool IsQuote(char c);

    protected:
        std::vector<Ogre::String> args;
    };

}


#endif

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
#include "CommandLineArgs.hpp"

using namespace Ogre;
using namespace OgreMax;


//Implementation---------------------------------------------------------------
CommandLineArgs::CommandLineArgs(const String& commandLine)
{
	Parse(commandLine);
}

CommandLineArgs::CommandLineArgs(int argc, char* argv[], bool ignoreFirst)
{
    this->args.reserve(argc);

    for (int index = ignoreFirst ? 1 : 0; index < argc; index++)
        this->args.push_back(argv[index]);
}

CommandLineArgs::~CommandLineArgs()
{
}

int CommandLineArgs::GetLength() const
{
	return (int)this->args.size();
}

const String& CommandLineArgs::GetArg(int index) const
{
	return this->args[index];
}

const String& CommandLineArgs::operator [] (int index) const
{
    return this->args[index];
}

void CommandLineArgs::Parse(const String& commandLine)
{
    //Clear old args
	this->args.clear();
    this->args.reserve(16);

    //Trim the command line
	String trimmedCommandLine(commandLine);
    StringUtil::trim(trimmedCommandLine);

    //If the command line is empty we're done
    if (trimmedCommandLine.empty())
		return;

    //The command line is not empty, so parse it
	size_t offset = 0;
    String arg;
    size_t trimmedCommandLineLength = trimmedCommandLine.length();
    while (offset < trimmedCommandLineLength)
	{
        arg.clear();

		char c = trimmedCommandLine[offset];
        if (IsQuote(c))
		{
			//A quoted parameter

            size_t nextQuoteFoundAt = trimmedCommandLine.find(c, offset + 1);
			if (nextQuoteFoundAt == String::npos)
			{
				//No closing quote. Take the rest of the commmand line beyond the opening quote
				arg = trimmedCommandLine.substr(offset + 1);
				offset = trimmedCommandLineLength;
			}
			else
			{
				//Has a closing quote. Take the substring inside of the quotes
				arg = trimmedCommandLine.substr(offset + 1, nextQuoteFoundAt - offset - 1);
				offset = nextQuoteFoundAt + 1;
			}

            this->args.push_back(arg);
		}
		else if (!isspace(c))
		{
			//An unquoted parameter

            size_t spaceFoundAt = trimmedCommandLine.find_first_of(' ', offset + 1);
            if (spaceFoundAt == String::npos)
            {
                //Reached end of string. Take the rest of the command line
                arg = trimmedCommandLine.substr(offset);
                offset = trimmedCommandLineLength;
            }
            else
            {
                //Reached the next space character
				arg = trimmedCommandLine.substr(offset, spaceFoundAt - offset);
				offset = spaceFoundAt + 1;
            }

            this->args.push_back(arg);
		}
		else
		{
			//Nothing was found. Move to next character
			offset++;
		}
	}
}

bool CommandLineArgs::IsQuote(char c)
{
	return c == '\"';
}


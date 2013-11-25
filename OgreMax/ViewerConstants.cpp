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
#include "ViewerConstants.hpp"

using namespace Ogre;
using namespace OgreMax;


//Static initialization--------------------------------------------------------
const String ViewerConstants::USER_CONTROLS_FILE_NAME("UserControls.xml");
const String ViewerConstants::MESH_FILE_EXTENSION(".mesh");
const String ViewerConstants::LUA_SCRIPT_MODULE_NAME("viewer");
const String ViewerConstants::LUA_SCRIPT_ACTIVATING_FUNCTION("Activating");
const String ViewerConstants::LUA_SCRIPT_DEACTIVATING_FUNCTION("Deactivating");
const String ViewerConstants::LUA_SCRIPT_UPDATE_FUNCTION("Update");
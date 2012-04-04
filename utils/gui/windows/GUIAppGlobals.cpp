/****************************************************************************/
/// @file    GUIAppGlobals.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 29.05.2005
/// @version $Id: GUIAppGlobals.cpp 8236 2010-02-10 11:16:41Z behrisch $
///
//
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================




#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "GUIAppGlobals.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS



// ===========================================================================
// variable definitions
// ===========================================================================

FXApp *gFXApp = 0;



/****************************************************************************/


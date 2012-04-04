/****************************************************************************/
/// @file    MSMoveReminder.cpp
/// @author  Daniel Krajzewicz
/// @date    2008-10-27
/// @version $Id: MSMoveReminder.cpp 8614 2010-04-20 21:34:20Z behrisch $
///
// Something on a lane to be noticed about vehicle movement
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

#include <string>
#include <cassert>
#include "MSLane.h"
#include "MSMoveReminder.h"


// ===========================================================================
// method definitions
// ===========================================================================
MSMoveReminder::MSMoveReminder(MSLane * const lane, const bool doAdd) throw()
        : myLane(lane) {
    if (myLane!=0 && doAdd) {
        // add reminder to lane
        myLane->addMoveReminder(this);
    }
}


/****************************************************************************/


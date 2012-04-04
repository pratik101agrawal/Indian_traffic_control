/****************************************************************************/
/// @file    MSJunctionControl.cpp
/// @author  Christian Roessel
/// @date    Tue, 06 Mar 2001
/// @version $Id: MSJunctionControl.cpp 8735 2010-05-06 09:13:40Z behrisch $
///
// Container for junctions; performs operations on all stored junctions
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

#include "MSJunctionControl.h"
#include "MSJunction.h"
#include <algorithm>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// member method definitions
// ===========================================================================
MSJunctionControl::MSJunctionControl() throw() {
}


MSJunctionControl::~MSJunctionControl() throw() {
}


void
MSJunctionControl::postloadInitContainer() throw(ProcessError) {
    const std::vector<MSJunction*> &junctions = buildAndGetStaticVector();
    for (std::vector<MSJunction*>::const_iterator i=junctions.begin(); i!=junctions.end(); ++i) {
        (*i)->postloadInit();
    }
}


void
MSJunctionControl::resetRequests() throw() {
    const std::vector<MSJunction*> &junctions = buildAndGetStaticVector();
    std::for_each(junctions.begin(), junctions.end(), std::mem_fun(& MSJunction::clearRequests));
}


/****************************************************************************/


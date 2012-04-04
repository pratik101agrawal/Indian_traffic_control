/****************************************************************************/
/// @file    MSGlobals.h
/// @author  Daniel Krajzewicz
/// @date    late summer 2003
/// @version $Id: MSGlobals.h 8724 2010-05-04 20:11:23Z behrisch $
///
// Some static variables for faster access
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
#ifndef MSGlobals_h
#define MSGlobals_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/common/SUMOTime.h>
#include <cstddef>


// ===========================================================================
// class declarations
// ===========================================================================
class MELoop;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSGlobals
 * This class holds some static variables, filled mostly with values coming
 *  from the command line or the simulation configuration file.
 * They are stored herein to allow a faster access than from the options
 *  container.
 */
class MSGlobals {
public:
    /// Information whether empty edges shall be written on dump
    static bool gOmitEmptyEdgesOnDump;

    /** Information how long the simulation shall wait until it recognizes
        a vehicle as a grid lock participant */
    static SUMOTime gTimeToGridlock;

    /// Information whether the simulation regards internal lanes
    static bool gUsingInternalLanes;

    /** information whether the network shall check for collisions */
    static bool gCheck4Accidents;

#ifdef HAVE_MESOSIM
    /// Information whether a state has been loaded
    static bool gStateLoaded;
#endif

#ifdef HAVE_MESOSIM
    /** Information whether mesosim shall be used */
    static bool gUseMesoSim;
    static MELoop *gMesoNet;

#endif

};


#endif

/****************************************************************************/


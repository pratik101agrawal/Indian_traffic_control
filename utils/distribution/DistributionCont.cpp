/****************************************************************************/
/// @file    DistributionCont.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id: DistributionCont.cpp 8367 2010-03-03 22:31:43Z behrisch $
///
// A container for distributions
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

#include "DistributionCont.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static variable definitions
// ===========================================================================
DistributionCont::TypedDistDict DistributionCont::myDict;


// ===========================================================================
// method definitions
// ===========================================================================
bool
DistributionCont::dictionary(const std::string &type, const std::string &id,
                             Distribution *d) {
    TypedDistDict::iterator i=myDict.find(type);

    if (i==myDict.end()) {
        myDict[type][id] = d;
        return true;
    }
    DistDict &dict = (*i).second;
    DistDict::iterator j=dict.find(id);
    if (j==dict.end()) {
        myDict[type][id] = d;
        return true;
    }
    return false;
}


Distribution *
DistributionCont::dictionary(const std::string &type,
                             const std::string &id) {
    TypedDistDict::iterator i=myDict.find(type);
    if (i==myDict.end()) {
        return 0;
    }
    DistDict &dict = (*i).second;
    DistDict::iterator j=dict.find(id);
    if (j==dict.end()) {
        return 0;
    }
    return (*j).second;
}



/****************************************************************************/


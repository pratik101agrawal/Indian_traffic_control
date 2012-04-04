/****************************************************************************/
/// @file    MSTrigger.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id: MSTrigger.h 8236 2010-02-10 11:16:41Z behrisch $
///
// An abstract device that changes the state of the micro simulation
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
#ifndef MSTrigger_h
#define MSTrigger_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/common/Named.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSTrigger
 * @brief An abstract device that changes the state of the micro simulation
 *
 * We name most of the additional microsim-structures "trigger" in order to
 *  allow some common operation on them.
 */
class MSTrigger : public Named {
public:
    /** @brief Constructor
     *
     * @param[in] id The id of the trigger
     */
    MSTrigger(const std::string &id) throw() : Named(id) { }


    /// @brief Destructor
    virtual ~MSTrigger() throw() { }


private:
    /// @brief Invalidated copy constructor.
    MSTrigger(const MSTrigger&);

    /// @brief Invalidated assignment operator.
    MSTrigger& operator=(const MSTrigger&);


};


#endif

/****************************************************************************/


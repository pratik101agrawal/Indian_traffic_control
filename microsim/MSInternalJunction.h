/****************************************************************************/
/// @file    MSInternalJunction.h
/// @author  Christian Roessel
/// @date    Wed, 12 Dez 2001
/// @version $Id: MSInternalJunction.h 8735 2010-05-06 09:13:40Z behrisch $
///
// junction.
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
#ifndef MSInternalJunction_h
#define MSInternalJunction_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MSLogicJunction.h"
#include <bitset>
#include <vector>
#include <string>


// ===========================================================================
// class declarations
// ===========================================================================
class MSLane;
class MSJunctionLogic;
class MSLink;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSInternalJunction
 * A class which realises junctions that do regard any kind of a right-of-way.
 * The rules for the right-of-way themselves are stored within the associated
 * "MSJunctionLogic" - structure.
 */
#ifdef HAVE_INTERNAL_LANES
class MSInternalJunction : public MSLogicJunction {
public:
    /** @brief Constructor
     * @param[in] id The id of the junction
     * @param[in] position The position of the junction
     * @param[in] shape The shape of the junction
     * @param[in] incoming The incoming lanes
     * @param[in] internal The internal lanes
     */
    MSInternalJunction(const std::string &id, const Position2D &position,
                       const Position2DVector &shape,
                       std::vector<MSLane*> incoming, std::vector<MSLane*> internal) throw();

    /// Destructor.
    virtual ~MSInternalJunction();

    /** Clears junction's and lane's requests to prepare for the next
        iteration. */
    bool clearRequests();
    void postloadInit() throw(ProcessError);

    const std::vector<MSLink*> &getFoeLinks(const MSLink *const srcLink) const throw() {
        return myInternalLinkFoes;
    }

    const std::vector<MSLane*> &getFoeInternalLanes(const MSLink *const srcLink) const throw() {
        return myInternalLaneFoes;
    }

private:

    std::vector<MSLink*> myInternalLinkFoes;
    std::vector<MSLane*> myInternalLaneFoes;

    /// @brief Invalidated copy constructor.
    MSInternalJunction(const MSInternalJunction&);

    /// @brief Invalidated assignment operator.
    MSInternalJunction& operator=(const MSInternalJunction&);

};


#endif
#endif

/****************************************************************************/


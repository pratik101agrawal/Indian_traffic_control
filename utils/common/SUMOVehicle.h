/****************************************************************************/
/// @file    SUMOVehicle.h
/// @author  Michael Behrisch
/// @date    Tue, 17 Feb 2009
/// @version $Id: SUMOVehicle.h 8740 2010-05-07 12:23:10Z behrisch $
///
// Abstract base class for vehicle representations
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
#ifndef SUMOVehicle_h
#define SUMOVehicle_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <utils/common/SUMOTime.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSVehicleType;
class MSRoute;
class MSEdge;

typedef std::vector<const MSEdge*> MSEdgeVector;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class SUMOVehicle
 * @brief Representation of a vehicle
 */
class SUMOVehicle {
public:
    /** @brief Get the vehicle's position along the lane
     * @return The position of the vehicle (in m from the lane's begin)
     */
    virtual const std::string& getID() const throw() = 0;

    /** @brief Get the vehicle's position along the lane
     * @return The position of the vehicle (in m from the lane's begin)
     */
    virtual SUMOReal getPositionOnLane() const throw() = 0;

    /** @brief Returns the vehicle's maximum speed
     * @return The vehicle's maximum speed
     */
    virtual SUMOReal getMaxSpeed() const = 0;

    /** @brief Returns the vehicle's current speed
     * @return The vehicle's speed
     */
    virtual SUMOReal getSpeed() const throw() = 0;

    /** @brief Returns the vehicle's type
     * @return The vehicle's type
     */
    virtual const MSVehicleType &getVehicleType() const = 0;

    /// Returns the current route
    virtual const MSRoute &getRoute() const = 0;

    /** @brief Returns the nSuccs'th successor of edge the vehicle is currently at
     *
     * If the rest of the route (counted from the current edge) than nSuccs,
     *  0 is returned.
     * @param[in] nSuccs The number of edge to look forward
     * @return The nSuccs'th following edge in the vehicle's route
     */
    virtual const MSEdge* succEdge(unsigned int nSuccs) const throw() = 0;

    /** @brief Adapt the vehicle's maximum speed depending on the reference speed
     * @param[in] the maximum speed on the edge
     * @return The vehicle's new maximum speed
     */
    virtual SUMOReal adaptMaxSpeed(SUMOReal referenceSpeed) = 0;

    /// Replaces the current route by the given edges
    virtual bool replaceRoute(const MSEdgeVector &edges, SUMOTime simTime, bool onInit=false) = 0;

};


#endif

/****************************************************************************/

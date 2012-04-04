/****************************************************************************/
/// @file    GUILane.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id: GUILane.h 8617 2010-04-21 15:06:43Z behrisch $
///
// Representation of a lane in the micro simulation (gui-version)
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
#ifndef GUILane_h
#define GUILane_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fx.h>
#include <string>
#include <utility>
#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include <utils/geom/Position2D.h>
#include <utils/geom/Position2DVector.h>
#include "GUILaneWrapper.h"
#include <utils/foxtools/MFXMutex.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSVehicle;
class MSNet;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUILane
 * @brief Representation of a lane in the micro simulation (gui-version)
 *
 * An extended MSLane. A mechanism to avoid concurrent
 * visualisation and simulation what may cause problems when vehicles
 * disappear is implemented using a mutex.
 */
class GUILane : public MSLane {
public:
    /** @brief Constructor
     *
     * @param[in] id The lane's id
     * @param[in] maxSpeed The speed allowed on this lane
     * @param[in] length The lane's length
     * @param[in] edge The edge this lane belongs to
     * @param[in] numericalID The numerical id of the lane
     * @param[in] allowed Vehicle classes that explicitely may drive on this lane
     * @param[in] disallowed Vehicle classes that are explicitaly forbidden on this lane
     * @see SUMOVehicleClass
     * @see MSLane
     */

    GUILane(const std::string &id, SUMOReal maxSpeed,
            SUMOReal length, size_t stripWidth,MSEdge * const edge, unsigned int numericalID,
            const Position2DVector &shape,
            const std::vector<SUMOVehicleClass> &allowed,
            const std::vector<SUMOVehicleClass> &disallowed) throw();


    /// @brief Destructor
    ~GUILane() throw();



    /// @name Vehicle emission
    ///@{

    /** @brief Tries to emit the given vehicle with the given state (speed and pos)
     *
     * Locks the lock, calls MSLane::isEmissionSuccess keeping the result,
     *  unlocks the lock and returns the result.
     *
     * @param[in] vehicle The vehicle to emit
     * @param[in] speed The speed with which it shall be emitted
     * @param[in] pos The position at which it shall be emitted
     * @param[in] recheckNextLanes Forces patching the speed for not being too fast on next lanes
     * @return Whether the vehicle could be emitted
     * @see MSLane::isEmissionSuccess
     */
    virtual bool isEmissionSuccess(MSVehicle* vehicle, SUMOReal speed, SUMOReal pos,
                                   bool recheckNextLanes, size_t startStripId) throw(ProcessError);
    ///@}



    /// @name Access to vehicles
    /// @{

    /** @brief Returns the vehicles container; locks it for microsimulation
     *
     * Locks "myLock" preventing usage by microsimulation.
     *
     * Please note that it is necessary to release the vehicles container
     *  afterwards using "releaseVehicles".
     * @return The vehicles on this lane
     * @see MSLane::getVehiclesSecure
     */
    const VehCont &getVehiclesSecure() const throw();//?? ACE ?? probably not used
    const VehCont &getVehiclesSecure(int) const throw();//?? ACE ?? modified for gui

    /** @brief Allows to use the container for microsimulation again
     *
     * Unlocks "myLock" preventing usage by microsimulation.
     * @see MSLane::releaseVehicles
     */
    void releaseVehicles() const throw();
    /// @}



    /// @name Vehicle movement (longitudinal)
    /// @{

    /** the same as in MSLane, but locks the access for the visualisation
        first; the access will be granted at the end of this method */
    bool moveCritical(SUMOTime t);

    /** the same as in MSLane, but locks the access for the visualisation
        first; the access will be granted at the end of this method */
    bool setCritical(SUMOTime t, std::vector<MSLane*> &into);

    /** the same as in MSLane, but locks the access for the visualisation
        first; the access will be granted at the end of this method */
    bool integrateNewVehicle(SUMOTime t);
    ///@}



    void detectCollisions(SUMOTime timestep);


    GUILaneWrapper *buildLaneWrapper(GUIGlObjectStorage &idStorage);
    MSVehicle *removeFirstVehicle();
    MSVehicle *removeVehicle(MSVehicle *remVehicle);

protected:
    /** the same as in MSLane, but locks the access for the visualisation
        first; the access will be granted at the end of this method */
    bool push(MSVehicle* veh, const std::vector<int> &stripID, bool hasMainStrip);

    MSVehicle* pop(SUMOTime t);

    /// moves myTmpVehicles int myVehicles after a lane change procedure
    void swapAfterLaneChange(SUMOTime t);

private:
    /// The mutex used to avoid concurrent updates of the vehicle buffer
    mutable MFXMutex myLock;


};


#endif

/****************************************************************************/


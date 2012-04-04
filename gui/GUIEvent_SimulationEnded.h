/****************************************************************************/
/// @file    GUIEvent_SimulationEnded.h
/// @author  Daniel Krajzewicz
/// @date    Thu, 19 Jun 2003
/// @version $Id: GUIEvent_SimulationEnded.h 8566 2010-04-06 07:45:21Z dkrajzew $
///
// Event sent when the the simulation is over
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
#ifndef GUIEvent_SimulationEnded_h
#define GUIEvent_SimulationEnded_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/gui/events/GUIEvent.h>
#include <utils/common/SUMOTime.h>
#include <microsim/MSNet.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIEvent_SimulationEnded
 * @brief Event sent when the the simulation is over
 *
 * Throw from GUIRunThread to GUIApplicationWindow.
 */
class GUIEvent_SimulationEnded : public GUIEvent {
public:
    /** @brief Constructor
     * @param[in] reason The reason the simulation has ended
     * @param[in] step The time step the simulation has ended at
     */
    GUIEvent_SimulationEnded(MSNet::SimulationState reason, SUMOTime step) throw()
            : GUIEvent(EVENT_SIMULATION_ENDED), myReason(reason), myStep(step) {}


    /// @brief Destructor
    ~GUIEvent_SimulationEnded() throw() { }


    /** @brief Returns the time step the simulation has ended at
     * @return The time step the simulation has ended at
     */
    SUMOTime getTimeStep() const throw() {
        return myStep;
    }


    /** @brief Returns the reason the simulation has ended due
     * @return The reason the simulation has ended
     */
    MSNet::SimulationState getReason() const {
        return myReason;
    }


protected:
    /// @brief The reason the simulation has ended
    MSNet::SimulationState myReason;

    /// @brief The time step the simulation has ended at
    SUMOTime myStep;


};


#endif

/****************************************************************************/


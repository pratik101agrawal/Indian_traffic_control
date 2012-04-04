/****************************************************************************/
/// @file    MSCFModel_Krauss.cpp
/// @author  Tobias Mayer
/// @date    Mon, 04 Aug 2009
/// @version $Id: MSCFModel_Krauss.cpp 8586 2010-04-13 12:01:54Z dkrajzew $
///
// Krauss car-following model, with acceleration decrease and faster start
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

#include <microsim/MSVehicle.h>
#include <microsim/MSLane.h>
#include "MSCFModel_Krauss.h"
#include <microsim/MSAbstractLaneChangeModel.h>
#include <utils/common/RandHelper.h>


// ===========================================================================
// method definitions
// ===========================================================================
MSCFModel_Krauss::MSCFModel_Krauss(const MSVehicleType* vtype,  SUMOReal accel, SUMOReal decel,
                                   SUMOReal dawdle, SUMOReal tau) throw()
        : MSCFModel(vtype, decel), myAccel(accel), myDawdle(dawdle), myTau(tau) {

    myTauDecel = decel * myTau;
    std::cout << "Krauss Loaded" << std::endl;
}


MSCFModel_Krauss::~MSCFModel_Krauss() throw() {}


SUMOReal
MSCFModel_Krauss::moveHelper(MSVehicle * const veh, const MSLane * const lane, SUMOReal vPos) const throw() {
    SUMOReal oldV = veh->getSpeed(); // save old v for optional acceleration computation
    SUMOReal vSafe = MIN2(vPos, veh->processNextStop(vPos)); // process stops
    // we need the acceleration for emission computation;
    //  in this case, we neglect dawdling, nonetheless, using
    //  vSafe does not incorporate speed reduction due to interaction
    //  on lane changing
    veh->setPreDawdleAcceleration(SPEED2ACCEL(vSafe-oldV));
    //
    SUMOReal vNext = dawdle(MIN3(lane->getMaxSpeed(), maxNextSpeed(oldV), vSafe));
    vNext =
        veh->getLaneChangeModel().patchSpeed(
            MAX2((SUMOReal) 0, oldV-(SUMOReal)ACCEL2SPEED(myDecel)), //!!! reverify
            vNext,
            MIN3(vSafe, veh->getLane().getMaxSpeed(), maxNextSpeed(oldV)),//vaccel(myState.mySpeed, myLane->maxSpeed())),
            vSafe);
    return MIN4(vNext, vSafe, veh->getLane().getMaxSpeed(), maxNextSpeed(oldV));
}


SUMOReal
MSCFModel_Krauss::ffeV(const MSVehicle * const veh, SUMOReal speed, SUMOReal gap, SUMOReal predSpeed) const throw() {
    return MIN2(_vsafe(gap, predSpeed), maxNextSpeed(speed));
}


SUMOReal
MSCFModel_Krauss::ffeV(const MSVehicle * const veh, SUMOReal gap, SUMOReal predSpeed) const throw() {
    return MIN2(_vsafe(gap, predSpeed), maxNextSpeed(veh->getSpeed()));
}


SUMOReal
MSCFModel_Krauss::ffeV(const MSVehicle * const veh, const MSVehicle *pred) const throw() {
    return MIN2(_vsafe(veh->gap2pred(*pred), pred->getSpeed()), maxNextSpeed(veh->getSpeed()));
}


SUMOReal
MSCFModel_Krauss::ffeS(const MSVehicle * const veh, SUMOReal gap) const throw() {
    return MIN2(_vsafe(gap, 0), maxNextSpeed(veh->getSpeed()));
}


SUMOReal
MSCFModel_Krauss::interactionGap(const MSVehicle * const veh, SUMOReal vL) const throw() {
    // Resolve the vsafe equation to gap. Assume predecessor has
    // speed != 0 and that vsafe will be the current speed plus acceleration,
    // i.e that with this gap there will be no interaction.
    SUMOReal vNext = MIN2(maxNextSpeed(veh->getSpeed()), veh->getLane().getMaxSpeed());
    SUMOReal gap = (vNext - vL) *
                   ((veh->getSpeed() + vL) * myInverseTwoDecel + myTau) +
                   vL * myTau;

    // Don't allow timeHeadWay < deltaT situations.
    return MAX2(gap, SPEED2DIST(vNext));
}


bool
MSCFModel_Krauss::hasSafeGap(SUMOReal speed, SUMOReal gap, SUMOReal predSpeed, SUMOReal laneMaxSpeed) const throw() {
    if (gap<0) {
        return false;
    }
    SUMOReal vSafe = MIN2(_vsafe(gap, predSpeed), maxNextSpeed(speed));
    SUMOReal vNext = MIN3(maxNextSpeed(speed), laneMaxSpeed, vSafe);
    return (vNext>=getSpeedAfterMaxDecel(speed) && gap>= SPEED2DIST(speed));
}


SUMOReal
MSCFModel_Krauss::dawdle(SUMOReal speed) const throw() {
    // generate random number out of [0,1]
    SUMOReal random = RandHelper::rand();
    // Dawdle.
    if (speed < myAccel) {
        // we should not prevent vehicles from driving just due to dawdling
        //  if someone is starting, he should definitely start
        // (but what about slow-to-start?)!!!
        speed -= ACCEL2SPEED(myDawdle * speed * random);
    } else {
        speed -= ACCEL2SPEED(myDawdle * getMaxAccel(speed) * random);
    }
    return MAX2(SUMOReal(0), speed);
}


/** Returns the SK-vsafe. */
SUMOReal MSCFModel_Krauss::_vsafe(SUMOReal gap, SUMOReal predSpeed) const throw() {
    if (predSpeed==0&&gap<0.01) {
        return 0;
    }
    SUMOReal vsafe = (SUMOReal)(-1. * myTauDecel + sqrt(myTauDecel*myTauDecel + (predSpeed*predSpeed) + (2. * myDecel * gap)));
    assert(vsafe >= 0);
    return vsafe;
}



//void MSCFModel::saveState(std::ostream &os) {}


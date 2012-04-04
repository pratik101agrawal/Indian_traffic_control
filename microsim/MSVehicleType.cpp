/****************************************************************************/
/// @file    MSVehicleType.cpp
/// @author  Christian Roessel
/// @date    Tue, 06 Mar 2001
/// @version $Id: MSVehicleType.cpp 8697 2010-04-30 06:34:23Z dkrajzew $
///
// The car-following model and parameter
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

#include "MSVehicleType.h"
#include "MSNet.h"
#include "cfmodels/MSCFModel_IDM.h"
#include "cfmodels/MSCFModel_Kerner.h"
#include "cfmodels/MSCFModel_Krauss.h"
#include "cfmodels/MSCFModel_KraussOrig1.h"
#include "cfmodels/MSCFModel_PWag2009.h"
#include "cfmodels/MSCFModel_IITBNN.h"
#include <cassert>
#include <utils/iodevices/BinaryInputDevice.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/RandHelper.h>
#include <utils/common/SUMOVTypeParameter.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
MSVehicleType::MSVehicleType(const std::string &id, SUMOReal length, size_t stripWidth,
                             SUMOReal maxSpeed, SUMOReal prob,
                             SUMOReal speedFactor, SUMOReal speedDev,
                             SUMOVehicleClass vclass,
                             SUMOEmissionClass emissionClass,
                             SUMOVehicleShape shape,
                             SUMOReal guiWidth, SUMOReal guiOffset,
                             int cfModel, const std::string &lcModel,
                             const RGBColor &c) throw()
        : myID(id), myLength(length), myStripWidth(stripWidth), myMaxSpeed(maxSpeed),
        myDefaultProbability(prob), mySpeedFactor(speedFactor),
        mySpeedDev(speedDev), myVehicleClass(vclass),
        myLaneChangeModel(lcModel),
        myEmissionClass(emissionClass), myColor(c),
        myWidth(guiWidth), myOffset(guiOffset), myShape(shape) {
    assert(myLength > 0);
    assert(getMaxSpeed() > 0);
}


MSVehicleType::~MSVehicleType() throw() {}


void
MSVehicleType::saveState(std::ostream &os) {
    FileHelpers::writeString(os, myID);
    FileHelpers::writeFloat(os, myLength);
    FileHelpers::writeFloat(os, getMaxSpeed());
    FileHelpers::writeInt(os, (int) myVehicleClass);
    FileHelpers::writeInt(os, (int) myEmissionClass);
    FileHelpers::writeInt(os, (int) myShape);
    FileHelpers::writeFloat(os, myWidth);
    FileHelpers::writeFloat(os, myOffset);
    FileHelpers::writeFloat(os, myDefaultProbability);
    FileHelpers::writeFloat(os, mySpeedFactor);
    FileHelpers::writeFloat(os, mySpeedDev);
    FileHelpers::writeFloat(os, myColor.red());
    FileHelpers::writeFloat(os, myColor.green());
    FileHelpers::writeFloat(os, myColor.blue());
    FileHelpers::writeInt(os, myCarFollowModel->getModelID());
    FileHelpers::writeString(os, myLaneChangeModel);
    //myCarFollowModel->saveState(os);
}


SUMOReal
MSVehicleType::get(const std::map<std::string, SUMOReal> &from, const std::string &name, SUMOReal defaultValue) throw() {
    std::map<std::string, SUMOReal>::const_iterator i = from.find(name);
    if (i==from.end()) {
        return defaultValue;
    }
    return (*i).second;
}


MSVehicleType *
MSVehicleType::build(SUMOVTypeParameter &from) throw(ProcessError) {
    MSVehicleType *vtype = new MSVehicleType(
        from.id, from.length, from.stripWidth, from.maxSpeed,
        from.defaultProbability, from.speedFactor, from.speedDev, from.vehicleClass, from.emissionClass,
        from.shape, from.width, from.offset, from.cfModel, from.lcModel, from.color);
    MSCFModel *model = 0;
    switch (from.cfModel) {
    case SUMO_TAG_CF_IDM:
        model = new MSCFModel_IDM(vtype,
                                  get(from.cfParameter, "accel", DEFAULT_VEH_ACCEL),
                                  get(from.cfParameter, "decel", DEFAULT_VEH_DECEL),
                                  get(from.cfParameter, "timeHeadWay", 1.5),
                                  get(from.cfParameter, "minGap", 5.),
                                  get(from.cfParameter, "tau", DEFAULT_VEH_TAU));
        break;
    case SUMO_TAG_CF_BKERNER:
        model = new MSCFModel_Kerner(vtype,
                                     get(from.cfParameter, "accel", DEFAULT_VEH_ACCEL),
                                     get(from.cfParameter, "decel", DEFAULT_VEH_DECEL),
                                     get(from.cfParameter, "tau", DEFAULT_VEH_TAU),
                                     get(from.cfParameter, "k", .5),
                                     get(from.cfParameter, "phi", 5.));
        break;
    case SUMO_TAG_CF_KRAUSS_ORIG1:
        model = new MSCFModel_KraussOrig1(vtype,
                                          get(from.cfParameter, "accel", DEFAULT_VEH_ACCEL),
                                          get(from.cfParameter, "decel", DEFAULT_VEH_DECEL),
                                          get(from.cfParameter, "sigma", DEFAULT_VEH_SIGMA),
                                          get(from.cfParameter, "tau", DEFAULT_VEH_TAU));
        break;
    case SUMO_TAG_CF_PWAGNER2009:
        model = new MSCFModel_PWag2009(vtype,
                                       get(from.cfParameter, "accel", DEFAULT_VEH_ACCEL),
                                       get(from.cfParameter, "decel", DEFAULT_VEH_DECEL),
                                       get(from.cfParameter, "sigma", DEFAULT_VEH_SIGMA),
                                       get(from.cfParameter, "tau", DEFAULT_VEH_TAU));
        break;
    case SUMO_TAG_CF_IITBNN:
	model = new MSCFModel_IITBNN(vtype,
                                     get(from.cfParameter, "accel", DEFAULT_VEH_ACCEL),
                                     get(from.cfParameter, "decel", DEFAULT_VEH_DECEL),
                                     get(from.cfParameter, "sigma", DEFAULT_VEH_SIGMA),
                                     get(from.cfParameter, "tau", DEFAULT_VEH_TAU));
        break;
    case SUMO_TAG_CF_KRAUSS:
    default:
        model = new MSCFModel_Krauss(vtype,
                                     get(from.cfParameter, "accel", DEFAULT_VEH_ACCEL),
                                     get(from.cfParameter, "decel", DEFAULT_VEH_DECEL),
                                     get(from.cfParameter, "sigma", DEFAULT_VEH_SIGMA),
                                     get(from.cfParameter, "tau", DEFAULT_VEH_TAU));
        break;
    }
    vtype->myCarFollowModel = model;
    return vtype;
}


/****************************************************************************/


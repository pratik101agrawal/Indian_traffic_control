/****************************************************************************/
/// @file    MSTriggeredRerouter.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 25 July 2005
/// @version $Id: MSTriggeredRerouter.cpp 8460 2010-03-17 22:22:24Z behrisch $
///
// Reroutes vehicles passing an edge
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

#include <string>
#include <algorithm>
#include <utils/common/MsgHandler.h>
#include <utils/common/Command.h>
#include <microsim/MSLane.h>
#include <microsim/MSNet.h>
#include <microsim/MSGlobals.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/common/UtilExceptions.h>
#include "MSTriggeredRerouter.h"
#include <utils/xml/XMLSubSys.h>
#include <utils/common/TplConvert.h>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/common/DijkstraRouterTT.h>
#include <utils/common/RandHelper.h>
#include <microsim/MSEdgeWeightsStorage.h>

#ifdef HAVE_MESOSIM
#include <mesosim/MELoop.h>
#endif

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * MSTriggeredRerouter::Setter - methods
 * ----------------------------------------------------------------------- */
MSTriggeredRerouter::Setter::Setter(MSTriggeredRerouter * const parent,
                                    MSLane * const lane) throw()
        : MSMoveReminder(lane), myParent(parent) {}


MSTriggeredRerouter::Setter::~Setter() throw() {}


bool
MSTriggeredRerouter::Setter::isStillActive(MSVehicle& veh, SUMOReal /*oldPos*/,
        SUMOReal /*newPos*/, SUMOReal /*newSpeed*/) throw() {
    myParent->reroute(veh, myLane->getEdge());
    return false;
}


bool
MSTriggeredRerouter::Setter::notifyEnter(MSVehicle& veh, bool, bool) throw() {
    myParent->reroute(veh, myLane->getEdge());
    return false;
}


/* -------------------------------------------------------------------------
 * MSTriggeredRerouter - methods
 * ----------------------------------------------------------------------- */
MSTriggeredRerouter::MSTriggeredRerouter(const std::string &id,
        const std::vector<MSEdge*> &edges,
        SUMOReal prob, const std::string &file, bool off)
        : MSTrigger(id), SUMOSAXHandler(file),
        myProbability(prob), myUserProbability(prob), myAmInUserMode(false) {
    // read in the trigger description
    if (!XMLSubSys::runParser(*this, file)) {
        throw ProcessError();
    }
    // build actors
#ifdef HAVE_MESOSIM
    if (MSGlobals::gUseMesoSim) {
        for (std::vector<MSEdge*>::const_iterator j=edges.begin(); j!=edges.end(); ++j) {
            MESegment *s = MSGlobals::gMesoNet->getSegmentForEdge(**j);
            s->addRerouter(this);
        }
    } else {
#endif
        for (std::vector<MSEdge*>::const_iterator j=edges.begin(); j!=edges.end(); ++j) {
            const std::vector<MSLane*> &destLanes = (*j)->getLanes();
            for (std::vector<MSLane*>::const_iterator i=destLanes.begin(); i!=destLanes.end(); ++i) {
                mySetter.push_back(new Setter(this, (*i)));
            }
        }
#ifdef HAVE_MESOSIM
    }
#endif
    if (off) {
        setUserMode(true);
        setUserUsageProbability(0);
    }
}


MSTriggeredRerouter::~MSTriggeredRerouter() throw() {
    {
        std::vector<Setter*>::iterator i;
        for (i=mySetter.begin(); i!=mySetter.end(); ++i) {
            delete *i;
        }
    }
}

// ------------ loading begin
void
MSTriggeredRerouter::myStartElement(SumoXMLTag element,
                                    const SUMOSAXAttributes &attrs) throw(ProcessError) {
    if (element==SUMO_TAG_INTERVAL) {
        bool ok = true;
        myCurrentIntervalBegin = attrs.getOptSUMOTimeReporting(SUMO_ATTR_BEGIN, "interval", 0, ok, -1);
        myCurrentIntervalEnd = attrs.getOptSUMOTimeReporting(SUMO_ATTR_END, "interval", 0, ok, -1);
    }

    if (element==SUMO_TAG_DEST_PROB_REROUTE) {
        // by giving probabilities of new destinations
        // get the destination edge
        std::string dest = attrs.getStringSecure(SUMO_ATTR_ID, "");
        if (dest=="") {
            throw ProcessError("MSTriggeredRerouter " + getID() + ": No destination edge id given.");
        }
        MSEdge *to = MSEdge::dictionary(dest);
        if (to==0) {
            throw ProcessError("MSTriggeredRerouter " + getID() + ": Destination edge '" + dest + "' is not known.");
        }
        // get the probability to reroute
        bool ok = true;
        SUMOReal prob = attrs.getOptSUMORealReporting(SUMO_ATTR_PROB, "rerouter/dest_prob_reroute", getID().c_str(), ok, 1.);
        if (!ok) {
            throw ProcessError();
        }
        if (prob<0) {
            throw ProcessError("MSTriggeredRerouter " + getID() + ": Attribute 'probability' for destination '" + dest + "' is negative (must not).");
        }
        // add
        myCurrentEdgeProb.add(prob, to);
    }

    if (element==SUMO_TAG_CLOSING_REROUTE) {
        // by closing
        std::string closed_id = attrs.getStringSecure(SUMO_ATTR_ID, "");
        if (closed_id=="") {
            throw ProcessError("MSTriggeredRerouter " + getID() + ": closed edge id given.");
        }
        MSEdge *closed = MSEdge::dictionary(closed_id);
        if (closed==0) {
            throw ProcessError("MSTriggeredRerouter " + getID() + ": Edge '" + closed_id + "' to close is not known.");
        }
        myCurrentClosed.push_back(closed);
    }

    if (element==SUMO_TAG_ROUTE_PROB_REROUTE) {
        // by explicite rerouting using routes
        // check if route exists
        std::string routeStr = attrs.getStringSecure(SUMO_ATTR_ID, "");
        if (routeStr=="") {
            throw ProcessError("MSTriggeredRerouter " + getID() + ": No route id given.");
        }
        const MSRoute* route = MSRoute::dictionary(routeStr);
        if (route == 0) {
            throw ProcessError("MSTriggeredRerouter " + getID() + ": Route '" + routeStr + "' does not exist.");
        }

        // get the probability to reroute
        bool ok = true;
        SUMOReal prob = attrs.getOptSUMORealReporting(SUMO_ATTR_PROB, "rerouter/dest_prob_reroute", getID().c_str(), ok, 1.);
        if (!ok) {
            throw ProcessError();
        }
        if (prob<0) {
            throw ProcessError("MSTriggeredRerouter " + getID() + ": Attribute 'probability' for route '" + routeStr + "' is negative (must not).");
        }
        // add
        myCurrentRouteProb.add(prob, route);
    }
}


void
MSTriggeredRerouter::myEndElement(SumoXMLTag element) throw(ProcessError) {
    if (element==SUMO_TAG_INTERVAL) {
        RerouteInterval ri;
        ri.begin = myCurrentIntervalBegin;
        ri.end = myCurrentIntervalEnd;
        ri.closed = myCurrentClosed;
        ri.edgeProbs = myCurrentEdgeProb;
        ri.routeProbs = myCurrentRouteProb;
        myCurrentClosed.clear();
        myCurrentEdgeProb.clear();
        myCurrentRouteProb.clear();
        myIntervals.push_back(ri);
    }
}


// ------------ loading end


bool
MSTriggeredRerouter::hasCurrentReroute(SUMOTime time, SUMOVehicle &veh) const {
    std::vector<RerouteInterval>::const_iterator i = myIntervals.begin();
    const MSRoute &route = veh.getRoute();
    while (i!=myIntervals.end()) {
        if ((*i).begin<=time && (*i).end>=time) {
            if ((*i).edgeProbs.getOverallProb()!=0||(*i).routeProbs.getOverallProb()!=0||route.containsAnyOf((*i).closed)) {
                return true;
            }
        }
        i++;
    }
    return false;
}


bool
MSTriggeredRerouter::hasCurrentReroute(SUMOTime time) const {
    std::vector<RerouteInterval>::const_iterator i = myIntervals.begin();
    while (i!=myIntervals.end()) {
        if ((*i).begin<=time && (*i).end>=time) {
            if ((*i).edgeProbs.getOverallProb()!=0||(*i).routeProbs.getOverallProb()!=0||(*i).closed.size()!=0) {
                return true;
            }
        }
        i++;
    }
    return false;
}


const MSTriggeredRerouter::RerouteInterval &
MSTriggeredRerouter::getCurrentReroute(SUMOTime time, SUMOVehicle &veh) const {
    std::vector<RerouteInterval>::const_iterator i = myIntervals.begin();
    const MSRoute &route = veh.getRoute();
    while (i!=myIntervals.end()) {
        if ((*i).begin<=time && (*i).end>=time) {
            if ((*i).edgeProbs.getOverallProb()!=0||(*i).routeProbs.getOverallProb()!=0||route.containsAnyOf((*i).closed)) {
                return *i;
            }
        }
        i++;
    }
    throw 1;
}


const MSTriggeredRerouter::RerouteInterval &
MSTriggeredRerouter::getCurrentReroute(SUMOTime) const {
    std::vector<RerouteInterval>::const_iterator i = myIntervals.begin();
    while (i!=myIntervals.end()) {
        if ((*i).edgeProbs.getOverallProb()!=0||(*i).routeProbs.getOverallProb()!=0||(*i).closed.size()!=0) {
            return *i;
        }
        i++;
    }
    throw 1;
}



void
MSTriggeredRerouter::reroute(SUMOVehicle &veh, const MSEdge &src) {
    // check whether the vehicle shall be rerouted
    SUMOTime time = MSNet::getInstance()->getCurrentTimeStep();
    if (!hasCurrentReroute(time, veh)) {
        return;
    }

    SUMOReal prob = myAmInUserMode ? myUserProbability : myProbability;
    if (RandHelper::rand() > prob) {
        return;
    }

    // get vehicle params
    const MSRoute &route = veh.getRoute();
    const MSEdge *lastEdge = route.getLastEdge();
    // get rerouting params
    const MSTriggeredRerouter::RerouteInterval &rerouteDef = getCurrentReroute(time, veh);
    const MSRoute *newRoute = rerouteDef.routeProbs.getOverallProb()>0 ? rerouteDef.routeProbs.get() : 0;
    // we will use the route if given rather than calling our own dijsktra...
    if (newRoute!=0) {
        veh.replaceRoute(newRoute->getEdges(), time);
        return;
    }
    // ok, try using a new destination
    const MSEdge *newEdge = rerouteDef.edgeProbs.getOverallProb()>0 ? rerouteDef.edgeProbs.get() : route.getLastEdge();
    if (newEdge==0) {
        newEdge = lastEdge;
    }

    // we have a new destination, let's replace the vehicle route
    MSEdgeWeightsStorage empty;
    MSNet::EdgeWeightsProxi proxi(empty, MSNet::getInstance()->getWeightsStorage());
    DijkstraRouterTT_ByProxi<MSEdge, SUMOVehicle, prohibited_withRestrictions<MSEdge, SUMOVehicle>, MSNet::EdgeWeightsProxi> router(MSEdge::dictSize(), true, &proxi, &MSNet::EdgeWeightsProxi::getTravelTime);
    router.prohibit(rerouteDef.closed);
    std::vector<const MSEdge*> edges;
    router.compute(&src, newEdge, &veh, MSNet::getInstance()->getCurrentTimeStep(), edges);
    veh.replaceRoute(edges, MSNet::getInstance()->getCurrentTimeStep());
}


void
MSTriggeredRerouter::setUserMode(bool val) {
    myAmInUserMode = val;
}


void
MSTriggeredRerouter::setUserUsageProbability(SUMOReal prob) {
    myUserProbability = prob;
}


bool
MSTriggeredRerouter::inUserMode() const {
    return myAmInUserMode;
}


SUMOReal
MSTriggeredRerouter::getProbability() const {
    return myAmInUserMode ? myUserProbability : myProbability;
}


SUMOReal
MSTriggeredRerouter::getUserProbability() const {
    return myUserProbability;
}



/****************************************************************************/


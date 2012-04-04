/****************************************************************************/
/// @file    MSEdgeControl.cpp
/// @author  Christian Roessel
/// @date    Mon, 09 Apr 2001
/// @version $Id: MSEdgeControl.cpp 8460 2010-03-17 22:22:24Z behrisch $
///
// Stores edges and lanes, performs moving of vehicle
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

#include "MSEdgeControl.h"
#include "MSEdge.h"
#include "MSLane.h"
#include <iostream>
#include <vector>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// member method definitions
// ===========================================================================
MSEdgeControl::MSEdgeControl(const std::vector< MSEdge* > &edges) throw()
        : myEdges(edges),
        myLanes(MSLane::dictSize()),
        myLastLaneChange(MSEdge::dictSize()) {
    // build the usage definitions for lanes
    for (std::vector< MSEdge* >::const_iterator i=myEdges.begin(); i!=myEdges.end(); ++i) {
        const std::vector<MSLane*> &lanes = (*i)->getLanes();
        if (lanes.size()==1) {
            size_t pos = (*lanes.begin())->getNumericalID();
            myLanes[pos].lane = *(lanes.begin());
            myLanes[pos].firstNeigh = lanes.end();
            myLanes[pos].lastNeigh = lanes.end();
            myLanes[pos].amActive = false;
            myLanes[pos].haveNeighbors = false;
        } else {
            for (std::vector<MSLane*>::const_iterator j=lanes.begin(); j!=lanes.end(); ++j) {
                size_t pos = (*j)->getNumericalID();
                myLanes[pos].lane = *j;
                myLanes[pos].firstNeigh = (j+1);
                myLanes[pos].lastNeigh = lanes.end();
                myLanes[pos].amActive = false;
                myLanes[pos].haveNeighbors = true;
            }
        }
        size_t pos = (*i)->getNumericalID();
        myLastLaneChange[pos] = -1;
    }
    // assign lane usage definitions to lanes
    for (size_t j=0; j<myLanes.size(); j++) {
        myLanes[j].lane->init(*this, myLanes[j].firstNeigh, myLanes[j].lastNeigh);
    }
}


MSEdgeControl::~MSEdgeControl() throw() {
}


void
MSEdgeControl::patchActiveLanes() throw() {
    for (std::set<MSLane*>::iterator i=myChangedStateLanes.begin(); i!=myChangedStateLanes.end(); ++i) {
        LaneUsage &lu = myLanes[(*i)->getNumericalID()];
        // if the lane was inactive but is now...
        if (!lu.amActive && (*i)->getVehicleNumber()>0) {
            // ... add to active lanes and mark as such
            if (lu.haveNeighbors) {
                myActiveLanes.push_front(*i);
            } else {
                myActiveLanes.push_back(*i);
            }
            lu.amActive = true;
        }
    }
    myChangedStateLanes.clear();
}

void
MSEdgeControl::moveCritical(SUMOTime t) throw() {
    for (std::list<MSLane*>::iterator i=myActiveLanes.begin(); i!=myActiveLanes.end();) {
        if ((*i)->getVehicleNumber()==0 || (*i)->moveCritical(t)) {
            myLanes[(*i)->getNumericalID()].amActive = false;
            i = myActiveLanes.erase(i);
        } else {
            ++i;
        }
    }
}


void
MSEdgeControl::moveFirst(SUMOTime t) throw() {
    myWithVehicles2Integrate.clear();
    for (std::list<MSLane*>::iterator i=myActiveLanes.begin(); i!=myActiveLanes.end();) {
        if ((*i)->getVehicleNumber()==0 || (*i)->setCritical(t, myWithVehicles2Integrate)) {
            myLanes[(*i)->getNumericalID()].amActive = false;
            i = myActiveLanes.erase(i);
        } else {
            ++i;
        }
    }
    for (std::vector<MSLane*>::iterator i=myWithVehicles2Integrate.begin(); i!=myWithVehicles2Integrate.end(); ++i) {
        if ((*i)->integrateNewVehicle(t)) {
            LaneUsage &lu = myLanes[(*i)->getNumericalID()];
            if (!lu.amActive) {
                if (lu.haveNeighbors) {
                    myActiveLanes.push_front(*i);
                } else {
                    myActiveLanes.push_back(*i);
                }
                lu.amActive = true;
            }
        }
    }
}


void
MSEdgeControl::changeLanes(SUMOTime t) throw() {
    std::vector<MSLane*> toAdd;
    for (std::list<MSLane*>::iterator i=myActiveLanes.begin(); i!=myActiveLanes.end();) {
        LaneUsage &lu = myLanes[(*i)->getNumericalID()];
        if (lu.haveNeighbors) {
            MSEdge &edge = (*i)->getEdge();
            if (myLastLaneChange[edge.getNumericalID()]!=t) {
                myLastLaneChange[edge.getNumericalID()] = t;
                edge.changeLanes(t);
                const std::vector<MSLane*> &lanes = edge.getLanes();
                for (std::vector<MSLane*>::const_iterator i=lanes.begin(); i!=lanes.end(); ++i) {
                    LaneUsage &lu = myLanes[(*i)->getNumericalID()];
                    if ((*i)->getVehicleNumber()>0 && !lu.amActive) {
                        toAdd.push_back(*i);
                        lu.amActive = true;
                    }
                }
            }
            ++i;
        } else {
            i = myActiveLanes.end();
        }
    }
    for (std::vector<MSLane*>::iterator i=toAdd.begin(); i!=toAdd.end(); ++i) {
        myActiveLanes.push_front(*i);
    }
}


void
MSEdgeControl::detectCollisions(SUMOTime timestep) throw() {
    // Detections is made by the edge's lanes, therefore hand over.
    for (std::list<MSLane*>::iterator i = myActiveLanes.begin(); i != myActiveLanes.end(); ++i) {
        (*i)->detectCollisions(timestep);
    }
}


std::vector<std::string>
MSEdgeControl::getEdgeNames() const throw() {
    std::vector<std::string> ret;
    for (std::vector<MSEdge*>::const_iterator i=myEdges.begin(); i!=myEdges.end(); ++i) {
        ret.push_back((*i)->getID());
    }
    return ret;
}


void
MSEdgeControl::gotActive(MSLane *l) throw() {
    myChangedStateLanes.insert(l);
}


/****************************************************************************/


/****************************************************************************/
/// @file    MSXMLRawOut.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 10.05.2004
/// @version $Id: MSXMLRawOut.cpp 8648 2010-04-27 09:34:37Z dkrajzew $
///
// Realises dumping the complete network state
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

#include <cassert>
#include <microsim/MSEdgeControl.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSStrip.h>
#include <microsim/MSGlobals.h>
#include <utils/iodevices/OutputDevice.h>
#include "MSXMLRawOut.h"

#ifdef HAVE_MESOSIM
#include <mesosim/MELoop.h>
#endif

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
void
MSXMLRawOut::write(OutputDevice &of, const MSEdgeControl &ec,
                   SUMOTime timestep) throw(IOError) {
    of.openTag("timestep") << " time=\"" << time2string(timestep) << "\">\n";
    const std::vector<MSEdge*> &edges = ec.getEdges();
    for (std::vector<MSEdge*>::const_iterator e=edges.begin(); e!=edges.end(); ++e) {
        writeEdge(of, **e);
    }
    of.closeTag();
}


void
MSXMLRawOut::writeEdge(OutputDevice &of, const MSEdge &edge) throw(IOError) {
    //en
    bool dump = !MSGlobals::gOmitEmptyEdgesOnDump;
    if (!dump) {
#ifdef HAVE_MESOSIM
        if (MSGlobals::gUseMesoSim) {
            MESegment* seg = MSGlobals::gMesoNet->getSegmentForEdge(edge);
            while (seg != 0) {
                if (seg->getCarNumber()!=0) {
                    dump = true;
                    break;
                }
                seg = seg->getNextSegment();
            }
        } else {
#endif
            const std::vector<MSLane*> &lanes = edge.getLanes();
            for (std::vector<MSLane*>::const_iterator lane=lanes.begin(); lane!=lanes.end(); ++lane) {
                if (((**lane).getVehicleNumber()!=0)) {
                    dump = true;
                    break;
                }
            }
#ifdef HAVE_MESOSIM
        }
#endif
    }
    //en
    //dump = true;
    if (dump) {
        of.openTag("edge") << " id=\"" << edge.getID() << "\">\n";
#ifdef HAVE_MESOSIM
        if (MSGlobals::gUseMesoSim) {
            MESegment* seg = MSGlobals::gMesoNet->getSegmentForEdge(edge);
            while (seg != 0) {
                seg->writeVehicles(of);
                seg = seg->getNextSegment();
            }
        } else {
#endif
            const std::vector<MSLane*> &lanes = edge.getLanes();
            for (std::vector<MSLane*>::const_iterator lane=lanes.begin(); lane!=lanes.end(); ++lane) {
                writeLane(of, **lane);
            }
#ifdef HAVE_MESOSIM
        }
#endif
        of.closeTag();
    }
}


void
MSXMLRawOut::writeLane(OutputDevice &of, const MSLane &lane) throw(IOError) {
    of.openTag("lane") << " id=\"" << lane.myID << "\">\n";
    for (std::vector<MSStrip*>::const_iterator strip = lane.myStrips.begin(); strip != lane.myStrips.end(); ++strip) {
        writeStrip(of, **strip);
    }
    of.closeTag();
}


void
MSXMLRawOut::writeStrip(OutputDevice &of, const MSStrip &strip) throw(IOError) {
    of.openTag("strip") << " id=\"" << strip.myID << "\"";
    if (strip.myVehicles.size()!=0) {
    //if (strip.getVehicleNumber()!=0) {
        of << ">\n";
        for (std::vector<MSVehicle*>::const_iterator veh = strip.myVehBuffer.begin();
                veh != strip.myVehBuffer.end(); ++veh) {
            //if ((*veh)->isMainStrip(strip))
            // XXX: Removed if condition for debugging, uncomment it later
                writeVehicle(of, **veh);
        }
        for (MSStrip::VehCont::const_iterator veh = strip.myVehicles.begin();
                veh != strip.myVehicles.end(); ++veh) {
            //if ((*veh)->isMainStrip(strip))
                writeVehicle(of, **veh);
        }
    }
    of.closeTag(strip.myVehicles.size()==0);
}


void
MSXMLRawOut::writeVehicle(OutputDevice &of, const SUMOVehicle &veh) throw(IOError) {
    of.openTag("vehicle") << " id=\"" << veh.getID() << "\" pos=\""
    << veh.getPositionOnLane() << "\" speed=\"" << veh.getSpeed() << "\"";
    of.closeTag(true);
}



/****************************************************************************/


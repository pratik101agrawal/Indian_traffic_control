/****************************************************************************/
/// @file    MSXMLRawOut.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 10.05.2004
/// @version $Id: MSXMLRawOut.h 8302 2010-02-19 13:09:51Z behrisch $
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
#ifndef MSXMLRawOut_h
#define MSXMLRawOut_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/common/SUMOTime.h>


// ===========================================================================
// class declarations
// ===========================================================================
class OutputDevice;
class MSEdgeControl;
class MSEdge;
class MSLane;
class MSStrip;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSXMLRawOut
 * @brief Realises dumping the complete network state
 *
 * The class offers a static method, which writes the complete dump of
 *  the given network into the given OutputDevice.
 *
 * @todo consider error-handling on write (using IOError)
 */
class MSXMLRawOut {
public:
    /** @brief Writes the complete network state of the given edges into the given device
     *
     * Opens the current time step, goes through the edges and writes each using
     *  writeEdge.
     *
     * @param[in] of The output device to use
     * @param[in] ec The EdgeControl which holds the edges to write
     * @param[in] timestep The current time step
     * @exception IOError If an error on writing occurs (!!! not yet implemented)
     */
    static void write(OutputDevice &of, const MSEdgeControl &ec,
                      SUMOTime timestep) throw(IOError);


    /** @brief Writes the dump of the given vehicle into the given device
     *
     * @param[in] of The output device to use
     * @param[in] veh The vehicle to dump
     * @exception IOError If an error on writing occurs (!!! not yet implemented)
     */
    static void writeVehicle(OutputDevice &of, const SUMOVehicle &veh) throw(IOError);


private:
    /** @brief Writes the dump of the given edge into the given device
     *
     * If the edge is not empty or also empty edges shall be dumped, the edge
     *  description is opened and writeLane is called for each lane.
     *
     * @param[in] of The output device to use
     * @param[in] edge The edge to dump
     * @todo MSGlobals::gOmitEmptyEdgesOnDump should not be used; rather the according option read in write
     * @exception IOError If an error on writing occurs (!!! not yet implemented)
     */
    static void writeEdge(OutputDevice &of, const MSEdge &edge) throw(IOError);


    /** @brief Writes the dump of the given lane into the given device
     *
     * Opens the lane description and goes through all vehicles, calling writeVehicle
     *  for each.
     *
     * @param[in] of The output device to use
     * @param[in] lane The lane to dump
     * @exception IOError If an error on writing occurs (!!! not yet implemented)
     */
    static void writeLane(OutputDevice &of, const MSLane &lane) throw(IOError);

    /** @brief Writes the dump of the given strip into the given device
     *
     * Opens the strip description and goes through all vehicles, calling writeVehicle
     *  for each only if this strip is the main strip for the vehicle.
     *
     * @param[in] of The output device to use
     * @param[in] strip The strip to dump
     * @exception IOError If an error on writing occurs (!!! not yet implemented)
     */
    static void writeStrip(OutputDevice &of, const MSStrip &strip) throw(IOError);

private:
    /// @brief Invalidated copy constructor.
    MSXMLRawOut(const MSXMLRawOut&);

    /// @brief Invalidated assignment operator.
    MSXMLRawOut& operator=(const MSXMLRawOut&);


};


#endif

/****************************************************************************/


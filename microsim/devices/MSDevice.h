/****************************************************************************/
/// @file    MSDevice.h
/// @author  Michael Behrisch, Daniel Krajzewicz
/// @date    Tue, 04 Dec 2007
/// @version $Id: MSDevice.h 8593 2010-04-15 13:22:34Z behrisch $
///
// Abstract in-vehicle device
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
#ifndef MSDevice_h
#define MSDevice_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <vector>
#include <microsim/MSVehicle.h>
#include <utils/common/UtilExceptions.h>


// ===========================================================================
// class declarations
// ===========================================================================
class OutputDevice;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSDevice
 * @brief Abstract in-vehicle device
 *
 * The MSDevice-interface brings the following interfaces to a vehicle that
 *  may be overwritten by real devices:
 * @arg Retrieval of the vehicle that holds the device
 * @arg Building and retrieval of a device id
 * @arg Methods called on vehicle movement / state change
 *
 * The "methods called on vehicle movement / state change" are called for each
 *  device within the corresponding vehicle methods. MSDevice brings already
 *  an empty (nothing doing) implementation of these.
 */
class MSDevice {
public:
    /** @brief Constructor
     *
     * @param[in] holder The vehicle that holds this device
     * @param[in] id The ID of the device
     */
    MSDevice(MSVehicle &holder, const std::string &id) throw()
            : myHolder(holder), myID(id) {
    }


    /// @brief Destructor
    virtual ~MSDevice() throw() { }


    /** @brief Returns the vehicle that holds this device
     *
     * @return The vehicle that holds this device
     */
    MSVehicle &getHolder() const throw() {
        return myHolder;
    }


    /** @brief Returns the id of this device
    * @return The device's ID
    */
    const std::string &getID() throw() {
        return myID;
    }


    /// @name Methods called on vehicle movement / state change
    /// @{

    /** @brief Called when the vehicle tries to get into the net */
    virtual void onTryEmit() { }


    /** @brief Update if vehicle enters a new lane in the move step.
     *
     * @param[in] enteredLane The lane the vehicle enters
     * @param[in] driven The distance driven by the vehicle within this time step
     */
    virtual void enterLaneAtMove(MSLane* enteredLane, SUMOReal driven) { }


    /** @brief Update of members if vehicle enters a new lane in the emit step
     *
     * @param[in] enteredLane The lane the vehicle enters
     * @param[in] state The vehicle's state during the emission
     */
    virtual void enterLaneAtEmit(MSLane* enteredLane, const MSVehicle::State &state) { }


    /** @brief Update of members if vehicle enters a new lane in the laneChange step.
     *
     * @param[in] enteredLane The lane the vehicle enters
     */
    virtual void enterLaneAtLaneChange(MSLane* enteredLane) { }


    /** @brief Update of members if vehicle leaves a new lane in the move step.
     *
     * @param[in] driven The distance driven by the vehicle within this time step
     */
    virtual void leaveLaneAtMove(SUMOReal driven) { }


    /** @brief Update of members if vehicle leaves a new lane in the lane change step. */
    virtual void leaveLane() { }


    /** @brief Called when the vehicle leaves the lane */
    virtual void onRemovalFromNet() { }
    // @}


    /** @brief Called on writing tripinfo output
     *
     * The device may write some statistics into the tripinfo output. It
     *  is assumed that the written information is a valid xml-snipplet, which
     *  will be embedded within the vehicle's information.
     *
     * The device should use the openTag / closeTag methods of the OutputDevice
     *  for correct indentation.
     *
     * @param[in] os The stream to write the information into
     * @exception IOError not yet implemented
     */
    virtual void tripInfoOutput(OutputDevice &os) const throw(IOError) {
    }


protected:
    /// @brief The vehicle that stores the device
    MSVehicle &myHolder;


private:
    /// @brief The built device id
    std::string myID;


private:
    /// @brief Invalidated copy constructor.
    MSDevice(const MSDevice&);

    /// @brief Invalidated assignment operator.
    MSDevice& operator=(const MSDevice&);

};


#endif

/****************************************************************************/


/****************************************************************************/
/// @file    MSInductLoop.cpp
/// @author  Christian Roessel
/// @modified Sagar Chordia
/// @date    2004-11-23
/// @version $Id: MSInductLoop.cpp 8763 2010-05-19 13:08:46Z dkrajzew $
///
// An unextended detector measuring at a fixed position on a fixed lane.
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


/*
 * Currently path for detector output file is to be specified explicitly on line 265.
 * Currently set to "/home/sagar/Desktop/detector" change as per your need.
 * Ensure to delete output file specified in path on line 265 for every new run, else new results will be appended to previous file.
 * TODO: Write detector output to output parameter(in this case dev)which is passed as function argument in writeXMLOutput *
 */
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MSInductLoop.h"
#include <cassert>
#include <numeric>
#include <utility>
#include <utils/common/WrappingCommand.h>
#include <utils/common/ToString.h>
#include <microsim/MSEventControl.h>
#include <microsim/MSLane.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/StringUtils.h>
#include <utils/iodevices/OutputDevice.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
MSInductLoop::MSInductLoop(const std::string& id,
                           MSLane * const lane,
                           SUMOReal positionInMeters) throw()
        : MSMoveReminder(lane), Named(id), myCurrentVehicle(0),
        myPosition(positionInMeters), myLastLeaveTime(0),
        myVehiclesOnDet(),myVehicleDataCont() ,myStripCount(lane->myStrips.size()){
    assert(myPosition >= 0 && myPosition <= myLane->getLength());
    std::cout<<myStripCount<<"\n";
    myVehiclesOnDet.resize(myStripCount);
    myVehicleDataCont.resize(myStripCount);
    myLastVehicleDataCont.resize(myStripCount);
    myCurrentVehicle.resize(myStripCount);
    myLastLeaveTime.resize(myStripCount);
    myLastOccupancy.resize(myStripCount);
    for(unsigned int i =0 ;i<myStripCount;i++){
    	std::cout<<myLastLeaveTime[i]<<myLastOccupancy[i]<<"\n";
    }
    reset();
    for(unsigned i=0;i<myStripCount;i++){
    	myLastLeaveTime[i] = STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep());
    }
}


MSInductLoop::~MSInductLoop() throw() {
	for(unsigned int i =0; i<myStripCount;i++){
		if (myCurrentVehicle[i]!=0) {
		        myCurrentVehicle[i]->quitRemindedLeft(this);
		    }
		    myCurrentVehicle[i] = 0;
	}
	myCurrentVehicle.clear();
	myLastLeaveTime.clear();
	myLastOccupancy.clear();
	myVehicleDataCont.clear();
	myVehiclesOnDet.clear();
	myLastVehicleDataCont.clear();
}


void
MSInductLoop::reset() throw() {
    myDismissedVehicleNumber = 0;
    for(unsigned int i =0; i<myStripCount;i++){
		myLastVehicleDataCont[i] = myVehicleDataCont[i];
		myVehicleDataCont[i].clear();
    }
}


bool
MSInductLoop::isStillActive(MSVehicle& veh, SUMOReal oldPos,
                            SUMOReal newPos, SUMOReal newSpeed) throw() {
   if (newPos < myPosition) {
        // detector not reached yet
        return true;
    }

    int mainStripNum = 0;int flag =0;
	for( MSLane::StripContIter it = myLane->myStrips.begin(); it!=myLane->myStrips.end();
			it ++){
		if(veh.isMainStrip(**it)){
			flag=1;
			break;
		}
		mainStripNum++;
	}

    if (flag == 1 && myVehiclesOnDet[mainStripNum].find(&veh) == myVehiclesOnDet[mainStripNum].end()) {
        // entered the detector by move
        SUMOReal entryTime = STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep());
        if (newSpeed!=0) {
            entryTime += (myPosition - oldPos) / newSpeed;
        }
        if (newPos - veh.getVehicleType().getLength() > myPosition) {
            // entered and passed detector in a single timestep
            SUMOReal leaveTime = STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep());
            leaveTime += (myPosition - oldPos + veh.getVehicleType().getLength()) / newSpeed;
            enterDetectorByMove(veh, entryTime);
            leaveDetectorByMove(veh, leaveTime);
            return false;
        }
        // entered detector, but not passed
        enterDetectorByMove(veh, entryTime);
        return true;
    }
    else if(flag==1){
        // vehicle has been on the detector the previous timestep
        if (newPos - veh.getVehicleType().getLength() >= myPosition) {
            // vehicle passed the detector
            SUMOReal leaveTime = STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep());
            leaveTime += (myPosition - oldPos + veh.getVehicleType().getLength()) / newSpeed;
            leaveDetectorByMove(veh, leaveTime);
            return false;
        }
        // vehicle stays on the detector
        return true;
    }
    else {std::cerr<<"Main strip not found\n";}
}


void
MSInductLoop::notifyLeave(MSVehicle& veh, bool isArrival, bool isLaneChange) throw() {
    if (veh.getPositionOnLane() > myPosition && veh.getPositionOnLane() - veh.getVehicleType().getLength() <= myPosition) {
        // vehicle is on detector during lane change
        leaveDetectorByLaneChange(veh);
    }
}


bool
MSInductLoop::notifyEnter(MSVehicle& veh, bool, bool) throw() {
    if (veh.getPositionOnLane() - veh.getVehicleType().getLength() > myPosition) {
        // vehicle-front is beyond detector. Ignore
        return false;
    }
    // vehicle is in front of detector
    return true;

}


SUMOReal
MSInductLoop::getCurrentSpeed() const throw() {
    std::vector<VehicleData> d = collectVehiclesOnDet(MSNet::getInstance()->getCurrentTimeStep()-DELTA_T);
    return d.size()!=0
           ? accumulate(d.begin(), d.end(), (SUMOReal) 0.0, speedSum) / (SUMOReal) d.size()
           : -1;
}


SUMOReal
MSInductLoop::getCurrentLength() const throw() {
    std::vector<VehicleData> d = collectVehiclesOnDet(MSNet::getInstance()->getCurrentTimeStep()-DELTA_T);
    return d.size()!=0
           ? accumulate(d.begin(), d.end(), (SUMOReal) 0.0, lengthSum) / (SUMOReal) d.size()
           : -1;
}


SUMOReal
MSInductLoop::getCurrentOccupancy() const throw() {
    SUMOTime tbeg = MSNet::getInstance()->getCurrentTimeStep()-DELTA_T;
    std::vector<VehicleData> d = collectVehiclesOnDet(tbeg);
    if (d.size()==0) {
        return -1;
    }
    SUMOReal occupancy = 0;
    for (std::vector< VehicleData >::const_iterator i=d.begin(); i!=d.end(); ++i) {
        SUMOReal timeOnDetDuringInterval = (*i).leaveTimeM - MAX2(STEPS2TIME(tbeg), (*i).entryTimeM);
        timeOnDetDuringInterval = MIN2(timeOnDetDuringInterval, TS);
        occupancy += timeOnDetDuringInterval;
    }
    return occupancy / TS *(SUMOReal) 100.;
}


SUMOReal
MSInductLoop::getCurrentPassedNumber() const throw() {
    std::vector<VehicleData> d = collectVehiclesOnDet(MSNet::getInstance()->getCurrentTimeStep()-DELTA_T);
    return (SUMOReal) d.size();
}


std::vector<std::string>
MSInductLoop::getCurrentVehicleIDs() const throw() {
    std::vector<VehicleData> d = collectVehiclesOnDet(MSNet::getInstance()->getCurrentTimeStep()-DELTA_T);
    std::vector<std::string> ret;
    for (std::vector<VehicleData>::iterator i=d.begin(); i!=d.end(); ++i) {
        ret.push_back((*i).idM);
    }
    return ret;
}


unsigned
MSInductLoop::getNVehContributed() const throw() {
    return (unsigned int) collectVehiclesOnDet(MSNet::getInstance()->getCurrentTimeStep()-DELTA_T).size();
}


SUMOReal
MSInductLoop::getTimestepsSinceLastDetection() const throw() {
    if (myVehiclesOnDet.size() != 0) {
        // detector is occupied
        return 0;
    }
    //random value of zero debugging needed
    return STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep()) - myLastLeaveTime[0];
}


void
MSInductLoop::writeXMLDetectorProlog(OutputDevice &dev) const throw(IOError) {
    dev.writeXMLHeader("detector");
}

void MSInductLoop::writeXMLOutput(OutputDevice &dev,
                             SUMOTime startTime, SUMOTime stopTime) throw(IOError) {
    SUMOReal t(STEPS2TIME(stopTime-startTime));
	unsigned nVehCrossed ;
	SUMOReal flow  ;
	SUMOReal occupancy = 0;
	SUMOReal meanLength, meanSpeed;
	dev.openTag("interval");
    dev <<" begin=\""<<time2string(startTime)<<"\" end=\""<<
        		time2string(stopTime)<<"\" \n";

    unsigned totVeh = 0;
    for(unsigned stripIndex=0;stripIndex<myStripCount;stripIndex++){
    	 int no_vehicles = myVehicleDataCont[stripIndex].size();
    	 nVehCrossed  = (unsigned) no_vehicles;
    	 flow = ((SUMOReal) no_vehicles / (SUMOReal) t) * (SUMOReal) 3600.0;

    	for (std::deque< VehicleData >::const_iterator i=myVehicleDataCont[stripIndex].begin(); i!=myVehicleDataCont[stripIndex].end(); ++i) {
			SUMOReal timeOnDetDuringInterval = (*i).leaveTimeM - MAX2(STEPS2TIME(startTime), (*i).entryTimeM);
			timeOnDetDuringInterval = MIN2(timeOnDetDuringInterval, t);
			occupancy += timeOnDetDuringInterval;
		}
		for (std::map< MSVehicle*, SUMOReal >::const_iterator i=myVehiclesOnDet[stripIndex].begin(); i!=myVehiclesOnDet[stripIndex].end(); ++i) {
			SUMOReal timeOnDetDuringInterval = STEPS2TIME(stopTime) - MAX2(STEPS2TIME(startTime), (*i).second);
			occupancy += timeOnDetDuringInterval;
		}
		occupancy = no_vehicles!=0 ? occupancy / t * (SUMOReal) 100.0 : 0;

		meanSpeed = no_vehicles!=0
							 ? accumulate(myVehicleDataCont[stripIndex].begin(), myVehicleDataCont[stripIndex].end(), (SUMOReal) 0.0, speedSum) / (SUMOReal) myVehicleDataCont[stripIndex].size()
							 : -1;

		meanLength = no_vehicles!=0
							  ? accumulate(myVehicleDataCont[stripIndex].begin(), myVehicleDataCont[stripIndex].end(), (SUMOReal) 0.0, lengthSum) / (SUMOReal) myVehicleDataCont[stripIndex].size()
							  : -1;
		if(no_vehicles >= 1){
			dev <<"\tstripNum=\""<<stripIndex<<" \"vehicle_id=\""<< (myVehicleDataCont[stripIndex].begin())->idM
			<<"\" flow=\""<<flow<<
			"\" occupancy=\""<<occupancy<<"\" speed=\""<<meanSpeed<<
			"\" length=\""<<meanLength<<
			"\" nVehEntered=\""<<no_vehicles<<"\" \n";
			totVeh += no_vehicles;
		}
		else {
			dev << "\tstripNum=\""<<stripIndex
				<<"\" flow=\""<<flow<<
				"\" occupancy=\""<<occupancy<<"\" speed=\""<<meanSpeed<<
				"\" length=\""<<meanLength<<
				"\" nVehEntered=\""<<no_vehicles<<"\" \n";
				totVeh += no_vehicles;
		}
    }
    dev << "total_Vehicles=\" "<<totVeh + myDismissedVehicleNumber <<"\"";
    dev.closeTag(true);
    reset();
}

void
MSInductLoop::enterDetectorByMove(MSVehicle& veh,
                                  SUMOReal entryTimestep) throw() {
	unsigned int mainStripNum = 0;
	int flag = 0;
	for( MSLane::StripContIter it = myLane->myStrips.begin(); it!=myLane->myStrips.end(); it ++){
		if(veh.isMainStrip(**it)){
			myVehiclesOnDet[mainStripNum].insert(std::make_pair(&veh, entryTimestep));
			flag = 1;
			break;
		}
		mainStripNum++;
	}

   if(mainStripNum < myStripCount) veh.quitRemindedEntered(this);
   /*if (myCurrentVehicle[mainStripNum]!=0 && myCurrentVehicle[mainStripNum]!=&veh) {
        // in fact, this is an error - a second vehicle is on the detector
        //  before the first one leaves... (collision)
        // Still, this seems to happen, but should not be handled herein.
        //  we will inform the user, etc., but continue as nothing had happened

        MsgHandler::getWarningInstance()->inform("Collision on e1-detector '" + getID() + "'.\n Vehicle '" + myCurrentVehicle[mainStripNum]->getID() +
                "' was aready at detector as '" + veh.getID() + "' entered.");
        leaveDetectorByMove(*(myCurrentVehicle[mainStripNum]), entryTimestep);
    }*/ //disabled by pulakesh_segmentation fault
    myCurrentVehicle[mainStripNum] = &veh;
}


void
MSInductLoop::leaveDetectorByMove(MSVehicle& veh,
                                  SUMOReal leaveTimestep) throw() {
	int mainStripNum = 0;int flag = 0;
	for( MSLane::StripContIter it = myLane->myStrips.begin(); it!=myLane->myStrips.end(); it ++){
		if(veh.isMainStrip(**it)){
			flag = 1;
			break;
		}
		mainStripNum++;
	}

    VehicleMap::iterator it = myVehiclesOnDet[mainStripNum].find(&veh);
    //uncommnet following for extra check
   // assert(it != myVehiclesOnDet[mainStripNum].end());
    if(it!= myVehiclesOnDet[mainStripNum].end()){
		SUMOReal entryTimestep = it->second;
		myVehiclesOnDet[mainStripNum].erase(it);
		myCurrentVehicle[mainStripNum] = 0;
		//uncomment following for extra check
		//assert(entryTimestep < leaveTimestep);
		myVehicleDataCont[mainStripNum].push_back(VehicleData(veh.getID(), veh.getVehicleType().getLength(), entryTimestep, leaveTimestep));
		myLastOccupancy[mainStripNum] = 0;
		veh.quitRemindedLeft(this);
    }
    else {
    	//exit(0);
    	}
}


void
MSInductLoop::leaveDetectorByLaneChange(MSVehicle& veh) throw() {

	// Discard entry data
	int mainStripNum = 0;
	int flag = 0;
	for( MSLane::StripContIter it = myLane->myStrips.begin(); it!=myLane->myStrips.end(); it ++){
		if(veh.isMainStrip(**it)){
			flag = 1;
			break;
		}
		mainStripNum++;
	}

	if(flag){
		VehicleMap::iterator it = myVehiclesOnDet[mainStripNum].find(&veh);
		myVehiclesOnDet[mainStripNum].erase(it);
		myDismissedVehicleNumber++;
		myCurrentVehicle[mainStripNum] = 0;
		veh.quitRemindedLeft(this);
	}
}


void
MSInductLoop::removeOnTripEnd(MSVehicle *veh) throw() {
	int mainStripNum = 0;
	int flag = 0;
	for( MSLane::StripContIter it = myLane->myStrips.begin(); it!=myLane->myStrips.end(); it ++){
		if(veh->isMainStrip(**it)){
			flag = 1;
			break;
		}
		mainStripNum++;
	}
	if(flag){
		myCurrentVehicle[mainStripNum] = 0;
		myVehiclesOnDet[mainStripNum].erase(veh);
	}
}


std::vector<MSInductLoop::VehicleData>
MSInductLoop::collectVehiclesOnDet(SUMOTime tMS) const throw() {
    SUMOReal t = STEPS2TIME(tMS);
    std::vector<VehicleData> ret;

    for(unsigned stripIndex = 0;stripIndex!=myStripCount; stripIndex ++){

		for (VehicleDataCont::const_iterator i=myVehicleDataCont[stripIndex].begin(); i!=myVehicleDataCont[stripIndex].end(); ++i) {
			if ((*i).leaveTimeM>=t) {
				ret.push_back(*i);
			}
		}
		for (VehicleDataCont::const_iterator i=myLastVehicleDataCont[stripIndex].begin(); i!=myLastVehicleDataCont[stripIndex].end(); ++i) {
			if ((*i).leaveTimeM>=t) {
				ret.push_back(*i);
			}
		}

		/*SUMOTime ct = MSNet::getInstance()->getCurrentTimeStep();

		for (VehicleMap::const_iterator i=myVehiclesOnDet[stripIndex].begin(); i!=myVehiclesOnDet[stripIndex].end(); ++i) {
			MSVehicle *v = (*i).first;
			VehicleData d(v->getID(), v->getVehicleType().getLength(), (*i).second, STEPS2TIME(ct));
			d.speedM = v->getSpeed();
			ret.push_back(d);
		}*/
    }
    return ret;
}


/****************************************************************************/


/****************************************************************************/
/// @file    MSAbstractLaneChangeModel.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 29.04.2005
/// @version $Id: MSAbstractLaneChangeModel.h 8236 2010-02-10 11:16:41Z behrisch $
///
//	�missingDescription�
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
#ifndef MSAbstractLaneChangeModel_h
#define MSAbstractLaneChangeModel_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MSLaneChanger.h"
#include "MSCFModel.h"

// ===========================================================================
// used enumeration
// ===========================================================================
enum LaneChangeAction {
    LCA_NONE = 0,
    LCA_URGENT = 1,
    LCA_SPEEDGAIN = 2,
    LCA_LEFT = 4,
    LCA_RIGHT = 8,

    LCA_BLOCKEDBY_LEADER = 16,
    LCA_BLOCKEDBY_FOLLOWER = 32,
    LCA_OVERLAPPING = 64,

    LCA_MAX = 128
};

enum ChangeRequest {
    REQUEST_NONE,  // vehicle doesn't want to change
    REQUEST_LEFT,  // vehicle want's to change to left lane
    REQUEST_RIGHT, // vehicle want's to change to right lane
    REQUEST_HOLD   // vehicle want's to keep the current lane
};

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSAbstractLaneChangeModel
 */
class MSAbstractLaneChangeModel {
public:
    class MSLCMessager {
    public:
        MSLCMessager(MSVehicle *leader,  MSVehicle *neighLead,
                     MSVehicle *neighFollow)
                : myLeader(leader), myNeighLeader(neighLead),
                myNeighFollower(neighFollow) { }

        ~MSLCMessager() { }

        void *informLeader(void *info, MSVehicle *sender) {
            assert(myLeader!=0);
            return myLeader->getLaneChangeModel().inform(info, sender);
        }

        void *informNeighLeader(void *info, MSVehicle *sender) {
            assert(myNeighLeader!=0);
            return myNeighLeader->getLaneChangeModel().inform(info, sender);
        }

        void *informNeighFollower(void *info, MSVehicle *sender) {
            assert(myNeighFollower!=0);
            return myNeighFollower->getLaneChangeModel().inform(info, sender);
        }
    private:
        MSVehicle *myLeader;
        MSVehicle *myNeighLeader;
        MSVehicle *myNeighFollower;

    };


    MSAbstractLaneChangeModel(MSVehicle &v)
            : myVehicle(v), myState(0),
#ifndef NO_TRACI
            myChangeRequest(REQUEST_NONE),
#endif
            myCarFollowModel(v.getCarFollowModel()) {
    }

    virtual ~MSAbstractLaneChangeModel() { }

    int getState() const {
        return myState;
    }

    void setState(int state) {
        myState = state;
    }

    virtual void prepareStep() { }

    /** @brief Called to examine whether the vehicle wants to change to right
        This method gets the information about the surrounding vehicles
        and whether another lane may be more preferable */
    virtual int wantsChangeToRight(
        MSLCMessager &msgPass, int blocked,
        const std::pair<MSVehicle*, SUMOReal> &leader,
        const std::vector<std::pair<MSVehicle*, SUMOReal> > &neighLead,
        const std::vector<std::pair<MSVehicle*, SUMOReal> > &neighFollow,
        const std::vector<MSStrip*> &neighStrips,
        const std::vector<MSVehicle::LaneQ> &preb,
        MSVehicle **lastBlocked) = 0;

    /** @brief Called to examine whether the vehicle wants to change to left
        This method gets the information about the surrounding vehicles
        and whether another lane may be more preferable */
    virtual int wantsChangeToLeft(
        MSLCMessager &msgPass, int blocked,
        const std::pair<MSVehicle*, SUMOReal> &leader,
        const std::vector<std::pair<MSVehicle*, SUMOReal> > &neighLead,
        const std::vector<std::pair<MSVehicle*, SUMOReal> > &neighFollow,
        const std::vector<MSStrip*> &neighStrips,
        const std::vector<MSVehicle::LaneQ> &preb,
        MSVehicle **lastBlocked) = 0;

    virtual void *inform(void *info, MSVehicle *sender) = 0;

    virtual SUMOReal patchSpeed(SUMOReal min, SUMOReal wanted, SUMOReal max,
                                SUMOReal vsafe) = 0;

    virtual void changed() = 0;

#ifndef NO_TRACI
    /**
     * The vehicle is requested to change the lane as soon as possible
     * without violating any directives defined by this lane change model
     *
     * @param request	indicates the requested change
     */
    virtual void requestLaneChange(ChangeRequest request) {
        myChangeRequest = request;
    };

    /**
     * Inform the model that a certain lane change request has been fulfilled
     * by the lane changer, so the request won't be taken into account the next time.
     *
     * @param request	indicates the request that was fulfilled
     */
    virtual void fulfillChangeRequest(ChangeRequest request) {
        if (request == myChangeRequest) {
            myChangeRequest = REQUEST_NONE;
        }
    }
#endif

protected:
    virtual bool congested(const MSVehicle * const neighLeader) {
        if (neighLeader==0) {
            return false;
        }
        // Congested situation are relevant only on highways (maxSpeed > 70km/h)
        // and congested on German Highways means that the vehicles have speeds
        // below 60km/h. Overtaking on the right is allowed then.
        if ((myVehicle.getLane().getMaxSpeed() <= 70.0 / 3.6) ||
                (neighLeader->getLane().getMaxSpeed() <= 70.0 / 3.6)) {

            return false;
        }
        if (myVehicle.congested() && neighLeader->congested()) {
            return true;
        }
        return false;
    }

    virtual bool predInteraction(const MSVehicle * const leader) {
        if (leader==0) {
            return false;
        }
        // let's check it on highways only
        if (leader->getSpeed()<(80.0*3.6)) {
            return false;
        }
        SUMOReal gap = leader->getPositionOnLane() - leader->getVehicleType().getLength() - myVehicle.getPositionOnLane();
        return gap < myCarFollowModel.interactionGap(&myVehicle, leader->getSpeed());
    }



protected:
    MSVehicle &myVehicle;

    /// @brief The vehicle's car following model
    const MSCFModel &myCarFollowModel;
    int myState;
#ifndef NO_TRACI
    ChangeRequest myChangeRequest;
#endif
};


#endif

/****************************************************************************/


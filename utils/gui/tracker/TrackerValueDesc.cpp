/****************************************************************************/
/// @file    TrackerValueDesc.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id: TrackerValueDesc.cpp 8738 2010-05-06 11:17:41Z behrisch $
///
// Storage for a tracked value
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
#include <vector>
#include <utils/common/RGBColor.h>
#include <utils/gui/globjects/GUIGlObject.h>
//#include <guisim/GUINet.h>
#include "TrackerValueDesc.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
TrackerValueDesc::TrackerValueDesc(const std::string &name,
                                   const RGBColor &col,
                                   GUIGlObject *o,
                                   SUMOTime recordBegin)
        : myName(name), myObject(o),
        myActiveCol(col), myInactiveCol(col),
        myMin(0), myMax(0),
        myAggregationInterval(TIME2STEPS(1)/DELTA_T), myInvalidValue(-1), myValidNo(0),
        myRecordingBegin(recordBegin), myTmpLastAggValue(0) {}


TrackerValueDesc::~TrackerValueDesc() {
    // just to quit cleanly on a failure
    if (myLock.locked()) {
        myLock.unlock();
    }
}


void
TrackerValueDesc::addValue(SUMOReal value) {
    if (myValues.size()==0) {
        myMin = value;
        myMax = value;
    } else {
        myMin = value < myMin ? value : myMin;
        myMax = value > myMax ? value : myMax;
    }
    myLock.lock();
    myValues.push_back(value);
    if (value!=myInvalidValue) {
        myTmpLastAggValue += value;
        myValidNo++;
    }
    const SUMOReal avg = myValidNo==0 ? static_cast<SUMOReal>(0) : myTmpLastAggValue / static_cast<SUMOReal>(myValidNo);
    if (myAggregationInterval==1 || myValues.size()%myAggregationInterval==1) {
        myAggregatedValues.push_back(avg);
    } else {
        myAggregatedValues.back() = avg;
    }
    if (myValues.size()%myAggregationInterval==0) {
        myTmpLastAggValue = 0;
        myValidNo = 0;
    }
    myLock.unlock();
}


SUMOReal
TrackerValueDesc::getRange() const {
    return myMax - myMin;
}


SUMOReal
TrackerValueDesc::getMin() const {
    return myMin;
}


SUMOReal
TrackerValueDesc::getMax() const {
    return myMax;
}


SUMOReal
TrackerValueDesc::getYCenter() const {
    return (myMin + myMax) / 2.0f;
}


const RGBColor &
TrackerValueDesc::getColor() const {
    return myActiveCol;
}


const std::vector<SUMOReal> &
TrackerValueDesc::getValues() {
    myLock.lock();
    return myValues;
}


const std::vector<SUMOReal> &
TrackerValueDesc::getAggregatedValues() {
    myLock.lock();
    return myAggregatedValues;
}


const std::string &
TrackerValueDesc::getName() const {
    return myName;
}

void
TrackerValueDesc::unlockValues() {
    myLock.unlock();
}


void
TrackerValueDesc::setAggregationSpan(SUMOTime as) {
    myLock.lock();
    if (myAggregationInterval != as/DELTA_T) {
        myAggregationInterval = as/DELTA_T;
        // ok, the aggregation has changed,
        //  let's recompute the list of aggregated values
        myAggregatedValues.clear();
        std::vector<SUMOReal>::const_iterator i=myValues.begin();
        while (i!=myValues.end()) {
            myTmpLastAggValue = 0;
            myValidNo = 0;
            for (size_t j=0; j<myAggregationInterval&&i!=myValues.end(); j++, ++i) {
                if ((*i)!=myInvalidValue) {
                    myTmpLastAggValue += (*i);
                    myValidNo++;
                }
            }
            if (myValidNo==0) {
                myAggregatedValues.push_back(0);
            } else {
                myAggregatedValues.push_back(myTmpLastAggValue / static_cast<SUMOReal>(myValidNo));
            }
        }
    }
    myLock.unlock();
}


SUMOTime
TrackerValueDesc::getAggregationSpan() const {
    return myAggregationInterval * DELTA_T;
}


SUMOTime
TrackerValueDesc::getRecordingBegin() const {
    return myRecordingBegin;
}



/****************************************************************************/


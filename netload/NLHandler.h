/****************************************************************************/
/// @file    NLHandler.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id: NLHandler.h 8715 2010-05-04 10:37:09Z behrisch $
///
// The XML-Handler for network loading
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
#ifndef NLHandler_h
#define NLHandler_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/sax/SAXException.hpp>
#include <xercesc/sax/AttributeList.hpp>
#include "NLBuilder.h"
#include "NLDiscreteEventBuilder.h"
#include "NLSucceedingLaneBuilder.h"
#include <microsim/MSLink.h>
#include <microsim/MSRouteHandler.h>
#include <microsim/traffic_lights/MSSimpleTrafficLightLogic.h>
#include <microsim/traffic_lights/MSActuatedTrafficLightLogic.h>
#include <microsim/MSBitSetLogic.h>
#include <utils/common/SUMOTime.h>


// ===========================================================================
// class declarations
// ===========================================================================
class NLContainer;
class NLDetectorBuilder;
class NLTriggerBuilder;
class MSTrafficLightLogic;
class NLGeomShapeBuilder;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NLNetHandler
 * @brief The XML-Handler for network loading
 *
 * The SAX2-handler responsible for parsing networks and routes to load.
 * This is an extension of the MSRouteHandler as routes and vehicles may also
 *  be loaded from network descriptions.
 */
class NLHandler : public MSRouteHandler {
public:
    /// @brief Definition of a lane vector
    typedef std::vector<MSLane*> LaneVector;

public:
    /** @brief Constructor
     *
     * @param[in] file Name of the parsed file
     * @param[in, out] net The network to fill
     * @param[in] detBuilder The detector builder to use
     * @param[in] triggerBuilder The trigger builder to use
     * @param[in] edgeBuilder The builder of edges to use
     * @param[in] junctionBuilder The builder of junctions to use
     * @param[in] shapeBuilder The geometric shapes builder to use
     */
    NLHandler(const std::string &file, MSNet &net,
              NLDetectorBuilder &detBuilder, NLTriggerBuilder &triggerBuilder,
              NLEdgeControlBuilder &edgeBuilder,
              NLJunctionControlBuilder &junctionBuilder,
              NLGeomShapeBuilder &shapeBuilder) throw();


    /// @brief Destructor
    virtual ~NLHandler() throw();


protected:
    /// @name inherited from GenericSAXHandler
    //@{

    /** @brief Called on the opening of a tag;
     *
     * @param[in] element ID of the currently opened element
     * @param[in] attrs Attributes within the currently opened element
     * @exception ProcessError If something fails
     * @see GenericSAXHandler::myStartElement
     * @todo Refactor/describe
     */
    virtual void myStartElement(SumoXMLTag element,
                                const SUMOSAXAttributes &attrs) throw(ProcessError);


    /** @brief Called when characters occure
     *
     * @param[in] element ID of the last opened element
     * @param[in] chars The read characters (complete)
     * @exception ProcessError If something fails
     * @see GenericSAXHandler::myCharacters
     * @todo Refactor/describe
     */
    virtual void myCharacters(SumoXMLTag element,
                              const std::string &chars) throw(ProcessError);


    /** @brief Called when a closing tag occurs
     *
     * @param[in] element ID of the currently opened element
     * @exception ProcessError If something fails
     * @see GenericSAXHandler::myEndElement
     * @todo Refactor/describe
     */
    virtual void myEndElement(SumoXMLTag element) throw(ProcessError);
    //@}


protected:
    void addParam(const SUMOSAXAttributes &attrs);

    /** @brief adds a detector
        Determines the type of the detector first, and then calls the
         appropriate method */
    virtual void addDetector(const SUMOSAXAttributes &attrs);


    /** @brief Builds an e1-detector using the given specification
     * @param[in] attrs The attributes that define the detector
     */
    virtual void addE1Detector(const SUMOSAXAttributes &attrs);

    /** @brief Builds an e2-detector using the given specification
     * @param[in] attrs The attributes that define the detector
     */
    virtual void addE2Detector(const SUMOSAXAttributes &attrs);

#ifdef _MESSAGES
    /** @brief Builds an e4-detector using the given specification
     * @param[in] attrs The attributes that define the detector
     */
    virtual void addMsgDetector(const SUMOSAXAttributes &attrs);
#endif

    /** @brief Starts building of an e3-detector using the given specification
     * @param[in] attrs The attributes that define the detector
     */
    void beginE3Detector(const SUMOSAXAttributes &attrs);

    /** @brief Adds an entry to the currently processed e3-detector
     * @param[in] attrs The attributes that define the entry
     */
    void addE3Entry(const SUMOSAXAttributes &attrs);

    /** @brief Adds an exit to the currently processed e3-detector
     * @param[in] attrs The attributes that define the exit
     */
    void addE3Exit(const SUMOSAXAttributes &attrs);

    /// Builds of an e3-detector using collected values
    virtual void endE3Detector();

    /** @brief Builds a vtype-detector using the given specification
     * @param[in] attrs The attributes that define the detector
     */
    virtual void addVTypeProbeDetector(const SUMOSAXAttributes &attrs);

    /** @brief Builds a routeprobe-detector using the given specification
     * @param[in] attrs The attributes that define the detector
     */
    virtual void addRouteProbeDetector(const SUMOSAXAttributes &attrs);

    /** @brief Builds edge or lane base mean data collector using the given specification
     * @param[in] attrs The attributes that define the detector
     */
    virtual void addEdgeLaneMeanData(const SUMOSAXAttributes &attrs, const char* objecttype);

    /** @brief Ends the detector building
     * @todo Remove this
     */
    void endDetector();

    /// Closes the process of building an edge
    virtual void closeEdge();

    /// Closes the process of building a lane
    virtual void closeLane();


protected:
    /// The net to fill (preinitialised)
    MSNet &myNet;


private:
    /// add the shape to the Lane
    void addLaneShape(const std::string &chars);

    /// begins the processing of an edge
    void beginEdgeParsing(const SUMOSAXAttributes &attrs);

    /// adds a lane to the previously opened edge
    void addLane(const SUMOSAXAttributes &attrs);

    /// adds a polygon
    void addPOI(const SUMOSAXAttributes &attrs);

    /// adds a polygon
    void addPoly(const SUMOSAXAttributes &attrs);

    /// adds a logic item to the current logic
    void addLogicItem(const SUMOSAXAttributes &attrs);

    /// begins the reading of a junction row logic
    void initJunctionLogic(const SUMOSAXAttributes &attrs);

    /// begins the reading of a traffic lights logic
    void initTrafficLightLogic(const SUMOSAXAttributes &attrs);

    /// adds a phase to the traffic lights logic currently build
    void addPhase(const SUMOSAXAttributes &attrs);


    /// opens a junction for processing
    virtual void openJunction(const SUMOSAXAttributes &attrs);

    /// adds a source
    virtual void addSource(const SUMOSAXAttributes &attrs);

#ifdef _MESSAGES
    /// adds a message emitter
    void addMsgEmitter(const SUMOSAXAttributes &attrs);
#endif

    /// adds a trigger
    void addTrigger(const SUMOSAXAttributes &attrs);

    /// opens the container of succeeding lanes for processing
    void openSucc(const SUMOSAXAttributes &attrs);

    /// adds a succeeding lane
    void addSuccLane(const SUMOSAXAttributes &attrs);


    /// adds the incoming lanes
    void addIncomingLanes(const std::string &chars);

    /// adds the incoming Polygon's Positions
    void addIncomingPolyPosititon(const std::string &chars);

#ifdef HAVE_INTERNAL_LANES
    /// adds the internal lanes
    void addInternalLanes(const std::string &chars);
#endif

    /// parses the shape of a junction
    void addJunctionShape(const std::string &chars);


    virtual void openWAUT(const SUMOSAXAttributes &attrs);
    void addWAUTSwitch(const SUMOSAXAttributes &attrs);
    void addWAUTJunction(const SUMOSAXAttributes &attrs);

    /// Parses network location description
    void setLocation(const SUMOSAXAttributes &attrs);

    /** @begin Parses a district and creates a pseudo edge for it
     *
     * Called on the occurence of a "district" element, this method
     *  retrieves the id of the district and creates a district type
     *  edge with this id.
     *
     * @param[in] attrs The attributes (of the "district"-element) to parse
     * @exception ProcessError If an edge given in district@edges is not known
     */
    void addDistrict(const SUMOSAXAttributes &attrs) throw(ProcessError);


    /** @begin Parses a district edge and connects it to the district
     *
     * Called on the occurence of a "dsource" or "dsink" element, this method
     *  retrieves the id of the approachable edge. If this edge is known
     *  and valid, the approaching edge is informed about it.
     *
     * @param[in] attrs The attributes to parse
     * @param[in] isSource whether a "dsource or a "dsink" was given
     * @todo No exception?
     */
    void addDistrictEdge(const SUMOSAXAttributes &attrs, bool isSource);


    /// sets the request size of the current junction logic
    void setRequestSize(const std::string &chars);

    /// sets the lane number of the current junction logic
    void setLaneNumber(const std::string &chars);

    /// sets the key of the current junction logic
    void setKey(const std::string &chars);

    /// sets the subkey of the current junction logic
    void setSubKey(const std::string &chars);

    /// Sets the offset a tl-logic shall be fired the first time after
    void setOffset(const std::string &chars);




    /// ends the processing of a junction
    virtual void closeJunction();

    void closeWAUT();

    /// closes the processing of a lane
    void closeSuccLane();

    /// Parses the given character into an enumeration typed link direction
    MSLink::LinkDirection parseLinkDir(char dir);

    /// Parses the given character into an enumeration typed link state
    MSLink::LinkState parseLinkState(char state);


protected:
    /// A builder for object actions
    NLDiscreteEventBuilder myActionBuilder;

    /// Information whether the currently parsed edge is internal and not wished, here
    bool myCurrentIsInternalToSkip;

    /// The detector builder to use
    NLDetectorBuilder &myDetectorBuilder;

    /// The type of the last detector
    std::string myCurrentDetectorType;

    /// The trigger builder to use
    NLTriggerBuilder &myTriggerBuilder;

    /** storage for edges during building */
    NLEdgeControlBuilder &myEdgeControlBuilder;

    /** storage for junctions during building */
    NLJunctionControlBuilder &myJunctionControlBuilder;

    NLGeomShapeBuilder &myShapeBuilder;

    /// storage for building succeeding lanes
    NLSucceedingLaneBuilder mySucceedingLaneBuilder;


    /// @name Information about a lane
    //@{

    /// The id of the current lane
    std::string myCurrentLaneID;

    /// The id of the current district
    std::string myCurrentDistrictID;

    /// The information whether the current lane is a depart lane
    bool myLaneIsDepart;

    /// The maximum speed allowed on the current lane
    SUMOReal myCurrentMaxSpeed;
    
    /// The current width of the lane (in number of strips)
    size_t myCurrentStripWidth;

    /// The length of the current lane
    SUMOReal myCurrentLength;

    /// Vehicle classes allowed on the current lane
    std::vector<SUMOVehicleClass> myAllowedClasses;

    /// Vehicle classes disallowed on the current lane
    std::vector<SUMOVehicleClass> myDisallowedClasses;

    /// The shape of the current lane
    Position2DVector myShape;
    //@}

    /// internal information whether a tls-logic is currently read
    bool myAmInTLLogicMode;

    /// The id of the currently processed WAUT
    std::string myCurrentWAUTID;

    /// The network offset
    Position2D myNetworkOffset;

    /// The network's boundaries
    Boundary myOrigBoundary, myConvBoundary;

    bool myCurrentIsBroken;

    /// @brief Whether deprecated usage of the "vclass" attribute was already reported
    bool myHaveWarnedAboutDeprecatedVClass;

    /// @brief Whether deprecated definition of a junction shape in characters was already reported
    bool myHaveWarnedAboutDeprecatedJunctionShape;

    /// @brief Whether deprecated definition of a lane shape in characters was already reported
    bool myHaveWarnedAboutDeprecatedLaneShape;

    /// @brief Whether deprecated definition of a polygon shape in characters was already reported
    bool myHaveWarnedAboutDeprecatedPolyShape;

    /// @brief Whether deprecated definition of network boundaries/projection was already reported
    bool myHaveWarnedAboutDeprecatedLocation;

    /// @brief Whether deprecated definition of phases was already reported
    bool myHaveWarnedAboutDeprecatedPhases;

private:
    /** invalid copy constructor */
    NLHandler(const NLHandler &s);

    /** invalid assignment operator */
    NLHandler &operator=(const NLHandler &s);

};


#endif

/****************************************************************************/


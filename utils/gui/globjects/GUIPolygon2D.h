/****************************************************************************/
/// @file    GUIPolygon2D.h
/// @author  Daniel Krajzewicz
/// @date    June 2006
/// @version $Id: GUIPolygon2D.h 8236 2010-02-10 11:16:41Z behrisch $
///
// The GUI-version of a polygon
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
#ifndef GUIPolygon2D_h
#define GUIPolygon2D_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <utils/shapes/Polygon2D.h>
#include <utils/gui/globjects/GUIGlObject_AbstractAdd.h>


// ===========================================================================
// class definitions
// ===========================================================================
/*
 * @class GUIPolygon2D
 * @brief The GUI-version of a polygon
 */
class GUIPolygon2D : public Polygon2D, public GUIGlObject_AbstractAdd {
public:
    /// Constructor
    GUIPolygon2D(GUIGlObjectStorage &idStorage, int layer,
                 const std::string name, const std::string type,
                 const RGBColor &color, const Position2DVector &Pos,
                 bool fill) throw();

    /// Destructor
    ~GUIPolygon2D() throw();



    /// @name inherited from GUIGlObject
    //@{

    /** @brief Returns an own popup-menu
     *
     * @param[in] app The application needed to build the popup-menu
     * @param[in] parent The parent window needed to build the popup-menu
     * @return The built popup-menu
     * @see GUIGlObject::getPopUpMenu
     */
    GUIGLObjectPopupMenu *getPopUpMenu(GUIMainWindow &app,
                                       GUISUMOAbstractView &parent) throw();


    /** @brief Returns an own parameter window
     *
     * @param[in] app The application needed to build the parameter window
     * @param[in] parent The parent window needed to build the parameter window
     * @return The built parameter window
     * @see GUIGlObject::getParameterWindow
     */
    GUIParameterTableWindow *getParameterWindow(GUIMainWindow &app,
            GUISUMOAbstractView &parent) throw();


    /** @brief Returns the id of the object as known to microsim
     *
     * @return The id of the polygon
     * @see GUIGlObject::getMicrosimID
     */
    const std::string &getMicrosimID() const throw();


    /** @brief Returns the boundary to which the view shall be centered in order to show the object
     *
     * @return The boundary the object is within
     * @see GUIGlObject::getCenteringBoundary
     */
    Boundary getCenteringBoundary() const throw();


    /** @brief Draws the object
     * @param[in] s The settings for the current view (may influence drawing)
     * @see GUIGlObject::drawGL
     */
    void drawGL(const GUIVisualizationSettings &s) const throw();
    //@}



    /// Returns the layer the object is located in
    int getLayer() const;

protected:
    /** @brief The layer this object is located in
     *
     * This value is used for determining which object to choose as being on top under the cursor
     */
    int myLayer;

};


#endif

/****************************************************************************/


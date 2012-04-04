/****************************************************************************/
/// @file    GUIBusStop.cpp
/// @author  Daniel Krajzewicz
/// @date    Wed, 07.12.2005
/// @version $Id: GUIBusStop.cpp 8725 2010-05-04 20:27:48Z behrisch $
///
// A lane area vehicles can halt at (gui-version)
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

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#include <string>
#include <utils/common/MsgHandler.h>
#include <utils/geom/Position2DVector.h>
#include <utils/geom/Line2D.h>
#include <utils/geom/Boundary.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/common/ToString.h>
#include <microsim/MSNet.h>
#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include <guisim/GUINet.h>
#include <guisim/GUIEdge.h>
#include "GUIBusStop.h"
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <gui/GUIGlobals.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <gui/GUIApplicationWindow.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <microsim/logging/FunctionBinding.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <foreign/polyfonts/polyfonts.h>
#include <utils/geom/GeomHelper.h>
#include <gui/GUIApplicationWindow.h>
#include <guisim/GUIBusStop.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
GUIBusStop::GUIBusStop(const std::string &id, MSNet &,
                       const std::vector<std::string> &lines, MSLane &lane,
                       SUMOReal frompos, SUMOReal topos) throw()
        : MSBusStop(id, lines, lane, frompos, topos),
        GUIGlObject_AbstractAdd(GUIGlObjectStorage::gIDStorage, "bus_stop:" + id, GLO_TRIGGER) {
    myFGShape = lane.getShape();
    myFGShape.move2side((SUMOReal) 1.65);
    myFGShape = myFGShape.getSubpart(frompos, topos);
    myFGShapeRotations.reserve(myFGShape.size()-1);
    myFGShapeLengths.reserve(myFGShape.size()-1);
    int e = (int) myFGShape.size() - 1;
    for (int i=0; i<e; ++i) {
        const Position2D &f = myFGShape[i];
        const Position2D &s = myFGShape[i+1];
        myFGShapeLengths.push_back(f.distanceTo(s));
        myFGShapeRotations.push_back((SUMOReal) atan2((s.x()-f.x()), (f.y()-s.y()))*(SUMOReal) 180.0/(SUMOReal) PI);
    }
    Position2DVector tmp = myFGShape;
    tmp.move2side(1.5);
    myFGSignPos = tmp.getLineCenter();
    myFGSignRot = 0;
    if (tmp.length()!=0) {
        myFGSignRot = myFGShape.rotationDegreeAtLengthPosition(SUMOReal((myFGShape.length() / 2.)));
        myFGSignRot -= 90;
    }
}


GUIBusStop::~GUIBusStop() throw() {}


GUIGLObjectPopupMenu *
GUIBusStop::getPopUpMenu(GUIMainWindow &app,
                         GUISUMOAbstractView &parent) throw() {
    GUIGLObjectPopupMenu *ret = new GUIGLObjectPopupMenu(app, parent, *this);
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    buildNameCopyPopupEntry(ret);
    buildSelectionPopupEntry(ret);
    buildPositionCopyEntry(ret, false);
    return ret;
}


GUIParameterTableWindow *
GUIBusStop::getParameterWindow(GUIMainWindow &,
                               GUISUMOAbstractView &) throw() {
    return 0;
}


const std::string &
GUIBusStop::getMicrosimID() const throw() {
    return getID();
}


void
GUIBusStop::drawGL(const GUIVisualizationSettings &s) const throw() {
    // (optional) set id
    if (s.needsGlID) {
        glPushName(getGlID());
    }
    glTranslated(0, 0, -.03);
    // draw the area
    glColor3d(76./255., 170./255., 50./255.);
    size_t i;
    GLHelper::drawBoxLines(myFGShape, myFGShapeRotations, myFGShapeLengths, 1.0);
    // draw the lines
    if (s.scale*s.addExaggeration>=10) {
        glTranslated(0, 0, -.01);
        glColor3d(76./255., 170./255., 50./255.);
        for (i=0; i!=myLines.size(); ++i) {
            glPushMatrix();
            glTranslated(myFGSignPos.x(), myFGSignPos.y(), 0);
            glRotated(180, 1, 0, 0);
            glRotated(myFGSignRot, 0, 0, 1);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            pfSetPosition(0, 0);
            pfSetScale(1.f);
            glScaled(s.addExaggeration, s.addExaggeration, 1);
            glTranslated(1.2, -(double)i, 0);
            pfDrawString(myLines[i].c_str());
            glPopMatrix();
        }
        glTranslated(0, 0, .01);
    }

    // draw the sign
    glPushMatrix();
    glTranslated(myFGSignPos.x(), myFGSignPos.y(), 0);
    int noPoints = 9;
    if (s.scale*s.addExaggeration>25) {
        noPoints = (int)(9.0 + (s.scale*s.addExaggeration) / 10.0);
        if (noPoints>36) {
            noPoints = 36;
        }
    }
    glScaled(s.addExaggeration, s.addExaggeration, 1);
    glColor3d(76./255., 170./255., 50./255.);
    GLHelper::drawFilledCircle((SUMOReal) 1.1, noPoints);
    if (s.scale*s.addExaggeration>=10) {
        glTranslated(0, 0, -.01);
        glColor3d(255./255., 235./255., 0./255.);
        GLHelper::drawFilledCircle((SUMOReal) 0.9, noPoints);
        glTranslated(0, 0, .01);
        // draw the H
        // not if scale to low
        if (s.scale*s.addExaggeration>=4.5) {
            glColor3d(76./255., 170./255., 50./255.);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glRotated(180, 1, 0, 0);
            glRotated(myFGSignRot, 0, 0, 1);
            glTranslated(0, 0, .02);
            pfSetPosition(0, 0);
            pfSetScale(1.6f);
            SUMOReal w = pfdkGetStringWidth("H");
            glTranslated(-w/2., 0.4, 0);
            pfDrawString("H");
            glTranslated(0, 0, -.02);
        }
    }
    glPopMatrix();
    glTranslated(0, 0, .03);
    // (optional) draw name
    if (s.drawAddName) {
        drawGLName(getCenteringBoundary().getCenter(), getMicrosimID(), s.addNameSize / s.scale);
    }
    // (optional) clear id
    if (s.needsGlID) {
        glPopName();
    }
}


Boundary
GUIBusStop::getCenteringBoundary() const throw() {
    Boundary b = myFGShape.getBoxBoundary();
    b.grow(20);
    return b;
}



/****************************************************************************/


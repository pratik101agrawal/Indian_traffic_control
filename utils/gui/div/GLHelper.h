/****************************************************************************/
/// @file    GLHelper.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id: GLHelper.h 8725 2010-05-04 20:27:48Z behrisch $
///
// Some methods which help to draw certain geometrical objects in openGL
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
#ifndef GLHelper_h
#define GLHelper_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <utility>
#include <utils/geom/Position2DVector.h>
#include <utils/geom/Line2D.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GLHelper
 * @brief Some methods which help to draw certain geometrical objects in openGL
 *
 * This class offers some static methods for drawing primitives in openGL.
 */
class GLHelper {
public:
    /** @brief Draws a filled polygon described by the list of points
     *
     * @param[in] v The polygon to draw
     * @param[in] close Whether the first point shall be appended
     */
    static void drawFilledPoly(const Position2DVector &v, bool close) throw();


    /** @brief Draws a thick line
     *
     * The line is drawn as a GL_QUADS.
     *
     * @param[in] beg The begin position of the line
     * @param[in] rot The direction the line shall be drawn to (in radiants)
     * @param[in] visLength The length of the line
     * @param[in] width The width of the line
     */
    static void drawBoxLine(const Position2D &beg, SUMOReal rot,
                            SUMOReal visLength, SUMOReal width) throw();


    /** @brief Draws a thick line using the mean of both given points as begin position
     *
     * The line is drawn as a GL_QUADS.
     *
     * @param[in] beg1 One of the begin positions of the line to use the mean of
     * @param[in] beg2 One of the begin positions of the line to use the mean of
     * @param[in] rot The direction the line shall be drawn to (in radiants)
     * @param[in] visLength The length of the line
     * @param[in] width The width of the line
     */
    static void drawBoxLine(const Position2D &beg1, const Position2D &beg2,
                            SUMOReal rot, SUMOReal visLength, SUMOReal width) throw();


    /** @brief Draws thick lines
     *
     * Each line is drawn using drawBoxLine.
     *
     * @param[in] geom The list of begin positions of the lines
     * @param[in] rots The directions the lines shall be drawn to (in radiants)
     * @param[in] lengths The lengths of the lines
     * @param[in] width The width of the lines
     * @see drawBoxLine
     */
    static void drawBoxLines(const Position2DVector &geom,
                             const std::vector<SUMOReal> &rots, const std::vector<SUMOReal> &lengths,
                             SUMOReal width) throw();


    /** @brief Draws thick lines using the mean of the points given in the point lists as begin positions
     *
     * Each line is drawn using drawBoxLine.
     *
     * @param[in] geom1 One of the lists to obtain the lines' begin positions to use the mean of
     * @param[in] geom2 One of the lists to obtain the lines' begin positions to use the mean of
     * @param[in] rots The directions the lines shall be drawn to (in radiants)
     * @param[in] lengths The lengths of the lines
     * @param[in] width The width of the lines
     * @see drawBoxLine
     */
    static void drawBoxLines(const Position2DVector &geom1,
                             const Position2DVector &geom2,
                             const std::vector<SUMOReal> &rots, const std::vector<SUMOReal> &lengths,
                             SUMOReal width) throw();


    /** @brief Draws thick lines
     *
     * Widths and length are computed internally by this method, each line is then
     *  drawn using drawBoxLine.
     *
     * @param[in] geom The list of begin positions of the lines
     * @param[in] width The width of the lines
     * @see drawBoxLine
     */
    static void drawBoxLines(const Position2DVector &geom, SUMOReal width) throw();


    /** @brief Draws a thin line
     *
     * The line is drawn as a GL_LINES.
     *
     * @param[in] beg The begin position of the line
     * @param[in] rot The direction the line shall be drawn to (in radiants)
     * @param[in] visLength The length of the line
     */
    static void drawLine(const Position2D &beg, SUMOReal rot,
                         SUMOReal visLength) throw();


    /** @brief Draws a thin line using the mean of both given points as begin position
     *
     * The line is drawn as a GL_LINES.
     *
     * @param[in] beg1 One of the begin positions of the line to use the mean of
     * @param[in] beg2 One of the begin positions of the line to use the mean of
     * @param[in] rot The direction the line shall be drawn to (in radiants)
     * @param[in] visLength The length of the line
     */
    static void drawLine(const Position2D &beg1, const Position2D &beg2,
                         SUMOReal rot, SUMOReal visLength) throw();


    /** @brief Draws a thin line along the given position vector
     *
     * The line is drawn as a GL_LINES.
     *
     * @param[in] v The positions vector to use
     */
    static void drawLine(const Position2DVector &v) throw();


    /** @brief Draws a thin line between the two points
     *
     * The line is drawn as a GL_LINES.
     *
     * @param[in] beg Begin of the line
     * @param[in] end End of the line
     */
    static void drawLine(const Position2D &beg, const Position2D &end) throw();


    /** @brief Draws a filled circle around (0,0)
     *
     * The circle is drawn by calling drawFilledCircle(width, steps, 0, 360).
     *
     * @param[in] width The width of the circle
     * @param[in] steps The number of steps to divide the circle into
     */
    static void drawFilledCircle(SUMOReal width, int steps=8) throw();


    /** @brief Draws a filled circle around (0,0)
     *
     * The circle is drawn use GL_TRIANGLES.
     *
     * @param[in] width The width of the circle
     * @param[in] steps The number of steps to divide the circle into
     * @param[in] beg The begin angle in degress
     * @param[in] end The end angle in degress
     */
    static void drawFilledCircle(SUMOReal width, int steps,
                                 SUMOReal beg, SUMOReal end) throw();


    /** @brief Draws an unfilled circle around (0,0)
     *
     * The circle is drawn by calling drawOutlineCircle(width, iwidth, steps, 0, 360).
     *
     * @param[in] width The (outer) width of the circle
     * @param[in] iwidth The inner width of the circle
     * @param[in] steps The number of steps to divide the circle into
     */
    static void drawOutlineCircle(SUMOReal width, SUMOReal iwidth,
                                  int steps=8) throw();


    /** @brief Draws an unfilled circle around (0,0)
     *
     * The circle is drawn use GL_TRIANGLES.
     *
     * @param[in] width The (outer) width of the circle
     * @param[in] iwidth The inner width of the circle
     * @param[in] steps The number of steps to divide the circle into
     * @param[in] beg The begin angle in degress
     * @param[in] end The end angle in degress
     */
    static void drawOutlineCircle(SUMOReal width, SUMOReal iwidth,
                                  int steps, SUMOReal beg, SUMOReal end) throw();


    /** @brief Draws a triangle at the end of the given line
     *
     * @param[in] l The line at which end the triangle shall be drawn
     * @param[in] tLength The length of the triangle
     * @param[in] tWidth The width of the triangle
     */
    static void drawTriangleAtEnd(const Line2D &l, SUMOReal tLength,
                                  SUMOReal tWidth) throw();

private:
    /// @brief Storage for precomputed sin/cos-values describing a circle
    static std::vector<std::pair<SUMOReal, SUMOReal> > myCircleCoords;

};


#endif

/****************************************************************************/


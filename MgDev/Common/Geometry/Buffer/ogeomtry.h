//
//  Copyright (C) 2004-2011 by Autodesk, Inc.
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of version 2.1 of the GNU Lesser
//  General Public License as published by the Free Software Foundation.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

/// \ingroup Geometry_Module

//------------------------------------------------------------------------------
//
// FILE: ogeomtry.h.
//
// PURPOSE: Declarations of geometry related functions and constants used by
//          OPS++. In addition, clipping function templates are defined.
//
//------------------------------------------------------------------------------

#ifndef _OGEOMTRY_H_
#define _OGEOMTRY_H_

#include <math.h>

// constants returned by OpsLineSide()

const int OPS_LEFT_OF_LINE  = -1;
const int OPS_ON_LINE       = 0;
const int OPS_RIGHT_OF_LINE = 1;

// constants specifying closure of elliptical arcs

const int OPS_ARC_CLOSE_NONE = 0;
const int OPS_ARC_CLOSE_PIE = 1;
const int OPS_ARC_CLOSE_CHORD = 2;

// prototypes for geometry related functions

OpsFloatPoint OpsSegmentMidpoint(const OpsFloatPoint endPts[]);
int OpsLineSide(const OpsFloatPoint endPts[], const OpsFloatPoint *point);
BOOL OpsLineSegmentsIntersect(const OpsFloatPoint endPts1[],
    const OpsFloatPoint endPts2[]);
BOOL OpsPointInEllipse(const OpsFloatExtent *ellipseRect,
    const OpsFloatPoint *point);
BOOL OpsLineSegmentIntersectsEllipse(const OpsFloatExtent *ellipseRect,
    const OpsFloatPoint segEndPts[]);
BOOL OpsPointInPolygon(const OpsFloatPoint vertices[], int nVertices,
    const OpsFloatPoint *point, int fillRule);
BOOL OpsPointInPolyPolygon(const OpsFloatPoint vertices[], int nPolygons,
    const int nPolyVerts[], const OpsFloatPoint *point, int fillRule);
int OpsWindingNumber(const OpsFloatPoint vertices[], int nVertices,
    const OpsFloatPoint *point);
BOOL OpsPolygonCentroid(const OpsFloatPoint vertices[], int nVertices,
    OpsFloatPoint *centroid);
void OpsPolygonCentroidTAW(const OpsFloatPoint vertices[], int nVertices,
    OpsFloatPoint *centroid);
void OpsPolygonCentroidBVM(const OpsFloatPoint vertices[], int nVertices,
    OpsFloatPoint *centroid);
void OpsPolygonCentroidWMC(const OpsFloatPoint vertices[], int nVertices,
    OpsFloatPoint *centroid);
int OpsPolylineCentroid(const OpsFloatPoint vertices[], int nVertices,
    OpsFloatPoint *centroid);
double OpsPolylineLength(const OpsFloatPoint vertices[], int nVertices);
double OpsLineLength(const OpsFloatPoint *endPt1, const OpsFloatPoint *endPt2);
void OpsPolyPolygonCentroid(const OpsFloatPoint vertices[],
    const int nPolyVerts[], int nPolygons, OpsFloatPoint *centroid);
double OpsPolygonArea(const OpsFloatPoint vertices[], int nVertices);
double OpsPolyPolygonArea(const OpsFloatPoint vertices[],
    const int nPolyVerts[], int nPolygons);
BOOL OpsPolygonizeEllipse(const OpsFloatExtent *ellipseRect, int nSegments,
    OpsFloatPoint vertices[]);
void OpsPolygonizeEllipseArc(const OpsFloatExtent *ellipseRect,
    double startAngleDeg, double endAngleRad, int closeStyle,
    int maxSegments, OpsFloatPoint vertices[], int &nVertices);
int OpsClipLine(const OpsFloatExtent &clipRect, OpsFloatPoint &endPt1,
    OpsFloatPoint &endPt2);
void OpsClipPolyline(const OpsFloatExtent &clipRect,
    const OpsFloatPoint inVertices[], int nVertices,
    OpsFloatPoint outVertices[], int &nPolylines, int nPolyVerts[]);
void OpsClipPolyPolyline(const OpsFloatExtent &clipRect,
    const OpsFloatPoint inVerts[], const int nInPolyVerts[], int nInPolylines,
    OpsFloatPoint outVerts[], int nOutPolyVerts[], int &nOutPolylines);
void OpsClipPolygon(const OpsFloatExtent &clipRect,
    const OpsFloatPoint inVerts[], int nInVerts,
    OpsFloatPoint outVerts[], int &nOutVerts);
void OpsClipPolyPolygon(const OpsFloatExtent &clipRect,
    const OpsFloatPoint inVerts[], const int nInPolyVerts[], int nInPolygons,
    OpsFloatPoint outVerts[], int nOutPolyVerts[], int &nOutPolygons);
void OpsTranslatePoints(const OpsFloatPoint inPoints[], int nPoints,
    float deltaX, float deltaY, OpsFloatPoint outPoints[]);
void OpsScalePoints(const OpsFloatPoint points[], int nPoints, float xScale,
    float yScale, OpsFloatPoint outPoints[]);
void OpsRotatePoints(const OpsFloatPoint points[], int nPoints,
    float rotateAngleRad, OpsFloatPoint outPoints[]);

#endif

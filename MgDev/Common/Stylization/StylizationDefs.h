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

#ifndef STYLIZATIONDEFS_H_
#define STYLIZATIONDEFS_H_

// std headers
#include <wchar.h>
#include <string>
#include <cstring>
#include <memory>

#include <float.h>
#define _USE_MATH_DEFINES //for PI, etc
#include <math.h>
#include <limits>
#include <climits>

// frequently used constants
const double M_PI180 = (M_PI / 180.0);
const double M_180PI = (180.0 / M_PI);
const double METERS_PER_INCH = 0.0254;
const double MILLIMETERS_PER_INCH = 25.4;
const double STANDARD_DISPLAY_DPI = 96.0;

// Line weight is limited by this value, in mm device units.  Is there any
// use case for rendering using a line weight greater than one meter?
const double MAX_LINEWEIGHT_IN_MM = 1000.0;

// Zero-length path segments (dots) are replaced with horizontal lines of
// this length.
const double LINE_SEGMENT_DOT_SIZE = 1.0e-5;

// the distance by which point labels are offset from the symbol's edge
const double POINT_LABEL_OFFSET_MM = 1.0;

// Threshold (in pixels) used by the RS_FontEngine to determine if it should optimize drawing
// or not.  When optimized, text is drawn as a line since it is so small.
const double TEXT_DRAWING_THRESHOLD = 3.0;

// clip offset is limited by this value, in device units
const double MAX_CLIPOFFSET_IN_METERS = 1.0;
const double MAX_CLIPOFFSET_IN_MM     = 1000.0 * MAX_CLIPOFFSET_IN_METERS;

// The maximum number of segments to use when chopping up a symbol
// for warping.  An example where this limit becomes important is
// with mapping space symbols on a map that's zoomed in very far.
const int MAX_CHOPBUFFER_SEGS = 100000;

// The number of polylines to stitch together in each batch.  This limit
// is needed since the current stitching algorithm scales as O(n^3).
const int STITCH_BATCH_SIZE = 100;


#define SE_INLINE inline

#ifdef _WIN32
#define SE_NOVTABLE __declspec(novtable)
#else
#define SE_NOVTABLE
#define _ASSERT(x)
#endif

#endif

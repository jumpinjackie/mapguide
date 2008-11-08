//
//  Copyright (C) 2007-2008 by Autodesk, Inc.
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

#ifndef SE_LINERENDERER_H_
#define SE_LINERENDERER_H_

#include "SE_Renderer.h"


// forward declare
struct HotSpot;


// The maximum number of segments to use when chopping up a symbol
// for warping.  An example where this limit becomes important is
// with mapping space symbols on a map that's zoomed in very far.
#define MAX_CHOPBUFFER_SEGS 100000


///////////////////////////////////////////////////////////////////////////////
class SE_LineRenderer
{
public:
    static void ProcessLineOverlapWrap(SE_Renderer* renderer, LineBuffer* geometry, SE_RenderLineStyle* style);

private:
    static int ConfigureHotSpots(SE_Renderer* renderer, LineBuffer* geometry, int cur_contour, SE_RenderLineStyle* style, RS_Bounds& styleBounds, HotSpot* hotspots);
    static int ComputePoints(SE_Renderer* renderer, LineBuffer* geometry, int cur_contour, HotSpot* hotspots);
    static void ChopLineBuffer(SE_Renderer* renderer, LineBuffer* inBuffer, LineBuffer* outBuffer);
    static LineBuffer* ClipPolyline(LineBufferPool* lbp, LineBuffer& geometry, double zMin, double zMax);
    static LineBuffer* ClipPolygon(LineBufferPool* lbp, LineBuffer& geometry, double zMin, double zMax);
    static int ClipLine(double zMin, double zMax, double* line, double* ret);
    static int ClipCode(double zMin, double zMax, double z);
};

#endif

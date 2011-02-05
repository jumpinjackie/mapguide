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

#include "stdafx.h"
#include "LineStyleDef.h"
#include <map>


// basic line styles
static const PixelRun PRSolid[] = {
    {1.0f,  true, Decoration_None, 0.0f, 0.0f}
//  {0.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRDash[] = {
    {12.0f,  true, Decoration_None, 0.0f, 0.0f},
    { 8.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRDot[] = {
    {4.0f,  true, Decoration_None, 0.0f, 0.0f},
    {4.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRDashDot[] = {
    {4.0f,  true, Decoration_None, 0.0f, 0.0f},
    {8.0f, false, Decoration_None, 0.0f, 0.0f},
    {4.0f,  true, Decoration_None, 0.0f, 0.0f},
    {8.0f, false, Decoration_None, 0.0f, 0.0f},
    {8.0f,  true, Decoration_None, 0.0f, 0.0f}
//  {0.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRDashDotDot[] = {
    {4.0f,  true, Decoration_None, 0.0f, 0.0f},
    {4.0f, false, Decoration_None, 0.0f, 0.0f},
    {4.0f,  true, Decoration_None, 0.0f, 0.0f},
    {4.0f, false, Decoration_None, 0.0f, 0.0f},
    {4.0f,  true, Decoration_None, 0.0f, 0.0f},
    {4.0f, false, Decoration_None, 0.0f, 0.0f},
    {8.0f,  true, Decoration_None, 0.0f, 0.0f}
//  {0.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRRailroad[] = {
    {9.0f,  true, Decoration_CrossTick, 5.0f, 2.0f}
//  {0.0f, false, Decoration_None, 0.0f, 0.0f}
};

// AutoCAD line styles
static const PixelRun PRBorder[] = {
    {48.0f,  true, Decoration_None, 0.0f, 0.0f},
    {24.0f, false, Decoration_None, 0.0f, 0.0f},
    {48.0f,  true, Decoration_None, 0.0f, 0.0f},
    {24.0f, false, Decoration_None, 0.0f, 0.0f},
    { 1.0f,  true, Decoration_None, 0.0f, 0.0f},
    {24.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRBorder2[] = {
    {24.0f,  true, Decoration_None, 0.0f, 0.0f},
    {12.0f, false, Decoration_None, 0.0f, 0.0f},
    {24.0f,  true, Decoration_None, 0.0f, 0.0f},
    {12.0f, false, Decoration_None, 0.0f, 0.0f},
    { 1.0f,  true, Decoration_None, 0.0f, 0.0f},
    {12.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRBorderX2[] = {
    {96.0f,  true, Decoration_None, 0.0f, 0.0f},
    {48.0f, false, Decoration_None, 0.0f, 0.0f},
    {96.0f,  true, Decoration_None, 0.0f, 0.0f},
    {48.0f, false, Decoration_None, 0.0f, 0.0f},
    { 1.0f,  true, Decoration_None, 0.0f, 0.0f},
    {48.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRCenter[] = {
    {124.0f,  true, Decoration_None, 0.0f, 0.0f},
    { 24.0f, false, Decoration_None, 0.0f, 0.0f},
    { 24.0f,  true, Decoration_None, 0.0f, 0.0f},
    { 24.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRCenter2[] = {
    {64.0f,  true, Decoration_None, 0.0f, 0.0f},
    {12.0f, false, Decoration_None, 0.0f, 0.0f},
    {12.0f,  true, Decoration_None, 0.0f, 0.0f},
    {12.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRCenterX2[] = {
    {250.0f,  true, Decoration_None, 0.0f, 0.0f},
    { 48.0f, false, Decoration_None, 0.0f, 0.0f},
    { 48.0f,  true, Decoration_None, 0.0f, 0.0f},
    { 48.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRDashDot3[] = {
    {48.0f,  true, Decoration_None, 0.0f, 0.0f},
    {24.0f, false, Decoration_None, 0.0f, 0.0f},
    { 1.0f,  true, Decoration_None, 0.0f, 0.0f},
    {24.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRDashDot2[] = {
    {24.0f,  true, Decoration_None, 0.0f, 0.0f},
    {12.0f, false, Decoration_None, 0.0f, 0.0f},
    { 1.0f,  true, Decoration_None, 0.0f, 0.0f},
    {12.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRDashDotX2[] = {
    {96.0f,  true, Decoration_None, 0.0f, 0.0f},
    {48.0f, false, Decoration_None, 0.0f, 0.0f},
    { 1.0f,  true, Decoration_None, 0.0f, 0.0f},
    {48.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRDashed[] = {
    {48.0f,  true, Decoration_None, 0.0f, 0.0f},
    {24.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRDashed2[] = {
    {24.0f,  true, Decoration_None, 0.0f, 0.0f},
    {12.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRDashedX2[] = {
    {96.0f,  true, Decoration_None, 0.0f, 0.0f},
    {48.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRDivide[] = {
    {48.0f,  true, Decoration_None, 0.0f, 0.0f},
    {24.0f, false, Decoration_None, 0.0f, 0.0f},
    { 1.0f,  true, Decoration_None, 0.0f, 0.0f},
    {24.0f, false, Decoration_None, 0.0f, 0.0f},
    { 1.0f,  true, Decoration_None, 0.0f, 0.0f},
    {24.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRDivide2[] = {
    {24.0f,  true, Decoration_None, 0.0f, 0.0f},
    {12.0f, false, Decoration_None, 0.0f, 0.0f},
    { 1.0f,  true, Decoration_None, 0.0f, 0.0f},
    {12.0f, false, Decoration_None, 0.0f, 0.0f},
    { 1.0f,  true, Decoration_None, 0.0f, 0.0f},
    {12.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRDivideX2[] = {
    {96.0f,  true, Decoration_None, 0.0f, 0.0f},
    {48.0f, false, Decoration_None, 0.0f, 0.0f},
    { 1.0f,  true, Decoration_None, 0.0f, 0.0f},
    {48.0f, false, Decoration_None, 0.0f, 0.0f},
    { 1.0f,  true, Decoration_None, 0.0f, 0.0f},
    {48.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRDot3[] = {
    { 1.0f,  true, Decoration_None, 0.0f, 0.0f},
    {24.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRDot2[] = {
    { 1.0f,  true, Decoration_None, 0.0f, 0.0f},
    {12.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRDotX2[] = {
    { 1.0f,  true, Decoration_None, 0.0f, 0.0f},
    {48.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRHidden[] = {
    {24.0f,  true, Decoration_None, 0.0f, 0.0f},
    {12.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRHidden2[] = {
    {12.0f,  true, Decoration_None, 0.0f, 0.0f},
    { 4.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRHiddenX2[] = {
    {48.0f,  true, Decoration_None, 0.0f, 0.0f},
    {24.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRPhantom[] = {
    {124.0f,  true, Decoration_None, 0.0f, 0.0f},
    { 24.0f, false, Decoration_None, 0.0f, 0.0f},
    { 24.0f,  true, Decoration_None, 0.0f, 0.0f},
    { 24.0f, false, Decoration_None, 0.0f, 0.0f},
    { 24.0f,  true, Decoration_None, 0.0f, 0.0f},
    { 24.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRPhantom2[] = {
    {64.0f,  true, Decoration_None, 0.0f, 0.0f},
    {12.0f, false, Decoration_None, 0.0f, 0.0f},
    {12.0f,  true, Decoration_None, 0.0f, 0.0f},
    {12.0f, false, Decoration_None, 0.0f, 0.0f},
    {12.0f,  true, Decoration_None, 0.0f, 0.0f},
    {12.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRPhantomX2[] = {
    {250.0f,  true, Decoration_None, 0.0f, 0.0f},
    { 48.0f, false, Decoration_None, 0.0f, 0.0f},
    { 48.0f,  true, Decoration_None, 0.0f, 0.0f},
    { 48.0f, false, Decoration_None, 0.0f, 0.0f},
    { 48.0f,  true, Decoration_None, 0.0f, 0.0f},
    { 48.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRACAD_ISO02W100[] = {
    {48.0f,  true, Decoration_None, 0.0f, 0.0f},
    {12.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRACAD_ISO03W100[] = {
    {48.0f,  true, Decoration_None, 0.0f, 0.0f},
    {72.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRACAD_ISO04W100[] = {
    {96.0f,  true, Decoration_None, 0.0f, 0.0f},
    {12.0f, false, Decoration_None, 0.0f, 0.0f},
    { 1.0f,  true, Decoration_None, 0.0f, 0.0f},
    {12.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRACAD_ISO05W100[] = {
    {96.0f,  true, Decoration_None, 0.0f, 0.0f},
    {12.0f, false, Decoration_None, 0.0f, 0.0f},
    { 1.0f,  true, Decoration_None, 0.0f, 0.0f},
    {12.0f, false, Decoration_None, 0.0f, 0.0f},
    { 1.0f,  true, Decoration_None, 0.0f, 0.0f},
    {12.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRACAD_ISO06W100[] = {
    {96.0f,  true, Decoration_None, 0.0f, 0.0f},
    {12.0f, false, Decoration_None, 0.0f, 0.0f},
    { 1.0f,  true, Decoration_None, 0.0f, 0.0f},
    {12.0f, false, Decoration_None, 0.0f, 0.0f},
    { 1.0f,  true, Decoration_None, 0.0f, 0.0f},
    {12.0f, false, Decoration_None, 0.0f, 0.0f},
    { 1.0f,  true, Decoration_None, 0.0f, 0.0f},
    {12.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRACAD_ISO07W100[] = {
    { 1.0f,  true, Decoration_None, 0.0f, 0.0f},
    {12.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRACAD_ISO08W100[] = {
    {96.0f,  true, Decoration_None, 0.0f, 0.0f},
    {12.0f, false, Decoration_None, 0.0f, 0.0f},
    {24.0f,  true, Decoration_None, 0.0f, 0.0f},
    {12.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRACAD_ISO09W100[] = {
    {96.0f,  true, Decoration_None, 0.0f, 0.0f},
    {12.0f, false, Decoration_None, 0.0f, 0.0f},
    {24.0f,  true, Decoration_None, 0.0f, 0.0f},
    {12.0f, false, Decoration_None, 0.0f, 0.0f},
    {24.0f,  true, Decoration_None, 0.0f, 0.0f},
    {12.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRACAD_ISO10W100[] = {
    {48.0f,  true, Decoration_None, 0.0f, 0.0f},
    {12.0f, false, Decoration_None, 0.0f, 0.0f},
    { 1.0f,  true, Decoration_None, 0.0f, 0.0f},
    {12.0f, false, Decoration_None, 0.0f, 0.0f},
};

static const PixelRun PRACAD_ISO11W100[] = {
    {48.0f,  true, Decoration_None, 0.0f, 0.0f},
    {12.0f, false, Decoration_None, 0.0f, 0.0f},
    {48.0f,  true, Decoration_None, 0.0f, 0.0f},
    {12.0f, false, Decoration_None, 0.0f, 0.0f},
    { 1.0f,  true, Decoration_None, 0.0f, 0.0f},
    {12.0f, false, Decoration_None, 0.0f, 0.0f},
};

static const PixelRun PRACAD_ISO12W100[] = {
    {48.0f,  true, Decoration_None, 0.0f, 0.0f},
    {12.0f, false, Decoration_None, 0.0f, 0.0f},
    { 1.0f,  true, Decoration_None, 0.0f, 0.0f},
    {12.0f, false, Decoration_None, 0.0f, 0.0f},
    { 1.0f,  true, Decoration_None, 0.0f, 0.0f},
    {12.0f, false, Decoration_None, 0.0f, 0.0f},
};

static const PixelRun PRACAD_ISO13W100[] = {
    {48.0f,  true, Decoration_None, 0.0f, 0.0f},
    {12.0f, false, Decoration_None, 0.0f, 0.0f},
    {48.0f,  true, Decoration_None, 0.0f, 0.0f},
    {12.0f, false, Decoration_None, 0.0f, 0.0f},
    { 1.0f,  true, Decoration_None, 0.0f, 0.0f},
    {12.0f, false, Decoration_None, 0.0f, 0.0f},
    { 1.0f,  true, Decoration_None, 0.0f, 0.0f},
    {12.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRACAD_ISO14W100[] = {
    {48.0f,  true, Decoration_None, 0.0f, 0.0f},
    {12.0f, false, Decoration_None, 0.0f, 0.0f},
    { 1.0f,  true, Decoration_None, 0.0f, 0.0f},
    {12.0f, false, Decoration_None, 0.0f, 0.0f},
    { 1.0f,  true, Decoration_None, 0.0f, 0.0f},
    {12.0f, false, Decoration_None, 0.0f, 0.0f},
    { 1.0f,  true, Decoration_None, 0.0f, 0.0f},
    {12.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRACAD_ISO15W100[] = {
    {48.0f,  true, Decoration_None, 0.0f, 0.0f},
    {12.0f, false, Decoration_None, 0.0f, 0.0f},
    {48.0f,  true, Decoration_None, 0.0f, 0.0f},
    {12.0f, false, Decoration_None, 0.0f, 0.0f},
    { 1.0f,  true, Decoration_None, 0.0f, 0.0f},
    {12.0f, false, Decoration_None, 0.0f, 0.0f},
    { 1.0f,  true, Decoration_None, 0.0f, 0.0f},
    {12.0f, false, Decoration_None, 0.0f, 0.0f},
    { 1.0f,  true, Decoration_None, 0.0f, 0.0f},
    {12.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRFenceline1[] = {
    {25.0f,  true, Decoration_None,   0.0f, 0.0f},
    { 9.0f, false, Decoration_Circle, 9.0f, 4.0f},
    {25.0f,  true, Decoration_None,   0.0f, 0.0f}
//  { 0.0f, false, Decoration_None,   0.0f, 0.0f}
};

static const PixelRun PRFenceline2[] = {
    {25.0f,  true, Decoration_None,   0.0f, 0.0f},
    { 9.0f, false, Decoration_Square, 9.0f, 4.0f},
    {25.0f,  true, Decoration_None,   0.0f, 0.0f}
//  { 0.0f, false, Decoration_None,   0.0f, 0.0f}
};

static const PixelRun PRTracks[] = {
    {13.0f,  true, Decoration_CrossTick, 25.0f, 6.0f}
//  { 0.0f, false, Decoration_None, 0.0f, 0.0f}
};

// World/MapInfo line styles
static const PixelRun PRLine03[] = {
    {1.0f,  true, Decoration_None, 0.0f, 0.0f},
    {2.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRLine04[] = {
    {2.0f,  true, Decoration_None, 0.0f, 0.0f},
    {2.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRLine05[] = {
    {4.0f,  true, Decoration_None, 0.0f, 0.0f},
    {2.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRLine06[] = {
    {8.0f,  true, Decoration_None, 0.0f, 0.0f},
    {2.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRLine07[] = {
    {16.0f,  true, Decoration_None, 0.0f, 0.0f},
    { 4.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRLine08[] = {
    {32.0f,  true, Decoration_None, 0.0f, 0.0f},
    { 8.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRLine09[] = {
    {5.0f,  true, Decoration_None, 0.0f, 0.0f},
    {5.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRLine10[] = {
    {1.0f,  true, Decoration_None, 0.0f, 0.0f},
    {7.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRLine11[] = {
    {4.0f,  true, Decoration_None, 0.0f, 0.0f},
    {8.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRLine12[] = {
    {8.0f,  true, Decoration_None, 0.0f, 0.0f},
    {8.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRLine13[] = {
    {16.0f,  true, Decoration_None, 0.0f, 0.0f},
    {16.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRLine14[] = {
    {11.0f,  true, Decoration_None, 0.0f, 0.0f},
    { 5.0f, false, Decoration_None, 0.0f, 0.0f},
    { 1.0f,  true, Decoration_None, 0.0f, 0.0f},
    { 5.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRLine15[] = {
    {18.0f,  true, Decoration_None, 0.0f, 0.0f},
    { 3.0f, false, Decoration_None, 0.0f, 0.0f},
    { 1.0f,  true, Decoration_None, 0.0f, 0.0f},
    { 3.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRLine16[] = {
    {18.0f,  true, Decoration_None, 0.0f, 0.0f},
    { 3.0f, false, Decoration_None, 0.0f, 0.0f},
    { 4.0f,  true, Decoration_None, 0.0f, 0.0f},
    { 3.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRLine17[] = {
    {32.0f,  true, Decoration_None, 0.0f, 0.0f},
    {12.0f, false, Decoration_None, 0.0f, 0.0f},
    { 6.0f,  true, Decoration_None, 0.0f, 0.0f},
    {12.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRLine18[] = {
    {32.0f,  true, Decoration_None, 0.0f, 0.0f},
    { 6.0f, false, Decoration_None, 0.0f, 0.0f},
    { 4.0f,  true, Decoration_None, 0.0f, 0.0f},
    { 6.0f, false, Decoration_None, 0.0f, 0.0f},
    { 4.0f,  true, Decoration_None, 0.0f, 0.0f},
    { 6.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRLine19[] = {
    {32.0f,  true, Decoration_None, 0.0f, 0.0f},
    { 6.0f, false, Decoration_None, 0.0f, 0.0f},
    { 4.0f,  true, Decoration_None, 0.0f, 0.0f},
    { 6.0f, false, Decoration_None, 0.0f, 0.0f},
    { 4.0f,  true, Decoration_None, 0.0f, 0.0f},
    { 6.0f, false, Decoration_None, 0.0f, 0.0f},
    { 4.0f,  true, Decoration_None, 0.0f, 0.0f},
    { 6.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRLine20[] = {
    {11.0f,  true, Decoration_None, 0.0f, 0.0f},
    { 5.0f, false, Decoration_None, 0.0f, 0.0f},
    { 1.0f,  true, Decoration_None, 0.0f, 0.0f},
    { 5.0f, false, Decoration_None, 0.0f, 0.0f},
    { 1.0f,  true, Decoration_None, 0.0f, 0.0f},
    { 5.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRLine21[] = {
    {20.0f,  true, Decoration_None, 0.0f, 0.0f},
    { 4.0f, false, Decoration_None, 0.0f, 0.0f},
    { 1.0f,  true, Decoration_None, 0.0f, 0.0f},
    { 4.0f, false, Decoration_None, 0.0f, 0.0f},
    { 1.0f,  true, Decoration_None, 0.0f, 0.0f},
    { 4.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRLine22[] = {
    {20.0f,  true, Decoration_None, 0.0f, 0.0f},
    { 4.0f, false, Decoration_None, 0.0f, 0.0f},
    { 1.0f,  true, Decoration_None, 0.0f, 0.0f},
    { 4.0f, false, Decoration_None, 0.0f, 0.0f},
    { 1.0f,  true, Decoration_None, 0.0f, 0.0f},
    { 4.0f, false, Decoration_None, 0.0f, 0.0f},
    { 1.0f,  true, Decoration_None, 0.0f, 0.0f},
    { 4.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRLine23[] = {
    {6.0f,  true, Decoration_None, 0.0f, 0.0f},
    {3.0f, false, Decoration_None, 0.0f, 0.0f},
    {1.0f,  true, Decoration_None, 0.0f, 0.0f},
    {3.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRLine24[] = {
    {6.0f,  true, Decoration_None, 0.0f, 0.0f},
    {3.0f, false, Decoration_None, 0.0f, 0.0f},
    {1.0f,  true, Decoration_None, 0.0f, 0.0f},
    {3.0f, false, Decoration_None, 0.0f, 0.0f},
    {1.0f,  true, Decoration_None, 0.0f, 0.0f},
    {3.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRLine25[] = {
    {10.0f,  true, Decoration_None, 0.0f, 0.0f},
    { 3.0f, false, Decoration_None, 0.0f, 0.0f},
    { 1.0f,  true, Decoration_None, 0.0f, 0.0f},
    { 3.0f, false, Decoration_None, 0.0f, 0.0f},
    { 4.0f,  true, Decoration_None, 0.0f, 0.0f},
    { 3.0f, false, Decoration_None, 0.0f, 0.0f},
    { 1.0f,  true, Decoration_None, 0.0f, 0.0f},
    { 3.0f, false, Decoration_None, 0.0f, 0.0f}
};


// table of the defined line style definitions - the table
// is indexed using the line style enumeration
static StyleDefinition s_styleDefs[] =
{
    {L"Solid",          PRSolid,          sizeof(PRSolid)          / sizeof(PixelRun)},
    {L"Dash",           PRDash,           sizeof(PRDash)           / sizeof(PixelRun)},
    {L"Dot",            PRDot,            sizeof(PRDot)            / sizeof(PixelRun)},
    {L"DashDot",        PRDashDot,        sizeof(PRDashDot)        / sizeof(PixelRun)},
    {L"DashDotDot",     PRDashDotDot,     sizeof(PRDashDotDot)     / sizeof(PixelRun)},
    {L"Rail",           PRRailroad,       sizeof(PRRailroad)       / sizeof(PixelRun)},
    {L"BORDER",         PRBorder,         sizeof(PRBorder)         / sizeof(PixelRun)},
    {L"BORDER2",        PRBorder2,        sizeof(PRBorder2)        / sizeof(PixelRun)},
    {L"BORDERX2",       PRBorderX2,       sizeof(PRBorderX2)       / sizeof(PixelRun)},
    {L"CENTER",         PRCenter,         sizeof(PRCenter)         / sizeof(PixelRun)},
    {L"CENTER2",        PRCenter2,        sizeof(PRCenter2)        / sizeof(PixelRun)},
    {L"CENTERX2",       PRCenterX2,       sizeof(PRCenterX2)       / sizeof(PixelRun)},
    {L"DASHDOT3",       PRDashDot3,       sizeof(PRDashDot3)       / sizeof(PixelRun)},
    {L"DASHDOT2",       PRDashDot2,       sizeof(PRDashDot2)       / sizeof(PixelRun)},
    {L"DASHDOTX2",      PRDashDotX2,      sizeof(PRDashDotX2)      / sizeof(PixelRun)},
    {L"DASHED",         PRDashed,         sizeof(PRDashed)         / sizeof(PixelRun)},
    {L"DASHED2",        PRDashed2,        sizeof(PRDashed2)        / sizeof(PixelRun)},
    {L"DASHEDX2",       PRDashedX2,       sizeof(PRDashedX2)       / sizeof(PixelRun)},
    {L"DIVIDE",         PRDivide,         sizeof(PRDivide)         / sizeof(PixelRun)},
    {L"DIVIDE2",        PRDivide2,        sizeof(PRDivide2)        / sizeof(PixelRun)},
    {L"DIVIDEX2",       PRDivideX2,       sizeof(PRDivideX2)       / sizeof(PixelRun)},
    {L"DOT3",           PRDot3,           sizeof(PRDot3)           / sizeof(PixelRun)},
    {L"DOT2",           PRDot2,           sizeof(PRDot2)           / sizeof(PixelRun)},
    {L"DOTX2",          PRDotX2,          sizeof(PRDotX2)          / sizeof(PixelRun)},
    {L"HIDDEN",         PRHidden,         sizeof(PRHidden)         / sizeof(PixelRun)},
    {L"HIDDEN2",        PRHidden2,        sizeof(PRHidden2)        / sizeof(PixelRun)},
    {L"HIDDENX2",       PRHiddenX2,       sizeof(PRHiddenX2)       / sizeof(PixelRun)},
    {L"PHANTOM",        PRPhantom,        sizeof(PRPhantom)        / sizeof(PixelRun)},
    {L"PHANTOM2",       PRPhantom2,       sizeof(PRPhantom2)       / sizeof(PixelRun)},
    {L"PHANTOMX2",      PRPhantomX2,      sizeof(PRPhantomX2)      / sizeof(PixelRun)},
    {L"ACAD_ISO02W100", PRACAD_ISO02W100, sizeof(PRACAD_ISO02W100) / sizeof(PixelRun)},
    {L"ACAD_ISO03W100", PRACAD_ISO03W100, sizeof(PRACAD_ISO03W100) / sizeof(PixelRun)},
    {L"ACAD_ISO04W100", PRACAD_ISO04W100, sizeof(PRACAD_ISO04W100) / sizeof(PixelRun)},
    {L"ACAD_ISO05W100", PRACAD_ISO05W100, sizeof(PRACAD_ISO05W100) / sizeof(PixelRun)},
    {L"ACAD_ISO06W100", PRACAD_ISO06W100, sizeof(PRACAD_ISO06W100) / sizeof(PixelRun)},
    {L"ACAD_ISO07W100", PRACAD_ISO07W100, sizeof(PRACAD_ISO07W100) / sizeof(PixelRun)},
    {L"ACAD_ISO08W100", PRACAD_ISO08W100, sizeof(PRACAD_ISO08W100) / sizeof(PixelRun)},
    {L"ACAD_ISO09W100", PRACAD_ISO09W100, sizeof(PRACAD_ISO09W100) / sizeof(PixelRun)},
    {L"ACAD_ISO10W100", PRACAD_ISO10W100, sizeof(PRACAD_ISO10W100) / sizeof(PixelRun)},
    {L"ACAD_ISO11W100", PRACAD_ISO11W100, sizeof(PRACAD_ISO11W100) / sizeof(PixelRun)},
    {L"ACAD_ISO12W100", PRACAD_ISO12W100, sizeof(PRACAD_ISO12W100) / sizeof(PixelRun)},
    {L"ACAD_ISO13W100", PRACAD_ISO13W100, sizeof(PRACAD_ISO13W100) / sizeof(PixelRun)},
    {L"ACAD_ISO14W100", PRACAD_ISO14W100, sizeof(PRACAD_ISO14W100) / sizeof(PixelRun)},
    {L"ACAD_ISO15W100", PRACAD_ISO15W100, sizeof(PRACAD_ISO15W100) / sizeof(PixelRun)},
    {L"FENCELINE1",     PRFenceline1,     sizeof(PRFenceline1)     / sizeof(PixelRun)},
    {L"FENCELINE2",     PRFenceline2,     sizeof(PRFenceline2)     / sizeof(PixelRun)},
    {L"TRACKS",         PRTracks,         sizeof(PRTracks)         / sizeof(PixelRun)},
    {L"LINE03",         PRLine03,         sizeof(PRLine03)         / sizeof(PixelRun)},
    {L"LINE04",         PRLine04,         sizeof(PRLine04)         / sizeof(PixelRun)},
    {L"LINE05",         PRLine05,         sizeof(PRLine05)         / sizeof(PixelRun)},
    {L"LINE06",         PRLine06,         sizeof(PRLine06)         / sizeof(PixelRun)},
    {L"LINE07",         PRLine07,         sizeof(PRLine07)         / sizeof(PixelRun)},
    {L"LINE08",         PRLine08,         sizeof(PRLine08)         / sizeof(PixelRun)},
    {L"LINE09",         PRLine09,         sizeof(PRLine09)         / sizeof(PixelRun)},
    {L"LINE10",         PRLine10,         sizeof(PRLine10)         / sizeof(PixelRun)},
    {L"LINE11",         PRLine11,         sizeof(PRLine11)         / sizeof(PixelRun)},
    {L"LINE12",         PRLine12,         sizeof(PRLine12)         / sizeof(PixelRun)},
    {L"LINE13",         PRLine13,         sizeof(PRLine13)         / sizeof(PixelRun)},
    {L"LINE14",         PRLine14,         sizeof(PRLine14)         / sizeof(PixelRun)},
    {L"LINE15",         PRLine15,         sizeof(PRLine15)         / sizeof(PixelRun)},
    {L"LINE16",         PRLine16,         sizeof(PRLine16)         / sizeof(PixelRun)},
    {L"LINE17",         PRLine17,         sizeof(PRLine17)         / sizeof(PixelRun)},
    {L"LINE18",         PRLine18,         sizeof(PRLine18)         / sizeof(PixelRun)},
    {L"LINE19",         PRLine19,         sizeof(PRLine19)         / sizeof(PixelRun)},
    {L"LINE20",         PRLine20,         sizeof(PRLine20)         / sizeof(PixelRun)},
    {L"LINE21",         PRLine21,         sizeof(PRLine21)         / sizeof(PixelRun)},
    {L"LINE22",         PRLine22,         sizeof(PRLine22)         / sizeof(PixelRun)},
    {L"LINE23",         PRLine23,         sizeof(PRLine23)         / sizeof(PixelRun)},
    {L"LINE24",         PRLine24,         sizeof(PRLine24)         / sizeof(PixelRun)},
    {L"LINE25",         PRLine25,         sizeof(PRLine25)         / sizeof(PixelRun)}
};


// maps used to store custom line styles / decorations
typedef std::map<std::wstring, StyleDefinition*> CUSTOMSTYLES;
typedef std::map<int, DecorationDefinition*> CUSTOMDECORS;
static CUSTOMSTYLES s_customStyles;
static CUSTOMDECORS s_customDecorations;


LineStyleDef::LineStyleDef()
{
    m_nRuns = 0;
    m_pixelRuns = NULL;
    m_decorDefs = NULL;
}


LineStyleDef::~LineStyleDef()
{
    delete [] m_pixelRuns;
    delete [] m_decorDefs;
}


// helper function used by SetStyle
static bool IsDot(PixelRun &pixelRun)
{
    return pixelRun.m_nPixels == 1.0f &&
           pixelRun.m_isOn &&
           pixelRun.m_decor == Decoration_None;
}


LineStyle LineStyleDef::FindLineStyle(const wchar_t* name)
{
    const int len = sizeof(s_styleDefs) / sizeof(StyleDefinition);

    int i=0;

    while (i<len && wcscmp(name, s_styleDefs[i].m_styleName) != 0) i++;

    int style = (i==len)? LineStyle_Solid : i;

    return (LineStyle)style;
}


// Initialize a LineStyleDef for the specified line style,
// drawing scale, and thickness.
//
// Input parameters:
//     lineStyle    - the line style
//     drawingScale - the drawing scale in model units per pixel
//     lineWeight   - the line weight in pixels
void LineStyleDef::SetStyle(LineStyle lineStyle, double drawingScale, double dpi, double lineWeight)
{
    /*
    // check for special case of no line style - cleanup and return
    if (lineStyle == LineStyle_None)
    {
        delete [] m_pixelRuns;
        delete [] m_decorDefs;
        m_nRuns = 0;
        m_pixelRuns = NULL;
        m_decorDefs = NULL;
        return;
    }
    */

    // verify the index
    int styleIndex = (int)lineStyle;
    if (styleIndex < 0 || styleIndex >= (int)(sizeof(s_styleDefs) / sizeof(StyleDefinition)))
        styleIndex = 0;

    SetStyleDef(s_styleDefs[styleIndex], drawingScale, dpi, lineWeight);
}


void LineStyleDef::SetStyle(const wchar_t* lineStyle, double drawingScale, double dpi, double lineWeight)
{
    // get the style definition - default is standard style 0 (solid)
    StyleDefinition* styleDef = &s_styleDefs[0];

    const int len = sizeof(s_styleDefs) / sizeof(StyleDefinition);

    // we generally reuse the same style repeatedly, so optimize for this
    static int lastIndex = -1;
    if (lastIndex >= 0 && lastIndex < len)
    {
        // last style used was a standard style
        if (::wcscmp(lineStyle, s_styleDefs[lastIndex].m_styleName) == 0)
        {
            SetStyleDef(s_styleDefs[lastIndex], drawingScale, dpi, lineWeight);
            return;
        }
    }
    else if (lastIndex == len)
    {
        // last style used was a custom style
        CUSTOMSTYLES::iterator iter = s_customStyles.find(lineStyle);
        if (iter != s_customStyles.end())
        {
            SetStyleDef(*iter->second, drawingScale, dpi, lineWeight);
            return;
        }
    }

    int i=0;
    while (i<len && wcscmp(lineStyle, s_styleDefs[i].m_styleName) != 0) i++;

    if (i < len)
    {
        // a standard style
        styleDef = &s_styleDefs[i];
        lastIndex = i;
    }
    else
    {
        // a custom style
        CUSTOMSTYLES::iterator iter = s_customStyles.find(lineStyle);
        if (iter != s_customStyles.end())
        {
            styleDef = iter->second;
            lastIndex = len;
        }
        else
        {
            lastIndex = -1;
        }
    }

    SetStyleDef(*styleDef, drawingScale, dpi, lineWeight);
}


void LineStyleDef::SetStyleDef(StyleDefinition& styleDef, double drawingScale, double dpi, double lineWeight)
{
    // conversion factor from points to map units
    float ptsToMap = (float)(drawingScale * dpi / 72.0f);

    // allocate space for the scaled definition
    delete [] m_pixelRuns;
    delete [] m_decorDefs;
    m_nRuns = styleDef.m_nRuns;
    m_pixelRuns = new PixelRun[m_nRuns];
    m_decorDefs = NULL; // allocated on demand below

    // compute the scaling factor based on the device resolution
//  if (drawingScale < 1.0f)
//      drawingScale = 1.0f;

    // the line weight in map units
    float scaledWeight = (float)((lineWeight < 0.0f)? -lineWeight : (lineWeight * drawingScale));

    // scale the style definition to map units
    for (int i=0; i<m_nRuns; ++i)
    {
        m_pixelRuns[i] = styleDef.m_pixelRuns[i];
        bool isDot = IsDot(m_pixelRuns[i]);

        m_pixelRuns[i].m_nPixels *= ptsToMap;

        if (!isDot)
        {
            if (lineWeight > 1.0f)
                m_pixelRuns[i].m_nPixels += scaledWeight;

            if (m_pixelRuns[i].m_decor != Decoration_None)
            {
                m_pixelRuns[i].m_decorOffset += 0.5f;
                m_pixelRuns[i].m_decorOffset *= ptsToMap;
//              m_pixelRuns[i].m_decorOffset -= 1.0f;   // to account for line cap?
                m_pixelRuns[i].m_decorSize   += 1.0f;   // added to make size better match OnSite 6
                m_pixelRuns[i].m_decorSize   *= ptsToMap;

                if (lineWeight > 1.0f)
                {
                    m_pixelRuns[i].m_decorOffset += scaledWeight / 2.0f;
                    m_pixelRuns[i].m_decorSize   += scaledWeight;
                }
            }
        }

        // process any custom decoration
        if (m_pixelRuns[i].m_decor >= MIN_DECORATION_ID)
        {
            CUSTOMDECORS::iterator iter = s_customDecorations.find(m_pixelRuns[i].m_decor);
            if (iter != s_customDecorations.end())
            {
                if (m_decorDefs == NULL)
                    m_decorDefs = new DecorationDefinition[m_nRuns];

                // note that segment coordinates are scaled when rendering
                m_decorDefs[i] = *(iter->second);
            }
            else
            {
                // custom decoration not found - reset it to none
                m_pixelRuns[i].m_decor = Decoration_None;
            }
        }
    }
}


std::vector<std::wstring> LineStyleDef::GetLineStyleNames()
{
    std::vector<std::wstring> names;

    // add the standard names
    const int len = sizeof(s_styleDefs) / sizeof(StyleDefinition);
    for (int i=0; i<len; ++i)
    {
        std::wstring name = s_styleDefs[i].m_styleName;
        names.push_back(name);
    }

    // add the user-defined names
    CUSTOMSTYLES::iterator iter = s_customStyles.begin();
    for (; iter != s_customStyles.end(); ++iter)
    {
        std::wstring name = iter->first;
        names.push_back(name);
    }

    return names;
}


bool LineStyleDef::RegisterDecoration(const int decorationID, const int nSegs, const int* segTypes, const float* segCoords)
{
    // validate input
    if (decorationID < MIN_DECORATION_ID || nSegs <= 0 || segTypes == NULL || segCoords == NULL)
        return false;

    // the first segment must be a move-to
    if (segTypes[0] != 0)
        return false;

    // verify the decoration doesn't already exist
    CUSTOMDECORS::iterator iter = s_customDecorations.find(decorationID);
    if (iter != s_customDecorations.end())
        return false;

    // allocate the new definition
    int* pSegTypes = new int[nSegs];
    ::memcpy(pSegTypes, segTypes, nSegs*sizeof(int));

    float* pSegCoords = new float[2*nSegs];
    ::memcpy(pSegCoords, segCoords, 2*nSegs*sizeof(float));

    DecorationDefinition* decorDef = new DecorationDefinition(nSegs, pSegTypes, pSegCoords);

    // insert it into the map
    s_customDecorations.insert(CUSTOMDECORS::value_type(decorationID, decorDef));

    return true;
}


bool LineStyleDef::UnRegisterDecoration(const int decorationID)
{
    // validate input
    if (decorationID < MIN_DECORATION_ID)
        return false;

    // verify the decoration exists
    CUSTOMDECORS::iterator iter = s_customDecorations.find(decorationID);
    if (iter == s_customDecorations.end())
        return false;

    // remove the definition from the map
    DecorationDefinition* decorDef = iter->second;
    s_customDecorations.erase(iter);

    // deallocate the definition
    delete [] decorDef->m_segTypes;
    delete [] decorDef->m_segCoords;
    delete decorDef;

    return true;
}


bool LineStyleDef::RegisterLineStyle(const std::wstring& name, const int nRuns, const PixelRun* pixelRuns)
{
    // validate input
    if (name.empty() || nRuns <= 0 || pixelRuns == NULL)
        return false;

    // verify the style doesn't already exist
    CUSTOMSTYLES::iterator iter = s_customStyles.find(name);
    if (iter != s_customStyles.end())
        return false;

    // allocate the new style
    PixelRun* pPixelRuns = new PixelRun[nRuns];
    ::memcpy(pPixelRuns, pixelRuns, nRuns*sizeof(PixelRun));

    StyleDefinition* styleDef = new StyleDefinition();
    styleDef->m_styleName = NULL;
    styleDef->m_nRuns = nRuns;
    styleDef->m_pixelRuns = pPixelRuns;

    // insert it into the map
    s_customStyles.insert(CUSTOMSTYLES::value_type(name, styleDef));

    return true;
}


bool LineStyleDef::UnRegisterLineStyle(const std::wstring& name)
{
    // validate input
    if (name.empty())
        return false;

    // verify the style exists
    CUSTOMSTYLES::iterator iter = s_customStyles.find(name);
    if (iter == s_customStyles.end())
        return false;

    // remove the definition from the map
    StyleDefinition* styleDef = iter->second;
    s_customStyles.erase(iter);

    // deallocate the definition
    delete [] styleDef->m_pixelRuns;
    delete styleDef;

    return true;
}

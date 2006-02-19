//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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
#include "LineStyle.h"
#include "LineStyleDef.h"

// basic line styles
static const PixelRun PRSolid[] = {
    {1.0f, true, Decoration_None, 0.0f, 0.0f},
    {0.0f, false, Decoration_None, 0.0f, 0.0f}
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
    {8.0f,  true, Decoration_None, 0.0f, 0.0f},
    {0.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRDashDotDot[] = {
    {4.0f,  true, Decoration_None, 0.0f, 0.0f},
    {4.0f, false, Decoration_None, 0.0f, 0.0f},
    {4.0f,  true, Decoration_None, 0.0f, 0.0f},
    {4.0f, false, Decoration_None, 0.0f, 0.0f},
    {4.0f,  true, Decoration_None, 0.0f, 0.0f},
    {4.0f, false, Decoration_None, 0.0f, 0.0f},
    {8.0f,  true, Decoration_None, 0.0f, 0.0f},
    {0.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRRailroad[] = {
    {9.0f, true, Decoration_CrossTick, 5.0f, 2.0f},
    {0.0f, false, Decoration_None, 0.0f, 0.0f}
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
    {25.0f,  true, Decoration_None,   0.0f, 0.0f},
    {0.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRFenceline2[] = {
    {25.0f,  true, Decoration_None,   0.0f, 0.0f},
    { 9.0f, false, Decoration_Square, 9.0f, 4.0f},
    {25.0f,  true, Decoration_None,   0.0f, 0.0f},
    {0.0f, false, Decoration_None, 0.0f, 0.0f}
};

static const PixelRun PRTracks[] = {
    {13.0f, true, Decoration_CrossTick, 25.0f, 6.0f},
    {0.0f, false, Decoration_None, 0.0f, 0.0f}
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


// compact style definitions are represented as arrays of PixelRun
// structures, along with the number of elements in each array
struct StyleDefinition
{
   const PixelRun* m_pixelRuns;
   int m_nRuns;
};


// table of the defined line style definitions - the table
// is indexed using the line style enumeration
static StyleDefinition s_styleDefs[] = {
    {PRSolid,          sizeof(PRSolid)          / sizeof(PixelRun)},
    {PRDash,           sizeof(PRDash)           / sizeof(PixelRun)},
    {PRDot,            sizeof(PRDot)            / sizeof(PixelRun)},
    {PRDashDot,        sizeof(PRDashDot)        / sizeof(PixelRun)},
    {PRDashDotDot,     sizeof(PRDashDotDot)     / sizeof(PixelRun)},
    {PRRailroad,       sizeof(PRRailroad)       / sizeof(PixelRun)},
    {PRBorder,         sizeof(PRBorder)         / sizeof(PixelRun)},
    {PRBorder2,        sizeof(PRBorder2)        / sizeof(PixelRun)},
    {PRBorderX2,       sizeof(PRBorderX2)       / sizeof(PixelRun)},
    {PRCenter,         sizeof(PRCenter)         / sizeof(PixelRun)},
    {PRCenter2,        sizeof(PRCenter2)        / sizeof(PixelRun)},
    {PRCenterX2,       sizeof(PRCenterX2)       / sizeof(PixelRun)},
    {PRDashDot3,       sizeof(PRDashDot3)       / sizeof(PixelRun)},
    {PRDashDot2,       sizeof(PRDashDot2)       / sizeof(PixelRun)},
    {PRDashDotX2,      sizeof(PRDashDotX2)      / sizeof(PixelRun)},
    {PRDashed,         sizeof(PRDashed)         / sizeof(PixelRun)},
    {PRDashed2,        sizeof(PRDashed2)        / sizeof(PixelRun)},
    {PRDashedX2,       sizeof(PRDashedX2)       / sizeof(PixelRun)},
    {PRDivide,         sizeof(PRDivide)         / sizeof(PixelRun)},
    {PRDivide2,        sizeof(PRDivide2)        / sizeof(PixelRun)},
    {PRDivideX2,       sizeof(PRDivideX2)       / sizeof(PixelRun)},
    {PRDot3,           sizeof(PRDot3)           / sizeof(PixelRun)},
    {PRDot2,           sizeof(PRDot2)           / sizeof(PixelRun)},
    {PRDotX2,          sizeof(PRDotX2)          / sizeof(PixelRun)},
    {PRHidden,         sizeof(PRHidden)         / sizeof(PixelRun)},
    {PRHidden2,        sizeof(PRHidden2)        / sizeof(PixelRun)},
    {PRHiddenX2,       sizeof(PRHiddenX2)       / sizeof(PixelRun)},
    {PRPhantom,        sizeof(PRPhantom)        / sizeof(PixelRun)},
    {PRPhantom2,       sizeof(PRPhantom2)       / sizeof(PixelRun)},
    {PRPhantomX2,      sizeof(PRPhantomX2)      / sizeof(PixelRun)},
    {PRACAD_ISO02W100, sizeof(PRACAD_ISO02W100) / sizeof(PixelRun)},
    {PRACAD_ISO03W100, sizeof(PRACAD_ISO03W100) / sizeof(PixelRun)},
    {PRACAD_ISO04W100, sizeof(PRACAD_ISO04W100) / sizeof(PixelRun)},
    {PRACAD_ISO05W100, sizeof(PRACAD_ISO05W100) / sizeof(PixelRun)},
    {PRACAD_ISO06W100, sizeof(PRACAD_ISO06W100) / sizeof(PixelRun)},
    {PRACAD_ISO07W100, sizeof(PRACAD_ISO07W100) / sizeof(PixelRun)},
    {PRACAD_ISO08W100, sizeof(PRACAD_ISO08W100) / sizeof(PixelRun)},
    {PRACAD_ISO09W100, sizeof(PRACAD_ISO09W100) / sizeof(PixelRun)},
    {PRACAD_ISO10W100, sizeof(PRACAD_ISO10W100) / sizeof(PixelRun)},
    {PRACAD_ISO11W100, sizeof(PRACAD_ISO11W100) / sizeof(PixelRun)},
    {PRACAD_ISO12W100, sizeof(PRACAD_ISO12W100) / sizeof(PixelRun)},
    {PRACAD_ISO13W100, sizeof(PRACAD_ISO13W100) / sizeof(PixelRun)},
    {PRACAD_ISO14W100, sizeof(PRACAD_ISO14W100) / sizeof(PixelRun)},
    {PRACAD_ISO15W100, sizeof(PRACAD_ISO15W100) / sizeof(PixelRun)},
    {PRFenceline1,     sizeof(PRFenceline1)     / sizeof(PixelRun)},
    {PRFenceline2,     sizeof(PRFenceline2)     / sizeof(PixelRun)},
    {PRTracks,         sizeof(PRTracks)         / sizeof(PixelRun)},
    {PRLine03,         sizeof(PRLine03)         / sizeof(PixelRun)},
    {PRLine04,         sizeof(PRLine04)         / sizeof(PixelRun)},
    {PRLine05,         sizeof(PRLine05)         / sizeof(PixelRun)},
    {PRLine06,         sizeof(PRLine06)         / sizeof(PixelRun)},
    {PRLine07,         sizeof(PRLine07)         / sizeof(PixelRun)},
    {PRLine08,         sizeof(PRLine08)         / sizeof(PixelRun)},
    {PRLine09,         sizeof(PRLine09)         / sizeof(PixelRun)},
    {PRLine10,         sizeof(PRLine10)         / sizeof(PixelRun)},
    {PRLine11,         sizeof(PRLine11)         / sizeof(PixelRun)},
    {PRLine12,         sizeof(PRLine12)         / sizeof(PixelRun)},
    {PRLine13,         sizeof(PRLine13)         / sizeof(PixelRun)},
    {PRLine14,         sizeof(PRLine14)         / sizeof(PixelRun)},
    {PRLine15,         sizeof(PRLine15)         / sizeof(PixelRun)},
    {PRLine16,         sizeof(PRLine16)         / sizeof(PixelRun)},
    {PRLine17,         sizeof(PRLine17)         / sizeof(PixelRun)},
    {PRLine18,         sizeof(PRLine18)         / sizeof(PixelRun)},
    {PRLine19,         sizeof(PRLine19)         / sizeof(PixelRun)},
    {PRLine20,         sizeof(PRLine20)         / sizeof(PixelRun)},
    {PRLine21,         sizeof(PRLine21)         / sizeof(PixelRun)},
    {PRLine22,         sizeof(PRLine22)         / sizeof(PixelRun)},
    {PRLine23,         sizeof(PRLine23)         / sizeof(PixelRun)},
    {PRLine24,         sizeof(PRLine24)         / sizeof(PixelRun)},
    {PRLine25,         sizeof(PRLine25)         / sizeof(PixelRun)}
};


wchar_t* LineStyleNames[] =
{
    L"Solid",
    L"Dash",
    L"Dot",
    L"DashDot",
    L"DashDotDot",
    L"Rail",
    L"BORDER",
    L"BORDER2",
    L"BORDERX2",
    L"CENTER",
    L"CENTER2",
    L"CENTERX2",
    L"DASHDOT3",
    L"DASHDOT2",
    L"DASHDOTX2",
    L"DASHED",
    L"DASHED2",
    L"DASHEDX2",
    L"DIVIDE",
    L"DIVIDE2",
    L"DIVIDEX2",
    L"DOT3",
    L"DOT2",
    L"DOTX2",
    L"HIDDEN",
    L"HIDDEN2",
    L"HIDDENX2",
    L"PHANTOM",
    L"PHANTOM2",
    L"PHANTOMX2",
    L"ACAD_ISO02W100",
    L"ACAD_ISO03W100",
    L"ACAD_ISO04W100",
    L"ACAD_ISO05W100",
    L"ACAD_ISO06W100",
    L"ACAD_ISO07W100",
    L"ACAD_ISO08W100",
    L"ACAD_ISO09W100",
    L"ACAD_ISO10W100",
    L"ACAD_ISO11W100",
    L"ACAD_ISO12W100",
    L"ACAD_ISO13W100",
    L"ACAD_ISO14W100",
    L"ACAD_ISO15W100",
    L"FENCELINE1",
    L"FENCELINE2",
    L"TRACKS",
    L"LINE03",
    L"LINE04",
    L"LINE05",
    L"LINE06",
    L"LINE07",
    L"LINE08",
    L"LINE09",
    L"LINE10",
    L"LINE11",
    L"LINE12",
    L"LINE13",
    L"LINE14",
    L"LINE15",
    L"LINE16",
    L"LINE17",
    L"LINE18",
    L"LINE19",
    L"LINE20",
    L"LINE21",
    L"LINE22",
    L"LINE23",
    L"LINE24",
    L"LINE25"
};



LineStyleDef::LineStyleDef()
{
    m_nRuns = 0;
    m_pixelRuns = NULL;
}

LineStyleDef::~LineStyleDef()
{
    delete [] m_pixelRuns;
}


// helper function used by SetStyle
static bool IsDot(PixelRun &pixelRun)
{
    return pixelRun.m_nPixels == 1.0f &&
           pixelRun.m_isOn &&
           pixelRun.m_decor == Decoration_None;
}


void LineStyleDef::SetStyle(const wchar_t* lineStyle, double drawingScale, double dpi, double lineWeight)
{
    SetStyle(FindLineStyle(lineStyle), drawingScale, dpi, lineWeight);
}

LineStyle LineStyleDef::FindLineStyle(const wchar_t* name)
{
    int len = sizeof(LineStyleNames) / sizeof(wchar_t*);

    int i=0;

    while (i<len && wcscmp(name, LineStyleNames[i]) != 0) i++;

    int style = (i==len) ? LineStyle_Solid : i;

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
        if (m_pixelRuns)
            delete [] m_pixelRuns;
        m_nRuns = 0;
        m_pixelRuns = NULL;
        return;
    }
    */

    // must have a valid line style = check parameters for validity
    //_ASSERT(drawingScale >  0.0f);
//  Debug::Assert(lineWeight   >= 0.0f);

    // verify the index
    int styleIndex = (int)lineStyle;
    if (styleIndex < 0 || styleIndex >= (int)(sizeof(s_styleDefs) / sizeof(StyleDefinition)))
        styleIndex = 0;

    // conversion factor from points to map units
    float ptsToMap = (float)(drawingScale * dpi / 72.0f);

    // allocate space for the scaled definition
    if (m_pixelRuns)
        delete [] m_pixelRuns;
    m_nRuns = s_styleDefs[styleIndex].m_nRuns;
    m_pixelRuns = new PixelRun[m_nRuns];

    // compute the scaling factor based on the device resolution
//  if (drawingScale < 1.0f)
//      drawingScale = 1.0f;

    // the line weight in map units
    float scaledWeight = (float)((lineWeight < 0.0f) ? -lineWeight : (lineWeight * ptsToMap));

    // scale the style definition to map units
    for (int i=0; i<m_nRuns; i++)
    {
        m_pixelRuns[i] = s_styleDefs[styleIndex].m_pixelRuns[i];
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
    }
}


//converts from MapGuide pattern to a WHIP toolkit dash pattern.
//Note that this cannot handle decorated patterns --> these are special cased
//in WT_Line_Pattern
STYLIZATION_API int LineStyleDef::ConvertToDashPattern(const wchar_t* lineStyleName, double dpi, double lineWeightPixels, WT_Dash_Pattern& dash, WT_Line_Pattern& lpat)
{
    //first detect decorated line patters -- these are
    //hardcoded/defined as WHIP WT_Line_Patterns
    if (_wcsicmp(lineStyleName, L"FENCELINE1") == 0)
    {
        lpat.set(WT_Line_Pattern::Decorated_Circle_Fence);
        return WT_Line_Pattern::Decorated_Circle_Fence;
    }
    else if (_wcsicmp(lineStyleName, L"FENCELINE2") == 0)
    {
        lpat.set(WT_Line_Pattern::Decorated_Square_Fence);
        return WT_Line_Pattern::Decorated_Square_Fence;
    }
    else if (_wcsicmp(lineStyleName, L"TRACKS") == 0)
    {
        lpat.set(WT_Line_Pattern::Decorated_Wide_Tracks);
        return WT_Line_Pattern::Decorated_Wide_Tracks;
    }
    else if (_wcsicmp(lineStyleName, L"Rail") == 0)
    {
        lpat.set(WT_Line_Pattern::Decorated_Tracks);
        return WT_Line_Pattern::Decorated_Tracks;
    }

    //for all other patterns we will use a custom defined
    //WT_Dash_Pattern

    LineStyle lineStyle = FindLineStyle(lineStyleName);

    // verify the index
    int styleIndex = (int)lineStyle;
    if (styleIndex < 0 || styleIndex >= (int)(sizeof(s_styleDefs) / sizeof(StyleDefinition)))
        styleIndex = 0;

    // conversion factor from points to pixels
    float ptsToMap = (float)(dpi / 72.0f);

    // allocate space for the scaled definition
    if (m_pixelRuns)
        delete [] m_pixelRuns;
    m_nRuns = s_styleDefs[styleIndex].m_nRuns;
    m_pixelRuns = new PixelRun[m_nRuns];

    // scale the style definition to pixel units
    for (int i=0; i<m_nRuns; i++)
    {
        m_pixelRuns[i] = s_styleDefs[styleIndex].m_pixelRuns[i];
        bool isDot = IsDot(m_pixelRuns[i]);

        m_pixelRuns[i].m_nPixels *= ptsToMap;

        if (!isDot)
        {
            if (lineWeightPixels > 1.0f)
                m_pixelRuns[i].m_nPixels += (float)lineWeightPixels;
        }
    }

    //convert to WHIP values for the on/off pixels for the custom dash pattern
    WT_Integer32 id = lineStyle + WT_Line_Pattern::Count + 1;
    WT_Integer16* runs = new WT_Integer16[m_nRuns];

    for (int i=0; i<m_nRuns; i++)
        runs[i] = (WT_Integer16)m_pixelRuns[i].m_nPixels;

    dash.set(id, (WT_Integer16)m_nRuns, runs);

    delete [] runs;

    return id;
}

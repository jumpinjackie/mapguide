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

#include "dwf/whiptk/whip_toolkit.h"

// enumeration defining the supported decorations
enum Decoration
{
    Decoration_None      = 0,
    Decoration_CrossTick = 1,
    Decoration_Square    = 2,
    Decoration_Circle    = 3
};


// PixelRun structures are used to define the line styles
// in a compact fashion.
struct PixelRun
{
    float m_nPixels;        // number of pixels in run
    bool m_isOn;            // are pixels on or off
    Decoration m_decor;     // each run can have a single decoration
    float m_decorSize;      // size of the decoration if any
    float m_decorOffset;    // pixel offset of decoration from start of run
};


// stores all data needed to represent a line style
class LineStyleDef
{
public:
    STYLIZATION_API LineStyleDef();
    STYLIZATION_API ~LineStyleDef();
    STYLIZATION_API void SetStyle(LineStyle lineStyle, double drawingScale, double dpi, double lineWeight);
    STYLIZATION_API void SetStyle(const wchar_t* lineStyle, double drawingScale, double dpi, double lineWeight);
    STYLIZATION_API LineStyle FindLineStyle(const wchar_t* name);
    STYLIZATION_API int ConvertToDashPattern(const wchar_t* lineStyleName, double dpi, double lineWeightPixels, WT_Dash_Pattern& dash, WT_Line_Pattern& lpat);

    int m_nRuns;            // number of elements in m_pixelRuns array
    PixelRun* m_pixelRuns;  // definition of the line style as pixel runs
};

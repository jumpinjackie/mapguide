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

#ifndef LINESTYLEDEF_H_
#define LINESTYLEDEF_H_

#include "StylizationDefs.h"
#include "StylizationAPI.h"
#include "LineStyle.h"
#include <vector>


// enumeration defining the standard supported decorations
enum Decoration
{
    Decoration_None      = 0,
    Decoration_CrossTick = 1,
    Decoration_Square    = 2,
    Decoration_Circle    = 3
};


// PixelRun structures are used to define the line styles in a compact
// fashion.  Note that pixel run data is defined in points - the name
// is misleading.
struct PixelRun
{
    float m_nPixels;        // number of pixels in run
    bool m_isOn;            // are pixels on or off
    Decoration m_decor;     // each run can have a single decoration
    float m_decorSize;      // size of the decoration if any
    float m_decorOffset;    // pixel offset of decoration from start of run
};


// Compact style definitions are represented as arrays of PixelRun
// structures, along with the number of elements in each array.
struct StyleDefinition
{
    const wchar_t* m_styleName;
    const PixelRun* m_pixelRuns;
    int m_nRuns;
};


// Custom decoration definitions are represented as arrays of segment
// types and coordinates, along with the number of segments.
struct DecorationDefinition
{
    int m_nSegs;
    const int* m_segTypes;
    const float* m_segCoords;

    DecorationDefinition()
    {
        m_nSegs = 0;
        m_segTypes = NULL;
        m_segCoords = NULL;
    }

    DecorationDefinition(int nSegs, int* pSegTypes, float* pSegCoords)
    {
        m_nSegs = nSegs;
        m_segTypes = pSegTypes;
        m_segCoords = pSegCoords;
    }
};


// stores all data needed to represent a line style
class LineStyleDef
{
public:
    STYLIZATION_API LineStyleDef();
    STYLIZATION_API ~LineStyleDef();
    STYLIZATION_API LineStyle FindLineStyle(const wchar_t* name);
    STYLIZATION_API void SetStyle(LineStyle lineStyle, double drawingScale, double dpi, double lineWeight);

    // this method is not thread-safe
    STYLIZATION_API void SetStyle(const wchar_t* lineStyle, double drawingScale, double dpi, double lineWeight);

    // ****************************************************************************

    // Returns a vector containing the names of all standard and custom line styles.
    STYLIZATION_API static std::vector<std::wstring> GetLineStyleNames();

    // user defined decorations must have index >= 1000
    #define MIN_DECORATION_ID 1000

    // Registers a custom decoration.  The return value indicates whether the
    // registration succeeded.
    //   decorationID   The unique ID to associate with the decoration.  Must be >= MIN_DECORATION_ID.
    //   nSegs          The number of segments (MoveTo / LineTo) in the decoration.
    //   segTypes       An array of length nSegs specifying the segment types (0=MoveTo, 1=LineTo).
    //                  A copy of the supplied array is made.
    //   segCoords      An array of length 2*nSegs containing the segment coordinates, stored as
    //                  x0,y0,x1,y1, ...  A copy of the supplied array is made.
    STYLIZATION_API static bool RegisterDecoration(const int decorationID, const int nSegs, const int* segTypes, const float* segCoords);

    // Unregisters a custom decoration.  Any resources associated with the decoration
    // are freed.  The return value indicates whether the unregistration succeeded.
    STYLIZATION_API static bool UnRegisterDecoration(const int decorationID);

    // Registers a custom line style.  The return value indicates whether the
    // registration succeeded.
    //   name           The unique name to associate with the line style.
    //   nRuns          The number of runs in the line style.
    //   pixelRuns      An array of PixelRun elements descrbing the line style.  A copy
    //                  of the supplied array is made.
    STYLIZATION_API static bool RegisterLineStyle(const std::wstring& name, const int nRuns, const PixelRun* pixelRuns);

    // Unregisters a custom line style.  Any resources associated with the line style
    // are freed.  The return value indicates whether the unregistration succeeded.
    STYLIZATION_API static bool UnRegisterLineStyle(const std::wstring& name);

    // ****************************************************************************

    int m_nRuns;                        // number of elements in m_pixelRuns array
    PixelRun* m_pixelRuns;              // definition of the line style as pixel runs
    DecorationDefinition* m_decorDefs;  // definition of any custom decorations

private:
    void SetStyleDef(StyleDefinition& styleDef, double drawingScale, double dpi, double lineWeight);
};

#endif

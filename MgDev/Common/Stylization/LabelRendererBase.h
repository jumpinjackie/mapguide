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

#ifndef LABELRENDERERBASE_H
#define LABELRENDERERBASE_H

#include "RendererStyles.h"
#include "Bounds.h"

class GDRenderer;
class LineBuffer;
struct RS_Font;


//////////////////////////////////////////////////////////////////////////////
class LabelRendererBase
{

public:
    LabelRendererBase(GDRenderer* renderer);

    virtual ~LabelRendererBase() {};

    virtual void StartLabels() = 0;

    virtual void ProcessLabel(double x, double y,
                              const RS_String& text, RS_TextDef& tdef) = 0;

    virtual void ProcessLabelGroup(RS_LabelInfo*    labels,
                                   int              nlabels,
                                   const RS_String& text,
                                   RS_OverpostType  type,
                                   bool             exclude,
                                   LineBuffer*      path) = 0;

    virtual void BlastLabels() = 0;

    virtual void AddExclusionRegion(RS_F_Point* pts, int npts) = 0;

protected:
    size_t SplitLabel(wchar_t* label, std::vector<wchar_t*>& line_breaks);

    double GetHorizontalAlignmentOffset(RS_TextDef& tdef, RS_F_Point* extent);
    double GetVerticalAlignmentOffset(RS_TextDef& tdef, const RS_Font* font, double actual_height, double line_height, size_t numLines);

    void DeviceToMappingBounds(RS_Bounds& b);
    void ComputeBounds(RS_F_Point* RESTRICT pts, int npts, RS_Bounds& b);
    void RotatedBounds(double x, double y, double width, double height, double angle_cw_rad, RS_F_Point* b);

    bool CloseEnough(RS_F_Point& p1, RS_F_Point& p2);

protected:
    GDRenderer* m_renderer;
};

#endif

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

#ifndef LABELRENDERERBASE_H_
#define LABELRENDERERBASE_H_

#include "RendererStyles.h"
#include "SE_LineBuffer.h"

class SE_LabelInfo;
class SE_Renderer;
class RS_TextMetrics;


//////////////////////////////////////////////////////////////////////////////
class LabelRendererBase
{
public:
    LabelRendererBase(SE_Renderer* se_renderer);

    virtual ~LabelRendererBase() {}

    virtual void StartLabels() = 0;

    // RS labels
    virtual void ProcessLabelGroup(RS_LabelInfo*    labels,
                                   int              nlabels,
                                   const RS_String& text,
                                   RS_OverpostType  type,
                                   bool             exclude,
                                   LineBuffer*      path,
                                   double           scaleLimit) = 0;

    // SE labels
    virtual void ProcessLabelGroup(SE_LabelInfo*    labels,
                                   int              nlabels,
                                   RS_OverpostType  type,
                                   bool             exclude,
                                   LineBuffer*      path) = 0;

    virtual void BlastLabels() = 0;

    virtual void AddExclusionRegion(RS_F_Point* pts, int npts) = 0;

protected:
    void GetRotatedTextPoints(RS_TextMetrics& tm, double insx, double insy, double angleRad, RS_F_Point* rotatedPts);
    void GetRotatedPoints(double x, double y, double width, double height, double angleRad, RS_F_Point* rotatedPts);
    bool CloseEnough(RS_F_Point& pt1, RS_F_Point& pt2);
    double MeterToMapSize(RS_Units units, double number);

protected:
    SE_Renderer* m_serenderer;
};

#endif

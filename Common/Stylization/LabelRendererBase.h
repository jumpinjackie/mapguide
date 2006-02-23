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

class LineBuffer;


//////////////////////////////////////////////////////////////////////////////
class LabelRendererBase
{

public:

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
};

#endif

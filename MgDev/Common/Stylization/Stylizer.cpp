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
#include "Stylizer.h"

///<summary>
/// Finds a scale range in the given collection which contains the given map scale
/// Returns NULL if none of the ranges in the collection match the map scale
///</summary>
MdfModel::VectorScaleRange* Stylizer::FindScaleRange(MdfModel::VectorScaleRangeCollection& src, double mapScale)
{
    for (int i=0; i<src.GetCount(); ++i)
    {
        MdfModel::VectorScaleRange* r = src.GetAt(i);

        if (mapScale >= r->GetMinScale() && mapScale < r->GetMaxScale())
            return r;
    }

    return NULL;
}

///<summary>
/// Finds a scale range in the given collection which contains the given map scale
/// Returns NULL if none of the ranges in the collection match the map scale
///</summary>
MdfModel::GridScaleRange* Stylizer::FindScaleRange(MdfModel::GridScaleRangeCollection& src, double mapScale)
{
    for (int i=0; i<src.GetCount(); ++i)
    {
        MdfModel::GridScaleRange* r = src.GetAt(i);

        if (mapScale >= r->GetMinScale() && mapScale < r->GetMaxScale())
            return r;
    }

    return NULL;
}

// Non-fatal stylizer exception logging mechanism.
static StylizerExceptionCallback g_stylizerExceptionCallback = NULL;

void SetStylizerExceptionCallback(StylizerExceptionCallback callbackFunction)
{
    g_stylizerExceptionCallback = callbackFunction;
}

void ProcessStylizerException(FdoException* exception, int line, wchar_t* file)
{
    if (NULL != g_stylizerExceptionCallback)
    {
        (*g_stylizerExceptionCallback)(exception, line, file);
    }
    exception->Release();
}



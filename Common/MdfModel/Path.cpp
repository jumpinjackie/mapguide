//
//  Copyright (C) 2007 by Autodesk, Inc.
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

//-------------------------------------------------------------------------
// DESCRIPTION:
// The Path class implementation.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "Path.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Initialize an instance of the Path class.
//-------------------------------------------------------------------------
Path::Path()
{
    // NOTE: if you change any defaults then you must also update
    //       IOPath::Write to use the correct default values.
    this->m_eLineCap = CapRound;
    this->m_eLineJoin = JoinRound;
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Delete all objects that have been created on the
//          heap or have been adopted.
//-------------------------------------------------------------------------
Path::~Path()
{
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& Path::GetGeometry() const
{
    return m_sGeometry;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void Path::SetGeometry(const MdfString& geometry)
{
    m_sGeometry = geometry;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& Path::GetFillColor() const
{
    return m_sFillColor;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void Path::SetFillColor(const MdfString& fillColor)
{
    m_sFillColor = fillColor;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& Path::GetLineColor() const
{
    return m_sLineColor;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void Path::SetLineColor(const MdfString& lineColor)
{
    m_sLineColor = lineColor;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& Path::GetLineWeight() const
{
    return m_sLineWeight;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void Path::SetLineWeight(const MdfString& lineWeight)
{
    m_sLineWeight = lineWeight;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& Path::GetLineWeightScalable() const
{
    return m_sLineWeightScalable;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void Path::SetLineWeightScalable(const MdfString& lineWeightScalable)
{
    m_sLineWeightScalable = lineWeightScalable;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
Path::LineCap Path::GetLineCap() const
{
    return m_eLineCap;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void Path::SetLineCap(LineCap lineCap)
{
    m_eLineCap = lineCap;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
Path::LineJoin Path::GetLineJoin() const
{
    return m_eLineJoin;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void Path::SetLineJoin(LineJoin lineJoin)
{
    m_eLineJoin = lineJoin;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& Path::GetLineMiterLimit() const
{
    return m_sLineMiterLimit;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void Path::SetLineMiterLimit(const MdfString& lineMiterLimit)
{
    m_sLineMiterLimit = lineMiterLimit;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void Path::AcceptVisitor(IGraphicElementVisitor& igeVisitor)
{
    igeVisitor.VisitPath(*this);
}

//
//  Copyright (C) 2007-2011 by Autodesk, Inc.
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

// initialize values for string properties
const wchar_t* Path::sLineCapDefault  = L"'Round'";                    // NOXLATE
const wchar_t* Path::sLineCapValues   = L"None|Round|Triangle|Square"; // NOXLATE
const wchar_t* Path::sLineJoinDefault = L"'Round'";                    // NOXLATE
const wchar_t* Path::sLineJoinValues  = L"None|Bevel|Round|Miter";     // NOXLATE


//-------------------------------------------------------------------------
// PURPOSE: Initialize an instance of the Path class.
//-------------------------------------------------------------------------
Path::Path()
{
    // default values
    // NOTE: values used in IOPath::Write must match these
//  this->m_sGeometry           = L"";     // NOXLATE
//  this->m_sFillColor          = L"";     // NOXLATE
//  this->m_sLineColor          = L"";     // NOXLATE
    this->m_sLineWeight         = L"0.0";  // NOXLATE
    this->m_sLineWeightScalable = L"true"; // NOXLATE
    this->m_sLineCap            = Path::sLineCapDefault;
    this->m_sLineJoin           = Path::sLineJoinDefault;
    this->m_sLineMiterLimit     = L"5.0";  // NOXLATE
    this->m_sScaleX             = L"1.0";  // NOXLATE
    this->m_sScaleY             = L"1.0";  // NOXLATE
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
    return this->m_sGeometry;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void Path::SetGeometry(const MdfString& geometry)
{
    this->m_sGeometry = geometry;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& Path::GetFillColor() const
{
    return this->m_sFillColor;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void Path::SetFillColor(const MdfString& fillColor)
{
    this->m_sFillColor = fillColor;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& Path::GetLineColor() const
{
    return this->m_sLineColor;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void Path::SetLineColor(const MdfString& lineColor)
{
    this->m_sLineColor = lineColor;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& Path::GetLineWeight() const
{
    return this->m_sLineWeight;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void Path::SetLineWeight(const MdfString& lineWeight)
{
    this->m_sLineWeight = lineWeight;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& Path::GetLineWeightScalable() const
{
    return this->m_sLineWeightScalable;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void Path::SetLineWeightScalable(const MdfString& lineWeightScalable)
{
    this->m_sLineWeightScalable = lineWeightScalable;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& Path::GetLineCap() const
{
    return this->m_sLineCap;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void Path::SetLineCap(const MdfString& lineCap)
{
    this->m_sLineCap = lineCap;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& Path::GetLineJoin() const
{
    return this->m_sLineJoin;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void Path::SetLineJoin(const MdfString& lineJoin)
{
    this->m_sLineJoin = lineJoin;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& Path::GetLineMiterLimit() const
{
    return this->m_sLineMiterLimit;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void Path::SetLineMiterLimit(const MdfString& lineMiterLimit)
{
    this->m_sLineMiterLimit = lineMiterLimit;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& Path::GetScaleX() const
{
    return this->m_sScaleX;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void Path::SetScaleX(const MdfString& scaleX)
{
    this->m_sScaleX = scaleX;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& Path::GetScaleY() const
{
    return this->m_sScaleY;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void Path::SetScaleY(const MdfString& scaleY)
{
    this->m_sScaleY = scaleY;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void Path::AcceptVisitor(IGraphicElementVisitor& igeVisitor)
{
    igeVisitor.VisitPath(*this);
}

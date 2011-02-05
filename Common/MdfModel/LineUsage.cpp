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
// The LineUsage class implementation.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "LineUsage.h"

using namespace MDFMODEL_NAMESPACE;

// initialize values for string properties
const wchar_t* LineUsage::sAngleControlDefault    = L"'FromGeometry'";         // NOXLATE
const wchar_t* LineUsage::sAngleControlValues     = L"FromAngle|FromGeometry"; // NOXLATE
const wchar_t* LineUsage::sUnitsControlDefault    = L"'Absolute'";             // NOXLATE
const wchar_t* LineUsage::sUnitsControlValues     = L"Absolute|Parametric";    // NOXLATE
const wchar_t* LineUsage::sVertexControlDefault10 = L"'OverlapNone'";          // NOXLATE - default for version 1.0.0
const wchar_t* LineUsage::sVertexControlDefault   = L"'OverlapWrap'";          // NOXLATE - default for version 1.1.0 and higher
const wchar_t* LineUsage::sVertexControlValues    = L"OverlapNone|OverlapDirect|OverlapNoWrap|OverlapWrap"; // NOXLATE
const wchar_t* LineUsage::sVertexJoinDefault      = L"'Round'";                // NOXLATE
const wchar_t* LineUsage::sVertexJoinValues       = L"None|Bevel|Round|Miter"; // NOXLATE


//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
LineUsage::LineUsage()
{
    // default values
    // NOTE: values used in IOLineUsage::Write must match these
    this->m_sAngleControl     = LineUsage::sAngleControlDefault;
    this->m_sUnitsControl     = LineUsage::sUnitsControlDefault;
    this->m_sVertexControl    = LineUsage::sVertexControlDefault;
    this->m_sAngle            = L"0.0"; // NOXLATE
//  this->m_sStartOffset      = L"";    // NOXLATE
//  this->m_sEndOffset        = L"";    // NOXLATE
    this->m_sRepeat           = L"0.0"; // NOXLATE
    this->m_sVertexAngleLimit = L"0.0"; // NOXLATE
    this->m_sVertexJoin       = LineUsage::sVertexJoinDefault;
    this->m_sVertexMiterLimit = L"5.0"; // NOXLATE
    this->m_pDefaultPath = NULL;
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Delete all objects that have been created on the
//          heap or have been adopted.
//-------------------------------------------------------------------------
LineUsage::~LineUsage()
{
    delete this->m_pDefaultPath;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& LineUsage::GetAngleControl() const
{
    return this->m_sAngleControl;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void LineUsage::SetAngleControl(const MdfString& angleControl)
{
    this->m_sAngleControl = angleControl;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& LineUsage::GetUnitsControl() const
{
    return this->m_sUnitsControl;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void LineUsage::SetUnitsControl(const MdfString& unitsControl)
{
    this->m_sUnitsControl = unitsControl;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& LineUsage::GetVertexControl() const
{
    return this->m_sVertexControl;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void LineUsage::SetVertexControl(const MdfString& vertexControl)
{
    this->m_sVertexControl = vertexControl;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& LineUsage::GetAngle() const
{
    return this->m_sAngle;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void LineUsage::SetAngle(const MdfString& angle)
{
    this->m_sAngle = angle;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& LineUsage::GetStartOffset() const
{
    return this->m_sStartOffset;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void LineUsage::SetStartOffset(const MdfString& startOffset)
{
    this->m_sStartOffset = startOffset;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& LineUsage::GetEndOffset() const
{
    return this->m_sEndOffset;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void LineUsage::SetEndOffset(const MdfString& endOffset)
{
    this->m_sEndOffset = endOffset;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& LineUsage::GetRepeat() const
{
    return this->m_sRepeat;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void LineUsage::SetRepeat(const MdfString& repeat)
{
    this->m_sRepeat = repeat;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& LineUsage::GetVertexAngleLimit() const
{
    return this->m_sVertexAngleLimit;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void LineUsage::SetVertexAngleLimit(const MdfString& vertexAngleLimit)
{
    this->m_sVertexAngleLimit = vertexAngleLimit;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& LineUsage::GetVertexJoin() const
{
    return this->m_sVertexJoin;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void LineUsage::SetVertexJoin(const MdfString& vertexJoin)
{
    this->m_sVertexJoin = vertexJoin;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& LineUsage::GetVertexMiterLimit() const
{
    return this->m_sVertexMiterLimit;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void LineUsage::SetVertexMiterLimit(const MdfString& vertexMiterLimit)
{
    this->m_sVertexMiterLimit = vertexMiterLimit;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
Path* LineUsage::GetDefaultPath()
{
    return this->m_pDefaultPath;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void LineUsage::AdoptDefaultPath(Path* defaultPath)
{
    if (this->m_pDefaultPath != defaultPath)
    {
        delete this->m_pDefaultPath;
        this->m_pDefaultPath = defaultPath;
    }
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
Path* LineUsage::OrphanDefaultPath()
{
    Path* pRet = this->m_pDefaultPath;
    this->m_pDefaultPath = NULL;
    return pRet;
}

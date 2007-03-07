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
// The LineUsage class implementation.
//-------------------------------------------------------------------------

#include "LineUsage.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
LineUsage::LineUsage()
{
    // NOTE: if you change any defaults then you must also update
    //       IOLineUsage::Write to use the correct default values.
    this->m_pDefaultPath = NULL;
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Delete all objects that have been created on the
//          heap or have been adopted.
//-------------------------------------------------------------------------
LineUsage::~LineUsage()
{
    if (this->m_pDefaultPath != NULL)
        delete this->m_pDefaultPath;
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
        if (this->m_pDefaultPath != NULL)
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

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void LineUsage::AcceptVisitor(IUsageVisitor& iuVisitor)
{
    iuVisitor.VisitLineUsage(*this);
}

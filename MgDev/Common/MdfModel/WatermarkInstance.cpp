//
//  Copyright (C) 2010-2011 by Autodesk, Inc.
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
// WatermarkInstance Class implementation.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "WatermarkInstance.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the WatermarkInstance class.
// PARAMETERS:
//     Input:
//          strName - the unique WatermarkInstance name. Cannot be an empty string.
//          strResourceId - the resource ID for the watermark definition.
//-------------------------------------------------------------------------
WatermarkInstance::WatermarkInstance(const MdfString& strName, const MdfString& strResourceId)
{
    this->m_strName = strName;
    this->m_strResourceId = strResourceId;
    this->m_usage = All;
    this->m_appearanceOverride = NULL;
    this->m_positionOverride = NULL;
    this->m_pWatermarkDefinition = NULL;
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Release any dynamically allocated resources.
//-------------------------------------------------------------------------
WatermarkInstance::~WatermarkInstance()
{
    delete this->m_appearanceOverride;
    delete this->m_positionOverride;
    delete this->m_pWatermarkDefinition;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor for the unique WatermarkInstance name.
//-------------------------------------------------------------------------
const MdfString& WatermarkInstance::GetName() const
{
    return this->m_strName;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor to the WatermarkInstance name.
// PARAMETERS:
//     Input:
//          strName - unique WatermarkInstance name
//-------------------------------------------------------------------------
void WatermarkInstance::SetName(const MdfString& strName)
{
    this->m_strName = strName;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor for the WatermarkDefinition resource ID that this WatermarkInstance references.
// RETURNS:
//-------------------------------------------------------------------------
const MdfString& WatermarkInstance::GetResourceId() const
{
    return this->m_strResourceId;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor to the WatermarkDefinition Uri that this WatermarkInstance references.
// PARAMETERS:
//      Input:
//          strResourceId - the watermark resource ID for this WatermarkInstance
//-------------------------------------------------------------------------
void WatermarkInstance::SetResourceId(const MdfString& strResourceId)
{
    this->m_strResourceId = strResourceId;
}

//-------------------------------------------------------------------------
// PURPOSE: Gets a value determining the meaning of the Usage property value.
// RETURNS: The type of value stored in the Usage property.
//-------------------------------------------------------------------------
const WatermarkInstance::Usage WatermarkInstance::GetUsage() const
{
    return this->m_usage;
}

//-------------------------------------------------------------------------
// PURPOSE: Sets a value determining the meaning of the Usage property value.
// PARAMETERS:
//          unit - The type of value stored in the Usage property.
//-------------------------------------------------------------------------
void WatermarkInstance::SetUsage(WatermarkInstance::Usage usage)
{
    this->m_usage = usage;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the appearance override property.
//          The WatermarkAppearance is the type of appearance override in watermark instance.
// RETURNS: The pointer of watermark appearance override.
//-------------------------------------------------------------------------
const WatermarkAppearance* WatermarkInstance::GetAppearanceOverride() const
{
    return this->m_appearanceOverride;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the appearance override property.
//          The WatermarkAppearance is the type of appearance override in watermark instance.
// RETURNS: The pointer of watermark appearance override.
//-------------------------------------------------------------------------
WatermarkAppearance* WatermarkInstance::GetAppearanceOverride()
{
    return this->m_appearanceOverride;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the appearance override property.
//          The WatermarkAppearance is the type of appearance override in watermark instance.
// PARAMETERS:
//        Input:
//          pAppearanceOverride - Adopted WatermarkAppearance object that is created on the heap.
//                             It may be NULL.
//-------------------------------------------------------------------------
void WatermarkInstance::AdoptAppearanceOverride(WatermarkAppearance* pAppearanceOverride)
{
    if (this->m_appearanceOverride != pAppearanceOverride)
    {
        delete this->m_appearanceOverride;
        this->m_appearanceOverride = pAppearanceOverride;
    }
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the appearance override property.
//          The WatermarkAppearance is the type of appearance override in watermark instance.
// RETURNS: The pointer to the orphaned WatermarkAppearance object. It may be NULL.
//-------------------------------------------------------------------------
WatermarkAppearance* WatermarkInstance::OrphanAppearanceOverride()
{
    WatermarkAppearance* pRet = this->m_appearanceOverride;
    this->m_appearanceOverride = NULL;
    return pRet;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the position override property.
//          The WatermarkPosition is the type of position override in watermark instance.
// RETURNS: The pointer of watermark position override.
//-------------------------------------------------------------------------
const WatermarkPosition* WatermarkInstance::GetPositionOverride() const
{
    return this->m_positionOverride;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the position override property.
//          The WatermarkPosition is the type of position override in watermark instance.
// RETURNS: The pointer of watermark position override.
//-------------------------------------------------------------------------
WatermarkPosition* WatermarkInstance::GetPositionOverride()
{
    return this->m_positionOverride;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the position property override.
//          The WatermarkPosition is the type of position override in watermark instance.
// PARAMETERS:
//        Input:
//          pPosition - Adopted WatermarkPosition object that is created on the heap.
//                             It may be NULL.
//-------------------------------------------------------------------------
void WatermarkInstance::AdoptPositionOverride(WatermarkPosition* pPositionOverride)
{
    if (this->m_positionOverride != pPositionOverride)
    {
        delete this->m_positionOverride;
        this->m_positionOverride = pPositionOverride;
    }
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the position property override.
//          The WatermarkPosition is the type of position override in watermark instance.
// RETURNS: The pointer to the orphaned WatermarkPosition object. It may be NULL.
//-------------------------------------------------------------------------
WatermarkPosition* WatermarkInstance::OrphanPositionOverride()
{
    WatermarkPosition* pRet = this->m_positionOverride;
    this->m_positionOverride = NULL;
    return pRet;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the watermark definition property.
//          The WatermarkDefinition is the type of watermark definition.
// RETURNS: The pointer of watermark definition.
//-------------------------------------------------------------------------
const WatermarkDefinition* WatermarkInstance::GetWatermarkDefinition() const
{
    return this->m_pWatermarkDefinition;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the watermark definition property.
//          The WatermarkDefinition is the type of watermark definition.
// RETURNS: The pointer of watermark definition.
//-------------------------------------------------------------------------
WatermarkDefinition* WatermarkInstance::GetWatermarkDefinition()
{
    return this->m_pWatermarkDefinition;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the watermark definition property.
//          The WatermarkDefinition is the type of watermark definition.
// PARAMETERS:
//        Input:
//          pWatermarkDefinition - Adopted WatermarkDefinition object that is created on the heap.
//                             It may be NULL.
//-------------------------------------------------------------------------
void WatermarkInstance::AdoptWatermarkDefinition(WatermarkDefinition* pWatermarkDefinition)
{
    if (this->m_pWatermarkDefinition != pWatermarkDefinition)
    {
        delete this->m_pWatermarkDefinition;
        this->m_pWatermarkDefinition = pWatermarkDefinition;
    }
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the watermark definition property.
//          The WatermarkDefinition is the type of watermark definition.
// RETURNS: The pointer to the orphaned WatermarkDefinition object. It may be NULL.
//-------------------------------------------------------------------------
WatermarkDefinition* WatermarkInstance::OrphanWatermarkDefinition()
{
    WatermarkDefinition* pRet = this->m_pWatermarkDefinition;
    this->m_pWatermarkDefinition = NULL;
    return pRet;
}

//-------------------------------------------------------------------------
// Determines whether this instance is the same as the supplied one.
//-------------------------------------------------------------------------
bool WatermarkInstance::Equals(WatermarkInstance* another)
{
    if (!another)
        return false;

    // check resource ID
    if (::wcscmp(this->m_strResourceId.c_str(), another->m_strResourceId.c_str()))
        return false;

    // check appearance
    if (!this->m_appearanceOverride)
    {
        if (another->m_appearanceOverride)
            return false;
    }
    else if (!this->m_appearanceOverride->Equals(another->m_appearanceOverride))
        return false;

    // check position
    if (!this->m_positionOverride)
    {
        if (another->m_positionOverride)
            return false;
    }
    else if (!this->m_positionOverride->Equals(another->m_positionOverride))
        return false;

    // all checks passed
    return true;
}

#ifdef _WIN32
#ifdef _DEBUG
int WatermarkInstance::DumpMemoryLeaks()
{
    return _CrtDumpMemoryLeaks();
}
#endif
#endif

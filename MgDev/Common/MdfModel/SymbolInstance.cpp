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
// The SymbolInstance class implementation.
//-------------------------------------------------------------------------

#include "SymbolInstance.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Initialize an instance of the SymbolInstance class.
//-------------------------------------------------------------------------
SymbolInstance::SymbolInstance()
{
    // default values
    // NOTE: update IOSymbolInstance::Write if you change these values
    this->m_sScaleX = L"1.0";
    this->m_sScaleY = L"1.0";
    this->m_sInsertionOffsetX = L"0.0";
    this->m_sInsertionOffsetY = L"0.0";
    this->m_eSizeContext = DeviceUnits;
    this->m_sDrawLast = L"false";
    this->m_sCheckExclusionRegion = L"false";
    this->m_sAddToExclusionRegion = L"false";
    this->m_pSymbolDefinition = NULL;
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Delete all objects that have been created on the
//          heap or have been adopted.
//-------------------------------------------------------------------------
SymbolInstance::~SymbolInstance()
{
    if (this->m_pSymbolDefinition != NULL)
        delete this->m_pSymbolDefinition;
}

//-------------------------------------------------------------------------
// PURPOSE: The symbol reference. We call it ResourceId because this allows
// it to be indexed by Berkley DB.
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& SymbolInstance::GetResourceId() const
{
    return this->m_sResourceId;
}

//-------------------------------------------------------------------------
// PURPOSE: Set the symbol reference. We call it ResourceId because this allows
// it to be indexed by Berkley DB.
// PARAMETERS:
//-------------------------------------------------------------------------
void SymbolInstance::SetResourceId(const MdfString& symbolReference)
{
    this->m_sResourceId = symbolReference;

    // Definition and Reference are mutually exclusive.  Setting one
    // clears the other.
    if (this->m_pSymbolDefinition != NULL)
    {
        delete this->m_pSymbolDefinition;
        this->m_pSymbolDefinition = NULL;
    }
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
SymbolDefinition* SymbolInstance::GetSymbolDefinition()
{
    return this->m_pSymbolDefinition;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void SymbolInstance::AdoptSymbolDefinition(SymbolDefinition* symbolDefinition)
{
    if (this->m_pSymbolDefinition != symbolDefinition)
    {
        if (this->m_pSymbolDefinition != NULL)
            delete this->m_pSymbolDefinition;
        this->m_pSymbolDefinition = symbolDefinition;

        // Definition and Reference are mutually exclusive.  Setting one
        // clears the other.
        this->m_sResourceId.clear();
    }
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
SymbolDefinition* SymbolInstance::OrphanSymbolDefinition()
{
    SymbolDefinition* pRet = this->m_pSymbolDefinition;
    this->m_pSymbolDefinition = NULL;
    return pRet;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
OverrideCollection* SymbolInstance::GetParameterOverrides()
{
    return &this->m_collOverrides;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& SymbolInstance::GetScaleX() const
{
    return this->m_sScaleX;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void SymbolInstance::SetScaleX(const MdfString& scaleX)
{
    this->m_sScaleX = scaleX;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& SymbolInstance::GetScaleY() const
{
    return this->m_sScaleY;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void SymbolInstance::SetScaleY(const MdfString& scaleY)
{
    this->m_sScaleY = scaleY;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& SymbolInstance::GetInsertionOffsetX() const
{
    return this->m_sInsertionOffsetX;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void SymbolInstance::SetInsertionOffsetX(const MdfString& insertionOffsetX)
{
    this->m_sInsertionOffsetX = insertionOffsetX;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& SymbolInstance::GetInsertionOffsetY() const
{
    return this->m_sInsertionOffsetY;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void SymbolInstance::SetInsertionOffsetY(const MdfString& insertionOffsetY)
{
    this->m_sInsertionOffsetY = insertionOffsetY;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
SizeContext SymbolInstance::GetSizeContext() const
{
    return this->m_eSizeContext;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void SymbolInstance::SetSizeContext(SizeContext sizeContext)
{
    this->m_eSizeContext = sizeContext;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& SymbolInstance::GetDrawLast() const
{
    return this->m_sDrawLast;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void SymbolInstance::SetDrawLast(const MdfString& drawLast)
{
    this->m_sDrawLast = drawLast;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& SymbolInstance::GetCheckExclusionRegion() const
{
    return this->m_sCheckExclusionRegion;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void SymbolInstance::SetCheckExclusionRegion(const MdfString& checkExclusionRegion)
{
    this->m_sCheckExclusionRegion = checkExclusionRegion;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& SymbolInstance::GetAddToExclusionRegion() const
{
    return this->m_sAddToExclusionRegion;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void SymbolInstance::SetAddToExclusionRegion(const MdfString& addToExclusionRegion)
{
    this->m_sAddToExclusionRegion = addToExclusionRegion;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& SymbolInstance::GetPositioningAlgorithm() const
{
    return this->m_sPositioningAlgorithm;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void SymbolInstance::SetPositioningAlgorithm(const MdfString& positioningAlgorithm)
{
    this->m_sPositioningAlgorithm = positioningAlgorithm;
}

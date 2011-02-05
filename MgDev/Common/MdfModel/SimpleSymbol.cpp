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
// The SimpleSymbol class implementation.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "SimpleSymbol.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Initialize an instance of the SimpleSymbol class.
//-------------------------------------------------------------------------
SimpleSymbol::SimpleSymbol()
{
    // default values
    // NOTE: values used in IOSimpleSymbol::Write must match these
//  this->m_sResourceId    = L"";  // NOXLATE
    this->m_sRenderingPass = L"0"; // NOXLATE

    this->m_pSymbolDefinition = NULL;
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Delete all objects that have been created on the
//          heap or have been adopted.
//-------------------------------------------------------------------------
SimpleSymbol::~SimpleSymbol()
{
    delete this->m_pSymbolDefinition;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
SimpleSymbolDefinition* SimpleSymbol::GetSymbolDefinition()
{
    return this->m_pSymbolDefinition;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void SimpleSymbol::AdoptSymbolDefinition(SimpleSymbolDefinition* symbolDefinition)
{
    if (this->m_pSymbolDefinition != symbolDefinition)
    {
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
SimpleSymbolDefinition* SimpleSymbol::OrphanSymbolDefinition()
{
    SimpleSymbolDefinition* pRet = this->m_pSymbolDefinition;
    this->m_pSymbolDefinition = NULL;
    return pRet;
}

//-------------------------------------------------------------------------
// PURPOSE: Symbol reference.  We call it ResourceId because this allows
// it to be indexed by Berkley DB.
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& SimpleSymbol::GetResourceId() const
{
    return this->m_sResourceId;
}

//-------------------------------------------------------------------------
// PURPOSE: Symbol Reference.  We call it ResourceId because this allows
// it to be indexed by Berkley DB.
// PARAMETERS:
//-------------------------------------------------------------------------
void SimpleSymbol::SetResourceId(const MdfString& resourceId)
{
    this->m_sResourceId = resourceId;

    // Definition and Reference are mutually exclusive.  Setting one
    // clears the other.
    delete this->m_pSymbolDefinition;
    this->m_pSymbolDefinition = NULL;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& SimpleSymbol::GetRenderingPass() const
{
    return this->m_sRenderingPass;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void SimpleSymbol::SetRenderingPass(const MdfString& renderingPass)
{
    this->m_sRenderingPass = renderingPass;
}

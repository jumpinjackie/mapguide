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
// The Override class implementation.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "Override.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Initialize an instance of the Override class.
//-------------------------------------------------------------------------
Override::Override()
{
    // default values
    // NOTE: values used in IOOverride::Write must match these
//  this->m_sSymbolName          = L""; // NOXLATE
//  this->m_sParameterIdentifier = L""; // NOXLATE
//  this->m_sParameterValue      = L""; // NOXLATE

    this->m_pThemeLabel = NULL;
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Delete all objects that have been created on the
//          heap or have been adopted.
//-------------------------------------------------------------------------
Override::~Override()
{
    delete this->m_pThemeLabel;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& Override::GetSymbolName() const
{
    return this->m_sSymbolName;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void Override::SetSymbolName(const MdfString& symbolName)
{
    this->m_sSymbolName = symbolName;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& Override::GetParameterIdentifier() const
{
    return this->m_sParameterIdentifier;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void Override::SetParameterIdentifier(const MdfString& parameterIdentifier)
{
    this->m_sParameterIdentifier = parameterIdentifier;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& Override::GetParameterValue() const
{
    return this->m_sParameterValue;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void Override::SetParameterValue(const MdfString& parameterValue)
{
    this->m_sParameterValue = parameterValue;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
ThemeLabel* Override::GetThemeLabel()
{
    return this->m_pThemeLabel;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void Override::AdoptThemeLabel(ThemeLabel* themeLabel)
{
    if (this->m_pThemeLabel != themeLabel)
    {
        delete this->m_pThemeLabel;
        this->m_pThemeLabel = themeLabel;
    }
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
ThemeLabel* Override::OrphanThemeLabel()
{
    ThemeLabel* pRet = this->m_pThemeLabel;
    this->m_pThemeLabel = NULL;
    return pRet;
}

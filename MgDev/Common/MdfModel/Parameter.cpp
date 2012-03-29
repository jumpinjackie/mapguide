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
// The Parameter class implementation.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "Parameter.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Initialize an instance of the Parameter class.
//-------------------------------------------------------------------------
Parameter::Parameter()
{
    // default values
    // NOTE: values used in IOParameter::Write must match these
//  this->m_sIdentifier   = L""; // NOXLATE
//  this->m_sDefaultValue = L""; // NOXLATE
//  this->m_sDisplayName  = L""; // NOXLATE
//  this->m_sDescription  = L""; // NOXLATE
    this->m_eDataType     = String;
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Delete all objects that have been created on the
//          heap or have been adopted.
//-------------------------------------------------------------------------
Parameter::~Parameter()
{
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& Parameter::GetIdentifier() const
{
    return this->m_sIdentifier;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void Parameter::SetIdentifier(const MdfString& identifier)
{
    this->m_sIdentifier = identifier;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& Parameter::GetDefaultValue() const
{
    return this->m_sDefaultValue;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void Parameter::SetDefaultValue(const MdfString& defaultValue)
{
    this->m_sDefaultValue = defaultValue;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& Parameter::GetDisplayName() const
{
    return this->m_sDisplayName;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void Parameter::SetDisplayName(const MdfString& displayName)
{
    this->m_sDisplayName = displayName;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& Parameter::GetDescription() const
{
    return this->m_sDescription;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void Parameter::SetDescription(const MdfString& description)
{
    this->m_sDescription = description;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
Parameter::DataType Parameter::GetDataType() const
{
    return this->m_eDataType;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void Parameter::SetDataType(Parameter::DataType dataType)
{
    this->m_eDataType = dataType;
}

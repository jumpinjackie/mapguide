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

//-------------------------------------------------------------------------
// DESCRIPTION:
// The NameStringPair class implementation.
// This class represents a pair of strings.  The first is the
// name and the second is the associated text.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "NameStringPair.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Initialize an instance of the NameStringPair class.
//-------------------------------------------------------------------------
NameStringPair::NameStringPair()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Initialize an instance of the NameStringPair class.
// PARAMETERS:
//     Input:
//          strName - the name
//          strText - the associated text
//-------------------------------------------------------------------------
NameStringPair::NameStringPair(const MdfString& strName, const MdfString& strValue)
: m_strName(strName)
, m_strValue(strValue)
{
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the Name property in this NameStringPair.
// RETURNS: The name of the name-value pair.
//-------------------------------------------------------------------------
const MdfString& NameStringPair::GetName() const
{
    return this->m_strName;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the Name property in this NameStringPair.
// PARAMETERS:
//      Input:
//         strName - The name.
//-------------------------------------------------------------------------
void NameStringPair::SetName(const MdfString& strName)
{
    this->m_strName = strName;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the Value property in this NameStringPair.
// RETURNS: The value of the name-value pair.
//-------------------------------------------------------------------------
const MdfString& NameStringPair::GetValue() const
{
    return this->m_strValue;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the Value property in this NameStringPair.
// PARAMETERS:
//      Input:
//         strValue - The value associate with the name.
//-------------------------------------------------------------------------
void NameStringPair::SetValue(const MdfString& strValue)
{
    this->m_strValue = strValue;
}

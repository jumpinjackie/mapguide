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
// RelateProperty Class Implementation.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "RelateProperty.h"

using namespace MDFMODEL_NAMESPACE;

static MdfString stChainDelimiterSymbol = L".";

//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the RelateProperty
// class.
// PARAMETERS:
//-------------------------------------------------------------------------
RelateProperty::RelateProperty()
{
}

RelateProperty::RelateProperty(const MdfString &rstrFeatureClassProperty,
                               const MdfString &rstrAttributeClassProperty,
                               const MdfString &primaryAttributeRelateName)
: m_strAttributeClassProperty(rstrFeatureClassProperty)
{
    this->SetFeatureClassProperty(rstrFeatureClassProperty, primaryAttributeRelateName);
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Delete all the adopted objects.
//-------------------------------------------------------------------------
RelateProperty::~RelateProperty()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the FeatureClassProperty property.
// RETURNS: A config file name
//-------------------------------------------------------------------------
const MdfString& RelateProperty::GetFeatureClassProperty(bool bStripPrimaryAttributeRelateName) const
{
    return bStripPrimaryAttributeRelateName? this->m_strFeatureClassProperty : this->m_strPrefixedFeatureClassProperty;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the FeatureClassProperty property.
// PARAMETERS:
//      Input:
//          primaryAttributeRelateName - name of the primary join (in the
//             case of a chain) or empty string
//          propertyName - FeatureClassProperty
//-------------------------------------------------------------------------
void RelateProperty::SetFeatureClassProperty(const MdfString& propertyName, const MdfString& primaryAttributeRelateName)
{
    this->m_strFeatureClassProperty = propertyName;
    this->m_strPrimaryAttributeRelate = primaryAttributeRelateName;

    // make a complete prefixed name
    this->m_strPrefixedFeatureClassProperty = L"";
    if (primaryAttributeRelateName.length() > 0)
    {
        this->m_strPrefixedFeatureClassProperty += primaryAttributeRelateName;
        this->m_strPrefixedFeatureClassProperty += stChainDelimiterSymbol; // add "."
    }
    this->m_strPrefixedFeatureClassProperty += propertyName;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the primary AttributeRelate property.
// RETURNS: A string representing the primary AttributeRelate name if
// any or empty string.
//-------------------------------------------------------------------------
const MdfString& RelateProperty::GetPrimaryAttributeRelateName() const
{
    return this->m_strPrimaryAttributeRelate;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the AttributeClassProperty property.
// RETURNS: A string representing the AttributeClassProperty.
//-------------------------------------------------------------------------
const MdfString& RelateProperty::GetAttributeClassProperty() const
{
    return this->m_strAttributeClassProperty;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the AttributeClassProperty property.
//-------------------------------------------------------------------------
void RelateProperty::SetAttributeClassProperty(const MdfString& attributeClass)
{
    this->m_strAttributeClassProperty = attributeClass;
}

void RelateProperty::ParseDelimitedClassName(const MdfString& delimitedName,
                                             MdfString& attributeRelateName,
                                             MdfString& propertyName)
{
    size_t pos = delimitedName.find(stChainDelimiterSymbol);
    if (pos == std::wstring::npos)
    {
        // string has no delimiter; no primary AttributeRelate
        attributeRelateName = L"";
        propertyName = delimitedName;
    }
    else
    {
        // primary AttributeRelate is found
        attributeRelateName = delimitedName.substr(0, pos);
        propertyName = delimitedName.substr(++pos);
    }
}

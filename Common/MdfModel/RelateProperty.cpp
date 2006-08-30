//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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
        
//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the RelateProperty
// class.
// PARAMETERS:
//-------------------------------------------------------------------------
RelateProperty::RelateProperty()
{
    //Default Values
	this->m_strFeatureClassProperty = L"";
	this->m_strAttributeClassProperty = L"";
}

RelateProperty::RelateProperty(const MdfString &rstrFeatureClassProperty,
                               const MdfString &rstrAttributeClassProperty)
{
    this->m_strFeatureClassProperty = rstrFeatureClassProperty;
    this->m_strAttributeClassProperty = rstrAttributeClassProperty;
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
const MdfString& RelateProperty::GetFeatureClassProperty() const
{
    return this->m_strFeatureClassProperty;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the FeatureClassProperty property.
// PARAMETERS:
//      Input:
//          name - FeatureClassProperty of exntesion
//-------------------------------------------------------------------------
void RelateProperty::SetFeatureClassProperty(const MdfString& featureClass)
{
    this->m_strFeatureClassProperty = featureClass;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the AttributeClassProperty property.
// RETURNS: A string representing the AttributeClassProperty.
//-------------------------------------------------------------------------
const MdfString& RelateProperty::GetAttributeClassProperty()const
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

//End of file.

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
// AttributeRelate Class Implementation.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "AttributeRelate.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the AttributeRelate
// class.
// PARAMETERS:
//-------------------------------------------------------------------------
AttributeRelate::AttributeRelate()
{
    // default values
    this->m_eRelateType = LeftOuter;
    this->m_bForceOneToOne = true;
}

AttributeRelate::AttributeRelate(const MdfString& joinName)
{
    // default values
    this->m_strName = joinName;
    this->m_eRelateType = LeftOuter;
    this->m_bForceOneToOne = true;
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Delete all the adopted objects.
//-------------------------------------------------------------------------
AttributeRelate::~AttributeRelate()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the CalculatedProperties property.
// RETURNS: A pointer to a modifiable RelatePropertyCollection.
//-------------------------------------------------------------------------
RelatePropertyCollection* AttributeRelate::GetRelateProperties()
{
    return &this->m_collRelateProperties;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Name property.
// RETURNS: A pointer to a new Name.
//-------------------------------------------------------------------------
const MdfString& AttributeRelate::GetName() const
{
    return this->m_strName;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the Name property.
// PARAMETERS:
//      Input:
//          name - new join name
//-------------------------------------------------------------------------

void AttributeRelate::SetName(const MdfString& joinName)
{
    this->m_strName = joinName;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the ResourceId property.
// RETURNS: A config file name
//-------------------------------------------------------------------------
const MdfString& AttributeRelate::GetResourceId() const
{
    return this->m_strResourceId;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the ResourceId property.
// PARAMETERS:
//      Input:
//          resourceId - ResourceId of exntesion
//-------------------------------------------------------------------------
void AttributeRelate::SetResourceId(const MdfString& resourceId)
{
    this->m_strResourceId = resourceId;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the AttributeClass property.
// RETURNS: A string representing the AttributeClass.
//-------------------------------------------------------------------------
const MdfString& AttributeRelate::GetAttributeClass() const
{
    return this->m_strAttributeClass;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the AttributeClass property.
//-------------------------------------------------------------------------
void AttributeRelate::SetAttributeClass(const MdfString& attributeClass)
{
    this->m_strAttributeClass = attributeClass;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the AttributeNameDelimiter property.
// RETURNS: A string representing the AttributeNameDelimiter. The
// default delimiter is hard coded to "|"
//-------------------------------------------------------------------------
const MdfString& AttributeRelate::GetAttributeNameDelimiter() const
{
    return this->m_strAttributeNameDelimiter;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the AttributeNameDelimiter property.
//-------------------------------------------------------------------------
void AttributeRelate::SetAttributeNameDelimiter(const MdfString& delimiterStr)
{
    this->m_strAttributeNameDelimiter = delimiterStr;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the RelateType attribute.
// RETURNS: A Relate type attribute
//-------------------------------------------------------------------------
AttributeRelate::RelateType AttributeRelate::GetRelateType() const
{
    return this->m_eRelateType;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the RelateType attribute.
// PARAMETERS:
//      Input:
//          typ - Relate type
//-------------------------------------------------------------------------
void AttributeRelate::SetRelateType(AttributeRelate::RelateType type)
{
    this->m_eRelateType = type;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the ForceOneToOne attribute.
// RETURNS: A value of the ForceOneToOne attribute
//-------------------------------------------------------------------------
bool AttributeRelate::GetForceOneToOne() const
{
    return this->m_bForceOneToOne;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the ForceOneToOne attribute.
// PARAMETERS:
//      Input:
//          bVal - true for enforcing 1 : 1, false allows 1 : many
//-------------------------------------------------------------------------
void AttributeRelate::SetForceOneToOne(bool bVal)
{
    this->m_bForceOneToOne = bVal;
}

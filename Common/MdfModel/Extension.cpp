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
// Extension Class Implementation.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "Extension.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the Extension
// class.
// PARAMETERS:
//-------------------------------------------------------------------------
Extension::Extension()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Delete all the adopted objects.
//-------------------------------------------------------------------------
Extension::~Extension()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the CalculatedProperties property.
//          The CalculatedProperties is a collection of name/expression pairs
//          defining computed fields
// RETURNS: A pointer to a modifiable CalculatedPropertyCollection.
//-------------------------------------------------------------------------
CalculatedPropertyCollection* Extension::GetCalculatedProperties()
{
    return &this->m_collCalculatedProperties;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the AttributeRelates property.
//          The AttributeRelates is a collection of joins
// RETURNS: A pointer to a modifiable AttributeRelateCollection.
//-------------------------------------------------------------------------
AttributeRelateCollection* Extension::GetAttributeRelates()
{
    return &this->m_collAttributeRelates;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Name property.
// RETURNS: A config file name
//-------------------------------------------------------------------------
const MdfString& Extension::GetName() const
{
    return this->m_strName;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the Name property.
// PARAMETERS:
//      Input:
//          name - Name of exntesion
//-------------------------------------------------------------------------
void Extension::SetName(const MdfString& name)
{
    this->m_strName = name;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the FeatureClass property.
// RETURNS: A string representing the FeatureClass.
//-------------------------------------------------------------------------
const MdfString& Extension::GetFeatureClass() const
{
    return this->m_strFeatureClass;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the FeatureClass property.
//-------------------------------------------------------------------------
void Extension::SetFeatureClass(const MdfString& featureClass)
{
    this->m_strFeatureClass = featureClass;
}

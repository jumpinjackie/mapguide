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
// CalculatedProperty Class Implementation.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "CalculatedProperty.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the CalculatedProperty
// class.
// PARAMETERS:
//-------------------------------------------------------------------------
CalculatedProperty::CalculatedProperty()
{
}

CalculatedProperty::CalculatedProperty(const MdfString &name, const MdfString &expression)
{
    this->m_strName = name;
    this->m_strExpression = expression;
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Delete all the adopted objects.
//-------------------------------------------------------------------------
CalculatedProperty::~CalculatedProperty()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Name property.
// RETURNS: A config file name
//-------------------------------------------------------------------------
const MdfString& CalculatedProperty::GetName() const
{
    return this->m_strName;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the Name property.
// PARAMETERS:
//      Input:
//          name - Name of exntesion
//-------------------------------------------------------------------------
void CalculatedProperty::SetName(const MdfString& name)
{
    this->m_strName = name;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Expression property.
// RETURNS: A string representing the Expression.
//-------------------------------------------------------------------------
const MdfString& CalculatedProperty::GetExpression() const
{
    return this->m_strExpression;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Expression property.
//-------------------------------------------------------------------------
void CalculatedProperty::SetExpression(const MdfString& expression)
{
    this->m_strExpression = expression;
}

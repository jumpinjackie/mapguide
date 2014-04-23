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
// DESCRIPTION: The PrintLayoutDefinition class implementation.
//-------------------------------------------------------------------------

#include "../stdafx.h"
#include "PrintLayoutDefinition.h"

///////////////////////////////////////////////////////////////////////////////
/// Orientation types
///
const MdfString OrientationType::Orientation0       = L"Orientation0";
const MdfString OrientationType::Orientation90      = L"Orientation90";
const MdfString OrientationType::Orientation180     = L"Orientation180";
const MdfString OrientationType::Orientation270     = L"Orientation270";

//-------------------------------------------------------------------------
// PURPOSE: Default constructor of the PrintLayoutDefinition class.
//-------------------------------------------------------------------------
PrintLayoutDefinition::PrintLayoutDefinition() :
    m_orientation(OrientationType::Orientation0)
{
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Delete all adopted objects.
//-------------------------------------------------------------------------
PrintLayoutDefinition::~PrintLayoutDefinition()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Manage properties of this PrintLayoutDefinition object.
//-------------------------------------------------------------------------
void PrintLayoutDefinition::SetName(const MdfString& name)
{
    m_name = name;
}

void PrintLayoutDefinition::SetDeviceName(const MdfString& deviceName)
{
    m_deviceName = deviceName;
}

void PrintLayoutDefinition::SetMediaName(const MdfString& mediaName)
{
    m_mediaName = mediaName;
}

void PrintLayoutDefinition::SetOrientation(const MdfString& orientation)
{
    m_orientation = orientation;
}

void PrintLayoutDefinition::SetUnits(const MdfString& units)
{
    m_units = units;
}

//
//  Copyright (C) 2011 by Autodesk, Inc.
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
// The URLData class implementation.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "URLData.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the URLData
// class.
// PARAMETERS:
//-------------------------------------------------------------------------
URLData::URLData()
{
}

URLData::~URLData()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the URL content property.
// RETURNS: A string representing the URL content.
//-------------------------------------------------------------------------
const MdfString& URLData::GetUrlContent() const
{
    return this->m_strUrlContent;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the URL content property.
//-------------------------------------------------------------------------
void URLData::SetUrlContent(const MdfString& urlContent)
{
    this->m_strUrlContent = urlContent;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the URL content override property.
// RETURNS: A string representing the URL content override.
//-------------------------------------------------------------------------
const MdfString& URLData::GetUrlContentOverride() const
{
    return this->m_strUrlContentOverride;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the URL content override property.
//-------------------------------------------------------------------------
void URLData::SetUrlContentOverride(const MdfString& urlContentOverride)
{
    this->m_strUrlContentOverride = urlContentOverride;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the URL description property.
// RETURNS: A string representing the URL description.
//-------------------------------------------------------------------------
const MdfString& URLData::GetUrlDescription() const
{
    return this->m_strUrLDescription;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the URL description property.
//-------------------------------------------------------------------------
void URLData::SetUrlDescription(const MdfString& urlDescription)
{
    this->m_strUrLDescription = urlDescription;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the URL description override property.
// RETURNS: A string representing the URL description override.
//-------------------------------------------------------------------------
const MdfString& URLData::GetUrlDescriptionOverrride() const
{
    return this->m_strUrlDescriptionOverride;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the URL description override property.
//-------------------------------------------------------------------------
void URLData::SetUrlDescriptionOverrride(const MdfString& urlDescriptionOverride)
{
    this->m_strUrlDescriptionOverride = urlDescriptionOverride;
}

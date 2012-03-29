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

#include "stdafx.h"
#include "FeatureSource.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the FeatureSource
// class.
// PARAMETERS:
//-------------------------------------------------------------------------
FeatureSource::FeatureSource()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Delete all the adopted objects.
//-------------------------------------------------------------------------
FeatureSource::~FeatureSource()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Parameters property.
//          The Parameters is a collection of objects, each of which
//          defines a pair of strings. It represents the connection information
//          associated with each feature source.
// RETURNS: A pointer to a modifiable NameStringPairCollection.
//-------------------------------------------------------------------------
NameStringPairCollection* FeatureSource::GetParameters()
{
    return &this->m_collParameters;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Extensions property.
//          The Extensions is a collection of named extensions defining joins
//          between feature classes in this feature source and the feature
//          classes in other feature sources.
// RETURNS: A pointer to a modifiable ExtensionCollection.
//-------------------------------------------------------------------------
ExtensionCollection* FeatureSource::GetExtensions()
{
    return &this->m_collExtensions;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the SupplementalSpatialContextInfo property.
//          The SupplementalSpatialContextInfo is Additional information for a
//          spatial context when the coordinate system is missing from the
//          feature source
// RETURNS: A pointer to a modifiable SupplementalSpatialContextInfoCollection.
//-------------------------------------------------------------------------
SupplementalSpatialContextInfoCollection*
FeatureSource::GetSupplementalSpatialContextInfo()
{
    return &this->m_collSupplementalSpatialContextInfo;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Provider property.
// RETURNS: A string representing the Provider.
//-------------------------------------------------------------------------
const MdfString& FeatureSource::GetProvider() const
{
    return this->m_strProvider;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Provider property.
//-------------------------------------------------------------------------
void FeatureSource::SetProvider(const MdfString& provider)
{
    this->m_strProvider = provider;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the LongTransaction property.
// RETURNS: A string representing the name of feature source.
//-------------------------------------------------------------------------
const MdfString& FeatureSource::GetLongTransaction() const
{
    return this->m_strLongTransaction;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the LongTransaction property.
//-------------------------------------------------------------------------
void FeatureSource::SetLongTransaction(const MdfString& name)
{
    this->m_strLongTransaction = name;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the ConfigurationDocument property.
// RETURNS: A string representing the name of feature source.
//-------------------------------------------------------------------------
const MdfString& FeatureSource::GetConfigurationDocument() const
{
    return this->m_strConfigurationDocument;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the ConfigurationDocument property.
//-------------------------------------------------------------------------
void FeatureSource::SetConfigurationDocument(const MdfString& name)
{
    this->m_strConfigurationDocument = name;
}

//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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

#include "HttpHandler.h"

//////////////////////////////////////////////////////////////////
/// <summary>
/// Constructor. Initialize string property collection.
/// </summary>
MgHttpRequestMetadata::MgHttpRequestMetadata()
{
    // Nothing here yet
}

/// <summary>
/// Adds the property name and value to the collection.
/// property name  -    case-insensitive
/// property value -    case-insensitive
/// </summary>
/// <param name="name">Input
/// Name of the property
/// Two property can not have same name.
/// </param>
/// <param name="value">Input
/// Value corresponding to the property.
/// NULL value and empty string is NOT allowed.
/// </param>
/// <returns>
/// TRUE  -  property name is successfully added.
/// FALSE -  property could not be added.
/// Possible cause - property value is NULL.
/// </returns>
bool MgHttpRequestMetadata::AddProperty(CREFSTRING property, CREFSTRING value)
{
    if (value.length()!=0 && !m_reqMetadataCollection.Contains(property))
    {
        m_reqMetadataCollection.Add(property, value);
        return true;
    }

    return false;
}

/// <summary>
/// Removes the property name and value from the collection.
/// property name - case-insensitive
/// </summary>
/// <param name="name">Input
/// Name of the property to be removed.
/// </param>
/// <returns>
/// TRUE  -  property name is successfully removed.
/// FALSE -  property name could not be removed.
/// Possible cause is property name does not exist.
/// </returns>
bool MgHttpRequestMetadata::RemoveProperty(CREFSTRING property)
{
    if (m_reqMetadataCollection.Contains(property))
    {
        m_reqMetadataCollection.Remove(property);
        return true;
    }

    return false;
}

/// <summary>
/// Retrieve the value for the specified property.
/// property name  - case-insensitive
/// </summary>
/// <param name="name">Input
/// Name of the property for which value to be retrieved.
/// </param>
/// <returns>
/// Value for the specified property or empty string if property does not exist.
/// </returns>
STRING MgHttpRequestMetadata::GetPropertyValue(CREFSTRING property)
{
    if (m_reqMetadataCollection.Contains(property))
    {
        return m_reqMetadataCollection.GetValue(property);
    }
    return L"";
}

/// <summary>
/// Update the value for the specified property.
/// property name  - case-insensitive
/// property value - case-insensitive
/// </summary>
/// <param name="name">Input
/// Name of the property to be updated.
/// </param>
/// <param name="value">Input
/// Value corresponding to the property.
/// NULL value and empty string is NOT allowed.
/// </param>
/// <returns>
/// TRUE  -  property is successfully updated.
/// FALSE -  property could not be updated.
/// Possible cause is property does not exist or value is NULL.
/// </returns>
bool MgHttpRequestMetadata::SetPropertyValue(CREFSTRING name, CREFSTRING value)
{
    if (m_reqMetadataCollection.Contains(name))
    {
        m_reqMetadataCollection.SetValue(name, value);
        return true;
    }

    return false;
}

/// <summary>
/// Retrieve the list of all properties.
/// </summary>
/// <returns>
/// A string collection containing names of all properties.
/// </returns>
MgStringCollection* MgHttpRequestMetadata::GetPropertyNames()
{
    Ptr<MgStringCollection> mgsCollection;
    mgsCollection = new MgStringCollection();

    for (int i = 0; i < m_reqMetadataCollection.GetCount(); i++)
    {
        mgsCollection->Add(m_reqMetadataCollection.GetValue(i));
    }

    return SAFE_ADDREF((MgStringCollection*)mgsCollection);
}

void MgHttpRequestMetadata::Dispose()
{
    delete this;
}

INT32 MgHttpRequestMetadata::GetClassId()
{
    return m_cls_id;
}

MgHttpRequestMetadata::~MgHttpRequestMetadata()
{
}

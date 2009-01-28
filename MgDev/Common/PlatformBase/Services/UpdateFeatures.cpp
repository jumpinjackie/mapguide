//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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

#include "PlatformBase.h"
#include "FeatureService.h"

MG_IMPL_DYNCREATE(MgUpdateFeatures)

/// <summary>
/// Constructor
/// </summary>
/// <param name="className">
/// Name of class from which data to be deleted
/// </param>
/// <param name="filterText">
/// Filter crieteria to be applied for deletion
/// </param>
/// <returns>
/// Nothing
/// </returns>
MgUpdateFeatures::MgUpdateFeatures(CREFSTRING className,
                                   MgPropertyCollection* propertyValues,
                                   CREFSTRING filterText)
{

    if (className.empty())
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(MgResources::BlankArgument);

        throw new MgInvalidArgumentException(L"MgUpdateFeatures.MgUpdateFeatures",
            __LINE__, __WFILE__, &arguments, L"MgStringEmpty", NULL);
    }

    if (propertyValues == NULL)
    {
        throw new MgNullArgumentException(L"MgUpdateFeatures.MgUpdateFeatures", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (propertyValues->GetCount() == 0)
    {
        MgStringCollection arguments;
        arguments.Add(L"2");
        arguments.Add(L"0");

        throw new MgInvalidArgumentException(L"MgUpdateFeatures.MgUpdateFeatures",
            __LINE__, __WFILE__, &arguments, L"MgCollectionEmpty", NULL);
    }

    m_className = className;
    m_properties = SAFE_ADDREF(propertyValues);
    m_filterText = filterText; // Filter text can be empty
}

/// <summary>
/// Destructor
/// </summary>
MgUpdateFeatures::~MgUpdateFeatures()
{
}

/// <summary>
/// Type of Command i.e. MgFeatureCommandType::DeleteFeatures
/// </summary>
/// <returns>
/// Returns enum value indicating type of command
/// </returns>
INT32 MgUpdateFeatures::GetCommandType() const
{
    return MgFeatureCommandType::UpdateFeatures;
}

/// <summary>
/// Get the featue class name specified in the constructor
/// </summary>
/// <returns>
/// the feature class name specified for which data are deleted
/// </returns>
STRING MgUpdateFeatures::GetFeatureClassName() const
{
    return this->m_className;
}

/// <summary>
/// Get the filter specified in the constructor
/// </summary>
/// <returns>
/// the filter specified for delete
/// </returns>
STRING MgUpdateFeatures::GetFilterText() const
{
    return this->m_filterText;
}

/// <summary>
/// Get the property values collection specified in the constructor
/// </summary>
/// <returns>
/// Collection of property values inserted
/// </returns>

MgPropertyCollection* MgUpdateFeatures::GetPropertyValues() const
{
    return SAFE_ADDREF((MgPropertyCollection*)this->m_properties);
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Serializes data to a TCP/IP stream.
///</summary>
///<param name="stream">
/// Stream
///</param>
void MgUpdateFeatures::Serialize(MgStream* stream)
{
    stream->WriteString(m_className);
    stream->WriteObject(this->m_properties);
    stream->WriteString(m_filterText);
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Deserializes data from a TCP/IP stream.
///</summary>
///<param name="stream">
/// Stream
///</param>

void MgUpdateFeatures::Deserialize(MgStream* stream)
{
    stream->GetString(m_className);
    m_properties = (MgPropertyCollection*)stream->GetObject();
    stream->GetString(m_filterText);
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the classId.
/// </summary>
INT32 MgUpdateFeatures::GetClassId()
{
    return m_cls_id;
}

void MgUpdateFeatures::Dispose()
{
    delete this;
}

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

MG_IMPL_DYNCREATE(MgDeleteFeatures)

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
MgDeleteFeatures::MgDeleteFeatures(CREFSTRING className, CREFSTRING filterText)
{
    if (className.empty())
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(MgResources::BlankArgument);

        throw new MgInvalidArgumentException(L"MgDeleteFeatures.MgDeleteFeatures",
            __LINE__, __WFILE__, &arguments, L"MgStringEmpty", NULL);
    }

    // Filter can be null, means delete the entire class
    m_className = className;
    m_filterText = filterText;
}

/// <summary>
/// Destructor
/// </summary>
MgDeleteFeatures::~MgDeleteFeatures()
{
}

/// <summary>
/// Type of Command i.e. MgFeatureCommandType::DeleteFeatures
/// </summary>
/// <returns>
/// Returns enum value indicating type of command
/// </returns>
INT32 MgDeleteFeatures::GetCommandType() const
{
    return MgFeatureCommandType::DeleteFeatures;
}

/// <summary>
/// Get the feature class name specified in the constructor
/// </summary>
/// <returns>
/// the feature class name specified for which data are deleted
/// </returns>
STRING MgDeleteFeatures::GetFeatureClassName() const
{
    return this->m_className;
}

/// <summary>
/// Get the filter specified in the constructor
/// </summary>
/// <returns>
/// the filter specified for delete
/// </returns>
STRING MgDeleteFeatures::GetFilterText() const
{
    return this->m_filterText;
}


//////////////////////////////////////////////////////////////////
///<summary>
/// Serializes data to a TCP/IP stream.
///</summary>
///<param name="stream">
/// Stream
///</param>
void MgDeleteFeatures::Serialize(MgStream* stream)
{
    stream->WriteString(m_className);
    stream->WriteString(m_filterText);
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Deserializes data from a TCP/IP stream.
///</summary>
///<param name="stream">
/// Stream
///</param>

void MgDeleteFeatures::Deserialize(MgStream* stream)
{
    stream->GetString(m_className);
    stream->GetString(m_filterText);
}


/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the classId.
/// </summary>
INT32 MgDeleteFeatures::GetClassId()
{
    return m_cls_id;
}

void MgDeleteFeatures::Dispose()
{
    delete this;
}

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

#include "ResourceServiceDefs.h"
#include "ResourceInfo.h"

const string MgResourceInfo::sm_elementName         = "Name";
const string MgResourceInfo::sm_elementMetadata     = "Metadata";
const string MgResourceInfo::sm_elementResourceId   = "ResourceId";

const string MgResourceInfo::sm_metadataUri     = "http://www.autodesk.com/MapGuide/Resource/Metadata";
const string MgResourceInfo::sm_metadataPrefix  = "Metadata";

const string MgResourceInfo::sm_metadataNames[] =
{
    "Depth",
    "Owner",
    "CreatedDate",
    "ModifiedDate",
    "Tags"
};

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

MgResourceInfo::MgResourceInfo(const MgResourceIdentifier& identifier,
    MgUserInformation* currUserInfo, time_t accessedTime, INT32 depth)
{
    MG_RESOURCE_SERVICE_TRY()

    // Initialize metadata.

    m_identifier = identifier;

    // Set the depth.

    if (depth < 0)
    {
        depth = m_identifier.GetDepth();
    }

    m_metadataValues[MgResourceInfo::Depth] = XmlValue(
        static_cast<double>(depth));

    // Set the owner.

    string owner;

    if (NULL != currUserInfo)
    {
        MgUtil::WideCharToMultiByte(currUserInfo->GetUserName(), owner);
    }

    m_metadataValues[MgResourceInfo::Owner] = XmlValue(owner);

    // Set the Created and Modified dates with the current coordinated
    // universal time (UTC).

    MgDateTime dateTime(accessedTime);

    m_metadataValues[MgResourceInfo::CreatedDate] =
        m_metadataValues[MgResourceInfo::ModifiedDate] =
            XmlValue(XmlValue::DATE_TIME, dateTime.ToXmlStringUtf8());

    // Set the tags.

    m_metadataValues[MgResourceInfo::Tags] = XmlValue("");

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourceInfo.MgResourceInfo")
}

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

MgResourceInfo::MgResourceInfo(const MgResourceIdentifier& identifier,
    const XmlDocument& xmlDoc)
{
    MG_RESOURCE_SERVICE_TRY()

    m_identifier = identifier;

    for (int i = 0; i < MAX_RESOURCE_METADATA_ATTRIBUTE_NUMBER; ++i)
    {
        XmlValue metadataValue;

        if (xmlDoc.getMetaData(
                MgResourceInfo::sm_metadataUri,
                MgResourceInfo::sm_metadataNames[i],
                metadataValue))
        {
            m_metadataValues[i] = metadataValue;
        }
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourceInfo.MgResourceInfo")
}

///----------------------------------------------------------------------------
/// <summary>
/// Constructs a new MgResourceInfo object and initializes the member variables
/// to values given by a referenced MgResourceInfo object.
/// </summary>
/// <param name="resourceInfo">
/// Reference to an MgResourceInfo object.
/// </param>
/// <exceptions>
/// MgOutOfMemoryException
/// </exceptions>
///----------------------------------------------------------------------------

MgResourceInfo::MgResourceInfo(const MgResourceInfo& resourceInfo)
{
    *this = resourceInfo;
}

///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgResourceInfo::~MgResourceInfo()
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Overloaded assignment operator for MgResourceInfo.
/// </summary>
/// <param name="resourceInfo">
/// Reference to the structure that is to be copied.
/// </param>
/// <return>
/// Reference to the structure (the one assigned to).
/// </return>
/// <exceptions>
/// MgOutOfMemoryException
/// </exceptions>
///----------------------------------------------------------------------------

MgResourceInfo& MgResourceInfo::operator=(const MgResourceInfo& resourceInfo)
{
    if (&resourceInfo != this)
    {
        m_identifier = resourceInfo.m_identifier;

        for (int i = 0; i < MAX_RESOURCE_METADATA_ATTRIBUTE_NUMBER; ++i)
        {
            m_metadataValues[i] = resourceInfo.m_metadataValues[i];
        }
    }

    return *this;
}

///----------------------------------------------------------------------------
/// <summary>
/// Sets the pathname and depth of the resource.
/// </summary>
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

void MgResourceInfo::SetIdentifier(CREFSTRING pathname, INT32 depth)
{
    MG_RESOURCE_SERVICE_TRY()

    m_identifier.SetResource(pathname);

    if (depth < 0)
    {
        depth = m_identifier.GetDepth();
    }

    m_metadataValues[MgResourceInfo::Depth] = XmlValue(
        static_cast<double>(depth));

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgResourceInfo.SetIdentifier")
}

///----------------------------------------------------------------------------
/// <summary>
/// Sets a metadata attribute.
/// </summary>
///----------------------------------------------------------------------------

void MgResourceInfo::SetAttribute(Attribute name, const XmlValue& value)
{
    m_metadataValues[name] = value;
}

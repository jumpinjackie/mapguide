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

#include "ResourceServiceDefs.h"
#include "ApplicationResourceContentManager.h"
#include "TagManager.h"

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// </exceptions>
///----------------------------------------------------------------------------

MgApplicationResourceContentManager::MgApplicationResourceContentManager(
    MgApplicationRepositoryManager& repositoryMan) :
    MgResourceContentManager(repositoryMan)
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgApplicationResourceContentManager::~MgApplicationResourceContentManager()
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Enumerates all resources which reference the specified resource.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// </exceptions>
///----------------------------------------------------------------------------

MgByteReader* MgApplicationResourceContentManager::EnumerateReferences(
    MgResourceIdentifier* resource)
{
    assert(NULL != resource);
    Ptr<MgByteReader> byteReader;

    MG_RESOURCE_SERVICE_TRY()

    string resourceId;
    MgUtil::WideCharToMultiByte(resource->ToString(), resourceId);

    // Set up an XQuery.

    string query = "collection('";
    query += m_container.getName();
    query += "')";
    query += "//*/ResourceId[text()=\"";
    query += resourceId;
    query += "\"]";

    // Execute the XQuery.

    XmlManager& xmlMan = m_container.getManager();
    XmlQueryContext queryContext = xmlMan.createQueryContext();
    XmlResults results = IsTransacted() ?
        xmlMan.query(GetXmlTxn(), query, queryContext, 0) :
        xmlMan.query(query, queryContext, 0);

    // Get the resource and populate the resource reference list.

    // this XML follows the ResourceReferenceList-1.0.0.xsd schema
    MgResourceIdentifier currResource;
    XmlValue xmlValue;
    string list = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";

    list += "<ResourceReferenceList xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"ResourceReferenceList-1.0.0.xsd\">\n";

    while (results.next(xmlValue)) // TODO: Need an XML writer
    {
        const XmlDocument& xmlDoc = xmlValue.asDocument();

        // Check if the current user has a read permission.

        currResource.SetResource(MgUtil::MultiByteToWideChar(xmlDoc.getName()));

        if (CheckPermission(currResource,
            MgResourcePermission::ReadOnly, false))
        {
            list += "\t<ResourceId>";
            list += xmlDoc.getName();
            list += "</ResourceId>\n";
        }
    }

    list += "</ResourceReferenceList>";

    // Create a byte reader.

    Ptr<MgByteSource> byteSource = new MgByteSource(
        (unsigned char*)list.c_str(), (INT32)list.length());

    byteSource->SetMimeType(MgMimeType::Xml);
    byteReader = byteSource->GetReader();

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgApplicationResourceContentManager.EnumerateReferences")

    return byteReader.Detach();
}

///////////////////////////////////////////////////////////////////////////
/// \brief
/// Enumerate all the parent Map Definition resources of the specified
/// resources.
///
/// Note that checking permissions is not required for this operation.
///
void MgApplicationResourceContentManager::EnumerateParentMapDefinitions(
    const set<string>& currSearchResources, set<string>& nextSearchResources,
    set<string>& childResources, set<STRING>& parentResources)
{
    MG_RESOURCE_SERVICE_TRY()

    // Reset the next search list.

    nextSearchResources.clear();

    // Do nothing if the current search list is empty.

    if (currSearchResources.empty())
    {
        return;
    }

    // Set up an XQuery.

    string query = "collection('";
    query += m_container.getName();
    query += "')";
    query += "//*/ResourceId[";

    for (set<string>::const_iterator i = currSearchResources.begin();
        i != currSearchResources.end( ); ++i)
    {
        if (i != currSearchResources.begin())
        {
            query += " or ";
        }

        query += "text()=\"";
        query += *i;
        query += "\"";
    }

    query += "]";

    // Execute the XQuery.

    XmlManager& xmlMan = m_container.getManager();
    XmlQueryContext queryContext = xmlMan.createQueryContext();
    XmlResults results = IsTransacted() ?
        xmlMan.query(GetXmlTxn(), query, queryContext, 0) :
        xmlMan.query(query, queryContext, 0);
    XmlDocument xmlDoc;

    while (results.next(xmlDoc))
    {
        string mbResourcePathname = xmlDoc.getName();
        STRING wcResourcePathname;
        MgUtil::MultiByteToWideChar(mbResourcePathname, wcResourcePathname);
        MgResourceIdentifier resource(wcResourcePathname);

        if (!resource.IsFolder())
        {
            // Insert the resource into the parent list if it is a Map Definition.
            // Otherwise, insert it into the child list.

            if (resource.IsResourceTypeOf(MgResourceType::MapDefinition))
            {
                parentResources.insert(wcResourcePathname);
            }
            else
            {
                std::pair<set<string>::iterator, bool> i =
                    childResources.insert(mbResourcePathname);

                // Insert the resource into the next search list if it is not in
                // the child list.

                if (i.second)
                {
                    nextSearchResources.insert(mbResourcePathname);
                }
            }
        }
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgApplicationResourceContentManager.EnumerateParentMapDefinitions")
}

///----------------------------------------------------------------------------
/// <summary>
/// Enumerates tagged data for the specified resource.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// </exceptions>
///----------------------------------------------------------------------------

MgByteReader* MgApplicationResourceContentManager::EnumerateResourceData(
    MgResourceIdentifier* resource)
{
    assert(NULL != resource);
    Ptr<MgByteReader> byteReader;

    MG_RESOURCE_SERVICE_TRY()

    // Get the resource tags.

    STRING resourceTags;
    GetResourceTags(resource, MgResourcePermission::ReadOnly, resourceTags);

    MgTagManager tagMan(resourceTags);
    MgTagMap& tagMap = tagMan.GetTagMap();

    // Populate the resource data list.

    // this XML follows the ResourceDataList-1.0.0.xsd schema
    MgTagMap::const_iterator i;
    string list = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";

    list += "<ResourceDataList xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"ResourceDataList-1.0.0.xsd\">\n";

    for (i = tagMap.begin(); i != tagMap.end(); ++i) // TODO: Need an XML writer
    {
        CREFSTRING dataName = (*i).first;

        if (!tagMan.IsReservedTag(dataName))
        {
            CREFSTRING dataType = (*i).second.GetAttribute(MgTagInfo::StorageType);

            list += "\t<ResourceData>\n";

            list += "\t\t<Name>";
            list += MgUtil::WideCharToMultiByte(dataName);
            list += "</Name>\n";

            list += "\t\t<Type>";
            list += MgUtil::WideCharToMultiByte(dataType);
            list += "</Type>\n";

            list += "\t</ResourceData>\n";
        }
    }

    list += "</ResourceDataList>";

    // Create a byte reader.

    Ptr<MgByteSource> byteSource = new MgByteSource(
        (unsigned char*)list.c_str(), (INT32)list.length());

    byteSource->SetMimeType(MgMimeType::Xml);
    byteReader = byteSource->GetReader();

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgApplicationResourceContentManager.EnumerateResourceData")

    return byteReader.Detach();
}

///----------------------------------------------------------------------------
/// <summary>
/// Gets the tags associated with the specified resource.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// </exceptions>
///----------------------------------------------------------------------------

XmlDocument MgApplicationResourceContentManager::GetResourceTags(
    MgResourceIdentifier* resource, CREFSTRING permission,
    REFSTRING resourceTags)
{
    assert(NULL != resource);
    XmlDocument xmlDoc = GetDocument(*resource, permission);

    MG_RESOURCE_SERVICE_TRY()

    XmlValue tagValue;

    xmlDoc.getMetaData(MgResourceInfo::sm_metadataUri,
        MgResourceInfo::sm_metadataNames[MgResourceInfo::Tags],
        tagValue);

    MgUtil::MultiByteToWideChar(tagValue.asString(), resourceTags);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgApplicationResourceContentManager.GetResourceTags")

    return xmlDoc;
}

///----------------------------------------------------------------------------
/// <summary>
/// Sets the tags associated with the specified resource.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// </exceptions>
///----------------------------------------------------------------------------

void MgApplicationResourceContentManager::SetResourceTags(
    MgResourceIdentifier& resource, CREFSTRING resourceTags,
    XmlDocument& xmlDoc)
{
    MG_RESOURCE_SERVICE_TRY()

    // Set the resource tags.

    XmlValue tagValue(MgUtil::WideCharToMultiByte(resourceTags));

    xmlDoc.setMetaData(
        MgResourceInfo::sm_metadataUri,
        MgResourceInfo::sm_metadataNames[MgResourceInfo::Tags],
        tagValue);

    // Update the modified date.

    const XmlValue accessedTime = m_repositoryMan.GetAccessedTime();
    XmlValue modifiedDate;

    if (xmlDoc.getMetaData(
            MgResourceInfo::sm_metadataUri,
            MgResourceInfo::sm_metadataNames[MgResourceInfo::ModifiedDate],
            modifiedDate)
        && !(modifiedDate == accessedTime))
    {
        xmlDoc.setMetaData(
            MgResourceInfo::sm_metadataUri,
            MgResourceInfo::sm_metadataNames[MgResourceInfo::ModifiedDate],
            accessedTime);
    }

    // Update the resource.

    XmlManager& xmlMan = m_container.getManager();
    XmlUpdateContext updateContext = xmlMan.createUpdateContext();

    UpdateDocument(resource, xmlDoc, updateContext);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgApplicationResourceContentManager.SetResourceTags")
}

///----------------------------------------------------------------------------
/// <summary>
/// Deletes tagged data from the specified document resource content.
/// </summary>
///----------------------------------------------------------------------------

void MgApplicationResourceContentManager::DeleteResourceData(MgResourceIdentifier* resource, XmlDocument& xmlDoc)
{
    MG_RESOURCE_SERVICE_TRY()

    XmlValue tagValue;

    if (xmlDoc.getMetaData(
            MgResourceInfo::sm_metadataUri,
            MgResourceInfo::sm_metadataNames[MgResourceInfo::Tags],
            tagValue))
    {
        STRING resourceTags;
        MgUtil::MultiByteToWideChar(tagValue.asString(), resourceTags);

        m_repositoryMan.DeleteResourceData(resourceTags, resource);
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgApplicationResourceContentManager.DeleteResourceData")
}

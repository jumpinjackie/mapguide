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

#include "ResourceServiceDefs.h"
#include "LibraryResourceContentManager.h"
#include "LibraryRepositoryManager.h"
#include "LibraryRepository.h"
#include "ResourceHeaderManager.h"

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///
/// <exceptions>
/// MgDbXmlException, MgDbException
/// </exceptions>
///----------------------------------------------------------------------------

MgLibraryResourceContentManager::MgLibraryResourceContentManager(
    MgLibraryRepositoryManager& repositoryMan) :
    MgApplicationResourceContentManager(repositoryMan)
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgLibraryResourceContentManager::~MgLibraryResourceContentManager()
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Checks whether or not the current user has the specified permission on the
/// specified resource.
/// </summary>
///
/// <exceptions>
/// MgUnauthorizedAccessException
/// MgPermissionDeniedException
/// </exceptions>
///----------------------------------------------------------------------------

bool MgLibraryResourceContentManager::CheckPermission(
    MgResourceIdentifier& resource, CREFSTRING permission, bool strict)
{
    return m_repositoryMan.GetResourceHeaderManager()->CheckPermission(
        resource, permission, strict);
}

///----------------------------------------------------------------------------
/// <summary>
/// Checks whether or not the current user has the specified permission on the
/// parent of the specified resource.
/// </summary>
///
/// <exceptions>
/// MgUnauthorizedAccessException
/// MgPermissionDeniedException
/// </exceptions>
///----------------------------------------------------------------------------

bool MgLibraryResourceContentManager::CheckParentPermission(
    MgResourceIdentifier& resource, CREFSTRING permission, bool strict)
{
    return m_repositoryMan.GetResourceHeaderManager()->CheckParentPermission(
        resource, permission, strict);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Packages the specified resource.
///
void MgLibraryResourceContentManager::PackageResource(MgResourceIdentifier& resource,
    MgResourcePackageMaker& packageMaker)
{
    ACE_ASSERT(resource.IsFolder());

    MG_RESOURCE_SERVICE_TRY()

    string resourcePathname;
    MgUtil::WideCharToMultiByte(resource.ToString(), resourcePathname);

    // Set up an XQuery.
    XmlManager& xmlMan = m_container.getManager();
    XmlQueryContext queryContext = xmlMan.createQueryContext();
    string query;

    if (m_repositoryMan.m_currUserIsAdmin)
    {
        queryContext.setEvaluationType(XmlQueryContext::Lazy);
        query = "collection('";
    }
    else
    {
        queryContext.setEvaluationType(XmlQueryContext::Eager);
        query = "for $i in collection('";
    }

    query += m_container.getName();
    query += "')";
    query += "/*[starts-with(dbxml:metadata('dbxml:name'),'";
    query += resourcePathname;

    if (m_repositoryMan.m_currUserIsAdmin)
    {
        query += "')]";
    }
    else
    {
        query += "')] order by dbxml:metadata('dbxml:name', $i) return $i";
    }

    // Execute the XQuery.
    XmlResults results = IsTransacted() ?
        xmlMan.query(GetXmlTxn(), query, queryContext, 0) :
        xmlMan.query(query, queryContext, 0);

    MgLibraryRepositoryManager& libraryRepositoryMan = (MgLibraryRepositoryManager&)m_repositoryMan;
    MgResourceDataFileManager* dataFileMan = libraryRepositoryMan.GetResourceDataFileManager();
    ACE_ASSERT(NULL != dataFileMan);
    MgResourceDataStreamManager* dataStreamMan = libraryRepositoryMan.GetResourceDataStreamManager();
    ACE_ASSERT(NULL != dataStreamMan);

    // Get the resources
    MgResourceIdentifier currResource;
    XmlValue xmlValue;

    while (results.next(xmlValue))
    {
        // Note that the permission checks were already done at the time we
        // packaged the resource headers.
        const XmlDocument& xmlDoc = xmlValue.asDocument();
        currResource.SetResource(MgUtil::MultiByteToWideChar(xmlDoc.getName()));

        // Package the resource content.
        if (!packageMaker.PackageResourceContent(currResource, xmlDoc))
        {
            // The current user has no read permission to this resource.
            continue;
        }

        // Skip packaging resource data if there is no resource tag.
        XmlValue tagValue;

        if (!xmlDoc.getMetaData(MgResourceInfo::sm_metadataUri,
            MgResourceInfo::sm_metadataNames[MgResourceInfo::Tags], tagValue))
        {
            continue;
        }

        STRING resourceTags;
        MgUtil::MultiByteToWideChar(tagValue.asString(), resourceTags);

        // Get the file path if it exists.
        MgTagManager tagMan(resourceTags);
        STRING filePath;
        MgTagInfo filePathTag;

        if (tagMan.GetTag(MgResourceTag::DataFilePath, filePathTag, false))
        {
            filePath = dataFileMan->GetResourceDataFilePath();
            filePath += filePathTag.GetAttribute(MgTagInfo::TokenValue);
            filePath += L"/";
        }

        // Enumerate the resource data.
        MgTagMap& tagMap = tagMan.GetTagMap();

        for (MgTagMap::const_iterator i = tagMap.begin();
            i != tagMap.end(); ++i)
        {
            CREFSTRING dataName = (*i).first;

            // Skip the reserved tags.
            if (tagMan.IsReservedTag(dataName))
            {
                continue;
            }

            // Get the resource data.
            const MgTagInfo& tagInfo = (*i).second;
            CREFSTRING mimeType = tagInfo.GetAttribute(MgTagInfo::MimeType);
            CREFSTRING dataType = tagInfo.GetAttribute(MgTagInfo::StorageType);
            Ptr<MgByteReader> byteReader;

            if (MgResourceDataType::File == dataType)
            {
                ACE_ASSERT(!filePath.empty());
                STRING pathname = filePath;
                pathname += dataName;

                byteReader = dataFileMan->GetResourceData(pathname, mimeType);
            }
            else if (MgResourceDataType::Stream == dataType)
            {
                string dataKey;
                MgUtil::WideCharToMultiByte(
                    tagInfo.GetAttribute(MgTagInfo::TokenValue), dataKey);

                byteReader = dataStreamMan->GetResourceData(dataKey, dataName,
                    mimeType);
            }
            else if (MgResourceDataType::String == dataType)
            {
                string data;
                MgUtil::WideCharToMultiByte(
                    tagInfo.GetAttribute(MgTagInfo::TokenValue), data);

                Ptr<MgByteSource> byteSource = new MgByteSource(
                    (BYTE_ARRAY_IN)data.c_str(), (INT32)data.length());

                byteSource->SetMimeType(mimeType);
                byteReader = byteSource->GetReader();
            }
            else
            {
                throw new MgInvalidResourceDataTypeException(
                    L"MgLibraryResourceContentManager.PackageResource",
                    __LINE__, __WFILE__, NULL, L"", NULL);
            }

            // Package the resource data.
            packageMaker.PackageResourceData(currResource, byteReader,
                dataName, dataType);
        }
    }

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgLibraryResourceContentManager.PackageResource")
}

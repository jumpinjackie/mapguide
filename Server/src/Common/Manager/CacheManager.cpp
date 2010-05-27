//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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

#include "CacheManager.h"
#include "ServiceManager.h"
#include "FdoConnectionManager.h"
#include "Fdo.h"
#include "FSDSAX2Parser.h"
#include "ServerResourceService.h"

// Process-wide MgCacheManager
Ptr<MgCacheManager> MgCacheManager::sm_cacheManager = (MgCacheManager*)NULL;

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Construct the object.
///
MgCacheManager::MgCacheManager()
{
    m_serviceManager = MgServiceManager::GetInstance();
    m_fdoConnectionManager = MgFdoConnectionManager::GetInstance();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destruct the object.
///
MgCacheManager::~MgCacheManager()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Dispose the object.
///
void MgCacheManager::Dispose()
{
    delete this;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Get pointer to a process-wide singleton.
///
MgCacheManager* MgCacheManager::GetInstance()
{
    MG_TRY()

    ACE_TRACE("MgCacheManager::GetInstance");

    if (NULL == MgCacheManager::sm_cacheManager.p)
    {
        // Perform Double-Checked Locking Optimization.
        ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, *ACE_Static_Object_Lock::instance(), NULL));

        if (NULL == MgCacheManager::sm_cacheManager.p)
        {
            MgCacheManager::sm_cacheManager = new MgCacheManager();
        }
    }

    MG_CATCH_AND_THROW(L"MgCacheManager.GetInstance")

    ACE_ASSERT(NULL != MgCacheManager::sm_cacheManager.p);

    // To avoid overheads and maintain thread safety,
    // do not assign this returned static singleton to a Ptr object.
    return MgCacheManager::sm_cacheManager;
}


///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Initialize the object.
///
void MgCacheManager::Initialize()
{
    MgConfiguration* configuration = MgConfiguration::GetInstance();
    ACE_ASSERT(NULL != configuration);

    INT32 cacheSize;
    INT32 cacheTimeLimit;

    configuration->GetIntValue(
        MgConfigProperties::FeatureServicePropertiesSection,
        MgConfigProperties::FeatureServicePropertyCacheSize,
        cacheSize,
        MgConfigProperties::DefaultFeatureServicePropertyCacheSize);

    configuration->GetIntValue(
        MgConfigProperties::FeatureServicePropertiesSection,
        MgConfigProperties::FeatureServicePropertyCacheTimeLimit,
        cacheTimeLimit,
        MgConfigProperties::DefaultFeatureServicePropertyCacheTimeLimit);

    m_featureServiceCache.Initialize(cacheSize, cacheTimeLimit);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Clear the caches.
///
void MgCacheManager::ClearCaches()
{
    // The mutex usage and the method call order here are important
    // because they ensure all the caches are in sync.
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_featureServiceCache.m_mutex));

    m_fdoConnectionManager->ClearCache();
    m_featureServiceCache.Clear();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Checks whether or not the current user has the specified permission on the
/// specified resource. This method may throw any exception.
///
void MgCacheManager::CheckPermission(MgResourceIdentifier* resource,
    CREFSTRING permission)
{
    Ptr<MgService> service = m_serviceManager->RequestService(MgServiceType::ResourceService);
    MgServerResourceService* resourceService = dynamic_cast<MgServerResourceService*>(service.p);

    if (NULL == resourceService)
    {
        throw new MgServiceNotAvailableException(
            L"MgCacheManager.CheckPermission",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }
    else
    {
        resourceService->CheckPermission(resource, permission);
    }
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Notify the applicable caches of the changed resource.
///
void MgCacheManager::NotifyResourceChanged(CREFSTRING resource)
{
    if (STRING::npos != resource.rfind(MgResourceType::FeatureSource))
    {
        // The mutex usage and the method call order here are important
        // because they ensure all the caches are in sync.
        ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_featureServiceCache.m_mutex));

        m_fdoConnectionManager->RemoveCachedFdoConnection(resource);
        m_featureServiceCache.RemoveEntry(resource);
    }
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Notify the applicable caches of the changed resource.
///
void MgCacheManager::NotifyResourceChanged(MgResourceIdentifier* resource)
{
    if (NULL != resource && resource->IsResourceTypeOf(MgResourceType::FeatureSource))
    {
        // The mutex usage and the method call order here are important
        // because they ensure all the caches are in sync.
        ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_featureServiceCache.m_mutex));

        m_fdoConnectionManager->RemoveCachedFdoConnection(resource);
        m_featureServiceCache.RemoveEntry(resource);
    }
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Return the cached feature source for a given resource.
/// If it does not exit, then create it.
///
MgFeatureSourceCacheItem* MgCacheManager::GetFeatureSourceCacheItem(MgResourceIdentifier* resource)
{
    Ptr<MgFeatureSourceCacheItem> cacheItem;

    MG_TRY()

    cacheItem = m_featureServiceCache.GetFeatureSource(resource);

    if (NULL == cacheItem.p)
    {
        // Get the Resource Service.
        Ptr<MgResourceService> resourceService = dynamic_cast<MgResourceService*>(
            m_serviceManager->RequestService(MgServiceType::ResourceService));
        ACE_ASSERT(NULL != resourceService.p);

        // Retrieve the XML document from the repository.
        string xmlContent;
        Ptr<MgByteReader> byteReader = resourceService->GetResourceContent(
            resource, MgResourcePreProcessingType::Substitution);

        byteReader->ToStringUtf8(xmlContent);

        // Validate the XML content and parse it.
        MdfParser::FSDSAX2Parser parser;

        if (string::npos != xmlContent.find("<FeatureSource"))
        {
            parser.ParseString(xmlContent.c_str(), xmlContent.length()*sizeof(char));
            if (!parser.GetSucceeded())
            {
                STRING errorMsg = parser.GetErrorMessage();
                MgStringCollection arguments;
                arguments.Add(errorMsg);
                throw new MgInvalidFeatureSourceException(
                    L"MgCacheManager::GetFeatureSourceCacheItem",
                    __LINE__, __WFILE__, &arguments, L"", NULL);
            }
        }

        // Detach the feature source from the parser.
        // It's now the caller's responsibility to delete it.
        auto_ptr<MdfModel::FeatureSource> featureSource;
        featureSource.reset(parser.DetachFeatureSource());

        if (NULL == featureSource.get())
        {
            MgResources* resources = MgResources::GetInstance();
            ACE_ASSERT(NULL != resources);
            STRING message = resources->GetResourceMessage(MgResources::FeatureService,
                L"MgInvalidFdoProvider", NULL);
            MgStringCollection arguments;
            arguments.Add(message);

            throw new MgInvalidFeatureSourceException(
                L"MgCacheManager.GetFeatureSourceCacheItem",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }

        cacheItem = new MgFeatureSourceCacheItem(featureSource.release());
        m_featureServiceCache.SetFeatureSource(resource, cacheItem.p);
    }
    else
    {
        CheckPermission(resource, MgResourcePermission::ReadOnly);
    }

    MG_CATCH_AND_THROW(L"MgCacheManager.GetFeatureSourceCacheItem")

    return cacheItem.Detach();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Return the cached spatial context info for a given resource.
/// If it does not exit, then create it.
///
MgSpatialContextCacheItem* MgCacheManager::GetSpatialContextCacheItem(MgResourceIdentifier* resource)
{
    Ptr<MgSpatialContextCacheItem> cacheItem;

    MG_TRY()

    cacheItem = m_featureServiceCache.GetSpatialContextInfo(resource);

    if (NULL == cacheItem.p)
    {
        Ptr<MgFeatureSourceCacheItem> featureSourceCacheItem = GetFeatureSourceCacheItem(resource);
        MdfModel::FeatureSource* featureSource = featureSourceCacheItem->Get();
        CHECKNULL(featureSource, L"MgCacheManager.GetSpatialContextCacheItem");

        MdfModel::SupplementalSpatialContextInfoCollection* spatialContexts =
            featureSource->GetSupplementalSpatialContextInfo();
        CHECKNULL(spatialContexts, L"MgCacheManager.GetSpatialContextCacheItem");

        auto_ptr<MgSpatialContextInfo> spatialContextInfo;
        spatialContextInfo.reset(new MgSpatialContextInfo());

        for (int i = 0; i < spatialContexts->GetCount(); ++i)
        {
            MdfModel::SupplementalSpatialContextInfo* spatialContext = spatialContexts->GetAt(i);
            CHECKNULL(spatialContext, L"MgCacheManager.GetSpatialContextCacheItem");

            // Get Name element.
            STRING name = (STRING)spatialContext->GetName();
            FdoString* propertyName = name.c_str();
            CHECKNULL(propertyName, L"MgCacheManager.GetSpatialContextCacheItem");

            // Get Coordinate System element.
            STRING coordinateSystem = (STRING)spatialContext->GetCoordinateSystem();
            FdoString* propertyCoordinateSystem = coordinateSystem.c_str();
            CHECKNULL(propertyCoordinateSystem, L"MgCacheManager.GetSpatialContextCacheItem");

            spatialContextInfo->insert(MgSpatialContextInfo::value_type(name, coordinateSystem));
        }

        cacheItem = new MgSpatialContextCacheItem(spatialContextInfo.release());
        m_featureServiceCache.SetSpatialContextInfo(resource, cacheItem.p);
    }
    else
    {
        CheckPermission(resource, MgResourcePermission::ReadOnly);
    }

    MG_CATCH_AND_THROW(L"MgCacheManager.GetSpatialContextCacheItem")

    return cacheItem.Detach();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Determine if a cache notification is needed for the specified resource
/// if it has been changed.
///
bool MgCacheManager::IsResourceChangeNotificationNeeded(MgResourceIdentifier* resource)
{
    bool notificationNeeded = false; 

    // Note that the filter below may be changed in the future or whenever
    // a new resource type is supported.
    // Currently, the cache manager only needs to know if the following types
    // of resources have been changed:
    //
    // Feature Service cache:
    //      MgResourceType::FeatureSource
    // Tile cache:
    //      MgResourceType::MapDefinition
    //      MgResourceType::LayerDefinition
    //      MgResourceType::DrawingSource
    //      MgResourceType::FeatureSource
    //      MgResourceType::SymbolDefinition
    //      MgResourceType::SymbolLibrary

    if (NULL != resource)
    {
        STRING resourceType = resource->GetResourceType();

        if (MgResourceType::Map                          != resourceType
         && MgResourceType::Folder                       != resourceType
         && MgResourceType::LoadProcedure                != resourceType
         && MgResourceType::PrintLayout                  != resourceType
         && MgResourceType::Selection                    != resourceType
         && MgResourceType::WebLayout                    != resourceType
         && MgResourceType::ApplicationDefinition        != resourceType
         && MgResourceType::PrintLayoutDefinition        != resourceType
         && MgResourceType::PrintLayoutElementDefinition != resourceType)
        {
            notificationNeeded = true; 
        }
    }

    return notificationNeeded; 
}

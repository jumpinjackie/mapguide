//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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

#include "AceCommon.h"
#include "FeatureServiceCache.h"

// Process-wide MgFeatureServiceCache
Ptr<MgFeatureServiceCache> MgFeatureServiceCache::m_FeatureServiceCache = MgFeatureServiceCache::GetInstance();

/// <summary>
/// Constructor
/// </summary>
MgFeatureServiceCache::MgFeatureServiceCache(void)
{
    m_featureSchemaCollection = new MgDisposableCollection();
    m_featureSchemaXmlCollection = new MgDisposableCollection();
    m_schemasCollection = new MgDisposableCollection();
    m_spatialContextCollection = new MgDisposableCollection();
    m_classesCollection = new MgDisposableCollection();
    m_classDefinitionCollection = new MgDisposableCollection();
    m_identityPropertiesCollection = new MgDisposableCollection();

    m_cacheLimit = MgConfigProperties::DefaultFeatureServicePropertyCacheSize;
    m_cacheTimeLimit = MgConfigProperties::DefaultFeatureServicePropertyCacheTimeLimit;
}

/// <summary>
/// Destructor
/// </summary>
MgFeatureServiceCache::~MgFeatureServiceCache(void)
{
    if (m_featureSchemaCollection != NULL)
    {
        m_featureSchemaCollection->Clear();
    }

    if (m_featureSchemaXmlCollection != NULL)
    {
        m_featureSchemaXmlCollection->Clear();
    }

    if (m_schemasCollection != NULL)
    {
        m_schemasCollection->Clear();
    }

    if (m_spatialContextCollection != NULL)
    {
        m_spatialContextCollection->Clear();
    }

    if (m_classesCollection != NULL)
    {
        m_classesCollection->Clear();
    }

    if (m_classDefinitionCollection != NULL)
    {
        m_classDefinitionCollection->Clear();
    }

    if (m_identityPropertiesCollection != NULL)
    {
        m_identityPropertiesCollection->Clear();
    }
}

/// <summary>
/// Self Destructor
/// </summary>
void MgFeatureServiceCache::Dispose(void)
{
    delete this;
}

/// <summary>
/// Get pointer to a process-wide MgFeatureServiceCache.
/// </summary>
MgFeatureServiceCache* MgFeatureServiceCache::GetInstance(void)
{
    MG_CONFIGURATION_TRY()

    ACE_TRACE ("MgFeatureServiceCache::GetInstance");

    if (MgFeatureServiceCache::m_FeatureServiceCache == NULL)
    {
        // Perform Double-Checked Locking Optimization.
        ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, *ACE_Static_Object_Lock::instance (), 0));
        if (MgFeatureServiceCache::m_FeatureServiceCache == NULL)
        {
            MgFeatureServiceCache::m_FeatureServiceCache = new MgFeatureServiceCache;
        }
    }

    MG_CONFIGURATION_CATCH_AND_THROW(L"MgFeatureServiceCache.GetInstance")

    // To avoid overheads and maintain thread safety,
    // do not assign this returned static singleton to a Ptr object.
    return MgFeatureServiceCache::m_FeatureServiceCache;
}

void MgFeatureServiceCache::Initialize(INT32 cacheLimit, INT32 cacheTimeLimit)
{
    m_cacheLimit = cacheLimit;
    m_cacheTimeLimit = cacheTimeLimit;
}

void MgFeatureServiceCache::AddFeatureSchema(MgResourceIdentifier* resource, CREFSTRING schemaName, MgFeatureSchemaCollection* featureSchemaCollection)
{
    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    // Check to see if caching is enabled
    if(0 < m_cacheLimit)
    {
        // Check to see if there is room in the cache
        INT32 size = m_featureSchemaCollection->GetCount();
        if(size == m_cacheLimit)
        {
            // If not we need to remove an entry
            Ptr<MgFeatureSchemaCacheEntry> removeEntry;
            removeEntry = (MgFeatureSchemaCacheEntry*)(m_featureSchemaCollection->GetItem(0));
            m_featureSchemaCollection->Remove(removeEntry);
        }

        Ptr<MgFeatureSchemaCacheEntry> cacheEntry;
        cacheEntry = new MgFeatureSchemaCacheEntry(resource->ToString(), schemaName, featureSchemaCollection);
        m_featureSchemaCollection->Add(cacheEntry);
    }
}

void MgFeatureServiceCache::RemoveFeatureSchema(MgResourceIdentifier* resource, CREFSTRING schemaName)
{
    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    INT32 size = m_featureSchemaCollection->GetCount();
    for(int i=0;i<size;i++)
    {
        Ptr<MgFeatureSchemaCacheEntry> cacheEntry;
        cacheEntry = (MgFeatureSchemaCacheEntry*)(m_featureSchemaCollection->GetItem(i));
        if(cacheEntry)
        {
            if((cacheEntry->m_resource == resource->ToString()) && (cacheEntry->m_schemaName == schemaName))
            {
                // Found a match, so remove it
                m_featureSchemaCollection->Remove(cacheEntry);
                break;
            }
        }
    }
}

MgFeatureSchemaCollection* MgFeatureServiceCache::ContainsFeatureSchema(MgResourceIdentifier* resource, CREFSTRING schemaName)
{
    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, false));

    Ptr<MgFeatureSchemaCollection> featureSchemaCollection;

    INT32 size = m_featureSchemaCollection->GetCount();
    for(int i=0;i<size;i++)
    {
        Ptr<MgFeatureSchemaCacheEntry> cacheEntry;
        cacheEntry = (MgFeatureSchemaCacheEntry*)(m_featureSchemaCollection->GetItem(i));
        if(cacheEntry)
        {
            if((cacheEntry->m_resource == resource->ToString()) && (cacheEntry->m_schemaName == schemaName))
            {
                // Found a match
                featureSchemaCollection = cacheEntry->m_featureSchemaCollection;
                break;
            }
        }
    }

    return featureSchemaCollection.Detach();
}

void MgFeatureServiceCache::AddFeatureSchemaXml(MgResourceIdentifier* resource, CREFSTRING schemaName, CREFSTRING schema)
{
    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    if(0 < m_cacheLimit)
    {
        // Check to see if there is room in the cache
        INT32 size = m_featureSchemaXmlCollection->GetCount();
        if(size == m_cacheLimit)
        {
            // If not we need to remove an entry
            Ptr<MgFeatureSchemaXmlCacheEntry> removeEntry;
            removeEntry = (MgFeatureSchemaXmlCacheEntry*)(m_featureSchemaXmlCollection->GetItem(0));
            m_featureSchemaXmlCollection->Remove(removeEntry);
        }

        Ptr<MgFeatureSchemaXmlCacheEntry> cacheEntry;
        cacheEntry = new MgFeatureSchemaXmlCacheEntry(resource->ToString(), schemaName, schema);
        m_featureSchemaXmlCollection->Add(cacheEntry);
    }
}

void MgFeatureServiceCache::RemoveFeatureSchemaXml(MgResourceIdentifier* resource, CREFSTRING schemaName)
{
    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    INT32 size = m_featureSchemaXmlCollection->GetCount();
    for(int i=0;i<size;i++)
    {
        Ptr<MgFeatureSchemaXmlCacheEntry> cacheEntry;
        cacheEntry = (MgFeatureSchemaXmlCacheEntry*)(m_featureSchemaXmlCollection->GetItem(i));
        if(cacheEntry)
        {
            if((cacheEntry->m_resource == resource->ToString()) && (cacheEntry->m_schemaName == schemaName))
            {
                // Found a match, so remove it
                m_featureSchemaXmlCollection->Remove(cacheEntry);
                break;
            }
        }
    }
}

STRING MgFeatureServiceCache::ContainsFeatureSchemaXml(MgResourceIdentifier* resource, CREFSTRING schemaName)
{
    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, false));

    STRING schemaXml;

    INT32 size = m_featureSchemaXmlCollection->GetCount();
    for(int i=0;i<size;i++)
    {
        Ptr<MgFeatureSchemaXmlCacheEntry> cacheEntry;
        cacheEntry = (MgFeatureSchemaXmlCacheEntry*)(m_featureSchemaXmlCollection->GetItem(i));
        if(cacheEntry)
        {
            if((cacheEntry->m_resource == resource->ToString()) && (cacheEntry->m_schemaName == schemaName))
            {
                // Found a match
                schemaXml = cacheEntry->m_schemaXml;
                break;
            }
        }
    }

    return schemaXml;
}

void MgFeatureServiceCache::AddSchemas(MgResourceIdentifier* resource, MgStringCollection* schemas)
{
    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    if(0 < m_cacheLimit)
    {
        // Check to see if there is room in the cache
        INT32 size = m_schemasCollection->GetCount();
        if(size == m_cacheLimit)
        {
            // If not we need to remove an entry
            Ptr<MgSchemasCacheEntry> removeEntry;
            removeEntry = (MgSchemasCacheEntry*)(m_schemasCollection->GetItem(0));
            m_schemasCollection->Remove(removeEntry);
        }

        Ptr<MgSchemasCacheEntry> cacheEntry;
        cacheEntry = new MgSchemasCacheEntry(resource->ToString(), schemas);
        m_schemasCollection->Add(cacheEntry);
    }
}

void MgFeatureServiceCache::RemoveSchemas(MgResourceIdentifier* resource)
{
    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    INT32 size = m_schemasCollection->GetCount();
    for(int i=0;i<size;i++)
    {
        Ptr<MgSchemasCacheEntry> cacheEntry;
        cacheEntry = (MgSchemasCacheEntry*)(m_schemasCollection->GetItem(i));
        if(cacheEntry)
        {
            if(cacheEntry->m_resource == resource->ToString())
            {
                // Found a match, so remove it
                m_schemasCollection->Remove(cacheEntry);
                break;
            }
        }
    }
}

MgStringCollection* MgFeatureServiceCache::ContainsSchemas(MgResourceIdentifier* resource)
{
    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, false));

    Ptr<MgStringCollection> schemasCollection;

    INT32 size = m_schemasCollection->GetCount();
    for(int i=0;i<size;i++)
    {
        Ptr<MgSchemasCacheEntry> cacheEntry;
        cacheEntry = (MgSchemasCacheEntry*)(m_schemasCollection->GetItem(i));
        if(cacheEntry)
        {
            if(cacheEntry->m_resource == resource->ToString())
            {
                // Found a match
                schemasCollection = cacheEntry->m_schemasCollection;
                break;
            }
        }
    }

    return schemasCollection.Detach();
}

void MgFeatureServiceCache::AddSpatialContext(MgResourceIdentifier* resource, bool bActive, MgSpatialContextReader* spatialContextReader)
{
    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    if(0 < m_cacheLimit)
    {
        // Check to see if there is room in the cache
        INT32 size = m_spatialContextCollection->GetCount();
        if(size == m_cacheLimit)
        {
            // If not we need to remove an entry
            Ptr<MgSpatialContextCacheEntry> removeEntry;
            removeEntry = (MgSpatialContextCacheEntry*)(m_spatialContextCollection->GetItem(0));
            m_spatialContextCollection->Remove(removeEntry);
        }

        Ptr<MgMemoryStreamHelper> helper = new MgMemoryStreamHelper();
        Ptr<MgStream> stream = new MgStream(helper);
        Ptr<MgSpatialContextReader> spatialContextReaderCopy = new MgSpatialContextReader();
        spatialContextReader->Serialize(stream);
        spatialContextReaderCopy->Deserialize(stream);

        Ptr<MgSpatialContextCacheEntry> cacheEntry;
        cacheEntry = new MgSpatialContextCacheEntry(resource->ToString(), bActive, spatialContextReaderCopy);
        m_spatialContextCollection->Add(cacheEntry);
    }
}

void MgFeatureServiceCache::RemoveSpatialContext(MgResourceIdentifier* resource, bool bActive)
{
    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    INT32 size = m_spatialContextCollection->GetCount();
    for(int i=0;i<size;i++)
    {
        Ptr<MgSpatialContextCacheEntry> cacheEntry;
        cacheEntry = (MgSpatialContextCacheEntry*)(m_spatialContextCollection->GetItem(i));
        if(cacheEntry)
        {
            if((cacheEntry->m_resource == resource->ToString()) && (cacheEntry->m_bActive == bActive))
            {
                // Found a match, so remove it
                m_spatialContextCollection->Remove(cacheEntry);
                break;
            }
        }
    }
}

MgSpatialContextReader* MgFeatureServiceCache::ContainsSpatialContext(MgResourceIdentifier* resource, bool bActive)
{
    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, false));

    Ptr<MgSpatialContextReader> spatialContextReader;

    INT32 size = m_spatialContextCollection->GetCount();
    for(int i=0;i<size;i++)
    {
        Ptr<MgSpatialContextCacheEntry> cacheEntry;
        cacheEntry = (MgSpatialContextCacheEntry*)(m_spatialContextCollection->GetItem(i));
        if(cacheEntry)
        {
            if((cacheEntry->m_resource == resource->ToString()) && (cacheEntry->m_bActive == bActive))
            {
                // Found a match
                Ptr<MgMemoryStreamHelper> helper = new MgMemoryStreamHelper();
                Ptr<MgStream> stream = new MgStream(helper);
                spatialContextReader = new MgSpatialContextReader;

                cacheEntry->m_spatialContextReader->Serialize(stream);
                spatialContextReader->Deserialize(stream);
                break;
            }
        }
    }

    return spatialContextReader.Detach();
}

void MgFeatureServiceCache::AddClasses(MgResourceIdentifier* resource, CREFSTRING schemaName, MgStringCollection* classes)
{
    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    if(0 < m_cacheLimit)
    {
        // Check to see if there is room in the cache
        INT32 size = m_classesCollection->GetCount();
        if(size == m_cacheLimit)
        {
            // If not we need to remove an entry
            Ptr<MgClassesCacheEntry> removeEntry;
            removeEntry = (MgClassesCacheEntry*)(m_classesCollection->GetItem(0));
            m_classesCollection->Remove(removeEntry);
        }

        Ptr<MgClassesCacheEntry> cacheEntry;
        cacheEntry = new MgClassesCacheEntry(resource->ToString(), schemaName, classes);
        m_classesCollection->Add(cacheEntry);
    }
}

void MgFeatureServiceCache::RemoveClasses(MgResourceIdentifier* resource, CREFSTRING schemaName)
{
    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    INT32 size = m_classesCollection->GetCount();
    for(int i=0;i<size;i++)
    {
        Ptr<MgClassesCacheEntry> cacheEntry;
        cacheEntry = (MgClassesCacheEntry*)(m_classesCollection->GetItem(i));
        if(cacheEntry)
        {
            if((cacheEntry->m_resource == resource->ToString()) && (cacheEntry->m_schemaName == schemaName))
            {
                // Found a match, so remove it
                m_classesCollection->Remove(cacheEntry);
                break;
            }
        }
    }
}

MgStringCollection* MgFeatureServiceCache::ContainsClasses(MgResourceIdentifier* resource, CREFSTRING schemaName)
{
    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, false));

    Ptr<MgStringCollection> classesCollection;

    INT32 size = m_classesCollection->GetCount();
    for(int i=0;i<size;i++)
    {
        Ptr<MgClassesCacheEntry> cacheEntry;
        cacheEntry = (MgClassesCacheEntry*)(m_classesCollection->GetItem(i));
        if(cacheEntry)
        {
            if((cacheEntry->m_resource == resource->ToString()) && (cacheEntry->m_schemaName == schemaName))
            {
                // Found a match
                classesCollection = cacheEntry->m_classesCollection;
                break;
            }
        }
    }

    return classesCollection.Detach();
}

void MgFeatureServiceCache::AddClassDefinition(MgResourceIdentifier* resource, CREFSTRING schemaName, CREFSTRING className, MgClassDefinition* classDefinition)
{
    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    if(0 < m_cacheLimit)
    {
        // Check to see if there is room in the cache
        INT32 size = m_classDefinitionCollection->GetCount();
        if(size == m_cacheLimit)
        {
            // If not we need to remove an entry
            Ptr<MgClassDefinitionCacheEntry> removeEntry;
            removeEntry = (MgClassDefinitionCacheEntry*)(m_classDefinitionCollection->GetItem(0));
            m_classDefinitionCollection->Remove(removeEntry);
        }

        Ptr<MgClassDefinitionCacheEntry> cacheEntry;
        cacheEntry = new MgClassDefinitionCacheEntry(resource->ToString(), schemaName, className, classDefinition);
        m_classDefinitionCollection->Add(cacheEntry);
    }
}

void MgFeatureServiceCache::RemoveClassDefinition(MgResourceIdentifier* resource, CREFSTRING schemaName, CREFSTRING className)
{
    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    INT32 size = m_classDefinitionCollection->GetCount();
    for(int i=0;i<size;i++)
    {
        Ptr<MgClassDefinitionCacheEntry> cacheEntry;
        cacheEntry = (MgClassDefinitionCacheEntry*)(m_classDefinitionCollection->GetItem(i));
        if(cacheEntry)
        {
            if((cacheEntry->m_resource == resource->ToString()) && (cacheEntry->m_schemaName == schemaName) && (cacheEntry->m_className == className))
            {
                // Found a match, so remove it
                m_classDefinitionCollection->Remove(cacheEntry);
                break;
            }
        }
    }
}

MgClassDefinition* MgFeatureServiceCache::ContainsClassDefinition(MgResourceIdentifier* resource, CREFSTRING schemaName, CREFSTRING className)
{
    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, false));

    Ptr<MgClassDefinition> classDefinition;

    INT32 size = m_classDefinitionCollection->GetCount();
    for(int i=0;i<size;i++)
    {
        Ptr<MgClassDefinitionCacheEntry> cacheEntry;
        cacheEntry = (MgClassDefinitionCacheEntry*)(m_classDefinitionCollection->GetItem(i));
        if(cacheEntry)
        {
            if((cacheEntry->m_resource == resource->ToString()) && (cacheEntry->m_schemaName == schemaName) && (cacheEntry->m_className == className))
            {
                // Found a match
                classDefinition = cacheEntry->m_classDefinition;
                break;
            }
        }
    }

    return classDefinition.Detach();
}

void MgFeatureServiceCache::AddIdentityProperties(MgResourceIdentifier* resource, CREFSTRING schemaName, CREFSTRING className, MgPropertyDefinitionCollection* identityPropertiesCollection)
{
    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    if(0 < m_cacheLimit)
    {
        // Check to see if there is room in the cache
        INT32 size = m_identityPropertiesCollection->GetCount();
        if(size == m_cacheLimit)
        {
            // If not we need to remove an entry
            Ptr<MgIdentityPropertiesCacheEntry> removeEntry;
            removeEntry = (MgIdentityPropertiesCacheEntry*)(m_identityPropertiesCollection->GetItem(0));
            m_identityPropertiesCollection->Remove(removeEntry);
        }

        Ptr<MgIdentityPropertiesCacheEntry> cacheEntry;
        cacheEntry = new MgIdentityPropertiesCacheEntry(resource->ToString(), schemaName, className, identityPropertiesCollection);
        m_identityPropertiesCollection->Add(cacheEntry);
    }
}

void MgFeatureServiceCache::RemoveIdentityProperties(MgResourceIdentifier* resource, CREFSTRING schemaName, CREFSTRING className)
{
    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    INT32 size = m_identityPropertiesCollection->GetCount();
    for(int i=0;i<size;i++)
    {
        Ptr<MgIdentityPropertiesCacheEntry> cacheEntry;
        cacheEntry = (MgIdentityPropertiesCacheEntry*)(m_identityPropertiesCollection->GetItem(i));
        if(cacheEntry)
        {
            if((cacheEntry->m_resource == resource->ToString()) && (cacheEntry->m_schemaName == schemaName) && (cacheEntry->m_className == className))
            {
                // Found a match, so remove it
                m_identityPropertiesCollection->Remove(cacheEntry);
                break;
            }
        }
    }
}

MgPropertyDefinitionCollection* MgFeatureServiceCache::ContainsIdentityProperties(MgResourceIdentifier* resource, CREFSTRING schemaName, CREFSTRING className)
{
    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, false));

    Ptr<MgPropertyDefinitionCollection> identityPropertiesCollection;

    INT32 size = m_identityPropertiesCollection->GetCount();
    for(int i=0;i<size;i++)
    {
        Ptr<MgIdentityPropertiesCacheEntry> cacheEntry;
        cacheEntry = (MgIdentityPropertiesCacheEntry*)(m_identityPropertiesCollection->GetItem(i));
        if(cacheEntry)
        {
            if((cacheEntry->m_resource == resource->ToString()) && (cacheEntry->m_schemaName == schemaName) && (cacheEntry->m_className == className))
            {
                // Found a match
                identityPropertiesCollection = cacheEntry->m_identityPropertiesCollection;
                break;
            }
        }
    }

    return identityPropertiesCollection.Detach();
}

void MgFeatureServiceCache::RemoveExpiredEntries()
{
    ACE_MT (ACE_GUARD (ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));

    // Get the current datetime
    ACE_Time_Value now = ACE_High_Res_Timer::gettimeofday();

    int i;
    INT32 size = m_featureSchemaCollection->GetCount();
    for(i=0;i<size;i++)
    {
        Ptr<MgFeatureSchemaCacheEntry> cacheEntry;
        cacheEntry = (MgFeatureSchemaCacheEntry*)(m_featureSchemaCollection->GetItem(i));
        if(cacheEntry)
        {
            ACE_Time_Value diffTime = now - cacheEntry->m_timeStamp;
            long diff = diffTime.sec();
            if(diff  > m_cacheTimeLimit)
            {
                // Entry has reached the timelimit
                m_featureSchemaCollection->Remove(cacheEntry);
            }
        }
    }

    size = m_featureSchemaXmlCollection->GetCount();
    for(i=0;i<size;i++)
    {
        Ptr<MgFeatureSchemaXmlCacheEntry> cacheEntry;
        cacheEntry = (MgFeatureSchemaXmlCacheEntry*)(m_featureSchemaXmlCollection->GetItem(i));
        if(cacheEntry)
        {
            ACE_Time_Value diffTime = now - cacheEntry->m_timeStamp;
            long diff = diffTime.sec();
            if(diff  > m_cacheTimeLimit)
            {
                // Entry has reached the timelimit
                m_featureSchemaXmlCollection->Remove(cacheEntry);
            }
        }
    }

    size = m_schemasCollection->GetCount();
    for(i=0;i<size;i++)
    {
        Ptr<MgSchemasCacheEntry> cacheEntry;
        cacheEntry = (MgSchemasCacheEntry*)(m_schemasCollection->GetItem(i));
        if(cacheEntry)
        {
            ACE_Time_Value diffTime = now - cacheEntry->m_timeStamp;
            long diff = diffTime.sec();
            if(diff  > m_cacheTimeLimit)
            {
                // Entry has reached the timelimit
                m_schemasCollection->Remove(cacheEntry);
            }
        }
    }

    size = m_spatialContextCollection->GetCount();
    for(i=0;i<size;i++)
    {
        Ptr<MgSpatialContextCacheEntry> cacheEntry;
        cacheEntry = (MgSpatialContextCacheEntry*)(m_spatialContextCollection->GetItem(i));
        if(cacheEntry)
        {
            ACE_Time_Value diffTime = now - cacheEntry->m_timeStamp;
            long diff = diffTime.sec();
            if(diff  > m_cacheTimeLimit)
            {
                // Entry has reached the timelimit
                m_spatialContextCollection->Remove(cacheEntry);
            }
        }
    }

    size = m_classesCollection->GetCount();
    for(i=0;i<size;i++)
    {
        Ptr<MgClassesCacheEntry> cacheEntry;
        cacheEntry = (MgClassesCacheEntry*)(m_classesCollection->GetItem(i));
        if(cacheEntry)
        {
            ACE_Time_Value diffTime = now - cacheEntry->m_timeStamp;
            long diff = diffTime.sec();
            if(diff  > m_cacheTimeLimit)
            {
                // Entry has reached the timelimit
                m_classesCollection->Remove(cacheEntry);
            }
        }
    }

    size = m_classDefinitionCollection->GetCount();
    for(i=0;i<size;i++)
    {
        Ptr<MgClassDefinitionCacheEntry> cacheEntry;
        cacheEntry = (MgClassDefinitionCacheEntry*)(m_classDefinitionCollection->GetItem(i));
        if(cacheEntry)
        {
            ACE_Time_Value diffTime = now - cacheEntry->m_timeStamp;
            long diff = diffTime.sec();
            if(diff  > m_cacheTimeLimit)
            {
                // Entry has reached the timelimit
                m_classDefinitionCollection->Remove(cacheEntry);
            }
        }
    }

    size = m_identityPropertiesCollection->GetCount();
    for(i=0;i<size;i++)
    {
        Ptr<MgIdentityPropertiesCacheEntry> cacheEntry;
        cacheEntry = (MgIdentityPropertiesCacheEntry*)(m_identityPropertiesCollection->GetItem(i));
        if(cacheEntry)
        {
            ACE_Time_Value diffTime = now - cacheEntry->m_timeStamp;
            long diff = diffTime.sec();
            if(diff  > m_cacheTimeLimit)
            {
                // Entry has reached the timelimit
                m_identityPropertiesCollection->Remove(cacheEntry);
            }
        }
    }
}

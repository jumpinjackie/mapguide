//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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

#ifndef _MG_FEATURE_SERVICE_CACHE_H_
#define _MG_FEATURE_SERVICE_CACHE_H_

#include "ServerFeatureDllExport.h"

class MgFeatureServiceCacheEntry : public MgGuardDisposable
{
public:
    MgFeatureServiceCacheEntry () { };
    virtual ~MgFeatureServiceCacheEntry () { };
    virtual void Dispose() = 0;

    STRING m_resource;
    ACE_Time_Value m_timeStamp;
};


class MgFeatureSchemaCacheEntry : public MgFeatureServiceCacheEntry 
{
public:
    MgFeatureSchemaCacheEntry(CREFSTRING resource, CREFSTRING schemaName, MgFeatureSchemaCollection* featureSchemaCollection)
    {
        m_resource = resource;
        m_schemaName = schemaName;
        m_timeStamp = ACE_High_Res_Timer::gettimeofday();
        m_featureSchemaCollection = SAFE_ADDREF(featureSchemaCollection);
    }

    virtual ~MgFeatureSchemaCacheEntry() {}
    virtual void Dispose() { delete this; }

    STRING m_schemaName;
    Ptr<MgFeatureSchemaCollection> m_featureSchemaCollection;

protected:
    MgFeatureSchemaCacheEntry() {}
};

class MgFeatureSchemaXmlCacheEntry : public MgFeatureServiceCacheEntry 
{
public:
    MgFeatureSchemaXmlCacheEntry(CREFSTRING resource, CREFSTRING schemaName, CREFSTRING schemaXml)
    {
        m_resource = resource;
        m_schemaName = schemaName;
        m_timeStamp = ACE_High_Res_Timer::gettimeofday();
        m_schemaXml = schemaXml;
    }

    virtual ~MgFeatureSchemaXmlCacheEntry() {}
    virtual void Dispose() { delete this; }

    STRING m_schemaName;
    STRING m_schemaXml;

protected:
    MgFeatureSchemaXmlCacheEntry() { }
};

class MgSchemasCacheEntry : public MgFeatureServiceCacheEntry 
{
public:
    MgSchemasCacheEntry(CREFSTRING resource, MgStringCollection* schemas)
    {
        m_resource = resource;
        m_timeStamp = ACE_High_Res_Timer::gettimeofday();
        m_schemasCollection = SAFE_ADDREF(schemas);
    }

    virtual ~MgSchemasCacheEntry() {}
    virtual void Dispose() { delete this; }

    Ptr<MgStringCollection> m_schemasCollection;

protected:
    MgSchemasCacheEntry() { }
};

class MgSpatialContextCacheEntry : public MgFeatureServiceCacheEntry 
{
public:
    MgSpatialContextCacheEntry(CREFSTRING resource, bool bActive, MgSpatialContextReader* spatialContextReader)
    {
        m_resource = resource;
        m_bActive = bActive;
        m_timeStamp = ACE_High_Res_Timer::gettimeofday();
        m_spatialContextReader = SAFE_ADDREF(spatialContextReader);
    }

    virtual ~MgSpatialContextCacheEntry() {}
    virtual void Dispose() { delete this; }

    bool m_bActive;
    Ptr<MgSpatialContextReader> m_spatialContextReader;

protected:
    MgSpatialContextCacheEntry() { }
};

class MgClassesCacheEntry : public MgFeatureServiceCacheEntry 
{
public:
    MgClassesCacheEntry(CREFSTRING resource, CREFSTRING schemaName, MgStringCollection* classes)
    {
        m_resource = resource;
        m_schemaName = schemaName;
        m_timeStamp = ACE_High_Res_Timer::gettimeofday();
        m_classesCollection = SAFE_ADDREF(classes);
    }

    virtual ~MgClassesCacheEntry() {}
    virtual void Dispose() { delete this; }

    STRING m_schemaName;
    Ptr<MgStringCollection> m_classesCollection;

protected:
    MgClassesCacheEntry() { }
};

class MgClassDefinitionCacheEntry : public MgFeatureServiceCacheEntry 
{
public:
    MgClassDefinitionCacheEntry(CREFSTRING resource, CREFSTRING schemaName, CREFSTRING className, MgClassDefinition* classDefinition)
    {
        m_resource = resource;
        m_schemaName = schemaName;
        m_className = className;
        m_timeStamp = ACE_High_Res_Timer::gettimeofday();
        m_classDefinition = SAFE_ADDREF(classDefinition);
    }

    virtual ~MgClassDefinitionCacheEntry() {}
    virtual void Dispose() { delete this; }

    STRING m_schemaName;
    STRING m_className;
    Ptr<MgClassDefinition> m_classDefinition;

protected:
    MgClassDefinitionCacheEntry() { }
};

class MgIdentityPropertiesCacheEntry : public MgFeatureServiceCacheEntry 
{
public:
    MgIdentityPropertiesCacheEntry(CREFSTRING resource, CREFSTRING schemaName, CREFSTRING className, MgPropertyDefinitionCollection* identityPropertiesCollection)
    {
        m_resource = resource;
        m_schemaName = schemaName;
        m_className = className;
        m_timeStamp = ACE_High_Res_Timer::gettimeofday();
        m_identityPropertiesCollection = SAFE_ADDREF(identityPropertiesCollection);
    }

    virtual ~MgIdentityPropertiesCacheEntry() {}
    virtual void Dispose() { delete this; }

    STRING m_schemaName;
    STRING m_className;
    Ptr<MgPropertyDefinitionCollection> m_identityPropertiesCollection;

protected:
    MgIdentityPropertiesCacheEntry() { }
};

class MgFeatureServiceCache;
template class Ptr<MgFeatureServiceCache>;

class MG_SERVER_FEATURE_API MgFeatureServiceCache : public MgGuardDisposable
{
    DECLARE_CLASSNAME(MgFeatureServiceCache)

public:
    /// <summary>
    /// Get pointer to a process-wide MgFeatureServiceCache.
    /// </summary>
    static MgFeatureServiceCache* GetInstance(void);

    void Initialize(INT32 cacheLimit, INT32 cacheTimeLimit);

    // The following functions work with all cache entries
    void ClearCache();
    void RemoveExpiredEntries();
    void RemoveEntry(MgResourceIdentifier* resource);

    // Feature Schema Cache
    void AddFeatureSchema(MgResourceIdentifier* resource, CREFSTRING schemaName, MgFeatureSchemaCollection* featureSchemaCollection);
    void RemoveFeatureSchema(MgResourceIdentifier* resource, CREFSTRING schemaName);
    MgFeatureSchemaCollection* ContainsFeatureSchema(MgResourceIdentifier* resource, CREFSTRING schemaName);

    // Feature Schema XML Cache
    void AddFeatureSchemaXml(MgResourceIdentifier* resource, CREFSTRING schemaName, CREFSTRING schema);
    void RemoveFeatureSchemaXml(MgResourceIdentifier* resource, CREFSTRING schemaName);
    STRING ContainsFeatureSchemaXml(MgResourceIdentifier* resource, CREFSTRING schemaName);

    // Feature Schemas Cache
    void AddSchemas(MgResourceIdentifier* resource, MgStringCollection* schemas);
    void RemoveSchemas(MgResourceIdentifier* resource);
    MgStringCollection* ContainsSchemas(MgResourceIdentifier* resource);

    // Spatial Context Cache
    void AddSpatialContext(MgResourceIdentifier* resource, bool bActive, MgSpatialContextReader* spatialContextReader);
    void RemoveSpatialContext(MgResourceIdentifier* resource, bool bActive);
    MgSpatialContextReader* ContainsSpatialContext(MgResourceIdentifier* resource, bool bActive);

    // Feature Schema Classes Cache
    void AddClasses(MgResourceIdentifier* resource, CREFSTRING schemaName, MgStringCollection* classes);
    void RemoveClasses(MgResourceIdentifier* resource, CREFSTRING schemaName);
    MgStringCollection* ContainsClasses(MgResourceIdentifier* resource, CREFSTRING schemaName);

    // Feature Schema Class Definition Cache
    void AddClassDefinition(MgResourceIdentifier* resource, CREFSTRING schemaName, CREFSTRING className, MgClassDefinition* classDefinition);
    void RemoveClassDefinition(MgResourceIdentifier* resource, CREFSTRING schemaName, CREFSTRING className);
    MgClassDefinition* ContainsClassDefinition(MgResourceIdentifier* resource, CREFSTRING schemaName, CREFSTRING className);

    // Feature Schema Class Identity Properties Cache
    void AddIdentityProperties(MgResourceIdentifier* resource, CREFSTRING schemaName, CREFSTRING className, MgPropertyDefinitionCollection* identityPropertiesCollection);
    void RemoveIdentityProperties(MgResourceIdentifier* resource, CREFSTRING schemaName, CREFSTRING className);
    MgPropertyDefinitionCollection* ContainsIdentityProperties(MgResourceIdentifier* resource, CREFSTRING schemaName, CREFSTRING className);

protected:

    /// <summary>
    /// Self Destructor
    /// </summary>

    virtual void Dispose();

    /// <summary>
    /// Destructor
    /// </summary>

    virtual ~MgFeatureServiceCache(void);

    MgFeatureServiceCache(void);

private:
    void RemoveExpiredEntries(MgDisposableCollection* collection);
    void RemoveEntry(MgResourceIdentifier* resource, MgDisposableCollection* collection);

    /// Pointer to a process-wide singleton.
    static Ptr<MgFeatureServiceCache> m_FeatureServiceCache;

    Ptr<MgDisposableCollection> m_featureSchemaCollection;
    Ptr<MgDisposableCollection> m_featureSchemaXmlCollection;
    Ptr<MgDisposableCollection> m_schemasCollection;
    Ptr<MgDisposableCollection> m_spatialContextCollection;
    Ptr<MgDisposableCollection> m_classesCollection;
    Ptr<MgDisposableCollection> m_classDefinitionCollection;
    Ptr<MgDisposableCollection> m_identityPropertiesCollection;

    INT32 m_cacheLimit;
    INT32 m_cacheTimeLimit;

    ACE_Recursive_Thread_Mutex m_mutex;
};

#endif

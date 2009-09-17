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

#ifndef _MG_SERVER_CREATE_FEATURE_SOURCE_H
#define _MG_SERVER_CREATE_FEATURE_SOURCE_H

#include "MapGuideCommon.h"
#include "Fdo.h"

class MgServerCreateFeatureSource
{
public:
    MgServerCreateFeatureSource();
    ~MgServerCreateFeatureSource();
    void CreateFeatureSource(MgResourceIdentifier* resource, MgFeatureSourceParams* sourceParams);
};

class MgServerCreateFileFeatureSource: public MgGuardDisposable
{
public:
    MgServerCreateFileFeatureSource(MgResourceIdentifier* resource, MgFileFeatureSourceParams* params);
    virtual ~MgServerCreateFileFeatureSource();
    void CreateFeatureSource(bool bCheckFeatureClass = false, bool bCheckSpatialContext = false);
    virtual void Dispose();

protected:
    virtual void CreateDataStore(FdoIConnection* conn);
    virtual void ApplySchemaAndCreateSpatialContext(FdoIConnection* conn);
    virtual void SetFeatureSourceDefinition(MgResourceService* resourceService);
    virtual void SetResourceData(MgResourceService* resourceService);
    virtual STRING GetFirstConnectionString();
    virtual STRING GetSecondConnectionString();
    virtual STRING GetFeatureSourceParameterString() const;

protected:
    STRING GetFileName() const;
    MgResourceService* GetResourceService();
    void ApplySchemaAndCreateSpatialContextInternal(FdoIConnection* conn);
    void SetResourceDataInternal(MgResourceService* resourceService, STRING source, STRING target);

protected:
    STRING m_fileExtension;
    STRING m_connectParamName;
    STRING m_tempFileName;
    MgResourceIdentifier* m_resource;
    MgFileFeatureSourceParams* m_params;
};

class MgServerCreateSdfFeatureSource: public MgServerCreateFileFeatureSource
{
public:
    MgServerCreateSdfFeatureSource(MgResourceIdentifier* resource, MgFileFeatureSourceParams* params);
    virtual ~MgServerCreateSdfFeatureSource();
protected:
    virtual STRING GetFeatureSourceParameterString() const;
};

class MgServerCreateShpFeatureSource: public MgServerCreateFileFeatureSource
{
public:
    MgServerCreateShpFeatureSource(MgResourceIdentifier* resource, MgFileFeatureSourceParams* params);
    virtual ~MgServerCreateShpFeatureSource();
protected:
    virtual void CreateDataStore(FdoIConnection* conn);
    virtual void ApplySchemaAndCreateSpatialContext(FdoIConnection* conn);
    virtual void SetResourceData(MgResourceService* resourceService);
    virtual STRING GetFirstConnectionString();
    virtual STRING GetSecondConnectionString();
    virtual STRING GetFeatureSourceParameterString() const;
};

class MgServerCreateSqliteFeatureSource: public MgServerCreateFileFeatureSource
{
public:
    MgServerCreateSqliteFeatureSource(MgResourceIdentifier* resource, MgFileFeatureSourceParams* params);
    virtual ~MgServerCreateSqliteFeatureSource();
protected:
    virtual STRING GetSecondConnectionString();
    virtual STRING GetFeatureSourceParameterString() const;
};

#endif

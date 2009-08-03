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

#include "ServerFeatureServiceDefs.h"
#include "ServerCreateFeatureSource.h"
#include "ServerFeatureConnection.h"
#include "ServerDescribeSchema.h"
#include "ServerFeatureUtil.h"

#include "Fdo/Commands/DataStore/ICreateDataStore.h"

MgServerCreateFeatureSource::MgServerCreateFeatureSource()
{
}

MgServerCreateFeatureSource::~MgServerCreateFeatureSource()
{
}

void MgServerCreateFeatureSource::CreateFeatureSource(MgResourceIdentifier* resource, MgFeatureSourceParams* sourceParams)
{
    MG_FEATURE_SERVICE_TRY()

    MgFileFeatureSourceParams* params = dynamic_cast<MgFileFeatureSourceParams*>(sourceParams);
    if (NULL != params)
    {
        Ptr<MgServerCreateFileFeatureSource> creator = NULL;
        STRING providerName = params->GetProviderName();
        if (providerName == L"OSGeo.SDF") // NOXLATE
            creator = new MgServerCreateSdfFeatureSource(resource, params);
        else if (providerName == L"OSGeo.SHP") // NOXLATE
            creator = new MgServerCreateShpFeatureSource(resource, params);
        else if (providerName == L"OSGeo.SQLite") // NOXLATE
            creator = new MgServerCreateSqliteFeatureSource(resource, params);
        else
            throw new MgInvalidArgumentException(L"MgServerCreateFeatureSource.CreateFeatureSource", __LINE__, __WFILE__, NULL, L"", NULL);
        creator->CreateFeatureSource();
    }

    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH_AND_THROW(resource, L"MgServerCreateFeatureSource.CreateFeatureSource")
}

MgServerCreateFileFeatureSource::MgServerCreateFileFeatureSource(
    MgResourceIdentifier* resource,
    MgFileFeatureSourceParams* params)
{
    m_resource = resource;
    SAFE_ADDREF(resource);

    m_params = params;
    SAFE_ADDREF(params);
}

MgServerCreateFileFeatureSource::~MgServerCreateFileFeatureSource()
{
    SAFE_RELEASE(m_resource);
    SAFE_RELEASE(m_params);
}

void MgServerCreateFileFeatureSource::CreateFeatureSource()
{
    //Some basic schema validation:
    //  A schema must be supplied
    //  The schema must define at least one class
    //  Each class must have an identity property
    //  A coordinate system must be defined
    //
    Ptr<MgFeatureSchema> schema = m_params->GetFeatureSchema();
    if(schema == NULL)
        throw new MgInvalidArgumentException(L"MgServerCreateFileFeatureSource.CreateFeatureSource", __LINE__, __WFILE__, NULL, L"MgMissingSchema", NULL);

    Ptr<MgClassDefinitionCollection> classes = schema->GetClasses();
    if(classes == NULL || classes->GetCount() == 0)
        throw new MgInvalidArgumentException(L"MgServerCreateFileFeatureSource.CreateFeatureSource", __LINE__, __WFILE__, NULL, L"MgMissingClassDef", NULL);

    for(INT32 ci = 0; ci < classes->GetCount(); ci++)
    {
        Ptr<MgClassDefinition> classDef = classes->GetItem(ci);
        Ptr<MgPropertyDefinitionCollection> idProps = classDef->GetIdentityProperties();
        if(idProps == NULL || idProps->GetCount() == 0)
            throw new MgInvalidArgumentException(L"MgServerCreateFileFeatureSource.CreateFeatureSource", __LINE__, __WFILE__, NULL, L"MgClassWOIdentity", NULL);
    }

    // A coordinate system must be defined
    if(m_params->GetCoordinateSystemWkt().empty())
        throw new MgInvalidArgumentException(L"MgServerCreateFileFeatureSource.CreateFeatureSource", __LINE__, __WFILE__, NULL, L"MgMissingSrs", NULL);

    // Connect to provider
    STRING connString = GetFirstConnectionString();
    STRING providerName = m_params->GetProviderName();
    Ptr<MgServerFeatureConnection> msfc = new MgServerFeatureConnection(providerName, connString);
    {
        if(NULL == msfc.p)
        {
            throw new MgConnectionFailedException(L"MgServerCreateFileFeatureSource.CreateFeatureSource", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        // The reference to the FDO connection from the MgServerFeatureConnection object must be cleaned up before the parent object
        // otherwise it leaves the FDO connection marked as still in use.
        FdoPtr<FdoIConnection> conn =  msfc->GetConnection();
        if (conn == NULL)
        {
            throw new MgConnectionFailedException(L"MgServerCreateFileFeatureSource.CreateFeatureSource", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        CreateDataStore(conn);
        ApplySchemaAndCreateSpatialContext(conn);

        // In general, the Close() should not be called directly on the FdoConnection
        // because connections should be pooled by the FdoConnectionManager.
        // In this case, the MgServerFeatureConnection object was created with an
        // empty connection string, which leads to the connection to be unmanaged by the pool.
        // Thus, it is ok to call Close() here.
        conn->Close();

        Ptr<MgResourceService> resourceService = GetResourceService();
        if (NULL != (MgResourceService*)resourceService)
        {
            SetFeatureSourceDefinition(resourceService);
            SetResourceData(resourceService);
        }
    }
}

void MgServerCreateFileFeatureSource::CreateDataStore(
    FdoIConnection* conn)
{
    m_tempFileName = MgFileUtil::GenerateTempFileName(true, STRING(L"tmp"), m_fileExtension); // NOXLATE

    // Create the datastore
    FdoPtr<FdoICreateDataStore> createDsCmd = static_cast<FdoICreateDataStore*>(conn->CreateCommand(FdoCommandType_CreateDataStore));
    FdoPtr<FdoIDataStorePropertyDictionary> dsProp = createDsCmd->GetDataStoreProperties();
    dsProp->SetProperty (m_connectParamName.c_str(), m_tempFileName.c_str());
    createDsCmd->Execute();
}

void MgServerCreateFileFeatureSource::ApplySchemaAndCreateSpatialContextInternal(
    FdoIConnection* conn)
{
    // Create the spatialcontext
    FdoPtr<FdoICreateSpatialContext> spatialContext = (FdoICreateSpatialContext*)conn->CreateCommand(FdoCommandType_CreateSpatialContext);
    spatialContext->SetCoordinateSystemWkt(m_params->GetCoordinateSystemWkt().c_str());
    spatialContext->SetDescription(m_params->GetSpatialContextDescription().c_str());
    spatialContext->SetName(m_params->GetSpatialContextName().c_str());
    spatialContext->SetXYTolerance(m_params->GetXYTolerance());
    spatialContext->SetZTolerance(m_params->GetZTolerance());
    spatialContext->Execute();

    // Create and set the schema
    Ptr<MgFeatureSchema> featureSchema = m_params->GetFeatureSchema();
    FdoPtr<FdoFeatureSchema> fdoSchema = MgServerFeatureUtil::GetFdoFeatureSchema(featureSchema);
    FdoPtr<FdoIApplySchema> applyschema = (FdoIApplySchema*)conn->CreateCommand(FdoCommandType_ApplySchema);
    applyschema->SetFeatureSchema(fdoSchema);
    applyschema->Execute();
}

void MgServerCreateFileFeatureSource::ApplySchemaAndCreateSpatialContext(
    FdoIConnection* conn)
{
    // Open the connection to the newly created file
    STRING connstr = GetSecondConnectionString();
    conn->SetConnectionString(connstr.c_str());
    conn->Open();

    ApplySchemaAndCreateSpatialContextInternal(conn);
}

STRING MgServerCreateFileFeatureSource::GetFileName() const
{
    STRING fileName = m_params->GetFileName();
    if (fileName.empty())
        fileName = m_resource->GetName() + L"." + m_fileExtension;

    return fileName;
}


void MgServerCreateFileFeatureSource::SetFeatureSourceDefinition(
    MgResourceService* resourceService)
{
    STRING fileName = GetFileName();

    STRING featureSource = L"<FeatureSource xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"FeatureSource-1.0.0.xsd\" version=\"1.0.0\">\n"; // NOXLATE
    featureSource += L"  <Provider>" + m_params->GetProviderName() + L"</Provider>\n"; // NOXLATE
    featureSource += GetFeatureSourceParameterString();
    featureSource += L"</FeatureSource>"; // NOXLATE

    string utf8Text = MgUtil::WideCharToMultiByte(featureSource);

    Ptr<MgByteSource> xmlSource = new MgByteSource((BYTE_ARRAY_IN) utf8Text.c_str(), (INT32)utf8Text.length());
    Ptr<MgByteReader> xmlReader = xmlSource->GetReader();
    resourceService->SetResource(m_resource, xmlReader, NULL);
}

void MgServerCreateFileFeatureSource::SetResourceDataInternal(
    MgResourceService* resourceService,
    STRING source,
    STRING target)
{
    Ptr<MgByteSource> byteSource = new MgByteSource(source, true);
    Ptr<MgByteReader> reader = byteSource->GetReader();
    resourceService->SetResourceData(m_resource, target, MgResourceDataType::File, reader);
}

void MgServerCreateFileFeatureSource::SetResourceData(
    MgResourceService* resourceService)
{
    STRING target = GetFileName();
    SetResourceDataInternal(resourceService, m_tempFileName, target);
    MgFileUtil::DeleteFile(m_tempFileName);
}

MgResourceService* MgServerCreateFileFeatureSource::GetResourceService()
{
    MgServiceManager* serviceMan = MgServiceManager::GetInstance();
    if (NULL != serviceMan)
    {
        MgResourceService* resourceService = dynamic_cast<MgResourceService*>(
        serviceMan->RequestService(MgServiceType::ResourceService));
        return resourceService;
    }
    return NULL;
}

STRING MgServerCreateFileFeatureSource::GetFirstConnectionString()
{
    return L"";
}

STRING MgServerCreateFileFeatureSource::GetSecondConnectionString()
{
    STRING connstr = m_connectParamName + L"=" + m_tempFileName;  // NOXLATE
    return connstr;
}

STRING MgServerCreateFileFeatureSource::GetFeatureSourceParameterString() const
{
    STRING fileName = GetFileName();
    STRING featureSource = L"  <Parameter>\n    <Name>" + m_connectParamName + L"</Name>\n"; // NOXLATE
    featureSource += L"     <Value>%MG_DATA_FILE_PATH%" + fileName + L"</Value>\n"; // NOXLATE
    featureSource += L"  </Parameter>\n"; // NOXLATE
    return featureSource;
}

void MgServerCreateFileFeatureSource::Dispose()
{
    delete this;
}

MgServerCreateSdfFeatureSource::MgServerCreateSdfFeatureSource(
    MgResourceIdentifier* resource, 
    MgFileFeatureSourceParams* params):
    MgServerCreateFileFeatureSource(resource, params)
{
    m_fileExtension = L"sdf";  // NOXLATE
    m_connectParamName = L"File";  // NOXLATE
}

MgServerCreateSdfFeatureSource::~MgServerCreateSdfFeatureSource()
{
}

STRING MgServerCreateSdfFeatureSource::GetFeatureSourceParameterString() const
{
    STRING fileName = GetFileName();
    STRING featureSource = L"  <Parameter>\n    <Name>" + m_connectParamName + L"</Name>\n"; // NOXLATE
    featureSource += L"     <Value>%MG_DATA_FILE_PATH%" + fileName + L"</Value>\n"; // NOXLATE
    featureSource += L"  </Parameter>\n\
  <Parameter>\n\
    <Name>ReadOnly</Name>\n\
    <Value>FALSE</Value>\n\
  </Parameter>\n"; // NOXLATE
    return featureSource;
}

MgServerCreateShpFeatureSource::MgServerCreateShpFeatureSource(
    MgResourceIdentifier* resource, 
    MgFileFeatureSourceParams* params):
    MgServerCreateFileFeatureSource(resource, params)
{
    m_fileExtension = L"shp";  // NOXLATE
    m_connectParamName = L"DefaultFileLocation";  // NOXLATE
}

MgServerCreateShpFeatureSource::~MgServerCreateShpFeatureSource()
{
}

void MgServerCreateShpFeatureSource::CreateDataStore(
    FdoIConnection* conn)
{
    // Do nothing because SHP files are created by FdoIApplySchema command.
}

void MgServerCreateShpFeatureSource::ApplySchemaAndCreateSpatialContext(
    FdoIConnection* conn)
{
    ApplySchemaAndCreateSpatialContextInternal(conn);
}

void MgServerCreateShpFeatureSource::SetResourceData(
    MgResourceService* resourceService)
{
    Ptr<MgStringCollection> sourceFiles = new MgStringCollection();
    Ptr<MgStringCollection> targetFiles = new MgStringCollection();
    MgFileUtil::GetFilesInDirectory(sourceFiles, m_tempFileName, false, false);
    MgFileUtil::GetFilesInDirectory(targetFiles, m_tempFileName, false, true);

    int i = 0;
    for ( i = 0; i < sourceFiles->GetCount(); ++i)
    {
        SetResourceDataInternal(resourceService, sourceFiles->GetItem(i), targetFiles->GetItem(i));
    }
    MgFileUtil::DeleteDirectory(m_tempFileName);
}

STRING MgServerCreateShpFeatureSource::GetFirstConnectionString()
{
    // For SHP feature source, m_tempFileName saves a temporary path 
    // instead of a temporary file name.
    m_tempFileName = MgFileUtil::GenerateTempPath();
    STRING connstr = m_connectParamName + L"=" + m_tempFileName;  // NOXLATE
    return connstr;
}

STRING MgServerCreateShpFeatureSource::GetSecondConnectionString()
{
    if (m_tempFileName.empty())
        m_tempFileName = MgFileUtil::GenerateTempPath();
    STRING connstr = m_connectParamName + L"=" + m_tempFileName;  // NOXLATE
    return connstr;
}

STRING MgServerCreateShpFeatureSource::GetFeatureSourceParameterString() const
{
    STRING featureSource = L"  <Parameter>\n    <Name>" + m_connectParamName + L"</Name>\n"; // NOXLATE
    featureSource += L"     <Value>%MG_DATA_FILE_PATH%</Value>\n"; // NOXLATE
    featureSource += L"  </Parameter>\n"; // NOXLATE
    return featureSource;
}

MgServerCreateSqliteFeatureSource::MgServerCreateSqliteFeatureSource(
    MgResourceIdentifier* resource, 
    MgFileFeatureSourceParams* params):
    MgServerCreateFileFeatureSource(resource, params)
{
    m_fileExtension = L"sqlite";  // NOXLATE
    m_connectParamName = L"File";  // NOXLATE
}

MgServerCreateSqliteFeatureSource::~MgServerCreateSqliteFeatureSource()
{
}

STRING MgServerCreateSqliteFeatureSource::GetSecondConnectionString()
{
    STRING connstr = m_connectParamName + L"=" + m_tempFileName + L";UseFdoMetadata=true";  // NOXLATE
    return connstr;
}

STRING MgServerCreateSqliteFeatureSource::GetFeatureSourceParameterString() const
{
    STRING fileName = GetFileName();
    STRING featureSource = L"  <Parameter>\n"; // NOXLATE
    featureSource += L"     <Name>" + m_connectParamName + L"</Name>\n"; // NOXLATE
    featureSource += L"     <Value>%MG_DATA_FILE_PATH%" + fileName + L"</Value>\n"; // NOXLATE
    featureSource += L"  </Parameter>\n"; // NOXLATE
    featureSource += L"  <Parameter>\n";  // NOXLATE
    featureSource += L"     <Name>UseFdoMetadata</Name>\n"; // NOXLATE
    featureSource += L"     <Value>true</Value>\n"; // NOXLATE
    featureSource += L"  </Parameter>\n"; // NOXLATE
    return featureSource;
}

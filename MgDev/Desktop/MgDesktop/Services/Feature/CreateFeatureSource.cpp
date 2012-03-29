#include "MgDesktop.h"
#include "CreateFeatureSource.h"
#include "FdoConnectionUtil.h"
#include "FeatureUtil.h"

MgCreateFeatureSource::MgCreateFeatureSource()
{
}

MgCreateFeatureSource::~MgCreateFeatureSource()
{
}

void MgCreateFeatureSource::CreateFeatureSource(MgResourceIdentifier* resource, MgFeatureSourceParams* sourceParams)
{
    MG_FEATURE_SERVICE_TRY()

    MgFileFeatureSourceParams* params = dynamic_cast<MgFileFeatureSourceParams*>(sourceParams);
    MgRdbmsFeatureSourceParams* rdbParams = dynamic_cast<MgRdbmsFeatureSourceParams*>(sourceParams);
    if (NULL != params)
    {
        STRING providerName = params->GetProviderName();
        Ptr<MgCreateFileFeatureSource> creator = NULL;    
        if (providerName == L"OSGeo.SDF") // NOXLATE
        {
            creator = new MgCreateSdfFeatureSource(resource, params);
            creator->CreateFeatureSource(false, false);
        }
        else if (providerName == L"OSGeo.SHP") // NOXLATE
        {
            creator = new MgCreateShpFeatureSource(resource, params);
            creator->CreateFeatureSource(true, false);
        }
        else if (providerName == L"OSGeo.SQLite") // NOXLATE
        {
            creator = new MgCreateSqliteFeatureSource(resource, params);
            creator->CreateFeatureSource(false, false);
        }
        else
            throw new MgInvalidArgumentException(L"MgCreateFeatureSource.CreateFeatureSource", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    else if (NULL != rdbParams)
    {
        STRING providerName = rdbParams->GetProviderName();
        Ptr<MgCreateRdbmsFeatureSource> creator = NULL;
        if (providerName == L"OSGeo.MySQL" ||
            providerName == L"OSGeo.SQLServerSpatial" ||
            providerName == L"OSGeo.PostgreSQL")
        {
            creator = new MgCreateRdbmsFeatureSource(resource, rdbParams);
            creator->CreateFeatureSource();
        }
        else
            throw new MgInvalidArgumentException(L"MgCreateFeatureSource.CreateFeatureSource", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH_AND_THROW(resource, L"MgCreateFeatureSource.CreateFeatureSource")
}

MgCreateFileFeatureSource::MgCreateFileFeatureSource(
    MgResourceIdentifier* resource,
    MgFileFeatureSourceParams* params)
{
    m_resource = resource;
    SAFE_ADDREF(resource);

    m_params = params;
    SAFE_ADDREF(params);
}

MgCreateFileFeatureSource::~MgCreateFileFeatureSource()
{
    SAFE_RELEASE(m_resource);
    SAFE_RELEASE(m_params);
}

void MgCreateFileFeatureSource::CreateFeatureSource(bool bCheckFeatureClass, bool bCheckSpatialContext)
{
    MG_FEATURE_SERVICE_TRY()

    //Some basic schema validation:
    //  A schema must be supplied
    //  The schema must define at least one class
    //  Each class must have an identity property
    //  A coordinate system must be defined
    //
    Ptr<MgFeatureSchema> schema = m_params->GetFeatureSchema();
    if(schema == NULL)
        throw new MgInvalidArgumentException(L"MgCreateFileFeatureSource.CreateFeatureSource", __LINE__, __WFILE__, NULL, L"MgMissingSchema", NULL);

    if (bCheckFeatureClass)
    {
        Ptr<MgClassDefinitionCollection> classes = schema->GetClasses();
        if(classes == NULL || classes->GetCount() == 0)
            throw new MgInvalidArgumentException(L"MgCreateFileFeatureSource.CreateFeatureSource", __LINE__, __WFILE__, NULL, L"MgMissingClassDef", NULL);

        for(INT32 ci = 0; ci < classes->GetCount(); ci++)
        {
            Ptr<MgClassDefinition> classDef = classes->GetItem(ci);
            Ptr<MgPropertyDefinitionCollection> idProps = classDef->GetIdentityProperties();
            if(idProps == NULL || idProps->GetCount() == 0)
                throw new MgInvalidArgumentException(L"MgCreateFileFeatureSource.CreateFeatureSource", __LINE__, __WFILE__, NULL, L"MgClassWOIdentity", NULL);
        }
    }

    if (bCheckSpatialContext)
    {
        // A coordinate system must be defined
        if(m_params->GetCoordinateSystemWkt().empty())
            throw new MgInvalidArgumentException(L"MgCreateFileFeatureSource.CreateFeatureSource", __LINE__, __WFILE__, NULL, L"MgMissingSrs", NULL);
    }

    // Connect to provider
    STRING connString = GetFirstConnectionString();
    STRING providerName = m_params->GetProviderName();
    
    
    Ptr<MgFeatureConnection> connWrap = new MgFeatureConnection(providerName, connString);
    {
        if(NULL == connWrap.p)
        {
            throw new MgConnectionFailedException(L"MgCreateFileFeatureSource.CreateFeatureSource", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        FdoPtr<FdoIConnection> conn = connWrap->GetConnection();
        if (conn == NULL)
        {
            throw new MgConnectionFailedException(L"MgCreateFileFeatureSource.CreateFeatureSource", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        CreateDataStore(conn);
        ApplySchemaAndCreateSpatialContext(conn);

        conn->Close();

        Ptr<MgResourceService> resourceService = GetResourceService();
        if (NULL != (MgResourceService*)resourceService)
        {
            SetFeatureSourceDefinition(resourceService);
            SetResourceData(resourceService);
        }
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgCreateFileFeatureSource::CreateFeatureSource")
}

void MgCreateFileFeatureSource::CreateDataStore(
    FdoIConnection* conn)
{
    MG_FEATURE_SERVICE_TRY()
    m_tempFileName = MgFileUtil::GenerateTempFileName(true, STRING(L"tmp"), m_fileExtension); // NOXLATE

    // Create the datastore
    FdoPtr<FdoICreateDataStore> createDsCmd = static_cast<FdoICreateDataStore*>(conn->CreateCommand(FdoCommandType_CreateDataStore));
    FdoPtr<FdoIDataStorePropertyDictionary> dsProp = createDsCmd->GetDataStoreProperties();
    dsProp->SetProperty (m_connectParamName.c_str(), m_tempFileName.c_str());
    createDsCmd->Execute();
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgCreateFileFeatureSource::CreateDataStore")
}

void MgCreateFileFeatureSource::ApplySchemaAndCreateSpatialContextInternal(FdoIConnection* conn)
{
    MG_FEATURE_SERVICE_TRY()
    // Create the spatialcontext
    if (!m_params->GetCoordinateSystemWkt().empty())
    {
        FdoPtr<FdoICreateSpatialContext> spatialContext = (FdoICreateSpatialContext*)conn->CreateCommand(FdoCommandType_CreateSpatialContext);
        spatialContext->SetCoordinateSystemWkt(m_params->GetCoordinateSystemWkt().c_str());
        spatialContext->SetDescription(m_params->GetSpatialContextDescription().c_str());
        spatialContext->SetName(m_params->GetSpatialContextName().c_str());
        spatialContext->SetXYTolerance(m_params->GetXYTolerance());
        spatialContext->SetZTolerance(m_params->GetZTolerance());
        spatialContext->Execute();
    }

    // Create and set the schema
    Ptr<MgFeatureSchema> featureSchema = m_params->GetFeatureSchema();
    FdoPtr<FdoFeatureSchema> fdoSchema = MgFeatureUtil::GetFdoFeatureSchema(featureSchema);
    FdoPtr<FdoIApplySchema> applyschema = (FdoIApplySchema*)conn->CreateCommand(FdoCommandType_ApplySchema);
    applyschema->SetFeatureSchema(fdoSchema);
    applyschema->Execute();
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgCreateFileFeatureSource::ApplySchemaAndCreateSpatialContextInternal")
}

void MgCreateFileFeatureSource::ApplySchemaAndCreateSpatialContext(FdoIConnection* conn)
{
    MG_FEATURE_SERVICE_TRY()
    // Open the connection to the newly created file
    STRING connstr = GetSecondConnectionString();
    conn->SetConnectionString(connstr.c_str());
    conn->Open();

    ApplySchemaAndCreateSpatialContextInternal(conn);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgCreateFileFeatureSource::ApplySchemaAndCreateSpatialContext")
}

STRING MgCreateFileFeatureSource::GetFileName() const
{
    STRING fileName = m_params->GetFileName();
    if (fileName.empty())
        fileName = m_resource->GetName() + L"." + m_fileExtension;

    return fileName;
}


void MgCreateFileFeatureSource::SetFeatureSourceDefinition(MgResourceService* resourceService)
{
    MG_FEATURE_SERVICE_TRY()
    STRING fileName = GetFileName();

    STRING featureSource = L"<FeatureSource xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"FeatureSource-1.0.0.xsd\" version=\"1.0.0\">\n"; // NOXLATE
    featureSource += L"  <Provider>" + m_params->GetProviderName() + L"</Provider>\n"; // NOXLATE
    featureSource += GetFeatureSourceParameterString();
    featureSource += L"</FeatureSource>"; // NOXLATE

    string utf8Text = MgUtil::WideCharToMultiByte(featureSource);

    Ptr<MgByteSource> xmlSource = new MgByteSource((BYTE_ARRAY_IN) utf8Text.c_str(), (INT32)utf8Text.length());
    Ptr<MgByteReader> xmlReader = xmlSource->GetReader();
    resourceService->SetResource(m_resource, xmlReader, NULL);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgCreateFileFeatureSource::SetFeatureSourceDefinition")
}

void MgCreateFileFeatureSource::SetResourceDataInternal(MgResourceService* resourceService,
                                                        STRING source,
                                                        STRING target)
{
    MG_FEATURE_SERVICE_TRY()
    Ptr<MgByteSource> byteSource = new MgByteSource(source, true);
    Ptr<MgByteReader> reader = byteSource->GetReader();
    resourceService->SetResourceData(m_resource, target, MgResourceDataType::File, reader);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgCreateFileFeatureSource::SetResourceDataInternal")
}

void MgCreateFileFeatureSource::SetResourceData(MgResourceService* resourceService)
{
    MG_FEATURE_SERVICE_TRY()
    STRING target = GetFileName();
    SetResourceDataInternal(resourceService, m_tempFileName, target);
    MgFileUtil::DeleteFile(m_tempFileName);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgCreateFileFeatureSource::SetResourceData")
}

MgResourceService* MgCreateFileFeatureSource::GetResourceService()
{
    Ptr<MgServiceFactory> fact = new MgServiceFactory();
    return static_cast<MgResourceService*>(fact->CreateService(MgServiceType::ResourceService));
}

STRING MgCreateFileFeatureSource::GetFirstConnectionString()
{
    return L"";
}

STRING MgCreateFileFeatureSource::GetSecondConnectionString()
{
    STRING connstr = m_connectParamName + L"=" + m_tempFileName;  // NOXLATE
    return connstr;
}

STRING MgCreateFileFeatureSource::GetFeatureSourceParameterString() const
{
    STRING fileName = GetFileName();
    STRING featureSource = L"  <Parameter>\n    <Name>" + m_connectParamName + L"</Name>\n"; // NOXLATE
    featureSource += L"     <Value>%MG_DATA_FILE_PATH%" + fileName + L"</Value>\n"; // NOXLATE
    featureSource += L"  </Parameter>\n"; // NOXLATE
    return featureSource;
}

void MgCreateFileFeatureSource::Dispose()
{
    delete this;
}

MgCreateSdfFeatureSource::MgCreateSdfFeatureSource(
    MgResourceIdentifier* resource,
    MgFileFeatureSourceParams* params):
    MgCreateFileFeatureSource(resource, params)
{
    m_fileExtension = L"sdf";  // NOXLATE
    m_connectParamName = L"File";  // NOXLATE
}

MgCreateSdfFeatureSource::~MgCreateSdfFeatureSource()
{
}

STRING MgCreateSdfFeatureSource::GetFeatureSourceParameterString() const
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

MgCreateShpFeatureSource::MgCreateShpFeatureSource(
    MgResourceIdentifier* resource,
    MgFileFeatureSourceParams* params):
    MgCreateFileFeatureSource(resource, params)
{
    m_fileExtension = L"shp";  // NOXLATE
    m_connectParamName = L"DefaultFileLocation";  // NOXLATE
}

MgCreateShpFeatureSource::~MgCreateShpFeatureSource()
{
}

void MgCreateShpFeatureSource::CreateDataStore(
    FdoIConnection* conn)
{
    // Do nothing because SHP files are created by FdoIApplySchema command.
}


void MgCreateShpFeatureSource::ApplySchemaAndCreateSpatialContext(FdoIConnection* conn)
{
    MG_FEATURE_SERVICE_TRY()
    ApplySchemaAndCreateSpatialContextInternal(conn);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgCreateShpFeatureSource::ApplySchemaAndCreateSpatialContext")
}

void MgCreateShpFeatureSource::SetResourceData(MgResourceService* resourceService)
{
    MG_FEATURE_SERVICE_TRY()
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
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgCreateShpFeatureSource::SetResourceData")
}

STRING MgCreateShpFeatureSource::GetFirstConnectionString()
{
    // For SHP feature source, m_tempFileName saves a temporary path
    // instead of a temporary file name.
    m_tempFileName = MgFileUtil::GenerateTempPath();
    STRING connstr = m_connectParamName + L"=" + m_tempFileName;  // NOXLATE
    return connstr;
}

STRING MgCreateShpFeatureSource::GetSecondConnectionString()
{
    if (m_tempFileName.empty())
        m_tempFileName = MgFileUtil::GenerateTempPath();
    STRING connstr = m_connectParamName + L"=" + m_tempFileName;  // NOXLATE
    return connstr;
}

STRING MgCreateShpFeatureSource::GetFeatureSourceParameterString() const
{
    STRING featureSource = L"  <Parameter>\n    <Name>" + m_connectParamName + L"</Name>\n"; // NOXLATE
    featureSource += L"     <Value>%MG_DATA_FILE_PATH%</Value>\n"; // NOXLATE
    featureSource += L"  </Parameter>\n"; // NOXLATE
    return featureSource;
}

MgCreateSqliteFeatureSource::MgCreateSqliteFeatureSource(
    MgResourceIdentifier* resource,
    MgFileFeatureSourceParams* params):
    MgCreateFileFeatureSource(resource, params)
{
    m_fileExtension = L"sqlite";  // NOXLATE
    m_connectParamName = L"File";  // NOXLATE
}

MgCreateSqliteFeatureSource::~MgCreateSqliteFeatureSource()
{
}

STRING MgCreateSqliteFeatureSource::GetSecondConnectionString()
{
    STRING connstr = m_connectParamName + L"=" + m_tempFileName + L";UseFdoMetadata=true";  // NOXLATE
    return connstr;
}

STRING MgCreateSqliteFeatureSource::GetFeatureSourceParameterString() const
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

MgCreateRdbmsFeatureSource::MgCreateRdbmsFeatureSource(MgResourceIdentifier* resource, MgRdbmsFeatureSourceParams* params)
{
    m_resource = resource;
    SAFE_ADDREF(resource);

    m_params = params;
    SAFE_ADDREF(params);
}

MgCreateRdbmsFeatureSource::~MgCreateRdbmsFeatureSource()
{
    SAFE_RELEASE(m_resource);
    SAFE_RELEASE(m_params);
}

void MgCreateRdbmsFeatureSource::CreateFeatureSource() 
{
    MG_FEATURE_SERVICE_TRY()
    STRING provider = m_params->GetProviderName();
    STRING service = m_params->GetService();
    STRING username = m_params->GetUsername();
    STRING password = m_params->GetPassword();
    STRING dataStore = m_params->GetDataStore();
    Ptr<MgFeatureSchema> mgSchema = m_params->GetFeatureSchema();

    STRING connStr = L"";
    connStr += L"Service=" + service;
    if (!username.empty())
        connStr += L";Username=" + username;
    if (!password.empty())
        connStr += L";Password=" + password;

    Ptr<MgFeatureConnection> conn = new MgFeatureConnection(provider, connStr);
    {
        FdoPtr<FdoIConnection> fdoConn = conn->GetConnection();

        //Create the data store
        FdoPtr<FdoICreateDataStore> create = (FdoICreateDataStore*)fdoConn->CreateCommand(FdoCommandType_CreateDataStore);
        FdoPtr<FdoIDataStorePropertyDictionary> dataProps = create->GetDataStoreProperties();

        dataProps->SetProperty(L"DataStore", dataStore.c_str());
        dataProps->SetProperty(L"IsFdoEnabled", (m_params->GetIsFdoEnabled() ? L"true" : L"false")); 

        create->Execute();
        fdoConn = NULL;
        conn = NULL;

        connStr += L";DataStore=" + dataStore;
        //Amend connection string to incorporate the created data store
        conn = new MgFeatureConnection(provider, connStr);
        if (!conn->IsConnectionOpen())
            throw new MgConnectionFailedException(L"MgCreateRdbmsFeatureSource::CreateFeatureSource", __LINE__, __WFILE__, NULL, L"", NULL);

        fdoConn = conn->GetConnection();

        //Create spatial context
        FdoPtr<FdoFgfGeometryFactory> geomFactory = FdoFgfGeometryFactory::GetInstance();
        FdoPtr<FdoIConnectionCapabilities> connCaps = fdoConn->GetConnectionCapabilities();
        FdoPtr<FdoICreateSpatialContext> createSc = (FdoICreateSpatialContext*)fdoConn->CreateCommand(FdoCommandType_CreateSpatialContext);
        
        //Extents is required for RDBMS data stores
        Ptr<MgEnvelope> extents = m_params->GetSpatialContextExtents();
        CHECKNULL((MgEnvelope*)extents, L"MgCreateRdbmsFeatureSource::CreateFeatureSource");

        Ptr<MgCoordinate> extentsLL = extents->GetLowerLeftCoordinate();
        Ptr<MgCoordinate> extentsUR = extents->GetUpperRightCoordinate();

        createSc->SetName(m_params->GetSpatialContextName().c_str());
        createSc->SetDescription(m_params->GetSpatialContextDescription().c_str());
        createSc->SetCoordinateSystem(m_params->GetCoordinateSystemName().c_str());

        //This is true for SQL Server
        if (!connCaps->SupportsCSysWKTFromCSysName())
            createSc->SetCoordinateSystemWkt(m_params->GetCoordinateSystemWkt().c_str());

        FdoPtr<FdoIEnvelope> env = geomFactory->CreateEnvelopeXY(
            extentsLL->GetX(),
            extentsLL->GetY(),
            extentsUR->GetX(),
            extentsUR->GetY());
            
        FdoPtr<FdoIGeometry> geom = geomFactory->CreateGeometry(env);
        FdoPtr<FdoByteArray> fgf = geomFactory->GetFgf(geom);
        createSc->SetExtent(fgf);
        createSc->SetExtentType(FdoSpatialContextExtentType_Static);
        createSc->Execute();

        //Now apply the specified schema
        FdoPtr<FdoIApplySchema> apply = (FdoIApplySchema*)fdoConn->CreateCommand(FdoCommandType_ApplySchema);

        FdoPtr<FdoFeatureSchema> fdoSchema = FdoFeatureSchema::Create(
            mgSchema->GetName().c_str(),
            mgSchema->GetDescription().c_str());

        //TODO: Should we be smart here or put the onus on the invoker to
        //ensure that geometry properties of the schema we are going to apply
        //are correctly pointing to the name of the spatial context we just 
        //created?

        MgFeatureUtil::UpdateFdoFeatureSchema(mgSchema, fdoSchema);
        apply->SetFeatureSchema(fdoSchema);
        apply->Execute();

        WriteFeatureSourceContent();
    }
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgCreateRdbmsFeatureSource::CreateFeatureSource")
}

void MgCreateRdbmsFeatureSource::WriteFeatureSourceContent()
{
    MG_FEATURE_SERVICE_TRY()
    Ptr<MgServiceFactory> fact = new MgServiceFactory();
    Ptr<MgdResourceService> resSvc = static_cast<MgdResourceService*>(fact->CreateService(MgServiceType::ResourceService));

    STRING xml = L"<FeatureSource xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"FeatureSource-1.0.0.xsd\" version=\"1.0.0\">\n"; // NOXLATE
    xml += L"  <Provider>" + m_params->GetProviderName() + L"</Provider>\n"; // NOXLATE
    xml += L"  <Parameter>\n    <Name>Service</Name>\n    <Value>" + m_params->GetService() + L"</Value>\n  </Parameter>\n"; // NOXLATE
    xml += L"  <Parameter>\n    <Name>Username</Name>\n    <Value>" + m_params->GetUsername() + L"</Value>\n  </Parameter>\n"; // NOXLATE
    xml += L"  <Parameter>\n    <Name>Password</Name>\n    <Value>" + m_params->GetPassword() + L"</Value>\n  </Parameter>\n"; // NOXLATE
    xml += L"  <Parameter>\n    <Name>DataStore</Name>\n    <Value>" + m_params->GetDataStore() + L"</Value>\n  </Parameter>\n"; // NOXLATE
    xml += L"</FeatureSource>"; // NOXLATE

    string utf8Text = MgUtil::WideCharToMultiByte(xml);

    Ptr<MgByteSource> xmlSource = new MgByteSource((BYTE_ARRAY_IN) utf8Text.c_str(), (INT32)utf8Text.length());
    Ptr<MgByteReader> xmlReader = xmlSource->GetReader();
    resSvc->SetResource(m_resource, xmlReader, NULL);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgCreateRdbmsFeatureSource::WriteFeatureSourceContent")
}

void MgCreateRdbmsFeatureSource::Dispose() { delete this; }
#include "MgDesktop.h"
#include "FdoConnectionUtil.h"
#include "Fdo.h"
#include "FeatureServiceCache.h"
#include "Services/Resource/UnmanagedDataManager.h"
#include "CryptographyUtil.h"

INT64 MgdFdoConnectionUtil::sm_nConnectionsCreated = 0L;
INT64 MgdFdoConnectionUtil::sm_nConnectionsClosed = 0L;

FdoIConnection* MgdFdoConnectionUtil::CreateConnection(CREFSTRING providerName, CREFSTRING connectionString)
{
    sm_nConnectionsCreated++;

    FdoPtr<FdoIConnection> conn;

    MG_FEATURE_SERVICE_TRY()

    MgdLogDetail logDetail(MgServiceType::FeatureService, MgdLogDetail::InternalTrace, L"MgdFdoConnectionUtil::CreateConnection", mgStackParams);
    logDetail.AddString(L"providerName", providerName);
    logDetail.AddString(L"connectionString", connectionString);
    logDetail.Create();

    Ptr<MgdServiceFactory> fact = new MgdServiceFactory();
    Ptr<MgdResourceService> resSvc = static_cast<MgdResourceService*>(fact->CreateService(MgServiceType::ResourceService));
    FdoPtr<FdoProviderNameTokens> tokens = FdoProviderNameTokens::Create(providerName.c_str());
    FdoStringsP tokenValues = tokens->GetNameTokens();
    FdoPtr<FdoStringElement> company = tokenValues->GetItem(0);
    FdoPtr<FdoStringElement> local = tokenValues->GetItem(1);

    STRING providerNoVersion = (FdoString*)company->GetString();

    providerNoVersion += L".";
    providerNoVersion += (FdoString*)local->GetString();

	FdoPtr<IConnectionManager> connMgr = FdoFeatureAccessManager::GetConnectionManager();
	conn = connMgr->CreateConnection(providerNoVersion.c_str());

    //Some providers may be sensitive to being assigned an empty string
    if (!connectionString.empty())
    {
	    conn->SetConnectionString(connectionString.c_str());
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFdoConnectionUtil::CreateConnection")

	return conn.Detach();
}

FdoIConnection* MgdFdoConnectionUtil::CreateConnection(MgResourceIdentifier* resource)
{
    sm_nConnectionsCreated++;

    FdoPtr<FdoIConnection> conn;

	MG_FEATURE_SERVICE_TRY()

    CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdFdoConnectionUtil::CreateConnection");
    Ptr<MgdServiceFactory> fact = new MgdServiceFactory();
    Ptr<MgdResourceService> resSvc = static_cast<MgdResourceService*>(fact->CreateService(MgServiceType::ResourceService));

	std::string xmlContent;
	Ptr<MgByteReader> content = resSvc->GetResourceContent(resource, L"");

	content->ToStringUtf8(xmlContent);

    MdfModel::FeatureSource* fs = GetFeatureSource(resource);
	STRING provider = (STRING)fs->GetProvider();
    STRING configDoc = (STRING)fs->GetConfigurationDocument();

	FdoPtr<IConnectionManager> connMgr = FdoFeatureAccessManager::GetConnectionManager();
    
    FdoPtr<FdoProviderNameTokens> tokens = FdoProviderNameTokens::Create(provider.c_str());
    FdoStringsP tokenValues = tokens->GetNameTokens();
    FdoPtr<FdoStringElement> company = tokenValues->GetItem(0);
    FdoPtr<FdoStringElement> local = tokenValues->GetItem(1);

    STRING providerNoVersion = (FdoString*)company->GetString();

    providerNoVersion += L".";
    providerNoVersion += (FdoString*)local->GetString();

	conn = connMgr->CreateConnection(providerNoVersion.c_str());
	FdoPtr<FdoIConnectionInfo> connInfo = conn->GetConnectionInfo();
	FdoPtr<FdoIConnectionPropertyDictionary> dict = connInfo->GetConnectionProperties();

    STRING username;
    STRING password;

    Ptr<MgByteReader> resData = resSvc->EnumerateResourceData(resource);
    Ptr<MgByteSink> sink = new MgByteSink(resData);
    STRING resDataXml;
    sink->ToString(resDataXml);

    if (resDataXml.find(MgResourceDataName::UserCredentials) != STRING::npos)
    {
        Ptr<MgByteReader> blob = resSvc->GetRawCredentials(resource);
        if (NULL != blob.p)
        {
            Ptr<MgByteSink> credSink = new MgByteSink(blob);
            std::string credentials;
            credSink->ToStringUtf8(credentials);

            MgCryptographyUtil crypto;
            std::string mbUsername;
            std::string mbPassword;
            crypto.DecryptCredentials(credentials, mbUsername, mbPassword);

            username = MgUtil::MultiByteToWideChar(mbUsername);
            password = MgUtil::MultiByteToWideChar(mbPassword);
        }
    }

	MdfModel::NameStringPairCollection* params = fs->GetParameters();
	for (INT32 i = 0; i < params->GetCount(); i++)
	{
		MdfModel::NameStringPair* pair = params->GetAt(i);
        
        STRING n = pair->GetName();
        STRING v = pair->GetValue();

        //Perform tag substitution if found
        PerformTagSubstitution(resSvc, v, resource, username, password);

		dict->SetProperty(n.c_str(), v.c_str());
	}

    MgdLogDetail logDetail(MgServiceType::FeatureService, MgdLogDetail::InternalTrace, L"MgdFdoConnectionUtil::CreateConnection", mgStackParams);
    logDetail.AddResourceIdentifier(L"resource", resource);
    logDetail.Create();

    FdoPtr<FdoIConnectionCapabilities> connCaps = conn->GetConnectionCapabilities();
    if (connCaps->SupportsConfiguration() && !configDoc.empty())
    {
        Ptr<MgByteReader> br = resSvc->GetResourceData(resource, configDoc, MgResourcePreProcessingType::Substitution);
        if (br == NULL)
        {
            STRING message = MgUtil::GetResourceMessage(MgResources::FeatureService, L"MgMissingConfiguration");

            Ptr<MgStringCollection> strCol;
            if (!message.empty())
            {
                strCol = new MgStringCollection();
                strCol->Add(message);
            }
            throw new MgdInvalidFeatureSourceException(L"MgdFdoConnectionUtil::CreateConnection",
                __LINE__, __WFILE__, (MgStringCollection*)strCol, L"", NULL);
        }

        Ptr<MgByte> bytes;
        bytes = NULL;

        MgByteSink byteSink(br);
        bytes = byteSink.ToBuffer();

        if (bytes)
        {
            FdoIoMemoryStreamP stream = FdoIoMemoryStream::Create();
            stream->Write((FdoByte*)bytes->Bytes(), (FdoSize)bytes->GetLength());
            stream->Reset();
            conn->SetConfiguration(stream);
        }
    }

	MG_FEATURE_SERVICE_CATCH_AND_THROW_WITH_FEATURE_SOURCE(L"MgdFdoConnectionUtil::CreateConnection", resource)

	return conn.Detach();
}

void MgdFdoConnectionUtil::PerformTagSubstitution(MgdResourceService* resSvc, REFSTRING str, MgResourceIdentifier* resource, CREFSTRING username, CREFSTRING password)
{
    const int dataTokenPos = str.find(MgResourceTag::DataFilePath);
    const int dataAliasPos = str.find(MgResourceTag::DataPathAliasBegin);
    const int usernamePos = str.find(MgResourceTag::Username);
    const int passwordPos = str.find(MgResourceTag::Password);
    if (dataTokenPos != STRING::npos)
    {
        STRING dataPath = resSvc->ResolveDataPath(resource);
        str.replace(dataTokenPos, MgResourceTag::DataFilePath.size(), dataPath);
    }
    else if (dataAliasPos != STRING::npos)
    {
        MgdUnmanagedDataManager::SubstituteDataPathAliases(str);
    }
    else if (usernamePos != STRING::npos && !username.empty())
    {
        str.replace(usernamePos, MgResourceTag::Username.size(), username);
    }
    else if (passwordPos != STRING::npos && !password.empty())
    {
        str.replace(passwordPos, MgResourceTag::Password.size(), password);
    }
}

STRING MgdFdoConnectionUtil::ParseNonQualifiedProviderName(CREFSTRING providerName)
{
    FdoPtr<FdoProviderNameTokens> tokens = FdoProviderNameTokens::Create(providerName.c_str());
    FdoStringsP tokenParts = tokens->GetNameTokens();

    FdoPtr<FdoStringElement> company = tokenParts->GetItem(0);
    FdoPtr<FdoStringElement> provider = tokenParts->GetItem(1);

    STRING name = company->GetString();
    name += L".";
    name += provider->GetString();

    return name;
}

MdfModel::FeatureSource* MgdFdoConnectionUtil::GetFeatureSource(MgResourceIdentifier* resource)
{
    MgdFeatureServiceCache* cache = MgdFeatureServiceCache::GetInstance();
    Ptr<MgdFeatureSourceCacheItem> fsCache = cache->GetFeatureSource(resource);

    MdfModel::FeatureSource* fs = NULL;

    if (NULL != fsCache)
    {
        fs = fsCache->Get();

        //There is a cached item. Return it.
        if (NULL != fs)
            return fs;
    }

    // Get the Resource Service.
    Ptr<MgdServiceFactory> fact = new MgdServiceFactory();
    Ptr<MgdResourceService> resourceService = static_cast<MgdResourceService*>(fact->CreateService(MgServiceType::ResourceService));
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
            throw new MgdInvalidFeatureSourceException(
                L"MgdFdoConnectionUtil::GetFeatureSource",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
    }

    // Detach the feature source from the parser.
    // It's now the caller's responsibility to delete it.
    fs = parser.DetachFeatureSource();

    if (NULL == fs)
    {
        MgResources* resources = MgResources::GetInstance();
        ACE_ASSERT(NULL != resources);
        STRING message = resources->GetResourceMessage(MgResources::FeatureService,
            L"MgInvalidFdoProvider", NULL);
        MgStringCollection arguments;
        arguments.Add(message);

        throw new MgdInvalidFeatureSourceException(
            L"MgdFdoConnectionUtil::GetFeatureSource",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    //Cache this item for future calls
    if (NULL != fsCache)
    {
        fsCache->Set(fs);
    }
    else
    {
        fsCache = new MgdFeatureSourceCacheItem(fs);
        cache->SetFeatureSource(resource, fsCache);
    }

    return fs;
}

void MgdFdoConnectionUtil::CloseConnection(FdoIConnection* conn)
{
    sm_nConnectionsClosed++;
    try 
    {
        conn->Close();
    }
    catch (FdoException* ex)
    {
        FDO_SAFE_RELEASE(ex);
    }
    catch (...) 
    { 
    
    }
}

#ifdef DEBUG_FDO_CONNECTION_POOL
void MgdFdoConnectionUtil::CheckCallStats()
{
    if (sm_nConnectionsCreated > sm_nConnectionsClosed)
    {
        ACE_DEBUG((LM_INFO, ACE_TEXT("[WARNING] Create/Close call mismatch - Create calls: %d, Close calls: %d\n"), sm_nConnectionsCreated, sm_nConnectionsClosed));
    }
}
#endif
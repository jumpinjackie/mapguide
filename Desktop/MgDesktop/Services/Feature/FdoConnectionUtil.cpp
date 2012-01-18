#include "MgDesktop.h"
#include "FdoConnectionUtil.h"
#include "Fdo.h"
#include "FeatureServiceCache.h"
#include "Services/Resource/UnmanagedDataManager.h"

FdoIConnection* MgFdoConnectionUtil::CreateConnection(CREFSTRING providerName, CREFSTRING connectionString)
{
    Ptr<MgServiceFactory> fact = new MgServiceFactory();
    Ptr<MgdResourceService> resSvc = static_cast<MgdResourceService*>(fact->CreateService(MgServiceType::ResourceService));
    FdoPtr<FdoProviderNameTokens> tokens = FdoProviderNameTokens::Create(providerName.c_str());
    FdoStringsP tokenValues = tokens->GetNameTokens();
    FdoPtr<FdoStringElement> company = tokenValues->GetItem(0);
    FdoPtr<FdoStringElement> local = tokenValues->GetItem(1);

    STRING providerNoVersion = (FdoString*)company->GetString();

    providerNoVersion += L".";
    providerNoVersion += (FdoString*)local->GetString();

	FdoPtr<IConnectionManager> connMgr = FdoFeatureAccessManager::GetConnectionManager();
	FdoPtr<FdoIConnection> conn = connMgr->CreateConnection(providerNoVersion.c_str());

    //Some providers may be sensitive to being assigned an empty string
    if (!connectionString.empty())
    {
	    conn->SetConnectionString(connectionString.c_str());
    }

	return conn.Detach();
}

FdoIConnection* MgFdoConnectionUtil::CreateConnection(MgResourceIdentifier* resource)
{
    CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgFdoConnectionUtil::CreateConnection");
    Ptr<MgServiceFactory> fact = new MgServiceFactory();
    Ptr<MgdResourceService> resSvc = static_cast<MgdResourceService*>(fact->CreateService(MgServiceType::ResourceService));

	std::string xmlContent;
	Ptr<MgByteReader> content = resSvc->GetResourceContent(resource, L"");

	content->ToStringUtf8(xmlContent);

    MdfModel::FeatureSource* fs = GetFeatureSource(resource);
	STRING provider = (STRING)fs->GetProvider();
    STRING configDoc = (STRING)fs->GetConfigurationDocument();

	FdoPtr<FdoIConnection> conn;

	MG_FEATURE_SERVICE_TRY()
	
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

	MdfModel::NameStringPairCollection* params = fs->GetParameters();
	for (INT32 i = 0; i < params->GetCount(); i++)
	{
		MdfModel::NameStringPair* pair = params->GetAt(i);
        
        STRING n = pair->GetName();
        STRING v = pair->GetValue();

        //Perform tag substitution if found
        PerformTagSubstitution(resSvc, v, resource);

		dict->SetProperty(n.c_str(), v.c_str());
	}

    FdoPtr<FdoIConnectionCapabilities> connCaps = conn->GetConnectionCapabilities();
    if (connCaps->SupportsConfiguration() && !configDoc.empty())
    {
        STRING path = resSvc->ResolveDataPath(resource);
        MgFileUtil::AppendSlashToEndOfPath(path);
        path += configDoc;

        FdoPtr<FdoIoFileStream> fs = FdoIoFileStream::Create(path.c_str(), L"r");
        conn->SetConfiguration(fs);
    }

	MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgFdoConnectionUtil::CreateConnection")

	return conn.Detach();
}

void MgFdoConnectionUtil::PerformTagSubstitution(MgdResourceService* resSvc, REFSTRING str, MgResourceIdentifier* resource)
{
    STRING dataToken = L"%MG_DATA_FILE_PATH%";
    const int pos = str.find(dataToken);
    if (pos >= 0)
    {
        STRING dataPath = resSvc->ResolveDataPath(resource);
        str.replace(pos, dataToken.size(), dataPath);
    }
    MgUnmanagedDataManager::SubstituteDataPathAliases(str);
}

STRING MgFdoConnectionUtil::ParseNonQualifiedProviderName(CREFSTRING providerName)
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

MdfModel::FeatureSource* MgFdoConnectionUtil::GetFeatureSource(MgResourceIdentifier* resource)
{
    MgFeatureServiceCache* cache = MgFeatureServiceCache::GetInstance();
    Ptr<MgFeatureSourceCacheItem> fsCache = cache->GetFeatureSource(resource);

    MdfModel::FeatureSource* fs = NULL;

    if (NULL != fsCache)
    {
        fs = fsCache->Get();

        //There is a cached item. Return it.
        if (NULL != fs)
            return fs;
    }

    // Get the Resource Service.
    Ptr<MgServiceFactory> fact = new MgServiceFactory();
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
            throw new MgInvalidFeatureSourceException(
                L"MgFdoConnectionUtil::GetFeatureSource",
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

        throw new MgInvalidFeatureSourceException(
            L"MgFdoConnectionUtil::GetFeatureSource",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    //Cache this item for future calls
    if (NULL != fsCache)
    {
        fsCache->Set(fs);
    }
    else
    {
        fsCache = new MgFeatureSourceCacheItem(fs);
        cache->SetFeatureSource(resource, fsCache);
    }

    return fs;
}
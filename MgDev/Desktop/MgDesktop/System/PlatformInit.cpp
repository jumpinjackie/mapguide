#include "MgDesktop.h"
#include "Log/LogManager.h"
#include "Fdo.h"
#include "Services/Rendering/MappingUtil.h"
#include "Services/Feature/FdoConnectionPool.h"

bool MgdPlatform::sm_init = false;

bool MgdPlatform::IsInitialized() { return sm_init; }

void MgdPlatform::Initialize(CREFSTRING configFile)
{
    if (sm_init)
        return;

    ACE::init();
    XMLPlatformUtils::Initialize();

    MgConfiguration* pConfiguration = MgConfiguration::GetInstance();
    pConfiguration->LoadConfiguration(configFile);

    // Get the resources path.
    STRING resourcesPath;
    pConfiguration->GetStringValue(MgdConfigProperties::GeneralPropertiesSection, 
                                   MgdConfigProperties::GeneralPropertyResourcesPath, 
                                   resourcesPath, 
                                   MgdConfigProperties::DefaultGeneralPropertyResourcesPath);

    // Get the default message locale.
    STRING defaultMessageLocale;
    pConfiguration->GetStringValue(MgdConfigProperties::GeneralPropertiesSection, MgdConfigProperties::GeneralPropertyDefaultMessageLocale, defaultMessageLocale, MgdConfigProperties::DefaultGeneralPropertyDefaultMessageLocale);

    // Init log manager
    MgdLogManager* pLogManager = MgdLogManager::GetInstance();
    pLogManager->Initialize();

    MgdServiceFactory::Initialize();

    // Init the Fdo Connection Pool
    MgdFdoConnectionPool::Initialize(pConfiguration);

    // Init resources
    MgResources* pResources = MgResources::GetInstance();
    pResources->Initialize(resourcesPath);
    pResources->LoadResources(defaultMessageLocale);

    // Init FDO
    STRING fdoPath;
    pConfiguration->GetStringValue(MgdConfigProperties::GeneralPropertiesSection, 
                                   MgdConfigProperties::GeneralPropertyFdoPath, 
                                   fdoPath, 
                                   MgdConfigProperties::DefaultGeneralPropertyFdoPath);

    // Check if path ends with a '/' if not, add one if needed
    MgFileUtil::AppendSlashToEndOfPath(fdoPath);

    // Inject MENTOR_DICTIONARY_PATH
    STRING mentorDictPath;
    pConfiguration->GetStringValue(MgdConfigProperties::GeneralPropertiesSection, 
                                   MgdConfigProperties::GeneralPropertyMentorDictionaryPath, 
                                   mentorDictPath, 
                                   MgdConfigProperties::DefaultGeneralPropertyMentorDictionaryPath);

    #ifdef WIN32
    HMODULE hlib = NULL;

    // Get the size of the PATH environment variable for this process
    DWORD size = GetEnvironmentVariableW(L"PATH", NULL, 0) + 1;

    wchar_t* pathBuffer = new wchar_t[size];
    if(pathBuffer)
    {
        // Get the PATH environment variable for this process
        size = GetEnvironmentVariableW(L"PATH", pathBuffer, size);

        // Add our own FDO path to the start because we want it searched 1st
        STRING updatedPath = fdoPath;
        updatedPath += L";";
        updatedPath += pathBuffer;

        // Set the PATH environment variable for this process
        SetEnvironmentVariableW(L"PATH", updatedPath.c_str());

        delete [] pathBuffer;
        pathBuffer = NULL;

        // Dump the paths to the trace log
        MG_LOG_TRACE_ENTRY(L"PATH = " + updatedPath);
    }

    Ptr<MgCoordinateSystemFactory> csFactory = new MgCoordinateSystemFactory();
    Ptr<MgCoordinateSystemCatalog> csCatalog = csFactory->GetCatalog();

    //If specified in config inject this path into MENTOR_DICTIONARY_PATH
    if (!mentorDictPath.empty())
    {
        csCatalog->SetDictionaryDir(mentorDictPath);
        
        //STRING setCmd = L"MENTOR_DICTIONARY_PATH=" + mentorDictPath;
        //_wputenv(setCmd.c_str());
        //SetEnvironmentVariableW(L"MENTOR_DICTIONARY_PATH", mentorDictPath.c_str());
    }
    #else
    void* hlib = NULL;
    string updatedPath;

    char* ldlibpath = getenv("LD_LIBRARY_PATH");
    if (ldlibpath != NULL)
    {
        updatedPath = ldlibpath;
        updatedPath += ":";
    }
    else
    {
        updatedPath = "";
    }
    updatedPath += MgUtil::WideCharToMultiByte(fdoPath);

    setenv("LD_LIBRARY_PATH", updatedPath.c_str(), 1);

    // Dump the paths to the trace log
    STRING libraryPath = MgUtil::MultiByteToWideChar(updatedPath);
    MG_LOG_TRACE_ENTRY(L"LD_LIBRARY_PATH = " + libraryPath);

    //If specified in config and checks out as a directory, inject this path into MENTOR_DICTIONARY_PATH
    if (!mentorDictPath.empty())
    {
        if (MgFileUtil::IsDirectory(mentorDictPath))
        {
            setenv("MENTOR_DICTIONARY_PATH", mentorDictPath.c_str(), 1);
        }
    }
    #endif

    // Load the Fdo library
    STRING fdoLibrary = fdoPath;

    #ifdef WIN32
    fdoLibrary += L"fdo.dll";
    hlib = LoadLibraryW(fdoLibrary.c_str());
    #else
    fdoLibrary += L"libFDO.so";
    hlib = dlopen(MG_WCHAR_TO_TCHAR(fdoLibrary), RTLD_NOW);
    #endif

    if (NULL == hlib)
    {
        ACE_DEBUG ((LM_DEBUG, ACE_TEXT("(%t) MgdPlatform::Initialize() - Failed to load FDO library.\n")));
        throw new MgFdoException(L"MgdPlatform::Initialize",
             __LINE__, __WFILE__, NULL, L"MgFailedToLoadFdoLibrary", NULL);
    }

    // Initialize the FdoIDisposable global thread locking
    FdoIDisposable::EnableGlobalThreadLocking(true);

    // Initialize Stylizer callback mechanism for non-fatal FDO exceptions
    MgdMappingUtil::InitializeStylizerCallback();

    // Precache the specified maps, which will prime any related caches
    STRING preCacheMaps;
    pConfiguration->GetStringValue(MgdConfigProperties::GeneralPropertiesSection, MgdConfigProperties::GeneralPropertyPreCacheMaps, preCacheMaps, MgdConfigProperties::DefaultGeneralPropertyPreCacheMaps);

    // Check if there is actually anything to precache
    if (!preCacheMaps.empty())
    {
        Ptr<MgStringCollection> preCacheMapsCollection;
        preCacheMapsCollection = MgStringCollection::ParseCollection(preCacheMaps, L",");

        if (preCacheMapsCollection->GetCount() > 0)
        {
            ACE_DEBUG((LM_INFO, ACE_TEXT("%W\n"), MgResources::PreCacheMapsStart.c_str()));

            for(INT32 i = 0; i < preCacheMapsCollection->GetCount(); i++)
            {
                try
                {
                    STRING mapRes = preCacheMapsCollection->GetItem(i);
                    ACE_DEBUG((LM_INFO, ACE_TEXT("  %W  "), mapRes.c_str()));
                    Ptr<MgResourceIdentifier> mapResId = new MgResourceIdentifier(mapRes);
                    Ptr<MgdMap> map = new MgdMap(mapResId, L"PreCacheMap");
                    ACE_DEBUG((LM_INFO, ACE_TEXT("<%W>\n"), MgResources::Success.c_str()));
                }
                catch(MgException* e)
                {
                    // Skip map entries that fail to be cached
                    STRING message = e->GetDetails();
                    SAFE_RELEASE(e);
                    ACE_DEBUG((LM_INFO, ACE_TEXT("<%W> %W\n"), MgResources::Failure.c_str(), message.c_str()));
                }
            }

            ACE_DEBUG((LM_INFO, ACE_TEXT("\n")));
        }
    }

    MG_LOG_TRACE_ENTRY(L"MgdPlatform::Initialize()");

    sm_init = true;
}

void MgdPlatform::Terminate()
{
    MG_LOG_TRACE_ENTRY(L"MgdPlatform::Terminate()");
    MG_TRY()

    MgdFdoConnectionPool::Cleanup();

    //This is important. Otherwise the process using this library will be left lingering
    MgdLogManager* pLogManager = MgdLogManager::GetInstance();
    pLogManager->StopLogThread();

    Ptr<MgdServiceFactory> fact = new MgdServiceFactory();;
    Ptr<MgdResourceService> resSvc = dynamic_cast<MgdResourceService*>(fact->CreateService(MgServiceType::ResourceService));
    resSvc->DeleteSessionFiles();

    XMLPlatformUtils::Terminate();
    ACE::fini();

    MG_CATCH_AND_RELEASE()
}

static bool InitializeStaticPlatformData();

static bool initStatic = InitializeStaticPlatformData();

#define EXCEPTION_CLASS_CREATOR(className) \
    fact->Register(MapGuide_Desktop_Exception_##className, Mgd##className::CreateObject);

bool InitializeStaticPlatformData()
{
    MgClassFactory* fact = MgClassFactory::GetInstance();

    //put in the map any class that can be serialized
    fact->Register(MapGuide_Desktop_MapLayer_Map, MgdMap::CreateObject);
    fact->Register(MapGuide_Desktop_MapLayer_Layer, MgdLayer::CreateObject);
    fact->Register(MapGuide_Desktop_MapLayer_Selection, MgdSelection::CreateObject);

    EXCEPTION_CLASS_CREATOR(ConnectionFailedException)
    EXCEPTION_CLASS_CREATOR(DuplicateParameterException)
    EXCEPTION_CLASS_CREATOR(DwfException)
    EXCEPTION_CLASS_CREATOR(DwfSectionNotFoundException)
    EXCEPTION_CLASS_CREATOR(DwfSectionResourceNotFoundException)
    EXCEPTION_CLASS_CREATOR(InvalidDwfPackageException)
    EXCEPTION_CLASS_CREATOR(InvalidDwfSectionException)
    EXCEPTION_CLASS_CREATOR(InvalidFeatureSourceException)
    EXCEPTION_CLASS_CREATOR(ParameterNotFoundException)
    EXCEPTION_CLASS_CREATOR(StylizeLayerFailedException)
    EXCEPTION_CLASS_CREATOR(InvalidPrintLayoutFontSizeUnitsException)
    EXCEPTION_CLASS_CREATOR(InvalidPrintLayoutPositionUnitsException)
    EXCEPTION_CLASS_CREATOR(InvalidPrintLayoutSizeUnitsException)

    return true;
}
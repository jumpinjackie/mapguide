#include "MgDesktop.h"
#include "Log/LogManager.h"
#include "Fdo.h"
#include "Services/Rendering/MappingUtil.h"
#include "Services/Feature/FdoConnectionPool.h"

bool MgPlatform::sm_init = false;

bool MgPlatform::IsInitialized() { return sm_init; }

void MgPlatform::Initialize(CREFSTRING configFile)
{
    if (sm_init)
        return;

    ACE::init();
    XMLPlatformUtils::Initialize();

    MgConfiguration* pConfiguration = MgConfiguration::GetInstance();
    pConfiguration->LoadConfiguration(configFile);

    // Get the resources path.
    STRING resourcesPath;
    pConfiguration->GetStringValue(MgConfigProperties::GeneralPropertiesSection, 
                                   MgConfigProperties::GeneralPropertyResourcesPath, 
                                   resourcesPath, 
                                   MgConfigProperties::DefaultGeneralPropertyResourcesPath);

    // Get the default message locale.
    STRING defaultMessageLocale;
    pConfiguration->GetStringValue(MgConfigProperties::GeneralPropertiesSection, MgConfigProperties::GeneralPropertyDefaultMessageLocale, defaultMessageLocale, MgConfigProperties::DefaultGeneralPropertyDefaultMessageLocale);

    // Init log manager
    MgLogManager* pLogManager = MgLogManager::GetInstance();
    pLogManager->Initialize();

    MgServiceFactory::Initialize();

    // Init the Fdo Connection Pool
    MgFdoConnectionPool::Initialize(pConfiguration);

    // Init resources
    MgResources* pResources = MgResources::GetInstance();
    pResources->Initialize(resourcesPath);
    pResources->LoadResources(defaultMessageLocale);

    // Init FDO
    STRING fdoPath;
    pConfiguration->GetStringValue(MgConfigProperties::GeneralPropertiesSection, 
                                   MgConfigProperties::GeneralPropertyFdoPath, 
                                   fdoPath, 
                                   MgConfigProperties::DefaultGeneralPropertyFdoPath);

    // Check if path ends with a '/' if not, add one if needed
    MgFileUtil::AppendSlashToEndOfPath(fdoPath);

    // Inject MENTOR_DICTIONARY_PATH
    STRING mentorDictPath;
    pConfiguration->GetStringValue(MgConfigProperties::GeneralPropertiesSection, 
                                   MgConfigProperties::GeneralPropertyMentorDictionaryPath, 
                                   mentorDictPath, 
                                   MgConfigProperties::DefaultGeneralPropertyMentorDictionaryPath);

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

    // NOTE: Altering the dictionary path via MgCoordinateSystemCatalog::SetDictionaryDir() does not
    // update the internal library status. Possible defect?
    /*
    if (csCatalog->GetLibraryStatus() != LibraryStatus::lsInitialized)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgPlatform::Initialize", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemInitializationFailedException", NULL);
    }*/

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
        ACE_DEBUG ((LM_DEBUG, ACE_TEXT("(%t) MgPlatform::Initialize() - Failed to load FDO library.\n")));
        throw new MgFdoException(L"MgPlatform::Initialize",
             __LINE__, __WFILE__, NULL, L"MgFailedToLoadFdoLibrary", NULL);
    }

    // Initialize the FdoIDisposable global thread locking
    FdoIDisposable::EnableGlobalThreadLocking(true);

    // Initialize Stylizer callback mechanism for non-fatal FDO exceptions
    MgMappingUtil::InitializeStylizerCallback();

    MG_LOG_TRACE_ENTRY(L"MgPlatform::Initialize()");

    sm_init = true;
}

void MgPlatform::Terminate()
{
    MG_LOG_TRACE_ENTRY(L"MgPlatform::Terminate()");
    MG_TRY()

    //MgFdoConnectionPool::Cleanup();

    //This is important. Otherwise the process using this library will be left lingering
    MgLogManager* pLogManager = MgLogManager::GetInstance();
    pLogManager->StopLogThread();

    Ptr<MgServiceFactory> fact = new MgServiceFactory();;
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
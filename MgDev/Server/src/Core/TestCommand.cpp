#include "LogManager.h"
#include "TestCommand.h"

MgTestCommand::MgTestCommand(CREFSTRING locale, CREFSTRING subSuite, CREFSTRING outputFile)
    : MgServerInteractiveCommand()
{
    m_locale = locale;
    m_subSuite = subSuite;
    m_outputFile = outputFile;
}

MgTestCommand::~MgTestCommand()
{
}

INT32 MgTestCommand::Execute()
{
    INT32 nResult = 0;

    // Run the test cases

    typedef int(*EXECUTE)(CREFSTRING, CREFSTRING);

    MG_LOG_TRACE_ENTRY(L"MgTestCommand::Execute() - Running the server unit tests.");
    ACE_DEBUG((LM_INFO, ACE_TEXT("Preparing to run the unit tests...\n\n")));

    try
    {
        // Let the site manager know that the check servers background thread needs to stop
        MgSiteManager* siteManager = MgSiteManager::GetInstance();
        siteManager->StopCheckServersThread();

        // Change the log file names to use the unit test names because we don't want to replace the existing log files
        MgLogManager* pLogManager = MgLogManager::GetInstance();
        STRING filename;

        filename = L"Test" + MgLogManager::DefaultAccessLogFileName;
        pLogManager->SetAccessLogFileName(filename);

        filename = L"Test" + MgLogManager::DefaultAdminLogFileName;
        pLogManager->SetAdminLogFileName(filename);

        filename = L"Test" + MgLogManager::DefaultAuthenticationLogFileName;
        pLogManager->SetAuthenticationLogFileName(filename);

        filename = L"Test" + MgLogManager::DefaultErrorLogFileName;
        pLogManager->SetErrorLogFileName(filename);

        filename = L"Test" + MgLogManager::DefaultSessionLogFileName;
        pLogManager->SetSessionLogFileName(filename);

        filename = L"Test" + MgLogManager::DefaultTraceLogFileName;
        pLogManager->SetTraceLogFileName(filename);

        EXECUTE execute = NULL;

#ifdef _WIN32
        HMODULE hlib = NULL;
#ifdef _DEBUG // load debug dll
        STRING library = L"MgUnitTestingd.dll";
        hlib = LoadLibrary(library.c_str());
#else // Load Release dll
        STRING library = L"MgUnitTesting.dll";
        hlib = LoadLibrary(library.c_str());
#endif

        if (hlib != NULL)
        {
            execute = (EXECUTE)GetProcAddress(hlib, "Execute");
        }
        else
        {
            ACE_DEBUG((LM_INFO, ACE_TEXT("Cannot open library: %W\n"), library.c_str()));
            throw new MgUnclassifiedException(L"MgTestCommand.Execute", __LINE__, __WFILE__, NULL, L"", NULL);
        }
#else
        string library = "libMgUnitTesting.so";
        void* hlib = dlopen(library.c_str(), RTLD_NOW);

        if (hlib != NULL)
        {
            execute = (EXECUTE)dlsym(hlib, "Execute");
        }
        else
        {
            ACE_DEBUG((LM_INFO, ACE_TEXT("Cannot open library: %s\n"), library.c_str()));
            throw new MgUnclassifiedException(L"MgTestCommand.Execute", __LINE__, __WFILE__, NULL, L"", NULL);
        }
#endif // _WIN32
        if (execute != NULL)
        {
            nResult = (*execute)(m_outputFile, m_subSuite);

            // The build script does not work with negative return codes, which is what is returned on a failure from CPPUnit.
            // Therefore, we change the -1 result to a positive 1 to indicate to the build script that an error occurred.
            if (nResult < 0)
            {
                nResult = -(nResult);
            }
        }
        else
        {
            // Failed to retrieve function
            ACE_DEBUG((LM_INFO, ACE_TEXT("Cannot locate 'Execute' procedure address inside library.\n")));
            throw new MgUnclassifiedException(L"MgTestCommand.Execute", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        ACE_DEBUG((LM_INFO, ACE_TEXT("Finished running the unit tests.\n\n")));
    }
    catch (MgException* e)
    {
        ACE_DEBUG((LM_ERROR, ACE_TEXT("Unable to run all the unit tests.\n")));
        ACE_DEBUG((LM_ERROR, ACE_TEXT("%W\n"), e->GetStackTrace(m_locale).c_str()));
        SAFE_RELEASE(e);

        nResult = -1;
    }
    catch (...)
    {
        ACE_DEBUG((LM_ERROR, ACE_TEXT("Unable to run all the unit tests.\n")));

        nResult = -1;
    }

    return nResult;
}
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

#include "Server.h"
#include "ClientAcceptor.h"
#include "OperationThread.h"
#include "LicenseManager.h"
#include "ServerManager.h"
#include "LoadBalanceManager.h"
#include "ServiceManager.h"
#include "LogManager.h"
#include "FdoConnectionManager.h"
#include "SignalHandler.h"
#include "FontManager.h"
#include "LongTransactionManager.h"
#include "CacheManager.h"
#include "ServerFeatureTransactionPool.h"

#include "Stylizer.h"
#include "Bounds.h"
#include "Renderer.h"
#include "MappingUtil.h"

#ifdef _DEBUG
void DebugOutput(const ACE_TCHAR* format, ...)
{
#ifdef _WIN32
    FILE* fptr = ACE_OS::fopen(ACE_TEXT("c:\\debug.txt"), ACE_TEXT("a"));
#else
    FILE* fptr = ACE_OS::fopen(ACE_TEXT("/home/debug.txt"), ACE_TEXT("a"));
#endif
    if(fptr)
    {
        // Start of variable args section.
        va_list argp;
        va_start (argp, format);

        vfprintf(fptr, MG_TCHAR_TO_CHAR(format), argp);
        ACE_OS::fclose(fptr);

        va_end (argp);
    }
}
#endif

ACE_RCSID(server, MgServer, "Server.cpp")

#ifdef _WIN32
MG_NT_SERVICE_DEFINE(MgServerService, MgServer, MG_WCHAR_TO_TCHAR(MgResources::ServerServiceName));
#endif

//////////////////////////////////////////////////////////////////
/// <summary>
/// Constructor.
/// </summary>
MgServer::MgServer()
{
    ACE_DEBUG ((LM_DEBUG, ACE_TEXT("(%t) MgServer::MgServer()\n")));
#ifdef ACE_WIN64
    ACE_DEBUG ((LM_DEBUG, ACE_TEXT("ACE_WIN64 DEFINED\n")));
#else
    ACE_DEBUG ((LM_DEBUG, ACE_TEXT("ACE_WIN64 NOT DEFINED\n")));
#endif

    reactor(ACE_Reactor::instance());

    m_bTestMode = false;
    m_bTestFdo = false;

#ifdef _DEBUG
    m_nClientRequestLimit = -1;   // -1 = No limit. DEBUG ONLY
#endif
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Destructor.
/// </summary>
MgServer::~MgServer()
{
    ACE_DEBUG ((LM_DEBUG, ACE_TEXT("(%t) MgServer::~MgServer()\n")));
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// ACE_Service_Object initialization.
/// </summary>
int MgServer::init(int argc, ACE_TCHAR *argv[])
{
    ACE_DEBUG ((LM_DEBUG, ACE_TEXT("(%t) MgServer::init()\n")));

    int nResult = 0;

    // Update the server service status
#ifdef _WIN32
    if(svc_handle_)
    {
        report_status(SERVICE_START_PENDING, 10000);
    }
#endif

    // Parse arguments
    ParseArgs(argc, argv);

    return nResult;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// ACE_Service_Object finalization.
/// </summary>
int MgServer::fini()
{
    MG_LOG_TRACE_ENTRY(L"MgServer::fini() - Start");
    ACE_DEBUG ((LM_DEBUG, ACE_TEXT("(%t) MgServer::fini()\n")));

    // Signal the worker threads to STOP
    MgServerManager* pMgServerManager = MgServerManager::GetInstance();
    pMgServerManager->StopWorkerThreads();

    // Unregister all the server services.
    {
        MG_TRY()

        MgLoadBalanceManager* loadBalanceManager = MgLoadBalanceManager::GetInstance();
        ACE_ASSERT(NULL != loadBalanceManager);

        if (NULL != loadBalanceManager)
        {
            loadBalanceManager->UnregisterServices();
        }

        // If an error occurs, catch the exception but do not throw it so that
        // the server can continue shutting down.

        MG_CATCH_AND_RELEASE()
    }

    // Orderly clean up applicable static manager objects which depend on
    // other third-party libraries so that the server can be cleanly shut down.
    MgFdoConnectionManager::Terminate();
    MgServiceManager::Terminate();
    MgLoadBalanceManager::Terminate();
    MgLongTransactionManager::Terminate();

    MgLicenseManager* licenseManager = MgLicenseManager::GetInstance();
    ACE_ASSERT(NULL != licenseManager);
    licenseManager->Terminate();

    // Log the current status.
    MgResources* pResources = MgResources::GetInstance();
    STRING message = pResources->FormatMessage(MgResources::ServerStopped, 0);

    ACE_DEBUG ((LM_INFO, ACE_TEXT("(%t) %W\n"), message.c_str()));
    MG_LOG_SYSTEM_ENTRY(LM_INFO, message.c_str());
    MG_LOG_ERROR_ENTRY(message.c_str());
    MG_LOG_TRACE_ENTRY(L"MgServer::fini() - End");

#ifdef _WIN32
    if(svc_handle_)
    {
        report_status(SERVICE_STOPPED);
    }
#endif

    // Signal the log thread to STOP
    MgLogManager* pMgLogManager = MgLogManager::GetInstance();
    pMgLogManager->StopLogThread();

    return 0;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Parse the startup options. The arguments here are for
/// testing/debugging use only. All the server configuration
/// settings come from a separate configuration file.
/// </summary>
void MgServer::ParseArgs (INT32 argc, ACE_TCHAR *argv[])
{
    ACE_DEBUG ((LM_DEBUG, ACE_TEXT("(%t) MgServer::ParseArgs()\n")));

    if(argc > 1)
    {
        // Remove the "/" character
        INT32 parameterSize = ACE_OS::strlen(argv[1]);
        ACE_TCHAR* parameter = new ACE_TCHAR[parameterSize+1];
        INT32 pos = 0;
        for(INT32 i=0;i<parameterSize;i++)
        {
            if((i == 0) && (argv[1][0] == ACE_TEXT('/')))
            {
                // Skip this character
            }
            else
            {
                parameter[pos] = argv[1][i];

                // Next position
                pos++;
            }
        }

        // Null terminate the string
        parameter[pos] = 0;

        if((ACE_OS::strcasecmp(parameter, MG_WCHAR_TO_TCHAR(MgResources::ServerCmdTest)) == 0) ||
           (ACE_OS::strcasecmp(parameter, MG_WCHAR_TO_TCHAR(MgResources::ServerCmdTestMode)) == 0))
        {
            // Test mode
            m_bTestMode = true;
            m_strTestFileName = L"";    // Default to no output filename
            m_strTestName = MgResources::ServerCmdTestDefaultTests;     // Default to all of the tests

            // If there is a 2nd parameter it is the test to run
            if(argc > 2)
            {
                m_strTestName = MG_TCHAR_TO_WCHAR(argv[2]);
            }

            // If there is a 3rd parameter it is the output filename
            if(argc > 3)
            {
                m_strTestFileName = MG_TCHAR_TO_WCHAR(argv[3]);
            }
        }
        else if(ACE_OS::strcasecmp(parameter, MG_WCHAR_TO_TCHAR(MgResources::ServerCmdTestFdo)) == 0)
        {
            // Test FDO
            m_bTestFdo = true;

            // If there is a 2nd parameter it is the output filename
            if(argc > 2)
            {
                m_strTestFileName = MG_TCHAR_TO_WCHAR(argv[2]);
            }
            else
            {
                m_strTestFileName = L"";
            }
        }

        delete[] parameter;
    }
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Main execution method.
/// </summary>
int MgServer::svc()
{
    MG_LOG_TRACE_ENTRY(L"MgServer::svc() - Start");
    ACE_DEBUG ((LM_DEBUG, ACE_TEXT("(%t) MgServer::svc()\n")));

    INT32 nResult = 0;

    MgServerManager* pServerManager = MgServerManager::GetInstance();

    if(m_bTestMode)
    {
        // Run the test cases

        typedef int (*EXECUTE)(CREFSTRING, CREFSTRING);

        MG_LOG_TRACE_ENTRY(L"MgServer::svc() - Running the server unit tests.");
        ACE_DEBUG((LM_INFO, ACE_TEXT("Preparing to run the unit tests...\n\n")));

        try
        {
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
                throw new MgUnclassifiedException(L"MgServer.svc", __LINE__, __WFILE__, NULL, L"", NULL);
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
                throw new MgUnclassifiedException(L"MgServer.svc", __LINE__, __WFILE__, NULL, L"", NULL);
            }
        #endif // _WIN32
            if (execute != NULL)
            {
                nResult = (*execute)(m_strTestFileName, m_strTestName);

                // The build script does not work with negative return codes, which is what is returned on a failure from CPPUnit.
                // Therefore, we change the -1 result to a positive 1 to indicate to the build script that an error occurred.
                if(nResult < 0)
                {
                    nResult = -(nResult);
                }
            }
            else
            {
                // Failed to retrieve function
                ACE_DEBUG((LM_INFO, ACE_TEXT("Cannot locate 'Execute' procedure address inside library.\n")));
                throw new MgUnclassifiedException(L"MgServer.svc", __LINE__, __WFILE__, NULL, L"", NULL);
            }

            ACE_DEBUG((LM_INFO, ACE_TEXT("Finished running the unit tests.\n\n")));
        }
        catch (MgException* e)
        {
            ACE_DEBUG((LM_ERROR, ACE_TEXT("Unable to run all the unit tests.\n")));
            ACE_DEBUG((LM_ERROR, ACE_TEXT("%W\n"), e->GetStackTrace(pServerManager->GetDefaultMessageLocale()).c_str()));
            SAFE_RELEASE(e);

            nResult = -1;
        }
        catch (...)
        {
            ACE_DEBUG((LM_ERROR, ACE_TEXT("Unable to run all the unit tests.\n")));

            nResult = -1;
        }
    }
    else if(m_bTestFdo)
    {
        // Run the FDO test cases

        typedef int (*EXECUTE)(CREFSTRING);

        MG_LOG_TRACE_ENTRY(L"MgServer::svc() - Running the FDO unit tests.");
        ACE_DEBUG((LM_INFO, ACE_TEXT("Preparing to run the FDO unit tests...\n\n")));

        try
        {
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
            STRING library = L"MgFdoUnitTestingd.dll";
            hlib = LoadLibrary(library.c_str());
            #else // Load Release dll
            STRING library = L"MgFdoUnitTesting.dll";
            hlib = LoadLibrary(library.c_str());
            #endif

            if (hlib != NULL)
            {
                execute = (EXECUTE)GetProcAddress(hlib, "Execute");
            }
            else
            {
                ACE_DEBUG((LM_INFO, ACE_TEXT("Cannot open library: %W\n"), library.c_str()));
                throw new MgUnclassifiedException(L"MgServer.svc", __LINE__, __WFILE__, NULL, L"", NULL);
            }
        #else
            string library = "libMgFdoUnitTesting.so";
            void* hlib = dlopen(library.c_str(), RTLD_NOW);

            if (hlib != NULL)
            {
                execute = (EXECUTE)dlsym(hlib, "Execute");
            }
            else
            {
                ACE_DEBUG((LM_INFO, ACE_TEXT("Cannot open library: %s\n"), library.c_str()));
                throw new MgUnclassifiedException(L"MgServer.svc", __LINE__, __WFILE__, NULL, L"", NULL);
            }
        #endif // _WIN32
            if (execute != NULL)
            {
                nResult = (*execute)(m_strTestFileName);

                // The build script does not work with negative return codes, which is what is returned on a failure from CPPUnit.
                // Therefore, we change the -1 result to a positive 1 to indicate to the build script that an error occurred.
                if(nResult < 0)
                {
                    nResult = -(nResult);
                }
            }
            else
            {
                // Failed to retrieve function
                ACE_DEBUG((LM_INFO, ACE_TEXT("Cannot locate 'Execute' procedure address inside library.\n")));
                throw new MgUnclassifiedException(L"MgServer.svc", __LINE__, __WFILE__, NULL, L"", NULL);
            }

            ACE_DEBUG((LM_INFO, ACE_TEXT("Finished running the FDO unit tests.\n")));
        }
        catch (MgException* e)
        {
            ACE_DEBUG((LM_ERROR, ACE_TEXT("Unable to run all the FDO unit tests.\n")));
            ACE_DEBUG((LM_ERROR, ACE_TEXT("%W\n"), e->GetStackTrace(pServerManager->GetDefaultMessageLocale()).c_str()));
            SAFE_RELEASE(e);

            nResult = -1;
        }
        catch (...)
        {
            ACE_DEBUG((LM_ERROR, ACE_TEXT("Unable to run all the FDO unit tests.\n")));

            nResult = -1;
        }
    }
    else
    {
        try
        {
            // Add signal handlers
            // Note: Windows can only catch the SIGINT (CTRL-C) signal. The rest of the signals apply to Linux only.
            MgSignalHandler hSignalInterrupt(SIGINT);
            MgSignalHandler hSignalQuit(SIGQUIT);
            MgSignalHandler hSignalTerminate(SIGTERM);

            ACE_Reactor::instance()->register_handler(SIGINT, &hSignalInterrupt);
            ACE_Reactor::instance()->register_handler(SIGQUIT, &hSignalQuit);
            ACE_Reactor::instance()->register_handler(SIGTERM, &hSignalTerminate);

            if((pServerManager->GetAdminPort() != pServerManager->GetClientPort()) &&
               (pServerManager->GetAdminPort() != pServerManager->GetSitePort()) &&
               (pServerManager->GetClientPort() != pServerManager->GetSitePort()))
            {
                MG_LOG_TRACE_ENTRY(L"MgServer::svc() - Setup thread pool/register event handlers.");
                ACE_DEBUG ((LM_DEBUG, ACE_TEXT("(%t) MgServer::svc() - Setup thread pool/register event handlers\n")));

                // Activate event timers.
                m_eventTimerManager.Activate();

                // Setup the thread manager and the operation threads
                ACE_Thread_Manager threadManager;

                MgOperationThread clientThreads(threadManager, pServerManager->GetClientThreads());
                MgOperationThread adminThreads(threadManager, pServerManager->GetAdminThreads());
                MgOperationThread siteThreads(threadManager, pServerManager->GetSiteThreads());

                // Set the message queue in the MgServerManager
                pServerManager->SetAdminMessageQueue(adminThreads.msg_queue_);
                pServerManager->SetClientMessageQueue(clientThreads.msg_queue_);
                pServerManager->SetSiteMessageQueue(siteThreads.msg_queue_);

                ACE_INET_Addr clientAddr((u_short)pServerManager->GetClientPort());
                ACE_INET_Addr adminAddr((u_short)pServerManager->GetAdminPort());
                ACE_INET_Addr siteAddr((u_short)pServerManager->GetSitePort());

                MgClientAcceptor clientAcceptor(clientAddr, ACE_Reactor::instance(), clientThreads.msg_queue_);
                nResult = clientAcceptor.Initialize();
                if(nResult == 0)
                {
                    MgClientAcceptor adminAcceptor(adminAddr, ACE_Reactor::instance(), adminThreads.msg_queue_);
                    nResult = adminAcceptor.Initialize();
                    if(nResult == 0)
                    {
                        MgClientAcceptor siteAcceptor(siteAddr, ACE_Reactor::instance(), siteThreads.msg_queue_);
                        nResult = siteAcceptor.Initialize();
                        if(nResult == 0)
                        {
                            MgResources* pResources = MgResources::GetInstance();
                            STRING message = pResources->FormatMessage(MgResources::ServerStarted, 0);
                            ACE_DEBUG ((LM_INFO, ACE_TEXT("(%t) %W\n"), message.c_str()));
                            MG_LOG_SYSTEM_ENTRY(LM_INFO, message.c_str());
                            MG_LOG_ERROR_ENTRY(message.c_str());

                            // Start up the thread pools
                            nResult = clientThreads.Activate();
                            if(nResult == 0)
                            {
                                nResult = adminThreads.Activate();
                                if(nResult == 0)
                                {
                                    nResult = siteThreads.Activate();
                                    if(nResult == 0)
                                    {
                                        MG_LOG_TRACE_ENTRY(L"MgServer::svc() - Before Event Loop");
                                        nResult = ACE_Reactor::instance()->run_reactor_event_loop();
                                        MG_LOG_TRACE_ENTRY(L"MgServer::svc() - After Event Loop");

                                        // The server is shutting down so cleanup, but do it fast
                                        // as this could happen during a shutdown.

                                        // Tell the operation threads to stop
                                        ACE_Message_Block* mb = new ACE_Message_Block(4);
                                        if(mb)
                                        {
                                            mb->msg_type(ACE_Message_Block::MB_STOP);
                                            clientThreads.putq(mb);
                                        }

                                        mb = new ACE_Message_Block(4);
                                        if(mb)
                                        {
                                            mb->msg_type(ACE_Message_Block::MB_STOP);
                                            adminThreads.putq(mb);
                                        }

                                        mb = new ACE_Message_Block(4);
                                        if(mb)
                                        {
                                            mb->msg_type(ACE_Message_Block::MB_STOP);
                                            siteThreads.putq(mb);
                                        }

                                        // Stop the operation threads
                                        clientThreads.close();
                                        adminThreads.close();
                                        siteThreads.close();

                                        // Ensure the thread manager waits until all operation threads are done before closing
                                        threadManager.wait();
                                        threadManager.close();

                                        // Remove signal handlers
                                        ACE_Reactor::instance()->remove_handler(SIGINT, (ACE_Sig_Action*)NULL, (ACE_Sig_Action*)NULL, -1);
                                        ACE_Reactor::instance()->remove_handler(SIGQUIT, (ACE_Sig_Action*)NULL, (ACE_Sig_Action*)NULL, -1);
                                        ACE_Reactor::instance()->remove_handler(SIGTERM, (ACE_Sig_Action*)NULL, (ACE_Sig_Action*)NULL, -1);

                                        // Deactivate event timers.
                                        m_eventTimerManager.Deactivate();
                                    }
                                }
                            }
                        }
                        else
                        {
                            // Deactivate event timers.
                            m_eventTimerManager.Deactivate();

                            // Site port in use
                            // Throw exception
                            wchar_t buffer[255];
                            ACE_OS::itoa(pServerManager->GetSitePort(), buffer, 10);

                            MgStringCollection arguments;
                            arguments.Add(buffer);
                            throw new MgPortNotAvailableException(L"MgServer.svc", __LINE__, __WFILE__, &arguments, L"", NULL);
                        }
                    }
                    else
                    {
                        // Deactivate event timers.
                        m_eventTimerManager.Deactivate();

                        // Admin port in use
                        // Throw exception
                        wchar_t buffer[255];
                        ACE_OS::itoa(pServerManager->GetAdminPort(), buffer, 10);

                        MgStringCollection arguments;
                        arguments.Add(buffer);
                        throw new MgPortNotAvailableException(L"MgServer.svc", __LINE__, __WFILE__, &arguments, L"", NULL);
                    }
                }
                else
                {
                    // Deactivate event timers.
                    m_eventTimerManager.Deactivate();

                    // Client port in use
                    // Throw exception
                    wchar_t buffer[255];
                    ACE_OS::itoa(pServerManager->GetClientPort(), buffer, 10);

                    MgStringCollection arguments;
                    arguments.Add(buffer);
                    throw new MgPortNotAvailableException(L"MgServer.svc", __LINE__, __WFILE__, &arguments, L"", NULL);
                }
            }
            else
            {
                ACE_DEBUG ((LM_DEBUG, ACE_TEXT("(%t) MgServer::svc() - Client/Admin/Site on same port which is not allowed.\n")));
                ACE_DEBUG ((LM_DEBUG, ACE_TEXT("(%t) MgServer::svc() - Server shutting down!!!!\n")));
                nResult = -1;
            }
        }
        catch (MgException* e)
        {
            STRING message;
            message = e->GetDetails(pServerManager->GetDefaultMessageLocale());

            ACE_DEBUG((LM_ERROR, ACE_TEXT("(%t) %W\n"), message.c_str()));
            ACE_DEBUG((LM_ERROR, ACE_TEXT("(%t) %W\n"), e->GetStackTrace(pServerManager->GetDefaultMessageLocale()).c_str()));

            MG_LOG_SYSTEM_ENTRY(LM_ERROR, message.c_str());
            MG_LOG_ERROR_ENTRY(message.c_str());

            SAFE_RELEASE(e);

            nResult = -1;
        }
        catch (...)
        {
            STRING message = L"Application terminated abnormally.\nUnknown exception.";

            ACE_DEBUG((LM_ERROR, ACE_TEXT("(%t) %W\n"), message.c_str()));

            MG_LOG_SYSTEM_ENTRY(LM_ERROR, message.c_str());
            MG_LOG_ERROR_ENTRY(message.c_str());

            nResult = -1;
        }
    }

    MG_LOG_TRACE_ENTRY(L"MgServer::svc() - End");
    ACE_DEBUG ((LM_DEBUG, ACE_TEXT("(%t) MgServer::svc() - End\n")));
    return nResult;
}

void MgServer::AddFontManagerFontAliases()
{
    FontManager* fontManager = FontManager::Instance();
    MgConfiguration* pConfiguration = MgConfiguration::GetInstance();

    Ptr<MgPropertyCollection> pMappings = pConfiguration->GetProperties(MgConfigProperties::FontAliasMappingSection);
    if (pMappings != NULL)
    {
        for (int i = 0; i < pMappings->GetCount(); i++)
        {
            Ptr<MgStringProperty> pProperty = (MgStringProperty*)pMappings->GetItem(i);
            STRING name = pProperty->GetName();
            STRING value = pProperty->GetValue();
            fontManager->AddFontAlias(name.c_str(), value.c_str());
        }
    }
}

int MgServer::open(void *args)
{
    ACE_DEBUG ((LM_DEBUG, ACE_TEXT("(%t) MgServer::open()\n")));
    ACE_UNUSED_ARG(args);
    int nResult = 0;

    MG_TRY()
    {
        // We report that the server service is running here because the server can sometimes
        // take longer then the Windows Service Control Manager limit of 30 seconds allows.
        // This startup delay can be due to repository integrity check which cannot be avoided.
        #ifdef _WIN32
        if(svc_handle_)
        {
            report_status(SERVICE_RUNNING);
        }
        #endif

        // Load the configuration file into memory
        ACE_DEBUG ((LM_DEBUG, ACE_TEXT("(%t) MgServer::open() - Loading configuration.\n")));
        MgConfiguration* pConfiguration = MgConfiguration::GetInstance();
        pConfiguration->LoadConfiguration(MgConfigProperties::ServerConfigurationFilename);

        // Get the resources path.
        STRING resourcesPath;
        pConfiguration->GetStringValue(MgConfigProperties::GeneralPropertiesSection, MgConfigProperties::GeneralPropertyResourcesPath, resourcesPath, MgConfigProperties::DefaultGeneralPropertyResourcesPath);

        // Get the default message locale.
        STRING defaultMessageLocale;
        pConfiguration->GetStringValue(MgConfigProperties::GeneralPropertiesSection, MgConfigProperties::GeneralPropertyDefaultMessageLocale, defaultMessageLocale, MgConfigProperties::DefaultGeneralPropertyDefaultMessageLocale);

        // Get the server locale.
        STRING locale;
        pConfiguration->GetStringValue(MgConfigProperties::GeneralPropertiesSection, MgConfigProperties::GeneralPropertyLocale, locale, MgConfigProperties::DefaultGeneralPropertyLocale);

        char* serverLocale = NULL;

        // If the server configuration property is defined use it
        if (locale.empty())
        {
            // Set the locale to the system default
            // NOTE: windows may return a code-paged multi-byte string here
            // e.g. in the case of "Norwegian (Bokmal)"
            serverLocale = ::setlocale(LC_ALL, "");
        }
        else
        {
            string localeTemp = MgUtil::WideCharToMultiByte(locale);
            serverLocale = ::setlocale(LC_ALL, localeTemp.c_str());
        }

        if (NULL == serverLocale)
        {
            MgStringCollection whatArguments;
            whatArguments.Add(pConfiguration->GetFileName());

            MgStringCollection whyArguments;
            whyArguments.Add(MgConfigProperties::GeneralPropertyLocale);
            whyArguments.Add(MgConfigProperties::GeneralPropertiesSection);
            whyArguments.Add(locale);

            throw new MgConfigurationException(
                L"MgServer.open",
                __LINE__, __WFILE__, &whatArguments,
                L"MgConfigurationPropertyValueIsInvalid", &whyArguments);
        }

        // Always use the "C" locale when dealing with numbers
        ::setlocale(LC_NUMERIC, "C");

        // Initialize the Log Manager
        ACE_DEBUG ((LM_DEBUG, ACE_TEXT("(%t) MgServer::open() - Initializing Log Manager.\n")));
        MgLogManager* pLogManager = MgLogManager::GetInstance();
        pLogManager->Initialize();

        // Load the resources
        // This will load the specified locale or default locale if one is not specified.
        // If additional locales are needed they will be loaded/cached on demand.
        ACE_DEBUG ((LM_DEBUG, ACE_TEXT("(%t) MgServer::open() - Loading resources.\n")));
        MgResources* pResources = MgResources::GetInstance();
        pResources->Initialize(resourcesPath);

        // Try loading the specified locale resource. If it fails default to loading "en".
        // If that fails then we will not start the server because we need our resources.
        try
        {
            pResources->LoadResources(defaultMessageLocale);
        }
        catch(MgException* e)
        {
            SAFE_RELEASE(e);

            // Log error message about failing to load the specified locale resources
            MgStringCollection arguments;
            arguments.Add(pResources->GetResourceFilename(defaultMessageLocale));
            arguments.Add(defaultMessageLocale);

            STRING message = pResources->FormatMessage(MgResources::FailedToLoadResourcesFile, &arguments);
            ACE_DEBUG((LM_INFO, ACE_TEXT("(%t) %W\n"), message.c_str()));
            MG_LOG_ERROR_ENTRY(message.c_str());

            // Check to see if we were attempting to load the "en" locale resources.
            if(MgResources::DefaultMessageLocale == defaultMessageLocale)
            {
                // The server needs to fail to start because the specified default message locale was
                // the same as "en"
                nResult = -1;
            }
            else
            {
                // Try loading the "en" locale resources instead
                defaultMessageLocale = MgResources::DefaultMessageLocale;

                try
                {
                    // Log message indicating that we defaulted to using the "en" resources.
                    message = MgResources::UsingDefaultResourceFile;
                    ACE_DEBUG((LM_INFO, ACE_TEXT("(%t) %W\n"), message.c_str()));
                    MG_LOG_ERROR_ENTRY(message.c_str());

                    pResources->LoadResources(defaultMessageLocale);
                }
                catch(MgException* e)
                {
                    SAFE_RELEASE(e);
                    nResult = -1;

                    // We also failed to load the default "en" resource
                    MgStringCollection arguments;
                    arguments.Add(pResources->GetResourceFilename(defaultMessageLocale));
                    arguments.Add(defaultMessageLocale);

                    STRING message = pResources->FormatMessage(MgResources::FailedToLoadResourcesFile, &arguments);
                    ACE_DEBUG((LM_INFO, ACE_TEXT("(%t) %W\n"), message.c_str()));
                    MG_LOG_ERROR_ENTRY(message.c_str());
                }
            }
        }

        if(0 == nResult)
        {
            // Check Coordinate System initialization, if the following fails it should throw an exception
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) MgServer::open() - Initializing Coordinate System Library.\n")));
            Ptr<MgCoordinateSystemFactory> csFactory = new MgCoordinateSystemFactory();
            Ptr<MgCoordinateSystemCatalog> csCatalog = csFactory->GetCatalog();

            // Check if we have a catalog
            if (NULL == csCatalog.p)
            {
                throw new MgCoordinateSystemInitializationFailedException(
                    L"MgServer.open", __LINE__, __WFILE__, NULL, L"", NULL);
            }

            // Did we successfully initialize the coordinate system library?
            LibraryStatus status = csCatalog->GetLibraryStatus();
            if(lsInitialized != status)
            {
                throw new MgCoordinateSystemInitializationFailedException(
                    L"MgServer.open", __LINE__, __WFILE__, NULL, L"", NULL);
            }

            // Initialize the License Manager.
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) MgServer::open() - Initializing License Manager.\n")));
            MgLicenseManager* licenseManager = MgLicenseManager::GetInstance();
            ACE_ASSERT(NULL != licenseManager);
            licenseManager->Initialize();

            // Initialize the Server Manager
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("(%t) MgServer::open() - Initializing Server Manager.\n")));
            MgServerManager* pServerManager = MgServerManager::GetInstance();
            pServerManager->Initialize(defaultMessageLocale);

            // We cannot add trace statements until the log manager is initialized.
            MG_LOG_TRACE_ENTRY(L"MgServer::open() - Start");

            // Initialize the Load Balance Manager.
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) MgServer::open() - Initializing Load Balance Manager.\n")));
            MgLoadBalanceManager* loadBalanceManager = MgLoadBalanceManager::GetInstance();
            loadBalanceManager->Initialize();

            // Initialize the Service Manager
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("(%t) MgServer::open() - Initializing Service Manager.\n")));
            MgServiceManager* pServiceManager = MgServiceManager::GetInstance();
            pServiceManager->Initialize();

            // Initialize the Event Timer Manager.
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) MgServer::open() - Initializing Event Timer Manager.\n")));
            m_eventTimerManager.Initialize();

            // Feature Service
            bool bDataConnectionPoolEnabled = MgConfigProperties::DefaultFeatureServicePropertyDataConnectionPoolEnabled;
            INT32 nDataConnectionPoolSize = MgConfigProperties::DefaultFeatureServicePropertyDataConnectionPoolSize;
            STRING dataConnectionPoolExcludedProviders = MgConfigProperties::DefaultFeatureServicePropertyDataConnectionPoolExcludedProviders;
            STRING dataConnectionPoolSizeCustom = MgConfigProperties::DefaultFeatureServicePropertyDataConnectionPoolSizeCustom;
            STRING dataConnectionUseLimit = MgConfigProperties::DefaultFeatureServicePropertyDataConnectionUseLimit;

            pConfiguration->GetBoolValue(MgConfigProperties::FeatureServicePropertiesSection, MgConfigProperties::FeatureServicePropertyDataConnectionPoolEnabled, bDataConnectionPoolEnabled, MgConfigProperties::DefaultFeatureServicePropertyDataConnectionPoolEnabled);
            pConfiguration->GetIntValue(MgConfigProperties::FeatureServicePropertiesSection, MgConfigProperties::FeatureServicePropertyDataConnectionPoolSize, nDataConnectionPoolSize, MgConfigProperties::DefaultFeatureServicePropertyDataConnectionPoolSize);
            pConfiguration->GetStringValue(MgConfigProperties::FeatureServicePropertiesSection, MgConfigProperties::FeatureServicePropertyDataConnectionPoolExcludedProviders, dataConnectionPoolExcludedProviders, MgConfigProperties::DefaultFeatureServicePropertyDataConnectionPoolExcludedProviders);
            pConfiguration->GetStringValue(MgConfigProperties::FeatureServicePropertiesSection, MgConfigProperties::FeatureServicePropertyDataConnectionPoolSizeCustom, dataConnectionPoolSizeCustom, MgConfigProperties::DefaultFeatureServicePropertyDataConnectionPoolSizeCustom);
            pConfiguration->GetStringValue(MgConfigProperties::FeatureServicePropertiesSection, MgConfigProperties::FeatureServicePropertyDataConnectionUseLimit, dataConnectionUseLimit, MgConfigProperties::DefaultFeatureServicePropertyDataConnectionUseLimit);

            // Add additional font mappings to the FontManager
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) MgServer::open() - Adding Font Manager Mappings.\n")));
            AddFontManagerFontAliases();

            // Initialize the Cache Manager.
            MgCacheManager* cacheManager = MgCacheManager::GetInstance();
            cacheManager->Initialize();

            // Initialize and load the FDO library
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("(%t) MgServer::open() - Initializing FDO.\n")));
            STRING fdoPath;
            pConfiguration->GetStringValue(MgConfigProperties::GeneralPropertiesSection, MgConfigProperties::GeneralPropertyFdoPath, fdoPath, MgConfigProperties::DefaultGeneralPropertyFdoPath);

            // Check if path ends with a '/' if not, add one if needed
            MgFileUtil::AppendSlashToEndOfPath(fdoPath);

            #ifdef _WIN32
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
            #endif

            // Load the Fdo library
            STRING fdoLibrary = fdoPath;

            #ifdef _WIN32
            fdoLibrary += L"fdo.dll";
            hlib = LoadLibraryW(fdoLibrary.c_str());
            #else
            fdoLibrary += L"libFDO.so";
            hlib = dlopen(MG_WCHAR_TO_TCHAR(fdoLibrary), RTLD_NOW);
            #endif

            if (NULL == hlib)
            {
                ACE_DEBUG ((LM_DEBUG, ACE_TEXT("(%t) MgServer::open() - Failed to load FDO library.\n")));
                throw new MgFdoException(L"MgServer.svc",
                     __LINE__, __WFILE__, NULL, L"MgFailedToLoadFdoLibrary", NULL);
            }

            // Initialize the FDO Connection Manager
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("(%t) MgServer::open() - Initializing FDO Connection Manager.\n")));
            MgEventTimer& dataConnectionTimer = m_eventTimerManager.GetEventTimer(MgEventTimer::DataConnectionTimeout);
            MgFdoConnectionManager* pFdoConnectionManager = MgFdoConnectionManager::GetInstance();
            pFdoConnectionManager->Initialize(bDataConnectionPoolEnabled, nDataConnectionPoolSize, dataConnectionTimer.GetEventTimeout(), dataConnectionPoolExcludedProviders, dataConnectionPoolSizeCustom, dataConnectionUseLimit);

            // Initialize the transaction pool
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("(%t) MgServer::open() - Initializing transaction pool.\n")));
            MgEventTimer& dataTransactionTimer = m_eventTimerManager.GetEventTimer(MgEventTimer::DataTransactionTimeout);
            MgServerFeatureTransactionPool* pTransactionPool = MgServerFeatureTransactionPool::GetInstance();
            pTransactionPool->Initialize(dataTransactionTimer.GetEventTimeout());

            // On startup, perform the service registration for the Site server.
            // Note that this event will be perfomed by a timer for the Support server.
            ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) MgServer::open() - Registering Services.\n")));
            if (pServerManager->IsSiteServer() && loadBalanceManager->RegisterServices())
            {
                m_eventTimerManager.GetEventTimer(
                    MgEventTimer::ServiceRegistration).Terminate();
            }

            // Register the MgException locale callback so that exception messages will be generated in the
            // user's locale.  The Server uses thread locale storage to save user information so this callback
            // is easy to implement.
            MgException::RegisterLocaleCallback(MgServer::LocaleCallback);

            // Initialize Stylizer callback mechanism for non-fatal FDO exceptions
            MgMappingUtil::InitializeStylizerCallback();

#ifdef _DEBUG
            MgEventTimer& connectionTimer = m_eventTimerManager.GetEventTimer(MgEventTimer::ConnectionTimeout);
            STRING strResourceFilename = pResources->GetResourceFilename(pServerManager->GetDefaultMessageLocale());

            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("Server Information:\n")));
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("\n  Commandline Options:\n")));
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("    Client Request Limit          : %d\n"), m_nClientRequestLimit));
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("    Test Mode                     : %s\n"), m_bTestMode == true ? ACE_TEXT("true") : ACE_TEXT("false")));
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("    Test FDO                      : %s\n"), m_bTestFdo == true ? ACE_TEXT("true") : ACE_TEXT("false")));
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("\n  General Properties:\n")));
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("    Machine IP                    : %s\n"), MG_WCHAR_TO_TCHAR(loadBalanceManager->GetLocalServerAddress())));
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("    Fdo path                      : %s\n"), MG_WCHAR_TO_TCHAR(fdoPath)));
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("    Logs path                     : %s\n"), MG_WCHAR_TO_TCHAR(pLogManager->GetLogsPath())));
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("    Default Message Locale        : %s\n"), MG_WCHAR_TO_TCHAR(pServerManager->GetDefaultMessageLocale())));
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("    Locale Resources File Loaded  : %s\n"), MG_WCHAR_TO_TCHAR(strResourceFilename)));
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("\n  Client Service Properties:\n")));
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("    Connection Timeout            : %d\n"), connectionTimer.GetEventTimeout()));
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("    Connection Timer Interval     : %d\n"), connectionTimer.GetInterval()));
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("\n  Drawing Service Properties:\n")));
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("\n  Feature Service Properties:\n")));
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("    Data Connection Pool Enabled           : %s\n"), bDataConnectionPoolEnabled == true ? ACE_TEXT("true") : ACE_TEXT("false")));
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("    Data Connection Pool Excluded Providers: %s\n"), MG_WCHAR_TO_TCHAR(dataConnectionPoolExcludedProviders)));
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("    Data Connection Pool Size              : %d\n"), nDataConnectionPoolSize));
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("    Data Connection Pool Size Custom       : %s\n"), MG_WCHAR_TO_TCHAR(dataConnectionPoolSizeCustom)));
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("    Data Connection Timeout                : %d\n"), dataConnectionTimer.GetEventTimeout()));
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("    Data Connection Timer Interval         : %d\n"), dataConnectionTimer.GetInterval()));
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("\n  Mapping Service Properties:\n")));
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("\n  Rendering Service Properties:\n")));
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("\n  Resource Service Properties:\n")));
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("\n  Site Service Properties:\n")));

            if (pServerManager->IsSiteServer())
            {
                MgEventTimer& sessionTimer = m_eventTimerManager.GetEventTimer(MgEventTimer::SessionTimeout);

                ACE_DEBUG ((LM_DEBUG, ACE_TEXT("    Session Timeout               : %d\n"), sessionTimer.GetEventTimeout()));
                ACE_DEBUG ((LM_DEBUG, ACE_TEXT("    Session Timer Interval        : %d\n"), sessionTimer.GetInterval()));
            }

            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("\n  Administrative Connection Properties:\n")));
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("    Port                          : %d\n"), pServerManager->GetAdminPort()));
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("    Thread Pool Size              : %d\n"), pServerManager->GetAdminThreads()));
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("    Email Address                 : %W\n"), pServerManager->GetAdminEmail().c_str()));
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("\n  Client Connection Properties:\n")));
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("    Port                          : %d\n"), pServerManager->GetClientPort()));
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("    Thread Pool Size              : %d\n"), pServerManager->GetClientThreads()));
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("\n  Site Connection Properties:\n")));
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("    Port                          : %d\n"), pServerManager->GetSitePort()));
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("    Thread Pool Size              : %d\n"), pServerManager->GetSiteThreads()));
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("    IP Address                    : %s\n"), MG_WCHAR_TO_TCHAR(pServerManager->GetSiteServerAddress())));
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("\n  Access Log Properties:\n")));
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("    Enabled                       : %s\n"), pLogManager->IsAccessLogEnabled() == true ? ACE_TEXT("true") : ACE_TEXT("false")));
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("    Filename                      : %s\n"), MG_WCHAR_TO_TCHAR(pLogManager->GetAccessLogFileName())));
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("    Parameters                    : %s\n"), MG_WCHAR_TO_TCHAR(pLogManager->GetAccessLogParameters())));
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("\n  Admin Log Properties:\n")));
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("    Enabled                       : %s\n"), pLogManager->IsAdminLogEnabled() == true ? ACE_TEXT("true") : ACE_TEXT("false")));
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("    Filename                      : %s\n"), MG_WCHAR_TO_TCHAR(pLogManager->GetAdminLogFileName())));
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("    Parameters                    : %s\n"), MG_WCHAR_TO_TCHAR(pLogManager->GetAdminLogParameters())));
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("\n  Authentication Log Properties:\n")));
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("    Enabled                       : %s\n"), pLogManager->IsAuthenticationLogEnabled() == true ? ACE_TEXT("true") : ACE_TEXT("false")));
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("    Filename                      : %s\n"), MG_WCHAR_TO_TCHAR(pLogManager->GetAuthenticationLogFileName())));
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("    Parameters                    : %s\n"), MG_WCHAR_TO_TCHAR(pLogManager->GetAuthenticationLogParameters())));
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("\n  Error Log Properties:\n")));
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("    Enabled                       : %s\n"), pLogManager->IsErrorLogEnabled() == true ? ACE_TEXT("true") : ACE_TEXT("false")));
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("    Filename                      : %s\n"), MG_WCHAR_TO_TCHAR(pLogManager->GetErrorLogFileName())));
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("    Parameters                    : %s\n"), MG_WCHAR_TO_TCHAR(pLogManager->GetErrorLogParameters())));
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("\n  Session Log Properties:\n")));
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("    Enabled                       : %s\n"), pLogManager->IsSessionLogEnabled() == true ? ACE_TEXT("true") : ACE_TEXT("false")));
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("    Filename                      : %s\n"), MG_WCHAR_TO_TCHAR(pLogManager->GetSessionLogFileName())));
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("    Parameters                    : %s\n"), MG_WCHAR_TO_TCHAR(pLogManager->GetSessionLogParameters())));
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("\n  Trace Log Properties:\n")));
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("    Enabled                       : %s\n"), pLogManager->IsTraceLogEnabled() == true ? ACE_TEXT("true") : ACE_TEXT("false")));
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("    Filename                      : %s\n"), MG_WCHAR_TO_TCHAR(pLogManager->GetTraceLogFileName())));
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("    Parameters                    : %s\n"), MG_WCHAR_TO_TCHAR(pLogManager->GetTraceLogParameters())));
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("\n")));
#endif
        }
    }
    MG_CATCH(L"MgServer.open")

    if (mgException != NULL)
    {
        // Server will shutdown if we are here.

        MgServerManager* serverManager = MgServerManager::GetInstance();
        STRING locale = (NULL == serverManager) ?
            MgResources::DefaultMessageLocale : serverManager->GetDefaultMessageLocale();
        STRING message = mgException->GetMessage(locale);
        STRING details = mgException->GetDetails(locale);
        STRING stackTrace = mgException->GetStackTrace(locale);

        ACE_DEBUG((LM_ERROR, ACE_TEXT("(%t) %W\n"), details.c_str()));
        MG_LOG_SYSTEM_ENTRY(LM_ERROR, details.c_str());
        MG_LOG_EXCEPTION_ENTRY(message.c_str(), stackTrace.c_str());

        nResult = -1;
    }

    if(0 == nResult)
    {
        MgResources* pResources = MgResources::GetInstance();
        STRING message = pResources->FormatMessage(MgResources::ServerStarting, 0);

        ACE_DEBUG ((LM_INFO, ACE_TEXT("(%t) %W\n"), message.c_str()));
        MG_LOG_SYSTEM_ENTRY(LM_INFO, message.c_str());

        // Identify the coordinate system base library
        Ptr<MgCoordinateSystemFactory> csFactory = new MgCoordinateSystemFactory();
        message = csFactory->GetBaseLibrary();
        ACE_DEBUG ((LM_INFO, ACE_TEXT("(%t) %W\n"), message.c_str()));

        // Start the service execution thread
        nResult = svc();
    }

    MG_LOG_TRACE_ENTRY(L"MgServer::open() - End");
    return nResult;
}

#ifdef _WIN32
void MgServer::handle_control(DWORD controlCode)
{
    ACE_DEBUG ((LM_DEBUG, ACE_TEXT("(%t) MgServer::handle_control() - Code: %d\n"), controlCode));

    switch(controlCode)
    {
        case SERVICE_CONTROL_PAUSE:
            break;
        case SERVICE_CONTROL_CONTINUE:
            break;
        case SERVICE_CONTROL_INTERROGATE:
            break;
        // For a shutdown, we can do cleanup but it must take place quickly
        // because the system will go down out from under us.
        case SERVICE_CONTROL_SHUTDOWN:
        case SERVICE_CONTROL_STOP:
            stop_requested(controlCode);
            return;
        default:
            break;
    }
}

void MgServer::stop_requested (DWORD controlCode)
{
    ACE_UNUSED_ARG(controlCode);

    if(svc_handle_)
    {
        report_status(SERVICE_STOP_PENDING);
    }

    ACE_Reactor::instance()->end_reactor_event_loop();
}
#endif

STRING MgServer::LocaleCallback()
{
    STRING locale;

    MG_TRY()

    MgUserInformation* currUserInfo = MgUserInformation::GetCurrentUserInfo();

    if (NULL != currUserInfo)
    {
        locale = currUserInfo->GetLocale();
    }

    MG_CATCH_AND_THROW(L"MgServer.LocaleCallback")

    return locale;
}

#if defined (ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION)
template class ACE_LOCK_SOCK_Acceptor<ACE_SYNCH_MUTEX>;
#elif defined (ACE_HAS_TEMPLATE_INSTANTIATION_PRAGMA)
#pragma instantiate ACE_LOCK_SOCK_Acceptor<ACE_SYNCH_MUTEX>
#endif /* ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION */

//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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

#include "MapGuideCommon.h"
#include "Server.h"
#include "ServerManager.h"
#include "LogManager.h"
#include "ProductVersion.h"
#include <xercesc/util/PlatformUtils.hpp>

using namespace XERCES_CPP_NAMESPACE;

// for memory leak detection
#ifdef _WIN32
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

ACE_RCSID(server, main, "main.cpp")

#ifdef _WIN32
// Windows NT Service
ACE_NT_SERVICE_REFERENCE(MgServerService);
#endif

// Function prototypes;
void ShowCommandlineHelp();

#ifdef _WIN32
BOOL ConsoleHandler(DWORD ctrlType);
ACE_WCHAR_T* GetErrorMessage(unsigned long error);
DWORD InstallService(ACE_TCHAR* serviceDisplayName, ACE_TCHAR* serverPath);
DWORD UninstallService();
#endif

//////////////////////////////////////////////////////////////////
/// <summary>
/// </summary>
int ACE_TMAIN(INT32 argc, ACE_TCHAR *argv[])
{
    int nResult = 0;

#ifdef _WIN32
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    #ifdef _DEBUG
    // to debug memory leaks, set a breakpoint here and set iBlock
    // to the block allocation you want to break on
    long iBlock = -1;
    _CrtSetBreakAlloc(iBlock);
    #endif
#endif

    ACE::init();

    // we need initialize Xerces before BDB
    XMLPlatformUtils::Initialize();

    MG_TRY()

    {
        // We only want LM_DEBUG messages with the DEBUG build
        #ifdef _DEBUG
        ACE_LOG_MSG->priority_mask(LM_DEBUG | LM_INFO | LM_NOTICE | LM_WARNING | LM_STARTUP | LM_ERROR | LM_CRITICAL | LM_ALERT | LM_EMERGENCY, ACE_Log_Msg::PROCESS);
        #else
        ACE_LOG_MSG->priority_mask(LM_INFO | LM_NOTICE | LM_WARNING | LM_STARTUP | LM_ERROR | LM_CRITICAL | LM_ALERT | LM_EMERGENCY, ACE_Log_Msg::PROCESS);
        #endif

        bool bRunServerService = true;

#ifdef _WIN32
        // Get the handle to the Module that created this process
        wchar_t serverPath[_MAX_PATH];
        HINSTANCE hInstance = GetModuleHandle(NULL);
        GetModuleFileNameW(hInstance, serverPath, _MAX_PATH);

        // Set working directory
        wchar_t drive[_MAX_DRIVE];
        wchar_t dir[_MAX_DIR];
        _wsplitpath(serverPath, drive, dir, NULL, NULL);

        wchar_t workingPath[_MAX_PATH];
        _wmakepath(workingPath, drive, dir, NULL, NULL);
        _wchdir(workingPath);
#else
        // Linux
#endif

        // Set default service display name
        ACE_TCHAR serviceDisplayName[255];
        ACE_OS::strcpy(serviceDisplayName, MG_WCHAR_TO_TCHAR(MgResources::ServerServiceDisplayName));

#ifdef _WIN32
        // Set the default service name and the display name
        SERVER::instance()->name(MG_WCHAR_TO_TCHAR(MgResources::ServerServiceName), serviceDisplayName);
#endif


#ifndef _WIN32
        // set the precision to match what is used in Windows
        unsigned int mode = 0x27F;
        asm("fldcw %0" : : "m" (*&mode));
#endif

        if(argc > 1)
        {
            if(argc == 3)
            {
                ACE_OS::strcpy(serviceDisplayName, argv[2]);

#ifdef _WIN32
                // Update the service display name
                SERVER::instance()->name(MG_WCHAR_TO_TCHAR(MgResources::ServerServiceName), serviceDisplayName);
#endif
            }

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

            // There are command line arguments
            if((ACE_OS::strcasecmp(parameter, ACE_TEXT("?")) == 0) || (ACE_OS::strcasecmp(parameter, MG_WCHAR_TO_TCHAR(MgResources::ServerCmdHelp)) == 0))
            {
                // Display the server command line options
                ShowCommandlineHelp();

                // Don't try and run it
                bRunServerService = false;
            }
#ifdef _WIN32
            else if(ACE_OS::strcasecmp(parameter, MG_WCHAR_TO_TCHAR(MgResources::ServerCmdInstall)) == 0)
            {
                // Install the server service. Automatically start it
                ACE_OS::printf(MG_WCHAR_TO_CHAR(MgResources::ServerCmdInstallInfo), MG_WCHAR_TO_CHAR(MgResources::ServerServiceName), MG_WCHAR_TO_CHAR(serviceDisplayName));

                // Don't try and run it because it will be automatically be started for us
                bRunServerService = false;

                // Windows service install
                DWORD dwError = InstallService(serviceDisplayName, (ACE_TCHAR*) MG_WCHAR_TO_TCHAR(serverPath));
                if(ERROR_SUCCESS != dwError)
                {
                    ACE_WCHAR_T* message = GetErrorMessage(dwError);

                    ACE_OS::printf(MG_WCHAR_TO_CHAR(MgResources::ServerCmdInstallFailed), MG_WCHAR_TO_CHAR(MgResources::ServerServiceName), MG_WCHAR_TO_CHAR(message));

                    delete [] message;
                    message = NULL;
                }
            }
            else if(ACE_OS::strcasecmp(parameter, MG_WCHAR_TO_TCHAR(MgResources::ServerCmdUninstall)) == 0)
            {
                // Uninstall the server service
                ACE_OS::printf(MG_WCHAR_TO_CHAR(MgResources::ServerCmdUninstallInfo), MG_WCHAR_TO_CHAR(MgResources::ServerServiceName));

                // Server service is being uninstalled, so don't try and run it
                bRunServerService = false;

                // Windows service uninstall
                DWORD dwError = UninstallService();
                if(ERROR_SUCCESS != dwError)
                {
                    ACE_WCHAR_T* message = GetErrorMessage(dwError);

                    ACE_OS::printf(MG_WCHAR_TO_CHAR(MgResources::ServerCmdUninstallFailed), MG_WCHAR_TO_CHAR(MgResources::ServerServiceName), MG_WCHAR_TO_CHAR(message));

                    delete [] message;
                    message = NULL;
                }
            }
            else if(ACE_OS::strcasecmp(parameter, MG_WCHAR_TO_TCHAR(MgResources::ServerCmdStart)) == 0)
            {
                // Start the server service
                ACE_OS::printf(MG_WCHAR_TO_CHAR(MgResources::ServerCmdStartInfo));

                SERVER::instance()->start_svc();
                bRunServerService = false;
            }
            else if(ACE_OS::strcasecmp(parameter, MG_WCHAR_TO_TCHAR(MgResources::ServerCmdStop)) == 0)
            {
                // Stop the server service
                ACE_OS::printf(MG_WCHAR_TO_CHAR(MgResources::ServerCmdStopInfo));

                SERVER::instance()->stop_svc();
                bRunServerService = false;
            }
#endif
            else if((ACE_OS::strcasecmp(parameter, MG_WCHAR_TO_TCHAR(MgResources::ServerCmdTest)) == 0) ||
                    (ACE_OS::strcasecmp(parameter, MG_WCHAR_TO_TCHAR(MgResources::ServerCmdTestMode)) == 0))
            {
                // Run the server unit tests
                ACE_OS::printf(MG_WCHAR_TO_CHAR(MgResources::ServerCmdTestInfo));

                // Run the server as a regular application
                nResult = SERVER::instance()->init(argc, argv);
                if(0 == nResult)
                {
                    nResult = SERVER::instance()->open();
                    SERVER::instance()->fini();
                }

                bRunServerService = false;
            }
#ifndef _WIN32  // Linux only Daemon mode
            else if(ACE_OS::strcasecmp(parameter, MG_WCHAR_TO_TCHAR(MgResources::ServerCmdDaemon)) == 0)
            {
                bRunServerService = true;
                ACE_OS::printf(MG_WCHAR_TO_CHAR(MgResources::ServerCmdDaemonInfo));
            }
#endif
            else if((ACE_OS::strcasecmp(parameter, MG_WCHAR_TO_TCHAR(MgResources::ServerCmdRun)) == 0) ||
                    (ACE_OS::strcasecmp(parameter, MG_WCHAR_TO_TCHAR(MgResources::ServerCmdInteractive)) == 0))
            {
                ACE_OS::printf(MG_WCHAR_TO_CHAR(MgResources::ServerCmdRunInfo));

#ifdef _WIN32
                HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
                SetConsoleMode(hStdin, ENABLE_PROCESSED_INPUT);
                SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleHandler, TRUE);
#endif

                // Run the server as a regular application
                nResult = SERVER::instance()->init(argc, argv);
                if(0 == nResult)
                {
                    nResult = SERVER::instance()->open();
                    SERVER::instance()->fini();
                }

                bRunServerService = false;
            }
            else if(ACE_OS::strcasecmp(parameter, MG_WCHAR_TO_TCHAR(MgResources::ServerCmdTestFdo)) == 0)
            {
                // Run the fdo unit tests
                ACE_OS::printf(MG_WCHAR_TO_CHAR(MgResources::ServerCmdTestFdoInfo));

                // Run the server as a regular application
                nResult = SERVER::instance()->init(argc, argv);
                if(0 == nResult)
                {
                    nResult = SERVER::instance()->open();
                    SERVER::instance()->fini();
                }

                bRunServerService = false;
            }
            else
            {
                // Unrecognized command line option
#ifdef _WIN32
                ACE_OS::printf(MG_WCHAR_TO_CHAR(MgResources::ServerCmdUnrecognizedInfo), MG_WCHAR_TO_CHAR(parameter));
#else
                ACE_OS::printf(MG_WCHAR_TO_CHAR(MgResources::ServerCmdUnrecognizedInfo), MG_TCHAR_TO_CHAR(parameter));
#endif
                ShowCommandlineHelp();

                // Don't try and run it
                bRunServerService = false;
            }

            // Free resources
            delete [] parameter;
            parameter = NULL;
        }

        if(bRunServerService)
        {
#ifdef _WIN32
            // The SCM is trying to start the server
            ACE_NT_SERVICE_RUN(MgServerService, SERVER::instance(), nReturn);
            if(0 == nReturn)
            {
                DWORD dwError = GetLastError();
                ACE_WCHAR_T* message = GetErrorMessage(dwError);

                ACE_OS::printf(MG_WCHAR_TO_CHAR(MgResources::ServerServiceStartFailure), dwError, MG_WCHAR_TO_CHAR(message), MG_WCHAR_TO_CHAR(MgResources::ServerCmdRun));

                delete [] message;
                message = NULL;
            }
#else
            // Linux Daemon code
            if (ACE::daemonize(ACE_TEXT("")) == 0)
            {
                // Open Stdin
                int nOpen = ACE_OS::open("/dev/null",O_RDWR);
                // stdout and stderr go to the same device as stdin.
                ACE_OS::dup(nOpen);
                ACE_OS::dup(nOpen);

                int nlock = ACE_OS::open("/var/lock/mgserver/mgserverd.lock",O_RDWR|O_CREAT);
                // If file cannot be opened exit.
                if (nlock < 0)
                    exit(1);

                // If file is locked exit
                if (lockf(nlock,F_TLOCK,0)  < 0)
                    exit(0);

                ACE_TCHAR strPID[20];

                // convert pid to string
                ACE_OS::sprintf(strPID,"%d\n",ACE_OS::getpid());

                //record pid to lockfile
                ACE_OS::write(nlock,strPID,ACE_OS::strlen(strPID));

                INT32 nResult = SERVER::instance()->init(argc, argv);

                if (nResult == 0)
                {
                    nResult = SERVER::instance()->open();
                    SERVER::instance()->fini();
                }
            }
#endif
        }
    }

    MG_CATCH(L"main()")

    if (mgException != NULL)
    {
        MgServerManager* serverManager = MgServerManager::GetInstance();
        STRING locale = (NULL == serverManager) ?
            MgResources::DefaultMessageLocale : serverManager->GetDefaultMessageLocale();
        STRING details = mgException->GetDetails(locale);

        ACE_DEBUG((LM_ERROR, ACE_TEXT("(%t) %W\n"), details.c_str()));
        MG_LOG_SYSTEM_ENTRY(LM_ERROR, details.c_str());
        MG_LOG_ERROR_ENTRY(details.c_str());

        nResult = -1;
    }

    XMLPlatformUtils::Terminate();

    ACE::fini();

    return nResult;
}

void ShowCommandlineHelp()
{
    // This shows the supported commnad line options for the various platforms
    ACE_OS::printf(MG_WCHAR_TO_CHAR(MgResources::ServerCmdHelpInfo1), MG_WCHAR_TO_CHAR(MgResources::ServerServiceDisplayName), MG_WCHAR_TO_CHAR(ProductVersion));

    // The commands are shown in alphabetical order

#ifndef _WIN32
    ACE_OS::printf(MG_WCHAR_TO_CHAR(MgResources::ServerCmdDaemonDescription));
#endif

    ACE_OS::printf(MG_WCHAR_TO_CHAR(MgResources::ServerCmdHelpDescription));

#ifdef _WIN32
    // Windows only commands
    ACE_OS::printf(MG_WCHAR_TO_CHAR(MgResources::ServerCmdInstallDescription), MG_WCHAR_TO_CHAR(MgResources::ServerServiceDisplayName));
#endif

    ACE_OS::printf(MG_WCHAR_TO_CHAR(MgResources::ServerCmdRunDescription));

#ifdef _WIN32
    // Windows only commands
    ACE_OS::printf(MG_WCHAR_TO_CHAR(MgResources::ServerCmdStartDescription));
    ACE_OS::printf(MG_WCHAR_TO_CHAR(MgResources::ServerCmdStopDescription));
#endif

    // Add the supported commands
    ACE_OS::printf(MG_WCHAR_TO_CHAR(MgResources::ServerCmdTestFdoDescription));
    ACE_OS::printf(MG_WCHAR_TO_CHAR(MgResources::ServerCmdTestDescription));

#ifdef _WIN32
    // Windows only commands
    ACE_OS::printf(MG_WCHAR_TO_CHAR(MgResources::ServerCmdUninstallDescription), MG_WCHAR_TO_CHAR(MgResources::ServerServiceDisplayName));
#endif

    ACE_OS::printf(MG_WCHAR_TO_CHAR(MgResources::ServerCmdHelpInfo2));
}

#ifdef _WIN32
ACE_WCHAR_T* GetErrorMessage(unsigned long error)
{
    ACE_WCHAR_T* message = new ACE_WCHAR_T[255];
    if(message)
    {
        if (FormatMessage(
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            error,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
            message,
            255,
            NULL ) == 0)
        {
            // Failed to get message.
            ACE_OS::sprintf(message, MgResources::ServerErrorCodeMessage.c_str(), error);
        }
    }

    return message;
}

BOOL ConsoleHandler(DWORD ctrlType)
{
    BOOL bResult = FALSE;

    switch( ctrlType )
    {
        case CTRL_C_EVENT:
            SERVER::instance ()->handle_control(SERVICE_CONTROL_STOP);
            bResult = TRUE;
            break;
        case CTRL_CLOSE_EVENT:
            SERVER::instance ()->handle_control(SERVICE_CONTROL_STOP);
            bResult = TRUE;
            break;
        case CTRL_BREAK_EVENT:
            SERVER::instance ()->handle_control(SERVICE_CONTROL_STOP);
            bResult = FALSE;
            break;
        case CTRL_LOGOFF_EVENT:
            bResult = FALSE;
            break;
        case CTRL_SHUTDOWN_EVENT:
            SERVER::instance ()->handle_control(SERVICE_CONTROL_STOP);
            bResult = FALSE;
            break;
        default:
            bResult = FALSE;
    }

    return bResult;
}

DWORD InstallService(ACE_TCHAR* serviceDisplayName, ACE_TCHAR* serverPath)
{
    DWORD dwError = ERROR_SUCCESS;

    if((serviceDisplayName) && (serverPath))
    {
        SC_HANDLE scm = ACE_TEXT_OpenSCManager(0, 0, SC_MANAGER_CREATE_SERVICE);
        if(scm)
        {
            SC_HANDLE sh = ACE_TEXT_CreateService(scm, MG_WCHAR_TO_TCHAR(MgResources::ServerServiceName), serviceDisplayName, SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_START, SERVICE_ERROR_NORMAL, serverPath, 0, 0, 0, 0, 0);
            if(sh)
            {
                // Set the description
                ACE_WCHAR_T serviceDescription[255];
                ACE_OS::strcpy(serviceDescription, MG_WCHAR_TO_TCHAR(MgResources::ServerServiceDescription));

                SERVICE_DESCRIPTION description;
                description.lpDescription = serviceDescription;

                if(ChangeServiceConfig2(sh, SERVICE_CONFIG_DESCRIPTION, &description))
                {
                    // Attempt to start the service
                    if(StartService(sh, 0, NULL) == 0)
                    {
                        dwError = GetLastError();
                    }
                }
                else
                {
                    // Failed to change the service configuration
                    dwError = GetLastError();
                }
            }
            else
            {
                // Failed to create the service
                dwError = GetLastError();
            }

            CloseServiceHandle(sh);
        }
        else
        {
            // Failed to open the service manager
            dwError = GetLastError();
        }

        CloseServiceHandle(scm);
    }
    else
    {
        dwError = ERROR_INVALID_PARAMETER;
    }

    return dwError;
}

DWORD UninstallService()
{
    DWORD dwError = ERROR_SUCCESS;

    SC_HANDLE scm = ACE_TEXT_OpenSCManager(0, 0, SC_MANAGER_ALL_ACCESS);
    if(scm)
    {
        SC_HANDLE sh = ACE_TEXT_OpenService(scm, MG_WCHAR_TO_TCHAR(MgResources::ServerServiceName), SERVICE_ALL_ACCESS);
        if(sh)
        {
            // See if the service is running
            SERVICE_STATUS serviceStatus;
            if(QueryServiceStatus(sh, &serviceStatus))
            {
                // If the service is not stopped, we need to stop it
                if(SERVICE_STOPPED != serviceStatus.dwCurrentState)
                {
                    // Need to stop the service before deleting it
                    if(ControlService(sh, SERVICE_CONTROL_STOP, &serviceStatus) == 0)
                    {
                        // Failed to stop the service
                        dwError = GetLastError();
                    }
                }

                // Delete the service
                if(DeleteService(sh) == 0)
                {
                    // Failed to delete the service
                    dwError = GetLastError();
                }
            }
            else
            {
                // Failed to stop the service
                dwError = GetLastError();
            }
        }
        else
        {
            // Failed to open the service
            dwError = GetLastError();
        }

        CloseServiceHandle(sh);
    }
    else
    {
        // Failed to open the service manager
        dwError = GetLastError();
    }

    CloseServiceHandle(scm);

    return dwError;
}
#endif

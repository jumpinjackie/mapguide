//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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

#include "AceCommon.h"
#include "HttpHandler.h"
#ifdef WIN32
#include "fcgi_config_x86.h"
#else
#include "fcgi_config.h"
#endif

#include <stdlib.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef _WIN32
#include <process.h>
#include <io.h>
#else
#include <time.h>
extern char **environ;
#endif

#include "fcgi_stdio.h"
#include "CgiGetParser.h"
#include "CgiPostParser.h"
#include "CgiResponseHandler.h"
#include "CgiStrings.h"
#include "WebSupport.h"
#include "Base64.h"


// Debugging utilities, useful for dumping what was received
// back over the wire.  These were taken from FastCgi example
// sources.
extern int EchoTest();
extern void PrintEnv(char *label, char **envp);
void DumpMessage(const char* format, ...);
void DumpMessage2(const char* msg);

bool ParseAuth(char* AuthString, MgHttpRequestParam* params);
bool AuthenticateOgcRequest(MgHttpRequestParam* params);

// Forward declare Web Tier initialization routine
void Initialize();
STRING gConfigPath;

// Main execution loop.  This loop executes correctly when run both as a
// standard Cgi app and when running as a FastCgi process.  This loop is
// single threaded.  Multiple FastCgi processes are started to handle
// concurrent incoming requests.
int main ()
{
    bool bFirstTime = true;
    bool bShouldContinue = true;
    int nRequests = 1;
    INT32 dbgPause = 0;

    CgiPostParser postParser;

    while (bShouldContinue && FCGI_Accept() >= 0)
    {
        MG_TRY()

            // If Web Tier generates an exception which is not caught
            // by the HTTP handler execute, we should exit to reduce
            // the likelihood of a hung agent lying around.
            bShouldContinue = false;

            if (bFirstTime)
            {
                bFirstTime = false;
#ifdef _WIN32
                // Set stdin and stdout to binary mode
                _setmode(0, _O_BINARY);
                _setmode(1, _O_BINARY);
#else
                //TODO: Does this need to be done under Linux?
#endif
                //Debugging code
                //EchoTest();
                //FCGI_Finish();
                //break;

                Initialize();
#ifdef _DEBUG
                MgConfiguration* cfg = MgConfiguration::GetInstance();
                cfg->GetIntValue(MgConfigProperties::AgentPropertiesSection, MgConfigProperties::AgentDebugPause, dbgPause, MgConfigProperties::DefaultAgentDebugPause);
#endif

                DumpMessage("Initialized process %d",getpid());
            }

            DumpMessage("Process %d servicing request #%d",getpid(), nRequests);

            // Construct self Url.  It is embedded into the output stream
            // of some requests (like GetMap).  Use a fully qualified URL.
            // TODO: Do we need to worry about https:// here?
            char* serverName = getenv(CgiStrings::ServerName);
            char* serverPort = getenv(CgiStrings::ServerPort);
            char* scriptName = getenv(CgiStrings::ScriptName);
            string url = CgiStrings::Http;
            if (NULL != serverName && NULL != serverPort && NULL != scriptName)
            {
                url.append(serverName);
                url += ':';
                url.append(serverPort);
                url.append(scriptName);
            }
            STRING wUrl = MgUtil::MultiByteToWideChar(url);
            Ptr<MgHttpRequest> request = new MgHttpRequest(wUrl);
            Ptr<MgHttpRequestParam> params = request->GetRequestParam();

            // Temporarily moved here to allow debugging post.
#ifdef _DEBUG
                if(dbgPause > 0)
                {
                    DumpMessage("Delaying request %d seconds", dbgPause);
#ifdef WIN32
                    Sleep(dbgPause * 1000L);
#else
                    struct timespec delay = { (time_t)dbgPause, 0 };
                    nanosleep(&delay, NULL);
#endif
                }
#endif

                // Parse inputs GET/POST
            char* query = getenv(CgiStrings::QueryString);
            char* requestMethod = getenv(CgiStrings::RequestMethod);
            if (NULL != requestMethod && NULL != strstr(requestMethod, "POST"))
            {
                // Must be a POST request
                postParser.Parse(params);
            }
            else if (NULL != query && strlen(query) > 0)
            {
                DumpMessage2("Query string present");
                DumpMessage2(query);

                // If we have a QUERY_STRING then we are a GET request
                CgiGetParser::Parse(query, params);
            }

            // Check for HTTP Basic Auth header
            char* auth = getenv(CgiStrings::HttpAuth);
            bool gotAuth = ParseAuth(auth, params);

            if (!gotAuth)
            {
                // And check for a REMOTE_USER remapped header
                auth = getenv(CgiStrings::HttpRemoteUser);
                gotAuth = ParseAuth(auth, params);
            }

            Ptr<MgPropertyCollection> paramList = params->GetParameters()->GetPropertyCollection();
            if (paramList != NULL)
            {
                //Check to be sure that we have some kind of credentials before continuing.  Either
                //username/password or sessionid.
                bool bValid = paramList->Contains(MgHttpResourceStrings::reqSession);
                // Strike two: no session? how about a username?
                if (!bValid)
                    bValid = paramList->Contains(MgHttpResourceStrings::reqUsername);
                // Strike three: no username either? How about if it's an XML POST
                if (!bValid)
                    bValid = params->GetXmlPostData().length() != 0;

                if (!bValid)
                {
                    // Invalid authentication information is not fatal, we should continue.
                    bShouldContinue = true;
                    CgiResponseHandler::RequestAuth();
                    FCGI_Finish();
                    continue;
                }

                Ptr<MgHttpResponse> response = request->Execute();

                // We got through the Execute.  It should be safe to continue.
                bShouldContinue = true;

                //Clean up temporary files.  Assume anything with a type is file.
                Ptr<MgStringCollection> paramNames = params->GetParameterNames();
                if (paramNames != NULL)
                {
                    for (int n = 0; n < paramNames->GetCount(); n++)
                    {
                        STRING name = paramNames->GetItem(n);
                        if (params->GetParameterType(name).length() > 0)
                        {
                            STRING fileName = params->GetParameterValue(name);
                            MgFileUtil::DeleteFile(fileName);
                        }
                    }
                }

                CgiResponseHandler::SendResponse(response);
            }

        MG_CATCH(L"FastCgiAgent.main");

        if (mgException != NULL)
        {
            CgiResponseHandler::SendError(mgException);
        }

        DumpMessage("Process %d serviced request #%d, continue %d",getpid(), nRequests++, bShouldContinue);

        FCGI_Finish();
    }

    DumpMessage("Exiting process %d",getpid());

    return 0;
}

bool ParseAuth(char* auth, MgHttpRequestParam* params)
{
    bool bGotAuth = false;

    //Bypass the standard authentication handling for WMS and WFS requests.
    //For these requests, use predefined credentials
    STRING op = params->GetParameterValue(MgHttpResourceStrings::reqOperation);
    if(op.length() == 0) {
        STRING requestValue = params->GetParameterValue(MgHttpResourceStrings::reqWmsRequest);
        if(requestValue.length() != 0)
            return AuthenticateOgcRequest(params);
    }

    const char* basic = CgiStrings::BasicAuth;
    if (NULL != auth && NULL != strstr(auth, basic))
    {
        char* base64 = strstr(auth, basic) + strlen(basic);
        unsigned long origLen = (unsigned long) strlen(base64);
        unsigned long len = Base64::GetDecodedLength(origLen);
        if (len < 128)
        {
            char buf[128];
            memset(buf, 0, 128);
            Base64::Decode((unsigned char*)buf, base64, origLen);
            char* split = strchr(buf, ':'); // NOXLATE
            if (NULL != split)
            {
                *split++ = '\0';
                string username = buf;
                string password = split;
                params->AddParameter(MgHttpResourceStrings::reqUsername,
                    MgUtil::MultiByteToWideChar(username));
                params->AddParameter(MgHttpResourceStrings::reqPassword,
                    MgUtil::MultiByteToWideChar(password));
                bGotAuth = true;
            }
        }
    }
    return bGotAuth;
}

bool AuthenticateOgcRequest(MgHttpRequestParam* params)
{
    bool isWms = true;

    // Determine with the optional service parameter if the request is WMS or WFS
    // If the service is present and specifies something else than WMS or WFS, refuse
    // authentication as MapGuide only supports these 2 services.
    STRING serviceValue = MgUtil::ToUpper(params->GetParameterValue(MgHttpResourceStrings::reqWmsService));
    if(serviceValue.length() != 0) {
        if(serviceValue == L"WFS")
            isWms = false;
        else if(serviceValue != L"WMS")
            return false;
    }

    // Get WMS/WFS password from configuration.
    STRING username, password;
    MgConfiguration* cfg = MgConfiguration::GetInstance();

    username = isWms ? MgUser::WmsUser : MgUser::WfsUser;
    cfg->GetStringValue(MgConfigProperties::OgcPropertiesSection, isWms? MgConfigProperties::WmsPassword: MgConfigProperties::WfsPassword, password, L"");

    //feed these values in as parameters
    params->AddParameter(MgHttpResourceStrings::reqUsername, username);
    params->AddParameter(MgHttpResourceStrings::reqPassword, password);

    return true;
}

void Initialize()
{
    char* path = getenv(CgiStrings::PhysicalPath);
    if (NULL != path && strlen(path) > 0)
    {
        gConfigPath = MgUtil::MultiByteToWideChar(string(path));
    }
    else
    {
#if defined _WIN32
        // IIS 6.0 does not honour APPL_PHYSICAL_PATH.  Use exe path.
        wchar_t modPath[_MAX_PATH+1];
        memset(modPath, 0, sizeof(wchar_t)*(_MAX_PATH+1));
        if (GetModuleFileName(NULL, modPath, _MAX_PATH) > 4)
        {
            wchar_t* lastSlash = wcsrchr(modPath, L'\\');
            if (NULL == lastSlash)
            {
                lastSlash = wcsrchr(modPath, L'/');
            }
            if (NULL != lastSlash)
            {
                lastSlash++;
                *lastSlash = L'\0';
                if (NULL != wcsstr(modPath, L"\\\\?\\"))
                {
                    // skip past '\\?\' at front of string.
                    gConfigPath = &(modPath[4]);
                }
                else
                {
                    gConfigPath = modPath;
                }
            }
        }
#endif

    }
    STRING configFile = gConfigPath;
    configFile.append(CgiStrings::WebConfig);

    string pathName = MgUtil::WideCharToMultiByte(configFile);
    DumpMessage("Loading configuration file: %s",pathName.c_str());

    MG_TRY()

    MgInitializeWebTier(configFile);

    MG_CATCH_AND_THROW(L"FastCgiAgent.Initialize");
}

void DumpMessage(const char* format, ...)
{
#if defined(_DEBUG) && defined(WIN32)
    va_list args;

    // retrieve the variable arguments
    va_start( args, format );

    if (NULL != format)
    {
        string logFile = MgUtil::WideCharToMultiByte(gConfigPath);
        logFile.append("mapAgent.log");

        FILE* fp = fopen(logFile.c_str(), "a+");
        if (fp)
        {
            vfprintf(fp, format, args);
            fprintf(fp, "\n");
            fclose(fp);
        }
    }
#endif
}

void DumpMessage2(const char* msg)
{
#if defined(_DEBUG) && defined(WIN32)
    if (NULL != msg)
    {
        string logFile = MgUtil::WideCharToMultiByte(gConfigPath);
        logFile.append("mapAgent.log");

        FILE* fp = fopen(logFile.c_str(), "a+");
        if (fp)
        {
            fputs(msg, fp);
            fputs("\n", fp);
            fclose(fp);
        }
    }
#endif
}

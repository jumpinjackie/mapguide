//
//  Copyright (C) 2004-2011 by Autodesk, Inc.
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
//

#include "MapGuideCommon.h"
#include "HttpHandler.h"
#include "stdafx.h"
#include "httpext.h"
#include "stdio.h"
#include "WebSupport.h"
#include "IsapiPostParser.h"
#include "Base64.h"
#include "IsapiResponseHandler.h"
#include "MapAgentCommon.h"
#include "MapAgentGetParser.h"
#include "MapAgentStrings.h"


void Initialize(EXTENSION_CONTROL_BLOCK *pECB);
STRING gConfigPath;

BOOL APIENTRY DllMain(HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    return TRUE;
}


BOOL WINAPI GetExtensionVersion(HSE_VERSION_INFO *pVer)
{
    pVer->dwExtensionVersion = HSE_VERSION;
    strncpy(pVer->lpszExtensionDesc, "MapAgent ISAPI Extension", HSE_MAX_EXT_DLL_NAME_LEN);  // NOXLATE

    return TRUE;
}


DWORD WINAPI HttpExtensionProc(EXTENSION_CONTROL_BLOCK *pECB)
{
    IsapiResponseHandler responseHandler(pECB);

    MG_TRY()

    Initialize(pECB);

    // Construct self Url.  It is embedded into the output stream
    // of some requests (like GetMap).  Use a fully qualified URL.
    //
    // If the GetServerVariable method does not find the specified variable, it returns false,
    // and the LPSTR parameter is NOT set to NULL, but remains uninitialized.
    // So checking for NULL is not sufficient.
    DWORD size = 1024;
    char serverName[1024];
    BOOL bServerName = pECB->GetServerVariable(pECB->ConnID, (LPSTR)MapAgentStrings::ServerName, serverName, &size);

    size = 1024;
    char serverPort[1024];
    BOOL bServerPort = pECB->GetServerVariable(pECB->ConnID, (LPSTR)MapAgentStrings::ServerPort, serverPort, &size);

    size = 1024;
    char scriptName[1024];
    BOOL bScriptName = pECB->GetServerVariable(pECB->ConnID, (LPSTR)MapAgentStrings::ScriptName, scriptName, &size);

    size = 1024;
    char remoteAddr[1024];
    BOOL bRemoteAddr = pECB->GetServerVariable(pECB->ConnID, (LPSTR)MapAgentStrings::RemoteAddr, remoteAddr, &size);

    size = 1024;
    char httpClientIp[1024];
    BOOL bHttpClientIp = pECB->GetServerVariable(pECB->ConnID, (LPSTR)MapAgentStrings::HttpClientIp, httpClientIp, &size);

    size = 1024;
    char httpXFF[1024];
    BOOL bHttpXFF = pECB->GetServerVariable(pECB->ConnID, (LPSTR)MapAgentStrings::HttpXForwardedFor, httpXFF, &size);

    size = 1024;
    char secure[1024];
    BOOL bSecure = pECB->GetServerVariable(pECB->ConnID, (LPSTR)MapAgentStrings::Secure, secure, &size);

    bool isSecure = (bSecure && secure != NULL && !stricmp(secure, "on"));  // NOXLATE

    string url = isSecure? MapAgentStrings::Https : MapAgentStrings::Http;
    if (bServerName && bServerPort && bScriptName
        && NULL != serverName && NULL != serverPort && NULL != scriptName)
    {
        url.append(serverName);
        url += ':';
        url.append(serverPort);
        url.append(scriptName);
    }
    STRING wUrl = MgUtil::MultiByteToWideChar(url);
    Ptr<MgHttpRequest> request = new MgHttpRequest(wUrl);
    Ptr<MgHttpRequestParam> params = request->GetRequestParam();

    // Parse inputs GET/POST
    char* query = pECB->lpszQueryString;
    char* requestMethod = pECB->lpszMethod;

    if (NULL != requestMethod && NULL != strstr(requestMethod, "POST"))  // NOXLATE
    {
        // Must be a POST request
        IsapiPostParser postParser(pECB);
        postParser.Parse(params);
    }
    else if (NULL != query && strlen(query) > 0)
    {
        // If we have a QUERY_STRING then we are a GET request
        MapAgentGetParser::Parse(query, params);
    }

    // check for CLIENTIP, if it's not there (and it shouldn't be),
    // add it in using httpClientIp. httpXFF or remoteAddr
    STRING clientIp = L"";
    if (!params->ContainsParameter(L"CLIENTIP")) // NOXLATE
    {
        if (bHttpClientIp && NULL != httpClientIp && strlen(httpClientIp) > 0
            && _stricmp(httpClientIp, MapAgentStrings::Unknown) != 0)
        {
            clientIp = MgUtil::MultiByteToWideChar(httpClientIp);
            params->AddParameter(L"CLIENTIP", clientIp); // NOXLATE
        }
        else if (bHttpXFF && NULL != httpXFF && strlen(httpXFF) > 0
            && _stricmp(httpXFF, MapAgentStrings::Unknown) != 0)
        {
            clientIp = MgUtil::MultiByteToWideChar(httpXFF);
            params->AddParameter(L"CLIENTIP", clientIp); // NOXLATE
        }
        else if (bRemoteAddr && NULL != remoteAddr && strlen(remoteAddr) > 0)
        {
            clientIp = MgUtil::MultiByteToWideChar(remoteAddr);
            params->AddParameter(L"CLIENTIP", clientIp); // NOXLATE
        }
    }

    // Check for HTTP Basic Auth header
    size = 1024;
    char auth[1024];
    BOOL bAuth = pECB->GetServerVariable(pECB->ConnID, (LPSTR)MapAgentStrings::HttpAuth, auth, &size);
    bool gotAuth = bAuth && MapAgentCommon::ParseAuth(auth, params);

    if (!gotAuth)
    {
        // And check for a REMOTE_USER remapped header
        size = 1024;
        pECB->GetServerVariable(pECB->ConnID, (LPSTR)MapAgentStrings::HttpRemoteUser, auth, &size);
        gotAuth = MapAgentCommon::ParseAuth(auth, params);
    }

    // Log request information
    string postData = "";
    if (requestMethod && _stricmp(requestMethod, "POST") == 0)  // NOXLATE
    {
        // Get the post xml data
        postData = params->GetXmlPostData();
    }

    STRING client = params->GetParameterValue(MgHttpResourceStrings::reqClientAgent);
    MapAgentCommon::LogRequest(client, clientIp, url, std::string(requestMethod), postData, std::string(query));

    Ptr<MgPropertyCollection> paramList = params->GetParameters()->GetPropertyCollection();
    if (paramList != NULL)
    {
        // Check to be sure that we have some kind of credentials before continuing.  Either
        // username/password or sessionid.
        bool bValid = paramList->Contains(MgHttpResourceStrings::reqSession);
        // Strike two: no session? how about a username?
        if (!bValid)
            bValid = paramList->Contains(MgHttpResourceStrings::reqUsername);
        // Strike three: no username either? How about if it's an XML POST
        if (!bValid)
            bValid = params->GetXmlPostData().length() != 0;

        // Certain operations do not require authentication
        STRING operation = params->GetParameterValue(L"OPERATION");
        if((_wcsicmp(operation.c_str(), L"GETSITESTATUS") == 0))
        {
            bValid = true;
        }

        if (!bValid)
        {
            // Invalid authentication information is not fatal, we should continue.
            responseHandler.RequestAuth();

            // clean up any temporary files we created
            MapAgentCommon::DeleteTempFiles(params);

            return HSE_STATUS_SUCCESS;
        }

        Ptr<MgHttpResponse> response = request->Execute();

        // NOTE: temporary files are deleted when we execute the request

        responseHandler.SendResponse(response);
    }

    MG_CATCH(L"IsapiAgent.HttpExtensionProc");

    if (mgException != NULL)
    {
        responseHandler.SendError(mgException);
    }

    return HSE_STATUS_SUCCESS;
}


BOOL WINAPI TerminateExtension(DWORD dwFlags)
{
    MgUninitializeWebTier();

    return TRUE;
}


void Initialize(EXTENSION_CONTROL_BLOCK *pECB)
{
    if (!IsWebTierInitialized())
    {
        DWORD dwSize = 1024;
        char path[1024];
        pECB->GetServerVariable(pECB->ConnID, (LPSTR)MapAgentStrings::PhysicalPath, path, &dwSize);

        if (strlen(path) > 0)
        {
            gConfigPath = MgUtil::MultiByteToWideChar(string(path));
        }

        STRING configFile = gConfigPath;
        configFile.append(L"\\");
        configFile.append(MapAgentStrings::WebConfig);

        MG_TRY()

        MgInitializeWebTier(configFile);

        MG_CATCH_AND_THROW(L"IsapiAgent.Initialize");
    }
}

//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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

#include "httpd.h"
#include "http_config.h"
#include "ap_config.h"
#include "http_log.h"
#include "http_protocol.h"
#include "http_main.h"
#include "util_script.h"
#include "http_core.h"
#include "apr_strings.h"
#include "apr_tables.h"

#ifdef strtoul
#undef strtoul
#endif
#define strtoul strtoul

#ifndef _WIN32
#ifdef REG_NOMATCH
#undef REG_NOMATCH
#endif
#endif

#include "WebSupport.h"
#include "HttpHandler.h"
#include "MapAgentGetParser.h"
#include "MapAgentStrings.h"
#include "MapAgentCommon.h"
#include "ApacheResponseHandler.h"
#include "ApachePostParser.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

extern "C" module AP_MODULE_DECLARE_DATA mgmapagent_module;

void Initialize(request_rec *r);
STRING gConfigPath;

// Iterate through the values in an apr_table.
// Used only during debugging/development to identify the available environment variables.
int iterate_func(void *req, const char *key, const char *value) {
    int stat;
    char *line;
    request_rec *r = (request_rec *)req;
    if (key == NULL || value == NULL || value[0] == '\0')
        return 1;

    line = apr_psprintf(r->pool, "%s => %s\n", key, value);
    stat = ap_rputs(line, r);

    return 1;
}

// Extract the values in the apr_tables.
// Used only during debugging/development to identify the available environment variables.
static int dump_request(request_rec *r) {

//    if (strcmp(r->handler, "mgmapagent-handler"))
//        return DECLINED;

    ap_set_content_type(r, "text/plain");

    if (r->header_only)
        return OK;

    apr_table_do(iterate_func, r, r->headers_in, NULL);
    apr_table_do(iterate_func, r, r->subprocess_env, NULL);
    apr_table_do(iterate_func, r, r->headers_out, NULL);

    return OK;
}

string GetServerVariable(request_rec *r, const char *variableName)
{
    string sValue;
    const char *value = apr_table_get(r->subprocess_env, variableName);
    if (value)
    {
        sValue.append(value);
    }

    return sValue;
}


static int mgmapagent_handler (request_rec *r)
{

    if (strcmp(r->handler, "mgmapagent_handler") != 0)  // NOXLATE
    {
        return DECLINED;
    }

    Initialize(r);

    // Construct self Url.  It is embedded into the output stream
    // of some requests (like GetMap).  Use a fully qualified URL.
    string serverName = GetServerVariable(r, MapAgentStrings::ServerName);
    string serverPort = GetServerVariable(r, MapAgentStrings::ServerPort);
    string scriptName = GetServerVariable(r, MapAgentStrings::ScriptName);

    string sSecure = GetServerVariable(r, MapAgentStrings::Secure);
    const char * secure = sSecure.c_str();
    bool isSecure = (secure != NULL && !_stricmp(secure, "on"));  // NOXLATE

    string url = isSecure ? MapAgentStrings::Https : MapAgentStrings::Http;

    if (!serverName.empty() && !serverPort.empty() && !scriptName.empty())
    {
        url.append(serverName);
        url += ':';
        url.append(serverPort);
        url.append(scriptName);
    }
    STRING wUrl = MgUtil::MultiByteToWideChar(url);
    Ptr<MgHttpRequest> request = new MgHttpRequest(wUrl);
    Ptr<MgHttpRequestParam> params = request->GetRequestParam();

    string query = GetServerVariable(r, MapAgentStrings::QueryString);
    string requestMethod = GetServerVariable(r, MapAgentStrings::RequestMethod);

    ApachePostParser postParser(r);

    if (!requestMethod.empty() && requestMethod.find("POST") != string::npos)  // NOXLATE
    {
        // Must be a POST request
        postParser.Parse(params);
    }
    else if (!query.empty())
    {
        MapAgentGetParser::Parse(query.c_str(), params);
    }

    // Check for HTTP Basic Auth header
    string auth = GetServerVariable(r, MapAgentStrings::HttpAuth);
    bool gotAuth = MapAgentCommon::ParseAuth((char *)auth.c_str(), params);

    if (!gotAuth)
    {
        // And check for a REMOTE_USER remapped header
        auth = GetServerVariable(r, MapAgentStrings::HttpRemoteUser);
        gotAuth = MapAgentCommon::ParseAuth((char *)auth.c_str(), params);
    }

    Ptr<MgPropertyCollection> paramList = params->GetParameters()->GetPropertyCollection();
    if (paramList != NULL)
    {
        ApacheResponseHandler responseHandler(r);

        // Check to be sure that we have some kind of credentials before continuing.  Either
        // username/password or sessionid.
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
            responseHandler.RequestAuth();
            return OK;
        }

        Ptr<MgHttpResponse> response = request->Execute();

        responseHandler.SendResponse(response);
    }

    return OK;
}

static void register_hooks (apr_pool_t *p)
{
    ap_hook_handler(mgmapagent_handler, NULL, NULL, APR_HOOK_FIRST);
}

extern "C" {
    module AP_MODULE_DECLARE_DATA mgmapagent_module =
    {
        STANDARD20_MODULE_STUFF,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        register_hooks,
    };
};

void Initialize(request_rec *r)
{
    ap_add_cgi_vars(r);
    ap_add_common_vars(r);

    // Uncomment for Debugging only
    //dump_request(r);

    if (!IsWebTierInitialized())
    {
        STRING scriptPath = MgUtil::MultiByteToWideChar(GetServerVariable(r, MapAgentStrings::ScriptFileName));

        STRING::size_type lastSlash = scriptPath.find_last_of(L"/");  // NOXLATE
        if (lastSlash < scriptPath.length())
        {
            gConfigPath = scriptPath.substr(0, lastSlash + 1);
        }
        else
        {
            gConfigPath = scriptPath;
        }

        STRING configFile = gConfigPath;
        configFile.append(MapAgentStrings::WebConfig);

        MG_TRY()

        MgInitializeWebTier(configFile);

        MG_CATCH_AND_THROW(L"ApacheAgent.Initialize");
    }
}


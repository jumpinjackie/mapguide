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

#include "MapAgentCommon.h"
#include "MapAgentStrings.h"
#include "Base64.h"


bool IsOgcRequest(MgHttpRequestParam* params)
{
    // Here we do just a crude triage.
    //
    // First up, the GET-method check: look for parameters
    STRING requestValue = params->GetParameterValue(MgHttpResourceStrings::reqWmsRequest);
    if(requestValue.length() != 0)
        return true;
    else { // plan B: the POST method check.
        // TODO: someday MapGuide other than just OGC may use POST with XML data;
        // at that time, further disambiguation will be necessary.
        if(params->GetXmlPostData().length() != 0)
            return true;
    }

    // Nope, doesn't look like it to me.
    return false;
}


bool ParseAuth(char* auth, MgHttpRequestParam* params)
{
    bool bGotAuth = false;

    ////////////////////////////////////////////////////////////////////////////
    //Bypass the standard authentication handling for OGC (WMS and WFS) requests.
    //For these requests, use predefined credentials
    //
    // OGC requests do not use OPERATION= parameter...
    STRING op = params->GetParameterValue(MgHttpResourceStrings::reqOperation);
    if(op.length() == 0  && IsOgcRequest(params))
        return AuthenticateOgcRequest(params);
    ////////////////////////////////////////////////////////////////////////////

    const char* basic = MapAgentStrings::BasicAuth;
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
    //
    // First, check for SERVICE= parameter, indicative of a GET-method request
    STRING serviceValue = MgUtil::ToUpper(params->GetParameterValue(MgHttpResourceStrings::reqWmsService));
    if(serviceValue.length() != 0)
    {
        if(serviceValue == L"WFS")  
            isWms = false;
        else if(serviceValue != L"WMS")
            return false;
    }
    else // Look for possible POST-method with content payload.
    {
        string sContents = params->GetXmlPostData();
        if(sContents.length() > 0)
        {
            // Look for the service attribute using a light-weight scan of the contents.
            // No need to invoke a full XML parsing for what's essentially pattern-matching.
            // (too bad there's no Perl-like RegExp nearby, though... )
            bool bWfs = sContents.find("service=\"WFS\"")            != string::npos  // NOXLATE
                      ||sContents.find("service='WFS'")              != string::npos; // NOXLATE

            bool bWms = sContents.find("service=\"WMS\"")            != string::npos  // NOXLATE
                      ||sContents.find("service='WMS'")              != string::npos; // NOXLATE

            // No determination made.
            if(!bWfs && !bWms)
            {
                // Tolerance and forgiveness?  Look for key namespace declarations before quitting?
                // Some requests forget the (required!!!) service= attribute, but do everything else
                // right, relying on the root element to carry the day.
                // No.  If we've gotten here, it's possibly a WMS request, since SERVICE=
                // isn't always required for them. TODO: this form of authentication should
                // run past the individual OGC objects to see if they concur.
                return isWms;
            }
            else if(bWfs && bWms) // which is it? this is not good.
                return false; // abandon the authentication attempt

            isWms = bWms;
        }
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

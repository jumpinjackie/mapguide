//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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


bool MapAgentCommon::IsOgcRequest(MgHttpRequestParam* params)
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


bool MapAgentCommon::ParseAuth(char* auth, MgHttpRequestParam* params)
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


bool MapAgentCommon::AuthenticateOgcRequest(MgHttpRequestParam* params)
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


// Is the thing pointed to an XML processing instruction?
bool MapAgentCommon::IsXmlPi(char* buf)
{
    bool retVal = false;

    if (buf != NULL)
    {
        retVal = (buf[0] == '<' &&
                      buf[1] == '?' &&
                      buf[2] == 'x' &&
                      buf[3] == 'm' &&
                      buf[4] == 'l');
    }

    return retVal;
}


void MapAgentCommon::ScanHeaders(char* partHdrStart, char* partHdrEnd, STRING& paramName, STRING& paramType, bool& bIsFile)
{
    *partHdrEnd = '\0';
    string hdr = partHdrStart;

    string nameTag = MapAgentStrings::PostName;
    string::size_type idx = hdr.find(nameTag);
    if (idx != hdr.npos)
    {
        string::size_type i = idx+nameTag.length();
        string::size_type j = hdr.find("\"", i);
        paramName = MgUtil::MultiByteToWideChar(hdr.substr(i, j-i));
    }

    string typeTag = MapAgentStrings::PostContent;
    idx = hdr.find(typeTag);
    if (idx != hdr.npos)
    {
        string::size_type i = idx+typeTag.length();
        string::size_type j = hdr.find(" ", i);
        paramType = MgUtil::MultiByteToWideChar(hdr.substr(i, j-i));
    }

    string fileTag = MapAgentStrings::PostFile;
    if (hdr.find(fileTag) != hdr.npos)
    {
        bIsFile = true;
    }

}


void MapAgentCommon::PopulateData(char* partHdrEnd, char** curBuf, char* endBuf, string& dataEndTag,
                                  STRING& paramName, STRING& paramType, MgHttpRequestParam* params, bool& bIsFile )
{
    if (paramName.length() > 0)
    {
        // Note:  dataEnd tag always start with "\r\n--" (see above)
        char* dataStart = partHdrEnd + 4;
        char* dataEnd = dataStart;
        size_t dataStartLen = strlen(dataStart);
        char match0 = dataEndTag[0];
        char match1 = dataEndTag[1];
        char match2 = dataEndTag[2];
        char match3 = dataEndTag[3];
        while (dataEnd < endBuf)
        {
            // This multi-and should virtually guarantee that the strstr
            // is only called once on the correct data.  It matches against
            // the constant part of the end tag.
            if (dataEnd[0] == match0 && dataEnd[1] == match1 &&
                dataEnd[2] == match2 && dataEnd[3] == match3)
            {
                if (strstr(dataEnd, dataEndTag.c_str()) == dataEnd)
                {
                    break;
                }
            }
            dataEnd++;
        }

        if (dataEnd > dataStart && dataEnd < endBuf)
        {
            if (bIsFile)
            {
                //TODO: Change infrastructure so byte reader can
                // be passed directly into HTTP call.  Possibly an
                // overload on AddParameter that takes a reader
                STRING fileName = MgFileUtil::GenerateTempFileName();
                Ptr<MgByte> bytes = new MgByte((BYTE_ARRAY_IN)dataStart, (INT32)(dataEnd-dataStart), MgByte::None);
                Ptr<MgByteSource> source = new MgByteSource(bytes);
                Ptr<MgByteReader> reader = source->GetReader();
                Ptr<MgByteSink> sink = new MgByteSink(reader);
                sink->ToFile(fileName);

                params->AddParameter(paramName, fileName);
                params->SetParameterType(paramName, paramType);

                // indicate this is a temporary file
                params->AddParameter(fileName, MapAgentStrings::TempfileKey);
                params->SetParameterType(fileName, MapAgentStrings::TempfileKey);
            }
            else
            {
                *dataEnd = '\0';
                string paramVal = dataStart;
                *dataEnd = '\r';
                wstring paramValue;
                MgUtil::MultiByteToWideChar(paramVal, paramValue);
                params->AddParameter(paramName, paramValue);
             }
        }

        if (curBuf != NULL)
            *curBuf = dataEnd-1;
    }
    else
    {
        if (curBuf != NULL)
            *curBuf = NULL;
    }
}


// Cleans up any temporary files created as part of the request.
// These are identified by the parameter type.
void MapAgentCommon::DeleteTempFiles(MgHttpRequestParam* params)
{
    Ptr<MgStringCollection> paramNames = params->GetParameterNames();
    if (paramNames != NULL)
    {
        for (int i=0; i<paramNames->GetCount(); ++i)
        {
            STRING name = paramNames->GetItem(i);
            STRING type = params->GetParameterType(name);
            if (type == MapAgentStrings::TempfileKey)
            {
                MgFileUtil::DeleteFile(name);
            }
        }
    }
}

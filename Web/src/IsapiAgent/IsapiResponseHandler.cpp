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

#include "MapGuideCommon.h"
#include "HttpHandler.h"
#include "HttpPrimitiveValue.h"
#include "IsapiResponseHandler.h"
#include "MapAgentStrings.h"

#include <stdlib.h>

IsapiResponseHandler::IsapiResponseHandler(EXTENSION_CONTROL_BLOCK *pECB) :
    m_pECB(pECB)
{
}

IsapiResponseHandler::~IsapiResponseHandler()
{
}

void IsapiResponseHandler::SendResponse(MgHttpResponse* response)
{
    MG_TRY()

    string sResponseHeader;
    char tempHeader[4096];

    Ptr<MgHttpResult> result = response->GetResult();
    STATUS status = result->GetStatusCode();
    if (status != 200)
    {
        STRING statusMessage = result->GetHttpStatusMessage();
        if (statusMessage == MapAgentStrings::FailedAuth1 ||
            statusMessage == MapAgentStrings::FailedAuth2)
        {
            RequestAuth();
        }
        else
        {
            //TODO: Use a resource for the HTML error message
            STRING shortError = result->GetErrorMessage();
            STRING longError = result->GetDetailedErrorMessage();
            sprintf(tempHeader, MapAgentStrings::StatusHeader, status, MG_WCHAR_TO_CHAR(statusMessage));
            sResponseHeader.append(tempHeader);
            sprintf(tempHeader, MapAgentStrings::ContentTypeHeader, MapAgentStrings::TextHtml, "");
            sResponseHeader.append(" ");
            sResponseHeader.append(tempHeader);
            sResponseHeader.append(MapAgentStrings::CrLf);
            m_pECB->dwHttpStatusCode = (DWORD)status;
            char tempStatus[4096];
            sprintf(tempStatus, "%d %s", status, MG_WCHAR_TO_CHAR(statusMessage));
            WriteHeader(sResponseHeader.c_str(), tempStatus);
            WriteContext("\r\n"
                "<html>\n<head>\n"
                "<title>%s</title>\n"
                "<meta http-equiv=\"Content-type\" content=\"text/html; charset=utf-8\">\n"
                "</head>\n"
                "<body>\n<h2>%s</h2>\n%s\n</body>\n</html>\n",
                MG_WCHAR_TO_CHAR(statusMessage),
                MG_WCHAR_TO_CHAR(shortError),
                MG_WCHAR_TO_CHAR(longError));

        }
    }
    else
    {

        // Status was ok.  Send the real result back.
        STRING contentType = result->GetResultContentType();
        STRING stringVal;

        sprintf(tempHeader, MapAgentStrings::StatusOkHeader);
        sResponseHeader.append(tempHeader);
        if (contentType.length() > 0)
        {
            // If we are returning text, state that it is utf-8.
            string charSet = "";
            if (contentType.find(L"text") != contentType.npos)  //NOXLATE
            {
                charSet = MapAgentStrings::Utf8Text;
            }
            sprintf(tempHeader, MapAgentStrings::ContentTypeHeader, MG_WCHAR_TO_CHAR(contentType), charSet.c_str());
            sResponseHeader.append(tempHeader);
        }
        else
        {
            sprintf(tempHeader, MapAgentStrings::ContentTypeHeader, MapAgentStrings::TextPlain, MapAgentStrings::Utf8Text);
            sResponseHeader.append(tempHeader);
        }

        Ptr<MgByteReader> outputReader;
        Ptr<MgDisposable> resultObj = result->GetResultObject();
        MgDisposable* pResultObj = (MgDisposable*)resultObj;

        if (NULL != dynamic_cast<MgByteReader*>(pResultObj))
        {
            outputReader = (MgByteReader*) SAFE_ADDREF(pResultObj);
        }
        else if (NULL != dynamic_cast<MgFeatureReader*>(pResultObj))
        {
            outputReader = ((MgFeatureReader*)pResultObj)->ToXml();
        }
        else if (NULL != dynamic_cast<MgStringCollection*>(pResultObj))
        {
            outputReader = ((MgStringCollection*)pResultObj)->ToXml();
        }
        else if (NULL != dynamic_cast<MgSqlDataReader*>(pResultObj))
        {
            outputReader = ((MgSqlDataReader*)pResultObj)->ToXml();
        }
        else if (NULL != dynamic_cast<MgDataReader*>(pResultObj))
        {
            outputReader = ((MgDataReader*)pResultObj)->ToXml();
        }
        else if (NULL != dynamic_cast<MgSpatialContextReader*>(pResultObj))
        {
            outputReader = ((MgSpatialContextReader*)pResultObj)->ToXml();
        }
        else if (NULL != dynamic_cast<MgLongTransactionReader*>(pResultObj))
        {
            outputReader = ((MgLongTransactionReader*)pResultObj)->ToXml();
        }
        else if (NULL != dynamic_cast<MgHttpPrimitiveValue*>(pResultObj))
        {
            stringVal = ((MgHttpPrimitiveValue*)pResultObj)->ToString();
        }

        if (stringVal.length() > 0)
        {
            string utf8 = MG_WCHAR_TO_CHAR(stringVal);
            sprintf(tempHeader, MapAgentStrings::ContentLengthHeader, utf8.length());
            sResponseHeader.append(tempHeader);
            sResponseHeader.append(MapAgentStrings::CrLf);
            DWORD dwHeaderSize = (DWORD)sResponseHeader.length();
            m_pECB->ServerSupportFunction(m_pECB->ConnID, HSE_REQ_SEND_RESPONSE_HEADER, NULL, &dwHeaderSize, (LPDWORD)sResponseHeader.c_str());

            DWORD dwBufLen = (DWORD)utf8.length();
            m_pECB->WriteClient(m_pECB->ConnID, (LPVOID)utf8.c_str(), &dwBufLen, 0);
        }
        else if (outputReader != NULL)
        {
            INT64 outLen = outputReader->GetLength();
            sprintf(tempHeader, MapAgentStrings::ContentLengthHeader, (INT32)outLen);
            sResponseHeader.append(tempHeader);
            sResponseHeader.append(MapAgentStrings::CrLf);
            WriteHeader(sResponseHeader.c_str());

            unsigned char buf[4096];
            DWORD dwSize;
            int nBytes = outputReader->Read(buf,4096);
            while (nBytes > 0)
            {
                dwSize = nBytes;
                m_pECB->WriteClient(m_pECB->ConnID, buf, &dwSize, 0);
                nBytes = outputReader->Read(buf,4096);
            }
        }
        else
        {
            sprintf(tempHeader, MapAgentStrings::ContentLengthHeader, 0);
            sResponseHeader.append(tempHeader);
            sResponseHeader.append(MapAgentStrings::CrLf);
            WriteHeader(sResponseHeader.c_str());
        }
    }

    MG_CATCH_AND_THROW(L"IsapiResponseHandler.SendResponse");
}

void IsapiResponseHandler::SendError(MgException* e)
{
    MG_TRY()
    STRING shortError = e->GetMessage();
    STRING longError = e->GetDetails();
    STRING statusMessage = e->GetClassName();
    DWORD status = 559;

    //TODO: Use a string resource for html error text format
    string sResponseHeader;
    char tempHeader[4096];

    sprintf(tempHeader, MapAgentStrings::StatusHeader, 559, MG_WCHAR_TO_CHAR(statusMessage));
    sResponseHeader.append(tempHeader);
    sprintf(tempHeader, MapAgentStrings::ContentTypeHeader, MapAgentStrings::TextHtml, MapAgentStrings::Utf8Text);
    sResponseHeader.append(tempHeader);
    sResponseHeader.append(MapAgentStrings::CrLf);
    m_pECB->dwHttpStatusCode = (DWORD)status;
    char tempStatus[4096];
    sprintf(tempStatus, "%d %s", status, MG_WCHAR_TO_CHAR(statusMessage));
    WriteHeader(sResponseHeader.c_str(), tempStatus);
    WriteContext("\r\n"
        "<html>\n<head>\n"
        "<title>%s</title>\n"
        "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n"
        "</head>\n"
        "<body>\n<h2>%s</h2>\n%s\n</body>\n</html>\n",
        MG_WCHAR_TO_CHAR(statusMessage),
        MG_WCHAR_TO_CHAR(shortError),
        MG_WCHAR_TO_CHAR(longError));


    MG_CATCH(L"IsapiResponseHandler.SendError")
}

void IsapiResponseHandler::RequestAuth()
{
    //TODO: Use string resources for realm and message

    char tempHeader[4096];
    string errorMsg = "You must enter a valid login ID and password to access this site\r\n";

    string sResponseHeader = MapAgentStrings::UnauthorizedHeader;
    sprintf(tempHeader, MapAgentStrings::WWWAuth, MG_WCHAR_TO_CHAR(MapAgentStrings::ProductName));
    sResponseHeader.append(tempHeader);
    sprintf(tempHeader, MapAgentStrings::ContentTypeHeader, MapAgentStrings::TextPlain, MapAgentStrings::Utf8Text);
    sResponseHeader.append(tempHeader);
    sprintf(tempHeader, MapAgentStrings::ContentLengthHeader, errorMsg.length());
    sResponseHeader.append(MapAgentStrings::CrLf);

    DWORD dwHeaderSize = (DWORD)sResponseHeader.length();

    DWORD dwContentSize;
    dwContentSize = (DWORD)errorMsg.length();

    const char* szHttpStatus = MapAgentStrings::Unauthorized;

    m_pECB->ServerSupportFunction(m_pECB->ConnID, HSE_REQ_SEND_RESPONSE_HEADER, (LPVOID) szHttpStatus, &dwHeaderSize, (LPDWORD)sResponseHeader.c_str());
    m_pECB->WriteClient(m_pECB->ConnID, (LPVOID) errorMsg.c_str(), &dwContentSize, 0);
}

void IsapiResponseHandler::WriteHeader(const char* szBuffer, const char* szStatusBuffer)
{
    DWORD dwUnused = 0;
    m_pECB->ServerSupportFunction(m_pECB->ConnID, HSE_REQ_SEND_RESPONSE_HEADER, (LPVOID)szStatusBuffer, &dwUnused, (LPDWORD)szBuffer);
}

void IsapiResponseHandler::WriteContext(const char *pszFormat, ...)
{
    char szBuffer[4096];
    va_list arg_ptr;
    va_start(arg_ptr, pszFormat);
    vsprintf(szBuffer, pszFormat, arg_ptr);
    va_end(arg_ptr);

    DWORD dwSize = (DWORD)strlen(szBuffer);
    m_pECB->WriteClient(m_pECB->ConnID, szBuffer, &dwSize, 0);
}

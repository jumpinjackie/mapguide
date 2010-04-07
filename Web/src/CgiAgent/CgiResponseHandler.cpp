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

#include "MapGuideCommon.h"
#include "HttpHandler.h"
#include "HttpPrimitiveValue.h"
#include "CgiResponseHandler.h"
#include "MapAgentStrings.h"

#include <stdlib.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef _WIN32
#include <process.h>
#else
extern char **environ;
#endif

extern void DumpMessage(const char* format, ...);

CgiResponseHandler::CgiResponseHandler()
{
}

CgiResponseHandler::~CgiResponseHandler()
{
}

void CgiResponseHandler::SendResponse(MgHttpResponse* response)
{
    MG_TRY()

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
            printf(MapAgentStrings::StatusHeader, status, MG_WCHAR_TO_CHAR(statusMessage));
            printf(MapAgentStrings::ContentTypeHeader, MapAgentStrings::TextHtml, "");
            printf("\r\n"
                "<html>\n<head>\n"
                "<title>%s</title>\n"
                "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n"
                "</head>\n"
                "<body>\n<h2>%s</h2>\n%s\n</body>\n</html>\n",
                MG_WCHAR_TO_CHAR(statusMessage),
                MG_WCHAR_TO_CHAR(shortError),
                MG_WCHAR_TO_CHAR(longError));

            DumpMessage(MG_WCHAR_TO_CHAR(longError));
        }
    }
    else
    {
        DumpMessage(MapAgentStrings::StatusOkHeader);

        // Status was ok.  Send the real result back.
        STRING contentType = result->GetResultContentType();
        STRING stringVal;

        printf(MapAgentStrings::StatusOkHeader);
        if (contentType.length() > 0)
        {
            // If we are returning text, state that it is utf-8.
            string charSet = "";
            if (contentType.find(L"text") != contentType.npos)  //NOXLATE
            {
                charSet = MapAgentStrings::Utf8Text;
            }
            printf(MapAgentStrings::ContentTypeHeader, MG_WCHAR_TO_CHAR(contentType), charSet.c_str());
        }
        else
        {
            printf(MapAgentStrings::ContentTypeHeader, MapAgentStrings::TextPlain, MapAgentStrings::Utf8Text);
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
            printf(MapAgentStrings::ContentLengthHeader, utf8.length());
            printf("\r\n%s",utf8.c_str());
        }
        else if (outputReader != NULL)
        {
            INT64 outLen = outputReader->GetLength();
            printf(MapAgentStrings::ContentLengthHeader,(INT32)outLen);
            printf("\r\n");
            unsigned char buf[4096];
            int nBytes = outputReader->Read(buf,4096);
            while (nBytes > 0)
            {
                fwrite(buf, 1, nBytes, stdout);
                nBytes = outputReader->Read(buf,4096);
            }
        }
        else
        {
            printf(MapAgentStrings::ContentLengthHeader, 0);
            printf("\r\n");
        }
    }

    MG_CATCH_AND_THROW(L"CgiResponseHandler.SendResponse");
}

void CgiResponseHandler::SendError(MgException* e)
{
    MG_TRY()
    STRING shortError = e->GetExceptionMessage();
    STRING longError = e->GetDetails();
    STRING statusMessage = e->GetClassName();

    //TODO: Use a string resource for html error text format
    printf(MapAgentStrings::StatusHeader, 559, MG_WCHAR_TO_CHAR(statusMessage));
    printf(MapAgentStrings::ContentTypeHeader, MapAgentStrings::TextHtml, MapAgentStrings::Utf8Text);
    printf("\r\n"
        "<html>\n<head>\n"
        "<title>%s</title>\n"
        "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n"
        "</head>\n"
        "<body>\n<h2>%s</h2>\n%s\n</body>\n</html>\n",
        MG_WCHAR_TO_CHAR(statusMessage),
        MG_WCHAR_TO_CHAR(shortError),
        MG_WCHAR_TO_CHAR(longError));

    DumpMessage(MG_WCHAR_TO_CHAR(longError));

    MG_CATCH(L"CgiResponseHandler.SendError")
}

void CgiResponseHandler::RequestAuth()
{
    //TODO: Use string resources for realm and message
    printf(MapAgentStrings::UnauthorizedHeader);
    printf(MapAgentStrings::WWWAuth, MG_WCHAR_TO_CHAR(MapAgentStrings::ProductName));
    string errorMsg = "You must enter a valid login ID and password to access this site\r\n";
    printf(MapAgentStrings::ContentTypeHeader, MapAgentStrings::TextPlain, MapAgentStrings::Utf8Text);
    printf(MapAgentStrings::ContentLengthHeader, errorMsg.length());
    printf("\r\n%s",errorMsg.c_str());
}

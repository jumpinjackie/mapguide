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

#include "ApacheResponseHandler.h"
#include "MapAgentStrings.h"

#include "httpd.h"
#include "http_config.h"
#include "http_core.h"
#include "http_log.h"
#include "http_protocol.h"

#include "ap_compat.h"
#include "apr_strings.h"

ApacheResponseHandler::ApacheResponseHandler(request_rec *r) : m_r(r)
{
}

ApacheResponseHandler::~ApacheResponseHandler()
{
}

void ApacheResponseHandler::SendResponse(MgHttpResponse *response)
{
    MG_TRY()

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
            sprintf(tempHeader, "%d %s", status, MG_WCHAR_TO_CHAR(statusMessage));
            m_r->status = status;
            m_r->status_line = tempHeader;
            apr_table_set(m_r->headers_out, MapAgentStrings::StatusKey, tempHeader);
            m_r->content_type = MapAgentStrings::TextHtml;
            ap_rprintf(m_r, "\r\n"
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

        apr_table_set(m_r->headers_out, MapAgentStrings::StatusKey, MapAgentStrings::StatusVal200OK);
        if (contentType.length() > 0)
        {
            // If we are returning text, state that it is utf-8.
            string charSet;
            if (contentType.find(L"text") != contentType.npos)  //NOXLATE
            {
                charSet = MapAgentStrings::Utf8Text;
            }
            sprintf(tempHeader, "%s%s", MG_WCHAR_TO_CHAR(contentType), charSet.c_str());
            m_r->content_type = apr_pstrdup(m_r->pool, tempHeader);
        }
        else
        {
            sprintf(tempHeader, "%s%s", MapAgentStrings::TextPlain, MapAgentStrings::Utf8Text);
            m_r->content_type = apr_pstrdup(m_r->pool, tempHeader);
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
            sprintf(tempHeader, "%d", utf8.length());
            apr_table_set(m_r->headers_out, MapAgentStrings::ContentLengthKey, tempHeader);

            ap_send_http_header(m_r);
            ap_rwrite(utf8.c_str(), (int)utf8.length(), m_r);

        }
        else if (outputReader != NULL)
        {
            INT64 outLen = outputReader->GetLength();
            sprintf(tempHeader, "%d", (INT32)outLen);
            apr_table_set(m_r->headers_out, MapAgentStrings::ContentLengthKey, tempHeader);
            ap_send_http_header(m_r);

            unsigned char buf[4096];
            int nBytes = outputReader->Read(buf,4096);
            while (nBytes > 0)
            {
                ap_rwrite(buf, nBytes, m_r);
                nBytes = outputReader->Read(buf,4096);
            }
        }
        else
        {
            apr_table_set(m_r->headers_out, MapAgentStrings::ContentLengthKey, "0");  // NOXLATE
            ap_send_http_header(m_r);
        }
    }

    MG_CATCH_AND_THROW(L"ApacheResponseHandler.SendResponse");
}

void ApacheResponseHandler::SendError(MgException *e)
{
    MG_TRY()
    STRING shortError = e->GetMessage();
    STRING longError = e->GetDetails();
    STRING statusMessage = e->GetClassName();

    //TODO: Use a string resource for html error text format

    char tempHeader[4096];

    sprintf(tempHeader, "%d %s", 559, MG_WCHAR_TO_CHAR(statusMessage));
    m_r->status = 559;
    m_r->status_line = tempHeader;
    apr_table_set(m_r->headers_out, MapAgentStrings::StatusKey, tempHeader);
    sprintf(tempHeader, "%s%s", MapAgentStrings::TextHtml, MapAgentStrings::Utf8Text);
    m_r->content_type = apr_pstrdup(m_r->pool, tempHeader);
    ap_rprintf(m_r, "\r\n"
        "<html>\n<head>\n"
        "<title>%s</title>\n"
        "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n"
        "</head>\n"
        "<body>\n<h2>%s</h2>\n%s\n</body>\n</html>\n",
        MG_WCHAR_TO_CHAR(statusMessage),
        MG_WCHAR_TO_CHAR(shortError),
        MG_WCHAR_TO_CHAR(longError));


    MG_CATCH(L"ApacheResponseHandler.SendError")
}

void ApacheResponseHandler::RequestAuth()
{
    //TODO: Use string resources for realm and message

    char tempHeader[4096];
    string errorMsg = "You must enter a valid login ID and password to access this site\r\n";

    sprintf(tempHeader, "%s%s", MapAgentStrings::TextPlain, MapAgentStrings::Utf8Text);
    m_r->status = HTTP_UNAUTHORIZED;
    m_r->content_type = apr_pstrdup(m_r->pool, tempHeader);
    apr_table_set(m_r->headers_out, MapAgentStrings::StatusKey, MapAgentStrings::Unauthorized);
    sprintf(tempHeader, MapAgentStrings::WWWAuthVal, MG_WCHAR_TO_CHAR(MapAgentStrings::ProductName));
    apr_table_set(m_r->headers_out, MapAgentStrings::WWWAuthKey, tempHeader);
    ap_send_http_header(m_r);
    ap_rputs(errorMsg.c_str(), m_r);
}

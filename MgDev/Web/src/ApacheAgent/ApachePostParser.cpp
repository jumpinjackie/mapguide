//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of version 2.1 of the GNU Lesser
//  General Public License as published by the Free Software Foundation.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTAB/ILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

#include "MapGuideCommon.h"
#include "ApachePostParser.h"

#include "MapAgentCommon.h"
#include "MapAgentGetParser.h"
#include "MapAgentStrings.h"

//TODO: Make MAXPOSTSIZE a webconfig parameter
static size_t MAXPOSTSIZE = 1000000000; // Limit to 1Gig

ApachePostParser::ApachePostParser(request_rec* r) : m_r(r)
{
    m_pBuffer = NULL;
}

ApachePostParser::~ApachePostParser()
{
}

void ApachePostParser::Parse(MgHttpRequestParam* params)
{
    MG_TRY()

    int rc;

    if ( (rc = ap_setup_client_block(m_r, REQUEST_CHUNKED_ERROR)) != OK )
    {
        throw new MgStreamIoException(L"ApachePostParser.Parse",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    long totalBytes = 0;

    if (ap_should_client_block(m_r))
    {
        char argsbuffer[HUGE_STRING_LEN];
        int rsize, len_read, rpos = 0;
        totalBytes = (long)m_r->remaining;

        if (totalBytes < 0 || totalBytes > (long)MAXPOSTSIZE)
        {
            throw new MgStreamIoException(L"ApachePostParser.Parse", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        m_pBuffer = (BYTE *)apr_pcalloc(m_r->pool, totalBytes + 1);

        if (NULL == m_pBuffer)
        {
            throw new MgOutOfMemoryException(L"ApachePostParser.Parse", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        while( (len_read = ap_get_client_block(m_r, argsbuffer, sizeof(argsbuffer))) > 0 )
        {
            if( (rpos + len_read) > totalBytes )
            {
                rsize = totalBytes - rpos;
            }
            else
            {
                rsize = len_read;
            }
            memcpy((char *)m_pBuffer + rpos, argsbuffer, rsize);
            rpos += rsize;
        }
    }

    const char* type = NULL;
    type = apr_table_get(m_r->headers_in, MapAgentStrings::ContentTypeKey);

    string content;
    if (type)
    {
        content.append(type);
    }

    if (content == MapAgentStrings::UrlEncoded && totalBytes > 0)
    {
        m_pBuffer[totalBytes] = '\0';

        // Here's another case of interoperability "Forgiveness
        // and Tolerance": degree, among other clients, sends along
        // a POST with xml contents, but fails to correctly set the
        // mime type.  You guessed it: it says it's url-encoded.
        // ----------------
        // If we got here, but determine that the contents look
        // and feel like XML, it's a safe bet that it should
        // be treated like XML.  Otherwise, we assume the
        // content-type is probably correct.  (Note that
        // the IsXmlPi should conveniently fail if it really IS
        // url-encoded, since the question mark in <?xml...?>
        // should itself be url-encoded: <%3Fxml... )
        if (MapAgentCommon::IsXmlPi((char *)m_pBuffer))
            params->SetXmlPostData((char *)m_pBuffer);
        else
            MapAgentGetParser::Parse((char *)m_pBuffer, params);
    }
    else if (content.find(MapAgentStrings::MultiPartForm) != content.npos)
    {
        const char* boundary = MapAgentStrings::PostBoundary;
        string::size_type tagIdx = content.find(boundary);
        if (tagIdx != content.npos)
        {
            string partTag = "--";
            partTag.append(content.substr(tagIdx+strlen(boundary)));
            string dataEndTag = "\r\n";
            dataEndTag.append(partTag);
            char* endBuf = (char *)&m_pBuffer[totalBytes];
            char* curBuf = (char *)m_pBuffer;
            while (curBuf < endBuf && NULL != curBuf)
            {
                // Isolate multipart header
                bool bOk = true;
                char* partHdrStart = NULL;
                char* partHdrEnd = NULL;
                bOk = (NULL != (partHdrStart = strstr(curBuf, partTag.c_str())));
                if (bOk) bOk = (NULL != (partHdrEnd = strstr(partHdrStart, "\r\n\r\n")));

                STRING paramName;
                STRING paramType;
                bool bIsFile = false;

                // Scan headers
                if (bOk)
                {
                    MapAgentCommon::ScanHeaders(partHdrStart, partHdrEnd, paramName, paramType, bIsFile);
                }

                // And populate the data
                MapAgentCommon::PopulateData(partHdrEnd, &curBuf, endBuf, dataEndTag, paramName, paramType, params, bIsFile);
            }
        }
    }

    // The check for text/xml is not always sufficient.  CarbonTools, for example,
    // fails to set Content-Type: text/xml and just sends Content-Type: utf-8.
    // A better check might be looking into the buffer to find "<?xml" at the beginning.
    else if (content.find(MapAgentStrings::TextXml) != content.npos || MapAgentCommon::IsXmlPi((char *)m_pBuffer))
    {
        m_pBuffer[totalBytes] = '\0';
        params->SetXmlPostData((char *)m_pBuffer);
    }
    else
    {
        // Throw a better exception here?
        throw new MgStreamIoException(L"ApachePostParser.Parse", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_CATCH_AND_THROW(L"ApachePostParser.Parse");
}




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
#include "IsapiPostParser.h"
#include "MapAgentCommon.h"
#include "MapAgentGetParser.h"
#include "MapAgentStrings.h"

//TODO: Make MAXPOSTSIZE a webconfig parameter
static size_t MAXPOSTSIZE = 1000000000; // Limit to 1Gig

IsapiPostParser::IsapiPostParser(EXTENSION_CONTROL_BLOCK* pECB)
{
    m_bufSize = 0;

    m_pECB = pECB;
    m_pBuffer = NULL;
    m_dwPos = 0;
}

IsapiPostParser::~IsapiPostParser()
{
    if (m_pBuffer != NULL)
    {
        free(m_pBuffer);
        m_pBuffer = NULL;
    }
}

void IsapiPostParser::Parse(MgHttpRequestParam* params)
{
    MG_TRY()

    // Get the content length
    DWORD dwTotalBytes = m_pECB->cbTotalBytes;

    if (dwTotalBytes < 0 || dwTotalBytes > MAXPOSTSIZE)
    {
        throw new MgStreamIoException(L"IsapiPostParser.Parse", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (dwTotalBytes > 0)
    {
        // Attempt to reuse the existing buffer if it's big enough.  If
        // not we need to reallocate a bigger one.  Note: to avoid a null
        // pointer, even when a first pass has Content-Length == 0 (and
        // thus not > the virgin m_bufSize,) we ALWAYS come through this
        // branch if the buffer is still null.
        if (dwTotalBytes > m_bufSize || m_pBuffer == NULL)
        {
            if (m_pBuffer != NULL)
            {
                free(m_pBuffer);
                m_pBuffer = NULL;
            }

            // Add extra byte for url encoded null termination
            m_pBuffer = (BYTE*)malloc(dwTotalBytes+1);

            if (NULL == m_pBuffer)
            {
                throw new MgOutOfMemoryException(L"IsapiPostParser.Parse", __LINE__, __WFILE__, NULL, L"", NULL);
            }

            m_bufSize = dwTotalBytes;
        }
    }

    // Copy the request info to the buffer
    DWORD dwTotalRead = 0;
    if (m_pECB->cbAvailable > 0)
    {
        dwTotalRead = m_pECB->cbAvailable;

        memcpy(m_pBuffer + m_dwPos, m_pECB->lpbData, m_pECB->cbAvailable);
        m_dwPos += m_pECB->cbAvailable;
    }
    // get the rest of the request
    while (dwTotalRead < dwTotalBytes)
    {
        DWORD dwBytesToRead = 512;
        BYTE pbBuffer[512];  // temporary buffer

        memset(pbBuffer, 0, 512);

        // when we hit the end, we can't read 512 bytes, so decrease read size
        if ( (dwTotalRead + dwBytesToRead) > dwTotalBytes )
        {
            dwBytesToRead = dwTotalBytes - dwTotalRead;
        }

        // read from client to temporary buffer
        m_pECB->ReadClient(m_pECB->ConnID, pbBuffer, &dwBytesToRead);

        // copy temp buffer to the byte array
        memcpy(m_pBuffer + m_dwPos, pbBuffer, dwBytesToRead);
        m_dwPos += dwBytesToRead;

        dwTotalRead += dwBytesToRead;
    }

    string content = m_pECB->lpszContentType;

    if (content.find(MapAgentStrings::UrlEncoded) == 0)
    {
        m_pBuffer[dwTotalBytes] = '\0';  // null terminate

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
            char* endBuf = (char *)&m_pBuffer[dwTotalBytes];
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
        m_pBuffer[dwTotalBytes] = '\0';
        params->SetXmlPostData((char *)m_pBuffer);
    }
    else
    {
        // Throw a better exception here?
        throw new MgStreamIoException(L"IsapiPostParser.Parse", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_CATCH_AND_THROW(L"IsapiPostParser.Parse");
}





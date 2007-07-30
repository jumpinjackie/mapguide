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
#include "HttpHandler.h"
#include "IsapiPostParser.h"
#include "MapAgentGetParser.h"
#include "MapAgentStrings.h"

//TODO: Make MAXPOSTSIZE a webconfig parameter
static size_t MAXPOSTSIZE = 1000000000; // Limit to 1Gig

// Is the thing pointed to an XML processing instruction?
bool IsXmlPi(char* buf)
{
    return buf[0] == '<' &&
           buf[1] == '?' &&
           buf[2] == 'x' &&
           buf[3] == 'm' &&
           buf[4] == 'l';
}

IsapiPostParser::IsapiPostParser(EXTENSION_CONTROL_BLOCK* pECB)
{
    m_bufSize = 0;

    m_pECB = pECB;
    m_pBuffer = NULL;
    m_dwPos = 0;
}

IsapiPostParser::~IsapiPostParser(void)
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

    if (content == MapAgentStrings::UrlEncoded)
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
        if (IsXmlPi((char *)m_pBuffer))
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
                    ScanHeaders(partHdrStart, partHdrEnd, paramName, paramType, bIsFile);
                }

                // And populate the data
                PopulateData(partHdrEnd, &curBuf, endBuf, dataEndTag, paramName, paramType, params, bIsFile);
            }
        }
    }

    // The check for text/xml is not always sufficient.  CarbonTools, for example,
    // fails to set Content-Type: text/xml and just sends Content-Type: utf-8.
    // A better check might be looking into the buffer to find "<?xml" at the beginning.
    else if (content.find(MapAgentStrings::TextXml) != content.npos || IsXmlPi((char *)m_pBuffer))
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

void IsapiPostParser::ScanHeaders(char* partHdrStart, char* partHdrEnd, STRING& paramName, STRING& paramType, bool& bIsFile)
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

void IsapiPostParser::PopulateData(char* partHdrEnd, char** curBuf, char* endBuf, string& dataEndTag, 
                                   STRING& paramName, STRING& paramType, MgHttpRequestParam* params, bool& bIsFile )
{
    if (paramName.length() > 0)
    {
        // Note:  dataEnd tag always start with "\r\n--" (see above)
        char* dataStart = partHdrEnd + 4;
        char* dataEnd = dataStart;
        int dataStartLen = strlen(dataStart);
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



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

#include "MapGuideCommon.h"
#include "HttpHandler.h"
#include "CgiPostParser.h"
#include "MapAgentCommon.h"
#include "MapAgentGetParser.h"
#include "MapAgentStrings.h"

extern void DumpMessage(const char* format, ...);
extern void DumpMessage2(const char* msg);

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

CgiPostParser::CgiPostParser()
{
    m_bufSize = 0;
    m_buf = NULL;
}

CgiPostParser::~CgiPostParser()
{
    if (m_buf != NULL)
    {
        free(m_buf);
        m_buf = NULL;
    }
}

void CgiPostParser::Parse(MgHttpRequestParam* params)
{
    MG_TRY()

    size_t bufPos = 0;

    // TODO: Use a memory mapped file here
    char* contentLength = getenv(MapAgentStrings::ContentLength);

    size_t nBytes = 0;
    if (NULL != contentLength)
    {
        DumpMessage("Content length %s", contentLength);
        nBytes = atoi(contentLength);
        if (nBytes < 0 || nBytes > MAXPOSTSIZE)
        {
            throw new MgStreamIoException(L"CgiPostParser.Parse", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        // Attempt to reuse the existing buffer if it's big enough.  If
        // not we need to reallocate a bigger one.  Note: to avoid a null
        // pointer, even when a first pass has Content-Length == 0 (and
        // thus not > the virgin m_bufSize,) we ALWAYS come through this
        // branch if the buffer is still null.
        if (nBytes > m_bufSize || m_buf == NULL)
        {
            if (m_buf != NULL)
            {
                free(m_buf);
                m_buf = NULL;
            }

            // Add extra byte for url encoded null termination
            m_buf = (char*)malloc(nBytes+1);

            if (NULL == m_buf)
            {
                throw new MgOutOfMemoryException(L"CgiPostParser.Parse", __LINE__, __WFILE__, NULL, L"", NULL);
            }

            m_bufSize = nBytes;
        }
    }

    DumpMessage("Attempting to read %d bytes", nBytes);

    size_t readBytes = 0;

    if (nBytes > 0)
    {
        readBytes = fread(m_buf, 1, nBytes, stdin);
    }

    DumpMessage("Read %d bytes", readBytes);
    DumpMessage2(m_buf);


    if (readBytes != nBytes)
    {
        //TODO:  Better exception message?
        throw new MgStreamIoException(L"CgiPostParser.Parse", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    char* contentType = getenv(MapAgentStrings::ContentType);
    if (NULL != contentType)
    {
        string content = contentType;
        DumpMessage("Content type: %s", content.c_str());

        if (content.find(MapAgentStrings::UrlEncoded) == 0)
        {
            m_buf[nBytes] = '\0';

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
            if(IsXmlPi(m_buf))
                params->SetXmlPostData(m_buf);
            else
                MapAgentGetParser::Parse(m_buf, params);
        }
        else if (content.find(MapAgentStrings::MultiPartForm) != content.npos)
        {
            // TODO: Factor this block into a descriptively-named method;
            // this one is waaaaaaaay too long.
            // Delegation code (if then else if then else if ...) shouldn't
            // mingle with actual "work" code.
            const char* boundary = MapAgentStrings::PostBoundary;
            string::size_type tagIdx = content.find(boundary);
            if (tagIdx != content.npos)
            {
                string partTag = "--";
                partTag.append(content.substr(tagIdx+strlen(boundary)));
                string dataEndTag = "\r\n";
                dataEndTag.append(partTag);
                char* endBuf = &m_buf[nBytes];
                char* curBuf = m_buf;
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

                    // And populate the data
                    if (paramName.length() > 0)
                    {
                        // Note:  dataEnd tag always start with "\r\n--" (see above)
                        char* dataStart = partHdrEnd + 4;
                        char* dataEnd = dataStart;
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

                        curBuf = dataEnd-1;
                    }
                    else
                    {
                        curBuf = NULL;
                    }
                }
            }
        }

        // The check for text/xml is not always sufficient.  CarbonTools, for example,
        // fails to set Content-Type: text/xml and just sends Content-Type: utf-8.
        // A better check might be looking into the buffer to find "<?xml" at the beginning.
        else if (content.find(MapAgentStrings::TextXml) != content.npos || IsXmlPi(m_buf))
        {
            m_buf[nBytes] = '\0';
            params->SetXmlPostData(m_buf);
        }
        else
        {
            //TODO: throw a better exception here
            throw new MgStreamIoException(L"CgiPostParser.Parse", __LINE__, __WFILE__, NULL, L"", NULL);
        }
    }

    MG_CATCH_AND_THROW(L"CgiPostParser.Parse");
}

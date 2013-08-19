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

#include "CgiReaderStreamer.h"
#include "MapAgentStrings.h"

extern void DumpMessage(const char* format, ...);
extern void DumpMessage2(const char* msg);


CgiReaderStreamer::CgiReaderStreamer(MgByteReader* reader) :
    MgHttpReaderStreamer(reader),
    m_bEndOfStream(false)
{
}


CgiReaderStreamer::~CgiReaderStreamer() 
{ 
    EndStream();
}


void CgiReaderStreamer::EndStream()
{
    if (!m_bEndOfStream)
    {
        //Write end of chunked response
        printf("0\r\n\r\n");
        m_bEndOfStream = true;
    }
}


void CgiReaderStreamer::SetChunkedEncoding() 
{
    printf("Connection: Keep-Alive");
    printf(MapAgentStrings::CrLf);
    printf("Transfer-Encoding: chunked");
    printf(MapAgentStrings::CrLf);
    printf(MapAgentStrings::CrLf);
}


void CgiReaderStreamer::WriteChunk(const char* str, size_t length)
{
    /*
    std::string logStr = "BEGIN - CgiReaderStreamer::WriteChunk";
    logStr.append(" str='");
    logStr.append(str);
    logStr.append("', length=");
    MgUtil::ReplaceString("%", "%%", logStr, -1);
    std::string lengthStr;
    MgUtil::Int32ToString(length, lengthStr);
    logStr.append(lengthStr);
    DumpMessage(logStr.c_str());
    */
    
    m_hexLen.clear();
    MgUtil::Int32ToHexString(length, m_hexLen);
    m_hexLen.append(MapAgentStrings::CrLf);

    printf(m_hexLen.c_str());
    //---- BEGIN chunk body ----//
    //Need to escape any characters that may trip up printf
    //Currently it is: '%'
    m_sCurrentChunk.clear();
    m_sCurrentChunk = str;
    MgUtil::ReplaceString("%", "%%", m_sCurrentChunk, -1);
    printf(m_sCurrentChunk.c_str());
    //---- END chunk body ----//
    printf(MapAgentStrings::CrLf);
    //DumpMessage("END - CgiReaderStreamer::WriteChunk");
}

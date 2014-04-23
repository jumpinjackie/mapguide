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

#include "HttpHandler.h"
#include "XmlJsonConvert.h"

#define CHUNK_SIZE 8192

MgHttpReaderStreamer::MgHttpReaderStreamer(MgByteReader* reader)
{
    m_reader = SAFE_ADDREF(reader);
}


MgHttpReaderStreamer::~MgHttpReaderStreamer() 
{
    m_reader = NULL;
}


void MgHttpReaderStreamer::SetChunkedEncoding() 
{
}


void MgHttpReaderStreamer::WriteChunk(const char* str, size_t length)
{
}


void MgHttpReaderStreamer::EndStream()
{
}


void MgHttpReaderStreamer::StreamResult()
{
    MG_TRY()

    SetChunkedEncoding();

    unsigned char buf[CHUNK_SIZE] = { 0 };
    INT32 read = m_reader->Read(buf, CHUNK_SIZE);
    while (read > 0)
    {
        WriteChunk((char*)buf, read);
        read = m_reader->Read(buf, CHUNK_SIZE);
    }

    MG_CATCH_AND_THROW(L"MgHttpReaderStreamer.StreamResult");
}
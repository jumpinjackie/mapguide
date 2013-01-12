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
#ifndef ISAPI_READER_STREAMER_H
#define ISAPI_READER_STREAMER_H

#include "HttpExt.h"
#include "HttpHandler.h"

class IsapiReaderStreamer : public MgHttpReaderStreamer
{
public:
    IsapiReaderStreamer(EXTENSION_CONTROL_BLOCK* rec, const std::string& sResponseHeader, MgReader* reader, CREFSTRING format);
    virtual ~IsapiReaderStreamer();

protected:
    virtual void SetChunkedEncoding();
    virtual void WriteChunk(const char* str, size_t length);
    virtual void EndStream();

private:
    EXTENSION_CONTROL_BLOCK *m_pECB;
    std::string m_hexLen;
    std::string m_sResponseHeader;
    bool m_bEndOfStream;
};

#endif
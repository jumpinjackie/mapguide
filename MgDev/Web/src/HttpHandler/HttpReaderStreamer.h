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
#ifndef MG_HTTP_READER_STREAMER_H
#define MG_HTTP_READER_STREAMER_H

/// <summary>
/// Purpose of this class is to provide a common base class for streaming out the contents of
/// an MgReader instance via chunked response encoding
/// </summary>
class MG_MAPAGENT_API MgHttpReaderStreamer : public MgGuardDisposable
{
public:
    void StreamResult();
    virtual ~MgHttpReaderStreamer();

protected:
    MgHttpReaderStreamer(MgReader* reader, CREFSTRING format);
    virtual void SetChunkedEncoding();
    virtual void WriteChunk(const char* str, size_t length);
    virtual void Dispose() { delete this; }
    virtual void EndStream();
    
private:
    void ToJson(string& xmlString, string& jsonString);
    Ptr<MgReader> m_reader;
    STRING m_format;
};

#endif
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

#ifndef _BINARYREADER_H
#define _BINARYREADER_H

#ifdef _WIN32
#include <hash_map>
#else
#include <list>
#include <ext/hash_map>
namespace stdext = ::__gnu_cxx;
using namespace std;
#endif

//the initial string buffer is created with this (smallish) size
//if we need bigger, either a new buffer will be allocated or a
//the bigger size will be used if it is the first buffer we
//create
#define STRING_CACHE_SIZE 256

///////////////////////////////////////////////////////////////////////////////
class BinaryReader
{
public:
    BinaryReader(unsigned char* data, int len);
    virtual ~BinaryReader();

    void Reset(unsigned char* data, int len);
    void SetPosition(int offset);
    int GetPosition();
    unsigned char* GetDataAtCurrentPosition();
    unsigned GetDataLen();

    double ReadDouble();
    float ReadSingle();
    short ReadInt16();
    unsigned short ReadUInt16();
    int ReadInt32();
    unsigned ReadUInt32();
    FdoInt64 ReadInt64();
    unsigned char ReadByte();
    char ReadChar();
    wchar_t* ReadString();
    wchar_t* ReadRawString(unsigned mbstrlen);
    FdoDateTime ReadDateTime();
    //add more as needed...

private:
    unsigned char* m_data;
    unsigned m_len;
    unsigned m_pos;

    //current unicode string buffer
    wchar_t* m_wcsCache;
    unsigned m_wcsCacheCurrent;
    unsigned m_wcsCacheLen;

    //maps offset of a string in the data record to an
    //offset of the unicode representation in the m_wcsCache
    stdext::hash_map<int, wchar_t*> m_wcsCachedStrings;

    //since we cannot invalidate pointers to strings we have returned
    //we need to keep all previous caches valid until we are reset or
    //destroyed.
    std::list<wchar_t*> m_stringCaches;
};

#endif

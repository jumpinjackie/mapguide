//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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

#include <stdafx.h>
#include "GwsBinaryFeatureReader.h"
#include "GwsQueryEngineImp.h"

GwsBinaryFeatureReader::GwsBinaryFeatureReader(unsigned char* pBuf, int len, int totalProps)
: m_rdr(pBuf, len)
{
    //read class name
    m_className = m_rdr.ReadString();
    m_fcNameLen = m_rdr.GetPosition();
    m_totalProps = totalProps;
}

GisString* GwsBinaryFeatureReader::QualifiedClassName()
{
    return m_className.c_str();
}

int GwsBinaryFeatureReader::SetPosition(int propertyIndex)
{
    //position where offset to property value is stored
    //remember: we have a m_fcNameLen byte string at the beginning
    //and then offsets for each property
    m_rdr.SetPosition(m_fcNameLen + propertyIndex * sizeof(int));

    //get offset to property value and position there
    int offset = m_rdr.ReadInt32();

    //compute how many bytes the
    //property value spans -- we do this by subtracting the two offsets
    int endoffset = 0;

    if (propertyIndex < m_totalProps - 1)
        endoffset = m_rdr.ReadInt32();
    else
        endoffset = m_rdr.GetDataLen();

    //skip to the beginning of the data value
    m_rdr.SetPosition(offset);

    //the length of the data
    //If it is 0, it means the property value is NULL or equal to
    //the default property value (if specified in the schema)
    int len = endoffset - offset;
    return len;
}

bool GwsBinaryFeatureReader::IsNull(GisInt32 iProp)
{
    int len = SetPosition (iProp);
    return (len == 0) ? true : false;
}

GisString * GwsBinaryFeatureReader::GetString (GisInt32 iProp)
{
    int len = SetPosition (iProp);
    assert(len != 0);
    return m_rdr.ReadRawString(len);
}
bool GwsBinaryFeatureReader::GetBoolean  (GisInt32 iProp)
{
    int len = SetPosition (iProp);
    assert(len != 0);
    len;
    unsigned char ret = m_rdr.ReadByte();
    return (ret != 0);
}

GisByte GwsBinaryFeatureReader::GetByte     (GisInt32 iProp)
{
    int len = SetPosition (iProp);
    assert(len != 0);
    len;
    return m_rdr.ReadByte();
}

GisDateTime GwsBinaryFeatureReader::GetDateTime (GisInt32 iProp)
{
    int len = SetPosition (iProp);
    assert(len != 0);
    len;
    return m_rdr.ReadDateTime();
}

double GwsBinaryFeatureReader::GetDouble   (GisInt32 iProp)
{
    int len = SetPosition (iProp);
    assert(len != 0);
    len;
    return m_rdr.ReadDouble();
}

GisInt16 GwsBinaryFeatureReader::GetInt16    (GisInt32 iProp)
{
    int len = SetPosition (iProp);
    assert(len != 0);
    len;
    return m_rdr.ReadInt16();
}

GisInt32 GwsBinaryFeatureReader::GetInt32    (GisInt32 iProp)
{
    int len = SetPosition (iProp);
    assert(len != 0);
    len;
    return m_rdr.ReadInt32();
}

GisInt64 GwsBinaryFeatureReader::GetInt64    (GisInt32 iProp)
{
    int len = SetPosition (iProp);
    assert(len != 0);
    len;
    return m_rdr.ReadInt64();
}

float GwsBinaryFeatureReader::GetSingle   (GisInt32 iProp)
{
    int len = SetPosition (iProp);
    assert(len != 0);
    len;
    return m_rdr.ReadSingle();
}

const GisByte * GwsBinaryFeatureReader::GetGeometry (GisInt32 iProp, GisInt32 * count)
{
    int len = SetPosition (iProp);
    assert(len != 0);

    void* ptr = m_rdr.GetDataAtCurrentPosition();

    //TODO: we are trying to change FDO to return the simple pointer to
    //the data instead of creating a GisByteArray.
    *count = len;
    return (const GisByte*)ptr;
}

GisByteArray*   GwsBinaryFeatureReader::GetGeometry (GisInt32 iProp)
{
    int len = SetPosition (iProp);
    assert(len != 0);
    void* ptr = m_rdr.GetDataAtCurrentPosition();
    return GisByteArray::Create((unsigned char*)ptr, len);
}
/*
FdoIFeatureReader* GwsBinaryFeatureReader::GetFeatureObject (GisInt32 iProp)
{
    int len = SetPosition (iProp);
    assert(len != 0);
    return m_rdr->GetFeatureObject (desc.m_name.c_str ());
}
*/

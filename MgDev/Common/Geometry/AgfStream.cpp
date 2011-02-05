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

#include "GeometryCommon.h"
#include "AgfStream.h"

//////////////////////////////////////////////////////////////////
// Constructors
//
MgAgfStream::MgAgfStream() : MgStream()
{
}

MgAgfStream::MgAgfStream(MgStreamHelper* streamHelper) : MgStream(streamHelper)
{
}

//////////////////////////////////////////////////////////////////
// This reads the int64 from the stream
//
void MgAgfStream::GetInt64(INT64& value)
{
    MgStreamReader::m_sHelper->GetINT64(value);
}

//////////////////////////////////////////////////////////////////
// This reads the int32 from the stream
//
void MgAgfStream::GetInt32(INT32& value)
{
    UINT32 uVal = 0;
    MgStreamReader::m_sHelper->GetUINT32(uVal);
    value = (INT32) uVal;
}

//////////////////////////////////////////////////////////////////
// This reads the int16 from the stream
//
void MgAgfStream::GetInt16(INT16& value)
{
    UINT16 val;
    MgStreamReader::m_sHelper->GetUINT16(val);
    value = val;
}

//////////////////////////////////////////////////////////////////
// This reads the float from the stream
//
void MgAgfStream::GetSingle(float& value)
{
    MgStreamReader::m_sHelper->GetData(&value, sizeof(float), true, false);
}

//////////////////////////////////////////////////////////////////
// This reads the double from the stream
//
void MgAgfStream::GetDouble(double& value)
{
    MgStreamReader::m_sHelper->GetData(&value, sizeof(double), true, false);
}

//////////////////////////////////////////////////////////////////
// This reads the boolean from the stream
//
void MgAgfStream::GetBoolean(bool& value)
{
    UINT8 tempValue;
    MgStreamReader::m_sHelper->GetUINT8(tempValue);
    value = (bool)tempValue;
}

//////////////////////////////////////////////////////////////////
// This reads the byte from the stream
//
void MgAgfStream::GetByte(BYTE& value)
{
    UINT8 val;
    MgStreamReader::m_sHelper->GetUINT8(val);
    value = val;
}

//////////////////////////////////////////////////////////////////
// This writes the int64 argument packet to the stream
//
MgStreamHelper::MgStreamStatus MgAgfStream::WriteInt64(INT64 value)
{
    return MgStreamWriter::m_sHelper->WriteINT64(value);
}

//////////////////////////////////////////////////////////////////
// This writes the int32 argument packet to the stream
//
MgStreamHelper::MgStreamStatus MgAgfStream::WriteInt32(INT32 value)
{
    return MgStreamWriter::m_sHelper->WriteUINT32((UINT32)value);
}

//////////////////////////////////////////////////////////////////
// This writes the int16 argument packet to the stream
//
MgStreamHelper::MgStreamStatus MgAgfStream::WriteInt16(INT16 value)
{
    return MgStreamWriter::m_sHelper->WriteUINT16(value);
}

//////////////////////////////////////////////////////////////////
// This writes the float argument packet to the stream
//
MgStreamHelper::MgStreamStatus MgAgfStream::WriteSingle(float value)
{
    return MgStreamWriter::m_sHelper->WriteBytes((const unsigned char*)&value, (int)sizeof(float));
}

//////////////////////////////////////////////////////////////////
// This writes the double argument packet to the stream
//
MgStreamHelper::MgStreamStatus MgAgfStream::WriteDouble(double value)
{
    return MgStreamWriter::m_sHelper->WriteBytes((const unsigned char*)&value, (int)sizeof(double));
}

//////////////////////////////////////////////////////////////////
// This writes the boolean argument packet to the stream
//
MgStreamHelper::MgStreamStatus MgAgfStream::WriteBoolean(bool value)
{
    UINT8 intVal = value;
    return MgStreamWriter::m_sHelper->WriteUINT8(intVal);
}

//////////////////////////////////////////////////////////////////
// This writes the byte argument packet to the stream
//
MgStreamHelper::MgStreamStatus MgAgfStream::WriteByte(BYTE value)
{
    return MgStreamWriter::m_sHelper->WriteUINT8((UINT8)value);
}

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
// The following methods are not applicable to the AgfStream. They
// overriden for the sole purpose of throwing an exception when called
// by mistake, rather than producing an unpredictable result.
//
void MgAgfStream::GetStreamHeader(MgStreamHeader& msh)
{
    throw new MgNotImplementedException(L"MgAgfStream.GetStreamHeader", __LINE__, __WFILE__, NULL, L"", NULL);
}

void MgAgfStream::GetOperationHeader(MgOperationPacket& operPacket)
{
    throw new MgNotImplementedException(L"MgAgfStream.GetOperationHeader", __LINE__, __WFILE__, NULL, L"", NULL);
}

void MgAgfStream::GetOperationResponseHeader(MgOperationResponsePacket& mop)
{
    throw new MgNotImplementedException(L"MgAgfStream.GetOperationResponseHeader", __LINE__, __WFILE__, NULL, L"", NULL);
}

void MgAgfStream::GetArgumentHeader(MgArgumentPacket& map)
{
    throw new MgNotImplementedException(L"MgAgfStream.GetArgumentHeader", __LINE__, __WFILE__, NULL, L"", NULL);
}

void MgAgfStream::GetBinaryStreamHeader(MgBinaryStreamArgumentPacket& bsap)
{
    throw new MgNotImplementedException(L"MgAgfStream.GetBinaryStreamHeader", __LINE__, __WFILE__, NULL, L"", NULL);
}

MgByteReader* MgAgfStream::GetStream()
{
    throw new MgNotImplementedException(L"MgAgfStream.GetStream", __LINE__, __WFILE__, NULL, L"", NULL);
}

MgStreamHelper::MgStreamStatus MgAgfStream::GetData(void* value, INT32 length)
{
    throw new MgNotImplementedException(L"MgAgfStream.GetData", __LINE__, __WFILE__, NULL, L"", NULL);
}

void MgAgfStream::GetClassId(INT32& value)
{
    throw new MgNotImplementedException(L"MgAgfStream.GetClassId", __LINE__, __WFILE__, NULL, L"", NULL);
}

void MgAgfStream::GetStreamEnd()
{
    throw new MgNotImplementedException(L"MgAgfStream.GetStreamEnd", __LINE__, __WFILE__, NULL, L"", NULL);
}


MgObject* MgAgfStream::GetObject()
{
    throw new MgNotImplementedException(L"MgAgfStream.GetObject", __LINE__, __WFILE__, NULL, L"", NULL);
}

void MgAgfStream::GetString(REFSTRING wcStr)
{
    throw new MgNotImplementedException(L"MgAgfStream.GetString", __LINE__, __WFILE__, NULL, L"", NULL);
}

MgStreamHelper::MgStreamStatus MgAgfStream::WriteStreamHeader(MgStreamHeader& msh)
{
    throw new MgNotImplementedException(L"MgAgfStream.WriteStreamHeader", __LINE__, __WFILE__, NULL, L"", NULL);
}

MgStreamHelper::MgStreamStatus MgAgfStream::WriteOperationHeader(MgOperationPacket& operPacket)
{
    throw new MgNotImplementedException(L"MgAgfStream.WriteOperationHeader", __LINE__, __WFILE__, NULL, L"", NULL);
}

MgStreamHelper::MgStreamStatus MgAgfStream::WriteBinaryStreamHeader(MgBinaryStreamArgumentPacket& bsap)
{
    throw new MgNotImplementedException(L"MgAgfStream.WriteOperationHeader", __LINE__, __WFILE__, NULL, L"", NULL);
}

MgStreamHelper::MgStreamStatus MgAgfStream::WriteOperationResponseHeader( MgOperationResponsePacket& mop )
{
    throw new MgNotImplementedException(L"MgAgfStream.WriteBinaryStreamHeader", __LINE__, __WFILE__, NULL, L"", NULL);
}

MgStreamHelper::MgStreamStatus MgAgfStream::WriteArgumentHeader(MgArgumentPacket& map)
{
    throw new MgNotImplementedException(L"MgAgfStream.WriteArgumentHeader", __LINE__, __WFILE__, NULL, L"", NULL);
}

MgStreamHelper::MgStreamStatus MgAgfStream::WriteString(CREFSTRING value)
{
    throw new MgNotImplementedException(L"MgAgfStream.WriteString", __LINE__, __WFILE__, NULL, L"", NULL);
}

MgStreamHelper::MgStreamStatus MgAgfStream::WriteStream(MgByteReader* byteReader)
{
    throw new MgNotImplementedException(L"MgAgfStream.WriteStream", __LINE__, __WFILE__, NULL, L"", NULL);
}

MgStreamHelper::MgStreamStatus MgAgfStream::WriteClassId(INT32 value)
{
    throw new MgNotImplementedException(L"MgAgfStream.WriteClassId", __LINE__, __WFILE__, NULL, L"", NULL);
}

MgStreamHelper::MgStreamStatus MgAgfStream::WriteStreamEnd()
{
    throw new MgNotImplementedException(L"MgAgfStream.WriteStreamEnd", __LINE__, __WFILE__, NULL, L"", NULL);
}

MgStreamHelper::MgStreamStatus MgAgfStream::WriteObject(MgObject* obj)
{
    throw new MgNotImplementedException(L"MgAgfStream.WriteObject", __LINE__, __WFILE__, NULL, L"", NULL);
}

MgStreamHelper::MgStreamStatus MgAgfStream::WriteResponseHeader(MgPacketParser::MgECode eCode, UINT32 noOfRetValues)
{
    throw new MgNotImplementedException(L"MgAgfStream.WriteResponseHeader", __LINE__, __WFILE__, NULL, L"", NULL);
}

MgStreamHelper::MgStreamStatus MgAgfStream::WriteControlPacket(MgControlPacket& packet)
{
    throw new MgNotImplementedException(L"MgAgfStream.WriteControlPacket", __LINE__, __WFILE__, NULL, L"", NULL);
}

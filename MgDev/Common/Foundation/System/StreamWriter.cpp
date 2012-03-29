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

#include "Foundation.h"

//////////////////////////////////////////////////////////////////
///<summary>
/// Constructors
///</summary>
MgStreamWriter::MgStreamWriter( MgStreamHelper* streamHelper )
{
    m_sHelper = SAFE_ADDREF(streamHelper);
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Destructor
///</summary>
MgStreamWriter::~MgStreamWriter()
{
    if (m_sHelper != NULL)
    {
        m_sHelper = NULL;
    }
}


//////////////////////////////////////////////////////////////////
///<summary>
/// Writes the stream header
///</summary>
MgStreamHelper::MgStreamStatus MgStreamWriter::WriteStreamHeader(MgStreamHeader& msh)
{
    MgStreamHelper::MgStreamStatus stat = MgStreamHelper::mssDone;

    // Stream header
    if (MgStreamHelper::mssDone == stat) stat = m_sHelper->WriteUINT32( msh.m_streamStart );
    // Stream version
    if (MgStreamHelper::mssDone == stat) stat = m_sHelper->WriteUINT32( msh.m_streamVersion );
    // Stream data header
    if (MgStreamHelper::mssDone == stat) stat = m_sHelper->WriteUINT32( msh.m_streamDataHdr );

    return stat;
}


//////////////////////////////////////////////////////////////////
///<summary>
/// This writes the operation header to the server
///</summary>
///<param name="operPacket">
/// Operation packet to write
///</param>
///<returns>
/// Stream status of write
///</returns>
MgStreamHelper::MgStreamStatus MgStreamWriter::WriteOperationHeader(MgOperationPacket& operPacket)
{
    MgStreamHelper::MgStreamStatus stat = MgStreamHelper::mssDone;

    MG_TRY()
        throw new MgNotImplementedException(L"MgStreamWriter.WriteOperationHeader", __LINE__, __WFILE__, NULL, L"", NULL);
    MG_CATCH_AND_THROW(L"MgStreamWriter.WriteOperationHeader")

    return stat;
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Writes the stream header to the server
///</summary>
MgStreamHelper::MgStreamStatus MgStreamWriter::WriteBinaryStreamHeader(MgBinaryStreamArgumentPacket& bsap)
{
    MgStreamHelper::MgStreamStatus stat = MgStreamHelper::mssDone;

    // Packet header
    if (MgStreamHelper::mssDone == stat) stat = m_sHelper->WriteUINT32(bsap.m_PacketHeader);
    // Argument type
    if (MgStreamHelper::mssDone == stat) stat = m_sHelper->WriteUINT32(bsap.m_ArgumentType);
    // Stream version
    if (MgStreamHelper::mssDone == stat) stat = m_sHelper->WriteUINT32(bsap.m_Version);
    // Stream size
    if (MgStreamHelper::mssDone == stat) stat = m_sHelper->WriteINT64(bsap.m_Length);

    return stat;
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Writes the response header from the server
///</summary>
///<param name="eCode">
/// eCode to send.
///</param>
///<param name="retValues">
/// no. of return arguments .
///</param>
///<returns>
/// Stream status
///</returns>
MgStreamHelper::MgStreamStatus MgStreamWriter::WriteOperationResponseHeader( MgOperationResponsePacket& mop )
{
    MgStreamHelper::MgStreamStatus stat = MgStreamHelper::mssDone;

    if (MgStreamHelper::mssDone == stat) stat = MgStreamWriter::m_sHelper->WriteUINT32( mop.m_PacketHeader );
    // Version
    if (MgStreamHelper::mssDone == stat) stat = MgStreamWriter::m_sHelper->WriteUINT32( mop.m_PacketVersion );
    // Get the code
    if (MgStreamHelper::mssDone == stat) stat = MgStreamWriter::m_sHelper->WriteUINT32( mop.m_ECode );
    // No. of return values
    if (MgStreamHelper::mssDone == stat) stat = MgStreamWriter::m_sHelper->WriteUINT32( mop.m_NumReturnValues );

    return stat;
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Writes the header to the server
///</summary>
MgStreamHelper::MgStreamStatus MgStreamWriter::WriteArgumentHeader(MgArgumentPacket& map)
{
    MgStreamHelper::MgStreamStatus stat = MgStreamHelper::mssDone;

    // Write packet header
    if (MgStreamHelper::mssDone == stat) stat = m_sHelper->WriteUINT32( map.m_PacketHeader );
    // Write argument type
    if (MgStreamHelper::mssDone == stat) stat = m_sHelper->WriteUINT32( map.m_ArgumentType );
    // Write length if it is string
    if (MgStreamHelper::mssDone == stat
        && MgPacketParser::matString == map.m_ArgumentType)
    {
        stat = m_sHelper->WriteUINT32((UINT32)map.m_Length);
    }

    return stat;
}

//////////////////////////////////////////////////////////////////
///<summary>
/// This writes the string argument packet to the server
///</summary>
MgStreamHelper::MgStreamStatus MgStreamWriter::WriteString(CREFSTRING value)
{
    MgStreamHelper::MgStreamStatus stat = MgStreamHelper::mssDone;

    MgArgumentPacket map;
    map.m_PacketHeader = (UINT32)MgPacketParser::mphArgumentSimple;
    map.m_ArgumentType = (UINT32)MgPacketParser::matString;

    stat = WriteArgumentHeader(map);
    if (MgStreamHelper::mssDone == stat) stat = m_sHelper->WriteString(value);

    return stat;
}

//////////////////////////////////////////////////////////////////
///<summary>
/// This writes the int64 argument packet to the server
///</summary>
MgStreamHelper::MgStreamStatus MgStreamWriter::WriteInt64(INT64 value)
{
    MgStreamHelper::MgStreamStatus stat = MgStreamHelper::mssDone;

    MgArgumentPacket map;
    map.m_PacketHeader = (UINT32)MgPacketParser::mphArgumentSimple;
    map.m_ArgumentType = (UINT32)MgPacketParser::matINT64;
    map.m_Length = sizeof(INT64);

    stat = WriteArgumentHeader(map);

    if (MgStreamHelper::mssDone == stat) stat = m_sHelper->WriteINT64(value);

    return stat;
}


//////////////////////////////////////////////////////////////////
///<summary>
/// This writes the int32 argument packet to the server
///</summary>
MgStreamHelper::MgStreamStatus MgStreamWriter::WriteInt32(INT32 value)
{
    MgStreamHelper::MgStreamStatus stat = MgStreamHelper::mssDone;

    MgArgumentPacket map;
    map.m_PacketHeader = (UINT32)MgPacketParser::mphArgumentSimple;
    map.m_ArgumentType = (UINT32)MgPacketParser::matINT32;
    map.m_Length = sizeof(UINT32);

    stat = WriteArgumentHeader(map);
    if (MgStreamHelper::mssDone == stat) stat = m_sHelper->WriteUINT32((UINT32)value);

    return stat;
}

//////////////////////////////////////////////////////////////////
///<summary>
/// This writes the int16 argument packet to the server
///</summary>
///<param name="value">
/// value to be written.
///</param>
///<returns>
///Nothing
///</returns>
MgStreamHelper::MgStreamStatus MgStreamWriter::WriteInt16(INT16 value)
{
    MgStreamHelper::MgStreamStatus stat = MgStreamHelper::mssDone;

    MgArgumentPacket map;
    map.m_PacketHeader = (UINT32)MgPacketParser::mphArgumentSimple;
    map.m_ArgumentType = (UINT32)MgPacketParser::matINT16;
    map.m_Length = sizeof(INT16);

    stat = WriteArgumentHeader(map);
    if (MgStreamHelper::mssDone == stat) stat = m_sHelper->WriteUINT16(value);

    return stat;
}


//////////////////////////////////////////////////////////////////
///<summary>
/// This writes the float argument packet to the server
///</summary>
MgStreamHelper::MgStreamStatus MgStreamWriter::WriteSingle(float value)
{

    MgStreamHelper::MgStreamStatus stat = MgStreamHelper::mssDone;

    MgArgumentPacket map;
    map.m_PacketHeader = (UINT32)MgPacketParser::mphArgumentSimple;
    map.m_ArgumentType = (UINT32)MgPacketParser::matFloat;
    map.m_Length = sizeof(float);

    stat = WriteArgumentHeader(map);

    if (MgStreamHelper::mssDone == stat) stat = m_sHelper->WriteBytes((const unsigned char*)&value, (int) map.m_Length);

    return stat;
}

//////////////////////////////////////////////////////////////////
///<summary>
/// This writes the double argument packet to the server
///</summary>
MgStreamHelper::MgStreamStatus MgStreamWriter::WriteDouble(double value)
{
    MgStreamHelper::MgStreamStatus stat = MgStreamHelper::mssDone;

    MgArgumentPacket map;
    map.m_PacketHeader = (UINT32)MgPacketParser::mphArgumentSimple;
    map.m_ArgumentType = (UINT32)MgPacketParser::matDouble;
    map.m_Length = sizeof(double);

    stat = WriteArgumentHeader(map);
    if (MgStreamHelper::mssDone == stat) stat = m_sHelper->WriteBytes((const unsigned char*)&value, (int) map.m_Length);

    return stat;
}

//////////////////////////////////////////////////////////////////
///<summary>
/// This writes the boolean argument packet to the server
///</summary>
MgStreamHelper::MgStreamStatus MgStreamWriter::WriteBoolean(bool value)
{
    MgStreamHelper::MgStreamStatus stat = MgStreamHelper::mssDone;

    MgArgumentPacket map;
    map.m_PacketHeader = (UINT32)MgPacketParser::mphArgumentSimple;
    map.m_ArgumentType = (UINT32)MgPacketParser::matINT8;
    map.m_Length = sizeof(INT8);
    UINT8 intVal = value;

    stat = WriteArgumentHeader(map);
    if (MgStreamHelper::mssDone == stat) stat = m_sHelper->WriteUINT8(intVal);

    return stat;
}

//////////////////////////////////////////////////////////////////
///<summary>
/// This writes the byte argument packet to the server
///</summary>
MgStreamHelper::MgStreamStatus MgStreamWriter::WriteByte(BYTE value)
{
    MgStreamHelper::MgStreamStatus stat = MgStreamHelper::mssDone;

    MgArgumentPacket map;
    map.m_PacketHeader = (UINT32)MgPacketParser::mphArgumentSimple;
    map.m_ArgumentType = (UINT32)MgPacketParser::matINT8;
    map.m_Length = sizeof(BYTE);

    stat = WriteArgumentHeader(map);
    if (MgStreamHelper::mssDone == stat) stat = m_sHelper->WriteUINT8((UINT8)value);

    return stat;
}

//////////////////////////////////////////////////////////////////
///<summary>
/// This writes the stream to the server
///</summary>
MgStreamHelper::MgStreamStatus MgStreamWriter::WriteStream(MgByteReader* byteReader)
{
    MgStreamHelper::MgStreamStatus stat = MgStreamHelper::mssDone;

    MgBinaryStreamArgumentPacket bsap;

    // Packet header
    bsap.m_PacketHeader = (UINT32)MgPacketParser::matStream;
    // Argument type
    bsap.m_ArgumentType = (UINT32)MgPacketParser::mphArgumentBinaryStream;
    // Stream version
    bsap.m_Version = MG_STREAM_VERSION;
    // Stream size
    bsap.m_Length = 0;

    if (byteReader != NULL)
    {
        bsap.m_Length = byteReader->GetLength();
    }

    stat = WriteBinaryStreamHeader(bsap);

    // We need to send flag here because '0' length byte reader is a possible
    // scenario where byte reader may not be null. To handle both situations
    // correctly, we need to inform that whether it is null or zero byte based bytereader.
    bool isNullByteReader = false;
    if (byteReader != NULL)
    {
        WriteBoolean(isNullByteReader); // Non-Null byte reader
    }
    else
    {
        isNullByteReader = true;
        WriteBoolean(isNullByteReader); // Null object
    }

    if (byteReader != NULL)
    {
        unsigned char buf[8192];
        INT32 len = byteReader->Read(buf,8192);

        while (len != 0 && MgStreamHelper::mssDone == stat)
        {
            // Write the length
            stat = m_sHelper->WriteUINT32(len);
            // Write the data
            if (MgStreamHelper::mssDone == stat) stat = m_sHelper->WriteBytes(buf, len);

            if (MgStreamHelper::mssDone == stat) len = byteReader->Read(buf,8192);
        }

        UINT32 end = 0;
        if (MgStreamHelper::mssDone == stat) stat = m_sHelper->WriteUINT32(end);
    }

    return stat;
}

//////////////////////////////////////////////////////////////////
///<summary>
/// This writes the class id argument packet to the server
///</summary>
MgStreamHelper::MgStreamStatus MgStreamWriter::WriteClassId(INT32 value)
{
    MgStreamHelper::MgStreamStatus stat = MgStreamHelper::mssDone;

    MgArgumentPacket map;
    map.m_PacketHeader = (UINT32)MgPacketParser::mphArgumentSimple;
    map.m_ArgumentType = (UINT32)MgPacketParser::matClassId;
    map.m_Length = sizeof(INT32);

    stat = WriteArgumentHeader(map);
    if (MgStreamHelper::mssDone == stat) stat = m_sHelper->WriteUINT32((UINT32)value);

    return stat;
}

///<summary>
/// Writes the end of stream header
///</summary>
MgStreamHelper::MgStreamStatus MgStreamWriter::WriteStreamEnd()
{
    MgStreamHelper::MgStreamStatus stat = m_sHelper->WriteUINT32( MgStreamParser::mshStreamEnd );
    m_sHelper->Flush();

   return stat;
}

///<summary>
/// Writes the Object to stream
///</summary>
MgStreamHelper::MgStreamStatus MgStreamWriter::WriteObject(MgObject* obj)
{
    MgStreamHelper::MgStreamStatus stat = MgStreamHelper::mssDone;

    //serialize this object or just send a class id representing a NULL object
    if(obj != NULL)
    {
        stat = WriteClassId(obj->GetClassId());
        //serialize the object
        if (MgStreamHelper::mssDone == stat) ((MgSerializable*)obj)->Serialize((MgStream*)this);
    }
    else
    {
        stat = WriteClassId(0);
    }

    return stat;
}


MgStreamHelper::MgStreamStatus MgStreamWriter::WriteResponseHeader(MgPacketParser::MgECode eCode, UINT32 noOfRetValues)
{
    MgStreamHelper::MgStreamStatus stat = MgStreamHelper::mssDone;

    MgStreamHeader msh;
    msh.m_streamStart = (UINT32)MgStreamParser::mshStreamStart;
    msh.m_streamVersion = MgStreamParser::StreamVersion;
    msh.m_streamDataHdr = (UINT32)MgStreamParser::mshStreamData;

    stat = WriteStreamHeader(msh);

    MgOperationResponsePacket morp;
    morp.m_ECode = (UINT32)eCode;
    morp.m_NumReturnValues = noOfRetValues;
    morp.m_PacketHeader = MgPacketParser::mphOperationResponse;
    morp.m_PacketVersion = 1;

    if (MgStreamHelper::mssDone == stat) stat = WriteOperationResponseHeader(morp);

    return stat;
}

MgStreamHelper::MgStreamStatus MgStreamWriter::WriteControlPacket(MgControlPacket& packet)
{
    MgStreamHelper::MgStreamStatus stat = MgStreamHelper::mssDone;

    // Send stream header
    MgStreamHeader msh;
    msh.m_streamStart = (UINT32) MgStreamParser::mshStreamStart;
    msh.m_streamVersion = MgStreamParser::StreamVersion;
    msh.m_streamDataHdr = MgStreamParser::mshStreamData;

    stat = WriteStreamHeader(msh);

    // Send control packet
    m_sHelper->WriteUINT32((UINT32)packet.m_PacketHeader);
    m_sHelper->WriteUINT32((UINT32)packet.m_PacketVersion);
    m_sHelper->WriteUINT32((UINT32)packet.m_ControlID);

    // End stream
    if (MgStreamHelper::mssDone == stat) stat = WriteStreamEnd();

    return stat;
}

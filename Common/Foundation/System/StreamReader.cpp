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

#include "Foundation.h"
#include "ByteSourceSocketStreamImpl.h"


//////////////////////////////////////////////////////////////////
///<summary>
/// Constructors
///</summary>
///<param name="streamHelper">
/// Stream helper object holding ACE_SOCK_STREAM
///</param>
MgStreamReader::MgStreamReader( MgStreamHelper* streamHelper )
:
m_conn(NULL)
{
    m_sHelper = SAFE_ADDREF(streamHelper);
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Destructor
///</summary>
MgStreamReader::~MgStreamReader()
{
    if (m_sHelper != NULL)
    {
        m_sHelper = NULL;
    }
    m_conn = NULL;
}

///<summary>
/// Reads the stream header
///</summary>
///<param name="msh">
///Stream header to hold the data
///<param>
void MgStreamReader::GetStreamHeader(MgStreamHeader& msh)
{
    CHECKNULL(m_sHelper, L"GetStreamHeader");

    // Stream header
    m_sHelper->GetUINT32( msh.m_streamStart );
    // Stream version
    m_sHelper->GetUINT32( msh.m_streamVersion );
    // Stream data header
    m_sHelper->GetUINT32( msh.m_streamDataHdr );
}

//////////////////////////////////////////////////////////////////
///<summary>
/// This writes the operation header to the server
///</summary>
///<param name="serviceId">
/// serviceId like ResourceService, DrawingService etc.
///</param>
///<returns>
///Nothing
///</returns>
void MgStreamReader::GetOperationHeader(MgOperationPacket& operPacket)
{
    MG_TRY()
        throw new MgNotImplementedException(L"MgStreamReader.GetOperationHeader", __LINE__, __WFILE__, NULL, L"", NULL);
    MG_CATCH_AND_THROW(L"MgStreamReader.GetOperationHeader")
}


//////////////////////////////////////////////////////////////////
///<summary>
/// Gets the response header from the server
///</summary>
///<param name="packetHeader">
/// Outparameter , packet header received.
///</param>
void MgStreamReader::GetOperationResponseHeader(MgOperationResponsePacket& mop)
{
    MG_TRY()
        throw new MgNotImplementedException(L"MgStreamReader.GetOperationResponseHeader", __LINE__, __WFILE__, NULL, L"", NULL);
    MG_CATCH_AND_THROW(L"MgStreamReader.GetOperationResponseHeader")
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Gets the header from the server
///</summary>
void MgStreamReader::GetArgumentHeader(MgArgumentPacket& map)
{
    CHECKNULL(m_sHelper, L"ReadArgumentHeader");

    // Read packet header
    m_sHelper->GetUINT32( map.m_PacketHeader, true, false );
    // Read argument type
    m_sHelper->GetUINT32( map.m_ArgumentType, true, false );
    // Read length if it is string
    if (map.m_ArgumentType == (UINT32)MgPacketParser::matString)
    {
        UINT32 strLen = 0;
        m_sHelper->GetUINT32(strLen);
        map.m_Length = strLen;
    }
}


void MgStreamReader::GetBinaryStreamHeader(MgBinaryStreamArgumentPacket& bsap)
{
    CHECKNULL(m_sHelper, L"GetBinaryStreamHeader");

    // Packet header
    m_sHelper->GetUINT32(bsap.m_PacketHeader);
    // Argument type
    m_sHelper->GetUINT32(bsap.m_ArgumentType);
    // Stream version
    m_sHelper->GetUINT32(bsap.m_Version);
    // Stream size
    m_sHelper->GetINT64(bsap.m_Length);
}


//////////////////////////////////////////////////////////////////
///<summary>
/// This reads the string from the server
///</summary>
///<param name="value">
/// Value to be read
///</param>
///<returns>
///Nothing
///</returns>
void MgStreamReader::GetString(REFSTRING wcStr)
{
    CHECKNULL(m_sHelper, L"GetString");

    string str;
    MgArgumentPacket map;

    GetArgumentHeader(map);

    if (map.m_ArgumentType != (UINT32)MgPacketParser::matString)
        throw new MgInvalidStreamHeaderException(L"GetString", __LINE__, __WFILE__, NULL, L"", NULL);

    str.resize((size_t)map.m_Length); // resize string to hold the required bytes
    m_sHelper->GetData( (void*)str.c_str(), (size_t)map.m_Length, true, false );
    MgUtil::MultiByteToWideChar(str, wcStr);
}

//////////////////////////////////////////////////////////////////
///<summary>
/// This reads the int64 from the server
///</summary>
///<param name="value">
/// Value to be read
///</param>
///<returns>
///Nothing
///</returns>
void MgStreamReader::GetInt64(INT64& value)
{
    CHECKNULL(m_sHelper, L"GetInt64");

    MgArgumentPacket map;
    GetArgumentHeader(map);

    if (map.m_ArgumentType != (UINT32)MgPacketParser::matINT64)
        throw new MgInvalidStreamHeaderException(L"GetInt64", __LINE__, __WFILE__, NULL, L"", NULL);

    m_sHelper->GetINT64(value);
}

//////////////////////////////////////////////////////////////////
///<summary>
/// This reads the int32 from the server
///</summary>
///<param name="value">
/// Value to be read
///</param>
///<returns>
///Nothing
///</returns>
void MgStreamReader::GetInt32(INT32& value)
{
    CHECKNULL(m_sHelper, L"GetInt32");

    MgArgumentPacket map;
    GetArgumentHeader(map);

    if (map.m_ArgumentType != (UINT32)MgPacketParser::matINT32)
        throw new MgInvalidStreamHeaderException(L"GetInt32", __LINE__, __WFILE__, NULL, L"", NULL);

    UINT32 uVal = 0;
    m_sHelper->GetUINT32(uVal);
    value = (INT32) uVal;
}

//////////////////////////////////////////////////////////////////
///<summary>
/// This reads the int16 from the server
///</summary>
///<param name="value">
/// Value to be read
///</param>
///<returns>
///Nothing
///</returns>
void  MgStreamReader::GetInt16(INT16& value)
{
    CHECKNULL(m_sHelper, L"GetInt16");

    MgArgumentPacket map;
    GetArgumentHeader(map);

    if (map.m_ArgumentType != (UINT32)MgPacketParser::matINT16)
        throw new MgInvalidStreamHeaderException(L"GetInt16", __LINE__, __WFILE__, NULL, L"", NULL);

    UINT16 val;
    m_sHelper->GetUINT16(val);
    value = val;
}


//////////////////////////////////////////////////////////////////
///<summary>
/// This reads the float from the server
///</summary>
///<param name="value">
/// Value to be read
///</param>
///<returns>
///Nothing
///</returns>
void MgStreamReader::GetSingle(float& value)
{
    CHECKNULL(m_sHelper, L"GetSingle");

    MgArgumentPacket map;
    GetArgumentHeader(map);

    if (map.m_ArgumentType != (UINT32)MgPacketParser::matFloat)
        throw new MgInvalidStreamHeaderException( L"GetSingle", __LINE__, __WFILE__, NULL, L"", NULL);

    m_sHelper->GetData( &value, sizeof(float), true, false );
}

//////////////////////////////////////////////////////////////////
///<summary>
/// This reads the double from the server
///</summary>
///<param name="value">
/// Value to be read
///</param>
///<returns>
///Nothing
///</returns>
void MgStreamReader::GetDouble(double& value)
{
    CHECKNULL(m_sHelper, L"GetDouble");

    MgArgumentPacket map;
    GetArgumentHeader(map);

    if (map.m_ArgumentType != (UINT32)MgPacketParser::matDouble)
        throw new MgInvalidStreamHeaderException( L"GetDouble", __LINE__, __WFILE__, NULL, L"", NULL);

    m_sHelper->GetData( &value, sizeof(double), true, false );
}

//////////////////////////////////////////////////////////////////
///<summary>
/// This reads the boolean from the server
///</summary>
///<param name="value">
/// Value to be read
///</param>
///<returns>
///Nothing
///</returns>
void MgStreamReader::GetBoolean(bool& value)
{
    CHECKNULL(m_sHelper, L"GetBoolean");

    MgArgumentPacket map;
    GetArgumentHeader(map);

    if (map.m_ArgumentType != (UINT32)MgPacketParser::matINT8)
        throw new MgInvalidStreamHeaderException( L"GetBoolean", __LINE__, __WFILE__, NULL, L"", NULL);

    UINT8 tempValue;
    m_sHelper->GetUINT8(tempValue);
    value = (bool)tempValue;
}

//////////////////////////////////////////////////////////////////
///<summary>
/// This reads the byte from the server
///</summary>
///<param name="value">
/// Value to be read
///</param>
///<returns>
///Nothing
///</returns>
void MgStreamReader::GetByte(BYTE& value)
{
    CHECKNULL(m_sHelper, L"GetByte");

    MgArgumentPacket map;
    GetArgumentHeader(map);

    if (map.m_ArgumentType != (UINT32)MgPacketParser::matINT8)
        throw new MgInvalidStreamHeaderException( L"GetByte", __LINE__, __WFILE__, NULL, L"", NULL);

    UINT8 val;
    m_sHelper->GetUINT8(val);
    value = val;
}

//////////////////////////////////////////////////////////////////
///<summary>
/// This reads the stream from the server
///</summary>
///<returns>
/// ByteReader
///</returns>
MgByteReader* MgStreamReader::GetStream(bool enableDirect)
{
    CHECKNULL(m_sHelper, L"GetStream");

    Ptr<MgByteReader> byteReader;
    MgBinaryStreamArgumentPacket bsap;

    GetBinaryStreamHeader(bsap);

    if (bsap.m_ArgumentType != (UINT32)MgPacketParser::mphArgumentBinaryStream)
        throw new MgInvalidStreamHeaderException( L"GetStream", __LINE__, __WFILE__, NULL, L"", NULL);

    bool isNullByteReader = false;
    GetBoolean(isNullByteReader);

    if (isNullByteReader)
    {
        return NULL;
    }

    ACE_HANDLE file = ACE_INVALID_HANDLE;
    Ptr<MgByte> bytes;
    STRING tempName;

    if (m_conn != NULL && enableDirect)
    {
        // Direct streaming semantics.  If our stream is backed by a ServerConnection
        // then we must be reading a server response.  Pass control of the ServerConnection
        // to our direct bound stream implementation
        ByteSourceSocketStreamImpl* socketSource = new ByteSourceSocketStreamImpl(bsap.m_Length, GetConnection());
        Ptr<MgByteSource> ptrByteSource = new MgByteSource(socketSource);
        byteReader = ptrByteSource->GetReader();

        return byteReader.Detach();
    }
    else if (bsap.m_Length > MgByte::MaxSize)
    {
        // Very large data set.  Use a file backed stream instead of
        // sucking back huge globs of virtual memory.
        tempName = MgFileUtil::GenerateTempFileName();

        file = ACE_OS::creat(MG_WCHAR_TO_TCHAR(tempName), 0);
        if (ACE_INVALID_HANDLE == file)
        {
            MgStringCollection arguments;
            arguments.Add(tempName);
            if(errno == EEXIST)
                throw new MgFileNotFoundException(L"GetStream", __LINE__, __WFILE__, &arguments, L"", NULL);
            else
                throw new MgFileIoException(L"GetStream", __LINE__, __WFILE__, &arguments, L"", NULL);
        }
    }
    else
    {
        bytes = new MgByte(NULL, (INT32)bsap.m_Length);
    }


    char* temp = NULL;
    UINT32 allocLen = 0;
    UINT32 len = 0;

    do
    {
        // Read the length
        m_sHelper->GetUINT32( len, true, false );
        if ( len > 0 )
        {
            if (len > allocLen)
            {
                if (temp != NULL) { delete [] temp; }
                temp = new char[len];
                allocLen = len;
            }

            // Read the data
            MgStreamHelper::MgStreamStatus stat = GetData(temp, len);
            if (MgStreamHelper::mssDone == stat)
            {
                if (ACE_INVALID_HANDLE == file)
                {
                    bytes->Append((BYTE_ARRAY_IN)temp, len);
                }
                else
                {
                    UINT32 dataWritten = 0;
                    while (dataWritten < len)
                    {
                        int written = ACE_OS::write(file, &temp[dataWritten], len-dataWritten);
                        if (written > 0)
                        {
                            dataWritten += written;
                        }
                    }
                }
            }
        }
    } while ( len > 0 );

    delete [] temp;

    if (ACE_INVALID_HANDLE == file)
    {
        Ptr<MgByteSource> ptrByteSource = new MgByteSource(bytes);
        byteReader = ptrByteSource->GetReader();
    }
    else
    {
        ACE_OS::close(file);
        Ptr<MgByteSource> ptrByteSource = new MgByteSource(tempName, true);
        byteReader = ptrByteSource->GetReader();
    }

    return byteReader.Detach();
}

//////////////////////////////////////////////////////////////////
///<summary>
/// This reads the data from the server
///</summary>
///<param name="value">
/// Value to be read
///</param>
///<returns>
/// size of data read
///</returns>
MgStreamHelper::MgStreamStatus MgStreamReader::GetData(void* value, INT32 length)
{
    CHECKNULL(m_sHelper, L"GetData");

    return m_sHelper->GetData( value, length , true, false);
}

//////////////////////////////////////////////////////////////////
///<summary>
/// This reads the class id argument packet from the server
///</summary>
///<param name="value">
/// value to be read.
///</param>
///<returns>
///Nothing
///</returns>
void MgStreamReader::GetClassId(INT32& value)
{
    CHECKNULL(m_sHelper, L"GetClassId");

    MgArgumentPacket map;
    GetArgumentHeader(map);

    if ((UINT32)MgPacketParser::matClassId != map.m_ArgumentType)
        throw new MgInvalidStreamHeaderException( L"GetClassId", __LINE__, __WFILE__, NULL, L"", NULL);

    UINT32 uVal = 0;
    m_sHelper->GetUINT32(uVal);
    value = (INT32) uVal;
}

///<summary>
/// Reads the end of stream header
///</summary>
///<returns>
/// Nothing
///</returns>
void MgStreamReader::GetStreamEnd()
{
    CHECKNULL(m_sHelper, L"GetStreamEnd");

    UINT32 dataRead;
    m_sHelper->GetUINT32( dataRead );

    if ( MgStreamParser::mshStreamEnd != dataRead )
        throw new MgInvalidStreamHeaderException( L"GetStreamEnd", __LINE__, __WFILE__, NULL, L"", NULL);
}


///<summary>
/// Reads the Object from stream
///</summary>
///<returns>
/// MgObject pointer
///</returns>
MgObject* MgStreamReader::GetObject()
{
    INT32 classId;
    GetClassId(classId);

    if(classId == 0)        //NULL pointer to object
        return NULL;

    Ptr<MgSerializable> obj = (MgSerializable*)MgUtil::CreateMgObject(classId); // throws NullPointerException
    //deserialize the object
    (obj)->Deserialize((MgStream*)this);

    return obj.Detach();
}

///<summary>
/// Returns the stream helper contained by this stream
///</summary>
///
MgStreamHelper* MgStreamReader::GetStreamHelper()
{
    return SAFE_ADDREF((MgStreamHelper*)m_sHelper);
}

///<summary>
/// Returns the connection this stream belongs to
///</summary>
///
MgServerConnectionBase* MgStreamReader::GetConnection()
{
    return SAFE_ADDREF(m_conn);
}

///<summary>
/// Sets the connection this stream belongs to
///</summary>
///
void MgStreamReader::SetConnection(MgServerConnectionBase* conn)
{
    m_conn = conn;
}

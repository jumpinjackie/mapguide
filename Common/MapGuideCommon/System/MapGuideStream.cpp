//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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

MG_IMPL_DYNCREATE(MgMapGuideStream)

/////////////////////////////////////////////////////////////////
///<summary>
/// Default Constructor
///</summary>
MgMapGuideStream::MgMapGuideStream() : MgStream(NULL)
{}

/////////////////////////////////////////////////////////////////
///<summary>
/// Constructors
///</summary>
///<param name="streamHelper">
/// Stream helper object holding ACE_SOCK_STREAM
///</param>
MgMapGuideStream::MgMapGuideStream( MgStreamHelper* streamHelper ) : MgStream(streamHelper)
{
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Destructor
///</summary>
MgMapGuideStream::~MgMapGuideStream()
{
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Self destructing method
///</summary>
 void MgMapGuideStream::Dispose()
{
    delete this;
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
void MgMapGuideStream::GetOperationHeader(MgOperationPacket& operPacket)
{
    CHECKNULL(MgStreamReader::m_sHelper, L"GetOperationHeader");

    // Packet header
    MgStreamReader::m_sHelper->GetUINT32(operPacket.m_PacketHeader);
    // Packet version
    MgStreamReader::m_sHelper->GetUINT32(operPacket.m_PacketVersion);
    // ServiceId
    MgStreamReader::m_sHelper->GetUINT32(operPacket.m_ServiceID);
    // Operation Id
    MgStreamReader::m_sHelper->GetUINT32(operPacket.m_OperationID);
    // Operation Version
    MgStreamReader::m_sHelper->GetUINT32(operPacket.m_OperationVersion);
    // Arguments
    MgStreamReader::m_sHelper->GetUINT32(operPacket.m_NumArguments);
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Gets the response header from the server
///</summary>
///<param name="packetHeader">
/// Outparameter , packet header received.
///</param>
void MgMapGuideStream::GetOperationResponseHeader(MgOperationResponsePacket& mop)
{
    CHECKNULL(MgStreamReader::m_sHelper, L"GetOperationResponseHeader");

    MgStreamReader::m_sHelper->GetUINT32( mop.m_PacketHeader, true, false );
    // Version
    MgStreamReader::m_sHelper->GetUINT32( mop.m_PacketVersion, true, false );
    // Get the code
    MgStreamReader::m_sHelper->GetUINT32( mop.m_ECode, true, false );
    // No. of return values
    MgStreamReader::m_sHelper->GetUINT32( mop.m_NumReturnValues, true, false );
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
MgStreamHelper::MgStreamStatus MgMapGuideStream::WriteOperationHeader(MgOperationPacket& operPacket)
{
    MgStreamHelper::MgStreamStatus stat = MgStreamHelper::mssDone;

    // Packet header
    if (MgStreamHelper::mssDone == stat) stat = MgStreamWriter::m_sHelper->WriteUINT32(operPacket.m_PacketHeader);
    // Packet version
    if (MgStreamHelper::mssDone == stat) stat = MgStreamWriter::m_sHelper->WriteUINT32(operPacket.m_PacketVersion);
    // ServiceId
    if (MgStreamHelper::mssDone == stat) stat = MgStreamWriter::m_sHelper->WriteUINT32(operPacket.m_ServiceID);
    // Operation Id
    if (MgStreamHelper::mssDone == stat) stat = MgStreamWriter::m_sHelper->WriteUINT32(operPacket.m_OperationID);
    // Operation Version
    if (MgStreamHelper::mssDone == stat) stat = MgStreamWriter::m_sHelper->WriteUINT32(operPacket.m_OperationVersion);

    // # arguments, first argument is user information
    if (MgStreamHelper::mssDone == stat) stat = MgStreamWriter::m_sHelper->WriteUINT32(operPacket.m_NumArguments+1);

    // UserInformation
    if (MgStreamHelper::mssDone == stat) stat = WriteObject(operPacket.m_UserInfo);

    return stat;
}


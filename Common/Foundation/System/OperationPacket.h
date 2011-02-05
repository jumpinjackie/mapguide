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

#ifndef MGOPERATIONPACKET_H_MG
#define MGOPERATIONPACKET_H_MG

#include "BasicPacket.h"
class MgUserInformation;

/// \cond INTERNAL
//  Operation Packet Wrapper Structure
struct MgOperationPacket : public MgBasicPacket
{
    MgOperationPacket()
    :
    m_PacketVersion(0),
    m_ServiceID(0),
    m_OperationID(0),
    m_OperationVersion(0),
    m_NumArguments(0),
    m_UserInfo(NULL)
    {
    }

    UINT32 m_PacketVersion;
    UINT32 m_ServiceID;
    UINT32 m_OperationID;
    UINT32 m_OperationVersion;
    UINT32 m_NumArguments;
    MgUserInformation* m_UserInfo;
};
/// \endcond

#endif  //  MGOPERATIONPACKET_H_MG

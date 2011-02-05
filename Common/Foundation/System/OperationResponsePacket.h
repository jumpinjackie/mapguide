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

#ifndef MGOPERATIONRESPONSEPACKET_H_MG
#define MGOPERATIONRESPONSEPACKET_H_MG

#include "BasicPacket.h"

/// \cond INTERNAL
//  Operation Response Packet Wrapper Structure
struct MgOperationResponsePacket : public MgBasicPacket
{
    MgOperationResponsePacket()
    :
    m_PacketVersion(0),
    m_ECode(0),
    m_NumReturnValues(0)
    {
    }

    UINT32 m_PacketVersion;
    UINT32 m_ECode;
    UINT32 m_NumReturnValues;
};
/// \endcond

#endif  //  MGOPERATIONRESPONSEPACKET_H_MG

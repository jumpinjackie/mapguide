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

#ifndef MGARGUMENTPACKET_H_MG
#define MGARGUMENTPACKET_H_MG

#include "BasicPacket.h"

/// \cond INTERNAL
///////////////////////////////////////////////////////////
//  Argument Packet Wrapper Structure
struct MgArgumentPacket : public MgBasicPacket
{
    MgArgumentPacket()
    :
    m_ArgumentType(0),
    m_pData(NULL),
    m_Length(0)
    {
    }

    virtual ~MgArgumentPacket()
    {
        delete[] m_pData;
        m_pData = NULL;
    }

    UINT32      m_ArgumentType;
    UINT8*      m_pData;
    INT64       m_Length;
};
/// \endcond

#endif  //  MGARGUMENTPACKET_H_MG

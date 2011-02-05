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

//-------------------------------------------------------------------------
//  Constructors and Destructors
//-------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////
//  <summary>
//  The default constructor for a MgPacketParser object.  However, since
//  this method is protected and this class is merely a wrapper for various
//  static methods, this constructor should never be called.
//  </summary>
MgPacketParser::MgPacketParser( void )
{
};

//-------------------------------------------------------------------------
//  Methods
//-------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////
//  <summary>
//  This static method reads the packet header from the given input data
//  stream.  It does not consume any information from the stream.
//  </summary>
//
//  <param name="pData">
//  A pointer the the MgStreamData object that encapsulates the incoming and
//  outgoing stream information.
//  </param>
//
//  <returns>
//  The PacketHeader enumeration member that has been read from the
//  incoming data stream.
//  </returns>
MgPacketParser::MgPacketHeader MgPacketParser::GetPacketHeader( MgStreamData* pData )
{
    ACE_ASSERT( pData );

    MgPacketParser::MgPacketHeader ret = MgPacketParser::mphUnknown;

    if ( pData )
    {
        MgStreamHelper* pHelper = pData->GetStreamHelper();

        UINT32 header = 0;

        MgStreamHelper::MgStreamStatus stat = pHelper->GetUINT32( header, true, true );

        if ( MgStreamHelper::mssDone == stat )
        {
            ret = (MgPacketParser::MgPacketHeader) header;
        };
    };

    return ret;
};


bool MgPacketParser::GetControlPacket( MgStreamData* pData, MgControlPacket* pPacket )
{
    ACE_ASSERT( pData );

    MgStreamHelper::MgStreamStatus stat = MgStreamHelper::mssError;

    if ( pData )
    {
        MgStreamHelper* pHelper = pData->GetStreamHelper();

        UINT64 dataread = pData->GetDataRead();
        UINT32 buffer = 0;

        // read packet header
        stat = pHelper->GetUINT32( buffer, true, false );
        if ( MgStreamHelper::mssDone == stat )
        {
            dataread += sizeof( buffer );
            pPacket->m_PacketHeader = buffer;
        }

        // read version
        stat = pHelper->GetUINT32( buffer, true, false );
        if ( MgStreamHelper::mssDone == stat )
        {
            dataread += sizeof( buffer );
            pPacket->m_PacketVersion = buffer;
        }

        // read control id
        stat = pHelper->GetUINT32( buffer, true, false );
        if ( MgStreamHelper::mssDone == stat )
        {
            dataread += sizeof( buffer );
            pPacket->m_ControlID = buffer;
        }

        //  update read totals
        pData->SetDataRead( dataread );
    }

    return ( stat != MgStreamHelper::mssDone ) ? false : true;
}

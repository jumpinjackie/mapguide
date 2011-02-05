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
//  The default constructor for a MgStreamParser object.  However, since
//  this method is protected and this class is merely a wrapper for various
//  static methods, this constructor should never be called.
//  </summary>
MgStreamParser::MgStreamParser( void )
{
};

//-------------------------------------------------------------------------
//  Public Methods
//-------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////
//  <summary>
//  This static method reads and consumes the STREAM START header from the
//  stream encapsulted by the given StreamData* parameter.  It sets the
//  version of the StreamData object during the read.
//  </summary>
//
//  <param name = "pStreamData">
//  A pointer the the StreamData object that encapsulates the incoming and
//  outgoing stream information.
//  </param>
//
//  <returns>
//  Returns true if successful; false otherwise.
//  </returns>
bool MgStreamParser::ParseStreamHeader( MgStreamData* pStreamData )
{
    ACE_ASSERT( pStreamData );

    bool ret = false;

    if ( pStreamData )
    {
        MgStreamHelper* pHelper = pStreamData->GetStreamHelper();

        UINT32 header = 0;

        MgStreamHelper::MgStreamStatus stat = pHelper->GetUINT32( header, true , false );

        if ( MgStreamHelper::mssDone == stat && MgStreamParser::mshStreamStart == header )
        {
            UINT32 version = 0;

            stat = pHelper->GetUINT32( version, true, false );

            if ( MgStreamHelper::mssDone == stat )
            {
                pStreamData->SetVersion( version );

                ret = true;
            }
        }

        if (false == ret)
        {
            // The stream may contain garbage when the connection is dropped for
            // some reason. This exception may be ignored to reduce noise.
            throw new MgInvalidStreamHeaderException(L"MgStreamParser.ParseStreamHeader",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }
        else if (MgStreamParser::StreamVersion != pStreamData->GetVersion())
        {
            throw new MgStreamIoException(L"MgStreamParser.ParseStreamHeader",
                __LINE__, __WFILE__, NULL, L"MgInvalidTCPProtocol", NULL);
        }
    }

    return ret;
};

///////////////////////////////////////////////////////////////////////////
//  <summary>
//  This static method reads and consumes the DATA header from the
//  stream encapsulted by the given StreamData* parameter.
//  </summary>
//
//  <param name = "pStreamData">
//  A pointer the the StreamData object that encapsulates the incoming and
//  outgoing stream information.
//  </param>
//
//  <returns>
//  Returns true if successful; false otherwise.
//  </returns>
bool MgStreamParser::ParseDataHeader( MgStreamData* pStreamData )
{
    ACE_ASSERT( pStreamData );

    bool ret = false;

    if ( pStreamData )
    {
        MgStreamHelper* pHelper = pStreamData->GetStreamHelper();

        UINT32 header = 0;

        MgStreamHelper::MgStreamStatus stat = pHelper->GetUINT32( header, true, false );

        if ( MgStreamHelper::mssDone == stat && MgStreamParser::mshStreamData == header )
        {
            ret = true;
        }
    }

    return ret;
};

///////////////////////////////////////////////////////////////////////////
//  <summary>
//  This static method reads and consumes the STREAM END header from the
//  stream encapsulted by the given StreamData* parameter.
//  </summary>
//
//  <param name = "pStreamData">
//  A pointer the the StreamData object that encapsulates the incoming and
//  outgoing stream information.
//  </param>
//
//  <returns>
//  Returns true if successful; false otherwise.
//  </returns>
bool MgStreamParser::ParseEndHeader( MgStreamData* pStreamData )
{
    ACE_ASSERT( pStreamData );

    bool ret = false;

    if ( pStreamData )
    {
        MgStreamHelper* pHelper = pStreamData->GetStreamHelper();

        UINT32 header = 0;

        MgStreamHelper::MgStreamStatus stat = pHelper->GetUINT32( header, true, false );

        if ( MgStreamHelper::mssDone == stat && MgStreamParser::mshStreamEnd == header )
        {
            ret = true;
        }
    }

    return ret;
};

///////////////////////////////////////////////////////////////////////////
//  <summary>
//  This static method writes a STREAM START header to the
//  stream encapsulted by the given StreamData* parameter.
//  </summary>
//
//  <param name = "pStreamData">
//  A pointer the the StreamData object that encapsulates the incoming and
//  outgoing stream information.
//  </param>
//
//  <returns>
//  Returns true if successful; false otherwise.
//  </returns>
bool MgStreamParser::WriteStreamHeader( MgStreamData* pStreamData )
{
    ACE_ASSERT ( pStreamData );

    bool ret = false;

    if ( pStreamData )
    {
        MgStreamHelper* pHelper = pStreamData->GetStreamHelper();

        MgStreamHelper::MgStreamStatus stat = pHelper->WriteUINT32( MgStreamParser::mshStreamStart );

        if ( MgStreamHelper::mssDone == stat )
        {
            //stat = pHelper->WriteUINT32( pStreamData->GetVersion() );
            stat = pHelper->WriteUINT32( 1 );   //  TODO:  majik number

            ret = ( stat != MgStreamHelper::mssDone ) ? false : true;
        }
    }

    return ret;
};

///////////////////////////////////////////////////////////////////////////
//  <summary>
//  This static method writes a STREAM END header to the
//  stream encapsulted by the given StreamData* parameter.
//  </summary>
//
//  <param name = "pStreamData">
//  A pointer the the StreamData object that encapsulates the incoming and
//  outgoing stream information.
//  </param>
//
//  <returns>
//  Returns true if successful; false otherwise.
//  </returns>
bool MgStreamParser::WriteEndHeader( MgStreamData* pStreamData )
{
    ACE_ASSERT( pStreamData );

    bool ret = false;

    if ( pStreamData )
    {
        MgStreamHelper* pHelper = pStreamData->GetStreamHelper();

        MgStreamHelper::MgStreamStatus stat = pHelper->WriteUINT32( MgStreamParser::mshStreamEnd );

        ret = ( stat != MgStreamHelper::mssDone ) ? false : true;
    }

    return ret;
};

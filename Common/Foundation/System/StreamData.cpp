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
#include "StreamData.h"

//-------------------------------------------------------------------------
//  Constructors/Destructors
//-------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////
//  <summary>
//  Constructs a MgStreamData object with the given parameters/
//  </summary>
//
//  <param name = "pHandler">
//  The ClientHandler object that is handling this stream.
//  </param>
//
//  <param name = "handle">
//  The ACE_HANDLE underlying the stream.
//  </param>
//
//  <param name = "pStreamHelper">
//  The MgStreamHelper object that handles data I/O for this stream.
//  </param>
MgStreamData::MgStreamData( ACE_HANDLE handle, MgStreamHelper* pStreamHelper ) :
    m_Handle( handle ),
    m_pStreamHelper( SAFE_ADDREF(pStreamHelper) ),
    m_Version( 0 ),
    m_DataSize( 0 ),
    m_DataRead( 0 ),
    m_bError( false )
{
    ACE_ASSERT( handle != 0 );
    ACE_ASSERT( pStreamHelper != NULL );
};

///////////////////////////////////////////////////////////////////////////
//  <summary>
//  Constructs a new MgStreamData object with the given MgStreamData object
//  </summary>
//
//  <param name = "copy">
//  The MgStreamData object to copy for the new object.
//  </param>
MgStreamData::MgStreamData( MgStreamData &copy ) :
    m_Handle( copy.m_Handle ),
    m_pStreamHelper( copy.m_pStreamHelper.Detach() ),
    m_Version( copy.m_Version ),
    m_DataSize( copy.m_DataSize ),
    m_DataRead( copy.m_DataRead ),
    m_bError( copy.m_bError )
{
    ACE_ASSERT( m_Handle != 0 );
    ACE_ASSERT( NULL != m_pStreamHelper.p );
};

///////////////////////////////////////////////////////////////////////////
//  <summary>
//  The destructor for the MgStreamData object.
//  </summary>
MgStreamData::~MgStreamData()
{
};

//-------------------------------------------------------------------------
//  Properties
//-------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////
//  <summary>
//  Gets the ACE_HANDLE associated with this MgStreamData object.
//  </summary>
//
//  <returns>
//  Returns the ACE_HANDLE associated with this MgStreamData object.
//  </returns>
ACE_HANDLE MgStreamData::GetHandle() const
{
    return m_Handle;
};

///////////////////////////////////////////////////////////////////////////
//  <summary>
//  Gets a pointer to the StreamHelper object that handles this objects
//  stream IO.
//  </summary>
//
//  <returns>
//  Returns a pointer the a MgStreamHelper object.
//  </returns>
MgStreamHelper* MgStreamData::GetStreamHelper()
{
    return m_pStreamHelper;
};

///////////////////////////////////////////////////////////////////////////
//  <summary>
//  Gets the version of this stream encapsulated by the MgStreamData object.
//  The version is not read from the stream automatically, but defaults
//  to zero.
//  </summary>
//
//  <returns>
//  Returns this version of this stream.
//  </returns>
UINT32 MgStreamData::GetVersion()
{
    return m_Version;
};

///////////////////////////////////////////////////////////////////////////
//  <summary>
//  Sets the version of the stream encapsulated by this MgStreamData object.
//  </summary>
void MgStreamData::SetVersion( UINT32 version )
{
    if ( version != 0 )
    {
        m_Version = version;
    }
    else
    {
        ACE_ERROR( ( LM_ERROR, ACE_TEXT( "MgStreamData Version cannot be zero" )));
    }

    return;
};

///////////////////////////////////////////////////////////////////////////
//  <summary>
//  Gets the size of the data packet.  This value is initialized to zero;
//  the actual value must be set manually.
//  </summary>
//
//  <returns>
//  The size of the data stream in bytes.
//  </returns>
INT64 MgStreamData::GetDataSize()
{
    return m_DataSize;
};

///////////////////////////////////////////////////////////////////////////
//  <summary>
//  Sets the size of the data packet.  Note that this method has no effect
//  on the data stream or response itself;  this property is merely for the
//  informational use of other objects.
//  </summary>
//
//  <param name = "size">
//  The size in bytes of the undelying data packet.
//  </param>
void MgStreamData::SetDataSize( INT64 size )
{
    m_DataSize = size;
};

///////////////////////////////////////////////////////////////////////////
//  <summary>
//  Gets the total number of bytes that have been read from the data section
//  of the stream.
//  </summary>
//
//  <returns>
//  The amounnt of data read from the stream in bytes.
//  </returns>
INT64 MgStreamData::GetDataRead()
{
    return m_DataRead;
};

///////////////////////////////////////////////////////////////////////////
//  <summary>
//  Sets the total number of bytes that have been read from the stream.
//  </summary>
//
//  <param name = "total">
//  The number of bytes that have been read from the data stream.
//  </param>
void MgStreamData::SetDataRead( INT64 total )
{
    m_DataRead = total;
};

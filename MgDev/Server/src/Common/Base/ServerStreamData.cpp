//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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
#include "ServerStreamData.h"

//-------------------------------------------------------------------------
//  Constructors/Destructors
//-------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////
//  <summary>
//  Constructs a MgServerStreamData object with the given parameters/
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
MgServerStreamData::MgServerStreamData( MgClientHandler* pHandler, ACE_HANDLE handle, MgStreamHelper* pStreamHelper ) :
    m_pClientHandler( SAFE_ADDREF(pHandler) ),
    MgStreamData( handle, pStreamHelper )

{
    ACE_ASSERT( pHandler != NULL );
};

///////////////////////////////////////////////////////////////////////////
//  <summary>
//  Constructs a new MgServerStreamData object with the given MgServerStreamData object
//  </summary>
//
//  <param name = "copy">
//  The MgServerStreamData object to copy for the new object.
//  </param>
MgServerStreamData::MgServerStreamData( MgServerStreamData &copy ) :
    m_pClientHandler( copy.m_pClientHandler.Detach() ),
    MgStreamData( copy )
{
    ACE_ASSERT( NULL != (MgClientHandler*) m_pClientHandler );
};

///////////////////////////////////////////////////////////////////////////
//  <summary>
//  The destructor for the MgServerStreamData object.
//  </summary>
MgServerStreamData::~MgServerStreamData()
{
};

//-------------------------------------------------------------------------
//  Properties
//-------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////
//  <summary>
//  Gets a pointer to the MgClientHandler that owns this stream.
//  </summary>
//
//  <returns>
//  Returns a pointer to a MgClientHandler.
//  </returns>
MgClientHandler*  MgServerStreamData::GetClientHandler()
{
    return SAFE_ADDREF((MgClientHandler*)m_pClientHandler);
};

// Debugging code...
void LogStream(char* buf, ...)
{
    va_list ap;
    va_start(ap, buf);
    static FILE* fp = NULL;
    if (NULL == fp)
    {
        fp = fopen("c:\\temp\\logStream.log","w");
    }

    time_t timeNow;
    time(&timeNow);

    if (NULL != fp)
    {
        fprintf(fp, ctime(&timeNow));
        vfprintf(fp, buf, ap);
        fflush(fp);
    }
    va_end(ap);
}


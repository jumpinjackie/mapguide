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

#ifndef MGSERVERSTREAMDATA_H_
#define MGSERVERSTREAMDATA_H_

#include "ServerBaseDllExport.h"

// Debugging code
void LogStream(char* buf, ...);

///////////////////////////////////////////////////////////////////////////
//  The StreamData class encapsulates the data pertaining to an
//  input/output stream.
class MG_SERVER_BASE_API MgServerStreamData : public MgStreamData
{
    DECLARE_CLASSNAME(MgServerStreamData)

    ///////////////////////////////////////////////////////////////////////
    ///  Constructors/Destructors
    public:

    MgServerStreamData( MgClientHandler* pHandle, ACE_HANDLE handle, MgStreamHelper* pStreamHelper );
    MgServerStreamData( MgServerStreamData & );
    virtual ~MgServerStreamData();

    ///////////////////////////////////////////////////////////////////////
    ///  Accessors
    public:

    MgClientHandler* GetClientHandler();

    ///////////////////////////////////////////////////////////////////////
    ///  Member Variables
    private:

    ///  the ClientHandler that is servicing this stream
    Ptr<MgClientHandler> m_pClientHandler;
};

#endif  //  MGSTREAMDATA_H_MG

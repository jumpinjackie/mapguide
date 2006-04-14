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

#ifndef IMGOPERATIONHADNLER_H
#define IMGOPERATIONHADNLER_H

class MgStreamData;
struct MgOperationPacket;

/// \cond INTERNAL
///////////////////////////////////////////////////////////////////////////
//  The IMgOperationHandler class defines the interface that all operations
//  processed and executed by the MapGuide server must implement.
class MG_SERVICE_API IMgOperationHandler
{
    public:
    IMgOperationHandler();
    virtual ~IMgOperationHandler();

    ///////////////////////////////////////////////////////////////////
    ///  Methods

    ///  Executes the operation
    virtual void Execute() = 0;

    ///  Initializes the operation from the stream
    virtual void Init(MgStreamData* data, const MgOperationPacket& packet) = 0;

    ///////////////////////////////////////////////////////////////////
    ///  Member Data
    protected :
    MgStreamData* m_data;
    MgOperationPacket m_packet;
};
/// \endcond

#endif  //  IMGOPERATIONHADNLER_H

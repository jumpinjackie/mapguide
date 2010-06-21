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

#ifndef MGTILESERVICEHANDLER_H
#define MGTILESERVICEHANDLER_H

#include "IServiceHandler.h"
#include "ServerTileDllExport.h"

///////////////////////////////////////////////////////////////////////////
/// This implementation of the IMgServiceHandler interface handles
/// operations not directly handled by any other service.
class MG_SERVER_TILE_API MgTileServiceHandler : public IMgServiceHandler
{
public:
    ///////////////////////////////////////////////////////////////////////////
    /// Constructor/Destructor
    MgTileServiceHandler(MgStreamData* data, const MgOperationPacket& packet);
    virtual ~MgTileServiceHandler( void );

    ///////////////////////////////////////////////////////////////////
    /// Overridden IMgServiceHandler methods
    virtual IMgServiceHandler::MgProcessStatus ProcessOperation();
};

#endif

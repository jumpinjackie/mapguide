//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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

#ifndef MG_SERVICE_HANDLER_FACTORY_H
#define MG_SERVICE_HANDLER_FACTORY_H

#include "IServiceHandler.h"

///////////////////////////////////////////////////////////////////////////
//  The MgServiceHandlerFactory class provides the functionality to get
//  a suitable IServiceHandler object that can service a given
//  operation.  This class is a singleton object.
class MgServiceHandlerFactory
{
    ///////////////////////////////////////////////////////////////////////
    //  Constructors/Destructors
    protected :
        MgServiceHandlerFactory( void );
    public :
        virtual ~MgServiceHandlerFactory( void );

    ///////////////////////////////////////////////////////////////////////
    //  Methods
    public :
        static MgServiceHandlerFactory* Instance();
        IMgServiceHandler* GetHandler(UINT32 serviceId, MgStreamData* data,
            const MgOperationPacket& packet);
};

#endif  //  MG_SERVICE_HANDLER_FACTORY_H

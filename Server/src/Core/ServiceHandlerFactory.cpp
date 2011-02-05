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

#include "MapGuideCommon.h"
#include "ServiceHandlerFactory.h"
#include "DrawingServiceHandler.h"
#include "FeatureServiceHandler.h"
#include "ServerMappingDllExport.h"
#include "MappingServiceHandler.h"
#include "RenderingServiceHandler.h"
#include "ResourceServiceHandler.h"
#include "ServerAdminServiceHandler.h"
#include "SiteServiceHandler.h"
#include "TileServiceHandler.h"
#include "KmlServiceHandler.h"

//  References the static singleton MgServiceHandlerFactory object
static auto_ptr<MgServiceHandlerFactory> s_pFactory;

//-------------------------------------------------------------------------
//  Constructors/Destructors
//-------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////
//  <summary>
//  The default constructor for a MgServiceHandlerFactory object.  This
//  constructor is protected;  an Instance can be obtained through
//  the Instance() method.
//  </summary>
MgServiceHandlerFactory::MgServiceHandlerFactory( void )
{
}

///////////////////////////////////////////////////////////////////////////
//  <summary>
//  The destructor for a MgServiceHandlerFactory object.
//  </summary>
MgServiceHandlerFactory::~MgServiceHandlerFactory( void )
{
}

//-------------------------------------------------------------------------
//  Methods
//-------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////
//  <summary>
//  This method allows access to the singelton instance of the
//  MgServiceHandlerFactory class.
//  </summary>
//
//  <returns>
//  Returns the singleton instance of the MgServiceHandlerFactory.
//  </returns>
MgServiceHandlerFactory* MgServiceHandlerFactory::Instance()
{
    if ( NULL == s_pFactory.get() )
        s_pFactory.reset(new MgServiceHandlerFactory());

    return s_pFactory.get();
}

///////////////////////////////////////////////////////////////////////////
//  <summary>
//  This method returns the IMgServiceHandler object corresponding to
//  the given parameters.  The caller owns the resultant object.
//  </summary>
//
//  <param name = "serviceId">
//  The service ID of the requested handler.
//  </param>
//
//  <param name="data">
//  A pointer the the MgStreamData object that encapsulates the incoming and
//  outgoing stream information.
//  </param>
//
//  <param name = "packet">
//  The operation packet that will be services by the handler.
//  </param>
//
//  <returns>
//  Returns an IMgServiceHandler object pointer that can service the operation.
//  Returns NULL if one cannot be obtained.
//  </returns>
IMgServiceHandler* MgServiceHandlerFactory::GetHandler(UINT32 serviceId,
    MgStreamData* data, const MgOperationPacket& packet)
{
    IMgServiceHandler* serviceHandler = NULL;

    MG_TRY()

    switch (serviceId)
    {
        case MgPacketParser::msiDrawing:
            serviceHandler = new MgDrawingServiceHandler(data, packet);
            break;

        case MgPacketParser::msiFeature:
            serviceHandler = new MgFeatureServiceHandler(data, packet);
            break;

        case MgPacketParser::msiMapping:
            serviceHandler = new MgMappingServiceHandler(data, packet);
            break;

        case MgPacketParser::msiRendering:
            serviceHandler = new MgRenderingServiceHandler(data, packet);
            break;

        case MgPacketParser::msiResource:
            serviceHandler = new MgResourceServiceHandler(data, packet);
            break;

        case MgPacketParser::msiServerAdmin:
            serviceHandler = new MgServerAdminServiceHandler(data, packet);
            break;

        case MgPacketParser::msiSite:
            serviceHandler = new MgSiteServiceHandler(data, packet);
            break;

        case MgPacketParser::msiTile:
            serviceHandler = new MgTileServiceHandler(data, packet);
            break;

        case MgPacketParser::msiKml:
            serviceHandler = new MgKmlServiceHandler(data, packet);
            break;

        case MgPacketParser::msiUnknown:
        default:
            throw new MgServiceNotSupportedException(L"GetHandler",
                __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_CATCH_AND_THROW(L"MgServiceHandlerFactory.GetHandler")

    return serviceHandler;
}

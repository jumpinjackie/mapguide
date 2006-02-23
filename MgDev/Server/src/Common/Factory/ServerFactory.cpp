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

#include "AceCommon.h"
#include "ServerFactory.h"
#include "ServerAdminService.h"
#include "ServerDrawingService.h"
#include "ServerFeatureService.h"
#include "ServerMappingService.h"
#include "ServerRenderingService.h"
#include "ServerResourceService.h"
#include "ServerSiteService.h"
#include "ServerTileService.h"

static bool InitializeStaticData();

static map<int,  MgObject* (*)()> classCreators;
static bool initStatic = InitializeStaticData();

bool InitializeStaticData()
{
    // Sample code to add class map
    // classCreators[ResourceService_ResourceIdentifier] = MgResourceIdentifier::CreateObject;
    return true;
}

extern "C" void* CreateMgServerObject(int classId)
{
    //find the object in the map
    MgObject*(*createFunc)() = classCreators[classId];
    if(createFunc == NULL)
        return NULL;

    MgObject* obj = (*createFunc)();
    return obj;
}

extern "C" void* CreateMgServerService(INT32 serviceType, MgConnectionProperties* mapConnection)
{
    MgService* service = NULL;
    switch(serviceType)
    {
        case MgServiceType::DrawingService:
            service = new MgServerDrawingService(mapConnection);
            break;
        case MgServiceType::FeatureService:
            service = new MgServerFeatureService(mapConnection);
            break;
        case MgServiceType::MappingService:
            service = new MgServerMappingService(mapConnection);
            break;
        case MgServiceType::RenderingService:
            service = new MgServerRenderingService(mapConnection);
            break;
        case MgServiceType::ResourceService:
            service = new MgServerResourceService(mapConnection);
            break;
        case MgServiceType::ServerAdminService:
            service = new MgServerAdminService(mapConnection);
            break;
        case MgServiceType::SiteService:
            service = new MgServerSiteService(mapConnection);
            break;
        case MgServiceType::TileService:
            service = new MgServerTileService(mapConnection);
            break;
        default:
            break;
    }
    return service;
}

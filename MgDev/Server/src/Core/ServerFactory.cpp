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
#include "ServerAdminService.h"
#include "ServerDrawingService.h"
#include "ServerFeatureService.h"
#include "ServerMappingService.h"
#include "ServerRenderingService.h"
#include "ServerResourceService.h"
#include "ServerSiteService.h"
#include "ServerTileService.h"
#include "ServerKmlService.h"
#include "ServerProfilingService.h"

static bool InitializeStaticData();

static map<int,  MgObject* (*)()> classCreators;
static bool initStatic = InitializeStaticData();

bool InitializeStaticData()
{
    MgServiceRegistry* registry = MgServiceRegistry::GetInstance();
    registry->RegisterService(MgServiceType::DrawingService, MgServerDrawingService::CreateService, sctLocalInProc);
    registry->RegisterService(MgServiceType::FeatureService, MgServerFeatureService::CreateService, sctLocalInProc);
    registry->RegisterService(MgServiceType::MappingService, MgServerMappingService::CreateService, sctLocalInProc);
    registry->RegisterService(MgServiceType::RenderingService, MgServerRenderingService::CreateService, sctLocalInProc);
    registry->RegisterService(MgServiceType::ResourceService, MgServerResourceService::CreateService, sctLocalInProc);
    registry->RegisterService(MgServiceType::TileService, MgServerTileService::CreateService, sctLocalInProc);
    registry->RegisterService(MgServiceType::KmlService, MgServerKmlService::CreateService, sctLocalInProc);
    registry->RegisterService(MgServiceType::ServerAdminService, MgServerAdminService::CreateService, sctLocalInProc);
    registry->RegisterService(MgServiceType::SiteService, MgServerSiteService::CreateService, sctLocalInProc);
    registry->RegisterService(MgServiceType::ProfilingService, MgServerProfilingService::CreateService, sctLocalInProc);

    return true;
}

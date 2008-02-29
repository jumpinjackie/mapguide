//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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

#ifdef _WIN32
#include "windows.h"
#else
#include <dlfcn.h>
#endif

#define EXCEPTION_CLASS_CREATOR(className) \
    fact->Register(MapGuide_Exception_##className, className::CreateObject);

static bool InitializeStaticData();

static bool initStatic = InitializeStaticData();

bool InitializeStaticData()
{
    MgClassFactory* fact = MgClassFactory::GetInstance();

    //put in the map any class that can be serialized
    EXCEPTION_CLASS_CREATOR(MgAllProviderConnectionsUsedException)
    EXCEPTION_CLASS_CREATOR(MgAuthenticationFailedException)
    EXCEPTION_CLASS_CREATOR(MgConnectionFailedException)
    EXCEPTION_CLASS_CREATOR(MgConnectionNotOpenException)
    EXCEPTION_CLASS_CREATOR(MgDbException)
    EXCEPTION_CLASS_CREATOR(MgDbXmlException)
    EXCEPTION_CLASS_CREATOR(MgDuplicateGroupException)
    EXCEPTION_CLASS_CREATOR(MgDuplicateNameException)
    EXCEPTION_CLASS_CREATOR(MgDuplicateParameterException)
    EXCEPTION_CLASS_CREATOR(MgDuplicateRepositoryException)
    EXCEPTION_CLASS_CREATOR(MgDuplicateRoleException)
    EXCEPTION_CLASS_CREATOR(MgDuplicateServerException)
    EXCEPTION_CLASS_CREATOR(MgDuplicateSessionException)
    EXCEPTION_CLASS_CREATOR(MgDuplicateUserException)
    EXCEPTION_CLASS_CREATOR(MgDwfException)
    EXCEPTION_CLASS_CREATOR(MgDwfSectionNotFoundException)
    EXCEPTION_CLASS_CREATOR(MgDwfSectionResourceNotFoundException)
    EXCEPTION_CLASS_CREATOR(MgEndOfStreamException)
    EXCEPTION_CLASS_CREATOR(MgEvaluationExpiredException)
    EXCEPTION_CLASS_CREATOR(MgGroupNotFoundException)
    EXCEPTION_CLASS_CREATOR(MgInvalidDwfPackageException)
    EXCEPTION_CLASS_CREATOR(MgInvalidDwfSectionException)
    EXCEPTION_CLASS_CREATOR(MgInvalidFeatureSourceException)
    EXCEPTION_CLASS_CREATOR(MgInvalidIpAddressException)
    EXCEPTION_CLASS_CREATOR(MgInvalidLicenseException)
    EXCEPTION_CLASS_CREATOR(MgInvalidLogEntryException)
    EXCEPTION_CLASS_CREATOR(MgInvalidOperationVersionException)
    EXCEPTION_CLASS_CREATOR(MgInvalidPasswordException)
    EXCEPTION_CLASS_CREATOR(MgInvalidPrintLayoutFontSizeUnitsException)
    EXCEPTION_CLASS_CREATOR(MgInvalidPrintLayoutPositionUnitsException)
    EXCEPTION_CLASS_CREATOR(MgInvalidPrintLayoutSizeUnitsException)
    EXCEPTION_CLASS_CREATOR(MgInvalidSerialNumberException)
    EXCEPTION_CLASS_CREATOR(MgInvalidServerNameException)
    EXCEPTION_CLASS_CREATOR(MgLicenseException)
    EXCEPTION_CLASS_CREATOR(MgLicenseExpiredException)
    EXCEPTION_CLASS_CREATOR(MgLogOpenFailedException)
    EXCEPTION_CLASS_CREATOR(MgOperationProcessingException)
    EXCEPTION_CLASS_CREATOR(MgParameterNotFoundException)
    EXCEPTION_CLASS_CREATOR(MgPathTooLongException)
    EXCEPTION_CLASS_CREATOR(MgPermissionDeniedException)
    EXCEPTION_CLASS_CREATOR(MgPortNotAvailableException)
    EXCEPTION_CLASS_CREATOR(MgPrintToScaleModeNotSelectedException)
    EXCEPTION_CLASS_CREATOR(MgRasterTransformationNotSupportedException)
    EXCEPTION_CLASS_CREATOR(MgRepositoryCreationFailedException)
    EXCEPTION_CLASS_CREATOR(MgRepositoryNotFoundException)
    EXCEPTION_CLASS_CREATOR(MgRepositoryNotOpenException)
    EXCEPTION_CLASS_CREATOR(MgRepositoryOpenFailedException)
    EXCEPTION_CLASS_CREATOR(MgRoleNotFoundException)
    EXCEPTION_CLASS_CREATOR(MgServerNotFoundException)
    EXCEPTION_CLASS_CREATOR(MgServerNotOnlineException)
    EXCEPTION_CLASS_CREATOR(MgSessionExpiredException)
    EXCEPTION_CLASS_CREATOR(MgSessionNotFoundException)
    EXCEPTION_CLASS_CREATOR(MgUnauthorizedAccessException)
    EXCEPTION_CLASS_CREATOR(MgUnsupportedProviderThreadModelException)
    EXCEPTION_CLASS_CREATOR(MgUriFormatException)

    fact->Register(MapGuide_MapLayer_Map, MgMap::CreateObject);
    fact->Register(MapGuide_MapLayer_Layer, MgLayer::CreateObject);
    fact->Register(MapGuide_MapLayer_Selection, MgSelection::CreateObject);
    fact->Register(MapGuide_Service_ServerInformation, MgServerInformation::CreateObject);
    fact->Register(MapGuide_Service_PackageStatusInformation, MgPackageStatusInformation::CreateObject);
    fact->Register(MapGuide_Service_UserInformation, MgUserInformation::CreateObject);
    fact->Register(MapGuide_Service_SiteInfo, MgSiteInfo::CreateObject);
    fact->Register(PlatformBase_FeatureService_DataReader, MgProxyDataReader::CreateObject);
    fact->Register(PlatformBase_FeatureService_FeatureReader, MgProxyFeatureReader::CreateObject);
    fact->Register(PlatformBase_FeatureService_GwsFeatureReader, MgProxyGwsFeatureReader::CreateObject);
    fact->Register(PlatformBase_FeatureService_SqlDataReader, MgProxySqlDataReader::CreateObject);
    fact->Register(MapGuide_MappingService_DwfVersion, MgDwfVersion::CreateObject);
    fact->Register(MapGuide_MappingService_PlotSpecification, MgPlotSpecification::CreateObject);
    fact->Register(MapGuide_MappingService_PrintLayout, MgPrintLayout::CreateObject);
    fact->Register(MapGuide_MappingService_Layout, MgLayout::CreateObject);
    fact->Register(MapGuide_MappingService_MapPlotCollection, MgMapPlotCollection::CreateObject);
    fact->Register(MapGuide_RenderingService_FeatureInformation, MgFeatureInformation::CreateObject);
    fact->Register(MapGuide_RenderingService_RenderingOptions, MgRenderingOptions::CreateObject);

    MgServiceRegistry* registry = MgServiceRegistry::GetInstance();
    registry->RegisterService(MgServiceType::DrawingService, MgProxyDrawingService::CreateService, sctRemoteServerToServer);
    registry->RegisterService(MgServiceType::MappingService, MgProxyMappingService::CreateService, sctRemoteServerToServer);
    registry->RegisterService(MgServiceType::FeatureService, MgProxyFeatureService::CreateService, sctRemoteServerToServer);
    registry->RegisterService(MgServiceType::RenderingService, MgProxyRenderingService::CreateService, sctRemoteServerToServer);
    registry->RegisterService(MgServiceType::ResourceService, MgProxyResourceService::CreateService, sctRemoteServerToServer);
    registry->RegisterService(MgServiceType::TileService, MgProxyTileService::CreateService, sctRemoteServerToServer);
    registry->RegisterService(MgServiceType::KmlService, MgProxyKmlService::CreateService, sctRemoteServerToServer);

    return true;
}




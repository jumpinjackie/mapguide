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

#ifndef _MAPGUIDE_H_
#define _MAPGUIDE_H_

#include "PlatformBase.h"

// MapGuide exports
#ifdef _WIN32
#ifdef MAPGUIDECOMMON_EXPORTS
#    define MG_MAPGUIDE_API __declspec(dllexport)
#else
#    define MG_MAPGUIDE_API __declspec(dllimport)
#endif
#else
#define MG_MAPGUIDE_API
#endif

// This define extracts the operation version
// 0x00XXYYZZ where XX = Major, YY = Minor and ZZ = Phase
#define EXTRACT_VERSION(Version, Major, Minor, Phase) \
    Major = (Version & 0x00ff0000) >> 16; \
    Minor = (Version & 0x0000ff00) >> 8; \
    Phase = (Version & 0x000000ff);

// This define builds the operation version
// 0x00XXYYZZ where XX = Major, YY = Minor and ZZ = Phase
#define BUILD_VERSION(Major, Minor, Phase) ((Major << 16) + (Minor << 8) + Phase)

#define VERSION_NO_PHASE(Version) (Version & 0x00ffff00)

#define VERSION_SUPPORTED(Major, Minor) (Major << 16) + (Minor << 8)

#ifndef _WIN32
extern long GetTickCount();
#endif

#include "System/MapGuideCommonClassId.h"
#include "System/ConfigProperties.h"
#include "System/UserInformation.h"
#include "System/MapGuideStream.h"
#include "System/CryptographyManager.h"
#include "Util/IpUtil.h"
#include "Util/TimerUtil.h"

#include "MapLayer/Layer.h"
#include "MapLayer/Map.h"
#include "MapLayer/Selection.h"

#include "Exception/AllProviderConnectionsUsedException.h"
#include "Exception/AuthenticationFailedException.h"
#include "Exception/ConnectionFailedException.h"
#include "Exception/ConnectionNotOpenException.h"
#include "Exception/DbException.h"
#include "Exception/DbXmlException.h"
#include "Exception/DuplicateGroupException.h"
#include "Exception/DuplicateNameException.h"
#include "Exception/DuplicateParameterException.h"
#include "Exception/DuplicateRepositoryException.h"
#include "Exception/DuplicateRoleException.h"
#include "Exception/DuplicateServerException.h"
#include "Exception/DuplicateSessionException.h"
#include "Exception/DuplicateUserException.h"
#include "Exception/DwfException.h"
#include "Exception/DwfSectionNotFoundException.h"
#include "Exception/DwfSectionResourceNotFoundException.h"
#include "Exception/EndOfStreamException.h"
#include "Exception/EvaluationExpiredException.h"
#include "Exception/GroupNotFoundException.h"
#include "Exception/InvalidDwfPackageException.h"
#include "Exception/InvalidDwfSectionException.h"
#include "Exception/InvalidFeatureSourceException.h"
#include "Exception/InvalidIpAddressException.h"
#include "Exception/InvalidLicenseException.h"
#include "Exception/InvalidLogEntryException.h"
#include "Exception/InvalidOperationVersionException.h"
#include "Exception/InvalidPasswordException.h"
#include "Exception/InvalidPrintLayoutFontSizeUnitsException.h"
#include "Exception/InvalidPrintLayoutPositionUnitsException.h"
#include "Exception/InvalidPrintLayoutSizeUnitsException.h"
#include "Exception/InvalidSerialNumberException.h"
#include "Exception/InvalidServerNameException.h"
#include "Exception/LicenseException.h"
#include "Exception/LicenseExpiredException.h"
#include "Exception/LogOpenFailedException.h"
#include "Exception/OperationProcessingException.h"
#include "Exception/ParameterNotFoundException.h"
#include "Exception/PathTooLongException.h"
#include "Exception/PermissionDeniedException.h"
#include "Exception/PortNotAvailableException.h"
#include "Exception/PrintToScaleModeNotSelectedException.h"
#include "Exception/RasterTransformationNotSupportedException.h"
#include "Exception/RepositoryCreationFailedException.h"
#include "Exception/RepositoryNotFoundException.h"
#include "Exception/RepositoryNotOpenException.h"
#include "Exception/RepositoryOpenFailedException.h"
#include "Exception/RoleNotFoundException.h"
#include "Exception/ServerNotFoundException.h"
#include "Exception/ServerNotOnlineException.h"
#include "Exception/SessionExpiredException.h"
#include "Exception/SessionNotFoundException.h"
#include "Exception/StylizeLayerFailedException.h"
#include "Exception/UnauthorizedAccessException.h"
#include "Exception/UnsupportedProviderThreadModelException.h"
#include "Exception/UriFormatException.h"

#include "Net/IOperationHandler.h"
#include "Services/Command.h"
#include "Services/Site.h"
#include "Services/SiteInfo.h"
#include "Services/SiteManager.h"
#include "Services/ConnectionProperties.h"
#include "Services/SiteConnection.h"
#include "Services/DrawingDefs.h"
#include "Services/DwfVersion.h"
#include "Services/DrawingService.h"
#include "Services/CustomLogoInfo.h"
#include "Services/CustomTextInfo.h"
#include "Services/PlotSpecification.h"
#include "Services/ImageFormats.h"
#include "Services/Layout.h"
#include "Services/PrintLayout.h"
#include "Services/MapPlot.h"
#include "Services/MapPlotCollection.h"
#include "Services/MapPlotInstruction.h"
#include "Services/FeatureInformation.h"
#include "Services/MappingDefs.h"
#include "Services/MappingService.h"
#include "Services/RenderingDefs.h"
#include "Services/RenderingOptions.h"
#include "Services/RenderingService.h"
#include "Services/TileDefs.h"
#include "Services/TileService.h"
#include "Services/KmlService.h"
#include "Services/ProfilingDefs.h"
#include "Services/ProfilingService.h"

#include "Services/PackageStatusInformation.h"
#include "Services/ProxyDataReader.h"
#include "Services/ProxyDrawingService.h"
#include "Services/ProxyFeatureReader.h"
#include "Services/ProxyFeatureService.h"
#include "Services/ProxyGwsFeatureReader.h"
#include "Services/ProxyKmlService.h"
#include "Services/ProxyMappingService.h"
#include "Services/ProxyRenderingService.h"
#include "Services/ProxyResourceService.h"
#include "Services/ProxySqlDataReader.h"
#include "Services/ProxyTileService.h"
#include "Services/ProxyFeatureTransaction.h"
#include "Services/ProxyProfilingService.h"
#include "Services/ServerAdmin.h"
#include "Services/ServerAdminDefs.h"
#include "Services/ServerConnection.h"
#include "Services/ServerConnectionImp.h"
#include "Services/ServerConnectionPool.h"
#include "Services/ServerConnectionStack.h"
#include "Services/ServerInformation.h"
#include "Services/UnitType.h"

#include "Controller/Controller.h"
#include "Controller/HtmlController.h"
#include "Controller/DwfController.h"


#endif

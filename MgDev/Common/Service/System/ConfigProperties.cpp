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
#include "ConfigProperties.h"

// Default configuration filenames
const STRING MgConfigProperties::DefaultConfigurationFilename                               = L"webconfig.ini";
const STRING MgConfigProperties::ServerConfigurationFilename                                = L"serverconfig.ini";

// Configuration file section/property names and default values

// ******************************************************************
// General Properties
// ******************************************************************
const STRING MgConfigProperties::GeneralPropertiesSection                                   = L"GeneralProperties";
const STRING MgConfigProperties::GeneralPropertyDefaultLocale                               = L"DefaultLocale";
const STRING MgConfigProperties::DefaultGeneralPropertyDefaultLocale                        = L"en";      // ISO 639-1 name
const STRING MgConfigProperties::GeneralPropertyDisplayName                                 = L"DisplayName";
const STRING MgConfigProperties::DefaultGeneralPropertyDisplayName                          = L"";
const STRING MgConfigProperties::GeneralPropertyLogsPath                                    = L"LogsPath";
const STRING MgConfigProperties::DefaultGeneralPropertyLogsPath                             = L"./Logs/";
const STRING MgConfigProperties::GeneralPropertyResourcesPath                               = L"ResourcesPath";
const STRING MgConfigProperties::DefaultGeneralPropertyResourcesPath                        = L"./Resources/";
const STRING MgConfigProperties::GeneralPropertyTcpIpMtu                                    = L"TcpIpMtu";
const INT32  MgConfigProperties::DefaultGeneralPropertyTcpIpMtu                             = 1460;
const STRING MgConfigProperties::GeneralPropertyTempPath                                    = L"TempPath";
const STRING MgConfigProperties::DefaultGeneralPropertyTempPath                             = L"./Temp/";
const STRING MgConfigProperties::GeneralPropertyFdoPath                                     = L"FdoPath";
const STRING MgConfigProperties::DefaultGeneralPropertyFdoPath                              = L"";
const STRING MgConfigProperties::GeneralPropertyConnectionTimeout                           = L"ConnectionTimeout";
const INT32  MgConfigProperties::DefaultGeneralPropertyConnectionTimeout                    = 120;
const STRING MgConfigProperties::GeneralPropertyConnectionTimerInterval                     = L"ConnectionTimerInterval";
const INT32  MgConfigProperties::DefaultGeneralPropertyConnectionTimerInterval              = 60;
const STRING MgConfigProperties::GeneralPropertyServiceRegistrationTimerInterval            = L"ServiceRegistrationTimerInterval";
const INT32  MgConfigProperties::DefaultGeneralPropertyServiceRegistrationTimerInterval     = 60;
const STRING MgConfigProperties::GeneralPropertyMaxLogFileSize                              = L"MaxLogFileSize";
const INT32  MgConfigProperties::DefaultGeneralPropertyMaxLogFileSize                       = 64;
const STRING MgConfigProperties::GeneralPropertyLogsDelimiter                               = L"LogsDelimiter";
const STRING MgConfigProperties::DefaultGeneralPropertyLogsDelimiter                        = L"\t";
const STRING MgConfigProperties::GeneralPropertyMaxLogFileSizeEnabled                       = L"MaxLogFileSizeEnabled";
const bool   MgConfigProperties::DefaultGeneralPropertyMaxLogFileSizeEnabled                = false;

// ******************************************************************
// Drawing Service Properties
// ******************************************************************
const STRING MgConfigProperties::DrawingServicePropertiesSection                            = L"DrawingServiceProperties";

// ******************************************************************
// Feature Service Properties
// ******************************************************************
const STRING MgConfigProperties::FeatureServicePropertiesSection                            = L"FeatureServiceProperties";
const STRING MgConfigProperties::FeatureServicePropertyDataConnectionPoolEnabled            = L"DataConnectionPoolEnabled";
const bool   MgConfigProperties::DefaultFeatureServicePropertyDataConnectionPoolEnabled     = false;
const STRING MgConfigProperties::FeatureServicePropertyDataConnectionPoolSize               = L"DataConnectionPoolSize";
const INT32  MgConfigProperties::DefaultFeatureServicePropertyDataConnectionPoolSize        = 100;
const STRING MgConfigProperties::FeatureServicePropertyDataConnectionTimeout                = L"DataConnectionTimeout";
const INT32  MgConfigProperties::DefaultFeatureServicePropertyDataConnectionTimeout         = 600;
const STRING MgConfigProperties::FeatureServicePropertyDataConnectionTimerInterval          = L"DataConnectionTimerInterval";
const INT32  MgConfigProperties::DefaultFeatureServicePropertyDataConnectionTimerInterval   = 60;
const STRING MgConfigProperties::FeatureServicePropertyDataCacheSize                        = L"DataCacheSize";
const INT32  MgConfigProperties::DefaultFeatureServicePropertyDataCacheSize                 = 100;

// ******************************************************************
// Mapping Service Properties
// ******************************************************************
const STRING MgConfigProperties::MappingServicePropertiesSection                            = L"MappingServiceProperties";

// ******************************************************************
// Rendering Service Properties
// ******************************************************************
const STRING MgConfigProperties::RenderingServicePropertiesSection                          = L"RenderingServiceProperties";
const STRING MgConfigProperties::RenderingServicePropertyTileExtentOffset                   = L"TileExtentOffset";
const double MgConfigProperties::DefaultRenderingServicePropertyTileExtentOffset            = 0.35;

// ******************************************************************
// Resource Service Properties
// ******************************************************************
const STRING MgConfigProperties::ResourceServicePropertiesSection                               = L"ResourceServiceProperties";
const STRING MgConfigProperties::ResourceServicePropertyLibraryResourceDataFilePath             = L"LibraryResourceDataFilePath";
const STRING MgConfigProperties::DefaultResourceServicePropertyLibraryResourceDataFilePath      = L"./Repositories/Library/DataFiles/";
const STRING MgConfigProperties::ResourceServicePropertyLibraryRepositoryPath                   = L"LibraryRepositoryPath";
const STRING MgConfigProperties::DefaultResourceServicePropertyLibraryRepositoryPath            = L"./Repositories/Library/";
const STRING MgConfigProperties::ResourceServicePropertySessionResourceDataFilePath             = L"SessionResourceDataFilePath";
const STRING MgConfigProperties::DefaultResourceServicePropertySessionResourceDataFilePath      = L"./Repositories/Session/DataFiles/";
const STRING MgConfigProperties::ResourceServicePropertySessionRepositoryPath                   = L"SessionRepositoryPath";
const STRING MgConfigProperties::DefaultResourceServicePropertySessionRepositoryPath            = L"./Repositories/Session/";
const STRING MgConfigProperties::ResourceServicePropertySiteRepositoryPath                      = L"SiteRepositoryPath";
const STRING MgConfigProperties::DefaultResourceServicePropertySiteRepositoryPath               = L"./Repositories/Site/";
const STRING MgConfigProperties::ResourceServicePropertyPackagesPath                            = L"PackagesPath";
const STRING MgConfigProperties::DefaultResourceServicePropertyPackagesPath                     = L"./Packages/";
const STRING MgConfigProperties::ResourceServicePropertyResourceSchemaFilePath                  = L"ResourceSchemaFilePath";
const STRING MgConfigProperties::DefaultResourceServicePropertyResourceSchemaFilePath           = L"./Schema/";
const STRING MgConfigProperties::ResourceServicePropertyResourceDataFileTrashFolderName         = L"ResourceDataFileTrashFolderName";
const STRING MgConfigProperties::DefaultResourceServicePropertyResourceDataFileTrashFolderName  = L"Trash";
const STRING MgConfigProperties::ResourceServicePropertyRepositoryCheckpointsTimerInterval      = L"RepositoryCheckpointsTimerInterval";
const INT32  MgConfigProperties::DefaultResourceServicePropertyRepositoryCheckpointsTimerInterval = 600;
const STRING MgConfigProperties::ResourceServicePropertyResourcePermissionCacheSize             = L"ResourcePermissionCacheSize";
const INT32  MgConfigProperties::DefaultResourceServicePropertyResourcePermissionCacheSize      = 1000;
const STRING MgConfigProperties::ResourceServicePropertyResourceValidationEnabled               = L"ResourceValidationEnabled";
const bool   MgConfigProperties::DefaultResourceServicePropertyResourceValidationEnabled        = true;

// ******************************************************************
// Site Service Properties
// ******************************************************************
const STRING MgConfigProperties::SiteServicePropertiesSection                               = L"SiteServiceProperties";
const STRING MgConfigProperties::SiteServicePropertySessionTimeout                          = L"SessionTimeout";
const INT32  MgConfigProperties::DefaultSiteServicePropertySessionTimeout                   = 1200;
const INT32  MgConfigProperties::MinimumSiteServicePropertySessionTimeout                   = 180;
const INT32  MgConfigProperties::MaximumSiteServicePropertySessionTimeout                   = 2147483647;
const STRING MgConfigProperties::SiteServicePropertySessionTimerInterval                    = L"SessionTimerInterval";
const INT32  MgConfigProperties::DefaultSiteServicePropertySessionTimerInterval             = 400;
const INT32  MgConfigProperties::MinimumSiteServicePropertySessionTimerInterval             = 60;
const INT32  MgConfigProperties::MaximumSiteServicePropertySessionTimerInterval             = 2147483647;

// ******************************************************************
// Tile Service Properties
// ******************************************************************
const STRING MgConfigProperties::TileServicePropertiesSection                               = L"TileServiceProperties";
const STRING MgConfigProperties::TileServicePropertyTileCachePath                           = L"TileCachePath";
const STRING MgConfigProperties::DefaultTileServicePropertyTileCachePath                    = L"./Repositories/TileCache/";

// ******************************************************************
// Administrative Connection Properties
// ******************************************************************
const STRING MgConfigProperties::AdministrativeConnectionPropertiesSection                  = L"AdministrativeConnectionProperties";
const STRING MgConfigProperties::AdministrativeConnectionPropertyEmail                      = L"Email";
const STRING MgConfigProperties::DefaultAdministrativeConnectionPropertyEmail               = L"";
const STRING MgConfigProperties::AdministrativeConnectionPropertyMaxConnections             = L"MaxConnections";
const INT32  MgConfigProperties::DefaultAdministrativeConnectionPropertyMaxConnections      = 20;
const STRING MgConfigProperties::AdministrativeConnectionPropertyPort                       = L"Port";
const INT32  MgConfigProperties::DefaultAdministrativeConnectionPropertyPort                = 5555;
const STRING MgConfigProperties::AdministrativeConnectionPropertyQueueSize                  = L"QueueSize";
const INT32  MgConfigProperties::DefaultAdministrativeConnectionPropertyQueueSize           = 10;
const STRING MgConfigProperties::AdministrativeConnectionPropertyThreadPoolSize             = L"ThreadPoolSize";
const INT32  MgConfigProperties::DefaultAdministrativeConnectionPropertyThreadPoolSize      = 5;

// ******************************************************************
// Client Connection Properties
// ******************************************************************
const STRING MgConfigProperties::ClientConnectionPropertiesSection                          = L"ClientConnectionProperties";
const STRING MgConfigProperties::ClientConnectionPropertyMaxConnections                     = L"MaxConnections";
const INT32  MgConfigProperties::DefaultClientConnectionPropertyMaxConnections              = 100;
const STRING MgConfigProperties::ClientConnectionPropertyPort                               = L"Port";
const INT32  MgConfigProperties::DefaultClientConnectionPropertyPort                        = 4444;
const STRING MgConfigProperties::ClientConnectionPropertyQueueSize                          = L"QueueSize";
const INT32  MgConfigProperties::DefaultClientConnectionPropertyQueueSize                   = 10;
const STRING MgConfigProperties::ClientConnectionPropertyThreadPoolSize                     = L"ThreadPoolSize";
const INT32  MgConfigProperties::DefaultClientConnectionPropertyThreadPoolSize              = 10;

// ******************************************************************
// Site Connection Properties
// ******************************************************************
const STRING MgConfigProperties::SiteConnectionPropertiesSection                            = L"SiteConnectionProperties";
const STRING MgConfigProperties::SiteConnectionPropertyIpAddress                            = L"IpAddress";
const STRING MgConfigProperties::DefaultSiteConnectionPropertyIpAddress                     = L"127.0.0.1";
const STRING MgConfigProperties::SiteConnectionPropertyMaxConnections                       = L"MaxConnections";
const INT32  MgConfigProperties::DefaultSiteConnectionPropertyMaxConnections                = 20;
const STRING MgConfigProperties::SiteConnectionPropertyPort                                 = L"Port";
const INT32  MgConfigProperties::DefaultSiteConnectionPropertyPort                          = 3333;
const STRING MgConfigProperties::SiteConnectionPropertyQueueSize                            = L"QueueSize";
const INT32  MgConfigProperties::DefaultSiteConnectionPropertyQueueSize                     = 10;
const STRING MgConfigProperties::SiteConnectionPropertyThreadPoolSize                       = L"ThreadPoolSize";
const INT32  MgConfigProperties::DefaultSiteConnectionPropertyThreadPoolSize                = 5;

// ******************************************************************
// Agent Properties
// ******************************************************************
const STRING MgConfigProperties::AgentPropertiesSection                                     = L"AgentProperties";
const STRING MgConfigProperties::AgentDebugPause                                            = L"DebugPause";
const INT32  MgConfigProperties::DefaultAgentDebugPause                                     = 0;
const STRING MgConfigProperties::AgentDisableAuthoring                                      = L"DisableAuthoring";
const STRING MgConfigProperties::AgentDisableWms                                            = L"DisableWms";
const STRING MgConfigProperties::AgentDisableWfs                                            = L"DisableWfs";

// ******************************************************************
// OGC Properties
// ******************************************************************
const STRING MgConfigProperties::OgcPropertiesSection                                       = L"OgcProperties";
const STRING MgConfigProperties::WmsPassword                                                = L"WmsPassword";
const STRING MgConfigProperties::WfsPassword                                                = L"WfsPassword";

// ******************************************************************
// Hosted Properties
// ******************************************************************
const STRING MgConfigProperties::HostPropertiesSection                                      = L"HostProperties";
const STRING MgConfigProperties::HostPropertyDrawingService                                 = L"DrawingService";
const bool   MgConfigProperties::DefaultHostPropertyDrawingService                          = false;
const STRING MgConfigProperties::HostPropertyFeatureService                                 = L"FeatureService";
const bool   MgConfigProperties::DefaultHostPropertyFeatureService                          = false;
const STRING MgConfigProperties::HostPropertyMappingService                                 = L"MappingService";
const bool   MgConfigProperties::DefaultHostPropertyMappingService                          = false;
const STRING MgConfigProperties::HostPropertyRenderingService                               = L"RenderingService";
const bool   MgConfigProperties::DefaultHostPropertyRenderingService                        = false;
const STRING MgConfigProperties::HostPropertyResourceService                                = L"ResourceService";
const bool   MgConfigProperties::DefaultHostPropertyResourceService                         = false;
const STRING MgConfigProperties::HostPropertyServerAdminService                             = L"ServerAdminService"; // for internal use only
const STRING MgConfigProperties::HostPropertySiteService                                    = L"SiteService";
const bool   MgConfigProperties::DefaultHostPropertySiteService                             = false;
const STRING MgConfigProperties::HostPropertyTileService                                    = L"TileService";
const bool   MgConfigProperties::DefaultHostPropertyTileService                             = false;

// ******************************************************************
// Access Log Properties
// ******************************************************************
const STRING MgConfigProperties::AccessLogPropertiesSection                                 = L"AccessLogProperties";
const STRING MgConfigProperties::AccessLogPropertyEnabled                                   = L"Enabled";
const bool   MgConfigProperties::DefaultAccessLogPropertyEnabled                            = true;
const STRING MgConfigProperties::AccessLogPropertyFilename                                  = L"Filename";
const STRING MgConfigProperties::DefaultAccessLogPropertyFilename                           = L"Access.log";
const STRING MgConfigProperties::AccessLogPropertyParameters                                = L"Parameters";
const STRING MgConfigProperties::DefaultAccessLogPropertyParameters                         = L"";

// ******************************************************************
// Admin Log Properties
// ******************************************************************
const STRING MgConfigProperties::AdminLogPropertiesSection                                  = L"AdminLogProperties";
const STRING MgConfigProperties::AdminLogPropertyEnabled                                    = L"Enabled";
const bool   MgConfigProperties::DefaultAdminLogPropertyEnabled                             = true;
const STRING MgConfigProperties::AdminLogPropertyFilename                                   = L"Filename";
const STRING MgConfigProperties::DefaultAdminLogPropertyFilename                            = L"Admin.log";
const STRING MgConfigProperties::AdminLogPropertyParameters                                 = L"Parameters";
const STRING MgConfigProperties::DefaultAdminLogPropertyParameters                          = L"";

// ******************************************************************
// Authentication Log Properties
// ******************************************************************
const STRING MgConfigProperties::AuthenticationLogPropertiesSection                         = L"AuthenticationLogProperties";
const STRING MgConfigProperties::AuthenticationLogPropertyEnabled                           = L"Enabled";
const bool   MgConfigProperties::DefaultAuthenticationLogPropertyEnabled                    = true;
const STRING MgConfigProperties::AuthenticationLogPropertyFilename                          = L"Filename";
const STRING MgConfigProperties::DefaultAuthenticationLogPropertyFilename                   = L"Authentication.log";
const STRING MgConfigProperties::AuthenticationLogPropertyParameters                        = L"Parameters";
const STRING MgConfigProperties::DefaultAuthenticationLogPropertyParameters                 = L"";

// ******************************************************************
// Error Log Properties
// ******************************************************************
const STRING MgConfigProperties::ErrorLogPropertiesSection                                  = L"ErrorLogProperties";
const STRING MgConfigProperties::ErrorLogPropertyEnabled                                    = L"Enabled";
const bool   MgConfigProperties::DefaultErrorLogPropertyEnabled                             = true;
const STRING MgConfigProperties::ErrorLogPropertyFilename                                   = L"Filename";
const STRING MgConfigProperties::DefaultErrorLogPropertyFilename                            = L"Error.log";
const STRING MgConfigProperties::ErrorLogPropertyParameters                                 = L"Parameters";
const STRING MgConfigProperties::DefaultErrorLogPropertyParameters                          = L"";

// ******************************************************************
// Session Log Properties
// ******************************************************************
const STRING MgConfigProperties::SessionLogPropertiesSection                                = L"SessionLogProperties";
const STRING MgConfigProperties::SessionLogPropertyEnabled                                  = L"Enabled";
const bool   MgConfigProperties::DefaultSessionLogPropertyEnabled                           = true;
const STRING MgConfigProperties::SessionLogPropertyFilename                                 = L"Filename";
const STRING MgConfigProperties::DefaultSessionLogPropertyFilename                          = L"Session.log";
const STRING MgConfigProperties::SessionLogPropertyParameters                               = L"Parameters";
const STRING MgConfigProperties::DefaultSessionLogPropertyParameters                        = L"";

// ******************************************************************
// Trace Log Properties
// ******************************************************************
const STRING MgConfigProperties::TraceLogPropertiesSection                                  = L"TraceLogProperties";
const STRING MgConfigProperties::TraceLogPropertyEnabled                                    = L"Enabled";
const bool   MgConfigProperties::DefaultTraceLogPropertyEnabled                             = true;
const STRING MgConfigProperties::TraceLogPropertyFilename                                   = L"Filename";
const STRING MgConfigProperties::DefaultTraceLogPropertyFilename                            = L"Trace.log";
const STRING MgConfigProperties::TraceLogPropertyParameters                                 = L"Parameters";
const STRING MgConfigProperties::DefaultTraceLogPropertyParameters                          = L"";

// ******************************************************************
// Support Servers section
// ******************************************************************
const STRING MgConfigProperties::SupportServersSection                                     = L"SupportServers";
const STRING MgConfigProperties::SupportServerIdentifiersProperty                          = L"Identifiers";

// ******************************************************************
// Support Server section
// ******************************************************************
const STRING MgConfigProperties::SupportServerSection                                      = L"SupportServer_";
const STRING MgConfigProperties::SupportServerNameProperty                                 = L"Name";
const STRING MgConfigProperties::SupportServerDescriptionProperty                          = L"Description";
const STRING MgConfigProperties::SupportServerAddressProperty                              = L"IpAddress";

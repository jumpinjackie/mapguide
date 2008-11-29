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

///////////////////////////////////////////////////////////////////////////////
/// Value ranges of numeric properties
///
#define MG_CONFIG_MAX_INT32                             2147483647 // 0x7FFFFFFF

#define MG_CONFIG_MIN_FS_CACHE_SIZE                     0
#define MG_CONFIG_MAX_FS_CACHE_SIZE                     5000

#define MG_CONFIG_MIN_FS_CACHE_TIMELIMIT                0
#define MG_CONFIG_MAX_FS_CACHE_TIMELIMIT                MG_CONFIG_MAX_INT32

#define MG_CONFIG_MIN_FS_CACHE_TIMERINTERVAL            0
#define MG_CONFIG_MAX_FS_CACHE_TIMERINTERVAL            MG_CONFIG_MAX_INT32

#define MG_CONFIG_MIN_CACHE_SIZE                        1
#define MG_CONFIG_MAX_CACHE_SIZE                        MG_CONFIG_MAX_INT32

#define MG_CONFIG_MIN_CONNECTION_POOL_SIZE              1
#define MG_CONFIG_MAX_CONNECTION_POOL_SIZE              1024

#define MG_CONFIG_MIN_CONNECTIONS                       1
#define MG_CONFIG_MAX_CONNECTIONS                       1024

#define MG_CONFIG_MIN_PORT_NUMBER                       0
#define MG_CONFIG_MAX_PORT_NUMBER                       65535

#define MG_CONFIG_MIN_QUEUE_SIZE                        1
#define MG_CONFIG_MAX_QUEUE_SIZE                        1024

#define MG_CONFIG_MIN_THREAD_POOL_SIZE                  1
#define MG_CONFIG_MAX_THREAD_POOL_SIZE                  1024

#define MG_CONFIG_MIN_TIMEOUT                           1
#define MG_CONFIG_MAX_TIMEOUT                           MG_CONFIG_MAX_INT32

#define MG_CONFIG_MIN_TIMER_INTERVAL                    1
#define MG_CONFIG_MAX_TIMER_INTERVAL                    MG_CONFIG_MAX_INT32

#define MG_CONFIG_MIN_JOIN_QUERY_BATCH_SIZE             1
#define MG_CONFIG_MAX_JOIN_QUERY_BATCH_SIZE             10000

///////////////////////////////////////////////////////////////////////////////
/// Length ranges of string properties
///
#define MG_CONFIG_MIN_EMAIL_LENGTH                      0
#define MG_CONFIG_MAX_EMAIL_LENGTH                      255

#define MG_CONFIG_MIN_FILE_NAME_LENGTH                  1
#define MG_CONFIG_MAX_FILE_NAME_LENGTH                  128
#define MG_CONFIG_FILE_NAME_RESERVED_CHARACTERS         L"\\/:*?\"<>|"

#define MG_CONFIG_MIN_FONT_NAME_LENGTH                  1
#define MG_CONFIG_MAX_FONT_NAME_LENGTH                  255

#define MG_CONFIG_MIN_FOLDER_NAME_LENGTH                1
#define MG_CONFIG_MAX_FOLDER_NAME_LENGTH                128
#define MG_CONFIG_FOLDER_NAME_RESERVED_CHARACTERS       MG_CONFIG_FILE_NAME_RESERVED_CHARACTERS

#define MG_CONFIG_MIN_IP_ADDRESS_LENGTH                 1
#define MG_CONFIG_MAX_IP_ADDRESS_LENGTH                 255

#define MG_CONFIG_MIN_LOCALE_LENGTH                     MG_LOCALE_LENGTH
#define MG_CONFIG_MAX_LOCALE_LENGTH                     MG_LOCALE_LENGTH

#define MG_CONFIG_MIN_OPTIONAL_STRING_LENGTH            0
#define MG_CONFIG_MAX_OPTIONAL_STRING_LENGTH            255

#define MG_CONFIG_MIN_LOG_PARAMETERS_LENGTH             0
#define MG_CONFIG_MAX_LOG_PARAMETERS_LENGTH             1024

#define MG_CONFIG_MIN_PASSWORD_LENGTH                   0
#define MG_CONFIG_MAX_PASSWORD_LENGTH                   64
#define MG_CONFIG_PASSWORD_RESERVED_CHARACTERS          L"\t\r\n\v\f"

#define MG_CONFIG_MIN_PATH_LENGTH                       1
#define MG_CONFIG_MAX_PATH_LENGTH                       255
#define MG_CONFIG_PATH_RESERVED_CHARACTERS              L"*?\"<>|"

#define MG_CONFIG_MIN_SERIAL_NUMBER_LENGTH              0
#define MG_CONFIG_MAX_SERIAL_NUMBER_LENGTH              12

#define MG_CONFIG_MIN_FS_CP_EXCLUDED_LENGTH             0
#define MG_CONFIG_MAX_FS_CP_EXCLUDED_LENGTH             1024

// Default configuration filenames
const STRING MgConfigProperties::DefaultConfigurationFilename                               = L"webconfig.ini";
const STRING MgConfigProperties::ServerConfigurationFilename                                = L"serverconfig.ini";

// Configuration file section/property names and default values

// ******************************************************************
// General Properties
// ******************************************************************
const STRING MgConfigProperties::GeneralPropertiesSection                                   = L"GeneralProperties";
const STRING MgConfigProperties::GeneralPropertyConnectionTimeout                           = L"ConnectionTimeout";
const INT32  MgConfigProperties::DefaultGeneralPropertyConnectionTimeout                    = 120;
const STRING MgConfigProperties::GeneralPropertyConnectionTimerInterval                     = L"ConnectionTimerInterval";
const INT32  MgConfigProperties::DefaultGeneralPropertyConnectionTimerInterval              = 60;
const STRING MgConfigProperties::GeneralPropertyLocale                                      = L"Locale";
const STRING MgConfigProperties::DefaultGeneralPropertyLocale                               = L"";
const STRING MgConfigProperties::GeneralPropertyDefaultMessageLocale                        = L"DefaultMessageLocale";
const STRING MgConfigProperties::DefaultGeneralPropertyDefaultMessageLocale                 = L"en";      // ISO 639-1 name
const STRING MgConfigProperties::GeneralPropertyDisplayName                                 = L"DisplayName";
const STRING MgConfigProperties::DefaultGeneralPropertyDisplayName                          = L"";
const STRING MgConfigProperties::GeneralPropertyFdoPath                                     = L"FdoPath";
const STRING MgConfigProperties::DefaultGeneralPropertyFdoPath                              = L"";
const STRING MgConfigProperties::GeneralPropertyLicenseServerPath                           = L"LicenseServerPath";
const STRING MgConfigProperties::DefaultGeneralPropertyLicenseServerPath                    = L"";
const STRING MgConfigProperties::GeneralPropertyLogsDelimiter                               = L"LogsDelimiter";
const STRING MgConfigProperties::DefaultGeneralPropertyLogsDelimiter                        = L"\t";
const STRING MgConfigProperties::GeneralPropertyLogsDetail                                  = L"LogsDetail";
const STRING MgConfigProperties::DefaultGeneralPropertyLogsDetail                           = L"";
const STRING MgConfigProperties::GeneralPropertyLogsPath                                    = L"LogsPath";
const STRING MgConfigProperties::DefaultGeneralPropertyLogsPath                             = L"Logs/";
const STRING MgConfigProperties::GeneralPropertyMachineIp                                   = L"MachineIp";
const STRING MgConfigProperties::DefaultGeneralPropertyMachineIp                            = L"127.0.0.1";
const STRING MgConfigProperties::GeneralPropertyMaxLogFileSize                              = L"MaxLogFileSize";
const INT32  MgConfigProperties::DefaultGeneralPropertyMaxLogFileSize                       = 1024;
const STRING MgConfigProperties::GeneralPropertyMaxLogFileSizeEnabled                       = L"MaxLogFileSizeEnabled";
const bool   MgConfigProperties::DefaultGeneralPropertyMaxLogFileSizeEnabled                = false;
const STRING MgConfigProperties::GeneralPropertyResourcesPath                               = L"ResourcesPath";
const STRING MgConfigProperties::DefaultGeneralPropertyResourcesPath                        = L"Resources/";
const STRING MgConfigProperties::GeneralPropertySerialNumber                                = L"SerialNumber";
const STRING MgConfigProperties::DefaultGeneralPropertySerialNumber                         = L"";
const STRING MgConfigProperties::GeneralPropertyServiceRegistrationTimerInterval            = L"ServiceRegistrationTimerInterval";
const INT32  MgConfigProperties::DefaultGeneralPropertyServiceRegistrationTimerInterval     = 60;
const STRING MgConfigProperties::GeneralPropertyTcpIpMtu                                    = L"TcpIpMtu";
const INT32  MgConfigProperties::DefaultGeneralPropertyTcpIpMtu                             = 1460;
const STRING MgConfigProperties::GeneralPropertyTempPath                                    = L"TempPath";
const STRING MgConfigProperties::DefaultGeneralPropertyTempPath                             = L"Temp/";
const STRING MgConfigProperties::GeneralPropertyWfsDocumentPath                             = L"WfsDocumentPath";
const STRING MgConfigProperties::DefaultGeneralPropertyWfsDocumentPath                      = L"Wfs/";
const STRING MgConfigProperties::GeneralPropertyWmsDocumentPath                             = L"WmsDocumentPath";
const STRING MgConfigProperties::DefaultGeneralPropertyWmsDocumentPath                      = L"Wms/";
const STRING MgConfigProperties::GeneralPropertyWorkerThreadPoolSize                        = L"WorkerThreadPoolSize"; // for internal use only
const INT32  MgConfigProperties::DefaultGeneralPropertyWorkerThreadPoolSize                 = 10;
const STRING MgConfigProperties::GeneralPropertyRenderer                                    = L"Renderer";
const STRING MgConfigProperties::DefaultGeneralPropertyRenderer                             = L"GD";

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
const INT32  MgConfigProperties::DefaultClientConnectionPropertyQueueSize                   = 20;
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
// Hosted Properties
// ******************************************************************
const STRING MgConfigProperties::HostPropertiesSection                                      = L"HostProperties";
const STRING MgConfigProperties::HostPropertyDrawingService                                 = L"DrawingService";
const bool   MgConfigProperties::DefaultHostPropertyDrawingService                          = false;
const STRING MgConfigProperties::HostPropertyFeatureService                                 = L"FeatureService";
const bool   MgConfigProperties::DefaultHostPropertyFeatureService                          = false;
const STRING MgConfigProperties::HostPropertyKmlService                                     = L"KmlService";
const bool   MgConfigProperties::DefaultHostPropertyKmlService                              = false;
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
// Drawing Service Properties
// ******************************************************************
const STRING MgConfigProperties::DrawingServicePropertiesSection                            = L"DrawingServiceProperties";

// ******************************************************************
// Feature Service Properties
// ******************************************************************
const STRING MgConfigProperties::FeatureServicePropertiesSection                            = L"FeatureServiceProperties";
const STRING MgConfigProperties::FeatureServicePropertyCacheSize                            = L"CacheSize";
const INT32  MgConfigProperties::DefaultFeatureServicePropertyCacheSize                     = 100;
const STRING MgConfigProperties::FeatureServicePropertyCacheTimeLimit                       = L"CacheTimeLimit";
const INT32  MgConfigProperties::DefaultFeatureServicePropertyCacheTimeLimit                = 86400;
const STRING MgConfigProperties::FeatureServicePropertyCacheTimerInterval                   = L"CacheTimerInterval";
const INT32  MgConfigProperties::DefaultFeatureServicePropertyCacheTimerInterval            = 3600;
const STRING MgConfigProperties::FeatureServicePropertyDataCacheSize                        = L"DataCacheSize";
const INT32  MgConfigProperties::DefaultFeatureServicePropertyDataCacheSize                 = 100;
const STRING MgConfigProperties::FeatureServicePropertyDataConnectionPoolEnabled            = L"DataConnectionPoolEnabled";
const bool   MgConfigProperties::DefaultFeatureServicePropertyDataConnectionPoolEnabled     = false;
const STRING MgConfigProperties::FeatureServicePropertyDataConnectionPoolExcludedProviders  = L"DataConnectionPoolExcludedProviders";
const STRING MgConfigProperties::DefaultFeatureServicePropertyDataConnectionPoolExcludedProviders = L""; // This means all providers are cached
const STRING MgConfigProperties::FeatureServicePropertyDataConnectionPoolSize               = L"DataConnectionPoolSize";
const INT32  MgConfigProperties::DefaultFeatureServicePropertyDataConnectionPoolSize        = 50;
const STRING MgConfigProperties::FeatureServicePropertyDataConnectionPoolSizeCustom         = L"DataConnectionPoolSizeCustom";
const STRING MgConfigProperties::DefaultFeatureServicePropertyDataConnectionPoolSizeCustom  = L"";
const STRING MgConfigProperties::FeatureServicePropertyDataConnectionTimeout                = L"DataConnectionTimeout";
const INT32  MgConfigProperties::DefaultFeatureServicePropertyDataConnectionTimeout         = 600;
const STRING MgConfigProperties::FeatureServicePropertyDataConnectionTimerInterval          = L"DataConnectionTimerInterval";
const INT32  MgConfigProperties::DefaultFeatureServicePropertyDataConnectionTimerInterval   = 60;
const STRING MgConfigProperties::FeatureServicePropertyJoinQueryBatchSize                   = L"JoinQueryBatchSize";
const INT32  MgConfigProperties::DefaultFeatureServicePropertyJoinQueryBatchSize            = 100;

// ******************************************************************
// Mapping Service Properties
// ******************************************************************
const STRING MgConfigProperties::MappingServicePropertiesSection                            = L"MappingServiceProperties";
const STRING MgConfigProperties::MappingServicePropertyLegendFont                           = L"LegendFont";
const STRING MgConfigProperties::DefaultMappingServicePropertyLegendFont                    = L"Arial";

// ******************************************************************
// Rendering Service Properties
// ******************************************************************
const STRING MgConfigProperties::RenderingServicePropertiesSection                          = L"RenderingServiceProperties";
const STRING MgConfigProperties::RenderingServicePropertyTileExtentOffset                   = L"TileExtentOffset";
const double MgConfigProperties::DefaultRenderingServicePropertyTileExtentOffset            = 0.35;
const STRING MgConfigProperties::RenderingServicePropertyRasterGridSize                     = L"RasterGridSize";
const INT32  MgConfigProperties::DefaultRenderingServicePropertyRasterGridSize              = 100;
const STRING MgConfigProperties::RenderingServicePropertyMinRasterGridSize                  = L"MinRasterGridSize";
const INT32  MgConfigProperties::DefaultRenderingServicePropertyMinRasterGridSize           = 10;
const STRING MgConfigProperties::RenderingServicePropertyRasterGridSizeOverrideRatio        = L"RasterGridSizeOverrideRatio";
const double MgConfigProperties::DefaultRenderingServicePropertyRasterGridSizeOverrideRatio = 0.25;
const STRING MgConfigProperties::RenderingServicePropertyRenderSelectionBatchSize           = L"RenderSelectionBatchSize";
const INT32  MgConfigProperties::DefaultRenderingServicePropertyRenderSelectionBatchSize    = 10000;

// ******************************************************************
// Font Alias Mappings section
// ******************************************************************
const STRING MgConfigProperties::FontAliasMappingSection                                    = L"FontAliases";

// ******************************************************************
// Unmanaged Data Mappings section
// ******************************************************************
const STRING MgConfigProperties::UnmanagedDataMappingsSection                               = L"UnmanagedDataMappings";

// ******************************************************************
// Resource Service Properties
// ******************************************************************
const STRING MgConfigProperties::ResourceServicePropertiesSection                               = L"ResourceServiceProperties";
const STRING MgConfigProperties::ResourceServicePropertyLibraryRepositoryPath                   = L"LibraryRepositoryPath";
const STRING MgConfigProperties::DefaultResourceServicePropertyLibraryRepositoryPath            = L"Repositories/Library/";
const STRING MgConfigProperties::ResourceServicePropertyLibraryResourceDataFilePath             = L"LibraryResourceDataFilePath";
const STRING MgConfigProperties::DefaultResourceServicePropertyLibraryResourceDataFilePath      = L"Repositories/Library/DataFiles/";
const STRING MgConfigProperties::ResourceServicePropertyPackagesPath                            = L"PackagesPath";
const STRING MgConfigProperties::DefaultResourceServicePropertyPackagesPath                     = L"Packages/";
const STRING MgConfigProperties::ResourceServicePropertyRepositoryCheckpointsTimerInterval      = L"RepositoryCheckpointsTimerInterval";
const INT32  MgConfigProperties::DefaultResourceServicePropertyRepositoryCheckpointsTimerInterval = 600;
const STRING MgConfigProperties::ResourceServicePropertyResourceChangeTimerInterval             = L"ResourceChangeTimerInterval";
const INT32  MgConfigProperties::DefaultResourceServicePropertyResourceChangeTimerInterval      = 5;
const STRING MgConfigProperties::ResourceServicePropertyResourceDataFileTrashFolderName         = L"ResourceDataFileTrashFolderName";
const STRING MgConfigProperties::DefaultResourceServicePropertyResourceDataFileTrashFolderName  = L"Trash";
const STRING MgConfigProperties::ResourceServicePropertyResourcePermissionCacheSize             = L"ResourcePermissionCacheSize";
const INT32  MgConfigProperties::DefaultResourceServicePropertyResourcePermissionCacheSize      = 1000;
const STRING MgConfigProperties::ResourceServicePropertyResourceSchemaFilePath                  = L"ResourceSchemaFilePath";
const STRING MgConfigProperties::DefaultResourceServicePropertyResourceSchemaFilePath           = L"Schema/";
const STRING MgConfigProperties::ResourceServicePropertySessionRepositoryPath                   = L"SessionRepositoryPath";
const STRING MgConfigProperties::DefaultResourceServicePropertySessionRepositoryPath            = L"Repositories/Session/";
const STRING MgConfigProperties::ResourceServicePropertySessionResourceDataFilePath             = L"SessionResourceDataFilePath";
const STRING MgConfigProperties::DefaultResourceServicePropertySessionResourceDataFilePath      = L"Repositories/Session/DataFiles/";
const STRING MgConfigProperties::ResourceServicePropertySiteRepositoryPath                      = L"SiteRepositoryPath";
const STRING MgConfigProperties::DefaultResourceServicePropertySiteRepositoryPath               = L"Repositories/Site/";
const STRING MgConfigProperties::ResourceServicePropertyResourceValidationEnabled               = L"ResourceValidationEnabled"; // for internal use only
const bool   MgConfigProperties::DefaultResourceServicePropertyResourceValidationEnabled        = true;
const STRING MgConfigProperties::ResourceServicePropertyRetryAttempts                           = L"RetryAttempts";             // for internal use only
const INT32  MgConfigProperties::DefaultResourceServicePropertyRetryAttempts                    = 10;
const STRING MgConfigProperties::ResourceServicePropertyRetryInterval                           = L"RetryInterval";             // for internal use only
const INT32  MgConfigProperties::DefaultResourceServicePropertyRetryInterval                    = 10;

// ******************************************************************
// Site Service Properties
// ******************************************************************
const STRING MgConfigProperties::SiteServicePropertiesSection                               = L"SiteServiceProperties";
const STRING MgConfigProperties::SiteServicePropertySessionTimeout                          = L"SessionTimeout";
const INT32  MgConfigProperties::DefaultSiteServicePropertySessionTimeout                   = 1200;
const STRING MgConfigProperties::SiteServicePropertySessionTimerInterval                    = L"SessionTimerInterval";
const INT32  MgConfigProperties::DefaultSiteServicePropertySessionTimerInterval             = 400;

// ******************************************************************
// Tile Service Properties
// ******************************************************************
const STRING MgConfigProperties::TileServicePropertiesSection                               = L"TileServiceProperties";
const STRING MgConfigProperties::TileServicePropertyRenderOnly                              = L"RenderOnly";
const bool   MgConfigProperties::DefaultTileServicePropertyRenderOnly                       = false;
const STRING MgConfigProperties::TileServicePropertyTileCachePath                           = L"TileCachePath";
const STRING MgConfigProperties::DefaultTileServicePropertyTileCachePath                    = L"Repositories/TileCache/";
const STRING MgConfigProperties::TileServicePropertyTileColumnsPerFolder                    = L"TileColumnsPerFolder";
const INT32 MgConfigProperties::DefaultTileServicePropertyTileColumnsPerFolder              = 30;
const STRING MgConfigProperties::TileServicePropertyTileRowsPerFolder                       = L"TileRowsPerFolder";
const INT32 MgConfigProperties::DefaultTileServicePropertyTileRowsPerFolder                 = 30;
const STRING MgConfigProperties::TileServicePropertyCreationCutoffTime                      = L"CreationCutoffTime";// for internal use only
const INT32  MgConfigProperties::DefaultTileServicePropertyCreationCutoffTime               = 120;
const STRING MgConfigProperties::TileServicePropertyPollingInterval                         = L"PollingInterval";   // for internal use only
const INT32  MgConfigProperties::DefaultTileServicePropertyPollingInterval                  = 1;
const STRING MgConfigProperties::TileServicePropertyTiledMapCacheSize                       = L"TiledMapCacheSize"; // for internal use only
const INT32  MgConfigProperties::DefaultTileServicePropertyTiledMapCacheSize                = 10;
const STRING MgConfigProperties::TileServicePropertyTileSizeX                               = L"DefaultTileSizeX";
const INT32 MgConfigProperties::DefaultTileServicePropertyTileSizeX                         = 300;
const STRING MgConfigProperties::TileServicePropertyTileSizeY                               = L"DefaultTileSizeY";
const INT32 MgConfigProperties::DefaultTileServicePropertyTileSizeY                         = 300;
const STRING MgConfigProperties::TileServicePropertyImageFormat                             = L"ImageFormat";
const STRING MgConfigProperties::DefaultTileServicePropertyImageFormat                      = L"PNG";

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
const bool   MgConfigProperties::DefaultSessionLogPropertyEnabled                           = false;
const STRING MgConfigProperties::SessionLogPropertyFilename                                 = L"Filename";
const STRING MgConfigProperties::DefaultSessionLogPropertyFilename                          = L"Session.log";
const STRING MgConfigProperties::SessionLogPropertyParameters                               = L"Parameters";
const STRING MgConfigProperties::DefaultSessionLogPropertyParameters                        = L"";

// ******************************************************************
// Trace Log Properties
// ******************************************************************
const STRING MgConfigProperties::TraceLogPropertiesSection                                  = L"TraceLogProperties";
const STRING MgConfigProperties::TraceLogPropertyEnabled                                    = L"Enabled";
const bool   MgConfigProperties::DefaultTraceLogPropertyEnabled                             = false;
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

// ******************************************************************
// Agent Properties
// ******************************************************************
const STRING MgConfigProperties::AgentPropertiesSection                                     = L"AgentProperties";
const STRING MgConfigProperties::AgentDebugPause                                            = L"DebugPause";
const INT32  MgConfigProperties::DefaultAgentDebugPause                                     = 0;
const STRING MgConfigProperties::AgentDisableAuthoring                                      = L"DisableAuthoring";
const STRING MgConfigProperties::AgentDisableWfs                                            = L"DisableWfs";
const STRING MgConfigProperties::AgentDisableWms                                            = L"DisableWms";

// ******************************************************************
// OGC Properties
// ******************************************************************
const STRING MgConfigProperties::OgcPropertiesSection                                       = L"OgcProperties";
const STRING MgConfigProperties::WfsPassword                                                = L"WfsPassword";
const STRING MgConfigProperties::WmsPassword                                                = L"WmsPassword";

// ******************************************************************
// Web Application Properties
// ******************************************************************
const STRING MgConfigProperties::WebApplicationPropertiesSection                            = L"WebApplicationProperties";
const STRING MgConfigProperties::TemplateRootFolder                                         = L"TemplateRootFolder";
const STRING MgConfigProperties::WidgetInfoFolder                                           = L"WidgetInfoFolder";
const STRING MgConfigProperties::ContainerInfoFolder                                        = L"ContainerInfoFolder";

///////////////////////////////////////////////////////////////////////////////
/// Common Configuration Validation Information Maps
///
const MgConfigValidationInfo MgConfigProperties::sm_cviGeneralProperties[] =
{
    { MgConfigProperties::GeneralPropertyConnectionTimeout                          , MgPropertyType::Int32     , MG_CONFIG_MIN_TIMEOUT                 , MG_CONFIG_MAX_TIMEOUT                 , L""                                       },
    { MgConfigProperties::GeneralPropertyConnectionTimerInterval                    , MgPropertyType::Int32     , MG_CONFIG_MIN_TIMER_INTERVAL          , MG_CONFIG_MAX_TIMER_INTERVAL          , L""                                       },
    { MgConfigProperties::GeneralPropertyLocale                                     , MgPropertyType::String    , MG_CONFIG_MIN_OPTIONAL_STRING_LENGTH  , MG_CONFIG_MAX_OPTIONAL_STRING_LENGTH  , L""                                       },
    { MgConfigProperties::GeneralPropertyDefaultMessageLocale                       , MgPropertyType::String    , MG_CONFIG_MIN_LOCALE_LENGTH           , MG_CONFIG_MAX_LOCALE_LENGTH           , L""                                       },
    { MgConfigProperties::GeneralPropertyDisplayName                                , MgPropertyType::String    , MG_CONFIG_MIN_OPTIONAL_STRING_LENGTH  , MG_CONFIG_MAX_OPTIONAL_STRING_LENGTH  , MG_CONFIG_FILE_NAME_RESERVED_CHARACTERS   },
    { MgConfigProperties::GeneralPropertyFdoPath                                    , MgPropertyType::String    , MG_CONFIG_MIN_PATH_LENGTH             , MG_CONFIG_MAX_PATH_LENGTH             , MG_CONFIG_PATH_RESERVED_CHARACTERS        },
    { MgConfigProperties::GeneralPropertyLicenseServerPath                          , MgPropertyType::String    , MG_CONFIG_MIN_OPTIONAL_STRING_LENGTH  , MG_CONFIG_MAX_OPTIONAL_STRING_LENGTH  , L""                                       },
    { MgConfigProperties::GeneralPropertyLogsDelimiter                              , MgPropertyType::String    , 1                                     , 128                                   , L""                                       },
    { MgConfigProperties::GeneralPropertyLogsDetail                                 , MgPropertyType::String    , MG_CONFIG_MIN_OPTIONAL_STRING_LENGTH  , MG_CONFIG_MAX_OPTIONAL_STRING_LENGTH  , L""                                       },
    { MgConfigProperties::GeneralPropertyLogsPath                                   , MgPropertyType::String    , MG_CONFIG_MIN_PATH_LENGTH             , MG_CONFIG_MAX_PATH_LENGTH             , MG_CONFIG_PATH_RESERVED_CHARACTERS        },
    { MgConfigProperties::GeneralPropertyMachineIp                                  , MgPropertyType::String    , MG_CONFIG_MIN_IP_ADDRESS_LENGTH       , MG_CONFIG_MAX_IP_ADDRESS_LENGTH       , L""                                       },
    { MgConfigProperties::GeneralPropertyMaxLogFileSize                             , MgPropertyType::Int32     , 1                                     , 2000000                               , L""                                       },
    { MgConfigProperties::GeneralPropertyMaxLogFileSizeEnabled                      , MgPropertyType::Boolean   , 0                                     , 1                                     , L""                                       },
    { MgConfigProperties::GeneralPropertyResourcesPath                              , MgPropertyType::String    , MG_CONFIG_MIN_PATH_LENGTH             , MG_CONFIG_MAX_PATH_LENGTH             , MG_CONFIG_PATH_RESERVED_CHARACTERS        },
    { MgConfigProperties::GeneralPropertySerialNumber                               , MgPropertyType::String    , MG_CONFIG_MIN_SERIAL_NUMBER_LENGTH    , MG_CONFIG_MAX_SERIAL_NUMBER_LENGTH    , L""                                       },
    { MgConfigProperties::GeneralPropertyServiceRegistrationTimerInterval           , MgPropertyType::Int32     , MG_CONFIG_MIN_TIMER_INTERVAL          , MG_CONFIG_MAX_TIMER_INTERVAL          , L""                                       },
    { MgConfigProperties::GeneralPropertyTcpIpMtu                                   , MgPropertyType::Int32     , 1                                     , 65535                                 , L""                                       },
    { MgConfigProperties::GeneralPropertyTempPath                                   , MgPropertyType::String    , MG_CONFIG_MIN_PATH_LENGTH             , MG_CONFIG_MAX_PATH_LENGTH             , MG_CONFIG_PATH_RESERVED_CHARACTERS        },
    { MgConfigProperties::GeneralPropertyWfsDocumentPath                            , MgPropertyType::String    , MG_CONFIG_MIN_PATH_LENGTH             , MG_CONFIG_MAX_PATH_LENGTH             , MG_CONFIG_PATH_RESERVED_CHARACTERS        },
    { MgConfigProperties::GeneralPropertyWmsDocumentPath                            , MgPropertyType::String    , MG_CONFIG_MIN_PATH_LENGTH             , MG_CONFIG_MAX_PATH_LENGTH             , MG_CONFIG_PATH_RESERVED_CHARACTERS        },
    { MgConfigProperties::GeneralPropertyWorkerThreadPoolSize                       , MgPropertyType::Int32     , MG_CONFIG_MIN_THREAD_POOL_SIZE        , MG_CONFIG_MAX_THREAD_POOL_SIZE        , L""                                       },
    { L""                                                                           , 0                         , 0.0                                   , 0.0                                   , L""                                       }
};

const MgConfigValidationInfo MgConfigProperties::sm_cviAdministrativeConnectionProperties[] =
{
    { MgConfigProperties::AdministrativeConnectionPropertyEmail                     , MgPropertyType::String    , MG_CONFIG_MIN_EMAIL_LENGTH            , MG_CONFIG_MAX_EMAIL_LENGTH            , L""                                       },
    { MgConfigProperties::AdministrativeConnectionPropertyMaxConnections            , MgPropertyType::Int32     , MG_CONFIG_MIN_CONNECTIONS             , MG_CONFIG_MAX_CONNECTIONS             , L""                                       },
    { MgConfigProperties::AdministrativeConnectionPropertyPort                      , MgPropertyType::Int32     , MG_CONFIG_MIN_PORT_NUMBER             , MG_CONFIG_MAX_PORT_NUMBER             , L""                                       },
    { MgConfigProperties::AdministrativeConnectionPropertyQueueSize                 , MgPropertyType::Int32     , MG_CONFIG_MIN_QUEUE_SIZE              , MG_CONFIG_MAX_QUEUE_SIZE              , L""                                       },
    { MgConfigProperties::AdministrativeConnectionPropertyThreadPoolSize            , MgPropertyType::Int32     , MG_CONFIG_MIN_THREAD_POOL_SIZE        , MG_CONFIG_MAX_THREAD_POOL_SIZE        , L""                                       },
    { L""                                                                           , 0                         , 0.0                                   , 0.0                                   , L""                                       }
};

const MgConfigValidationInfo MgConfigProperties::sm_cviClientConnectionProperties[] =
{
    { MgConfigProperties::ClientConnectionPropertyMaxConnections                    , MgPropertyType::Int32     , MG_CONFIG_MIN_CONNECTIONS             , MG_CONFIG_MAX_CONNECTIONS             , L""                                       },
    { MgConfigProperties::ClientConnectionPropertyPort                              , MgPropertyType::Int32     , MG_CONFIG_MIN_PORT_NUMBER             , MG_CONFIG_MAX_PORT_NUMBER             , L""                                       },
    { MgConfigProperties::ClientConnectionPropertyQueueSize                         , MgPropertyType::Int32     , MG_CONFIG_MIN_QUEUE_SIZE              , MG_CONFIG_MAX_QUEUE_SIZE              , L""                                       },
    { MgConfigProperties::ClientConnectionPropertyThreadPoolSize                    , MgPropertyType::Int32     , MG_CONFIG_MIN_THREAD_POOL_SIZE        , MG_CONFIG_MAX_THREAD_POOL_SIZE        , L""                                       },
    { L""                                                                           , 0                         , 0.0                                   , 0.0                                   , L""                                       }
};

const MgConfigValidationInfo MgConfigProperties::sm_cviSiteConnectionProperties[] =
{
    { MgConfigProperties::SiteConnectionPropertyIpAddress                           , MgPropertyType::String    , MG_CONFIG_MIN_IP_ADDRESS_LENGTH       , MG_CONFIG_MAX_IP_ADDRESS_LENGTH       , L""                                       },
    { MgConfigProperties::SiteConnectionPropertyMaxConnections                      , MgPropertyType::Int32     , MG_CONFIG_MIN_CONNECTIONS             , MG_CONFIG_MAX_CONNECTIONS             , L""                                       },
    { MgConfigProperties::SiteConnectionPropertyPort                                , MgPropertyType::Int32     , MG_CONFIG_MIN_PORT_NUMBER             , MG_CONFIG_MAX_PORT_NUMBER             , L""                                       },
    { MgConfigProperties::SiteConnectionPropertyQueueSize                           , MgPropertyType::Int32     , MG_CONFIG_MIN_QUEUE_SIZE              , MG_CONFIG_MAX_QUEUE_SIZE              , L""                                       },
    { MgConfigProperties::SiteConnectionPropertyThreadPoolSize                      , MgPropertyType::Int32     , MG_CONFIG_MIN_THREAD_POOL_SIZE        , MG_CONFIG_MAX_THREAD_POOL_SIZE        , L""                                       },
    { L""                                                                           , 0                         , 0.0                                   , 0.0                                   , L""                                       }
};

///////////////////////////////////////////////////////////////////////////////
/// Server Configuration Validation Information Maps
///
const MgConfigValidationInfo MgConfigProperties::sm_cviHostProperties[] =
{
    { MgConfigProperties::HostPropertyDrawingService                                , MgPropertyType::Boolean   , 0                                     , 1                                     , L""                                       },
    { MgConfigProperties::HostPropertyFeatureService                                , MgPropertyType::Boolean   , 0                                     , 1                                     , L""                                       },
    { MgConfigProperties::HostPropertyKmlService                                    , MgPropertyType::Boolean   , 0                                     , 1                                     , L""                                       },
    { MgConfigProperties::HostPropertyMappingService                                , MgPropertyType::Boolean   , 0                                     , 1                                     , L""                                       },
    { MgConfigProperties::HostPropertyRenderingService                              , MgPropertyType::Boolean   , 0                                     , 1                                     , L""                                       },
    { MgConfigProperties::HostPropertyResourceService                               , MgPropertyType::Boolean   , 0                                     , 1                                     , L""                                       },
    { MgConfigProperties::HostPropertyServerAdminService                            , MgPropertyType::Boolean   , 1                                     , 1                                     , L""                                       },
    { MgConfigProperties::HostPropertySiteService                                   , MgPropertyType::Boolean   , 0                                     , 1                                     , L""                                       },
    { MgConfigProperties::HostPropertyTileService                                   , MgPropertyType::Boolean   , 0                                     , 1                                     , L""                                       },
    { L""                                                                           , 0                         , 0.0                                   , 0.0                                   , L""                                       }
};

const MgConfigValidationInfo MgConfigProperties::sm_cviDrawingServiceProperties[] =
{
    { L""                                                                           , 0                         , 0.0                                   , 0.0                                  , L""                                        }
};

const MgConfigValidationInfo MgConfigProperties::sm_cviFeatureServiceProperties[] =
{
    { MgConfigProperties::FeatureServicePropertyCacheSize                           , MgPropertyType::Int32     , MG_CONFIG_MIN_FS_CACHE_SIZE           , MG_CONFIG_MAX_FS_CACHE_SIZE           , L""                                       },
    { MgConfigProperties::FeatureServicePropertyCacheTimeLimit                      , MgPropertyType::Int32     , MG_CONFIG_MIN_FS_CACHE_TIMELIMIT      , MG_CONFIG_MAX_FS_CACHE_TIMELIMIT      , L""                                       },
    { MgConfigProperties::FeatureServicePropertyCacheTimerInterval                  , MgPropertyType::Int32     , MG_CONFIG_MIN_FS_CACHE_TIMERINTERVAL  , MG_CONFIG_MAX_FS_CACHE_TIMERINTERVAL  , L""                                       },
    { MgConfigProperties::FeatureServicePropertyDataCacheSize                       , MgPropertyType::Int32     , MG_CONFIG_MIN_CACHE_SIZE              , MG_CONFIG_MAX_CACHE_SIZE              , L""                                       },
    { MgConfigProperties::FeatureServicePropertyDataConnectionPoolEnabled           , MgPropertyType::Boolean   , 0                                     , 1                                     , L""                                       },
    { MgConfigProperties::FeatureServicePropertyDataConnectionPoolExcludedProviders , MgPropertyType::String    , MG_CONFIG_MIN_FS_CP_EXCLUDED_LENGTH   , MG_CONFIG_MAX_FS_CP_EXCLUDED_LENGTH   , L""                                       },
    { MgConfigProperties::FeatureServicePropertyDataConnectionPoolSize              , MgPropertyType::Int32     , MG_CONFIG_MIN_CONNECTION_POOL_SIZE    , MG_CONFIG_MAX_CONNECTION_POOL_SIZE    , L""                                       },
    { MgConfigProperties::FeatureServicePropertyDataConnectionPoolSizeCustom        , MgPropertyType::String    , MG_CONFIG_MIN_FS_CP_EXCLUDED_LENGTH   , MG_CONFIG_MAX_FS_CP_EXCLUDED_LENGTH   , L""                                       },
    { MgConfigProperties::FeatureServicePropertyDataConnectionTimeout               , MgPropertyType::Int32     , MG_CONFIG_MIN_TIMEOUT                 , MG_CONFIG_MAX_TIMEOUT                 , L""                                       },
    { MgConfigProperties::FeatureServicePropertyDataConnectionTimerInterval         , MgPropertyType::Int32     , MG_CONFIG_MIN_TIMER_INTERVAL          , MG_CONFIG_MAX_TIMER_INTERVAL          , L""                                       },
    { MgConfigProperties::FeatureServicePropertyJoinQueryBatchSize                  , MgPropertyType::Int32     , MG_CONFIG_MIN_JOIN_QUERY_BATCH_SIZE   , MG_CONFIG_MAX_JOIN_QUERY_BATCH_SIZE   , L""                                       },
    { L""                                                                           , 0                         , 0.0                                   , 0.0                                   , L""                                       }
};

const MgConfigValidationInfo MgConfigProperties::sm_cviMappingServiceProperties[] =
{
    { MgConfigProperties::MappingServicePropertyLegendFont                          , MgPropertyType::String    , MG_CONFIG_MIN_FONT_NAME_LENGTH        , MG_CONFIG_MAX_FONT_NAME_LENGTH        , L""                                       },
    { L""                                                                           , 0                         , 0.0                                   , 0.0                                   , L""                                       }
};

const MgConfigValidationInfo MgConfigProperties::sm_cviRenderingServiceProperties[] =
{
    { MgConfigProperties::RenderingServicePropertyTileExtentOffset                  , MgPropertyType::Double    , 0.0                                   , 1.0                                   , L""                                       },
    { MgConfigProperties::RenderingServicePropertyRasterGridSize                    , MgPropertyType::Int32     , 1                                     , MG_CONFIG_MAX_INT32                   , L""                                       },
    { MgConfigProperties::RenderingServicePropertyMinRasterGridSize                 , MgPropertyType::Int32     , 1                                     , MG_CONFIG_MAX_INT32                   , L""                                       },
    { MgConfigProperties::RenderingServicePropertyRasterGridSizeOverrideRatio       , MgPropertyType::Double    , 0.0                                   , 1.0                                   , L""                                       },
    { MgConfigProperties::RenderingServicePropertyRenderSelectionBatchSize          , MgPropertyType::Int32     , 0                                     , MG_CONFIG_MAX_INT32                   , L""                                       },
    { L""                                                                           , 0                         , 0.0                                   , 0.0                                   , L""                                       }
};

const MgConfigValidationInfo MgConfigProperties::sm_cviResourceServiceProperties[] =
{
    { MgConfigProperties::ResourceServicePropertyLibraryRepositoryPath              , MgPropertyType::String    , MG_CONFIG_MIN_PATH_LENGTH             , MG_CONFIG_MAX_PATH_LENGTH             , MG_CONFIG_PATH_RESERVED_CHARACTERS        },
    { MgConfigProperties::ResourceServicePropertyLibraryResourceDataFilePath        , MgPropertyType::String    , MG_CONFIG_MIN_PATH_LENGTH             , MG_CONFIG_MAX_PATH_LENGTH             , MG_CONFIG_PATH_RESERVED_CHARACTERS        },
    { MgConfigProperties::ResourceServicePropertyPackagesPath                       , MgPropertyType::String    , MG_CONFIG_MIN_PATH_LENGTH             , MG_CONFIG_MAX_PATH_LENGTH             , MG_CONFIG_PATH_RESERVED_CHARACTERS        },
    { MgConfigProperties::ResourceServicePropertyRepositoryCheckpointsTimerInterval , MgPropertyType::Int32     , MG_CONFIG_MIN_TIMER_INTERVAL          , MG_CONFIG_MAX_TIMER_INTERVAL          , L""                                       },
    { MgConfigProperties::ResourceServicePropertyResourceChangeTimerInterval        , MgPropertyType::Int32     , MG_CONFIG_MIN_TIMER_INTERVAL          , MG_CONFIG_MAX_TIMER_INTERVAL          , L""                                       },
    { MgConfigProperties::ResourceServicePropertyResourceDataFileTrashFolderName    , MgPropertyType::String    , MG_CONFIG_MIN_FOLDER_NAME_LENGTH      , MG_CONFIG_MAX_FOLDER_NAME_LENGTH      , MG_CONFIG_FOLDER_NAME_RESERVED_CHARACTERS },
    { MgConfigProperties::ResourceServicePropertyResourcePermissionCacheSize        , MgPropertyType::Int32     , MG_CONFIG_MIN_CACHE_SIZE              , MG_CONFIG_MAX_CACHE_SIZE              , L""                                       },
    { MgConfigProperties::ResourceServicePropertyResourceSchemaFilePath             , MgPropertyType::String    , MG_CONFIG_MIN_PATH_LENGTH             , MG_CONFIG_MAX_PATH_LENGTH             , MG_CONFIG_PATH_RESERVED_CHARACTERS        },
    { MgConfigProperties::ResourceServicePropertySessionRepositoryPath              , MgPropertyType::String    , MG_CONFIG_MIN_PATH_LENGTH             , MG_CONFIG_MAX_PATH_LENGTH             , MG_CONFIG_PATH_RESERVED_CHARACTERS        },
    { MgConfigProperties::ResourceServicePropertySessionResourceDataFilePath        , MgPropertyType::String    , MG_CONFIG_MIN_PATH_LENGTH             , MG_CONFIG_MAX_PATH_LENGTH             , MG_CONFIG_PATH_RESERVED_CHARACTERS        },
    { MgConfigProperties::ResourceServicePropertySiteRepositoryPath                 , MgPropertyType::String    , MG_CONFIG_MIN_PATH_LENGTH             , MG_CONFIG_MAX_PATH_LENGTH             , MG_CONFIG_PATH_RESERVED_CHARACTERS        },
    { MgConfigProperties::ResourceServicePropertyResourceValidationEnabled          , MgPropertyType::Boolean   , 0                                     , 1                                     , L""                                       },
    { MgConfigProperties::ResourceServicePropertyRetryAttempts                      , MgPropertyType::Int32     , 0                                     , 1000                                  , L""                                       },
    { MgConfigProperties::ResourceServicePropertyRetryInterval                      , MgPropertyType::Int32     , 0                                     , 60000                                 , L""                                       },
    { L""                                                                           , 0                         , 0.0                                   , 0.0                                   , L""                                       }
};

const MgConfigValidationInfo MgConfigProperties::sm_cviSiteServiceProperties[] =
{
    { MgConfigProperties::SiteServicePropertySessionTimeout                         , MgPropertyType::Int32     , MG_CONFIG_MIN_TIMEOUT                 , MG_CONFIG_MAX_TIMEOUT                 , L""                                       },
    { MgConfigProperties::SiteServicePropertySessionTimerInterval                   , MgPropertyType::Int32     , MG_CONFIG_MIN_TIMER_INTERVAL          , MG_CONFIG_MAX_TIMER_INTERVAL          , L""                                       },
    { L""                                                                           , 0                         , 0.0                                   , 0.0                                   , L""                                       }
};

const MgConfigValidationInfo MgConfigProperties::sm_cviTileServiceProperties[] =
{
    { MgConfigProperties::TileServicePropertyRenderOnly                             , MgPropertyType::Boolean   , 0                                     , 1                                     , L""                                       },
    { MgConfigProperties::TileServicePropertyTileCachePath                          , MgPropertyType::String    , MG_CONFIG_MIN_PATH_LENGTH             , MG_CONFIG_MAX_PATH_LENGTH             , MG_CONFIG_PATH_RESERVED_CHARACTERS        },
    { MgConfigProperties::TileServicePropertyTileColumnsPerFolder                   , MgPropertyType::Int32     , 1                                     , 1000                                  , L""                                       },
    { MgConfigProperties::TileServicePropertyTileRowsPerFolder                      , MgPropertyType::Int32     , 1                                     , 1000                                  , L""                                       },
    { MgConfigProperties::TileServicePropertyCreationCutoffTime                     , MgPropertyType::Int32     , 1                                     , 600                                   , L""                                       },
    { MgConfigProperties::TileServicePropertyPollingInterval                        , MgPropertyType::Int32     , 1                                     , 60                                    , L""                                       },
    { MgConfigProperties::TileServicePropertyTiledMapCacheSize                      , MgPropertyType::Int32     , MG_CONFIG_MIN_CACHE_SIZE              , MG_CONFIG_MAX_CACHE_SIZE              , L""                                       },
    { MgConfigProperties::TileServicePropertyTileSizeX                              , MgPropertyType::Int32     , 50                                    , 10000                                 , L""                                       },
    { MgConfigProperties::TileServicePropertyTileSizeY                              , MgPropertyType::Int32     , 50                                    , 10000                                 , L""                                       },
    { MgConfigProperties::TileServicePropertyImageFormat                            , MgPropertyType::String    , 2                                     , 4                                     , L""                                       },
    { L""                                                                           , 0                         , 0.0                                   , 0.0                                   , L""                                       }
};

const MgConfigValidationInfo MgConfigProperties::sm_cviAccessLogProperties[] =
{
    { MgConfigProperties::AccessLogPropertyEnabled                                  , MgPropertyType::Boolean   , 0                                     , 1                                     , L""                                       },
    { MgConfigProperties::AccessLogPropertyFilename                                 , MgPropertyType::String    , MG_CONFIG_MIN_FILE_NAME_LENGTH        , MG_CONFIG_MAX_FILE_NAME_LENGTH        , MG_CONFIG_FILE_NAME_RESERVED_CHARACTERS   },
    { MgConfigProperties::AccessLogPropertyParameters                               , MgPropertyType::String    , MG_CONFIG_MIN_LOG_PARAMETERS_LENGTH   , MG_CONFIG_MAX_LOG_PARAMETERS_LENGTH   , L""                                       },
    { L""                                                                           , 0                         , 0.0                                   , 0.0                                   , L""                                       }
};

const MgConfigValidationInfo MgConfigProperties::sm_cviAdminLogProperties[] =
{
    { MgConfigProperties::AdminLogPropertyEnabled                                   , MgPropertyType::Boolean   , 0                                     , 1                                     , L""                                       },
    { MgConfigProperties::AdminLogPropertyFilename                                  , MgPropertyType::String    , MG_CONFIG_MIN_FILE_NAME_LENGTH        , MG_CONFIG_MAX_FILE_NAME_LENGTH        , MG_CONFIG_FILE_NAME_RESERVED_CHARACTERS   },
    { MgConfigProperties::AdminLogPropertyParameters                                , MgPropertyType::String    , MG_CONFIG_MIN_LOG_PARAMETERS_LENGTH   , MG_CONFIG_MAX_LOG_PARAMETERS_LENGTH   , L""                                       },
    { L""                                                                           , 0                         , 0.0                                   , 0.0                                   , L""                                       }
};

const MgConfigValidationInfo MgConfigProperties::sm_cviAuthenticationLogProperties[] =
{
    { MgConfigProperties::AuthenticationLogPropertyEnabled                          , MgPropertyType::Boolean   , 0                                     , 1                                     , L""                                       },
    { MgConfigProperties::AuthenticationLogPropertyFilename                         , MgPropertyType::String    , MG_CONFIG_MIN_FILE_NAME_LENGTH        , MG_CONFIG_MAX_FILE_NAME_LENGTH        , MG_CONFIG_FILE_NAME_RESERVED_CHARACTERS   },
    { MgConfigProperties::AuthenticationLogPropertyParameters                       , MgPropertyType::String    , MG_CONFIG_MIN_LOG_PARAMETERS_LENGTH   , MG_CONFIG_MAX_LOG_PARAMETERS_LENGTH   , L""                                       },
    { L""                                                                           , 0                         , 0.0                                   , 0.0                                   , L""                                       }
};

const MgConfigValidationInfo MgConfigProperties::sm_cviErrorLogProperties[] =
{
    { MgConfigProperties::ErrorLogPropertyEnabled                                   , MgPropertyType::Boolean   , 0                                     , 1                                     , L""                                       },
    { MgConfigProperties::ErrorLogPropertyFilename                                  , MgPropertyType::String    , MG_CONFIG_MIN_FILE_NAME_LENGTH        , MG_CONFIG_MAX_FILE_NAME_LENGTH        , MG_CONFIG_FILE_NAME_RESERVED_CHARACTERS   },
    { MgConfigProperties::ErrorLogPropertyParameters                                , MgPropertyType::String    , MG_CONFIG_MIN_LOG_PARAMETERS_LENGTH   , MG_CONFIG_MAX_LOG_PARAMETERS_LENGTH   , L""                                       },
    { L""                                                                           , 0                         , 0.0                                   , 0.0                                   , L""                                       }
};

const MgConfigValidationInfo MgConfigProperties::sm_cviSessionLogProperties[] =
{
    { MgConfigProperties::SessionLogPropertyEnabled                                 , MgPropertyType::Boolean   , 0                                     , 1                                     , L""                                       },
    { MgConfigProperties::SessionLogPropertyFilename                                , MgPropertyType::String    , MG_CONFIG_MIN_FILE_NAME_LENGTH        , MG_CONFIG_MAX_FILE_NAME_LENGTH        , MG_CONFIG_FILE_NAME_RESERVED_CHARACTERS   },
    { MgConfigProperties::SessionLogPropertyParameters                              , MgPropertyType::String    , MG_CONFIG_MIN_LOG_PARAMETERS_LENGTH   , MG_CONFIG_MAX_LOG_PARAMETERS_LENGTH   , L""                                       },
    { L""                                                                           , 0                         , 0.0                                   , 0.0                                   , L""                                       }
};

const MgConfigValidationInfo MgConfigProperties::sm_cviTraceLogProperties[] =
{
    { MgConfigProperties::TraceLogPropertyEnabled                                   , MgPropertyType::Boolean   , 0                                     , 1                                     , L""                                       },
    { MgConfigProperties::TraceLogPropertyFilename                                  , MgPropertyType::String    , MG_CONFIG_MIN_FILE_NAME_LENGTH        , MG_CONFIG_MAX_FILE_NAME_LENGTH        , MG_CONFIG_FILE_NAME_RESERVED_CHARACTERS   },
    { MgConfigProperties::TraceLogPropertyParameters                                , MgPropertyType::String    , MG_CONFIG_MIN_LOG_PARAMETERS_LENGTH   , MG_CONFIG_MAX_LOG_PARAMETERS_LENGTH   , L""                                       },
    { L""                                                                           , 0                         , 0.0                                   , 0.0                                   , L""                                       }
};

///////////////////////////////////////////////////////////////////////////////
/// Web Configuration Validation Information Maps
///
const MgConfigValidationInfo MgConfigProperties::sm_cviAgentProperties[] =
{
    { MgConfigProperties::AgentDebugPause                                           , MgPropertyType::Int32     , 0                                     , 86400                                 , L""                                       },
    { MgConfigProperties::AgentDisableAuthoring                                     , MgPropertyType::Boolean   , 0                                     , 1                                     , L""                                       },
    { MgConfigProperties::AgentDisableWfs                                           , MgPropertyType::Boolean   , 0                                     , 1                                     , L""                                       },
    { MgConfigProperties::AgentDisableWms                                           , MgPropertyType::Boolean   , 0                                     , 1                                     , L""                                       },
    { L""                                                                           , 0                         , 0.0                                   , 0.0                                   , L""                                       }
};

const MgConfigValidationInfo MgConfigProperties::sm_cviOgcProperties[] =
{
    { MgConfigProperties::WfsPassword                                               , MgPropertyType::String    , MG_CONFIG_MIN_PASSWORD_LENGTH         , MG_CONFIG_MAX_PASSWORD_LENGTH         , MG_CONFIG_PASSWORD_RESERVED_CHARACTERS    },
    { MgConfigProperties::WmsPassword                                               , MgPropertyType::String    , MG_CONFIG_MIN_PASSWORD_LENGTH         , MG_CONFIG_MAX_PASSWORD_LENGTH         , MG_CONFIG_PASSWORD_RESERVED_CHARACTERS    },
    { L""                                                                           , 0                         , 0.0                                   , 0.0                                   , L""                                       }
};

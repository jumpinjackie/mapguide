#include "ConfigProperties.h"

//From UserInformation.h in MapGuideCommon

/// Locale codes are 2 letter language codes defined by ISO 639-1. Please refer
/// to http://www.loc.gov/standards/iso639-2/php/English_list.php
#define MG_LOCALE_LENGTH    2
#define MG_EXTENDED_LOCALE_LENGTH    5

#define MG_API_VERSION(major,minor,phase) ((major<<16) + (minor<<8) + phase)


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

#define MG_CONFIG_MIN_TRANSACTION_TIMEOUT               1
#define MG_CONFIG_MAX_TRANSACTION_TIMEOUT               1800 // 30 minutes

#define MG_CONFIG_MIN_TRANSACTION_TIMER_INTERVAL        1
#define MG_CONFIG_MAX_TRANSACTION_TIMER_INTERVAL        1800 // 30 minutes

#define MG_CONFIG_MIN_JOIN_QUERY_BATCH_SIZE             1
#define MG_CONFIG_MAX_JOIN_QUERY_BATCH_SIZE             10000

#define MG_CONFIG_MIN_RENDER_SELECTION_SIZE             1000
#define MG_CONFIG_MAX_RENDER_SELECTION_SIZE             100000

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
const STRING MgdConfigProperties::DefaultConfigurationFilename                               = L"webconfig.ini";
const STRING MgdConfigProperties::ServerConfigurationFilename                                = L"serverconfig.ini";

// Configuration file section/property names and default values

// ******************************************************************
// General Properties
// ******************************************************************
const STRING MgdConfigProperties::GeneralPropertiesSection                                   = L"GeneralProperties";
const STRING MgdConfigProperties::GeneralPropertyLocale                                      = L"Locale";
const STRING MgdConfigProperties::DefaultGeneralPropertyLocale                               = L"";
const STRING MgdConfigProperties::GeneralPropertyDefaultMessageLocale                        = L"DefaultMessageLocale";
const STRING MgdConfigProperties::DefaultGeneralPropertyDefaultMessageLocale                 = L"en";      // ISO 639-1 name
const STRING MgdConfigProperties::GeneralPropertyFdoPath                                     = L"FdoPath";
const STRING MgdConfigProperties::DefaultGeneralPropertyFdoPath                              = L"FDO/";
const STRING MgdConfigProperties::GeneralPropertyMentorDictionaryPath                        = L"MentorDictionaryPath";
const STRING MgdConfigProperties::DefaultGeneralPropertyMentorDictionaryPath                 = L"";
const STRING MgdConfigProperties::GeneralPropertyLinuxMemDebug                               = L"LinuxMemDebug";
const bool   MgdConfigProperties::DefaultGeneralPropertyLinuxMemDebug                        = false;
const STRING MgdConfigProperties::GeneralPropertyResourcesPath                               = L"ResourcesPath";
const STRING MgdConfigProperties::DefaultGeneralPropertyResourcesPath                        = L"Resources/";
const STRING MgdConfigProperties::GeneralPropertyTempPath                                    = L"TempPath";
const STRING MgdConfigProperties::DefaultGeneralPropertyTempPath                             = L"Temp/";
const STRING MgdConfigProperties::GeneralPropertyWfsDocumentPath                             = L"WfsDocumentPath";
const STRING MgdConfigProperties::DefaultGeneralPropertyWfsDocumentPath                      = L"Wfs/";
const STRING MgdConfigProperties::GeneralPropertyWmsDocumentPath                             = L"WmsDocumentPath";
const STRING MgdConfigProperties::DefaultGeneralPropertyWmsDocumentPath                      = L"Wms/";
const STRING MgdConfigProperties::GeneralPropertyRenderer                                    = L"Renderer";
const STRING MgdConfigProperties::DefaultGeneralPropertyRenderer                             = L"GD";
const STRING MgdConfigProperties::GeneralPropertyPreCacheMaps                                = L"PreCacheMaps";
const STRING MgdConfigProperties::DefaultGeneralPropertyPreCacheMaps                         = L"";
const STRING MgdConfigProperties::GeneralPropertyLogsDelimiter                               = L"LogsDelimiter";
const STRING MgdConfigProperties::DefaultGeneralPropertyLogsDelimiter                        = L"\t";
const STRING MgdConfigProperties::GeneralPropertyLogsDetail                                  = L"LogsDetail";
const STRING MgdConfigProperties::DefaultGeneralPropertyLogsDetail                           = L"";
const STRING MgdConfigProperties::GeneralPropertyLogsPath                                    = L"LogsPath";
const STRING MgdConfigProperties::DefaultGeneralPropertyLogsPath                             = L"Logs/";
const STRING MgdConfigProperties::GeneralPropertyMaxLogFileSize                              = L"MaxLogFileSize";
const INT32  MgdConfigProperties::DefaultGeneralPropertyMaxLogFileSize                       = 1024;
const STRING MgdConfigProperties::GeneralPropertyMaxLogFileSizeEnabled                       = L"MaxLogFileSizeEnabled";
const bool   MgdConfigProperties::DefaultGeneralPropertyMaxLogFileSizeEnabled                = false;

// ******************************************************************
// Drawing Service Properties
// ******************************************************************
const STRING MgdConfigProperties::DrawingServicePropertiesSection                            = L"DrawingServiceProperties";

// ******************************************************************
// Feature Service Properties
// ******************************************************************
const STRING MgdConfigProperties::FeatureServicePropertiesSection                            = L"FeatureServiceProperties";
const STRING MgdConfigProperties::FeatureServicePropertyCacheSize                            = L"CacheSize";
const INT32  MgdConfigProperties::DefaultFeatureServicePropertyCacheSize                     = 100;
const STRING MgdConfigProperties::FeatureServicePropertyCacheTimeLimit                       = L"CacheTimeLimit";
const INT32  MgdConfigProperties::DefaultFeatureServicePropertyCacheTimeLimit                = 86400;
const STRING MgdConfigProperties::FeatureServicePropertyCacheTimerInterval                   = L"CacheTimerInterval";
const INT32  MgdConfigProperties::DefaultFeatureServicePropertyCacheTimerInterval            = 3600;
const STRING MgdConfigProperties::FeatureServicePropertyDataCacheSize                        = L"DataCacheSize";
const INT32  MgdConfigProperties::DefaultFeatureServicePropertyDataCacheSize                 = 100;
const STRING MgdConfigProperties::FeatureServicePropertyDataConnectionPoolEnabled            = L"DataConnectionPoolEnabled";
const bool   MgdConfigProperties::DefaultFeatureServicePropertyDataConnectionPoolEnabled     = false;
const STRING MgdConfigProperties::FeatureServicePropertyDataConnectionPoolExcludedProviders  = L"DataConnectionPoolExcludedProviders";
const STRING MgdConfigProperties::DefaultFeatureServicePropertyDataConnectionPoolExcludedProviders = L""; // This means all providers are cached
const STRING MgdConfigProperties::FeatureServicePropertyDataConnectionPoolSize               = L"DataConnectionPoolSize";
const INT32  MgdConfigProperties::DefaultFeatureServicePropertyDataConnectionPoolSize        = 50;
const STRING MgdConfigProperties::FeatureServicePropertyDataConnectionPoolSizeCustom         = L"DataConnectionPoolSizeCustom";
const STRING MgdConfigProperties::DefaultFeatureServicePropertyDataConnectionPoolSizeCustom  = L"";
const STRING MgdConfigProperties::FeatureServicePropertyDataConnectionTimeout                = L"DataConnectionTimeout";
const INT32  MgdConfigProperties::DefaultFeatureServicePropertyDataConnectionTimeout         = 600;
const STRING MgdConfigProperties::FeatureServicePropertyDataConnectionTimerInterval          = L"DataConnectionTimerInterval";
const INT32  MgdConfigProperties::DefaultFeatureServicePropertyDataConnectionTimerInterval   = 60;
const STRING MgdConfigProperties::FeatureServicePropertyJoinQueryBatchSize                   = L"JoinQueryBatchSize";
const INT32  MgdConfigProperties::DefaultFeatureServicePropertyJoinQueryBatchSize            = 100;
const STRING MgdConfigProperties::FeatureServicePropertyDataConnectionUseLimit               = L"DataConnectionUseLimit";
const STRING MgdConfigProperties::DefaultFeatureServicePropertyDataConnectionUseLimit        = L"";
const STRING MgdConfigProperties::FeatureServicePropertyDataTransactionTimeout               = L"DataTransactionTimeout";
const INT32  MgdConfigProperties::DefaultFeatureServicePropertyDataTransactionTimeout        = 360;
const STRING MgdConfigProperties::FeatureServicePropertyDataTransactionTimerInterval         = L"DataTransactionTimerInterval";
const INT32  MgdConfigProperties::DefaultFeatureServicePropertyDataTransactionTimerInterval  = 60;
const STRING MgdConfigProperties::FeatureServicePropertyFDOConnectionTimeoutCustom           = L"FDOConnectionTimeoutCustom";
const STRING MgdConfigProperties::DefaultFeatureServicePropertyFDOConnectionTimeoutCustom    = L"OSGeo.WMS:120";
const STRING MgdConfigProperties::FeatureServicePropertyUseFdoJoinOptimization               = L"UseFdoJoinOptimization";
const bool   MgdConfigProperties::DefaultFeatureServicePropertyUseFdoJoinOptimization        = true;

// ******************************************************************
// Mapping Service Properties
// ******************************************************************
const STRING MgdConfigProperties::MappingServicePropertiesSection                            = L"MappingServiceProperties";
const STRING MgdConfigProperties::MappingServicePropertyLegendFont                           = L"LegendFont";
const STRING MgdConfigProperties::DefaultMappingServicePropertyLegendFont                    = L"Arial";

// ******************************************************************
// Rendering Service Properties
// ******************************************************************
const STRING MgdConfigProperties::RenderingServicePropertiesSection                          = L"RenderingServiceProperties";
const STRING MgdConfigProperties::RenderingServicePropertyTileExtentOffset                   = L"TileExtentOffset";
const double MgdConfigProperties::DefaultRenderingServicePropertyTileExtentOffset            = 0.35;
const STRING MgdConfigProperties::RenderingServicePropertyRasterGridSize                     = L"RasterGridSize";
const INT32  MgdConfigProperties::DefaultRenderingServicePropertyRasterGridSize              = 100;
const STRING MgdConfigProperties::RenderingServicePropertyMinRasterGridSize                  = L"MinRasterGridSize";
const INT32  MgdConfigProperties::DefaultRenderingServicePropertyMinRasterGridSize           = 10;
const STRING MgdConfigProperties::RenderingServicePropertyRasterGridSizeOverrideRatio        = L"RasterGridSizeOverrideRatio";
const double MgdConfigProperties::DefaultRenderingServicePropertyRasterGridSizeOverrideRatio = 0.25;
const STRING MgdConfigProperties::RenderingServicePropertyRasterGridSizeForPlot              = L"RasterGridSizeForPlot";
const INT32  MgdConfigProperties::DefaultRenderingServicePropertyRasterGridSizeForPlot       = 50;
const STRING MgdConfigProperties::RenderingServicePropertyMinRasterGridSizeForPlot           = L"MinRasterGridSizeForPlot";
const INT32  MgdConfigProperties::DefaultRenderingServicePropertyMinRasterGridSizeForPlot    = 5;
const STRING MgdConfigProperties::RenderingServicePropertyRasterGridSizeOverrideRatioForPlot = L"RasterGridSizeOverrideRatioForPlot";
const double MgdConfigProperties::DefaultRenderingServicePropertyRasterGridSizeOverrideRatioForPlot = 0.10;
const STRING MgdConfigProperties::RenderingServicePropertyRenderSelectionBatchSize           = L"RenderSelectionBatchSize";
const INT32  MgdConfigProperties::DefaultRenderingServicePropertyRenderSelectionBatchSize    = 50000;
const STRING MgdConfigProperties::RenderingServicePropertyClampPoints                        = L"ClampPoints";
const bool   MgdConfigProperties::DefaultRenderingServicePropertyClampPoints                 = false;
const STRING MgdConfigProperties::RenderingServicePropertyGeneralizeData                     = L"GeneralizeData";
const bool   MgdConfigProperties::DefaultRenderingServicePropertyGeneralizeData              = false;
const STRING MgdConfigProperties::RenderingServicePropertyMaxRasterImageWidth                = L"MaxRasterImageWidth";
const INT32  MgdConfigProperties::DefaultRenderingServicePropertyMaxRasterImageWidth         = 2048;
const STRING MgdConfigProperties::RenderingServicePropertyMaxRasterImageHeight               = L"MaxRasterImageHeight";
const INT32  MgdConfigProperties::DefaultRenderingServicePropertyMaxRasterImageHeight        = 2048;

// ******************************************************************
// Font Alias Mappings section
// ******************************************************************
const STRING MgdConfigProperties::FontAliasMappingSection                                    = L"FontAliases";

// ******************************************************************
// Unmanaged Data Mappings section
// ******************************************************************
const STRING MgdConfigProperties::UnmanagedDataMappingsSection                               = L"UnmanagedDataMappings";

// ******************************************************************
// Resource Service Properties
// ******************************************************************
const STRING MgdConfigProperties::ResourceServicePropertiesSection                               = L"ResourceServiceProperties";
const STRING MgdConfigProperties::ResourceServicePropertyLibraryRepositoryPath                   = L"LibraryRepositoryPath";
const STRING MgdConfigProperties::DefaultResourceServicePropertyLibraryRepositoryPath            = L"Repositories/Library/Content/";
const STRING MgdConfigProperties::ResourceServicePropertyLibraryResourceDataFilePath             = L"LibraryResourceDataFilePath";
const STRING MgdConfigProperties::DefaultResourceServicePropertyLibraryResourceDataFilePath      = L"Repositories/Library/DataFiles/";
const STRING MgdConfigProperties::ResourceServicePropertyPackagesPath                            = L"PackagesPath";
const STRING MgdConfigProperties::DefaultResourceServicePropertyPackagesPath                     = L"Packages/";
const STRING MgdConfigProperties::ResourceServicePropertyResourceDataFileTrashFolderName         = L"ResourceDataFileTrashFolderName";
const STRING MgdConfigProperties::DefaultResourceServicePropertyResourceDataFileTrashFolderName  = L"Trash";
const STRING MgdConfigProperties::ResourceServicePropertyResourceSchemaFilePath                  = L"ResourceSchemaFilePath";
const STRING MgdConfigProperties::DefaultResourceServicePropertyResourceSchemaFilePath           = L"Schema/";
const STRING MgdConfigProperties::ResourceServicePropertySessionRepositoryPath                   = L"SessionRepositoryPath";
const STRING MgdConfigProperties::DefaultResourceServicePropertySessionRepositoryPath            = L"Repositories/Session/Content/";
const STRING MgdConfigProperties::ResourceServicePropertySessionResourceDataFilePath             = L"SessionResourceDataFilePath";
const STRING MgdConfigProperties::DefaultResourceServicePropertySessionResourceDataFilePath      = L"Repositories/Session/DataFiles/";
const STRING MgdConfigProperties::ResourceServicePropertySiteRepositoryPath                      = L"SiteRepositoryPath";
const STRING MgdConfigProperties::DefaultResourceServicePropertySiteRepositoryPath               = L"Repositories/Site/";
const STRING MgdConfigProperties::ResourceServicePropertyResourceValidationEnabled               = L"ResourceValidationEnabled"; // for internal use only
const bool   MgdConfigProperties::DefaultResourceServicePropertyResourceValidationEnabled        = true;
const STRING MgdConfigProperties::ResourceServicePropertyRetryAttempts                           = L"RetryAttempts";             // for internal use only
const INT32  MgdConfigProperties::DefaultResourceServicePropertyRetryAttempts                    = 50;
const STRING MgdConfigProperties::ResourceServicePropertyRetryInterval                           = L"RetryInterval";             // for internal use only
const INT32  MgdConfigProperties::DefaultResourceServicePropertyRetryInterval                    = 25;
const STRING MgdConfigProperties::ResourceServicePropertySessionRepositoriesConfig               = L"SessionRepositoriesConfig";
const STRING MgdConfigProperties::DefaultResourceServicePropertySessionRepositoriesConfig        = L"FilePerSession";
const STRING MgdConfigProperties::ResourceServicePropertySessionRepositoriesLimit                = L"SessionRepositoriesLimit";
const INT32  MgdConfigProperties::DefaultResourceServicePropertySessionRepositoriesLimit         = 200;

// ******************************************************************
// Site Service Properties
// ******************************************************************
const STRING MgdConfigProperties::SiteServicePropertiesSection                               = L"SiteServiceProperties";
const STRING MgdConfigProperties::SiteServicePropertySessionTimeout                          = L"SessionTimeout";
const INT32  MgdConfigProperties::DefaultSiteServicePropertySessionTimeout                   = 1200;
const STRING MgdConfigProperties::SiteServicePropertySessionTimerInterval                    = L"SessionTimerInterval";
const INT32  MgdConfigProperties::DefaultSiteServicePropertySessionTimerInterval             = 400;

// ******************************************************************
// Tile Service Properties
// ******************************************************************
const STRING MgdConfigProperties::TileServicePropertiesSection                               = L"TileServiceProperties";
const STRING MgdConfigProperties::TileServicePropertyRenderOnly                              = L"RenderOnly";
const bool   MgdConfigProperties::DefaultTileServicePropertyRenderOnly                       = false;
const STRING MgdConfigProperties::TileServicePropertyTileCachePath                           = L"TileCachePath";
const STRING MgdConfigProperties::DefaultTileServicePropertyTileCachePath                    = L"Repositories/TileCache/";
const STRING MgdConfigProperties::TileServicePropertyTileColumnsPerFolder                    = L"TileColumnsPerFolder";
const INT32 MgdConfigProperties::DefaultTileServicePropertyTileColumnsPerFolder              = 30;
const STRING MgdConfigProperties::TileServicePropertyTileRowsPerFolder                       = L"TileRowsPerFolder";
const INT32 MgdConfigProperties::DefaultTileServicePropertyTileRowsPerFolder                 = 30;
const STRING MgdConfigProperties::TileServicePropertyCreationCutoffTime                      = L"CreationCutoffTime";// for internal use only
const INT32  MgdConfigProperties::DefaultTileServicePropertyCreationCutoffTime               = 120;
const STRING MgdConfigProperties::TileServicePropertyPollingInterval                         = L"PollingInterval";   // for internal use only
const INT32  MgdConfigProperties::DefaultTileServicePropertyPollingInterval                  = 1;
const STRING MgdConfigProperties::TileServicePropertyTiledMapCacheSize                       = L"TiledMapCacheSize"; // for internal use only
const INT32  MgdConfigProperties::DefaultTileServicePropertyTiledMapCacheSize                = 10;
const STRING MgdConfigProperties::TileServicePropertyTileSizeX                               = L"DefaultTileSizeX";
const INT32 MgdConfigProperties::DefaultTileServicePropertyTileSizeX                         = 300;
const STRING MgdConfigProperties::TileServicePropertyTileSizeY                               = L"DefaultTileSizeY";
const INT32 MgdConfigProperties::DefaultTileServicePropertyTileSizeY                         = 300;
const STRING MgdConfigProperties::TileServicePropertyImageFormat                             = L"ImageFormat";
const STRING MgdConfigProperties::DefaultTileServicePropertyImageFormat                      = L"PNG";

// ******************************************************************
// Access Log Properties
// ******************************************************************
const STRING MgdConfigProperties::AccessLogPropertiesSection                                 = L"AccessLogProperties";
const STRING MgdConfigProperties::AccessLogPropertyEnabled                                   = L"Enabled";
const bool   MgdConfigProperties::DefaultAccessLogPropertyEnabled                            = true;
const STRING MgdConfigProperties::AccessLogPropertyFilename                                  = L"Filename";
const STRING MgdConfigProperties::DefaultAccessLogPropertyFilename                           = L"Access.log";
const STRING MgdConfigProperties::AccessLogPropertyParameters                                = L"Parameters";
const STRING MgdConfigProperties::DefaultAccessLogPropertyParameters                         = L"";

// ******************************************************************
// Admin Log Properties
// ******************************************************************
const STRING MgdConfigProperties::AdminLogPropertiesSection                                  = L"AdminLogProperties";
const STRING MgdConfigProperties::AdminLogPropertyEnabled                                    = L"Enabled";
const bool   MgdConfigProperties::DefaultAdminLogPropertyEnabled                             = true;
const STRING MgdConfigProperties::AdminLogPropertyFilename                                   = L"Filename";
const STRING MgdConfigProperties::DefaultAdminLogPropertyFilename                            = L"Admin.log";
const STRING MgdConfigProperties::AdminLogPropertyParameters                                 = L"Parameters";
const STRING MgdConfigProperties::DefaultAdminLogPropertyParameters                          = L"";

// ******************************************************************
// Authentication Log Properties
// ******************************************************************
const STRING MgdConfigProperties::AuthenticationLogPropertiesSection                         = L"AuthenticationLogProperties";
const STRING MgdConfigProperties::AuthenticationLogPropertyEnabled                           = L"Enabled";
const bool   MgdConfigProperties::DefaultAuthenticationLogPropertyEnabled                    = true;
const STRING MgdConfigProperties::AuthenticationLogPropertyFilename                          = L"Filename";
const STRING MgdConfigProperties::DefaultAuthenticationLogPropertyFilename                   = L"Authentication.log";
const STRING MgdConfigProperties::AuthenticationLogPropertyParameters                        = L"Parameters";
const STRING MgdConfigProperties::DefaultAuthenticationLogPropertyParameters                 = L"";

// ******************************************************************
// Error Log Properties
// ******************************************************************
const STRING MgdConfigProperties::ErrorLogPropertiesSection                                  = L"ErrorLogProperties";
const STRING MgdConfigProperties::ErrorLogPropertyEnabled                                    = L"Enabled";
const bool   MgdConfigProperties::DefaultErrorLogPropertyEnabled                             = true;
const STRING MgdConfigProperties::ErrorLogPropertyFilename                                   = L"Filename";
const STRING MgdConfigProperties::DefaultErrorLogPropertyFilename                            = L"Error.log";
const STRING MgdConfigProperties::ErrorLogPropertyParameters                                 = L"Parameters";
const STRING MgdConfigProperties::DefaultErrorLogPropertyParameters                          = L"";

// ******************************************************************
// Performance Log Properties
// ******************************************************************
const STRING MgdConfigProperties::PerformanceLogPropertiesSection                            = L"PerformanceLogProperties";
const STRING MgdConfigProperties::PerformanceLogPropertyEnabled                              = L"Enabled";
const bool   MgdConfigProperties::DefaultPerformanceLogPropertyEnabled                       = false;
const STRING MgdConfigProperties::PerformanceLogPropertyFilename                             = L"Filename";
const STRING MgdConfigProperties::DefaultPerformanceLogPropertyFilename                      = L"Performance.log";
const STRING MgdConfigProperties::PerformanceLogPropertyParameters                           = L"Parameters";
const STRING MgdConfigProperties::DefaultPerformanceLogPropertyParameters                    = L"";
const STRING MgdConfigProperties::PerformanceLogPropertyInterval                             = L"Interval";
const INT32  MgdConfigProperties::DefaultPerformanceLogPropertyInterval                      = 300;

// ******************************************************************
// Session Log Properties
// ******************************************************************
const STRING MgdConfigProperties::SessionLogPropertiesSection                                = L"SessionLogProperties";
const STRING MgdConfigProperties::SessionLogPropertyEnabled                                  = L"Enabled";
const bool   MgdConfigProperties::DefaultSessionLogPropertyEnabled                           = false;
const STRING MgdConfigProperties::SessionLogPropertyFilename                                 = L"Filename";
const STRING MgdConfigProperties::DefaultSessionLogPropertyFilename                          = L"Session.log";
const STRING MgdConfigProperties::SessionLogPropertyParameters                               = L"Parameters";
const STRING MgdConfigProperties::DefaultSessionLogPropertyParameters                        = L"";

// ******************************************************************
// Trace Log Properties
// ******************************************************************
const STRING MgdConfigProperties::TraceLogPropertiesSection                                  = L"TraceLogProperties";
const STRING MgdConfigProperties::TraceLogPropertyEnabled                                    = L"Enabled";
const bool   MgdConfigProperties::DefaultTraceLogPropertyEnabled                             = false;
const STRING MgdConfigProperties::TraceLogPropertyFilename                                   = L"Filename";
const STRING MgdConfigProperties::DefaultTraceLogPropertyFilename                            = L"Trace.log";
const STRING MgdConfigProperties::TraceLogPropertyParameters                                 = L"Parameters";
const STRING MgdConfigProperties::DefaultTraceLogPropertyParameters                          = L"";

///////////////////////////////////////////////////////////////////////////////
/// Common Configuration Validation Information Maps
///
const MgConfigValidationInfo MgdConfigProperties::sm_cviGeneralProperties[] =
{
    { MgdConfigProperties::GeneralPropertyLocale                                     , MgPropertyType::String    , MG_CONFIG_MIN_OPTIONAL_STRING_LENGTH  , MG_CONFIG_MAX_OPTIONAL_STRING_LENGTH  , L""                                       },
    { MgdConfigProperties::GeneralPropertyDefaultMessageLocale                       , MgPropertyType::String    , MG_CONFIG_MIN_LOCALE_LENGTH           , MG_CONFIG_MAX_LOCALE_LENGTH           , L""                                       },
    { MgdConfigProperties::GeneralPropertyFdoPath                                    , MgPropertyType::String    , MG_CONFIG_MIN_PATH_LENGTH             , MG_CONFIG_MAX_PATH_LENGTH             , MG_CONFIG_PATH_RESERVED_CHARACTERS        },
    { MgdConfigProperties::GeneralPropertyLinuxMemDebug                              , MgPropertyType::Boolean   , 0                                     , 1                                     , L""                                       },
    { MgdConfigProperties::GeneralPropertyResourcesPath                              , MgPropertyType::String    , MG_CONFIG_MIN_PATH_LENGTH             , MG_CONFIG_MAX_PATH_LENGTH             , MG_CONFIG_PATH_RESERVED_CHARACTERS        },
    { MgdConfigProperties::GeneralPropertyTempPath                                   , MgPropertyType::String    , MG_CONFIG_MIN_PATH_LENGTH             , MG_CONFIG_MAX_PATH_LENGTH             , MG_CONFIG_PATH_RESERVED_CHARACTERS        },
    { MgdConfigProperties::GeneralPropertyWfsDocumentPath                            , MgPropertyType::String    , MG_CONFIG_MIN_PATH_LENGTH             , MG_CONFIG_MAX_PATH_LENGTH             , MG_CONFIG_PATH_RESERVED_CHARACTERS        },
    { MgdConfigProperties::GeneralPropertyWmsDocumentPath                            , MgPropertyType::String    , MG_CONFIG_MIN_PATH_LENGTH             , MG_CONFIG_MAX_PATH_LENGTH             , MG_CONFIG_PATH_RESERVED_CHARACTERS        },
    { L""                                                                           , 0                         , 0.0                                   , 0.0                                   , L""                                       }
};

///////////////////////////////////////////////////////////////////////////////
/// Server Configuration Validation Information Maps
///

const MgConfigValidationInfo MgdConfigProperties::sm_cviDrawingServiceProperties[] =
{
    { L""                                                                           , 0                         , 0.0                                   , 0.0                                  , L""                                        }
};

const MgConfigValidationInfo MgdConfigProperties::sm_cviFeatureServiceProperties[] =
{
    { MgdConfigProperties::FeatureServicePropertyCacheSize                           , MgPropertyType::Int32     , MG_CONFIG_MIN_FS_CACHE_SIZE           , MG_CONFIG_MAX_FS_CACHE_SIZE           , L""                                       },
    { MgdConfigProperties::FeatureServicePropertyCacheTimeLimit                      , MgPropertyType::Int32     , MG_CONFIG_MIN_FS_CACHE_TIMELIMIT      , MG_CONFIG_MAX_FS_CACHE_TIMELIMIT      , L""                                       },
    { MgdConfigProperties::FeatureServicePropertyCacheTimerInterval                  , MgPropertyType::Int32     , MG_CONFIG_MIN_FS_CACHE_TIMERINTERVAL  , MG_CONFIG_MAX_FS_CACHE_TIMERINTERVAL  , L""                                       },
    { MgdConfigProperties::FeatureServicePropertyDataCacheSize                       , MgPropertyType::Int32     , MG_CONFIG_MIN_CACHE_SIZE              , MG_CONFIG_MAX_CACHE_SIZE              , L""                                       },
    { MgdConfigProperties::FeatureServicePropertyDataConnectionPoolEnabled           , MgPropertyType::Boolean   , 0                                     , 1                                     , L""                                       },
    { MgdConfigProperties::FeatureServicePropertyDataConnectionPoolExcludedProviders , MgPropertyType::String    , MG_CONFIG_MIN_FS_CP_EXCLUDED_LENGTH   , MG_CONFIG_MAX_FS_CP_EXCLUDED_LENGTH   , L""                                       },
    { MgdConfigProperties::FeatureServicePropertyDataConnectionPoolSize              , MgPropertyType::Int32     , MG_CONFIG_MIN_CONNECTION_POOL_SIZE    , MG_CONFIG_MAX_CONNECTION_POOL_SIZE    , L""                                       },
    { MgdConfigProperties::FeatureServicePropertyDataConnectionPoolSizeCustom        , MgPropertyType::String    , MG_CONFIG_MIN_FS_CP_EXCLUDED_LENGTH   , MG_CONFIG_MAX_FS_CP_EXCLUDED_LENGTH   , L""                                       },
    { MgdConfigProperties::FeatureServicePropertyDataConnectionTimeout               , MgPropertyType::Int32     , MG_CONFIG_MIN_TIMEOUT                 , MG_CONFIG_MAX_TIMEOUT                 , L""                                       },
    { MgdConfigProperties::FeatureServicePropertyDataConnectionTimerInterval         , MgPropertyType::Int32     , MG_CONFIG_MIN_TIMER_INTERVAL          , MG_CONFIG_MAX_TIMER_INTERVAL          , L""                                       },
    { MgdConfigProperties::FeatureServicePropertyJoinQueryBatchSize                  , MgPropertyType::Int32     , MG_CONFIG_MIN_JOIN_QUERY_BATCH_SIZE   , MG_CONFIG_MAX_JOIN_QUERY_BATCH_SIZE   , L""                                       },
    { MgdConfigProperties::FeatureServicePropertyDataConnectionUseLimit              , MgPropertyType::String    , MG_CONFIG_MIN_FS_CP_EXCLUDED_LENGTH   , MG_CONFIG_MAX_FS_CP_EXCLUDED_LENGTH   , L""                                       },
    { MgdConfigProperties::FeatureServicePropertyDataTransactionTimeout              , MgPropertyType::Int32     , MG_CONFIG_MIN_TIMEOUT                 , MG_CONFIG_MAX_TIMEOUT                 , L""                                       },
    { MgdConfigProperties::FeatureServicePropertyDataTransactionTimerInterval        , MgPropertyType::Int32     , MG_CONFIG_MIN_TIMER_INTERVAL          , MG_CONFIG_MAX_TIMER_INTERVAL          , L""                                       },
    { L""                                                                           , 0                         , 0.0                                   , 0.0                                   , L""                                       }
};

const MgConfigValidationInfo MgdConfigProperties::sm_cviMappingServiceProperties[] =
{
    { MgdConfigProperties::MappingServicePropertyLegendFont                          , MgPropertyType::String    , MG_CONFIG_MIN_FONT_NAME_LENGTH        , MG_CONFIG_MAX_FONT_NAME_LENGTH        , L""                                       },
    { L""                                                                           , 0                         , 0.0                                   , 0.0                                   , L""                                       }
};

const MgConfigValidationInfo MgdConfigProperties::sm_cviRenderingServiceProperties[] =
{
    { MgdConfigProperties::RenderingServicePropertyTileExtentOffset                  , MgPropertyType::Double    , 0.0                                   , 1.0                                   , L""                                       },
    { MgdConfigProperties::RenderingServicePropertyRasterGridSize                    , MgPropertyType::Int32     , 1                                     , MG_CONFIG_MAX_INT32                   , L""                                       },
    { MgdConfigProperties::RenderingServicePropertyMinRasterGridSize                 , MgPropertyType::Int32     , 1                                     , MG_CONFIG_MAX_INT32                   , L""                                       },
    { MgdConfigProperties::RenderingServicePropertyRasterGridSizeOverrideRatio       , MgPropertyType::Double    , 0.0                                   , 1.0                                   , L""                                       },
    { MgdConfigProperties::RenderingServicePropertyRasterGridSizeForPlot             , MgPropertyType::Int32     , 1                                     , MG_CONFIG_MAX_INT32                   , L""                                       },
    { MgdConfigProperties::RenderingServicePropertyMinRasterGridSizeForPlot          , MgPropertyType::Int32     , 1                                     , MG_CONFIG_MAX_INT32                   , L""                                       },
    { MgdConfigProperties::RenderingServicePropertyRasterGridSizeOverrideRatioForPlot, MgPropertyType::Double    , 0.0                                   , 1.0                                   , L""                                       },
    { MgdConfigProperties::RenderingServicePropertyRenderSelectionBatchSize          , MgPropertyType::Int32     , MG_CONFIG_MIN_RENDER_SELECTION_SIZE   , MG_CONFIG_MAX_RENDER_SELECTION_SIZE   , L""                                       },
    { MgdConfigProperties::RenderingServicePropertyMaxRasterImageWidth               , MgPropertyType::Int32     , 1                                     , MG_CONFIG_MAX_INT32                   , L""                                       },
    { MgdConfigProperties::RenderingServicePropertyMaxRasterImageHeight              , MgPropertyType::Int32     , 1                                     , MG_CONFIG_MAX_INT32                   , L""                                       },
    { L""                                                                           , 0                         , 0.0                                   , 0.0                                   , L""                                       }
};

const MgConfigValidationInfo MgdConfigProperties::sm_cviResourceServiceProperties[] =
{
    { MgdConfigProperties::ResourceServicePropertyLibraryRepositoryPath              , MgPropertyType::String    , MG_CONFIG_MIN_PATH_LENGTH             , MG_CONFIG_MAX_PATH_LENGTH             , MG_CONFIG_PATH_RESERVED_CHARACTERS        },
    { MgdConfigProperties::ResourceServicePropertyLibraryResourceDataFilePath        , MgPropertyType::String    , MG_CONFIG_MIN_PATH_LENGTH             , MG_CONFIG_MAX_PATH_LENGTH             , MG_CONFIG_PATH_RESERVED_CHARACTERS        },
    { MgdConfigProperties::ResourceServicePropertyPackagesPath                       , MgPropertyType::String    , MG_CONFIG_MIN_PATH_LENGTH             , MG_CONFIG_MAX_PATH_LENGTH             , MG_CONFIG_PATH_RESERVED_CHARACTERS        },
    { MgdConfigProperties::ResourceServicePropertyResourceDataFileTrashFolderName    , MgPropertyType::String    , MG_CONFIG_MIN_FOLDER_NAME_LENGTH      , MG_CONFIG_MAX_FOLDER_NAME_LENGTH      , MG_CONFIG_FOLDER_NAME_RESERVED_CHARACTERS },
    { MgdConfigProperties::ResourceServicePropertyResourceSchemaFilePath             , MgPropertyType::String    , MG_CONFIG_MIN_PATH_LENGTH             , MG_CONFIG_MAX_PATH_LENGTH             , MG_CONFIG_PATH_RESERVED_CHARACTERS        },
    { MgdConfigProperties::ResourceServicePropertySessionRepositoryPath              , MgPropertyType::String    , MG_CONFIG_MIN_PATH_LENGTH             , MG_CONFIG_MAX_PATH_LENGTH             , MG_CONFIG_PATH_RESERVED_CHARACTERS        },
    { MgdConfigProperties::ResourceServicePropertySessionResourceDataFilePath        , MgPropertyType::String    , MG_CONFIG_MIN_PATH_LENGTH             , MG_CONFIG_MAX_PATH_LENGTH             , MG_CONFIG_PATH_RESERVED_CHARACTERS        },
    { MgdConfigProperties::ResourceServicePropertySiteRepositoryPath                 , MgPropertyType::String    , MG_CONFIG_MIN_PATH_LENGTH             , MG_CONFIG_MAX_PATH_LENGTH             , MG_CONFIG_PATH_RESERVED_CHARACTERS        },
    { MgdConfigProperties::ResourceServicePropertyResourceValidationEnabled          , MgPropertyType::Boolean   , 0                                     , 1                                     , L""                                       },
    { MgdConfigProperties::ResourceServicePropertyRetryAttempts                      , MgPropertyType::Int32     , 0                                     , 1000                                  , L""                                       },
    { MgdConfigProperties::ResourceServicePropertyRetryInterval                      , MgPropertyType::Int32     , 0                                     , 60000                                 , L""                                       },
    { MgdConfigProperties::ResourceServicePropertySessionRepositoriesLimit           , MgPropertyType::Int32     , 0                                     , 60000                                 , L""                                       },
    { L""                                                                           , 0                         , 0.0                                   , 0.0                                   , L""                                       }
};

const MgConfigValidationInfo MgdConfigProperties::sm_cviSiteServiceProperties[] =
{
    { MgdConfigProperties::SiteServicePropertySessionTimeout                         , MgPropertyType::Int32     , MG_CONFIG_MIN_TIMEOUT                 , MG_CONFIG_MAX_TIMEOUT                 , L""                                       },
    { MgdConfigProperties::SiteServicePropertySessionTimerInterval                   , MgPropertyType::Int32     , MG_CONFIG_MIN_TIMER_INTERVAL          , MG_CONFIG_MAX_TIMER_INTERVAL          , L""                                       },
    { L""                                                                           , 0                         , 0.0                                   , 0.0                                   , L""                                       }
};

const MgConfigValidationInfo MgdConfigProperties::sm_cviTileServiceProperties[] =
{
    { MgdConfigProperties::TileServicePropertyRenderOnly                             , MgPropertyType::Boolean   , 0                                     , 1                                     , L""                                       },
    { MgdConfigProperties::TileServicePropertyTileCachePath                          , MgPropertyType::String    , MG_CONFIG_MIN_PATH_LENGTH             , MG_CONFIG_MAX_PATH_LENGTH             , MG_CONFIG_PATH_RESERVED_CHARACTERS        },
    { MgdConfigProperties::TileServicePropertyTileColumnsPerFolder                   , MgPropertyType::Int32     , 1                                     , 1000                                  , L""                                       },
    { MgdConfigProperties::TileServicePropertyTileRowsPerFolder                      , MgPropertyType::Int32     , 1                                     , 1000                                  , L""                                       },
    { MgdConfigProperties::TileServicePropertyCreationCutoffTime                     , MgPropertyType::Int32     , 1                                     , 600                                   , L""                                       },
    { MgdConfigProperties::TileServicePropertyPollingInterval                        , MgPropertyType::Int32     , 1                                     , 60                                    , L""                                       },
    { MgdConfigProperties::TileServicePropertyTiledMapCacheSize                      , MgPropertyType::Int32     , MG_CONFIG_MIN_CACHE_SIZE              , MG_CONFIG_MAX_CACHE_SIZE              , L""                                       },
    { MgdConfigProperties::TileServicePropertyTileSizeX                              , MgPropertyType::Int32     , 50                                    , 10000                                 , L""                                       },
    { MgdConfigProperties::TileServicePropertyTileSizeY                              , MgPropertyType::Int32     , 50                                    , 10000                                 , L""                                       },
    { MgdConfigProperties::TileServicePropertyImageFormat                            , MgPropertyType::String    , 2                                     , 4                                     , L""                                       },
    { L""                                                                           , 0                         , 0.0                                   , 0.0                                   , L""                                       }
};

const MgConfigValidationInfo MgdConfigProperties::sm_cviAccessLogProperties[] =
{
    { MgdConfigProperties::AccessLogPropertyEnabled                                  , MgPropertyType::Boolean   , 0                                     , 1                                     , L""                                       },
    { MgdConfigProperties::AccessLogPropertyFilename                                 , MgPropertyType::String    , MG_CONFIG_MIN_FILE_NAME_LENGTH        , MG_CONFIG_MAX_FILE_NAME_LENGTH        , MG_CONFIG_FILE_NAME_RESERVED_CHARACTERS   },
    { MgdConfigProperties::AccessLogPropertyParameters                               , MgPropertyType::String    , MG_CONFIG_MIN_LOG_PARAMETERS_LENGTH   , MG_CONFIG_MAX_LOG_PARAMETERS_LENGTH   , L""                                       },
    { L""                                                                           , 0                         , 0.0                                   , 0.0                                   , L""                                       }
};

const MgConfigValidationInfo MgdConfigProperties::sm_cviAdminLogProperties[] =
{
    { MgdConfigProperties::AdminLogPropertyEnabled                                   , MgPropertyType::Boolean   , 0                                     , 1                                     , L""                                       },
    { MgdConfigProperties::AdminLogPropertyFilename                                  , MgPropertyType::String    , MG_CONFIG_MIN_FILE_NAME_LENGTH        , MG_CONFIG_MAX_FILE_NAME_LENGTH        , MG_CONFIG_FILE_NAME_RESERVED_CHARACTERS   },
    { MgdConfigProperties::AdminLogPropertyParameters                                , MgPropertyType::String    , MG_CONFIG_MIN_LOG_PARAMETERS_LENGTH   , MG_CONFIG_MAX_LOG_PARAMETERS_LENGTH   , L""                                       },
    { L""                                                                           , 0                         , 0.0                                   , 0.0                                   , L""                                       }
};

const MgConfigValidationInfo MgdConfigProperties::sm_cviAuthenticationLogProperties[] =
{
    { MgdConfigProperties::AuthenticationLogPropertyEnabled                          , MgPropertyType::Boolean   , 0                                     , 1                                     , L""                                       },
    { MgdConfigProperties::AuthenticationLogPropertyFilename                         , MgPropertyType::String    , MG_CONFIG_MIN_FILE_NAME_LENGTH        , MG_CONFIG_MAX_FILE_NAME_LENGTH        , MG_CONFIG_FILE_NAME_RESERVED_CHARACTERS   },
    { MgdConfigProperties::AuthenticationLogPropertyParameters                       , MgPropertyType::String    , MG_CONFIG_MIN_LOG_PARAMETERS_LENGTH   , MG_CONFIG_MAX_LOG_PARAMETERS_LENGTH   , L""                                       },
    { L""                                                                           , 0                         , 0.0                                   , 0.0                                   , L""                                       }
};

const MgConfigValidationInfo MgdConfigProperties::sm_cviErrorLogProperties[] =
{
    { MgdConfigProperties::ErrorLogPropertyEnabled                                   , MgPropertyType::Boolean   , 0                                     , 1                                     , L""                                       },
    { MgdConfigProperties::ErrorLogPropertyFilename                                  , MgPropertyType::String    , MG_CONFIG_MIN_FILE_NAME_LENGTH        , MG_CONFIG_MAX_FILE_NAME_LENGTH        , MG_CONFIG_FILE_NAME_RESERVED_CHARACTERS   },
    { MgdConfigProperties::ErrorLogPropertyParameters                                , MgPropertyType::String    , MG_CONFIG_MIN_LOG_PARAMETERS_LENGTH   , MG_CONFIG_MAX_LOG_PARAMETERS_LENGTH   , L""                                       },
    { L""                                                                           , 0                         , 0.0                                   , 0.0                                   , L""                                       }
};

const MgConfigValidationInfo MgdConfigProperties::sm_cviPerformanceLogProperties[] =
{
    { MgdConfigProperties::PerformanceLogPropertyEnabled                             , MgPropertyType::Boolean   , 0                                     , 1                                     , L""                                       },
    { MgdConfigProperties::PerformanceLogPropertyFilename                            , MgPropertyType::String    , MG_CONFIG_MIN_FILE_NAME_LENGTH        , MG_CONFIG_MAX_FILE_NAME_LENGTH        , MG_CONFIG_FILE_NAME_RESERVED_CHARACTERS   },
    { MgdConfigProperties::PerformanceLogPropertyParameters                          , MgPropertyType::String    , MG_CONFIG_MIN_LOG_PARAMETERS_LENGTH   , MG_CONFIG_MAX_LOG_PARAMETERS_LENGTH   , L""                                       },
    { MgdConfigProperties::PerformanceLogPropertyInterval                            , MgPropertyType::Int32     , 0                                     , 60000                                 , L""                                       },
    { L""                                                                           , 0                         , 0.0                                   , 0.0                                   , L""                                       }
};

const MgConfigValidationInfo MgdConfigProperties::sm_cviSessionLogProperties[] =
{
    { MgdConfigProperties::SessionLogPropertyEnabled                                 , MgPropertyType::Boolean   , 0                                     , 1                                     , L""                                       },
    { MgdConfigProperties::SessionLogPropertyFilename                                , MgPropertyType::String    , MG_CONFIG_MIN_FILE_NAME_LENGTH        , MG_CONFIG_MAX_FILE_NAME_LENGTH        , MG_CONFIG_FILE_NAME_RESERVED_CHARACTERS   },
    { MgdConfigProperties::SessionLogPropertyParameters                              , MgPropertyType::String    , MG_CONFIG_MIN_LOG_PARAMETERS_LENGTH   , MG_CONFIG_MAX_LOG_PARAMETERS_LENGTH   , L""                                       },
    { L""                                                                           , 0                         , 0.0                                   , 0.0                                   , L""                                       }
};

const MgConfigValidationInfo MgdConfigProperties::sm_cviTraceLogProperties[] =
{
    { MgdConfigProperties::TraceLogPropertyEnabled                                   , MgPropertyType::Boolean   , 0                                     , 1                                     , L""                                       },
    { MgdConfigProperties::TraceLogPropertyFilename                                  , MgPropertyType::String    , MG_CONFIG_MIN_FILE_NAME_LENGTH        , MG_CONFIG_MAX_FILE_NAME_LENGTH        , MG_CONFIG_FILE_NAME_RESERVED_CHARACTERS   },
    { MgdConfigProperties::TraceLogPropertyParameters                                , MgPropertyType::String    , MG_CONFIG_MIN_LOG_PARAMETERS_LENGTH   , MG_CONFIG_MAX_LOG_PARAMETERS_LENGTH   , L""                                       },
    { L""                                                                           , 0                         , 0.0                                   , 0.0                                   , L""                                       }
};
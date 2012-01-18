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
const STRING MgConfigProperties::DefaultConfigurationFilename                               = L"webconfig.ini";
const STRING MgConfigProperties::ServerConfigurationFilename                                = L"serverconfig.ini";

// Configuration file section/property names and default values

// ******************************************************************
// General Properties
// ******************************************************************
const STRING MgConfigProperties::GeneralPropertiesSection                                   = L"GeneralProperties";
const STRING MgConfigProperties::GeneralPropertyLocale                                      = L"Locale";
const STRING MgConfigProperties::DefaultGeneralPropertyLocale                               = L"";
const STRING MgConfigProperties::GeneralPropertyDefaultMessageLocale                        = L"DefaultMessageLocale";
const STRING MgConfigProperties::DefaultGeneralPropertyDefaultMessageLocale                 = L"en";      // ISO 639-1 name
const STRING MgConfigProperties::GeneralPropertyFdoPath                                     = L"FdoPath";
const STRING MgConfigProperties::DefaultGeneralPropertyFdoPath                              = L"FDO/";
const STRING MgConfigProperties::GeneralPropertyMentorDictionaryPath                        = L"MentorDictionaryPath";
const STRING MgConfigProperties::DefaultGeneralPropertyMentorDictionaryPath                 = L"";
const STRING MgConfigProperties::GeneralPropertyLinuxMemDebug                               = L"LinuxMemDebug";
const bool   MgConfigProperties::DefaultGeneralPropertyLinuxMemDebug                        = false;
const STRING MgConfigProperties::GeneralPropertyResourcesPath                               = L"ResourcesPath";
const STRING MgConfigProperties::DefaultGeneralPropertyResourcesPath                        = L"Resources/";
const STRING MgConfigProperties::GeneralPropertyTempPath                                    = L"TempPath";
const STRING MgConfigProperties::DefaultGeneralPropertyTempPath                             = L"Temp/";
const STRING MgConfigProperties::GeneralPropertyWfsDocumentPath                             = L"WfsDocumentPath";
const STRING MgConfigProperties::DefaultGeneralPropertyWfsDocumentPath                      = L"Wfs/";
const STRING MgConfigProperties::GeneralPropertyWmsDocumentPath                             = L"WmsDocumentPath";
const STRING MgConfigProperties::DefaultGeneralPropertyWmsDocumentPath                      = L"Wms/";
const STRING MgConfigProperties::GeneralPropertyRenderer                                    = L"Renderer";
const STRING MgConfigProperties::DefaultGeneralPropertyRenderer                             = L"GD";
const STRING MgConfigProperties::GeneralPropertyPreCacheMaps                                = L"PreCacheMaps";
const STRING MgConfigProperties::DefaultGeneralPropertyPreCacheMaps                         = L"";

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
const STRING MgConfigProperties::FeatureServicePropertyDataConnectionUseLimit               = L"DataConnectionUseLimit";
const STRING MgConfigProperties::DefaultFeatureServicePropertyDataConnectionUseLimit        = L"";
const STRING MgConfigProperties::FeatureServicePropertyDataTransactionTimeout               = L"DataTransactionTimeout";
const INT32  MgConfigProperties::DefaultFeatureServicePropertyDataTransactionTimeout        = 360;
const STRING MgConfigProperties::FeatureServicePropertyDataTransactionTimerInterval         = L"DataTransactionTimerInterval";
const INT32  MgConfigProperties::DefaultFeatureServicePropertyDataTransactionTimerInterval  = 60;
const STRING MgConfigProperties::FeatureServicePropertyFDOConnectionTimeoutCustom           = L"FDOConnectionTimeoutCustom";
const STRING MgConfigProperties::DefaultFeatureServicePropertyFDOConnectionTimeoutCustom    = L"OSGeo.WMS:120";

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
const STRING MgConfigProperties::RenderingServicePropertyRasterGridSizeForPlot              = L"RasterGridSizeForPlot";
const INT32  MgConfigProperties::DefaultRenderingServicePropertyRasterGridSizeForPlot       = 50;
const STRING MgConfigProperties::RenderingServicePropertyMinRasterGridSizeForPlot           = L"MinRasterGridSizeForPlot";
const INT32  MgConfigProperties::DefaultRenderingServicePropertyMinRasterGridSizeForPlot    = 5;
const STRING MgConfigProperties::RenderingServicePropertyRasterGridSizeOverrideRatioForPlot = L"RasterGridSizeOverrideRatioForPlot";
const double MgConfigProperties::DefaultRenderingServicePropertyRasterGridSizeOverrideRatioForPlot = 0.10;
const STRING MgConfigProperties::RenderingServicePropertyRenderSelectionBatchSize           = L"RenderSelectionBatchSize";
const INT32  MgConfigProperties::DefaultRenderingServicePropertyRenderSelectionBatchSize    = 50000;
const STRING MgConfigProperties::RenderingServicePropertyClampPoints                        = L"ClampPoints";
const bool   MgConfigProperties::DefaultRenderingServicePropertyClampPoints                 = false;
const STRING MgConfigProperties::RenderingServicePropertyGeneralizeData                     = L"GeneralizeData";
const bool   MgConfigProperties::DefaultRenderingServicePropertyGeneralizeData              = false;
const STRING MgConfigProperties::RenderingServicePropertyMaxRasterImageWidth                = L"MaxRasterImageWidth";
const INT32  MgConfigProperties::DefaultRenderingServicePropertyMaxRasterImageWidth         = 2048;
const STRING MgConfigProperties::RenderingServicePropertyMaxRasterImageHeight               = L"MaxRasterImageHeight";
const INT32  MgConfigProperties::DefaultRenderingServicePropertyMaxRasterImageHeight        = 2048;

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
const STRING MgConfigProperties::DefaultResourceServicePropertyLibraryRepositoryPath            = L"Repositories/Library/Content/";
const STRING MgConfigProperties::ResourceServicePropertyLibraryResourceDataFilePath             = L"LibraryResourceDataFilePath";
const STRING MgConfigProperties::DefaultResourceServicePropertyLibraryResourceDataFilePath      = L"Repositories/Library/DataFiles/";
const STRING MgConfigProperties::ResourceServicePropertyPackagesPath                            = L"PackagesPath";
const STRING MgConfigProperties::DefaultResourceServicePropertyPackagesPath                     = L"Packages/";
const STRING MgConfigProperties::ResourceServicePropertyResourceDataFileTrashFolderName         = L"ResourceDataFileTrashFolderName";
const STRING MgConfigProperties::DefaultResourceServicePropertyResourceDataFileTrashFolderName  = L"Trash";
const STRING MgConfigProperties::ResourceServicePropertyResourceSchemaFilePath                  = L"ResourceSchemaFilePath";
const STRING MgConfigProperties::DefaultResourceServicePropertyResourceSchemaFilePath           = L"Schema/";
const STRING MgConfigProperties::ResourceServicePropertySessionRepositoryPath                   = L"SessionRepositoryPath";
const STRING MgConfigProperties::DefaultResourceServicePropertySessionRepositoryPath            = L"Repositories/Session/Content/";
const STRING MgConfigProperties::ResourceServicePropertySessionResourceDataFilePath             = L"SessionResourceDataFilePath";
const STRING MgConfigProperties::DefaultResourceServicePropertySessionResourceDataFilePath      = L"Repositories/Session/DataFiles/";
const STRING MgConfigProperties::ResourceServicePropertySiteRepositoryPath                      = L"SiteRepositoryPath";
const STRING MgConfigProperties::DefaultResourceServicePropertySiteRepositoryPath               = L"Repositories/Site/";
const STRING MgConfigProperties::ResourceServicePropertyResourceValidationEnabled               = L"ResourceValidationEnabled"; // for internal use only
const bool   MgConfigProperties::DefaultResourceServicePropertyResourceValidationEnabled        = true;
const STRING MgConfigProperties::ResourceServicePropertyRetryAttempts                           = L"RetryAttempts";             // for internal use only
const INT32  MgConfigProperties::DefaultResourceServicePropertyRetryAttempts                    = 50;
const STRING MgConfigProperties::ResourceServicePropertyRetryInterval                           = L"RetryInterval";             // for internal use only
const INT32  MgConfigProperties::DefaultResourceServicePropertyRetryInterval                    = 25;
const STRING MgConfigProperties::ResourceServicePropertySessionRepositoriesConfig               = L"SessionRepositoriesConfig";
const STRING MgConfigProperties::DefaultResourceServicePropertySessionRepositoriesConfig        = L"FilePerSession";
const STRING MgConfigProperties::ResourceServicePropertySessionRepositoriesLimit                = L"SessionRepositoriesLimit";
const INT32  MgConfigProperties::DefaultResourceServicePropertySessionRepositoriesLimit         = 200;

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

///////////////////////////////////////////////////////////////////////////////
/// Common Configuration Validation Information Maps
///
const MgConfigValidationInfo MgConfigProperties::sm_cviGeneralProperties[] =
{
    { MgConfigProperties::GeneralPropertyLocale                                     , MgPropertyType::String    , MG_CONFIG_MIN_OPTIONAL_STRING_LENGTH  , MG_CONFIG_MAX_OPTIONAL_STRING_LENGTH  , L""                                       },
    { MgConfigProperties::GeneralPropertyDefaultMessageLocale                       , MgPropertyType::String    , MG_CONFIG_MIN_LOCALE_LENGTH           , MG_CONFIG_MAX_LOCALE_LENGTH           , L""                                       },
    { MgConfigProperties::GeneralPropertyFdoPath                                    , MgPropertyType::String    , MG_CONFIG_MIN_PATH_LENGTH             , MG_CONFIG_MAX_PATH_LENGTH             , MG_CONFIG_PATH_RESERVED_CHARACTERS        },
    { MgConfigProperties::GeneralPropertyLinuxMemDebug                              , MgPropertyType::Boolean   , 0                                     , 1                                     , L""                                       },
    { MgConfigProperties::GeneralPropertyResourcesPath                              , MgPropertyType::String    , MG_CONFIG_MIN_PATH_LENGTH             , MG_CONFIG_MAX_PATH_LENGTH             , MG_CONFIG_PATH_RESERVED_CHARACTERS        },
    { MgConfigProperties::GeneralPropertyTempPath                                   , MgPropertyType::String    , MG_CONFIG_MIN_PATH_LENGTH             , MG_CONFIG_MAX_PATH_LENGTH             , MG_CONFIG_PATH_RESERVED_CHARACTERS        },
    { MgConfigProperties::GeneralPropertyWfsDocumentPath                            , MgPropertyType::String    , MG_CONFIG_MIN_PATH_LENGTH             , MG_CONFIG_MAX_PATH_LENGTH             , MG_CONFIG_PATH_RESERVED_CHARACTERS        },
    { MgConfigProperties::GeneralPropertyWmsDocumentPath                            , MgPropertyType::String    , MG_CONFIG_MIN_PATH_LENGTH             , MG_CONFIG_MAX_PATH_LENGTH             , MG_CONFIG_PATH_RESERVED_CHARACTERS        },
    { L""                                                                           , 0                         , 0.0                                   , 0.0                                   , L""                                       }
};

///////////////////////////////////////////////////////////////////////////////
/// Server Configuration Validation Information Maps
///

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
    { MgConfigProperties::FeatureServicePropertyDataConnectionUseLimit              , MgPropertyType::String    , MG_CONFIG_MIN_FS_CP_EXCLUDED_LENGTH   , MG_CONFIG_MAX_FS_CP_EXCLUDED_LENGTH   , L""                                       },
    { MgConfigProperties::FeatureServicePropertyDataTransactionTimeout              , MgPropertyType::Int32     , MG_CONFIG_MIN_TIMEOUT                 , MG_CONFIG_MAX_TIMEOUT                 , L""                                       },
    { MgConfigProperties::FeatureServicePropertyDataTransactionTimerInterval        , MgPropertyType::Int32     , MG_CONFIG_MIN_TIMER_INTERVAL          , MG_CONFIG_MAX_TIMER_INTERVAL          , L""                                       },
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
    { MgConfigProperties::RenderingServicePropertyRasterGridSizeForPlot             , MgPropertyType::Int32     , 1                                     , MG_CONFIG_MAX_INT32                   , L""                                       },
    { MgConfigProperties::RenderingServicePropertyMinRasterGridSizeForPlot          , MgPropertyType::Int32     , 1                                     , MG_CONFIG_MAX_INT32                   , L""                                       },
    { MgConfigProperties::RenderingServicePropertyRasterGridSizeOverrideRatioForPlot, MgPropertyType::Double    , 0.0                                   , 1.0                                   , L""                                       },
    { MgConfigProperties::RenderingServicePropertyRenderSelectionBatchSize          , MgPropertyType::Int32     , MG_CONFIG_MIN_RENDER_SELECTION_SIZE   , MG_CONFIG_MAX_RENDER_SELECTION_SIZE   , L""                                       },
    { MgConfigProperties::RenderingServicePropertyMaxRasterImageWidth               , MgPropertyType::Int32     , 1                                     , MG_CONFIG_MAX_INT32                   , L""                                       },
    { MgConfigProperties::RenderingServicePropertyMaxRasterImageHeight              , MgPropertyType::Int32     , 1                                     , MG_CONFIG_MAX_INT32                   , L""                                       },
    { L""                                                                           , 0                         , 0.0                                   , 0.0                                   , L""                                       }
};

const MgConfigValidationInfo MgConfigProperties::sm_cviResourceServiceProperties[] =
{
    { MgConfigProperties::ResourceServicePropertyLibraryRepositoryPath              , MgPropertyType::String    , MG_CONFIG_MIN_PATH_LENGTH             , MG_CONFIG_MAX_PATH_LENGTH             , MG_CONFIG_PATH_RESERVED_CHARACTERS        },
    { MgConfigProperties::ResourceServicePropertyLibraryResourceDataFilePath        , MgPropertyType::String    , MG_CONFIG_MIN_PATH_LENGTH             , MG_CONFIG_MAX_PATH_LENGTH             , MG_CONFIG_PATH_RESERVED_CHARACTERS        },
    { MgConfigProperties::ResourceServicePropertyPackagesPath                       , MgPropertyType::String    , MG_CONFIG_MIN_PATH_LENGTH             , MG_CONFIG_MAX_PATH_LENGTH             , MG_CONFIG_PATH_RESERVED_CHARACTERS        },
    { MgConfigProperties::ResourceServicePropertyResourceDataFileTrashFolderName    , MgPropertyType::String    , MG_CONFIG_MIN_FOLDER_NAME_LENGTH      , MG_CONFIG_MAX_FOLDER_NAME_LENGTH      , MG_CONFIG_FOLDER_NAME_RESERVED_CHARACTERS },
    { MgConfigProperties::ResourceServicePropertyResourceSchemaFilePath             , MgPropertyType::String    , MG_CONFIG_MIN_PATH_LENGTH             , MG_CONFIG_MAX_PATH_LENGTH             , MG_CONFIG_PATH_RESERVED_CHARACTERS        },
    { MgConfigProperties::ResourceServicePropertySessionRepositoryPath              , MgPropertyType::String    , MG_CONFIG_MIN_PATH_LENGTH             , MG_CONFIG_MAX_PATH_LENGTH             , MG_CONFIG_PATH_RESERVED_CHARACTERS        },
    { MgConfigProperties::ResourceServicePropertySessionResourceDataFilePath        , MgPropertyType::String    , MG_CONFIG_MIN_PATH_LENGTH             , MG_CONFIG_MAX_PATH_LENGTH             , MG_CONFIG_PATH_RESERVED_CHARACTERS        },
    { MgConfigProperties::ResourceServicePropertySiteRepositoryPath                 , MgPropertyType::String    , MG_CONFIG_MIN_PATH_LENGTH             , MG_CONFIG_MAX_PATH_LENGTH             , MG_CONFIG_PATH_RESERVED_CHARACTERS        },
    { MgConfigProperties::ResourceServicePropertyResourceValidationEnabled          , MgPropertyType::Boolean   , 0                                     , 1                                     , L""                                       },
    { MgConfigProperties::ResourceServicePropertyRetryAttempts                      , MgPropertyType::Int32     , 0                                     , 1000                                  , L""                                       },
    { MgConfigProperties::ResourceServicePropertyRetryInterval                      , MgPropertyType::Int32     , 0                                     , 60000                                 , L""                                       },
    { MgConfigProperties::ResourceServicePropertySessionRepositoriesLimit           , MgPropertyType::Int32     , 0                                     , 60000                                 , L""                                       },
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

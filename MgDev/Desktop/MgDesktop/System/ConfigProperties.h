#ifndef DESKTOP_CONFIG_PROPERTIES_H
#define DESKTOP_CONFIG_PROPERTIES_H

#include "MgDesktop.h"

//TODO: Remove un-used config properties

/// \cond INTERNAL
//////////////////////////////////////////////////////////////////
/// \brief
/// Properties stored in an MgConfiguration object.  Note that the properties are
/// organized into Property Sections.  These sections may be used in the API to
/// set/get groups of configuration properties.
/// INTERNAL_ONLY:
class MG_DESKTOP_API MgConfigProperties
{
    DECLARE_CLASSNAME(MgConfigProperties)

EXTERNAL_API:

    /// Default configuration filenames
    static const STRING DefaultConfigurationFilename;                   /// value("webconfig.ini")
    static const STRING ServerConfigurationFilename;                    /// value("serverconfig.ini")

    ///////////////////////////////////////////////////////////////////////////
    /// Common Configuration Validation Information Maps
    ///

    /// GENERAL PROPERTIES SECTION ---------------------------------------------------------------------------------------

    /// General server properties
    static const STRING GeneralPropertiesSection;                       /// value("GeneralProperties")

    /// Server's locale
    static const STRING GeneralPropertyLocale;                          /// value("Locale")
    static const STRING DefaultGeneralPropertyLocale;                   /// value("")

    /// Server's default message locale
    static const STRING GeneralPropertyDefaultMessageLocale;            /// value("DefaultMessageLocale")
    static const STRING DefaultGeneralPropertyDefaultMessageLocale;     /// value("en")

    /// Fdo file path
    static const STRING GeneralPropertyFdoPath;                         /// value("FdoPath")
    static const STRING DefaultGeneralPropertyFdoPath;                  /// value("FDO/")

    /// Mentor dictionary path
    static const STRING GeneralPropertyMentorDictionaryPath;            /// value("FdoPath")
    static const STRING DefaultGeneralPropertyMentorDictionaryPath;     /// value("")

    /// Memory allocator debugging for Linux
    static const STRING GeneralPropertyLinuxMemDebug;               /// value("LinuxMemDebug")
    static const bool DefaultGeneralPropertyLinuxMemDebug;        /// value(false);

    /// Root folder for server's localization resources
    static const STRING GeneralPropertyResourcesPath;                   /// value("ResourcesPath")
    static const STRING DefaultGeneralPropertyResourcesPath;            /// value("Resources/")

    /// Temporary file path
    static const STRING GeneralPropertyTempPath;                        /// value("TempPath")
    static const STRING DefaultGeneralPropertyTempPath;                 /// value("Temp/")

    /// Document path location for WFS documents
    static const STRING GeneralPropertyWfsDocumentPath;                 /// value("WfsDocumentPath")
    static const STRING DefaultGeneralPropertyWfsDocumentPath;          /// value("Wfs/")

    /// Document path location for WMS documents
    static const STRING GeneralPropertyWmsDocumentPath;                 /// value("WmsDocumentPath")
    static const STRING DefaultGeneralPropertyWmsDocumentPath;          /// value("Wms/")

    /// Sets desired image renderer
    static const STRING GeneralPropertyRenderer;                        /// value("Renderer")
    static const STRING DefaultGeneralPropertyRenderer;                 /// value("GD")

    /// Sets the maps to precache
    static const STRING GeneralPropertyPreCacheMaps;                    /// value("PreCacheMaps")
    static const STRING DefaultGeneralPropertyPreCacheMaps;             /// value("")

EXTERNAL_API:

    /// DRAWING SERVICE PROPERTIES SECTION -------------------------------------------------------------------------------

    /// Drawing Service properties.  Note that in V1.0 this property section is only defined on the Site Server.
    static const STRING DrawingServicePropertiesSection;                /// value("DrawingServiceProperties")

    /// FEATURE SERVICE PROPERTIES SECTION -------------------------------------------------------------------------------

    /// Feature Service properties.  Note that in V1.0 this property section is only defined on the Site Server.
    static const STRING FeatureServicePropertiesSection;                        /// value("FeatureServiceProperties")

    /// Sets the maximum number of internal data objects to cache
    static const STRING FeatureServicePropertyCacheSize;                        /// value("CacheSize")
    static const INT32 DefaultFeatureServicePropertyCacheSize;                  /// value(100)

    /// Sets the maximum amount of time (in seconds) to cache the internal data objects
    static const STRING FeatureServicePropertyCacheTimeLimit;                   /// value("CacheTimeLimit")
    static const INT32 DefaultFeatureServicePropertyCacheTimeLimit;             /// value(86400)

    /// Sets the time duration (in seconds) between cache timelimit checks
    static const STRING FeatureServicePropertyCacheTimerInterval;               /// value("CacheTimerInterval")
    static const INT32 DefaultFeatureServicePropertyCacheTimerInterval;         /// value(3600)

    /// Sets the maximum number of features fetched to reduce the network traffic
    static const STRING FeatureServicePropertyDataCacheSize;                    /// value("DataCacheSize")
    static const INT32 DefaultFeatureServicePropertyDataCacheSize;              /// value(100)

    /// Sets whether to pool connections
    static const STRING FeatureServicePropertyDataConnectionPoolEnabled;        /// value("DataConnectionPoolEnabled")
    static const bool DefaultFeatureServicePropertyDataConnectionPoolEnabled;   /// value(false)

    /// Sets the excluded privders from the pooled data connections
    static const STRING FeatureServicePropertyDataConnectionPoolExcludedProviders;          /// value("DataConnectionPoolExcludedProviders")
    static const STRING DefaultFeatureServicePropertyDataConnectionPoolExcludedProviders;   /// value("OSGeo.SDF,OSGeo.SHP")

    /// Sets the number of pooled data connections
    static const STRING FeatureServicePropertyDataConnectionPoolSize;           /// value("DataConnectionPoolSize")
    static const INT32 DefaultFeatureServicePropertyDataConnectionPoolSize;     /// value(50)

    /// Sets the number of pooled data connections for a specific provider
    static const STRING FeatureServicePropertyDataConnectionPoolSizeCustom;         /// value("DataConnectionPoolSizeCustom")
    static const STRING DefaultFeatureServicePropertyDataConnectionPoolSizeCustom;  /// value("")

    /// Sets the maximum amount of time (in seconds) for data connection idle activity before the data connection is closed
    static const STRING FeatureServicePropertyDataConnectionTimeout;            /// value("DataConnectionTimeout")
    static const INT32 DefaultFeatureServicePropertyDataConnectionTimeout;      /// value(600)

    /// Sets the time duration (in seconds) between timeout checks
    static const STRING FeatureServicePropertyDataConnectionTimerInterval;      /// value("DataConnectionTimerInterval")
    static const INT32 DefaultFeatureServicePropertyDataConnectionTimerInterval;/// value(60)

    /// Sets the batch size used by the join query algorithm
    static const STRING FeatureServicePropertyJoinQueryBatchSize;               /// value("JoinQueryBatchSize")
    static const INT32 DefaultFeatureServicePropertyJoinQueryBatchSize;         /// value(100)

    /// Sets the number of uses for a FDO connection for a specific provider before it is released
    static const STRING FeatureServicePropertyDataConnectionUseLimit;         /// value("DataConnectionUseLimit")
    static const STRING DefaultFeatureServicePropertyDataConnectionUseLimit;  /// value("")

    /// Sets the maximum amount of time (in seconds) for an idle FDO transaction before the transaction is dropped
    static const STRING FeatureServicePropertyDataTransactionTimeout;            /// value("DataTransactionTimeout")
    static const INT32 DefaultFeatureServicePropertyDataTransactionTimeout;      /// value(360)

    /// Sets the time duration (in seconds) between timeout checks
    static const STRING FeatureServicePropertyDataTransactionTimerInterval;      /// value("DataTransactionTimerInterval")
    static const INT32 DefaultFeatureServicePropertyDataTransactionTimerInterval;/// value(60)

    /// Set the time duration in seconds for an FDO connection
    static const STRING FeatureServicePropertyFDOConnectionTimeoutCustom;        /// value("FDOConnectionTimeoutCustom")
    static const STRING DefaultFeatureServicePropertyFDOConnectionTimeoutCustom; /// value("OSGeo.WMS:120")

    /// MAPPING SERVICE PROPERTIES SECTION -------------------------------------------------------------------------------

    /// Mapping Service properties
    static const STRING MappingServicePropertiesSection;                        /// value("MappingServiceProperties")

    /// Sets the font to use when rendering legend elements
    static const STRING MappingServicePropertyLegendFont;                       /// value("LegendFont")
    static const STRING DefaultMappingServicePropertyLegendFont;                /// value("Arial")

    /// RENDERING SERVICE PROPERTIES SECTION -----------------------------------------------------------------------------

    /// Rendering Service properties
    static const STRING RenderingServicePropertiesSection;                      /// value("RenderingServiceProperties")

    /// Sets the maximum request extent offset to use when requesting features for a tile, specified as a factor of the tile size
    static const STRING RenderingServicePropertyTileExtentOffset;               /// value("TileExtentOffset")
    static const double DefaultRenderingServicePropertyTileExtentOffset;        /// value(0.35)

    /// Sets desired raster re-projection grid size
    static const STRING RenderingServicePropertyRasterGridSize;                 /// value("RasterGridSize")
    static const INT32 DefaultRenderingServicePropertyRasterGridSize;           /// value(100)

    /// Sets desired minimum raster re-projection grid size
    static const STRING RenderingServicePropertyMinRasterGridSize;              /// value("MinRasterGridSize")
    static const INT32 DefaultRenderingServicePropertyMinRasterGridSize;        /// value(10)

    /// Sets desired raster re-projection grid size override ratio
    static const STRING RenderingServicePropertyRasterGridSizeOverrideRatio;         /// value("RasterGridSizeOverrideRatio")
    static const double DefaultRenderingServicePropertyRasterGridSizeOverrideRatio;  /// value(0.25)

    /// Sets desired raster re-projection grid size for plot
    static const STRING RenderingServicePropertyRasterGridSizeForPlot;               /// value("RasterGridSizeForPlot")
    static const INT32 DefaultRenderingServicePropertyRasterGridSizeForPlot;         /// value(50)

    /// Sets desired minimum raster re-projection grid size for plot
    static const STRING RenderingServicePropertyMinRasterGridSizeForPlot;            /// value("MinRasterGridSizeForPlot")
    static const INT32 DefaultRenderingServicePropertyMinRasterGridSizeForPlot;      /// value(5)

    /// Sets desired raster re-projection grid size override ratio for plot
    static const STRING RenderingServicePropertyRasterGridSizeOverrideRatioForPlot;         /// value("RasterGridSizeOverrideRatioForPlot")
    static const double DefaultRenderingServicePropertyRasterGridSizeOverrideRatioForPlot;  /// value(0.10)

    /// Sets the batch size for rendering a selection
    static const STRING RenderingServicePropertyRenderSelectionBatchSize;       /// value("RenderSelectionBatchSize")
    static const INT32 DefaultRenderingServicePropertyRenderSelectionBatchSize; /// value(10000)

    /// Sets the clamp points renderer option for AGG
    static const STRING RenderingServicePropertyClampPoints;                    /// value("ClampPoints")
    static const bool DefaultRenderingServicePropertyClampPoints;               /// value(false)

    /// Sets the generalize data renderer option
    static const STRING RenderingServicePropertyGeneralizeData;                 /// value("GeneralizeData")
    static const bool DefaultRenderingServicePropertyGeneralizeData;            /// value(false)

    /// Sets the max width for raster image
    static const STRING RenderingServicePropertyMaxRasterImageWidth;           /// value("MaxRasterImageWidth")
    static const INT32  DefaultRenderingServicePropertyMaxRasterImageWidth;    /// value(2048)

    /// Sets the max height for raster image
    static const STRING RenderingServicePropertyMaxRasterImageHeight;          /// value("MaxRasterImageHeight")
    static const INT32  DefaultRenderingServicePropertyMaxRasterImageHeight;   /// value(2048)


    /// FONT ALIASES SECTION -----------------------------------------------------------------------------

    /// font alias mappings
    static const STRING FontAliasMappingSection;                               /// value("FontAliases")

    /// unmanaged data mappings
    static const STRING UnmanagedDataMappingsSection;                          /// value("UnmanagedDataMappings")

    /// RESOURCE SERVICE PROPERTIES SECTION ------------------------------------------------------------------------------

    /// Resource Service properties.  Note that in V1.0 this property section is only defined on the Site Server.
    static const STRING ResourceServicePropertiesSection;               /// value("ResourceServiceProperties")

    /// Sets the root of the library repository
    static const STRING ResourceServicePropertyLibraryRepositoryPath;               /// value("LibraryRepositoryPath")
    static const STRING DefaultResourceServicePropertyLibraryRepositoryPath;        /// value("Repositories/Library/")

    /// Sets the root of the library resource data files
    static const STRING ResourceServicePropertyLibraryResourceDataFilePath;         /// value("LibraryResourceDataFilePath")
    static const STRING DefaultResourceServicePropertyLibraryResourceDataFilePath;  /// value("Repositories/Library/DataFiles/")

    /// Sets the path to resource packages
    static const STRING ResourceServicePropertyPackagesPath;                        /// value("PackagesPath")
    static const STRING DefaultResourceServicePropertyPackagesPath;                 /// value("Packages/")

    /// Sets the trash folder name
    static const STRING ResourceServicePropertyResourceDataFileTrashFolderName;         /// value("ResourceDataFileTrashFolderName")
    static const STRING DefaultResourceServicePropertyResourceDataFileTrashFolderName;  /// value("Trash")

    /// Sets the path to all resource schema files
    static const STRING ResourceServicePropertyResourceSchemaFilePath;              /// value("ResourceSchemaFilePath")
    static const STRING DefaultResourceServicePropertyResourceSchemaFilePath;       /// value("Schema/")

INTERNAL_API:

    /// Enables/disables the resource validation
    static const STRING ResourceServicePropertyResourceValidationEnabled;       /// value("ResourceValidationEnabled")
    static const bool DefaultResourceServicePropertyResourceValidationEnabled;  /// value(true)

    /// Sets the number of retry attempts per operation
    static const STRING ResourceServicePropertyRetryAttempts;                   /// value("RetryAttempts")
    static const INT32 DefaultResourceServicePropertyRetryAttempts;             /// value(50)

    /// Sets the time duration (in milliseconds) between retry attempts
    static const STRING ResourceServicePropertyRetryInterval;                   /// value("RetryInterval")
    static const INT32 DefaultResourceServicePropertyRetryInterval;             /// value(25)

    /// Sets whether to use a single file session repository or a session repository file per session
    static const STRING ResourceServicePropertySessionRepositoriesConfig;       /// value("SessionRepositoriesConfig")
    static const STRING DefaultResourceServicePropertySessionRepositoriesConfig;/// value("FilePerSession")

    /// Sets the maximum number of allowed active sessions
    static const STRING ResourceServicePropertySessionRepositoriesLimit;        /// value("SessionRepositoriesLimit")
    static const INT32 DefaultResourceServicePropertySessionRepositoriesLimit;  /// value(200)

EXTERNAL_API:

    /// Sets the root of the session repository
    static const STRING ResourceServicePropertySessionRepositoryPath;               /// value("SessionRepositoryPath")
    static const STRING DefaultResourceServicePropertySessionRepositoryPath;        /// value("Repositories/Session/")

    /// Sets the root of the session resource data files
    static const STRING ResourceServicePropertySessionResourceDataFilePath;         /// value("SessionResourceDataFilePath")
    static const STRING DefaultResourceServicePropertySessionResourceDataFilePath;  /// value("Repositories/Session/DataFiles/")

    /// Sets the root of the site repository
    static const STRING ResourceServicePropertySiteRepositoryPath;                  /// value("SiteRepositoryPath")
    static const STRING DefaultResourceServicePropertySiteRepositoryPath;           /// value("Repositories/Site/")

    /// SITE SERVICE PROPERTIES SECTION ----------------------------------------------------------------------------------

    /// Site Service properties.  Note that in V1.0 this property section is only defined on the Site Server.
    static const STRING SiteServicePropertiesSection;                   /// value("SiteServiceProperties")

    /// Sets the maximum amount of time (in seconds) for session timeout
    static const STRING SiteServicePropertySessionTimeout;              /// value("SessionTimeout")
    static const INT32 DefaultSiteServicePropertySessionTimeout;        /// value(1200)

    /// Sets the time duration (in seconds) between session timeout checks
    static const STRING SiteServicePropertySessionTimerInterval;        /// value("SessionTimerInterval")
    static const INT32 DefaultSiteServicePropertySessionTimerInterval;  /// value(400)

    /// TILE SERVICE PROPERTIES SECTION ----------------------------------------------------------------------------------

    /// Tile Service properties
    static const STRING TileServicePropertiesSection;                   /// value("TileServiceProperties")

    /// Specifies whether the tile is only rendered
    static const STRING TileServicePropertyRenderOnly;                  /// value("RenderOnly")
    static const bool DefaultTileServicePropertyRenderOnly;             /// value(false)

    /// Sets the root of the image tile cache
    static const STRING TileServicePropertyTileCachePath;               /// value("TileCachePath")
    static const STRING DefaultTileServicePropertyTileCachePath;        /// value("Repositories/TileCache/")

    // Sets the number of columns of tiles per folder
    static const STRING TileServicePropertyTileColumnsPerFolder;        /// value("TileColumnsPerFolder")
    static const INT32 DefaultTileServicePropertyTileColumnsPerFolder;  /// value(30)

    // Sets the number of rows of tiles per folder
    static const STRING TileServicePropertyTileRowsPerFolder;           /// value("TileRowsPerFolder")
    static const INT32 DefaultTileServicePropertyTileRowsPerFolder;     /// value(30)

    // Sets the image size and format (PNG/JPG) of the tiles
    static const STRING TileServicePropertyTileSizeX;                   /// value("DefaultTileSizeX");
    static const INT32 DefaultTileServicePropertyTileSizeX;             /// value(300);
    static const STRING TileServicePropertyTileSizeY;                   /// value("DefaultTileSizeY");
    static const INT32 DefaultTileServicePropertyTileSizeY;             /// value(300);
    static const STRING TileServicePropertyImageFormat;                 /// value("ImageFormat")
    static const STRING DefaultTileServicePropertyImageFormat;          /// value("PNG")

INTERNAL_API:

    /// Sets the maximum amount of time (in seconds) to create a tile
    static const STRING TileServicePropertyCreationCutoffTime;          /// value("CreationCutoffTime")
    static const INT32 DefaultTileServicePropertyCreationCutoffTime;    /// value(120)

    /// Sets the time duration (in seconds) between lock checks
    static const STRING TileServicePropertyPollingInterval;             /// value("PollingInterval")
    static const INT32 DefaultTileServicePropertyPollingInterval;       /// value(1)

    /// Sets the limit on the number of cached MgMap objects used for tile generation
    static const STRING TileServicePropertyTiledMapCacheSize;           /// value("TiledMapCacheSize")
    static const INT32 DefaultTileServicePropertyTiledMapCacheSize;     /// value(10)

public:

    // Common Configuration Validation Information Maps
    static const MgConfigValidationInfo sm_cviGeneralProperties[];

    // Server Configuration Validation Information Maps
    static const MgConfigValidationInfo sm_cviDrawingServiceProperties[];
    static const MgConfigValidationInfo sm_cviFeatureServiceProperties[];
    static const MgConfigValidationInfo sm_cviMappingServiceProperties[];
    static const MgConfigValidationInfo sm_cviRenderingServiceProperties[];
    static const MgConfigValidationInfo sm_cviResourceServiceProperties[];
    static const MgConfigValidationInfo sm_cviSiteServiceProperties[];
    static const MgConfigValidationInfo sm_cviTileServiceProperties[];

};
/// \endcond

#endif
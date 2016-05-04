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
class MG_DESKTOP_API MgdConfigProperties
{
    DECLARE_CLASSNAME(MgdConfigProperties)

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

    /// Sets the field delimiter in the logs
    static const STRING GeneralPropertyLogsDelimiter;                   /// value("LogsDelimiter")
    static const STRING DefaultGeneralPropertyLogsDelimiter;            /// value("\t")

    /// Log detail level for services
    static const STRING GeneralPropertyLogsDetail;                      /// value("LogsDetail")
    static const STRING DefaultGeneralPropertyLogsDetail;               /// value("")

    /// Root folder for server's log files
    static const STRING GeneralPropertyLogsPath;                        /// value("LogsPath")
    static const STRING DefaultGeneralPropertyLogsPath;                 /// value("Logs/")

    /// Sets the maximum log file size (in kilobytes)
    static const STRING GeneralPropertyMaxLogFileSize;                  /// value("MaxLogFileSize")
    static const INT32 DefaultGeneralPropertyMaxLogFileSize;            /// value(64)

    /// Specifies whether the maximum log size restriction is enabled
    static const STRING GeneralPropertyMaxLogFileSizeEnabled;           /// value("MaxLogFileSizeEnabled")
    static const bool DefaultGeneralPropertyMaxLogFileSizeEnabled;      /// value(false)

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

    /// Defines whether to use the FDO Join APIs for Feature Joins where applicable and supported
    static const STRING FeatureServicePropertyUseFdoJoinOptimization;            /// value("UseFdoJoinOptimization")
    static const bool DefaultFeatureServicePropertyUseFdoJoinOptimization;       /// value(true)

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

EXTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// The remaining properties are log properties.  For each type of log, there is a "Parameters" property.
    /// These parameters may be used to customize a log and are listed below.  Note that not
    /// all of the parameters may be used with a particular log.  See the list of valid
    /// parameters for each log type below. The Parameters string is a list of
    /// comma-delimited parameter names.  Each log type has a different set of possible
    /// parameters.
    ///
    /// <table border="1" class="RuledTable">
    ///    <tr>
    ///        <th>Parameter</th>
    ///        <th>Description</th>
    ///    </tr>
    ///    <tr>
    ///        <td>ALL</td>
    ///        <td>Activates all valid parameters for log.</td>
    ///    </tr>
    ///    <tr>
    ///        <td>AKEY</td>
    ///        <td>Access Key used by the map layer.</td>
    ///    </tr>
    ///    <tr>
    ///        <td>CLIENT</td>
    ///        <td>The client and version that generated the request.</td>
    ///    </tr>
    ///    <tr>
    ///        <td>CLIENTID</td>
    ///        <td>A unique identifier for the client computer that generated the request.</td>
    ///    </tr>
    ///    <tr>
    ///        <td>DATASRC</td>
    ///        <td>Data source name.</td>
    ///    </tr>
    ///    <tr>
    ///        <td>ITEM</td>
    ///        <td>The type of request</td>
    ///    </tr>
    ///    <tr>
    ///        <td>LYRTYPE</td>
    ///        <td>Layer Type.</td>
    ///    </tr>
    ///    <tr>
    ///        <td>MLNAME</td>
    ///        <td>Map layer name.</td>
    ///    </tr>
    ///    <tr>
    ///        <td>MWIN</td>
    ///        <td>A composite item that generates entries for MWFURL, XMIN, YMIN, XMAX, and YMAX.</td>
    ///    </tr>
    ///    <tr>
    ///        <td>MWFURL</td>
    ///        <td>The URL of the map that generated the request.</td>
    ///    </tr>
    ///    <tr>
    ///        <td>NLAYER</td>
    ///        <td>The number of layers in the request.</td>
    ///    </tr>
    ///    <tr>
    ///        <td>NSCALOG</td>
    ///        <td>A composite item that generates several entries that conform to the NCSA/CERN log file standard.</td>
    ///    </tr>
    ///    <tr>
    ///        <td>PASSWORD</td>
    ///        <td>Password used by the map layer.</td>
    ///    </tr>
    ///    <tr>
    ///        <td>PREC</td>
    ///        <td>A number that indicates map data precision.</td>
    ///    </tr>
    ///    <tr>
    ///        <td>REQID</td>
    ///        <td>The request identification number.</td>
    ///    </tr>
    ///    <tr>
    ///        <td>REQTIME</td>
    ///        <td>Time taken to process the request in seconds.</td>
    ///    </tr>
    ///    <tr>
    ///        <td>SCALE</td>
    ///        <td>The current scale.</td>
    ///    </tr>
    ///    <tr>
    ///        <td>TABLE</td>
    ///        <td>Feature table name used by the map layer.</td>
    ///    </tr>
    ///    <tr>
    ///        <td>TRACKID</td>
    ///        <td>Customer tracking ID.</td>
    ///    </tr>
    ///    <tr>
    ///        <td>USER</td>
    ///        <td>User ID used by this map layer.</td>
    ///    </tr>
    ///    <tr>
    ///        <td>XMAX</td>
    ///        <td>The maximum X value of the area covered by the request.</td>
    ///    </tr>
    ///    <tr>
    ///        <td>YMAX</td>
    ///        <td>The maximum Y value of the area covered by the request.</td>
    ///    </tr>
    ///    <tr>
    ///        <td>XMIN</td>
    ///        <td>The minimum X value of the area covered by the request.</td>
    ///    </tr>
    ///    <tr>
    ///        <td>YMIN</td>
    ///        <td>The minimum Y value of the area covered by the request.</td>
    ///    </tr>
    /// </table>
    /// \n
    /// The valid Access log customization parameters are:
    /// CLIENT, CLIENTID, ITEM, NSCALOG, REQID, PREC, REQTIME, MWIN, MWFURL,
    /// XMAX, YMAX, XMIN, YMIN, NLAYER, SCALE, TRACKID.
    /// \n
    /// The valid Map Layer Access log customization parameters are:
    /// AKEY, CLIENT, CLIENTID, DATASRC, LYRTYPE, MLNAME, MWIN, MWFURL,
    /// NSCALOG, PASSWORD, PREC, REQID, REQTIME, SCALE, TABLE, TRACKID,
    /// USER, XMAX, YMAX, XMIN, YMIN.
    ///
    /// TODO:
    /// - Determine the valid log parameters for the various logs.
    ///

    /// ACCESS LOG PROPERTIES SECTION ------------------------------------------------------------------------------------

    /// Access Log properties.  This log records all requests to the MapGuide server.
    /// A record is created for each request after it has been completed.
    static const STRING AccessLogPropertiesSection;                 /// value("AccessLogProperties")

    /// Enables/disables the Access log
    static const STRING AccessLogPropertyEnabled;                   /// value("Enabled")
    static const bool DefaultAccessLogPropertyEnabled;              /// value(true)

    /// The Access log's file name.  NOTE: A log's filename may include the following special
    /// characters: %d,%m,%y, standing for date, month, year.  If one or all of these are used,
    /// the special char will be replaced by the actual calendar value when the log file is
    /// created.  A new log file will be created when the shortest time period has elapsed.
    /// That is, if the filename includes %d, a new log file will be created everyday.  If the
    /// filename contains both %m and %y, a new log file is created every month.
    static const STRING AccessLogPropertyFilename;                  /// value("Filename")
    static const STRING DefaultAccessLogPropertyFilename;           /// value("Access.log")

    /// The Access log's parameters
    static const STRING AccessLogPropertyParameters;                /// value("Parameters")
    static const STRING DefaultAccessLogPropertyParameters;         /// value("")

    /// ADMIN LOG PROPERTIES SECTION -------------------------------------------------------------------------------------

    /// Admin Log properties.  This log records all administrative operations (server status
    /// changes, configuration changes, etc.).
    static const STRING AdminLogPropertiesSection;                  /// value("AdminLogProperties")

    /// Enables/disables the Admin log
    static const STRING AdminLogPropertyEnabled;                    /// value("Enabled")
    static const bool DefaultAdminLogPropertyEnabled;               /// value(true)

    /// The Admin log's file name.  NOTE: As with the Access Log, the special characters: %d,
    /// %m, %y may be used in the filename.
    static const STRING AdminLogPropertyFilename;                   /// value("Filename")
    static const STRING DefaultAdminLogPropertyFilename;            /// value("Admin.log")

    /// The Admin log's parameters
    static const STRING AdminLogPropertyParameters;                 /// value("Parameters")
    static const STRING DefaultAdminLogPropertyParameters;          /// value("")

    /// AUTHENTICATION LOG PROPERTIES SECTION ----------------------------------------------------------------------------

    /// Authentication Log properties.  This log records the success or failure of all
    /// authentication operations.
    static const STRING AuthenticationLogPropertiesSection;         /// value("AuthenticationLogProperties")

    /// Enables/disables the Authentication log
    static const STRING AuthenticationLogPropertyEnabled;           /// value("Enabled")
    static const bool DefaultAuthenticationLogPropertyEnabled;      /// value(true)

    /// The Authentication log's file name.  NOTE: As with the Access Log, the special
    /// characters: %d, %m, %y may be used in the filename.
    static const STRING AuthenticationLogPropertyFilename;          /// value("Filename")
    static const STRING DefaultAuthenticationLogPropertyFilename;   /// value("Authentication.log")

    /// The Authentication log's parameters
    static const STRING AuthenticationLogPropertyParameters;        /// value("Parameters")
    static const STRING DefaultAuthenticationLogPropertyParameters; /// value("")

    /// ERROR LOG PROPERTIES SECTION -------------------------------------------------------------------------------------

    /// Error Log properties.  This log records all errors that occur during the MapGuide
    /// server's operation.
    static const STRING ErrorLogPropertiesSection;                  /// value("ErrorLogProperties")

    /// Enables/disables the Error log
    static const STRING ErrorLogPropertyEnabled;                    /// value("Enabled")
    static const bool DefaultErrorLogPropertyEnabled;               /// value(true)

    /// The Error log's file name.  NOTE: As with the Access Log, the special characters:
    /// %d, %m, %y may be used in the file name.
    static const STRING ErrorLogPropertyFilename;                   /// value("Filename")
    static const STRING DefaultErrorLogPropertyFilename;            /// value("Error.log")

    /// The Error log's parameters
    static const STRING ErrorLogPropertyParameters;                 /// value("Parameters")
    static const STRING DefaultErrorLogPropertyParameters;          /// value("")

    /// PERFORMANCE LOG PROPERTIES SECTION -------------------------------------------------------------------------------------

    /// Performance Log properties.  This log records all server statistical information.
    static const STRING PerformanceLogPropertiesSection;                  /// value("PerformanceLogProperties")

    /// Enables/disables the Performance log
    static const STRING PerformanceLogPropertyEnabled;                    /// value("Enabled")
    static const bool DefaultPerformanceLogPropertyEnabled;               /// value(false)

    /// The Performance log's file name.  NOTE: As with the Access Log, the special characters: %d,
    /// %m, %y may be used in the filename.
    static const STRING PerformanceLogPropertyFilename;                   /// value("Filename")
    static const STRING DefaultPerformanceLogPropertyFilename;            /// value("Performance.log")

    /// The Performance log's parameters
    static const STRING PerformanceLogPropertyParameters;                 /// value("Parameters")
    static const STRING DefaultPerformanceLogPropertyParameters;          /// value("")

    /// Sets the time duration (in seconds) between logging performance statistics
    static const STRING PerformanceLogPropertyInterval;                   /// value("Interval")
    static const INT32 DefaultPerformanceLogPropertyInterval;             /// value(300)

    /// SESSION LOG PROPERTIES SECTION -----------------------------------------------------------------------------------

    /// Session Log properties.  This log records state information for each connection
    /// to the MapGuide server, such as connection ID, connection, duration,
    /// user name, operations received, and operations processed.
    static const STRING SessionLogPropertiesSection;                /// value("SessionLogProperties")

    /// Enables/disables the Session log
    static const STRING SessionLogPropertyEnabled;                  /// value("Enabled")
    static const bool DefaultSessionLogPropertyEnabled;             /// value(true)

    /// The Session log's file name.  NOTE: As with the Access Log, the special
    /// characters: %d, %m, %y may be used in the filename.
    static const STRING SessionLogPropertyFilename;                 /// value("Filename")
    static const STRING DefaultSessionLogPropertyFilename;          /// value("Session.log")

    /// The Session log's parameters
    static const STRING SessionLogPropertyParameters;               /// value("Parameters")
    static const STRING DefaultSessionLogPropertyParameters;        /// value("")

    /// TRACE LOG PROPERTIES SECTION -------------------------------------------------------------------------------------

    /// Trace Log properties.  This log records the details for each request (logged
    /// in the access log).  For example, each request can include data from several
    /// maps or feature sets, and a detailed record would be created for each one.
    static const STRING TraceLogPropertiesSection;                  /// value("TraceLogProperties")

    /// Enables/disables the Trace log
    static const STRING TraceLogPropertyEnabled;                    /// value("Enabled")
    static const bool DefaultTraceLogPropertyEnabled;               /// value(true)

    /// The Trace log's file name.  NOTE: As with the Access Log, the special
    /// characters: %d, %m, %y may be used in the filename.
    static const STRING TraceLogPropertyFilename;                   /// value("Filename")
    static const STRING DefaultTraceLogPropertyFilename;            /// value("Trace.log")

    /// The Trace log's parameters
    static const STRING TraceLogPropertyParameters;                 /// value("Parameters")
    static const STRING DefaultTraceLogPropertyParameters;          /// value("")

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

    static const MgConfigValidationInfo sm_cviAccessLogProperties[];
    static const MgConfigValidationInfo sm_cviAdminLogProperties[];
    static const MgConfigValidationInfo sm_cviAuthenticationLogProperties[];
    static const MgConfigValidationInfo sm_cviErrorLogProperties[];
    static const MgConfigValidationInfo sm_cviPerformanceLogProperties[];
    static const MgConfigValidationInfo sm_cviSessionLogProperties[];
    static const MgConfigValidationInfo sm_cviTraceLogProperties[];

};
/// \endcond

#endif
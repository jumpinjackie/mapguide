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

#ifndef MGCONFIGPROPERTIES_H_
#define MGCONFIGPROPERTIES_H_

struct MgConfigValidationInfo;

/// \cond INTERNAL
//////////////////////////////////////////////////////////////////
/// \brief
/// Properties stored in an MgConfiguration object.  Note that the properties are
/// organized into Property Sections.  These sections may be used in the API to
/// set/get groups of configuration properties.
/// INTERNAL_ONLY:
class MG_MAPGUIDE_API MgConfigProperties
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

    /// Sets the maximum amount of time (in seconds) for connection idle activity before the socket connection is closed
    static const STRING GeneralPropertyConnectionTimeout;               /// value("ConnectionTimeout")
    static const INT32 DefaultGeneralPropertyConnectionTimeout;         /// value(120)

    /// Sets the time duration (in seconds) between timeout checks
    static const STRING GeneralPropertyConnectionTimerInterval;         /// value("ConnectionTimerInterval")
    static const INT32 DefaultGeneralPropertyConnectionTimerInterval;   /// value(60)

    /// Server's locale
    static const STRING GeneralPropertyLocale;                          /// value("Locale")
    static const STRING DefaultGeneralPropertyLocale;                   /// value("")

    /// Server's default message locale
    static const STRING GeneralPropertyDefaultMessageLocale;            /// value("DefaultMessageLocale")
    static const STRING DefaultGeneralPropertyDefaultMessageLocale;     /// value("en")

    /// Server's display name
    static const STRING GeneralPropertyDisplayName;                     /// value("DisplayName")
    static const STRING DefaultGeneralPropertyDisplayName;              /// value("")

    /// Fdo file path
    static const STRING GeneralPropertyFdoPath;                         /// value("FdoPath")
    static const STRING DefaultGeneralPropertyFdoPath;                  /// value("")

    /// Path of the license server
    static const STRING GeneralPropertyLicenseServerPath;               /// value("LicenseServerPath")
    static const STRING DefaultGeneralPropertyLicenseServerPath;        /// value("");

    /// Memory allocator debugging for Linux
    static const STRING GeneralPropertyLinuxMemDebug;               /// value("LinuxMemDebug")
    static const bool DefaultGeneralPropertyLinuxMemDebug;        /// value(false);

    /// Sets the field delimiter in the logs
    static const STRING GeneralPropertyLogsDelimiter;                   /// value("LogsDelimiter")
    static const STRING DefaultGeneralPropertyLogsDelimiter;            /// value("\t")

    /// Log detail level for services
    static const STRING GeneralPropertyLogsDetail;                      /// value("LogsDetail")
    static const STRING DefaultGeneralPropertyLogsDetail;               /// value("")

    /// Root folder for server's log files
    static const STRING GeneralPropertyLogsPath;                        /// value("LogsPath")
    static const STRING DefaultGeneralPropertyLogsPath;                 /// value("Logs/")

    /// Sets the Machine IP address
    static const STRING GeneralPropertyMachineIp;                       /// value("MachineIp")
    static const STRING DefaultGeneralPropertyMachineIp;                /// value("127.0.0.1")

    /// Sets the maximum log file size (in kilobytes)
    static const STRING GeneralPropertyMaxLogFileSize;                  /// value("MaxLogFileSize")
    static const INT32 DefaultGeneralPropertyMaxLogFileSize;            /// value(64)

    /// Specifies whether the maximum log size restriction is enabled
    static const STRING GeneralPropertyMaxLogFileSizeEnabled;           /// value("MaxLogFileSizeEnabled")
    static const bool DefaultGeneralPropertyMaxLogFileSizeEnabled;      /// value(false)

    /// Root folder for server's localization resources
    static const STRING GeneralPropertyResourcesPath;                   /// value("ResourcesPath")
    static const STRING DefaultGeneralPropertyResourcesPath;            /// value("Resources/")

    /// Product serial number
    static const STRING GeneralPropertySerialNumber;                    /// value("SerialNumber")
    static const STRING DefaultGeneralPropertySerialNumber;             /// value("");

    /// Sets the time duration (in seconds) between service registrations
    static const STRING GeneralPropertyServiceRegistrationTimerInterval;        /// value("ServiceRegistrationTimerInterval")
    static const INT32 DefaultGeneralPropertyServiceRegistrationTimerInterval;  /// value(60)

    /// TCP/IP Maximum Transmission Unit
    static const STRING GeneralPropertyTcpIpMtu;                        /// value("TcpIpMtu")
    static const INT32 DefaultGeneralPropertyTcpIpMtu;                  /// value(1460)

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

INTERNAL_API:

    static const STRING GeneralPropertyWorkerThreadPoolSize;            /// value("WorkerThreadPoolSize")
    static const INT32 DefaultGeneralPropertyWorkerThreadPoolSize;      /// value(1)

    /// Sets the failover retry time
    static const STRING GeneralPropertyFailoverRetryTime;               /// value("FailoverRetryTime")
    static const INT32 DefaultGeneralPropertyFailoverRetryTime;         /// value(60)

EXTERNAL_API:

    /// ADMINISTRATIVE CONNECTION PROPERTIES SECTION ---------------------------------------------------------------------

    /// Administrative Connection properties
    static const STRING AdministrativeConnectionPropertiesSection;              /// value("AdministrativeConnectionProperties")

    /// Administrator's email address
    static const STRING AdministrativeConnectionPropertyEmail;                  /// value("Email")
    static const STRING DefaultAdministrativeConnectionPropertyEmail;           /// value("")

    /// Maximum number of simultaneous administrative connections
    static const STRING AdministrativeConnectionPropertyMaxConnections;         /// value("MaxConnections")
    static const INT32 DefaultAdministrativeConnectionPropertyMaxConnections;   /// value(20)

    /// Sets the port on which administrative connections are made
    static const STRING AdministrativeConnectionPropertyPort;                   /// value("Port")
    static const INT32 DefaultAdministrativeConnectionPropertyPort;             /// value(5555)

    /// Sets the size of the administrative operation queue
    static const STRING AdministrativeConnectionPropertyQueueSize;              /// value("QueueSize")
    static const INT32 DefaultAdministrativeConnectionPropertyQueueSize;        /// value(10)

    /// Sets the number of worker threads available for administrative processing
    static const STRING AdministrativeConnectionPropertyThreadPoolSize;         /// value("ThreadPoolSize")
    static const INT32 DefaultAdministrativeConnectionPropertyThreadPoolSize;   /// value(5)

    /// CLIENT CONNECTION PROPERTIES SECTION -----------------------------------------------------------------------------

    /// Client Connection properties
    static const STRING ClientConnectionPropertiesSection;              /// value("ClientConnectionProperties")

    /// Maximum number of simultaneous client connections
    static const STRING ClientConnectionPropertyMaxConnections;         /// value("MaxConnections")
    static const INT32 DefaultClientConnectionPropertyMaxConnections;   /// value(100)

    /// Sets the port on which client connections are made
    static const STRING ClientConnectionPropertyPort;                   /// value("Port")
    static const INT32 DefaultClientConnectionPropertyPort;             /// value(4444)

    /// Sets the size of the client operation queue
    static const STRING ClientConnectionPropertyQueueSize;              /// value("QueueSize")
    static const INT32 DefaultClientConnectionPropertyQueueSize;        /// value(10)

    /// Sets the number of worker threads available for client processing
    static const STRING ClientConnectionPropertyThreadPoolSize;         /// value("ThreadPoolSize")
    static const INT32 DefaultClientConnectionPropertyThreadPoolSize;   /// value(10)

    /// SITE CONNECTION PROPERTIES SECTION -------------------------------------------------------------------------------

    /// Site Connection properties
    static const STRING SiteConnectionPropertiesSection;                /// value("SiteConnectionProperties")

    /// Sets the Site Connection IP address
    static const STRING SiteConnectionPropertyIpAddress;                /// value("IpAddress")
    static const STRING DefaultSiteConnectionPropertyIpAddress;         /// value("127.0.0.1")

    /// Maximum number of simultaneous site connections
    static const STRING SiteConnectionPropertyMaxConnections;           /// value("MaxConnections")
    static const INT32 DefaultSiteConnectionPropertyMaxConnections;     /// value(20)

    /// Sets the port on which site connections are made
    static const STRING SiteConnectionPropertyPort;                     /// value("Port")
    static const INT32 DefaultSiteConnectionPropertyPort;               /// value(3333)

    /// Sets the size of the site operation queue
    static const STRING SiteConnectionPropertyQueueSize;                /// value("QueueSize")
    static const INT32 DefaultSiteConnectionPropertyQueueSize;          /// value(10)

    /// Sets the number of worker threads available for site processing
    static const STRING SiteConnectionPropertyThreadPoolSize;           /// value("ThreadPoolSize")
    static const INT32 DefaultSiteConnectionPropertyThreadPoolSize;     /// value(5)

    /// HOST PROPERTIES SECTION ------------------------------------------------------------------------------------------

    /// Host properties
    static const STRING HostPropertiesSection;                          /// value("HostProperties")

    /// Enables/disables the Drawing Service.  Note that in V1.0 this service is only
    /// available on the Site Server and must be enabled there.
    static const STRING HostPropertyDrawingService;                     /// value("DrawingService")
    static const bool DefaultHostPropertyDrawingService;                /// value(false)

    /// Enables/disables the Feature Service.  Note that in V1.0 this service is only
    /// available on the Site Server and must be enabled there.
    static const STRING HostPropertyFeatureService;                     /// value("FeatureService")
    static const bool DefaultHostPropertyFeatureService;                /// value(false)

    /// Enables/disables the Kml Service.
    static const STRING HostPropertyKmlService;                         /// value("KmlService")
    static const bool DefaultHostPropertyKmlService;                    /// value(false)

    /// Enables/disables the Mapping Service
    static const STRING HostPropertyMappingService;                     /// value("MappingService")
    static const bool DefaultHostPropertyMappingService;                /// value(false)

    /// Enables/disables the Rendering Service
    static const STRING HostPropertyRenderingService;                   /// value("RenderingService")
    static const bool DefaultHostPropertyRenderingService;              /// value(false)

    /// Enables/disables the Resource Service.  Note that in V1.0 this service is only
    /// available on the Site Server and must be enabled there.
    static const STRING HostPropertyResourceService;                    /// value("ResourceService")
    static const bool DefaultHostPropertyResourceService;               /// value(false)

    /// Enables/disables the Profiling Service
    static const STRING HostPropertyProfilingService;                   /// value("ProfilingService")
    static const bool DefaultHostPropertyProfilingService;              /// value(false)

INTERNAL_API:

    /// Enables/disables the ServerAdmin Service (for internal use only).
    /// Note that, this service is available on each server.
    static const STRING HostPropertyServerAdminService;                 /// value("ServerAdminService")

EXTERNAL_API:

    /// Enables/disables the Site Service.  Note that in V1.0 this service is only
    /// available on the Site Server and must be enabled there.
    static const STRING HostPropertySiteService;                        /// value("SiteService")
    static const bool DefaultHostPropertySiteService;                   /// value(false)

    /// Enables/disables the Tile Service
    static const STRING HostPropertyTileService;                        /// value("TileService")
    static const bool DefaultHostPropertyTileService;                   /// value(false)

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
    static const STRING FeatureServicePropertyFDOConnectionTimeout;              /// value("FDOConnectionTimeout")
    static const INT32 DefaultFeatureServicePropertyFDOConnectionTimeout;        /// value(120)

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

    /// Sets the time duration (in seconds) between repository checkpoints
    static const STRING ResourceServicePropertyRepositoryCheckpointsTimerInterval;      /// value("RepositoryCheckpointsTimerInterval")
    static const INT32 DefaultResourceServicePropertyRepositoryCheckpointsTimerInterval;/// value(600)

    /// Sets the time duration (in seconds) between resource change notifications
    static const STRING ResourceServicePropertyResourceChangeTimerInterval;         /// value("ResourceChangeTimerInterval")
    static const INT32 DefaultResourceServicePropertyResourceChangeTimerInterval;   /// value(5)

    /// Sets the trash folder name
    static const STRING ResourceServicePropertyResourceDataFileTrashFolderName;         /// value("ResourceDataFileTrashFolderName")
    static const STRING DefaultResourceServicePropertyResourceDataFileTrashFolderName;  /// value("Trash")

    /// Sets the maximum number of resources with permission information to be cached
    static const STRING ResourceServicePropertyResourcePermissionCacheSize;             /// value("ResourcePermissionCacheSize")
    static const INT32 DefaultResourceServicePropertyResourcePermissionCacheSize;       /// value(1000)

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

    /// DB Environment PROPERTIES SECTION -------------------------------------------------------------------------------

    /// DB Environment properties. This is for setting DB Environment.
    static const STRING DBEnvironmentPropertiesSection;             /// value("DBEnvironmentProperties")

    /// The size for library cache size in MB
    static const STRING LibraryCacheSizeParameters;                 /// value("LibraryCacheSize")
    static const INT32 DefaultLibraryCacheSizeParameters;           /// value(32)

    /// The size for session cache size in MB
    static const STRING SessionCacheSizeParamters;                  /// value("SessionCacheSize")
    static const INT32 DefaultSessionCacheSizeParamters;            /// value(2)
         
    /// The size for library DB page in KB
    static const STRING DBPageSizeParameters;                       /// value("DBPageSize")
    static const INT32 DefaultDBPageSizeParameters;                 /// value(32)
          
    /// The size for library DBXML page in KB
    static const STRING DBXMLPageSizeParameters;                    /// value("DBXMLPageSize")
    static const INT32 DefaultDBXMLPageSizeParameters;              /// value(32)

    /// The size for library log buffer in MB
    static const STRING LibraryLogBufferSizeParameters;             /// value("LibraryLogBufferSize")
    static const INT32 DefaultLibraryLogBufferSizeParameters;       /// value(12)

    /// The size for session log buffer in MB
    static const STRING SessionLogBufferSizeParameters;             /// value("SessionLogBufferSize")
    static const INT32 DefaultSessionLogBufferSizeParameters;       /// value(1)

    /// The max number of DB transactions
    static const STRING DBMaxTransactionsParamters;                 /// value("DBMaxTransactions")
    static const INT32 DefaultDBMaxTransactionsParamters;           /// value(1000)

    /// The size for session DB page in KB
    static const STRING SessionDBPageSizeParameters;                /// value("SessionDBPageSize")
    static const INT32 DefaultSessionDBPageSizeParameters;          /// value(2)

    /// The size for session DBXML page in KB
    static const STRING SessionDBXMLPageSizeParameters;             /// value("SessionDBXMLPageSize")
    static const double DefaultSessionDBXMLPageSizeParameters;      /// value(0.5)

    /// The time out for lock and transaction in second
    static const STRING DBTimeoutParameters;                        /// value("DBTimeout")
    static const double DefaultDBTimeoutParameters;                 /// value(0.2)

    /// The max number of DB lockers
    static const STRING DBMaxLockersParameters;                     /// value("DBMaxLockers")
    static const INT32 DefaultDBMaxLockersParameters;               /// value(1000)

INTERNAL_API:

    /// SUPPORT SERVERS SECTION -----------------------------------------------------------------------------------------

    /// Support Servers section
    static const STRING SupportServersSection;                      /// value("SupportServers")

    /// Comma-delimited ID list of all support servers at a site
    static const STRING SupportServerIdentifiersProperty;           /// value("Identifiers")


    /// SUPPORT SERVER SECTION ------------------------------------------------------------------------------------------

    /// Support Server section (to be suffixed with an identifier)
    static const STRING SupportServerSection;                       /// value("SupportServer_")

    /// Name of a support server
    static const STRING SupportServerNameProperty;                  /// value("Name")

    /// Description of a support server
    static const STRING SupportServerDescriptionProperty;           /// value("Description")

    /// IP address of a support server
    static const STRING SupportServerAddressProperty;               /// value("IpAddress")

EXTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////
    /// Web Configuration Validation Information Maps
    ///

    /// AGENT PROPERTIES SECTION -----------------------------------------------------------------------------------------

    /// Agent properties
    static const STRING AgentPropertiesSection;                         /// value("AgentProperties")

    /// Number of seconds to wait before processing each request value
    static const STRING AgentDebugPause;                                /// value("DebugPause")
    static const INT32 DefaultAgentDebugPause;                          /// value(0)

    /// Disables Http operations used for Authoring
    static const STRING AgentDisableAuthoring;                          /// value("DisableAuthoring")

    /// Disables Http operations used for OGC Wfs
    static const STRING AgentDisableWfs;                                /// value("DisableWfs")

    /// Disables Http operations used for OGC Wms
    static const STRING AgentDisableWms;                                /// value("DisableWms")

    /// Agent error log enabled
    static const STRING AgentErrorLogEnabled;                           /// value("ErrorLogEnabled")
    static const bool DefaultAgentErrorLogEnabled;                      /// value(false)

    /// Agent error log filename
    static const STRING AgentErrorLogFilename;                          /// value("ErrorLogFilename")
    static const STRING DefaultAgentErrorLogFilename;                   /// value("Error.log")

    /// Agent request log enabled
    static const STRING AgentRequestLogEnabled;                         /// value("RequestLogEnabled")
    static const bool DefaultAgentRequestLogEnabled;                    /// value(false)

    /// Agent error log filename
    static const STRING AgentRequestLogFilename;                        /// value("RequestLogFilename")
    static const STRING DefaultAgentRequestLogFilename;                 /// value("Request.log")

    /// OGC PROPERTIES SECTION -------------------------------------------------------------------------------------------

    /// Ogc properties
    static const STRING OgcPropertiesSection;                           /// value("OgcProperties")

    /// Password for Wfs requests
    static const STRING WfsPassword;                                    /// value("WfsPassword")

    /// Password for Wms requests
    static const STRING WmsPassword;                                    /// value("WmsPassword")

    /// Enable OGC CITE Test for WFS
    static const STRING CITEWfsEnabled;                                 /// value("CITEWfsEnabled")
    static const bool DefaultCITEWfsEnabled;                            /// value(false)

    /// Enable OGC CITE Test for WMS 
    static const STRING CITEWmsEnabled;                                 /// value("CITEWmsEnabled") 
    static const bool DefaultCITEWmsEnabled;                            /// value(false)

    /// WEB APPLICATION PROPERTIES SECTION -------------------------------------------------------------------------------------------

    /// Viewer framework properties
    static const STRING WebApplicationPropertiesSection;                /// value("WebApplicationProperties")

    /// Root folder for templates
    static const STRING TemplateRootFolder;                             /// value("TemplateRootFolder")

    /// Folder containing widget info
    static const STRING WidgetInfoFolder;                               /// value("WidgetInfoFolder")

    /// Folder containing container info
    static const STRING ContainerInfoFolder;                            /// value("ContainerInfoFolder")

public:

    // Common Configuration Validation Information Maps
    static const MgConfigValidationInfo sm_cviGeneralProperties[];
    static const MgConfigValidationInfo sm_cviAdministrativeConnectionProperties[];
    static const MgConfigValidationInfo sm_cviClientConnectionProperties[];
    static const MgConfigValidationInfo sm_cviSiteConnectionProperties[];

    // Server Configuration Validation Information Maps
    static const MgConfigValidationInfo sm_cviHostProperties[];
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
    static const MgConfigValidationInfo sm_cviDBEnvironmentProperties[];

    // Web Configuration Validation Information Maps
    static const MgConfigValidationInfo sm_cviAgentProperties[];
    static const MgConfigValidationInfo sm_cviOgcProperties[];
};
/// \endcond

#endif

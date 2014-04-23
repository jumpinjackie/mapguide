#ifndef DESKTOP_CLASSID_H
#define DESKTOP_CLASSID_H

// Predefined ranges for each service
#define MAPGUIDE_DESKTOP_EXCEPTION_ID           40000
#define MAPGUIDE_DESKTOP_MAPLAYER_ID            40500
#define MAPGUIDE_DESKTOP_SERVICE_ID             40600
#define MAPGUIDE_DESKTOP_DRAWINGSERVICE_ID      40700
#define MAPGUIDE_DESKTOP_FEATURESERVICE_ID      40800
#define MAPGUIDE_DESKTOP_MAPPINGSERVICE_ID      40900
#define MAPGUIDE_DESKTOP_RENDERINGSERVICE_ID    41000
#define MAPGUIDE_DESKTOP_RESOURCESERVICE_ID     41100
#define MAPGUIDE_DESKTOP_TILESERVICE_ID         41200
#define MAPGUIDE_DESKTOP_KMLSERVICE_ID          41300
#define MAPGUIDE_DESKTOP_PROFILING_SERVICE_ID   41400

// MapLayer API
#define MapGuide_Desktop_MapLayer_Map           MAPGUIDE_DESKTOP_MAPLAYER_ID+0
#define MapGuide_Desktop_MapLayer_Layer         MAPGUIDE_DESKTOP_MAPLAYER_ID+1
#define MapGuide_Desktop_MapLayer_Selection     MAPGUIDE_DESKTOP_MAPLAYER_ID+2
#define MapGuide_Desktop_MapLayer_Controller    MAPGUIDE_DESKTOP_MAPLAYER_ID+3
#define MapGuide_Desktop_MapLayer_AdvController MAPGUIDE_DESKTOP_MAPLAYER_ID+4
#define MapGuide_Desktop_MapLayer_ZcvController MAPGUIDE_DESKTOP_MAPLAYER_ID+5

// MapGuide Exceptions
#define MapGuide_Desktop_Exception_AuthenticationFailedException              MAPGUIDE_DESKTOP_EXCEPTION_ID+0
#define MapGuide_Desktop_Exception_ConnectionFailedException                  MAPGUIDE_DESKTOP_EXCEPTION_ID+1
#define MapGuide_Desktop_Exception_ConnectionNotOpenException                 MAPGUIDE_DESKTOP_EXCEPTION_ID+2
#define MapGuide_Desktop_Exception_DbException                                MAPGUIDE_DESKTOP_EXCEPTION_ID+3
#define MapGuide_Desktop_Exception_DbXmlException                             MAPGUIDE_DESKTOP_EXCEPTION_ID+4
#define MapGuide_Desktop_Exception_DuplicateGroupException                    MAPGUIDE_DESKTOP_EXCEPTION_ID+5
#define MapGuide_Desktop_Exception_DuplicateNameException                     MAPGUIDE_DESKTOP_EXCEPTION_ID+6
#define MapGuide_Desktop_Exception_DuplicateParameterException                MAPGUIDE_DESKTOP_EXCEPTION_ID+7
#define MapGuide_Desktop_Exception_DuplicateRepositoryException               MAPGUIDE_DESKTOP_EXCEPTION_ID+8
#define MapGuide_Desktop_Exception_DuplicateRoleException                     MAPGUIDE_DESKTOP_EXCEPTION_ID+9
#define MapGuide_Desktop_Exception_DuplicateServerException                   MAPGUIDE_DESKTOP_EXCEPTION_ID+10
#define MapGuide_Desktop_Exception_DuplicateSessionException                  MAPGUIDE_DESKTOP_EXCEPTION_ID+11
#define MapGuide_Desktop_Exception_DuplicateUserException                     MAPGUIDE_DESKTOP_EXCEPTION_ID+12
#define MapGuide_Desktop_Exception_DwfException                               MAPGUIDE_DESKTOP_EXCEPTION_ID+13
#define MapGuide_Desktop_Exception_DwfSectionNotFoundException                MAPGUIDE_DESKTOP_EXCEPTION_ID+14
#define MapGuide_Desktop_Exception_DwfSectionResourceNotFoundException        MAPGUIDE_DESKTOP_EXCEPTION_ID+15
#define MapGuide_Desktop_Exception_EndOfStreamException                       MAPGUIDE_DESKTOP_EXCEPTION_ID+16
#define MapGuide_Desktop_Exception_EvaluationExpiredException                 MAPGUIDE_DESKTOP_EXCEPTION_ID+17
#define MapGuide_Desktop_Exception_GroupNotFoundException                     MAPGUIDE_DESKTOP_EXCEPTION_ID+18
#define MapGuide_Desktop_Exception_InvalidDwfPackageException                 MAPGUIDE_DESKTOP_EXCEPTION_ID+19
#define MapGuide_Desktop_Exception_InvalidDwfSectionException                 MAPGUIDE_DESKTOP_EXCEPTION_ID+20
#define MapGuide_Desktop_Exception_InvalidFeatureSourceException              MAPGUIDE_DESKTOP_EXCEPTION_ID+21
#define MapGuide_Desktop_Exception_InvalidIpAddressException                  MAPGUIDE_DESKTOP_EXCEPTION_ID+22
#define MapGuide_Desktop_Exception_InvalidLicenseException                    MAPGUIDE_DESKTOP_EXCEPTION_ID+23
#define MapGuide_Desktop_Exception_InvalidLogEntryException                   MAPGUIDE_DESKTOP_EXCEPTION_ID+24
#define MapGuide_Desktop_Exception_InvalidOperationVersionException           MAPGUIDE_DESKTOP_EXCEPTION_ID+25
#define MapGuide_Desktop_Exception_InvalidPasswordException                   MAPGUIDE_DESKTOP_EXCEPTION_ID+26
#define MapGuide_Desktop_Exception_InvalidPrintLayoutFontSizeUnitsException   MAPGUIDE_DESKTOP_EXCEPTION_ID+27
#define MapGuide_Desktop_Exception_InvalidPrintLayoutPositionUnitsException   MAPGUIDE_DESKTOP_EXCEPTION_ID+28
#define MapGuide_Desktop_Exception_InvalidPrintLayoutSizeUnitsException       MAPGUIDE_DESKTOP_EXCEPTION_ID+29
#define MapGuide_Desktop_Exception_InvalidSerialNumberException               MAPGUIDE_DESKTOP_EXCEPTION_ID+30
#define MapGuide_Desktop_Exception_InvalidServerNameException                 MAPGUIDE_DESKTOP_EXCEPTION_ID+31
#define MapGuide_Desktop_Exception_LicenseException                           MAPGUIDE_DESKTOP_EXCEPTION_ID+32
#define MapGuide_Desktop_Exception_LicenseExpiredException                    MAPGUIDE_DESKTOP_EXCEPTION_ID+33
#define MapGuide_Desktop_Exception_LogOpenFailedException                     MAPGUIDE_DESKTOP_EXCEPTION_ID+34
#define MapGuide_Desktop_Exception_OperationProcessingException               MAPGUIDE_DESKTOP_EXCEPTION_ID+35
#define MapGuide_Desktop_Exception_ParameterNotFoundException                 MAPGUIDE_DESKTOP_EXCEPTION_ID+36
#define MapGuide_Desktop_Exception_PathTooLongException                       MAPGUIDE_DESKTOP_EXCEPTION_ID+37
#define MapGuide_Desktop_Exception_PermissionDeniedException                  MAPGUIDE_DESKTOP_EXCEPTION_ID+38
#define MapGuide_Desktop_Exception_PortNotAvailableException                  MAPGUIDE_DESKTOP_EXCEPTION_ID+39
#define MapGuide_Desktop_Exception_PrintToScaleModeNotSelectedException       MAPGUIDE_DESKTOP_EXCEPTION_ID+40
#define MapGuide_Desktop_Exception_RepositoryCreationFailedException          MAPGUIDE_DESKTOP_EXCEPTION_ID+41
#define MapGuide_Desktop_Exception_RepositoryNotFoundException                MAPGUIDE_DESKTOP_EXCEPTION_ID+42
#define MapGuide_Desktop_Exception_RepositoryNotOpenException                 MAPGUIDE_DESKTOP_EXCEPTION_ID+43
#define MapGuide_Desktop_Exception_RepositoryOpenFailedException              MAPGUIDE_DESKTOP_EXCEPTION_ID+44
#define MapGuide_Desktop_Exception_RoleNotFoundException                      MAPGUIDE_DESKTOP_EXCEPTION_ID+45
#define MapGuide_Desktop_Exception_ServerNotFoundException                    MAPGUIDE_DESKTOP_EXCEPTION_ID+46
#define MapGuide_Desktop_Exception_ServerNotOnlineException                   MAPGUIDE_DESKTOP_EXCEPTION_ID+47
#define MapGuide_Desktop_Exception_SessionExpiredException                    MAPGUIDE_DESKTOP_EXCEPTION_ID+48
#define MapGuide_Desktop_Exception_UnauthorizedAccessException                MAPGUIDE_DESKTOP_EXCEPTION_ID+49
#define MapGuide_Desktop_Exception_UriFormatException                         MAPGUIDE_DESKTOP_EXCEPTION_ID+50
#define MapGuide_Desktop_Exception_StylizeLayerFailedException                MAPGUIDE_DESKTOP_EXCEPTION_ID+51
#define MapGuide_Desktop_Exception_SessionNotFoundException                   MAPGUIDE_DESKTOP_EXCEPTION_ID+52
#define MapGuide_Desktop_Exception_UnsupportedProviderThreadModelException    MAPGUIDE_DESKTOP_EXCEPTION_ID+53
#define MapGuide_Desktop_Exception_AllProviderConnectionsUsedException        MAPGUIDE_DESKTOP_EXCEPTION_ID+54
#define MapGuide_Desktop_Exception_RasterTransformationNotSupportedException  MAPGUIDE_DESKTOP_EXCEPTION_ID+55
#define MapGuide_Desktop_Exception_AliasNotFoundException                     MAPGUIDE_DESKTOP_EXCEPTION_ID+56


//Basic Service functionality
#define MapGuide_Desktop_ServiceFactory                         MAPGUIDE_DESKTOP_SERVICE_ID+0
//#define MapGuide_Desktop_Service_ServerConnection               MAPGUIDE_DESKTOP_SERVICE_ID+0
//#define MapGuide_Desktop_Service_SiteConnection                 MAPGUIDE_DESKTOP_SERVICE_ID+1
//#define MapGuide_Desktop_Service_ConnectionProperties           MAPGUIDE_DESKTOP_SERVICE_ID+2
//#define MapGuide_Desktop_Service_ServerInformation              MAPGUIDE_DESKTOP_SERVICE_ID+3
//#define MapGuide_Desktop_Service_PackageStatusInformation       MAPGUIDE_DESKTOP_SERVICE_ID+4
//#define MapGuide_Desktop_Service_Site                           MAPGUIDE_DESKTOP_SERVICE_ID+5
//#define MapGuide_Desktop_Service_UserInformation                MAPGUIDE_DESKTOP_SERVICE_ID+6
//#define MapGuide_Desktop_Service_ServerAdmin                    MAPGUIDE_DESKTOP_SERVICE_ID+7
//#define MapGuide_Desktop_Service_SiteInfo                       MAPGUIDE_DESKTOP_SERVICE_ID+8
//#define MapGuide_Desktop_Service_SqlResult                      MAPGUIDE_DESKTOP_SERVICE_ID+9

#define MapGuide_Desktop_FeatureService_FeatureService              MAPGUIDE_DESKTOP_FEATURESERVICE_ID+0

// Drawing Service
#define MapGuide_Desktop_DrawingService_DrawingService          MAPGUIDE_DESKTOP_DRAWINGSERVICE_ID+0

// Mapping Service
#define MapGuide_Desktop_MappingService_MappingService                      MAPGUIDE_DESKTOP_MAPPINGSERVICE_ID+0
#define MapGuide_Desktop_MappingService_DwfVersion                          MAPGUIDE_DESKTOP_MAPPINGSERVICE_ID+1
#define MapGuide_Desktop_MappingService_PlotSpecification                   MAPGUIDE_DESKTOP_MAPPINGSERVICE_ID+2
#define MapGuide_Desktop_MappingService_PrintLayout                         MAPGUIDE_DESKTOP_MAPPINGSERVICE_ID+3
#define MapGuide_Desktop_MappingService_Layout                              MAPGUIDE_DESKTOP_MAPPINGSERVICE_ID+4
#define MapGuide_Desktop_MappingService_MapPlot                             MAPGUIDE_DESKTOP_MAPPINGSERVICE_ID+5
#define MapGuide_Desktop_MappingService_MapPlotCollection                   MAPGUIDE_DESKTOP_MAPPINGSERVICE_ID+6

// Rendering Service
#define MapGuide_Desktop_RenderingService_RenderingService                  MAPGUIDE_DESKTOP_RENDERINGSERVICE_ID+0
#define MapGuide_Desktop_RenderingService_FeatureInformation                MAPGUIDE_DESKTOP_RENDERINGSERVICE_ID+1
#define MapGuide_Desktop_RenderingService_RenderingOptions                  MAPGUIDE_DESKTOP_RENDERINGSERVICE_ID+2

// Resource Service
#define MapGuide_Desktop_ResourceService_ResourceService                    MAPGUIDE_DESKTOP_RESOURCESERVICE_ID+0


// Tile Service
#define MapGuide_Desktop_TileService_TileService                            MAPGUIDE_DESKTOP_TILESERVICE_ID+0

// KML Service
#define MapGuide_Desktop_KmlService_KmlService                              MAPGUIDE_DESKTOP_KMLSERVICE_ID+0

// Profiling
#define MapGuide_Desktop_ProfilingService_ProfilingService					MAPGUIDE_DESKTOP_PROFILING_SERVICE_ID+0

#endif
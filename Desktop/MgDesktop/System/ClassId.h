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

// MapLayer API
#define MapGuide_Desktop_MapLayer_Map           MAPGUIDE_DESKTOP_MAPLAYER_ID+0
#define MapGuide_Desktop_MapLayer_Layer         MAPGUIDE_DESKTOP_MAPLAYER_ID+1
#define MapGuide_Desktop_MapLayer_Selection     MAPGUIDE_DESKTOP_MAPLAYER_ID+2
#define MapGuide_Desktop_MapLayer_Controller    MAPGUIDE_DESKTOP_MAPLAYER_ID+3
#define MapGuide_Desktop_MapLayer_AdvController MAPGUIDE_DESKTOP_MAPLAYER_ID+4
#define MapGuide_Desktop_MapLayer_ZcvController MAPGUIDE_DESKTOP_MAPLAYER_ID+5

// MapGuide Exceptions
#define MapGuide_Desktop_Exception_MgAuthenticationFailedException              MAPGUIDE_DESKTOP_EXCEPTION_ID+0
#define MapGuide_Desktop_Exception_MgConnectionFailedException                  MAPGUIDE_DESKTOP_EXCEPTION_ID+1
#define MapGuide_Desktop_Exception_MgConnectionNotOpenException                 MAPGUIDE_DESKTOP_EXCEPTION_ID+2
#define MapGuide_Desktop_Exception_MgDbException                                MAPGUIDE_DESKTOP_EXCEPTION_ID+3
#define MapGuide_Desktop_Exception_MgDbXmlException                             MAPGUIDE_DESKTOP_EXCEPTION_ID+4
#define MapGuide_Desktop_Exception_MgDuplicateGroupException                    MAPGUIDE_DESKTOP_EXCEPTION_ID+5
#define MapGuide_Desktop_Exception_MgDuplicateNameException                     MAPGUIDE_DESKTOP_EXCEPTION_ID+6
#define MapGuide_Desktop_Exception_MgDuplicateParameterException                MAPGUIDE_DESKTOP_EXCEPTION_ID+7
#define MapGuide_Desktop_Exception_MgDuplicateRepositoryException               MAPGUIDE_DESKTOP_EXCEPTION_ID+8
#define MapGuide_Desktop_Exception_MgDuplicateRoleException                     MAPGUIDE_DESKTOP_EXCEPTION_ID+9
#define MapGuide_Desktop_Exception_MgDuplicateServerException                   MAPGUIDE_DESKTOP_EXCEPTION_ID+10
#define MapGuide_Desktop_Exception_MgDuplicateSessionException                  MAPGUIDE_DESKTOP_EXCEPTION_ID+11
#define MapGuide_Desktop_Exception_MgDuplicateUserException                     MAPGUIDE_DESKTOP_EXCEPTION_ID+12
#define MapGuide_Desktop_Exception_MgDwfException                               MAPGUIDE_DESKTOP_EXCEPTION_ID+13
#define MapGuide_Desktop_Exception_MgDwfSectionNotFoundException                MAPGUIDE_DESKTOP_EXCEPTION_ID+14
#define MapGuide_Desktop_Exception_MgDwfSectionResourceNotFoundException        MAPGUIDE_DESKTOP_EXCEPTION_ID+15
#define MapGuide_Desktop_Exception_MgEndOfStreamException                       MAPGUIDE_DESKTOP_EXCEPTION_ID+16
#define MapGuide_Desktop_Exception_MgEvaluationExpiredException                 MAPGUIDE_DESKTOP_EXCEPTION_ID+17
#define MapGuide_Desktop_Exception_MgGroupNotFoundException                     MAPGUIDE_DESKTOP_EXCEPTION_ID+18
#define MapGuide_Desktop_Exception_MgInvalidDwfPackageException                 MAPGUIDE_DESKTOP_EXCEPTION_ID+19
#define MapGuide_Desktop_Exception_MgInvalidDwfSectionException                 MAPGUIDE_DESKTOP_EXCEPTION_ID+20
#define MapGuide_Desktop_Exception_MgInvalidFeatureSourceException              MAPGUIDE_DESKTOP_EXCEPTION_ID+21
#define MapGuide_Desktop_Exception_MgInvalidIpAddressException                  MAPGUIDE_DESKTOP_EXCEPTION_ID+22
#define MapGuide_Desktop_Exception_MgInvalidLicenseException                    MAPGUIDE_DESKTOP_EXCEPTION_ID+23
#define MapGuide_Desktop_Exception_MgInvalidLogEntryException                   MAPGUIDE_DESKTOP_EXCEPTION_ID+24
#define MapGuide_Desktop_Exception_MgInvalidOperationVersionException           MAPGUIDE_DESKTOP_EXCEPTION_ID+25
#define MapGuide_Desktop_Exception_MgInvalidPasswordException                   MAPGUIDE_DESKTOP_EXCEPTION_ID+26
#define MapGuide_Desktop_Exception_MgInvalidPrintLayoutFontSizeUnitsException   MAPGUIDE_DESKTOP_EXCEPTION_ID+27
#define MapGuide_Desktop_Exception_MgInvalidPrintLayoutPositionUnitsException   MAPGUIDE_DESKTOP_EXCEPTION_ID+28
#define MapGuide_Desktop_Exception_MgInvalidPrintLayoutSizeUnitsException       MAPGUIDE_DESKTOP_EXCEPTION_ID+29
#define MapGuide_Desktop_Exception_MgInvalidSerialNumberException               MAPGUIDE_DESKTOP_EXCEPTION_ID+30
#define MapGuide_Desktop_Exception_MgInvalidServerNameException                 MAPGUIDE_DESKTOP_EXCEPTION_ID+31
#define MapGuide_Desktop_Exception_MgLicenseException                           MAPGUIDE_DESKTOP_EXCEPTION_ID+32
#define MapGuide_Desktop_Exception_MgLicenseExpiredException                    MAPGUIDE_DESKTOP_EXCEPTION_ID+33
#define MapGuide_Desktop_Exception_MgLogOpenFailedException                     MAPGUIDE_DESKTOP_EXCEPTION_ID+34
#define MapGuide_Desktop_Exception_MgOperationProcessingException               MAPGUIDE_DESKTOP_EXCEPTION_ID+35
#define MapGuide_Desktop_Exception_MgParameterNotFoundException                 MAPGUIDE_DESKTOP_EXCEPTION_ID+36
#define MapGuide_Desktop_Exception_MgPathTooLongException                       MAPGUIDE_DESKTOP_EXCEPTION_ID+37
#define MapGuide_Desktop_Exception_MgPermissionDeniedException                  MAPGUIDE_DESKTOP_EXCEPTION_ID+38
#define MapGuide_Desktop_Exception_MgPortNotAvailableException                  MAPGUIDE_DESKTOP_EXCEPTION_ID+39
#define MapGuide_Desktop_Exception_MgPrintToScaleModeNotSelectedException       MAPGUIDE_DESKTOP_EXCEPTION_ID+40
#define MapGuide_Desktop_Exception_MgRepositoryCreationFailedException          MAPGUIDE_DESKTOP_EXCEPTION_ID+41
#define MapGuide_Desktop_Exception_MgRepositoryNotFoundException                MAPGUIDE_DESKTOP_EXCEPTION_ID+42
#define MapGuide_Desktop_Exception_MgRepositoryNotOpenException                 MAPGUIDE_DESKTOP_EXCEPTION_ID+43
#define MapGuide_Desktop_Exception_MgRepositoryOpenFailedException              MAPGUIDE_DESKTOP_EXCEPTION_ID+44
#define MapGuide_Desktop_Exception_MgRoleNotFoundException                      MAPGUIDE_DESKTOP_EXCEPTION_ID+45
#define MapGuide_Desktop_Exception_MgServerNotFoundException                    MAPGUIDE_DESKTOP_EXCEPTION_ID+46
#define MapGuide_Desktop_Exception_MgServerNotOnlineException                   MAPGUIDE_DESKTOP_EXCEPTION_ID+47
#define MapGuide_Desktop_Exception_MgSessionExpiredException                    MAPGUIDE_DESKTOP_EXCEPTION_ID+48
#define MapGuide_Desktop_Exception_MgUnauthorizedAccessException                MAPGUIDE_DESKTOP_EXCEPTION_ID+49
#define MapGuide_Desktop_Exception_MgUriFormatException                         MAPGUIDE_DESKTOP_EXCEPTION_ID+50
#define MapGuide_Desktop_Exception_MgStylizeLayerFailedException                MAPGUIDE_DESKTOP_EXCEPTION_ID+51
#define MapGuide_Desktop_Exception_MgSessionNotFoundException                   MAPGUIDE_DESKTOP_EXCEPTION_ID+52
#define MapGuide_Desktop_Exception_MgUnsupportedProviderThreadModelException    MAPGUIDE_DESKTOP_EXCEPTION_ID+53
#define MapGuide_Desktop_Exception_MgAllProviderConnectionsUsedException        MAPGUIDE_DESKTOP_EXCEPTION_ID+54
#define MapGuide_Desktop_Exception_MgRasterTransformationNotSupportedException  MAPGUIDE_DESKTOP_EXCEPTION_ID+55
#define MapGuide_Desktop_Exception_MgAliasNotFoundException                     MAPGUIDE_DESKTOP_EXCEPTION_ID+56


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
//#define MapGuide_Desktop_MappingService_MappingService                      MAPGUIDE_DESKTOP_MAPPINGSERVICE_ID+0
#define MapGuide_Desktop_MappingService_DwfVersion                          MAPGUIDE_DESKTOP_MAPPINGSERVICE_ID+1
#define MapGuide_Desktop_MappingService_PlotSpecification                   MAPGUIDE_DESKTOP_MAPPINGSERVICE_ID+2
#define MapGuide_Desktop_MappingService_PrintLayout                         MAPGUIDE_DESKTOP_MAPPINGSERVICE_ID+3
#define MapGuide_Desktop_MappingService_Layout                              MAPGUIDE_DESKTOP_MAPPINGSERVICE_ID+4
#define MapGuide_Desktop_MappingService_MapPlot                             MAPGUIDE_DESKTOP_MAPPINGSERVICE_ID+5
#define MapGuide_Desktop_MappingService_MapPlotCollection                   MAPGUIDE_DESKTOP_MAPPINGSERVICE_ID+6

// Rendering Service
#define MapGuide_Desktop_RenderingService_RenderingService                  MAPGUIDE_DESKTOP_RENDERINGSERVICE_ID+0
//#define MapGuide_Desktop_RenderingService_FeatureInformation                MAPGUIDE_DESKTOP_RENDERINGSERVICE_ID+1
#define MapGuide_Desktop_RenderingService_RenderingOptions                  MAPGUIDE_DESKTOP_RENDERINGSERVICE_ID+2

// Resource Service
#define MapGuide_Desktop_ResourceService_ResourceService                    MAPGUIDE_DESKTOP_RESOURCESERVICE_ID+0


// Tile Service
#define MapGuide_Desktop_TileService_TileService                            MAPGUIDE_DESKTOP_TILESERVICE_ID+0

// Tile Service
#define MapGuide_Desktop_KmlService_KmlService                              MAPGUIDE_DESKTOP_KMLSERVICE_ID+0


#endif
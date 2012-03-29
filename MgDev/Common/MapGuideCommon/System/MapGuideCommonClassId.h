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

#ifndef MAPGUIDE_COMMON_CLASSID_H_
#define MAPGUIDE_COMMON_CLASSID_H_

// Predefined ranges for each service
#define MAPGUIDE_EXCEPTION_ID           30000
#define MAPGUIDE_MAPLAYER_ID            30500
#define MAPGUIDE_SERVICE_ID             30600
#define MAPGUIDE_DRAWINGSERVICE_ID      30700
#define MAPGUIDE_FEATURESERVICE_ID      30800
#define MAPGUIDE_MAPPINGSERVICE_ID      30900
#define MAPGUIDE_RENDERINGSERVICE_ID    31000
#define MAPGUIDE_RESOURCESERVICE_ID     31100
#define MAPGUIDE_TILESERVICE_ID         31200
#define MAPGUIDE_KMLSERVICE_ID          31300
#define MAPGUIDE_PROFILINGSERVICE_ID    31400


// MapGuide Exceptions
#define MapGuide_Exception_MgAuthenticationFailedException              MAPGUIDE_EXCEPTION_ID+0
#define MapGuide_Exception_MgConnectionFailedException                  MAPGUIDE_EXCEPTION_ID+1
#define MapGuide_Exception_MgConnectionNotOpenException                 MAPGUIDE_EXCEPTION_ID+2
#define MapGuide_Exception_MgDbException                                MAPGUIDE_EXCEPTION_ID+3
#define MapGuide_Exception_MgDbXmlException                             MAPGUIDE_EXCEPTION_ID+4
#define MapGuide_Exception_MgDuplicateGroupException                    MAPGUIDE_EXCEPTION_ID+5
#define MapGuide_Exception_MgDuplicateNameException                     MAPGUIDE_EXCEPTION_ID+6
#define MapGuide_Exception_MgDuplicateParameterException                MAPGUIDE_EXCEPTION_ID+7
#define MapGuide_Exception_MgDuplicateRepositoryException               MAPGUIDE_EXCEPTION_ID+8
#define MapGuide_Exception_MgDuplicateRoleException                     MAPGUIDE_EXCEPTION_ID+9
#define MapGuide_Exception_MgDuplicateServerException                   MAPGUIDE_EXCEPTION_ID+10
#define MapGuide_Exception_MgDuplicateSessionException                  MAPGUIDE_EXCEPTION_ID+11
#define MapGuide_Exception_MgDuplicateUserException                     MAPGUIDE_EXCEPTION_ID+12
#define MapGuide_Exception_MgDwfException                               MAPGUIDE_EXCEPTION_ID+13
#define MapGuide_Exception_MgDwfSectionNotFoundException                MAPGUIDE_EXCEPTION_ID+14
#define MapGuide_Exception_MgDwfSectionResourceNotFoundException        MAPGUIDE_EXCEPTION_ID+15
#define MapGuide_Exception_MgEndOfStreamException                       MAPGUIDE_EXCEPTION_ID+16
#define MapGuide_Exception_MgEvaluationExpiredException                 MAPGUIDE_EXCEPTION_ID+17
#define MapGuide_Exception_MgGroupNotFoundException                     MAPGUIDE_EXCEPTION_ID+18
#define MapGuide_Exception_MgInvalidDwfPackageException                 MAPGUIDE_EXCEPTION_ID+19
#define MapGuide_Exception_MgInvalidDwfSectionException                 MAPGUIDE_EXCEPTION_ID+20
#define MapGuide_Exception_MgInvalidFeatureSourceException              MAPGUIDE_EXCEPTION_ID+21
#define MapGuide_Exception_MgInvalidIpAddressException                  MAPGUIDE_EXCEPTION_ID+22
#define MapGuide_Exception_MgInvalidLicenseException                    MAPGUIDE_EXCEPTION_ID+23
#define MapGuide_Exception_MgInvalidLogEntryException                   MAPGUIDE_EXCEPTION_ID+24
#define MapGuide_Exception_MgInvalidOperationVersionException           MAPGUIDE_EXCEPTION_ID+25
#define MapGuide_Exception_MgInvalidPasswordException                   MAPGUIDE_EXCEPTION_ID+26
#define MapGuide_Exception_MgInvalidPrintLayoutFontSizeUnitsException   MAPGUIDE_EXCEPTION_ID+27
#define MapGuide_Exception_MgInvalidPrintLayoutPositionUnitsException   MAPGUIDE_EXCEPTION_ID+28
#define MapGuide_Exception_MgInvalidPrintLayoutSizeUnitsException       MAPGUIDE_EXCEPTION_ID+29
#define MapGuide_Exception_MgInvalidSerialNumberException               MAPGUIDE_EXCEPTION_ID+30
#define MapGuide_Exception_MgInvalidServerNameException                 MAPGUIDE_EXCEPTION_ID+31
#define MapGuide_Exception_MgLicenseException                           MAPGUIDE_EXCEPTION_ID+32
#define MapGuide_Exception_MgLicenseExpiredException                    MAPGUIDE_EXCEPTION_ID+33
#define MapGuide_Exception_MgLogOpenFailedException                     MAPGUIDE_EXCEPTION_ID+34
#define MapGuide_Exception_MgOperationProcessingException               MAPGUIDE_EXCEPTION_ID+35
#define MapGuide_Exception_MgParameterNotFoundException                 MAPGUIDE_EXCEPTION_ID+36
#define MapGuide_Exception_MgPathTooLongException                       MAPGUIDE_EXCEPTION_ID+37
#define MapGuide_Exception_MgPermissionDeniedException                  MAPGUIDE_EXCEPTION_ID+38
#define MapGuide_Exception_MgPortNotAvailableException                  MAPGUIDE_EXCEPTION_ID+39
#define MapGuide_Exception_MgPrintToScaleModeNotSelectedException       MAPGUIDE_EXCEPTION_ID+40
#define MapGuide_Exception_MgRepositoryCreationFailedException          MAPGUIDE_EXCEPTION_ID+41
#define MapGuide_Exception_MgRepositoryNotFoundException                MAPGUIDE_EXCEPTION_ID+42
#define MapGuide_Exception_MgRepositoryNotOpenException                 MAPGUIDE_EXCEPTION_ID+43
#define MapGuide_Exception_MgRepositoryOpenFailedException              MAPGUIDE_EXCEPTION_ID+44
#define MapGuide_Exception_MgRoleNotFoundException                      MAPGUIDE_EXCEPTION_ID+45
#define MapGuide_Exception_MgServerNotFoundException                    MAPGUIDE_EXCEPTION_ID+46
#define MapGuide_Exception_MgServerNotOnlineException                   MAPGUIDE_EXCEPTION_ID+47
#define MapGuide_Exception_MgSessionExpiredException                    MAPGUIDE_EXCEPTION_ID+48
#define MapGuide_Exception_MgUnauthorizedAccessException                MAPGUIDE_EXCEPTION_ID+49
#define MapGuide_Exception_MgUriFormatException                         MAPGUIDE_EXCEPTION_ID+50
#define MapGuide_Exception_MgStylizeLayerFailedException                MAPGUIDE_EXCEPTION_ID+51
#define MapGuide_Exception_MgSessionNotFoundException                   MAPGUIDE_EXCEPTION_ID+52
#define MapGuide_Exception_MgUnsupportedProviderThreadModelException    MAPGUIDE_EXCEPTION_ID+53
#define MapGuide_Exception_MgAllProviderConnectionsUsedException        MAPGUIDE_EXCEPTION_ID+54
#define MapGuide_Exception_MgRasterTransformationNotSupportedException  MAPGUIDE_EXCEPTION_ID+55


// MapLayer API
#define MapGuide_MapLayer_Map           MAPGUIDE_MAPLAYER_ID+0
#define MapGuide_MapLayer_Layer         MAPGUIDE_MAPLAYER_ID+1
#define MapGuide_MapLayer_Selection     MAPGUIDE_MAPLAYER_ID+2
#define MapGuide_MapLayer_Controller    MAPGUIDE_MAPLAYER_ID+3
#define MapGuide_MapLayer_AdvController MAPGUIDE_MAPLAYER_ID+4
#define MapGuide_MapLayer_ZcvController MAPGUIDE_MAPLAYER_ID+5


//Basic Service functionality
#define MapGuide_Service_ServerConnection               MAPGUIDE_SERVICE_ID+0
#define MapGuide_Service_SiteConnection                 MAPGUIDE_SERVICE_ID+1
#define MapGuide_Service_ConnectionProperties           MAPGUIDE_SERVICE_ID+2
#define MapGuide_Service_ServerInformation              MAPGUIDE_SERVICE_ID+3
#define MapGuide_Service_PackageStatusInformation       MAPGUIDE_SERVICE_ID+4
#define MapGuide_Service_Site                           MAPGUIDE_SERVICE_ID+5
#define MapGuide_Service_UserInformation                MAPGUIDE_SERVICE_ID+6
#define MapGuide_Service_ServerAdmin                    MAPGUIDE_SERVICE_ID+7
#define MapGuide_Service_SiteInfo                       MAPGUIDE_SERVICE_ID+8
#define MapGuide_Service_SqlResult                      MAPGUIDE_SERVICE_ID+9

// Drawing Service
#define MapGuide_DrawingService_DrawingService          MAPGUIDE_DRAWINGSERVICE_ID+0

// Mapping Service
#define MapGuide_MappingService_MappingService                      MAPGUIDE_MAPPINGSERVICE_ID+0
#define MapGuide_MappingService_DwfVersion                          MAPGUIDE_MAPPINGSERVICE_ID+1
#define MapGuide_MappingService_PlotSpecification                   MAPGUIDE_MAPPINGSERVICE_ID+2
#define MapGuide_MappingService_PrintLayout                         MAPGUIDE_MAPPINGSERVICE_ID+3
#define MapGuide_MappingService_Layout                              MAPGUIDE_MAPPINGSERVICE_ID+4
#define MapGuide_MappingService_MapPlot                             MAPGUIDE_MAPPINGSERVICE_ID+5
#define MapGuide_MappingService_MapPlotCollection                   MAPGUIDE_MAPPINGSERVICE_ID+6

// Rendering Service
#define MapGuide_RenderingService_RenderingService                  MAPGUIDE_RENDERINGSERVICE_ID+0
#define MapGuide_RenderingService_FeatureInformation                MAPGUIDE_RENDERINGSERVICE_ID+1
#define MapGuide_RenderingService_RenderingOptions                  MAPGUIDE_RENDERINGSERVICE_ID+2

// Resource Service
#define MapGuide_ResourceService_ResourceService                    MAPGUIDE_RESOURCESERVICE_ID+0


// Tile Service
#define MapGuide_TileService_TileService                            MAPGUIDE_TILESERVICE_ID+0

// KML Service
#define MapGuide_KmlService_KmlService                              MAPGUIDE_KMLSERVICE_ID+0

// Profiling Service
#define MapGuide_ProfilingService_ProfilingService                  MAPGUIDE_PROFILINGSERVICE_ID+0
#endif

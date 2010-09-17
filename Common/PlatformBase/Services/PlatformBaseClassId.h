//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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

// Predefined object identifiers ranges for each class of object.  By convention,
// each library has a range of 10 thousand ids.  Ids are broken into groups.

#define PLATFORMBASE_COLLECTION_ID          10000
#define PLATFORMBASE_DATA_ID                10250
#define PLATFORMBASE_EXCEPTION_ID           10500
#define PLATFORMBASE_PROPERTY_ID            10750
#define PLATFORMBASE_SYSTEM_ID              11000
#define PLATFORMBASE_CORESERVICE_ID         11250
#define PLATFORMBASE_RESOURCESERVICE_ID     11500
#define PLATFORMBASE_FEATURESERVICE_ID      11750
#define PLATFORMBASE_MAPLAYER_ID            12000
#define PLATFORMBASE_PRINTLAYOUTSERVICE_ID  12250

// Platform Collection
#define PlatformBase_Collection_IntCollection                       PLATFORMBASE_COLLECTION_ID+0
#define PlatformBase_Collection_PropertyDefinitionCollection        PLATFORMBASE_COLLECTION_ID+1
#define PlatformBase_Collection_SerializableCollection              PLATFORMBASE_COLLECTION_ID+2
#define PlatformBase_Collection_StringPropertyCollection            PLATFORMBASE_COLLECTION_ID+3
#define PlatformBase_Collection_ParameterCollection                 PLATFORMBASE_COLLECTION_ID+4

// Platform Data
#define PlatformBase_Data_Color                                     PLATFORMBASE_DATA_ID+0
#define PlatformBase_Property_PropertyDefinition                    PLATFORMBASE_DATA_ID+1
#define PlatformBase_Property_BlobProperty                          PLATFORMBASE_DATA_ID+2
#define PlatformBase_Property_BooleanProperty                       PLATFORMBASE_DATA_ID+3
#define PlatformBase_Property_ByteProperty                          PLATFORMBASE_DATA_ID+4
#define PlatformBase_Property_ClobProperty                          PLATFORMBASE_DATA_ID+5
#define PlatformBase_Property_DateTimeProperty                      PLATFORMBASE_DATA_ID+6
#define PlatformBase_Property_DoubleProperty                        PLATFORMBASE_DATA_ID+7
#define PlatformBase_Property_Int16Property                         PLATFORMBASE_DATA_ID+8
#define PlatformBase_Property_Int32Property                         PLATFORMBASE_DATA_ID+9
#define PlatformBase_Property_Int64Property                         PLATFORMBASE_DATA_ID+10
#define PlatformBase_Property_SingleProperty                        PLATFORMBASE_DATA_ID+11
#define PlatformBase_Data_Point3D                                   PLATFORMBASE_DATA_ID+12
#define PlatformBase_Data_Vector3D                                  PLATFORMBASE_DATA_ID+13
#define PlatformBase_Data_Size2D                                    PLATFORMBASE_DATA_ID+14
#define PlatformBase_Data_Margin                                    PLATFORMBASE_DATA_ID+15

// Platform Exceptions
#define PlatformBase_Exception_MgArrayTypeMismatchException                 PLATFORMBASE_EXCEPTION_ID+0
#define PlatformBase_Exception_MgDuplicateResourceDataException             PLATFORMBASE_EXCEPTION_ID+1
#define PlatformBase_Exception_MgDuplicateResourceException                 PLATFORMBASE_EXCEPTION_ID+2
#define PlatformBase_Exception_MgEmptyFeatureSetException                   PLATFORMBASE_EXCEPTION_ID+3
#define PlatformBase_Exception_MgFdoException                               PLATFORMBASE_EXCEPTION_ID+4
#define PlatformBase_Exception_MgFeatureServiceException                    PLATFORMBASE_EXCEPTION_ID+5
#define PlatformBase_Exception_MgInvalidLayerDefinitionException            PLATFORMBASE_EXCEPTION_ID+6
#define PlatformBase_Exception_MgInvalidMapDefinitionException              PLATFORMBASE_EXCEPTION_ID+7
#define PlatformBase_Exception_MgInvalidRepositoryNameException             PLATFORMBASE_EXCEPTION_ID+8
#define PlatformBase_Exception_MgInvalidRepositoryTypeException             PLATFORMBASE_EXCEPTION_ID+9
#define PlatformBase_Exception_MgInvalidResourceDataNameException           PLATFORMBASE_EXCEPTION_ID+10
#define PlatformBase_Exception_MgInvalidResourceDataTypeException           PLATFORMBASE_EXCEPTION_ID+11
#define PlatformBase_Exception_MgInvalidResourceNameException               PLATFORMBASE_EXCEPTION_ID+12
#define PlatformBase_Exception_MgInvalidResourcePathException               PLATFORMBASE_EXCEPTION_ID+13
#define PlatformBase_Exception_MgInvalidResourcePreProcessingTypeException  PLATFORMBASE_EXCEPTION_ID+14
#define PlatformBase_Exception_MgInvalidResourceTypeException               PLATFORMBASE_EXCEPTION_ID+15
#define PlatformBase_Exception_MgInvalidSymbolDefinitionException           PLATFORMBASE_EXCEPTION_ID+16
#define PlatformBase_Exception_MgLayerNotFoundException                     PLATFORMBASE_EXCEPTION_ID+17
#define PlatformBase_Exception_MgResourceBusyException                      PLATFORMBASE_EXCEPTION_ID+18
#define PlatformBase_Exception_MgResourceDataNotFoundException              PLATFORMBASE_EXCEPTION_ID+19
#define PlatformBase_Exception_MgResourceNotFoundException                  PLATFORMBASE_EXCEPTION_ID+20
#define PlatformBase_Exception_MgServiceNotAvailableException               PLATFORMBASE_EXCEPTION_ID+21
#define PlatformBase_Exception_MgServiceNotSupportedException               PLATFORMBASE_EXCEPTION_ID+22
#define PlatformBase_Exception_MgUserNotFoundException                      PLATFORMBASE_EXCEPTION_ID+23
#define PlatformBase_Exception_MgInvalidWatermarkDefinitionException            PLATFORMBASE_EXCEPTION_ID+24

// MapLayer API
#define PlatformBase_MapLayer_MapBase                               PLATFORMBASE_MAPLAYER_ID+0
#define PlatformBase_MapLayer_LayerGroup                            PLATFORMBASE_MAPLAYER_ID+1
#define PlatformBase_MapLayer_LayerCollection                       PLATFORMBASE_MAPLAYER_ID+2
#define PlatformBase_MapLayer_LayerBase                             PLATFORMBASE_MAPLAYER_ID+3
#define PlatformBase_MapLayer_LayerGroupCollection                  PLATFORMBASE_MAPLAYER_ID+4
#define PlatformBase_MapLayer_MapCollection                         PLATFORMBASE_MAPLAYER_ID+5
#define PlatformBase_MapLayer_ReadOnlyLayerCollection               PLATFORMBASE_MAPLAYER_ID+6
#define PlatformBase_MapLayer_SelectionBase                         PLATFORMBASE_MAPLAYER_ID+7

// Core Service
#define PlatformBase_Service_ServerConnection                       PLATFORMBASE_CORESERVICE_ID+0
#define PlatformBase_Service_Service                                PLATFORMBASE_CORESERVICE_ID+1
#define PlatformBase_Service_Stream                                 PLATFORMBASE_CORESERVICE_ID+2
#define PlatformBase_Service_MgSiteConnection                       PLATFORMBASE_CORESERVICE_ID+3
#define PlatformBase_Service_MgConnectionProperties                 PLATFORMBASE_CORESERVICE_ID+4
#define PlatformBase_Service_MgServerInformation                    PLATFORMBASE_CORESERVICE_ID+5
#define PlatformBase_Service_MgPackageStatusInformation             PLATFORMBASE_CORESERVICE_ID+6
#define PlatformBase_Service_Warning                                PLATFORMBASE_CORESERVICE_ID+7

// Feature Service
#define PlatformBase_FeatureService_ClassDefinition                 PLATFORMBASE_FEATURESERVICE_ID+0
#define PlatformBase_FeatureService_DataPropertyDefinition          PLATFORMBASE_FEATURESERVICE_ID+1
#define PlatformBase_FeatureService_FeatureConnection               PLATFORMBASE_FEATURESERVICE_ID+2
#define PlatformBase_FeatureService_FeatureReader                   PLATFORMBASE_FEATURESERVICE_ID+3
#define PlatformBase_FeatureService_FeatureService                  PLATFORMBASE_FEATURESERVICE_ID+4
#define PlatformBase_FeatureService_FeatureServiceCommon            PLATFORMBASE_FEATURESERVICE_ID+5
#define PlatformBase_FeatureService_GeometricPropertyDefinition     PLATFORMBASE_FEATURESERVICE_ID+6
#define PlatformBase_FeatureService_Geometry                        PLATFORMBASE_FEATURESERVICE_ID+7
#define PlatformBase_FeatureService_GeometryProperty                PLATFORMBASE_FEATURESERVICE_ID+8
#define PlatformBase_FeatureService_ObjectPropertyDefinition        PLATFORMBASE_FEATURESERVICE_ID+9
#define PlatformBase_FeatureService_SpatialContextExtentType        PLATFORMBASE_FEATURESERVICE_ID+10
#define PlatformBase_FeatureService_SpatialContextReader            PLATFORMBASE_FEATURESERVICE_ID+11
#define PlatformBase_FeatureService_SqlDataReader                   PLATFORMBASE_FEATURESERVICE_ID+12
#define PlatformBase_FeatureService_FeatureSet                      PLATFORMBASE_FEATURESERVICE_ID+13
#define PlatformBase_FeatureService_FeatureProperty                 PLATFORMBASE_FEATURESERVICE_ID+14
#define PlatformBase_FeatureService_SpatialContextData              PLATFORMBASE_FEATURESERVICE_ID+15
#define PlatformBase_FeatureService_LongTransactionReader           PLATFORMBASE_FEATURESERVICE_ID+16
#define PlatformBase_FeatureService_LongTransactionData             PLATFORMBASE_FEATURESERVICE_ID+17
#define PlatformBase_FeatureService_RasterPropertyDefinition        PLATFORMBASE_FEATURESERVICE_ID+18
#define PlatformBase_FeatureService_Raster                          PLATFORMBASE_FEATURESERVICE_ID+19
#define PlatformBase_FeatureService_RasterProperty                  PLATFORMBASE_FEATURESERVICE_ID+20
#define PlatformBase_FeatureService_FeatureQueryOptions             PLATFORMBASE_FEATURESERVICE_ID+21
#define PlatformBase_FeatureService_FeatureAggregateOptions         PLATFORMBASE_FEATURESERVICE_ID+22
#define PlatformBase_FeatureService_DataReader                      PLATFORMBASE_FEATURESERVICE_ID+23
#define PlatformBase_FeatureService_FeatureCommandCollection        PLATFORMBASE_FEATURESERVICE_ID+24
#define PlatformBase_FeatureService_DeleteFeatures                  PLATFORMBASE_FEATURESERVICE_ID+25
#define PlatformBase_FeatureService_InsertFeatures                  PLATFORMBASE_FEATURESERVICE_ID+26
#define PlatformBase_FeatureService_UpdateFeatures                  PLATFORMBASE_FEATURESERVICE_ID+27
#define PlatformBase_FeatureService_FeatureSchema                   PLATFORMBASE_FEATURESERVICE_ID+28
#define PlatformBase_FeatureService_FeatureSchemaCollection         PLATFORMBASE_FEATURESERVICE_ID+29
#define PlatformBase_FeatureService_ClassDefinitionCollection       PLATFORMBASE_FEATURESERVICE_ID+30
#define PlatformBase_FeatureService_CreateSdfParams                 PLATFORMBASE_FEATURESERVICE_ID+31
#define PlatformBase_FeatureService_GwsFeatureReader                PLATFORMBASE_FEATURESERVICE_ID+32
#define PlatformBase_FeatureService_LockFeatures                    PLATFORMBASE_FEATURESERVICE_ID+33
#define PlatformBase_FeatureService_UnlockFeatures                  PLATFORMBASE_FEATURESERVICE_ID+34
#define PlatformBase_FeatureService_GeometryTypeInfo                PLATFORMBASE_FEATURESERVICE_ID+35
#define PlatformBase_FeatureService_FileFeatureSourceParams         PLATFORMBASE_FEATURESERVICE_ID+36
#define PlatformBase_FeatureService_FeatureTransaction              PLATFORMBASE_FEATURESERVICE_ID+37
#define PlatformBase_FeatureService_Parameter                       PLATFORMBASE_FEATURESERVICE_ID+38

// Resource Service
#define PlatformBase_ResourceService_ResourceIdentifier             PLATFORMBASE_RESOURCESERVICE_ID+0
#define PlatformBase_ResourceService_ResourceService                PLATFORMBASE_RESOURCESERVICE_ID+1
#define PlatformBase_ResourceService_Resource                       PLATFORMBASE_RESOURCESERVICE_ID+26

// Print Layout Service
#define PlatformBase_PrintLayoutService_PrintLayoutServiceBase      PLATFORMBASE_PRINTLAYOUTSERVICE_ID+0
#define PlatformBase_PrintLayoutService_PrintLayoutBase             PLATFORMBASE_PRINTLAYOUTSERVICE_ID+1
#define PlatformBase_PrintLayoutService_PrintLayoutElementBase      PLATFORMBASE_PRINTLAYOUTSERVICE_ID+2
#define PlatformBase_PrintLayoutService_PrintLayoutElementCollection PLATFORMBASE_PRINTLAYOUTSERVICE_ID+3
#define PlatformBase_PrintLayoutService_MapViewportBase             PLATFORMBASE_PRINTLAYOUTSERVICE_ID+4
#define PlatformBase_PrintLayoutService_MapView                     PLATFORMBASE_PRINTLAYOUTSERVICE_ID+5
#define PlatformBase_PrintLayoutService_PropertyMapping             PLATFORMBASE_PRINTLAYOUTSERVICE_ID+6
#define PlatformBase_PrintLayoutService_PropertyMappingCollection   PLATFORMBASE_PRINTLAYOUTSERVICE_ID+7

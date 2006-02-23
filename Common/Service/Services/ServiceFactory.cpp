//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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

#include "AceCommon.h"
#include "SiteConnection.h"
#include "ServiceFactory.h"
#include "ProxySqlDataReader.h"
#include "ProxyDataReader.h"
#include "ProxyFeatureReader.h"
#include "LongTransactionData.h"
#include "SpatialContextData.h"
#include "ProxyGwsFeatureReader.h"

#ifdef WIN32
#include "windows.h"
#else
#include <dlfcn.h>
#endif

static bool InitializeStaticData();

static map<int,  MgObject* (*)()> classCreators;
static bool initStatic = InitializeStaticData();

#define EXCEPTION_CLASS_CREATOR(className) \
    classCreators[Common_Exception_##className]  =   className::CreateObject;


bool InitializeStaticData()
{
    //put in the map any class that can be serialized in a command
    classCreators[CoreData_Data_ByteReader]                 = MgByteReader::CreateObject;
    classCreators[CoreData_Data_DateTime]                   = MgDateTime::CreateObject;
    classCreators[CoreData_Data_Color]                      = MgColor::CreateObject;

    // classCreators[CoreData_Collection_Collection]                   =  MgCollection::CreateObject;
    // classCreators[CoreData_Collection_NamedCollection]              =  MgNamedCollection::CreateObject;
    classCreators[CoreData_Collection_PropertyCollection]           =  MgPropertyCollection::CreateObject;
    classCreators[CoreData_Collection_StringCollection]             =  MgStringCollection::CreateObject;
    classCreators[CoreData_Collection_StringPropertyCollection]     =  MgStringPropertyCollection::CreateObject;
    classCreators[CoreData_Collection_IntCollection]                =  MgIntCollection::CreateObject;
    classCreators[CoreData_Collection_PropertyDefinitionCollection] =  MgPropertyDefinitionCollection::CreateObject;
    classCreators[CoreData_Collection_BatchPropertyCollection]      =  MgBatchPropertyCollection::CreateObject;
    classCreators[CoreData_Collection_SerializableCollection]       =  MgSerializableCollection::CreateObject;

    classCreators[CoreData_Property_BlobProperty]           = MgBlobProperty::CreateObject;
    classCreators[CoreData_Property_BooleanProperty]        = MgBooleanProperty::CreateObject;
    classCreators[CoreData_Property_ByteProperty]           = MgByteProperty::CreateObject;
    classCreators[CoreData_Property_ClobProperty]           = MgClobProperty::CreateObject;
    classCreators[CoreData_Property_DateTimeProperty]       = MgDateTimeProperty::CreateObject;
    classCreators[CoreData_Property_DoubleProperty]         = MgDoubleProperty::CreateObject;
    classCreators[CoreData_Property_FeatureProperty]        = MgFeatureProperty::CreateObject;
    classCreators[CoreData_Property_GeometryProperty]       = MgGeometryProperty::CreateObject;
    classCreators[CoreData_Property_Int16Property]          = MgInt16Property::CreateObject;
    classCreators[CoreData_Property_Int32Property]          = MgInt32Property::CreateObject;
    classCreators[CoreData_Property_Int64Property]          = MgInt64Property::CreateObject;
    classCreators[CoreData_Property_SingleProperty]         = MgSingleProperty::CreateObject;
    classCreators[CoreData_Property_StringProperty]         = MgStringProperty::CreateObject;
    classCreators[CoreData_Property_PropertyDefinition]     = MgPropertyDefinition::CreateObject;

    classCreators[CoreData_System_Warning]                  = MgWarnings::CreateObject;
    classCreators[CoreData_System_MgUserInformation]        = MgUserInformation::CreateObject;

    classCreators[ResourceService_ResourceIdentifier]       = MgResourceIdentifier::CreateObject;

    EXCEPTION_CLASS_CREATOR(MgArgumentOutOfRangeException)
    EXCEPTION_CLASS_CREATOR(MgArrayTypeMismatchException)
    EXCEPTION_CLASS_CREATOR(MgAuthenticationFailedException)
    EXCEPTION_CLASS_CREATOR(MgClassNotFoundException)
    EXCEPTION_CLASS_CREATOR(MgConfigurationException)
    EXCEPTION_CLASS_CREATOR(MgConfigurationLoadFailedException)
    EXCEPTION_CLASS_CREATOR(MgConfigurationSaveFailedException)
    EXCEPTION_CLASS_CREATOR(MgConnectionFailedException)
    EXCEPTION_CLASS_CREATOR(MgConnectionNotOpenException)
    EXCEPTION_CLASS_CREATOR(MgCoordinateSystemComputationFailedException)
    EXCEPTION_CLASS_CREATOR(MgCoordinateSystemConversionFailedException)
    EXCEPTION_CLASS_CREATOR(MgCoordinateSystemMeasureFailedException)
    EXCEPTION_CLASS_CREATOR(MgCoordinateSystemTransformFailedException)
    EXCEPTION_CLASS_CREATOR(MgDateTimeException)
    EXCEPTION_CLASS_CREATOR(MgDbException)
    EXCEPTION_CLASS_CREATOR(MgDbXmlException)
    EXCEPTION_CLASS_CREATOR(MgDecryptionException)
    EXCEPTION_CLASS_CREATOR(MgDirectoryNotFoundException)
    EXCEPTION_CLASS_CREATOR(MgDivideByZeroException)
    EXCEPTION_CLASS_CREATOR(MgDomainException)
    EXCEPTION_CLASS_CREATOR(MgDuplicateDirectoryException)
    EXCEPTION_CLASS_CREATOR(MgDuplicateFileException)
    EXCEPTION_CLASS_CREATOR(MgDuplicateGroupException)
    EXCEPTION_CLASS_CREATOR(MgDuplicateNameException)
    EXCEPTION_CLASS_CREATOR(MgDuplicateObjectException)
    EXCEPTION_CLASS_CREATOR(MgDuplicateParameterException)
    EXCEPTION_CLASS_CREATOR(MgDuplicateRepositoryException)
    EXCEPTION_CLASS_CREATOR(MgDuplicateResourceDataException)
    EXCEPTION_CLASS_CREATOR(MgDuplicateResourceException)
    EXCEPTION_CLASS_CREATOR(MgDuplicateRoleException)
    EXCEPTION_CLASS_CREATOR(MgDuplicateServerException)
    EXCEPTION_CLASS_CREATOR(MgDuplicateSessionException)
    EXCEPTION_CLASS_CREATOR(MgDuplicateUserException)
    EXCEPTION_CLASS_CREATOR(MgDwfException)
    EXCEPTION_CLASS_CREATOR(MgDwfSectionNotFoundException)
    EXCEPTION_CLASS_CREATOR(MgDwfSectionResourceNotFoundException)
    EXCEPTION_CLASS_CREATOR(MgEmptyFeatureSetException)
    EXCEPTION_CLASS_CREATOR(MgEncryptionException)
    EXCEPTION_CLASS_CREATOR(MgEndOfStreamException)
    EXCEPTION_CLASS_CREATOR(MgFdoException)
    EXCEPTION_CLASS_CREATOR(MgFeatureServiceException)
    EXCEPTION_CLASS_CREATOR(MgFileIoException)
    EXCEPTION_CLASS_CREATOR(MgFileNotFoundException)
    EXCEPTION_CLASS_CREATOR(MgGeometryException)
    EXCEPTION_CLASS_CREATOR(MgGroupNotFoundException)
    EXCEPTION_CLASS_CREATOR(MgIndexOutOfRangeException)
    EXCEPTION_CLASS_CREATOR(MgInvalidArgumentException)
    EXCEPTION_CLASS_CREATOR(MgInvalidCastException)
    EXCEPTION_CLASS_CREATOR(MgInvalidCoordinateSystemException)
    EXCEPTION_CLASS_CREATOR(MgInvalidCoordinateSystemTypeException)
    EXCEPTION_CLASS_CREATOR(MgInvalidCoordinateSystemUnitsException)
    EXCEPTION_CLASS_CREATOR(MgInvalidDwfPackageException)
    EXCEPTION_CLASS_CREATOR(MgInvalidDwfSectionException)
    EXCEPTION_CLASS_CREATOR(MgInvalidFeatureSourceException)
    EXCEPTION_CLASS_CREATOR(MgInvalidIpAddressException)
    EXCEPTION_CLASS_CREATOR(MgInvalidLicenseException)
    EXCEPTION_CLASS_CREATOR(MgInvalidLogEntryException)
    EXCEPTION_CLASS_CREATOR(MgInvalidMapDefinitionException)
    EXCEPTION_CLASS_CREATOR(MgInvalidMimeTypeException)
    EXCEPTION_CLASS_CREATOR(MgInvalidOperationException)
    EXCEPTION_CLASS_CREATOR(MgInvalidOperationVersionException)
    EXCEPTION_CLASS_CREATOR(MgInvalidPasswordException)
    EXCEPTION_CLASS_CREATOR(MgInvalidPrintLayoutFontSizeUnitsException)
    EXCEPTION_CLASS_CREATOR(MgInvalidPrintLayoutPositionUnitsException)
    EXCEPTION_CLASS_CREATOR(MgInvalidPrintLayoutSizeUnitsException)
    EXCEPTION_CLASS_CREATOR(MgInvalidPropertyTypeException)
    EXCEPTION_CLASS_CREATOR(MgInvalidRepositoryNameException)
    EXCEPTION_CLASS_CREATOR(MgInvalidRepositoryTypeException)
    EXCEPTION_CLASS_CREATOR(MgInvalidResourceDataNameException)
    EXCEPTION_CLASS_CREATOR(MgInvalidResourceDataTypeException)
    EXCEPTION_CLASS_CREATOR(MgInvalidResourceNameException)
    EXCEPTION_CLASS_CREATOR(MgInvalidResourcePathException)
    EXCEPTION_CLASS_CREATOR(MgInvalidResourcePreProcessingTypeException)
    EXCEPTION_CLASS_CREATOR(MgInvalidResourceTypeException)
    EXCEPTION_CLASS_CREATOR(MgInvalidServerNameException)
    EXCEPTION_CLASS_CREATOR(MgInvalidStreamHeaderException)
    EXCEPTION_CLASS_CREATOR(MgIoException)
    EXCEPTION_CLASS_CREATOR(MgLayerNotFoundException)
    EXCEPTION_CLASS_CREATOR(MgLengthException)
    EXCEPTION_CLASS_CREATOR(MgLicenseException)
    EXCEPTION_CLASS_CREATOR(MgLicenseExpiredException)
    EXCEPTION_CLASS_CREATOR(MgLogicException)
    EXCEPTION_CLASS_CREATOR(MgLogOpenFailedException)
    EXCEPTION_CLASS_CREATOR(MgNotFiniteNumberException)
    EXCEPTION_CLASS_CREATOR(MgNotImplementedException)
    EXCEPTION_CLASS_CREATOR(MgNullArgumentException)
    EXCEPTION_CLASS_CREATOR(MgNullPropertyValueException)
    EXCEPTION_CLASS_CREATOR(MgNullReferenceException)
    EXCEPTION_CLASS_CREATOR(MgObjectNotFoundException)
    EXCEPTION_CLASS_CREATOR(MgOperationProcessingException)
    EXCEPTION_CLASS_CREATOR(MgOutOfMemoryException)
    EXCEPTION_CLASS_CREATOR(MgOutOfRangeException)
    EXCEPTION_CLASS_CREATOR(MgOverflowException)
    EXCEPTION_CLASS_CREATOR(MgParameterNotFoundException)
    EXCEPTION_CLASS_CREATOR(MgPathTooLongException)
    EXCEPTION_CLASS_CREATOR(MgPermissionDeniedException)
    EXCEPTION_CLASS_CREATOR(MgPlatformNotSupportedException)
    EXCEPTION_CLASS_CREATOR(MgPortNotAvailableException)
    EXCEPTION_CLASS_CREATOR(MgPrintToScaleModeNotSelectedException)
    EXCEPTION_CLASS_CREATOR(MgRepositoryCreationFailedException)
    EXCEPTION_CLASS_CREATOR(MgRepositoryNotFoundException)
    EXCEPTION_CLASS_CREATOR(MgRepositoryNotOpenException)
    EXCEPTION_CLASS_CREATOR(MgRepositoryOpenFailedException)
    EXCEPTION_CLASS_CREATOR(MgResourceDataNotFoundException)
    EXCEPTION_CLASS_CREATOR(MgResourceNotFoundException)
    EXCEPTION_CLASS_CREATOR(MgResourcesException)
    EXCEPTION_CLASS_CREATOR(MgResourcesLoadFailedException)
    EXCEPTION_CLASS_CREATOR(MgResourceTagNotFoundException)
    EXCEPTION_CLASS_CREATOR(MgRoleNotFoundException)
    EXCEPTION_CLASS_CREATOR(MgRuntimeException)
    EXCEPTION_CLASS_CREATOR(MgServerNotFoundException)
    EXCEPTION_CLASS_CREATOR(MgServerNotOnlineException)
    EXCEPTION_CLASS_CREATOR(MgServiceNotAvailableException)
    EXCEPTION_CLASS_CREATOR(MgServiceNotSupportedException)
    EXCEPTION_CLASS_CREATOR(MgSessionExpiredException)
    EXCEPTION_CLASS_CREATOR(MgStreamIoException)
    EXCEPTION_CLASS_CREATOR(MgTemporaryFileNotAvailableException)
    EXCEPTION_CLASS_CREATOR(MgUnauthorizedAccessException)
    EXCEPTION_CLASS_CREATOR(MgUnclassifiedException)
    EXCEPTION_CLASS_CREATOR(MgUnderflowException)
    EXCEPTION_CLASS_CREATOR(MgUriFormatException)
    EXCEPTION_CLASS_CREATOR(MgUserNotFoundException)
    EXCEPTION_CLASS_CREATOR(MgXmlException)
    EXCEPTION_CLASS_CREATOR(MgXmlParserException)

    // CoreService
    classCreators[CoreService_MgServerInformation]                =  MgServerInformation::CreateObject;

    // FeatureService
    classCreators[FeatureService_ClassDefinition]                 =  MgClassDefinition::CreateObject;
    classCreators[FeatureService_DataPropertyDefinition]          =  MgDataPropertyDefinition::CreateObject;
    classCreators[FeatureService_FeatureReader]                   =  MgProxyFeatureReader::CreateObject;
    // classCreators[FeatureService_FeatureService]                  =  MgFeatureService::CreateObject;
    classCreators[FeatureService_GeometricPropertyDefinition]     =  MgGeometricPropertyDefinition::CreateObject;
    classCreators[FeatureService_RasterPropertyDefinition]        =  MgRasterPropertyDefinition::CreateObject;
    // classCreators[FeatureService_Geometry]                        =  MgGeometry::CreateObject;
    classCreators[FeatureService_ObjectPropertyDefinition]        =  MgObjectPropertyDefinition::CreateObject;
    // classCreators[FeatureService_SpatialContextExtentType]        =  MgSpatialContextExtentType::CreateObject;
    classCreators[FeatureService_SpatialContextReader]            =  MgSpatialContextReader::CreateObject;
    classCreators[FeatureService_SpatialContextData]              =  MgSpatialContextData::CreateObject;
    classCreators[FeatureService_SqlDataReader]                   =  MgProxySqlDataReader::CreateObject;
    classCreators[FeatureService_DataReader]                      =  MgProxyDataReader::CreateObject;
    classCreators[FeatureService_FeatureSet]                      =  MgFeatureSet::CreateObject;
    classCreators[FeatureService_Raster]                          =  MgRaster::CreateObject;
    classCreators[FeatureService_LongTransactionReader]           =  MgLongTransactionReader::CreateObject;
    classCreators[FeatureService_LongTransactionData]             =  MgLongTransactionData::CreateObject;
    classCreators[FeatureService_RasterProperty]                  =  MgRasterProperty::CreateObject;
    classCreators[FeatureService_FeatureQueryOptions]             =  MgFeatureQueryOptions::CreateObject;
    classCreators[FeatureService_FeatureAggregateOptions]         =  MgFeatureAggregateOptions::CreateObject;
    classCreators[FeatureService_GwsFeatureReader]                =  MgProxyGwsFeatureReader::CreateObject;

    classCreators[FeatureService_FeatureCommandCollection]        =  MgFeatureCommandCollection::CreateObject;
    classCreators[FeatureService_DeleteFeatures]                  =  MgDeleteFeatures::CreateObject;
    classCreators[FeatureService_InsertFeatures]                  =  MgInsertFeatures::CreateObject;
    classCreators[FeatureService_UpdateFeatures]                  =  MgUpdateFeatures::CreateObject;
    classCreators[FeatureService_FeatureSchema]                   =  MgFeatureSchema::CreateObject;
    classCreators[FeatureService_FeatureSchemaCollection]         =  MgFeatureSchemaCollection::CreateObject;
    classCreators[FeatureService_ClassDefinitionCollection]       =  MgClassDefinitionCollection::CreateObject;
    classCreators[FeatureService_CreateSdfParams]                 =  MgCreateSdfParams::CreateObject;


    //mapping services
    classCreators[MappingService_DwfVersion]                      =  MgDwfVersion::CreateObject;
    classCreators[MappingService_PlotSpecification]               =  MgPlotSpecification::CreateObject;
    classCreators[MappingService_PrintLayout]                     =  MgPrintLayout::CreateObject;
    classCreators[MappingService_MapPlotCollection]               =  MgMapPlotCollection::CreateObject;
    classCreators[MappingService_Layout]                          =  MgLayout::CreateObject;

    //rendering services
    classCreators[RenderingService_FeatureInformation]            =  MgFeatureInformation::CreateObject;

    //map/layer api
    classCreators[MapLayer_Map]                                   =  MgMap::CreateObject;
    classCreators[MapLayer_Layer]                                 =  MgLayer::CreateObject;
    classCreators[MapLayer_LayerGroup]                            =  MgLayerGroup::CreateObject;
    classCreators[MapLayer_Selection]                             =  MgSelection::CreateObject;

    //Geometry
    classCreators[Geometry_CurvePolygon]                          =  MgCurvePolygon::CreateObject;
    classCreators[Geometry_CurveString]                           =  MgCurveString::CreateObject;
    classCreators[Geometry_LineString]                            =  MgLineString::CreateObject;
    classCreators[Geometry_Point]                                 =  MgPoint::CreateObject;
    classCreators[Geometry_Polygon]                               =  MgPolygon::CreateObject;
    classCreators[Geometry_MultiCurvePolygon]                     =  MgMultiCurvePolygon::CreateObject;
    classCreators[Geometry_MultiCurveString]                      =  MgMultiCurveString::CreateObject;
    classCreators[Geometry_MultiLineString]                       =  MgMultiLineString::CreateObject;
    classCreators[Geometry_MultiPoint]                            =  MgMultiPoint::CreateObject;
    classCreators[Geometry_MultiPolygon]                          =  MgMultiPolygon::CreateObject;
    classCreators[Geometry_MultiGeometry]                         =  MgMultiGeometry::CreateObject;

    classCreators[Geometry_Envelope]                              =  MgEnvelope::CreateObject;
    classCreators[Geometry_CoordinateXY]                          =  MgCoordinateXY::CreateObject;
    classCreators[Geometry_CoordinateXYZ]                         =  MgCoordinateXYZ::CreateObject;
    classCreators[Geometry_CoordinateXYM]                         =  MgCoordinateXYM::CreateObject;
    classCreators[Geometry_CoordinateXYZM]                        =  MgCoordinateXYZM::CreateObject;

    return true;
}

extern "C" void* CreateMgObject(int classId)
{
    //find the object in the map
    MgObject*(*createFunc)() = classCreators[classId];
    if(createFunc == NULL)
        return NULL;

    MgObject* obj = (*createFunc)();
    return obj;
}

#ifdef WIN32

HMODULE LoadDLL(ServerConnectionType sct)
{
    HMODULE hlib = NULL;
// Load the relevant dll
#ifdef _DEBUG // load debug dll
    hlib = LoadLibrary(L"MgServerFactoryd.dll");
#else // Load Release dll
    hlib = LoadLibrary(L"MgServerFactory.dll");
#endif

    return hlib;

}

#else  // ! WIN32

void* LoadDLL(ServerConnectionType sct)
{
    void* hlib = dlopen("libMgServerFactory.so", RTLD_LAZY);

#ifdef _DEBUG

    if (NULL == hlib)
        printf("\nCannot load %s factory DLL : %s", (sct == sctLocalInProc? "server": "proxy"), dlerror());

#endif

    return hlib;

}

#endif

void* GetMgServerService(ServerConnectionType sct, INT16 serviceType, MgConnectionProperties* connProp)
{
    MgService* service = NULL;
    typedef void*(*CREATEFUNC)(INT32, MgConnectionProperties*);
    CREATEFUNC createFunc = NULL;

#ifdef WIN32
    static HMODULE hServerLib = NULL;
    if (NULL == hServerLib)
    {
        hServerLib = LoadDLL(sct);
    }

    if( hServerLib != NULL )
    {
        createFunc = (CREATEFUNC)GetProcAddress(hServerLib, "CreateMgServerService");
    }
#else // ! WIN32
    static void* hServerLib = NULL;
    if (NULL == hServerLib)
    {
        hServerLib = LoadDLL(sct);
    }

    if( hServerLib != NULL )
    {
        createFunc = (CREATEFUNC)dlsym(hServerLib, "CreateMgServerService");
    }

#endif

    if(createFunc != NULL)
    {
        service = (MgService*)(*createFunc)(serviceType, connProp);
    }

    return service;
}

extern "C" void* CreateMgService(ServerConnectionType sct, INT32 serviceType, MgConnectionProperties* connProp)
{
    MgService* service = NULL;

    if (sctRemoteServerToServer == sct)
    {
        switch(serviceType)
        {
            case MgServiceType::DrawingService:
                service = new MgProxyDrawingService(connProp);
                break;
            case MgServiceType::FeatureService:
                service = new MgProxyFeatureService(connProp);
                break;
            case MgServiceType::MappingService:
                service = new MgProxyMappingService(connProp);
                break;
            case MgServiceType::RenderingService:
                service = new MgProxyRenderingService(connProp);
                break;
            case MgServiceType::ResourceService:
                service = new MgProxyResourceService(connProp);
                break;
            case MgServiceType::TileService:
                service = new MgProxyTileService(connProp);
                break;
            case MgServiceType::ServerAdminService:
                // Admin Service is a server object.
                break;
            case MgServiceType::SiteService:
                // Site Service is a server object.
                break;
            default:
                break;
        }
    }
    else if (sctServerToHttp == sct)
    {
        // Fill in code here for HTTP proxy services
    }
    else if (sctLocalInProc == sct)
    {
        service = (MgService*)GetMgServerService(sct, serviceType, connProp);
    }

    return service;
}

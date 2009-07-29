//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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

#include "PlatformBase.h"

#ifdef _WIN32
#include "windows.h"
#else
#include <dlfcn.h>
#endif


#include "Foundation.h"

// Process-wide MgServiceRegistry
Ptr<MgServiceRegistry> MgServiceRegistry::m_serviceRegistry = (MgServiceRegistry*)NULL;

MgServiceRegistry::MgServiceRegistry()
{
}

MgServiceRegistry* MgServiceRegistry::GetInstance()
{
    MG_TRY()

    ACE_TRACE ("MgServiceRegistry::GetInstance");

    if (MgServiceRegistry::m_serviceRegistry == NULL)
    {
        // Perform Double-Checked Locking Optimization.
        ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, *ACE_Static_Object_Lock::instance (), 0));
        if (MgServiceRegistry::m_serviceRegistry == NULL)
        {
            MgServiceRegistry::m_serviceRegistry = new MgServiceRegistry();
        }
    }

    MG_CATCH_AND_THROW(L"MgServiceRegistry.GetInstance")

    // To avoid overhead and maintain thread safety,
    // do not assign this returned static singleton to a Ptr object.
    return MgServiceRegistry::m_serviceRegistry;
}


void MgServiceRegistry::Dispose()
{
    delete this;
}


MgService* MgServiceRegistry::CreateService(INT16 serviceType, ServerConnectionType connType)
{
    ServiceRegistry& registry = m_serviceCreators[connType];
    const ServiceCreatorFunc& func = registry[serviceType];
    if (NULL == func)
    {
        throw new MgServiceNotSupportedException(L"MgServiceRegistry.CreateService", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MgService* obj = (*func)();
    return obj;
}


void MgServiceRegistry::RegisterService(INT16 serviceType, ServiceCreatorFunc creator, ServerConnectionType connType)

{
    if (NULL == creator)
    {
        throw new MgInvalidArgumentException(L"MgServiceRegistry.RegisterClass", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    m_serviceCreators[connType][serviceType] = creator;
}


static bool InitializeStaticPlatformData();

static bool initStatic = InitializeStaticPlatformData();

#define EXCEPTION_CLASS_CREATOR(className) \
    fact->Register(PlatformBase_Exception_##className, className::CreateObject);

bool InitializeStaticPlatformData()
{
    MgClassFactory* fact = MgClassFactory::GetInstance();

    //put in the map any class that can be serialized
    fact->Register(PlatformBase_Collection_IntCollection, MgIntCollection::CreateObject);
    fact->Register(PlatformBase_Collection_PropertyDefinitionCollection, MgPropertyDefinitionCollection::CreateObject);
    fact->Register(PlatformBase_Collection_SerializableCollection, MgSerializableCollection::CreateObject);
    fact->Register(PlatformBase_Collection_StringPropertyCollection, MgStringPropertyCollection::CreateObject);
    fact->Register(PlatformBase_Data_Color, MgColor::CreateObject);
    fact->Register(PlatformBase_Data_Point3D, MgPoint3D::CreateObject);
    fact->Register(PlatformBase_Data_Vector3D, MgVector3D::CreateObject);
    fact->Register(PlatformBase_Property_PropertyDefinition, MgPropertyDefinition::CreateObject);
    fact->Register(PlatformBase_Property_BlobProperty, MgBlobProperty::CreateObject);
    fact->Register(PlatformBase_Property_BooleanProperty, MgBooleanProperty::CreateObject);
    fact->Register(PlatformBase_Property_ByteProperty, MgByteProperty::CreateObject);
    fact->Register(PlatformBase_Property_ClobProperty, MgClobProperty::CreateObject);
    fact->Register(PlatformBase_Property_DateTimeProperty, MgDateTimeProperty::CreateObject);
    fact->Register(PlatformBase_Property_DoubleProperty, MgDoubleProperty::CreateObject);
    fact->Register(PlatformBase_Property_Int16Property, MgInt16Property::CreateObject);
    fact->Register(PlatformBase_Property_Int32Property, MgInt32Property::CreateObject);
    fact->Register(PlatformBase_Property_Int64Property, MgInt64Property::CreateObject);
    fact->Register(PlatformBase_Property_SingleProperty, MgSingleProperty::CreateObject);
    fact->Register(PlatformBase_Service_Warning, MgWarnings::CreateObject);
    fact->Register(PlatformBase_FeatureService_ClassDefinition, MgClassDefinition::CreateObject);
    fact->Register(PlatformBase_FeatureService_DataPropertyDefinition, MgDataPropertyDefinition::CreateObject);
    fact->Register(PlatformBase_FeatureService_GeometricPropertyDefinition, MgGeometricPropertyDefinition::CreateObject);
    fact->Register(PlatformBase_FeatureService_GeometryProperty, MgGeometryProperty::CreateObject);
    fact->Register(PlatformBase_FeatureService_ObjectPropertyDefinition, MgObjectPropertyDefinition::CreateObject);
    fact->Register(PlatformBase_FeatureService_SpatialContextReader, MgSpatialContextReader::CreateObject);
    fact->Register(PlatformBase_FeatureService_FeatureSet, MgFeatureSet::CreateObject);
    fact->Register(PlatformBase_FeatureService_FeatureProperty, MgFeatureProperty::CreateObject);
    fact->Register(PlatformBase_FeatureService_SpatialContextData, MgSpatialContextData::CreateObject);
    fact->Register(PlatformBase_FeatureService_LongTransactionReader, MgLongTransactionReader::CreateObject);
    fact->Register(PlatformBase_FeatureService_LongTransactionData, MgLongTransactionData::CreateObject);
    fact->Register(PlatformBase_FeatureService_RasterPropertyDefinition, MgRasterPropertyDefinition::CreateObject);
    fact->Register(PlatformBase_FeatureService_Raster, MgRaster::CreateObject);
    fact->Register(PlatformBase_FeatureService_RasterProperty, MgRasterProperty::CreateObject);
    fact->Register(PlatformBase_FeatureService_FeatureQueryOptions, MgFeatureQueryOptions::CreateObject);
    fact->Register(PlatformBase_FeatureService_FeatureAggregateOptions, MgFeatureAggregateOptions::CreateObject);
    fact->Register(PlatformBase_FeatureService_FeatureCommandCollection, MgFeatureCommandCollection::CreateObject);
    fact->Register(PlatformBase_FeatureService_DeleteFeatures, MgDeleteFeatures::CreateObject);
    fact->Register(PlatformBase_FeatureService_InsertFeatures, MgInsertFeatures::CreateObject);
    fact->Register(PlatformBase_FeatureService_UpdateFeatures, MgUpdateFeatures::CreateObject);
    fact->Register(PlatformBase_FeatureService_LockFeatures, MgLockFeatures::CreateObject);
    fact->Register(PlatformBase_FeatureService_UnlockFeatures, MgUnlockFeatures::CreateObject);
    fact->Register(PlatformBase_FeatureService_FeatureSchema, MgFeatureSchema::CreateObject);
    fact->Register(PlatformBase_FeatureService_FeatureSchemaCollection, MgFeatureSchemaCollection::CreateObject);
    fact->Register(PlatformBase_FeatureService_ClassDefinitionCollection, MgClassDefinitionCollection::CreateObject);
    fact->Register(PlatformBase_FeatureService_CreateSdfParams, MgCreateSdfParams::CreateObject);
    fact->Register(PlatformBase_ResourceService_ResourceIdentifier, MgResourceIdentifier::CreateObject);

    fact->Register(PlatformBase_MapLayer_MapBase, MgMapBase::CreateObject);
    fact->Register(PlatformBase_MapLayer_LayerGroup, MgLayerGroup::CreateObject);
    fact->Register(PlatformBase_MapLayer_LayerBase, MgLayerBase::CreateObject);
    fact->Register(PlatformBase_MapLayer_SelectionBase, MgSelectionBase::CreateObject);

    fact->Register(PlatformBase_PrintLayoutService_PrintLayoutBase, MgPrintLayoutBase::CreateObject);
    fact->Register(PlatformBase_PrintLayoutService_PrintLayoutElementBase, MgPrintLayoutElementBase::CreateObject);
    fact->Register(PlatformBase_PrintLayoutService_MapViewportBase, MgMapViewportBase::CreateObject);
    fact->Register(PlatformBase_PrintLayoutService_MapView, MgMapView::CreateObject);
    fact->Register(PlatformBase_PrintLayoutService_PropertyMapping, MgPropertyMapping::CreateObject);

    EXCEPTION_CLASS_CREATOR(MgArrayTypeMismatchException)
    EXCEPTION_CLASS_CREATOR(MgDuplicateResourceDataException)
    EXCEPTION_CLASS_CREATOR(MgDuplicateResourceException)
    EXCEPTION_CLASS_CREATOR(MgEmptyFeatureSetException)
    EXCEPTION_CLASS_CREATOR(MgFdoException)
    EXCEPTION_CLASS_CREATOR(MgFeatureServiceException)
    EXCEPTION_CLASS_CREATOR(MgInvalidMapDefinitionException)
    EXCEPTION_CLASS_CREATOR(MgInvalidRepositoryNameException)
    EXCEPTION_CLASS_CREATOR(MgInvalidRepositoryTypeException)
    EXCEPTION_CLASS_CREATOR(MgInvalidResourceDataNameException)
    EXCEPTION_CLASS_CREATOR(MgInvalidResourceDataTypeException)
    EXCEPTION_CLASS_CREATOR(MgInvalidResourceNameException)
    EXCEPTION_CLASS_CREATOR(MgInvalidResourcePathException)
    EXCEPTION_CLASS_CREATOR(MgInvalidResourcePreProcessingTypeException)
    EXCEPTION_CLASS_CREATOR(MgInvalidResourceTypeException)
    EXCEPTION_CLASS_CREATOR(MgLayerNotFoundException)
    EXCEPTION_CLASS_CREATOR(MgResourceBusyException)
    EXCEPTION_CLASS_CREATOR(MgResourceDataNotFoundException)
    EXCEPTION_CLASS_CREATOR(MgResourceNotFoundException)
    EXCEPTION_CLASS_CREATOR(MgServiceNotAvailableException)
    EXCEPTION_CLASS_CREATOR(MgServiceNotSupportedException)
    EXCEPTION_CLASS_CREATOR(MgUserNotFoundException)

    return true;
}



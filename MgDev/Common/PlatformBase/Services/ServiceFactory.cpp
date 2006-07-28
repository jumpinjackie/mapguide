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

#include "PlatformBase.h"

#ifdef WIN32
#include "windows.h"
#else
#include <dlfcn.h>
#endif


#include "Foundation.h"

// Process-wide MgServiceFactory
Ptr<MgServiceFactory> MgServiceFactory::m_serviceFactory = (MgServiceFactory*)NULL;

MgServiceFactory::MgServiceFactory()
{
}

MgServiceFactory* MgServiceFactory::GetInstance()
{
    MG_TRY()

    ACE_TRACE ("MgServiceFactory::GetInstance");

    if (MgServiceFactory::m_serviceFactory == NULL)
    {
        // Perform Double-Checked Locking Optimization.
        ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, *ACE_Static_Object_Lock::instance (), 0));
        if (MgServiceFactory::m_serviceFactory == NULL)
        {
            MgServiceFactory::m_serviceFactory = new MgServiceFactory();
        }
    }

    MG_CATCH_AND_THROW(L"MgServiceFactory.GetInstance")

    // To avoid overhead and maintain thread safety,
    // do not assign this returned static singleton to a Ptr object.
    return MgServiceFactory::m_serviceFactory;
}


void MgServiceFactory::Dispose()
{
    delete this;
}


MgService* MgServiceFactory::CreateService(INT16 serviceType, ServerConnectionType connType)
{
    ServiceRegistry registry = m_serviceCreators[connType];
    const ServiceCreatorFunc& func = registry[serviceType];
    if (NULL == func)
    {
        throw new MgServiceNotSupportedException(L"MgServiceFactory.CreateService", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MgService* obj = (*func)();
    return obj;
}


void MgServiceFactory::RegisterService(INT16 serviceType, ServiceCreatorFunc creator, ServerConnectionType connType)

{
    if (NULL == creator)
    {
        throw new MgInvalidArgumentException(L"MgServiceFactory.RegisterClass", __LINE__, __WFILE__, NULL, L"", NULL);
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
    fact->Register(PlatformBase_CoreService_Warning, MgWarnings::CreateObject);
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
    fact->Register(PlatformBase_FeatureService_FeatureSchema, MgFeatureSchema::CreateObject);
    fact->Register(PlatformBase_FeatureService_FeatureSchemaCollection, MgFeatureSchemaCollection::CreateObject);
    fact->Register(PlatformBase_FeatureService_ClassDefinitionCollection, MgClassDefinitionCollection::CreateObject);
    fact->Register(PlatformBase_FeatureService_CreateSdfParams, MgCreateSdfParams::CreateObject);
    fact->Register(PlatformBase_ResourceService_ResourceIdentifier, MgResourceIdentifier::CreateObject);

    EXCEPTION_CLASS_CREATOR(MgArrayTypeMismatchException)
    EXCEPTION_CLASS_CREATOR(MgInvalidMapDefinitionException)
    EXCEPTION_CLASS_CREATOR(MgInvalidRepositoryNameException)
    EXCEPTION_CLASS_CREATOR(MgInvalidRepositoryTypeException)
    EXCEPTION_CLASS_CREATOR(MgInvalidResourceNameException)
    EXCEPTION_CLASS_CREATOR(MgInvalidResourcePathException)
    EXCEPTION_CLASS_CREATOR(MgInvalidResourceTypeException)
    EXCEPTION_CLASS_CREATOR(MgInvalidResourceNameException)
    EXCEPTION_CLASS_CREATOR(MgServiceNotSupportedException)


    return true;
}


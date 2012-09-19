#include "FSDSAX2Parser.h"
#include "FeatureService.h"
#include "ResourceService.h"
#include "ServiceFactory.h"
#include "Fdo.h"
#include "PlatformBase.h"

#include "FeatureReader.h"
#include "ScrollableFeatureReader.h"
#include "DataReader.h"
#include "SqlReader.h"

#include "Services/Feature/FeatureUtil.h"
#include "Services/Feature/ProjectedFeatureReader.h"

#include "Services/Feature/Commands/ApplySchema.h"
#include "Services/Feature/Commands/CreateFeatureSource.h"
#include "Services/Feature/Commands/DescribeSchema.h"
#include "Services/Feature/Commands/EnumerateDataStores.h"
#include "Services/Feature/Commands/GetConnectionPropertyValues.h"
#include "Services/Feature/Commands/GetFeatureProviders.h"
#include "Services/Feature/Commands/GetLongTransactions.h"
#include "Services/Feature/Commands/GetProviderCapabilities.h"
#include "Services/Feature/Commands/GetSchemaMapping.h"
#include "Services/Feature/Commands/GetSpatialContexts.h"
#include "Services/Feature/Commands/SelectFeatures.h"
#include "Services/Feature/Commands/SqlCommand.h"
#include "Services/Feature/Commands/UpdateFeatures.h"

MgdFeatureService::MgdFeatureService() 
: MgFeatureService()
{ 
}

MgdFeatureService::~MgdFeatureService() 
{ 
	
}

MgByteReader* MgdFeatureService::GetFeatureProviders() 
{ 
    Ptr<MgByteReader> reader;

    MG_LOG_OPERATION_MESSAGE(L"GetFeatureProviders");

    MG_FEATURE_SERVICE_TRY()

    MgdGetFeatureProviders getProviders;
    reader = getProviders.GetFeatureProviders();

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_FEATURE_SERVICE_CATCH(L"MgdFeatureService::GetFeatureProviders")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_FEATURE_SERVICE_THROW()

	return reader.Detach();
}

MgStringCollection* MgdFeatureService::GetConnectionPropertyValues(CREFSTRING providerName,
                                                                   CREFSTRING propertyName,
                                                                   CREFSTRING partialConnString) 
{ 
    Ptr<MgStringCollection> values;
    MG_LOG_OPERATION_MESSAGE(L"GetConnectionPropertyValues");

    MG_FEATURE_SERVICE_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 3);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(providerName.c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(propertyName.c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(partialConnString.c_str());
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdFeatureService::GetConnectionPropertyValues()");

    MgdGetConnectionPropertyValues cmd;
    values = cmd.GetConnectionPropertyValues(providerName, propertyName, partialConnString);

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_FEATURE_SERVICE_CATCH(L"MgdFeatureService::GetConnectionPropertyValues")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_FEATURE_SERVICE_THROW()

	return values.Detach();
}

bool MgdFeatureService::TestConnection(CREFSTRING providerName, CREFSTRING connectionString) 
{
	bool ok = false;

    MG_LOG_OPERATION_MESSAGE(L"TestConnection");

    MG_FEATURE_SERVICE_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 2);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(providerName.c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(connectionString.c_str());
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdFeatureService::TestConnection()");

	Ptr<MgdFeatureConnection> conn = new MgdFeatureConnection(providerName, connectionString);
    ok = conn->IsConnectionOpen();

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_FEATURE_SERVICE_CATCH(L"MgdFeatureService::TestConnection")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_FEATURE_SERVICE_THROW()

	return ok;
}

bool MgdFeatureService::TestConnection(MgResourceIdentifier* resource) 
{
    bool ok = false;

    MG_LOG_OPERATION_MESSAGE(L"TestConnection");

    MG_FEATURE_SERVICE_TRY()

    CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdFeatureService::TestConnection");

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 1);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdFeatureService::TestConnection()");

	Ptr<MgdFeatureConnection> conn = new MgdFeatureConnection(resource);
	ok = conn->IsConnectionOpen();

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_FEATURE_SERVICE_CATCH_WITH_FEATURE_SOURCE(L"MgdFeatureService::TestConnection", resource)

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_FEATURE_SERVICE_THROW()

	return ok;
}

MgByteReader* MgdFeatureService::GetCapabilities(CREFSTRING providerName) 
{
    Ptr<MgByteReader> ret;
    
    MG_LOG_OPERATION_MESSAGE(L"GetCapabilities");

    MG_FEATURE_SERVICE_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 1);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(providerName.c_str());
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdFeatureService::GetCapabilities()");

	ret = GetCapabilities(providerName, L"");

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_FEATURE_SERVICE_CATCH(L"MgdFeatureService::GetCapabilities")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_FEATURE_SERVICE_THROW()

    return ret.Detach();
}

MgByteReader* MgdFeatureService::GetCapabilities(CREFSTRING providerName, CREFSTRING connectionString) 
{
    Ptr<MgByteReader> caps;

    MG_LOG_OPERATION_MESSAGE(L"GetCapabilities");

    MG_FEATURE_SERVICE_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 2);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(providerName.c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(connectionString.c_str());
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdFeatureService::GetCapabilities()");

    MgdGetProviderCapabilities getCaps(providerName, connectionString);
    caps = getCaps.GetProviderCapabilities();
    
    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_FEATURE_SERVICE_CATCH(L"MgdFeatureService::GetCapabilities")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_FEATURE_SERVICE_THROW()

	return caps.Detach();
}

void MgdFeatureService::ApplySchema(MgResourceIdentifier* resource, MgFeatureSchema* schema) 
{ 
    MG_LOG_OPERATION_MESSAGE(L"ApplySchema");

    MG_FEATURE_SERVICE_TRY()

    CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdFeatureService::ApplySchema");
    CHECKARGUMENTNULL(schema, L"MgdFeatureService::ApplySchema");

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 2);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgFeatureSchema");
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdFeatureService::ApplySchema()");

    MgdApplySchema apply;
    apply.ApplySchema(resource, schema);

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH(resource, L"MgdFeatureService::ApplySchema")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_FEATURE_SERVICE_THROW()
}

MgFeatureSchemaCollection* MgdFeatureService::DescribeSchema(MgResourceIdentifier* resource,
                                                             CREFSTRING schemaName,
                                                             MgStringCollection* classNames) 
{
    Ptr<MgFeatureSchemaCollection> ret;

    MG_LOG_OPERATION_MESSAGE(L"DescribeSchema");

    MG_FEATURE_SERVICE_TRY()
    
    CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdFeatureService::DescribeSchema");
    
    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 3);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(schemaName.c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == classNames) ? L"MgStringCollection" : classNames->GetLogString().c_str());
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdFeatureService::DescribeSchema()");

    ret = DescribeSchema(resource, schemaName, classNames, true);

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_FEATURE_SERVICE_CATCH_WITH_FEATURE_SOURCE(L"MgdFeatureService::DescribeSchema", resource)

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_FEATURE_SERVICE_THROW()

    return ret.Detach();
}

MgFeatureSchemaCollection* MgdFeatureService::DescribeSchema(MgResourceIdentifier* resource,
                                                             CREFSTRING schemaName,
                                                             MgStringCollection* findClassNames,
                                                             bool serialize) 
{ 
	CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdFeatureService::DescribeSchema");

    Ptr<MgFeatureSchemaCollection> fsCollection;
    Ptr<MgFeatureSchemaCollection> clone;
    MG_FEATURE_SERVICE_TRY()

    MgdDescribeSchema describe;
    MgStringCollection extendedClassNames;
    //Need to weed out extended class names as this API is not extended class aware.
    //TODO: Put this logic into DescribeSchema proper
    if (NULL != findClassNames)
    {
        for (INT32 i = 0; i < findClassNames->GetCount(); i++)
        {
            STRING name = findClassNames->GetItem(i);
            if (describe.CheckExtendedFeatureClass(resource, name))
            {
                extendedClassNames.Add(name);
            }
        }
        for (INT32 i = 0; i < extendedClassNames.GetCount(); i++)
        {
            findClassNames->Remove(extendedClassNames.GetItem(i));
        }
    }
    fsCollection = describe.DescribeSchema(resource, schemaName, findClassNames, serialize);
    
    //In MapGuide Server returning this potentially cached copy is okay as a new instance is created
    //travelling the server/webtier boundary. This is not the case in mg-desktop so we have to clone
    //this result and return the clone instead.
    clone = new MgFeatureSchemaCollection();
    for (INT32 i = 0; i < fsCollection->GetCount(); i++)
    {
        Ptr<MgFeatureSchema> schema = fsCollection->GetItem(i);
        Ptr<MgFeatureSchema> clonedSchema = MgdFeatureUtil::CloneMgSchema(schema);
        clone->Add(clonedSchema);
    }

    //TODO: Multiple schemas without class names?
    Ptr<MgFeatureSchema> schema = fsCollection->GetItem(0);
    Ptr<MgClassDefinitionCollection> parentClasses = schema->GetClasses();

    //Add extended classes
    if (extendedClassNames.GetCount() > 0)
    {
        for (INT32 i = 0; i < extendedClassNames.GetCount(); i++)
        {
            STRING clsName = extendedClassNames.GetItem(i);
            //This API is extended class aware
            Ptr<MgClassDefinition> klass = GetClassDefinition(resource, schemaName, clsName);
            Ptr<MgClassDefinition> cloneCls = MgdFeatureUtil::CloneMgClassDefinition(klass);
            parentClasses->Add(cloneCls);
        }
    }

    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH_AND_THROW(resource, L"MgdFeatureService::DescribeSchema")

    return clone.Detach();
}

MgFeatureSchemaCollection* MgdFeatureService::DescribeSchema(MgResourceIdentifier* resource,
                                                             CREFSTRING schemaName) 
{ 
    Ptr<MgFeatureSchemaCollection> ret;
    MG_LOG_OPERATION_MESSAGE(L"DescribeSchema");

    MG_FEATURE_SERVICE_TRY()

    CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdFeatureService::DescribeSchema");

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 2);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(schemaName.c_str());
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdFeatureService::DescribeSchema()");

	ret = DescribeSchema(resource, schemaName, NULL);
    
    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_FEATURE_SERVICE_CATCH_WITH_FEATURE_SOURCE(L"MgdFeatureService::DescribeSchema", resource)

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_FEATURE_SERVICE_THROW()

    return ret.Detach();
}

STRING MgdFeatureService::DescribeSchemaAsXml(MgResourceIdentifier* resource,
                                              CREFSTRING schemaName,
                                              MgStringCollection* classNames) 
{
    STRING schemaXml;

    MG_LOG_OPERATION_MESSAGE(L"DescribeSchemaAsXml");

    MG_FEATURE_SERVICE_TRY()

    CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdFeatureService::DescribeSchemaAsXml");

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 3);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(schemaName.c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == classNames) ? L"MgStringCollection" : classNames->GetLogString().c_str());
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdFeatureService::DescribeSchemaAsXml()");

    MgdDescribeSchema describe;
    schemaXml = describe.DescribeSchemaAsXml(resource, schemaName, classNames);

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_FEATURE_SERVICE_CATCH_WITH_FEATURE_SOURCE(L"MgdFeatureService::DescribeSchemaAsXml", resource)

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_FEATURE_SERVICE_THROW()

    return schemaXml;
}

STRING MgdFeatureService::DescribeSchemaAsXml(MgResourceIdentifier* resource,
                                              CREFSTRING schemaName) 
{ 
    STRING ret;
    MG_LOG_OPERATION_MESSAGE(L"DescribeSchemaAsXml");

    MG_FEATURE_SERVICE_TRY()

    CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdFeatureService:DescribeSchemaAsXml");

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 2);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(schemaName.c_str());
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdFeatureService::DescribeSchemaAsXml()");

	ret = DescribeSchemaAsXml(resource, schemaName, NULL);
    
    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_FEATURE_SERVICE_CATCH_WITH_FEATURE_SOURCE(L"MgdFeatureService::DescribeSchemaAsXml", resource)

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_FEATURE_SERVICE_THROW()

    return ret;
}

STRING MgdFeatureService::SchemaToXml(MgFeatureSchemaCollection* schemas) 
{ 
    STRING serializedXml;
    MG_LOG_OPERATION_MESSAGE(L"SchemaToXml");

    MG_FEATURE_SERVICE_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 1);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgFeatureSchemaCollection");
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdFeatureService::SchemaToXml()");

    CHECKARGUMENTNULL(schemas, L"MgdFeatureService::SchemaToXml");

    MgdDescribeSchema describe;
    serializedXml = describe.SchemaToXml(schemas);

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_FEATURE_SERVICE_CATCH(L"MgdFeatureService::SchemaToXml")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_FEATURE_SERVICE_THROW()

    return serializedXml;
}

MgFeatureSchemaCollection* MgdFeatureService::XmlToSchema(CREFSTRING xml) 
{ 
	Ptr<MgFeatureSchemaCollection> mgSchemaCol;

    MG_LOG_OPERATION_MESSAGE(L"XmlToSchema");

    MG_FEATURE_SERVICE_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 1);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"STRING");
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdFeatureService::XmlToSchema()");

    MgdDescribeSchema describe;
    mgSchemaCol = describe.XmlToSchema(xml);

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_FEATURE_SERVICE_CATCH(L"MgdFeatureService::XmlToSchema")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_FEATURE_SERVICE_THROW()

    return mgSchemaCol.Detach();
}

MgFeatureReader* MgdFeatureService::SelectFeatures(MgResourceIdentifier* resource,
                                                   CREFSTRING className,
                                                   MgFeatureQueryOptions* options) 
{ 
    Ptr<MgFeatureReader> ret;

    MG_LOG_OPERATION_MESSAGE(L"SelectFeatures");

    MG_FEATURE_SERVICE_TRY()

    CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdFeatureService::SelectFeatures");

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 3);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(className.c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgFeatureQueryOptions");
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MgdLogDetail logDetail(MgServiceType::FeatureService, MgdLogDetail::Trace, L"MgdFeatureService::SelectFeatures", mgStackParams);
    logDetail.AddResourceIdentifier(L"Resource", resource);
    logDetail.AddString(L"ClassName", className);
    logDetail.AddObject(L"Options", options);
    logDetail.Create();

    MgdSelectFeatures select;
    ret = dynamic_cast<MgFeatureReader*>(select.SelectFeatures(resource, className, options, false));
    CHECKNULL(ret.p, L"MgdFeatureService::SelectFeatures");

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_FEATURE_SERVICE_CATCH_WITH_FEATURE_SOURCE(L"MgdFeatureService::SelectFeatures", resource)

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_FEATURE_SERVICE_THROW()

    return ret.Detach();
}

MgFeatureReader* MgdFeatureService::SelectFeatures(MgResourceIdentifier* resource,
                                                   CREFSTRING className,
                                                   MgFeatureQueryOptions* options,
                                                   CREFSTRING coordinateSystem) 
{ 
    Ptr<MgFeatureReader> ret;
    MG_LOG_OPERATION_MESSAGE(L"SelectFeatures");

    MG_FEATURE_SERVICE_TRY()

    CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdFeatureService::SelectFeatures");

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 4);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(className.c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgFeatureQueryOptions");
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"STRING");
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

	MgdLogDetail logDetail(MgServiceType::FeatureService, MgdLogDetail::Trace, L"MgdFeatureService::SelectFeatures", mgStackParams);
    logDetail.AddResourceIdentifier(L"Resource", resource);
    logDetail.AddString(L"ClassName", className);
    logDetail.AddObject(L"Options", options);
    logDetail.AddString(L"CoordinateSystem", coordinateSystem);
    logDetail.Create();

    MgdSelectFeatures select;
    Ptr<MgFeatureReader> innerReader = dynamic_cast<MgFeatureReader*>(select.SelectFeatures(resource, className, options, false));
    CHECKNULL(innerReader.p, L"MgdFeatureService::SelectFeatures");

    //If coordinate system specified, set up a CS Transform to pass to the feature reader
    Ptr<MgCoordinateSystemTransform> xform;
    if (!coordinateSystem.empty())
    {
        Ptr<MgClassDefinition> clsDef = innerReader->GetClassDefinition();
        if (NULL != clsDef)
        {
            Ptr<MgPropertyDefinitionCollection> clsProps = clsDef->GetProperties();
            STRING geomProp = clsDef->GetDefaultGeometryPropertyName();
            if (!geomProp.empty() && clsProps->IndexOf(geomProp) >= 0)
            {
                Ptr<MgGeometricPropertyDefinition> geomPropDef = static_cast<MgGeometricPropertyDefinition*>(clsProps->GetItem(geomProp));
                STRING scName = geomPropDef->GetSpatialContextAssociation();
                Ptr<MgSpatialContextReader> scReader = GetSpatialContexts(resource, false);
                while (scReader->ReadNext())
                {
                    if (scReader->GetName() == scName)
                    {
                        //TODO: Some FDO providers don't even use WKTs for spatial context. Capabilities
                        //should tell us the correct way to construct our MgCoordinateSystem object

                        STRING wkt = scReader->GetCoordinateSystemWkt();

                        Ptr<MgCoordinateSystemFactory> fact = new MgCoordinateSystemFactory();

                        Ptr<MgCoordinateSystem> source = fact->Create(wkt);
                        Ptr<MgCoordinateSystem> target = fact->Create(coordinateSystem);

                        xform = fact->GetTransform(source, target);
                        break;
                    }
                }
            }
        }
    }
    
    if (NULL != xform)
    {
        ret = new MgdProjectedFeatureReader(innerReader, xform);
    }
    else
    {
        ret = innerReader;
    }
    
    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_FEATURE_SERVICE_CATCH_WITH_FEATURE_SOURCE(L"MgdFeatureService::SelectFeatures", resource)

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_FEATURE_SERVICE_THROW()

    return ret.Detach();
}

MgdScrollableFeatureReader* MgdFeatureService::SelectFeaturesExtended(MgResourceIdentifier* resource,
                                                                      CREFSTRING className,
                                                                      MgFeatureQueryOptions* options)
{
    Ptr<MgdScrollableFeatureReader> ret;
    MG_LOG_OPERATION_MESSAGE(L"SelectFeaturesExtended");

    MG_FEATURE_SERVICE_TRY()

    CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdFeatureService::SelectFeatureExtended");

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 3);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(className.c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgFeatureQueryOptions");
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdFeatureService::SelectFeaturesExtended()");

    MgdLogDetail logDetail(MgServiceType::FeatureService, MgdLogDetail::Trace, L"MgdFeatureService::SelectFeaturesExtended", mgStackParams);
    logDetail.AddResourceIdentifier(L"Resource", resource);
    logDetail.AddString(L"ClassName", className);
    logDetail.AddObject(L"Options", options);
    logDetail.Create();

    MgdSelectFeatures select;
    ret = dynamic_cast<MgdScrollableFeatureReader*>(select.SelectFeatures(resource, className, options, false, true));
    CHECKNULL(ret.p, L"MgdFeatureService::SelectFeaturesExtended");
    
    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_FEATURE_SERVICE_CATCH_WITH_FEATURE_SOURCE(L"MgdFeatureService::SelectFeaturesExtended", resource)

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_FEATURE_SERVICE_THROW()

    return ret.Detach();
}

MgDataReader* MgdFeatureService::SelectAggregate(MgResourceIdentifier* resource,
                                                 CREFSTRING className,
                                                 MgFeatureAggregateOptions* options) 
{ 
    Ptr<MgDataReader> reader;
    MG_LOG_OPERATION_MESSAGE(L"SelectAggregate");

    MG_FEATURE_SERVICE_TRY()

    CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdFeatureService::SelectAggregate");
	CHECKARGUMENTNULL(options, L"MgdFeatureService::SelectAggregate");

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 3);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(className.c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgFeatureAggregateOptions");
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MgdLogDetail logDetail(MgServiceType::FeatureService, MgdLogDetail::Trace, L"MgdFeatureService::SelectAggregate", mgStackParams);
    logDetail.AddResourceIdentifier(L"Resource", resource);
    logDetail.AddString(L"ClassName", className);
    logDetail.AddObject(L"Options", options);
    logDetail.Create();

    MgdSelectFeatures select;
    reader = dynamic_cast<MgDataReader*>(select.SelectFeatures(resource, className, options, true));
    CHECKNULL(reader.p, L"MgdFeatureService::SelectAggregate");

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH(resource, L"MgdFeatureService::SelectAggregate")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_FEATURE_SERVICE_THROW()

	return reader.Detach();
}

MgPropertyCollection* MgdFeatureService::UpdateFeatures(MgResourceIdentifier* resource,
                                                        MgFeatureCommandCollection* commands,
                                                        bool useTransaction) 
{ 
    Ptr<MgPropertyCollection> result;
    MG_LOG_OPERATION_MESSAGE(L"UpdateFeatures");

    MG_FEATURE_SERVICE_TRY()

    CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdFeatureService::UpdateFeatures");
    CHECKARGUMENTNULL(commands, L"MgdFeatureService::UpdateFeatures");

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 3);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgFeatureCommandCollection");
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_BOOL(useTransaction);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdFeatureService::UpdateFeatures()");

    MgdUpdateFeaturesCommand cmd;
    result = cmd.Execute(resource, commands, useTransaction);

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH(resource, L"MgdFeatureService::UpdateFeatures")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_FEATURE_SERVICE_THROW()

    return result.Detach();
}

MgPropertyCollection* MgdFeatureService::UpdateFeatures(MgResourceIdentifier* resource,
                                                        MgFeatureCommandCollection* commands,
                                                        MgTransaction* transaction) 
{ 
    Ptr<MgPropertyCollection> ret;
    MG_LOG_OPERATION_MESSAGE(L"UpdateFeatures");

    MG_FEATURE_SERVICE_TRY()

    CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdFeatureService::UpdateFeatures");

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 3);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgFeatureCommandCollection");
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgTransaction");
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdFeatureService::UpdateFeatures()");

	CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdFeatureService::UpdateFeatures");
    CHECKARGUMENTNULL(commands, L"MgdFeatureService::UpdateFeatures");

    MgdUpdateFeaturesCommand cmd;
    ret = cmd.Execute(resource, commands, transaction);

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH(resource, L"MgdFeatureService::UpdateFeatures")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_FEATURE_SERVICE_THROW()

    return ret.Detach();
}

MgFeatureReader* MgdFeatureService::InsertFeatures(MgResourceIdentifier* resource, CREFSTRING className, MgPropertyCollection* propertyValues)
{
    Ptr<MgFeatureReader> ret;
    MG_LOG_OPERATION_MESSAGE(L"InsertFeatures");

    MG_FEATURE_SERVICE_TRY()

    CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdFeatureService::InsertFeatures");

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 3);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(className.c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgPropertyCollection");
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdFeatureService::InsertFeatures()");

	MgdUpdateFeaturesCommand cmd;
    ret = cmd.ExecuteInsert(resource, className, propertyValues, NULL);

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_FEATURE_SERVICE_CATCH_WITH_FEATURE_SOURCE(L"MgdFeatureService::InsertFeatures", resource)

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_FEATURE_SERVICE_THROW()

    return ret.Detach();
}

MgFeatureReader* MgdFeatureService::InsertFeatures(MgResourceIdentifier* resource, CREFSTRING className, MgPropertyCollection* propertyValues, MgTransaction* trans)
{
    Ptr<MgFeatureReader> reader;
    MG_LOG_OPERATION_MESSAGE(L"InsertFeatures");

    MG_FEATURE_SERVICE_TRY()

    CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdFeatureService::InsertFeatures");

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 4);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(className.c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgPropertyCollection");
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgTransaction");
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdFeatureService::InsertFeatures()");

    MgdUpdateFeaturesCommand cmd;
    reader = cmd.ExecuteInsert(resource, className, propertyValues, trans);

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH(resource, L"MgdFeatureService::InsertFeatures")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_FEATURE_SERVICE_THROW()

	return reader.Detach();
}

MgPropertyCollection* MgdFeatureService::InsertFeatures(MgResourceIdentifier* resource, CREFSTRING className, MgBatchPropertyCollection* batchPropertyValues)
{
    Ptr<MgPropertyCollection> ret;
    MG_LOG_OPERATION_MESSAGE(L"InsertFeatures");

    MG_FEATURE_SERVICE_TRY()

    CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdFeatureService::InsertFeatures");

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 3);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(className.c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgBatchPropertyCollection");
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdFeatureService::InsertFeatures()");

    MgdUpdateFeaturesCommand cmd;
    ret = cmd.ExecuteInsert(resource, className, batchPropertyValues, NULL);
    
    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_FEATURE_SERVICE_CATCH_WITH_FEATURE_SOURCE(L"MgdFeatureService::InsertFeatures", resource)

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_FEATURE_SERVICE_THROW()
    return ret.Detach();
}

MgPropertyCollection* MgdFeatureService::InsertFeatures(MgResourceIdentifier* resource, CREFSTRING className, MgBatchPropertyCollection* batchPropertyValues, MgTransaction* trans)
{
    Ptr<MgPropertyCollection> ret;
    MG_LOG_OPERATION_MESSAGE(L"InsertFeatures");

    MG_FEATURE_SERVICE_TRY()

    CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdFeatureService::InsertFeatures");

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 4);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(className.c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgBatchPropertyCollection");
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgTransaction");
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdFeatureService::InsertFeatures()");

    MgdUpdateFeaturesCommand cmd;
    ret = cmd.ExecuteInsert(resource, className, batchPropertyValues, trans);

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH(resource, L"MgdFeatureService::InsertFeatures")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_FEATURE_SERVICE_THROW()
    return ret.Detach();
}

int MgdFeatureService::UpdateFeatures(MgResourceIdentifier* resource, CREFSTRING className, MgPropertyCollection* propertyValues, CREFSTRING filter)
{
    int ret = 0;
    MG_LOG_OPERATION_MESSAGE(L"UpdateFeatures");

    MG_FEATURE_SERVICE_TRY()

    CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdFeatureService::UpdateFeatures");

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 4);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(className.c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgPropertyCollection");
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"STRING");
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdFeatureService::UpdateFeatures()");

	ret = UpdateFeatures(resource, className, propertyValues, filter, NULL);
    
    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_FEATURE_SERVICE_CATCH_WITH_FEATURE_SOURCE(L"MgdFeatureService::UpdateFeatures", resource)

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_FEATURE_SERVICE_THROW()
    return ret;
}

int MgdFeatureService::UpdateFeatures(MgResourceIdentifier* resource, CREFSTRING className, MgPropertyCollection* batchPropertyValues, CREFSTRING filter, MgTransaction* trans)
{
    int updated = 0;
    MG_LOG_OPERATION_MESSAGE(L"UpdateFeatures");

    MG_FEATURE_SERVICE_TRY()

    CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdFeatureService::UpdateFeatures");

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 5);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(className.c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgPropertyCollection");
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"STRING");
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgTransaction");
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdFeatureService::UpdateFeatures()");

    MgdUpdateFeaturesCommand cmd;
    updated = cmd.ExecuteUpdate(resource, className, batchPropertyValues, filter, trans);

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH(resource, L"MgdFeatureService::UpdateFeatures")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_FEATURE_SERVICE_THROW()

	return updated;
}

int MgdFeatureService::DeleteFeatures(MgResourceIdentifier* resource, CREFSTRING className, CREFSTRING filter)
{
    int deleted = 0;
    MG_LOG_OPERATION_MESSAGE(L"DeleteFeatures");

    MG_FEATURE_SERVICE_TRY()

    CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdFeatureService::DeleteFeatures");

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 3);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(className.c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"STRING");
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdFeatureService::DeleteFeatures()");

	MgdUpdateFeaturesCommand cmd;
    deleted = cmd.ExecuteDelete(resource, className, filter, NULL);
    
    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_FEATURE_SERVICE_CATCH_WITH_FEATURE_SOURCE(L"MgdFeatureService::DeleteFeatures", resource)

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_FEATURE_SERVICE_THROW()
    return deleted;
}

int MgdFeatureService::DeleteFeatures(MgResourceIdentifier* resource, CREFSTRING className, CREFSTRING filter, MgTransaction* trans)
{
    int deleted = 0;
    MG_LOG_OPERATION_MESSAGE(L"DeleteFeatures");

    MG_FEATURE_SERVICE_TRY()

    CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdFeatureService::DeleteFeatures");

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 4);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(className.c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"STRING");
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgTransaction");
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdFeatureService::DeleteFeatures()");

    MgdUpdateFeaturesCommand cmd;
    deleted = cmd.ExecuteDelete(resource, className, filter, trans);

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH(resource, L"MgdFeatureService::DeleteFeatures")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_FEATURE_SERVICE_THROW()

	return deleted;
}

void MgdFeatureService::PurgeCache(MgResourceIdentifier* resource)
{
    MG_LOG_OPERATION_MESSAGE(L"PurgeCache");

    MG_FEATURE_SERVICE_TRY()

    CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdFeatureService::PurgeCache");

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 1);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdFeatureService::PurgeCache()");

    MgdFeatureServiceCache* cache = MgdFeatureServiceCache::GetInstance();
    cache->RemoveEntry(resource);
    MgdFdoConnectionPool::PurgeCachedConnections(resource);

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_FEATURE_SERVICE_CATCH_WITH_FEATURE_SOURCE(L"MgdFeatureService::PurgeCache", resource)

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_FEATURE_SERVICE_THROW()
}

MgFeatureReader* MgdFeatureService::GetLockedFeatures(MgResourceIdentifier* resource,
                                                      CREFSTRING className,
                                                      MgFeatureQueryOptions* options) 
{ 
    Ptr<MgFeatureReader> ret;
    MG_LOG_OPERATION_MESSAGE(L"GetLockedFeatures");

    MG_FEATURE_SERVICE_TRY()

    CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdFeatureService::GetLockedFeatures");

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 3);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(className.c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgFeatureQueryOptions");
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdFeatureService::GetLockedFeatures()");

	MgdSelectFeatures select;
    ret = dynamic_cast<MgFeatureReader*>(select.SelectFeatures(resource, className, options, false, false, true));
    CHECKNULL(ret.p, L"MgdFeatureService::GetLockedFeatures");

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_FEATURE_SERVICE_CATCH_WITH_FEATURE_SOURCE(L"MgdFeatureService::GetLockedFeatures", resource)

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_FEATURE_SERVICE_THROW()

    return ret.Detach();
}

MgTransaction* MgdFeatureService::BeginTransaction(MgResourceIdentifier* resource) 
{ 
    Ptr<MgTransaction> trans;
    MG_LOG_OPERATION_MESSAGE(L"BeginTransaction");

    MG_FEATURE_SERVICE_TRY()

    CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdFeatureService::BeginTransaction");

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 1);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdFeatureService::BeginTransaction()");

    Ptr<MgdFeatureConnection> conn = new MgdFeatureConnection(resource);
    trans = new MgdTransaction(conn, resource);

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH(resource, L"MgdFeatureService::BeginTransaction")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_FEATURE_SERVICE_THROW()

    return trans.Detach();
}

MgSqlDataReader* MgdFeatureService::ExecuteSqlQuery(MgResourceIdentifier* resource, CREFSTRING sqlStatement) 
{
    Ptr<MgSqlDataReader> reader;
    MG_LOG_OPERATION_MESSAGE(L"ExecuteSqlQuery");

    MG_FEATURE_SERVICE_TRY()

    CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdFeatureService::ExecuteSqlQuery");
    if (sqlStatement.empty())
        throw new MgInvalidArgumentException(L"MgdFeatureService::ExecuteSqlQuery", __LINE__, __WFILE__, NULL, L"", NULL);

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 2);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(sqlStatement.c_str());
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdFeatureService::ExecuteSqlQuery()");

	MgdSqlCommand cmd;
    reader = cmd.ExecuteQuery(resource, sqlStatement, NULL, NULL);

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH(resource, L"MgdFeatureService::ExecuteSqlQuery")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_FEATURE_SERVICE_THROW()

	return reader.Detach();
}

MgSqlDataReader* MgdFeatureService::ExecuteSqlQuery(MgResourceIdentifier* resource,
                                                    CREFSTRING sqlStatement,
                                                    MgParameterCollection* params,
                                                    MgTransaction* trans) 
{
    Ptr<MgSqlDataReader> ret;
    MG_LOG_OPERATION_MESSAGE(L"ExecuteSqlQuery");

    MG_FEATURE_SERVICE_TRY()

    CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdFeatureService::ExecuteSqlQuery");

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 4);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(sqlStatement.c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgParameterCollection");
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgTransaction");
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdFeatureService::ExecuteSqlQuery()");

    MgdSqlCommand cmd;
    ret = cmd.ExecuteQuery(resource, sqlStatement, params, trans);

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_FEATURE_SERVICE_CATCH_WITH_FEATURE_SOURCE(L"MgdFeatureService::ExecuteSqlQuery", resource)

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_FEATURE_SERVICE_THROW()
    return ret.Detach();
}

MgSqlDataReader* MgdFeatureService::ExecuteSqlQuery(MgResourceIdentifier* resource,
                                                     CREFSTRING sqlStatement,
                                                     MgParameterCollection* params,
                                                     MgTransaction* trans,
                                                     INT32 fetchSize)
{
    Ptr<MgSqlDataReader> reader;
    MG_FEATURE_SERVICE_TRY()

	CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdFeatureService::ExecuteSqlQuery");
    if (sqlStatement.empty())
        throw new MgInvalidArgumentException(L"MgdFeatureService::ExecuteSqlQuery", __LINE__, __WFILE__, NULL, L"", NULL);

    CHECKARGUMENTNULL(params, L"MgdFeatureService::ExecuteSqlQuery");
    CHECKARGUMENTNULL(trans, L"MgdFeatureService::ExecuteSqlQuery");

    MgdSqlCommand cmd;
    reader = cmd.ExecuteQuery(resource, sqlStatement, params, trans, fetchSize);

    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH_AND_THROW(resource, L"MgdFeatureService::ExecuteSqlQuery")
    return reader.Detach();
}

INT32 MgdFeatureService::ExecuteSqlNonQuery(MgResourceIdentifier* resource,
                                            CREFSTRING sqlNonSelectStatement,
                                            MgParameterCollection* params,
                                            MgTransaction* trans)
{
    INT32 ret = 0;
    MG_LOG_OPERATION_MESSAGE(L"ExecuteSqlNonQuery");

    MG_FEATURE_SERVICE_TRY()

    CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdFeatureService::ExecuteSqlNonQuery");
    if (sqlNonSelectStatement.empty())
        throw new MgInvalidArgumentException(L"MgdFeatureService::ExecuteSqlNonQuery", __LINE__, __WFILE__, NULL, L"", NULL);

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 4);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(sqlNonSelectStatement.c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgParameterCollection");
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgTransaction");
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdFeatureService::ExecuteSqlNonQuery()");

    CHECKARGUMENTNULL(params, L"MgdFeatureService::ExecuteSqlQuery");
    CHECKARGUMENTNULL(trans, L"MgdFeatureService::ExecuteSqlQuery");

    MgdSqlCommand cmd;
    ret = cmd.ExecuteNonQuery(resource, sqlNonSelectStatement, params, trans);

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH(resource, L"MgdFeatureService::ExecuteSqlNonQuery")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_FEATURE_SERVICE_THROW()
    return ret;
}

INT32 MgdFeatureService::ExecuteSqlNonQuery(MgResourceIdentifier* resource,
                                            CREFSTRING sqlNonSelectStatement) 
{ 
    INT32 ret = 0;
    MG_LOG_OPERATION_MESSAGE(L"ExecuteSqlNonQuery");

    MG_FEATURE_SERVICE_TRY()

    CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdFeatureService::ExecuteSqlNonQuery");
    if (sqlNonSelectStatement.empty())
        throw new MgInvalidArgumentException(L"MgdFeatureService::ExecuteSqlNonQuery", __LINE__, __WFILE__, NULL, L"", NULL);

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 2);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(sqlNonSelectStatement.c_str());
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MG_LOG_TRACE_ENTRY(L"MgdFeatureService::ExecuteSqlNonQuery()");

    MgdSqlCommand cmd;
    ret = cmd.ExecuteNonQuery(resource, sqlNonSelectStatement, NULL, NULL);

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH(resource, L"MgdFeatureService::ExecuteSqlNonQuery")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_FEATURE_SERVICE_THROW()

	return ret;
}

MgSpatialContextReader* MgdFeatureService::GetSpatialContexts(MgResourceIdentifier* resId, bool bActiveOnly) 
{
    Ptr<MgSpatialContextReader> mgSpatialContextReader;
    MG_LOG_OPERATION_MESSAGE(L"GetSpatialContexts");

    MG_FEATURE_SERVICE_TRY()

    CHECK_FEATURE_SOURCE_ARGUMENT(resId, L"MgdResourceService::GetSpatialContext");

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 2);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resId) ? L"MgResourceIdentifier" : resId->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_BOOL(bActiveOnly);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MgdLogDetail logDetail(MgServiceType::FeatureService, MgdLogDetail::Trace, L"MgdFeatureService::GetSpatialContexts", mgStackParams);
    logDetail.AddResourceIdentifier(L"Id", resId);
    logDetail.AddBool(L"ActiveOnly", bActiveOnly);
    logDetail.Create();

	CHECK_FEATURE_SOURCE_ARGUMENT(resId, L"MgdFeatureService::GetSpatialContexts");
	
    MgdGetSpatialContexts cmd;
    mgSpatialContextReader = cmd.GetSpatialContexts(resId);

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH(resId, L"MgdFeatureService::GetSpatialContexts")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_FEATURE_SERVICE_THROW()

    return mgSpatialContextReader.Detach();
}

MgLongTransactionReader* MgdFeatureService::GetLongTransactions(MgResourceIdentifier* resource,
                                                                bool bActiveOnly) 
{ 
    Ptr<MgLongTransactionReader> reader;
    MG_LOG_OPERATION_MESSAGE(L"GetLongTransactions");

    MG_FEATURE_SERVICE_TRY()
    
    CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdFeatureService::GetLongTransactions");

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 2);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_BOOL(bActiveOnly);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MgdGetLongTransactions getTrans;
    reader = getTrans.GetLongTransactions(resource, bActiveOnly);

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH(resource, L"MgdFeatureService::GetLongTransactions")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_FEATURE_SERVICE_THROW()

	return reader.Detach();
}

bool MgdFeatureService::SetLongTransaction(MgResourceIdentifier* resource, CREFSTRING longTransactionName) 
{ 
    bool ok = false;
    MG_LOG_OPERATION_MESSAGE(L"SetLongTransaction");

    MG_FEATURE_SERVICE_TRY()

    CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdFeatureService::SetLongTransaction");
    if (longTransactionName.empty())
        throw new MgNullArgumentException(L"MgdFeatureService::SetLongTransaction", __LINE__, __WFILE__, NULL, L"", NULL);

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 2);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(longTransactionName.c_str());
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

	Ptr<MgdFeatureConnection> connWrap = new MgdFeatureConnection(resource);
	FdoPtr<FdoIConnection> conn = connWrap->GetConnection();
	FdoPtr<FdoIActivateLongTransaction> activate = (FdoIActivateLongTransaction*)conn->CreateCommand(FdoCommandType_ActivateLongTransaction);
	activate->SetName(longTransactionName.c_str());
	activate->Execute();
	ok = true;

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH(resource, L"MgdFeatureService::SetLongTransaction")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_FEATURE_SERVICE_THROW()

	return ok;
}

MgStringCollection* MgdFeatureService::GetSchemas(MgResourceIdentifier* resource) 
{
    Ptr<MgStringCollection> schemaNames;
    MG_LOG_OPERATION_MESSAGE(L"GetSchemas");

    MG_FEATURE_SERVICE_TRY()

    CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdFeatureService::GetSchemas");

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 1);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MgdDescribeSchema describe;
    schemaNames = describe.GetSchemas(resource);

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH(resource, L"MgdFeatureService::GetSchemas")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_FEATURE_SERVICE_THROW()

    return schemaNames.Detach();
}

MgStringCollection* MgdFeatureService::GetClasses(MgResourceIdentifier* resource, CREFSTRING schemaName) 
{ 
    Ptr<MgStringCollection> classNames;
    MG_LOG_OPERATION_MESSAGE(L"GetClasses");

    MG_FEATURE_SERVICE_TRY()
    
    CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdFeatureService::GetClasses");

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 2);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(schemaName.c_str());
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MgdDescribeSchema describe;
    classNames = describe.GetClasses(resource, schemaName);

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH(resource, L"MgdFeatureService::GetClasses")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_FEATURE_SERVICE_THROW()

    return classNames.Detach();
}

MgClassDefinition* MgdFeatureService::GetClassDefinition(MgResourceIdentifier* resource,
                                                         CREFSTRING schemaName,
                                                         CREFSTRING className) 
{ 
    Ptr<MgClassDefinition> ret;
    MG_LOG_OPERATION_MESSAGE(L"GetClassDefinition");

    MG_FEATURE_SERVICE_TRY()

    CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdFeatureService::GetClassDefinition");

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 3);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(schemaName.c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(className.c_str());
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MgdDescribeSchema cmd;
    Ptr<MgClassDefinition> clsDef = cmd.GetClassDefinition(resource, schemaName, className, true);

    //In MapGuide Server returning this potentially cached copy is okay as a new instance is created
    //travelling the server/webtier boundary. This is not the case in mg-desktop so we have to clone
    //this result and return the clone instead.
    ret = MgdFeatureUtil::CloneMgClassDefinition(clsDef);

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_FEATURE_SERVICE_CATCH_WITH_FEATURE_SOURCE(L"MgdFeatureService::GetClassDefinition", resource)

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_FEATURE_SERVICE_THROW()

    return ret.Detach();
}

//Though internal (and thus, normally wouldn't be implemented), it's needed by MgdMappingUtil
MgClassDefinition* MgdFeatureService::GetClassDefinition(MgResourceIdentifier* resource,
                                              CREFSTRING schemaName,
                                              CREFSTRING className,
                                              bool serialize) 
{
    CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdFeatureService::GetClassDefinition");
	
	Ptr<MgClassDefinition> classDefinition;
    Ptr<MgClassDefinition> clone;

    MG_FEATURE_SERVICE_TRY()

    MgdDescribeSchema describe;
    classDefinition = describe.GetClassDefinition(resource, schemaName, className, serialize);

    //In MapGuide Server returning this potentially cached copy is okay as a new instance is created
    //travelling the server/webtier boundary. This is not the case in mg-desktop so we have to clone
    //this result and return the clone instead.
    clone = MgdFeatureUtil::CloneMgClassDefinition(classDefinition);

    MG_FEATURE_SERVICE_CATCH_AND_THROW_WITH_FEATURE_SOURCE(L"MgdFeatureService::GetClassDefinition", resource)

    return clone.Detach();
}

void MgdFeatureService::CreateFeatureSource(MgResourceIdentifier* resource, MgFeatureSourceParams* sourceParams) 
{ 
    MG_LOG_OPERATION_MESSAGE(L"CreateFeatureSource");

    MG_FEATURE_SERVICE_TRY()

    CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdFeatureService::CreateFeatureSource");
    CHECKARGUMENTNULL(sourceParams, L"MgdFeatureService::CreateFeatureSource");

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 2);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(L"MgFeatureSourceParams");
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    Ptr<MgdCreateFeatureSource> creator = new MgdCreateFeatureSource();
    creator->CreateFeatureSource(resource, sourceParams);

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH(resource, L"MgdFeatureService::CreateFeatureSource")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_FEATURE_SERVICE_THROW()
}

MgByteReader* MgdFeatureService::DescribeWfsFeatureType(MgResourceIdentifier* featureSourceId,
                                                        MgStringCollection* featureClasses) 
{ 
	NOT_IMPLEMENTED(L"MgdFeatureService::DescribeWfsFeatureType");
}

MgByteReader* MgdFeatureService::DescribeWfsFeatureType(MgResourceIdentifier* featureSourceId,
                                                        MgStringCollection* featureClasses,
                                                        CREFSTRING namespacePrefix,
                                                        CREFSTRING namespaceUrl) 
{ 
	NOT_IMPLEMENTED(L"MgdFeatureService::DescribeWfsFeatureType");
}

MgByteReader* MgdFeatureService::GetWfsFeature(MgResourceIdentifier* featureSourceId,
                                               CREFSTRING featureClass,
                                               MgStringCollection* requiredProperties,
                                               CREFSTRING srs,
                                               CREFSTRING wfsFilter,
                                               INT32 maxFeatures) 
{ 
    Ptr<MgByteReader> ret;
    MG_FEATURE_SERVICE_TRY()
	ret = GetWfsFeature(featureSourceId, featureClass, requiredProperties, srs, wfsFilter, maxFeatures, L"1.0.0", L"text/xml; subtype=gml/2.1.2", L"", L"", L"");
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureService::GetWfsFeature")
    return ret;
}

MgByteReader* MgdFeatureService::GetWfsFeature(MgResourceIdentifier* featureSourceId,
                                               CREFSTRING featureClass,
                                               MgStringCollection* requiredProperties,
                                               CREFSTRING srs,
                                               CREFSTRING filter,
                                               INT32 maxFeatures,
                                               CREFSTRING wfsVersion,
                                               CREFSTRING outputFormat,
                                               CREFSTRING sortCriteria,
                                               CREFSTRING namespacePrefix,
                                               CREFSTRING namespaceUrl) 
{ 
	NOT_IMPLEMENTED(L"MgdFeatureService::GetWfsFeature");
}

MgByteReader* MgdFeatureService::EnumerateDataStores(CREFSTRING providerName, CREFSTRING partialConnString) 
{ 
    Ptr<MgByteReader> reader;
    MG_LOG_OPERATION_MESSAGE(L"EnumerateDataStores");

    MG_FEATURE_SERVICE_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 2);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(providerName.c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(partialConnString.c_str());
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MgdEnumerateDataStores cmd;


    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_FEATURE_SERVICE_CATCH(L"MgdFeatureService::EnumerateDataStores")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_FEATURE_SERVICE_THROW()

    return reader.Detach();
}

MgByteReader* MgdFeatureService::GetSchemaMapping(CREFSTRING providerName, CREFSTRING partialConnString) 
{	
    Ptr<MgByteReader> byteReader;

	MG_LOG_OPERATION_MESSAGE(L"GetSchemaMapping");

    MG_FEATURE_SERVICE_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 2);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(providerName.c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(partialConnString.c_str());
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MgdGetSchemaMapping descMapping;
    byteReader = descMapping.GetSchemaMapping(providerName, partialConnString);

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_FEATURE_SERVICE_CATCH(L"MgdFeatureService::GetSchemaMapping")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_FEATURE_SERVICE_THROW()

    return byteReader.Detach();
}

void MgdFeatureService::RegisterProvider(CREFSTRING providerLibraryPath)
{
    MG_LOG_OPERATION_MESSAGE(L"RegisterProvider");

    MG_FEATURE_SERVICE_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 1);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(providerLibraryPath.c_str());
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    FdoPtr<IProviderRegistry> provReg = FdoFeatureAccessManager::GetProviderRegistry();
    FdoPtr<IConnectionManager> connMgr = FdoFeatureAccessManager::GetConnectionManager();

    //As of FDO 3.6 this supports FDO dll paths
    FdoPtr<FdoIConnection> conn = connMgr->CreateConnection(providerLibraryPath.c_str());
    FdoPtr<FdoIConnectionInfo> connInfo = conn->GetConnectionInfo();

    //FDO connections already have this information!
    provReg->RegisterProvider(connInfo->GetProviderName(),
                              connInfo->GetProviderDisplayName(),
                              connInfo->GetProviderDescription(),
                              connInfo->GetProviderVersion(),
                              connInfo->GetFeatureDataObjectsVersion(),
                              providerLibraryPath.c_str(),
                              false);

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_FEATURE_SERVICE_CATCH(L"MgdFeatureService::RegisterProvider")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_FEATURE_SERVICE_THROW()
}

void MgdFeatureService::UnregisterProvider(CREFSTRING providerName)
{
    MG_LOG_OPERATION_MESSAGE(L"UnregisterProvider");

    MG_FEATURE_SERVICE_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 1);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(providerName.c_str());
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    FdoPtr<IProviderRegistry> reg = FdoFeatureAccessManager::GetProviderRegistry();
    reg->UnregisterProvider(providerName.c_str());

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_FEATURE_SERVICE_CATCH(L"MgdFeatureService::UnregisterProvider")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_FEATURE_SERVICE_THROW()
}

//INTERNAL_API:
//These API's don't need to be implemented as we're not a server process sending data down the wire

//Unfortunately, this is needed by MgdMap, so we have to implement it
MgPropertyDefinitionCollection* MgdFeatureService::GetIdentityProperties(MgResourceIdentifier* resource,
                                                                         CREFSTRING schemaName,
                                                                         CREFSTRING className)

{
    Ptr<MgPropertyDefinitionCollection> propDefs;
    MG_LOG_OPERATION_MESSAGE(L"GetIdentityProperties");

    MG_FEATURE_SERVICE_TRY()

    CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdFeatureService::GetIdentityProperties");

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 3);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(schemaName.c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(className.c_str());
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MgdDescribeSchema describe;
    Ptr<MgStringCollection> classNames = new MgStringCollection();
    classNames->Add(className);
    Ptr<MgClassDefinitionCollection> classDefs = describe.GetIdentityProperties(resource, schemaName, classNames);
    if (classDefs->GetCount() != 1)
    {
        throw new MgClassNotFoundException(L"MgdFeatureService::GetIdentityProperties", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    Ptr<MgClassDefinition> classDef = classDefs->GetItem(0);
    propDefs = classDef->GetIdentityProperties();

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_FEATURE_SERVICE_CATCH_WITH_FEATURE_SOURCE(L"MgdFeatureService::GetIdentityProperties", resource)

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_FEATURE_SERVICE_THROW()

    return propDefs.Detach();
}

//Unfortunately, this is needed by MgdMap, so we have to implement it
//TODO: Check if this requires immutability
MgClassDefinitionCollection* MgdFeatureService::GetIdentityProperties(MgResourceIdentifier* resource,
                                                                      CREFSTRING schemaName,
                                                                      MgStringCollection* classNames)
{
    Ptr<MgClassDefinitionCollection> classDefs;
    MG_LOG_OPERATION_MESSAGE(L"GetIdentityProperties");

    MG_FEATURE_SERVICE_TRY()

    CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdFeatureService::GetIdentityProperties");

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 3);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == resource) ? L"MgResourceIdentifier" : resource->ToString().c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(schemaName.c_str());
    MG_LOG_OPERATION_MESSAGE_ADD_SEPARATOR();
    MG_LOG_OPERATION_MESSAGE_ADD_STRING((NULL == classNames) ? L"MgStringCollection" : classNames->GetLogString().c_str());
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    MgdDescribeSchema describe;
    classDefs = describe.GetIdentityProperties(resource, schemaName, classNames);

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_FEATURE_SERVICE_CATCH_WITH_FEATURE_SOURCE(L"MgdFeatureService::GetIdentityProperties", resource)

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_FEATURE_SERVICE_THROW()

    return classDefs.Detach();
}

MgByteReader* MgdFeatureService::QueryCacheInfo()
{
    Ptr<MgByteReader> ret;

    MG_LOG_OPERATION_MESSAGE(L"QueryCacheInfo");

    MG_FEATURE_SERVICE_TRY()

    MG_LOG_OPERATION_MESSAGE_INIT(MG_API_VERSION(1, 0, 0), 0);
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_START();
    MG_LOG_OPERATION_MESSAGE_PARAMETERS_END();

    STRING content = L"<FdoConnectionPoolInfo>";

    std::vector<PoolCacheEntry*> entries;
    MgdFdoConnectionPool::GetCacheInfo(entries);

    for (std::vector<PoolCacheEntry*>::iterator it = entries.begin(); it != entries.end(); it++)
    {
        content += L"<PoolCacheEntry>";
        {
            content += L"<ResourceId>";
            content += (*it)->ResourceId;
            content += L"</ResourceId>";

            content += L"<Provider>";
            content += (*it)->ProviderName;
            content += L"</Provider>";

            content += L"<OpenConnections>";
            STRING sOpen;
            MgUtil::Int32ToString((*it)->OpenCount, sOpen);
            content += sOpen;
            content += L"</OpenConnections>";

            content += L"<ClosedConnections>";
            STRING sClosed;
            MgUtil::Int32ToString((*it)->ClosedCount, sClosed);
            content += sClosed;
            content += L"</ClosedConnections>";
        }
        content += L"</PoolCacheEntry>";
    }

    content += L"</FdoConnectionPoolInfo>";

    for (std::vector<PoolCacheEntry*>::iterator it = entries.begin(); it != entries.end(); it++)
    {
        delete (*it);
    }

    entries.clear();

    string utf8Text = MgUtil::WideCharToMultiByte(content);
    Ptr<MgByteSource> byteSource = new MgByteSource((BYTE_ARRAY_IN) utf8Text.c_str(), (INT32)utf8Text.length());
    byteSource->SetMimeType(MgMimeType::Xml);
	ret = byteSource->GetReader();

    // Successful operation
    MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Success.c_str());

    MG_FEATURE_SERVICE_CATCH(L"MgdFeatureService::QueryCacheInfo")

    if (mgException != NULL)
    {
        // Failed operation
        MG_LOG_OPERATION_MESSAGE_ADD_STRING(MgResources::Failure.c_str());
        MG_DESKTOP_LOG_EXCEPTION();
    }

    // Add access log entry for operation
    MG_LOG_OPERATION_MESSAGE_ACCESS_ENTRY();

    MG_FEATURE_SERVICE_THROW()

    return ret.Detach();
}

//NOTE: This is needed by MgRaster::GetStream(), but this is implemented in MgdRasterHelper which we redirect the call to
//So it is not implemented here
MgByteReader* MgdFeatureService::GetRaster(CREFSTRING reader, INT32 xSize, INT32 ySize, STRING propName) 
{ 
    NOT_IMPLEMENTED(L"MgdFeatureService::GetRaster"); 
}

// Feature
MgBatchPropertyCollection* MgdFeatureService::GetFeatures(CREFSTRING featureReader) { NOT_IMPLEMENTED(L"MgdFeatureService::GetFeatures"); }
bool MgdFeatureService::CloseFeatureReader(CREFSTRING featureReader) { NOT_IMPLEMENTED(L"MgdFeatureService::CloseFeatureReader"); }

// SQL
MgBatchPropertyCollection* MgdFeatureService::GetSqlRows(CREFSTRING sqlReader) { NOT_IMPLEMENTED(L"MgdFeatureService::GetSqlRows"); }
bool MgdFeatureService::CloseSqlReader(CREFSTRING sqlReader) { NOT_IMPLEMENTED(L"MgdFeatureService::CloseSqlReader"); }

// Data
MgBatchPropertyCollection* MgdFeatureService::GetDataRows(CREFSTRING dataReader) { NOT_IMPLEMENTED(L"MgdFeatureService::GetDataRows"); }
bool MgdFeatureService::CloseDataReader(CREFSTRING dataReader) { NOT_IMPLEMENTED(L"MgdFeatureService::CloseDataReader"); }

STRING MgdFeatureService::GetFdoCacheInfo() { NOT_IMPLEMENTED(L"MgdFeatureService::GetFdoCacheInfo"); }

// Commit the transaction specified by the transaction id.
bool MgdFeatureService::CommitTransaction(CREFSTRING transactionId) { NOT_IMPLEMENTED(L"MgdFeatureService::CommitTransaction"); }

// Rollback the transaction specified by the transaction id.
bool MgdFeatureService::RollbackTransaction(CREFSTRING transactionId) { NOT_IMPLEMENTED(L"MgdFeatureService::RollbackTransaction"); }

//Add save point
STRING MgdFeatureService::AddSavePoint(CREFSTRING transactionId, CREFSTRING suggestName) { NOT_IMPLEMENTED(L"MgdFeatureService::AddSavePoint"); }

//Roll back to a save point
bool MgdFeatureService::RollbackSavePoint(CREFSTRING transactionId, CREFSTRING savePointName) { NOT_IMPLEMENTED(L"MgdFeatureService::RollbackSavePoint"); }

//Release a save point
bool MgdFeatureService::ReleaseSavePoint(CREFSTRING transactionId, CREFSTRING savePointName) { NOT_IMPLEMENTED(L"MgdFeatureService::ReleaseSavePoint"); }
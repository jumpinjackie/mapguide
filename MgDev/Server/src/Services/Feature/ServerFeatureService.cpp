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

#include "ServerFeatureServiceDefs.h"
#include "BaseService.h"
#include "ResourceService.h"
#include "FeatureService.h"
#include "ServerFeatureService.h"
#include "ServerGetFeatureProviders.h"
#include "ServerGetProviderCapabilities.h"
#include "ServerFeatureConnection.h"
#include "ServerDescribeSchema.h"
#include "ServerSelectFeatures.h"
#include "ServerGetFeatures.h"
#include "ServerFeatureReaderIdentifierPool.h"
#include "ServiceManager.h"
#include "ServerGetConnectionPropertyValues.h"
#include "ServerGetSpatialContexts.h"
#include "ServerGetLongTransactions.h"
#include "ServerSqlProcessor.h"
#include "ServerSqlDataReaderPool.h"
#include "ServerSqlCommand.h"
#include "FeatureDistribution.h"
#include "ServerDataProcessor.h"
#include "ServerDataReaderPool.h"
#include "ServerFeatureUtil.h"
#include "ServerUpdateFeatures.h"
#include "ServerCreateFeatureSource.h"
#include "ServerFeatureReader.h"
#include "ServerEnumerateDataStores.h"
#include "ServerGetSchemaMapping.h"
#include "FilterUtil.h"

#include <fdo/Xml/FeatureSerializer.h>
#include <fdo/Xml/FeatureWriter.h>
#include <fdo/Xml/FeaturePropertyWriter.h>
#include <fdo/Xml/FeatureFlags.h>


//////////////////////////////////////////////////////////////////
/// <summary>
/// Default constructor - needed to prevent a compile error under GCC 3
/// </summary>
MgServerFeatureService::MgServerFeatureService() : MgFeatureService(NULL)
{
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Construct an MgServerFeatureService object
/// </summary>
MgServerFeatureService::MgServerFeatureService(MgConnectionProperties* connection) : MgFeatureService(connection)
{
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Destructor
/// </summary>
MgServerFeatureService::~MgServerFeatureService(void)
{
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// This method returns list of all providers, their connection properties
/// and (default or enumerated) values for these properties, if any, in XML
/// format.
///
/// Schema Definition: FeatureProviders.xsd
/// Sample XML:        FeatureProviders.xml
/// </summary>
/// <returns>Byte array representing XML (or error status)
/// </returns>
MgByteReader* MgServerFeatureService::GetFeatureProviders()
{
    MG_LOG_TRACE_ENTRY(L"MgServerFeatureService::GetFeatureProviders()");

    MgServerGetFeatureProviders msgfp;
    return msgfp.GetFeatureProviders();
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// This method returns list of all values for the connection property
/// specified based on partial connection string. To understand this, let
/// us take an example:
///
/// Oracle Provider for Fdo has 3 services running on each Machine A, Machine B
/// and Machine C.
///
/// <table>
/// Machine     IP Address    ServiceName
/// ---------   -----------   ----------------------
/// Machine A   144.111.1.1   Canada, Europe, Russia
/// Machine B   192.111.1.2   Japan, China, India
/// Machine C   159.111.2.1   USA, Mexico, Australia
/// </table>
///
/// List of properties required for connection:
/// <table>
/// PropertyName    Enumerable      Values
/// ------------    ----------      -------------------------------------------------
/// UserName        No
/// Password        No
/// IPAddress       Yes             144.111.1.1
///                                 192.111.1.2
///                                 159.111.2.1
/// ServiceName     Yes             (This value is dependent on IPAddress
///                                  and therefore value is unknown)
///
/// DatabaseName    Yes             (This value is dependent on IPAddress+ServiceName
///                                  and therefore value is unknown)
/// </table>
///
/// User builds partial connection string as
/// UserName=test{ } Password=test{ } IPAddress=144.111.1.1
///
/// Using this partial connection string user requests values for ServiceName property.
///
/// Internally Fdo provider, if it has some dependency on a certain property, it needs
/// to look into property dictionary and find out if a dependent property values are already
/// set. In this case it is an IPAddress only but there can be more than one property.
///
/// If all dependent values are set, it will return the list of values for the dependent
/// property values. The result for the above connectionString would be
///
/// -        Canada,
/// -        Europe,
/// -        Russia
///
/// In case of SQLServer, we might have more than one database per service. Therefore
/// once the list of services are retrieved, user builds another connectionString as
///
/// UserName=test{ } Password=test{ } IPAddress=144.111.1.1{ } ServiceName=Canada
///
/// and requests property values for "DatabaseName". This would result in list of
/// databases available to the users e.g.
///
/// -       Alberta
/// -       British Columbia
/// -       Manitoba
///
/// The final connectionString for Open would be
///
/// UserName=test{ } Password=test{ } IPAddress=144.111.1.1{ } ServiceName=Canada{ } DatabaseName=Alberta
/// </summary>
///
/// <param name="propertyName">Input
/// Name of property for which values need to be retrieved. (Can not be NULL)
/// </param>
///
/// <param name="partialConnString">Input
/// Partial connection string on which property request is dependent.
/// NULL value indicates no dependency.
/// </param>
///
/// <returns>
/// String Collection or NULL if nothing is found
/// </returns>
MgStringCollection* MgServerFeatureService::GetConnectionPropertyValues( CREFSTRING providerName,
                                                                         CREFSTRING propertyName,
                                                                         CREFSTRING partialConnString )
{
    MG_LOG_TRACE_ENTRY(L"MgServerFeatureService::GetConnectionPropertyValues()");

    MgServerGetConnectionPropertyValues msgpv;
    return msgpv.GetConnectionPropertyValues(providerName, propertyName, partialConnString);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Attempts to connect to the Feature Provider given by the connection string
/// </summary>
/// <param name="connectionString">
/// Connection string for Feature Provider
/// </param>
/// <returns>
/// True if connection was successful
/// </returns>
bool MgServerFeatureService::TestConnection(CREFSTRING providerName, CREFSTRING connectionString)
{
    MG_LOG_TRACE_ENTRY(L"MgServerFeatureService::TestConnection()");

    MgServerFeatureConnection msfc(providerName, connectionString);

    // TODO: Verify that when FdoIConnection is released it closes the connection in destructor
    return msfc.IsConnectionOpen();
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Attempts to connect to the Feature Provider given by the connection string
/// </summary>
/// <param name="connectionString">
/// Connection string for Feature Provider
/// </param>
/// <returns>
/// True if connection was successful
/// </returns>
bool MgServerFeatureService::TestConnection( MgResourceIdentifier* resource )
{
    MG_LOG_TRACE_ENTRY(L"MgServerFeatureService::TestConnection()");

    MgServerFeatureConnection msfc(resource);

    // TODO: Verify that when FdoIConnection is released it closes the connection in destructor
    return msfc.IsConnectionOpen();
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// This method returns capabilities of the provider applicable for the
/// connection. This would provide details on the following
/// capabilities:
/// 1. Connection
/// 2. Schema
/// 3. Command
/// 4. Filter
/// 5. Expression
///
/// Schema Definition: FeatureProviderCapabilities.xsd
/// Sample XML:        FeatureProviderCapabilities.xml
/// </summary>
/// <param name="resource">Input
/// A resource identifier referring to connection string
/// </param>
/// <returns>
/// Byte array representing XML (or NULL)
/// </returns>
///
/// EXCEPTIONS:
/// MgInvalidResourceIdentifer
/// NOTE:
/// Subject to change with FDO R2
MgByteReader* MgServerFeatureService::GetCapabilities( CREFSTRING providerName )
{
    MG_LOG_TRACE_ENTRY(L"MgServerFeatureService::GetCapabilities()");

    MgServerGetProviderCapabilities msgpc(providerName);
    return msgpc.GetProviderCapabilities();
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// This method returns a collection of ALL ( IF NO NAME IS SUPPLIED ) schemas
/// </summary>
/// <param name="resource">Input
/// A resource identifier referring to connection string
/// </param>
/// <param name="schemaName">Input
/// A schema name or NULL to retrieve all available schemas
/// </param>
/// <returns>
/// MgFeatureSchemaCollection
/// </returns>
///
/// EXCEPTIONS:
/// MgInvalidResourceIdentifer
MgFeatureSchemaCollection* MgServerFeatureService::DescribeSchema( MgResourceIdentifier* resource,
                                                      CREFSTRING schemaName )
{
    MG_LOG_TRACE_ENTRY(L"MgServerFeatureService::DescribeSchema()");

    MgServerDescribeSchema msds;
    return msds.DescribeSchema(resource, schemaName);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// This method returns list of ALL ( IF NO NAME IS SUPPLIED ) schemas
/// and details on each class available in the schema with Data,Geometry and
/// Object property definitions.
///
/// Schema Definition: FdoSchemaDesc.xsd
/// Sample XML:        FdoSchemaDesc.xml
///
/// </summary>
/// <param name="resource">Input
/// A resource identifier referring to connection string
/// </param>
/// <param name="schemaName">Input
/// A schema name or NULL to retrieve all available schemas
/// </param>
/// <returns>
/// String representing XML (or NULL)
/// </returns>
///
/// EXCEPTIONS:
/// MgInvalidResourceIdentifer
STRING MgServerFeatureService::DescribeSchemaAsXml( MgResourceIdentifier* resource,
                                                      CREFSTRING schemaName )
{
    MG_LOG_TRACE_ENTRY(L"MgServerFeatureService::DescribeSchemaAsXml()");

    MgServerDescribeSchema msds;
    return msds.DescribeSchemaAsXml(resource, schemaName);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Select features returns feature information from an Fdo Provider.  An MgFeatureReader
/// object provides one way, read-only iteration over the features.  The various properties
/// of a feature class are accessed by name using methods of the MgFeatureReader.
///
/// The desired feature class is specified by name. A subset of the feature properties
/// can be specified using the properties string collection.  Futhermore, an Fdo filter text
/// can be applied to limit the result set.
/// </summary>
/// <param name="resource">Input
/// A resource identifier referring to feature source
/// </param>
/// <param name="className">Input
/// A valid class name on which select operation needs to be executed. It should not be NULL.
/// </param>
/// <param name="options">Input
/// MgFeatureQueryOptions instance containing all required filters for this select operation
/// </param>
/// <returns>
/// MgFeatureReader which operates on the instance of actual reader returned from the
/// FdoProvider.
/// </returns>
/// EXCEPTIONS:
/// MgFeatureSourceException
/// MgInvalidArgumentException
/// MgFdoException
/// NOTE:
/// 1. Please refer to Overview section of this document or Fdo provider documents
///    for details on connection properties, schema, classes, class proeprties and
///    filter text.
/// 2. Interrogation of class definition would allow to determine properties of classes
///    which can be used for filter text.
MgFeatureReader* MgServerFeatureService::SelectFeatures( MgResourceIdentifier* resource,
                                                         CREFSTRING className,
                                                         MgFeatureQueryOptions* options )
{
    MG_LOG_TRACE_ENTRY(L"MgServerFeatureService::SelectFeatures()");

    MgServerSelectFeatures mssf;
    return (MgFeatureReader*)mssf.SelectFeatures(resource, className, options, false);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// This operation allows user to select features and then combine them into a group (means aggregate them).
/// Once the features are grouped together into a single set of data, they do not represent features, in other
/// words, they do not corresponds to underlying database schema. Therefore, this operation returns MgDataReader.
/// Computed properties applied in SelectFeatures would only be applicable per feature. For example, a polygon feature
/// can compute its area while retriveal. The area would be computed property for a specific feature instance.
/// In SelectAggregate operation, user might want to group all area for a particular zone and sum them to
/// find total area based on certain criteria. The total area of a particular zone does not represent a feature instance (row).
///
/// To find about aggregate functionalities available, please refer to GetCapabilities document returned by GetCapabilities
/// operation.
/// </summary>
/// <param name="resource">Input
/// A resource identifier referring to feature source
/// </param>
/// <param name="className">Input
/// A valid class name on which select operation needs to be executed. It should not be NULL.
/// </param>
/// <param name="options">Input
/// MgFeatureAggregateOptions instance containing all required filters for this select operation
/// </param>
/// <returns>
/// MgDataReader pointer which operates on the instance of actual reader returned from the
/// FdoProvider ( OR NULL ).
/// </returns>
/// EXCEPTIONS:
/// MgFeatureSourceException
/// MgInvalidArgumentException
/// MgFdoException
///
/// NOTE:
/// 1. Please refer to Overview section of this document or Fdo provider documents
///    for details on connection properties, schema, classes, class proeprties,
///    filter text and spatial operations.
/// 2. Please refer to Geometry API documentation for Geometry details.
/// 3. Interrogation of provider capabilities will inform list of operations supported
/// 4. Interrogation of class definition would allow to determine properties of classes
///    which can be used for filter text.
MgDataReader* MgServerFeatureService::SelectAggregate( MgResourceIdentifier* resource,
                                                       CREFSTRING className,
                                                       MgFeatureAggregateOptions* options )
{
    MG_LOG_TRACE_ENTRY(L"MgServerFeatureService::SelectAggregate()");

    MgServerSelectFeatures mssf;
    return (MgDataReader*)mssf.SelectFeatures(resource, className, options, true);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// It executes all the commands specified in command collection.
/// </summary>
/// <param name="resource">Input
/// A resource identifier referring to connection string
/// </param>
/// <param name="commands">Input
/// Collection of commands to be executed
/// </param>
/// <param name="useTransaction">Input
/// true  -  all commands will be executed in transaction
/// false -  not to use transaction
/// </param>
/// <returns>
/// Integer collection referring to result for each command
/// Index of commandCollection and index of IntCollection would match the result.
/// </returns>
///
/// EXCEPTIONS:
/// MgInvalidResourceIdentifer
MgPropertyCollection* MgServerFeatureService::UpdateFeatures( MgResourceIdentifier* resource,
                                                         MgFeatureCommandCollection* commands,
                                                         bool useTransaction )
{
    MG_LOG_TRACE_ENTRY(L"MgServerFeatureService::UpdateFeatures()");

    // Remove any cached references to this resource because it has been updated (Insert/Update/Delete).
    RemoveFeatureServiceCacheEntry(resource);

    MgServerUpdateFeatures asuf;
    return asuf.Execute(resource, commands, useTransaction);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// This method executes the SELECT SQL statement specified and returns a pointer to
/// SqlDataReader instance. This instance can be used to retrieve column information
/// and related values.
///
/// NOTE:
/// Serialize() method of SqlDataReader would be able to convert data returned
/// to AWKFF or XML stream.
/// </summary>
/// <param name="resource">Input
/// A resource identifier referring to connection string
/// </param>
/// <param name="sqlStatement">Input
/// This would allow users to specify free format SQL SELECT statement like
/// SELECT * FROM CLASSNAME WHERE COLOR = RED. This would return all rows
/// from "CLASSNAME" where COLOR column has value RED.
/// </param>
/// <returns>
/// SqlDataReader pointer, an instance of reader pointing to the actual reader
/// from FdoProvider (or NULL).
///
/// If any statement other than SELECT is passed to this method, it would return failure.
/// </returns>
///
/// EXCEPTIONS:
/// MgInvalidResourceIdentifer
/// MgInvalidSqlStatement
/// MgSqlNotSupported
MgSqlDataReader* MgServerFeatureService::ExecuteSqlQuery( MgResourceIdentifier* resource,
                                                         CREFSTRING sqlStatement )
{
    MG_LOG_TRACE_ENTRY(L"MgServerFeatureService::ExecuteSqlQuery()");

    MgServerSqlCommand sqlCommand;
    return sqlCommand.ExecuteQuery(resource, sqlStatement);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// This method executes all SQL statements supported by providers except SELECT.
/// </summary>
/// <param name="resource">Input
/// A resource identifier referring to connection string
/// </param>
/// <param name="sqlNonSelectStatement">Input
/// This would allow users to specify free format SQL statement like INSERT/UPDATE/DELETE/CREATE
/// </param>
/// <returns>
/// An positive integer value indicating how many instances (rows) have been affected.
///
/// NOTE: It is possible that only few rows were affected than actually expected.
/// In this scenario, warning object will contain the details on the failed records.
/// </returns>
///
/// EXCEPTIONS:
/// MgInvalidResourceIdentifer
INT32 MgServerFeatureService::ExecuteSqlNonQuery( MgResourceIdentifier* resource,
                                                 CREFSTRING sqlNonSelectStatement )
{
    MG_LOG_TRACE_ENTRY(L"MgServerFeatureService::ExecuteSqlNonQuery()");

    MgServerSqlCommand sqlCommand;
    return sqlCommand.ExecuteNonQuery(resource, sqlNonSelectStatement);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// This method executes the SELECT statement to return all available spatial context
/// for the provider. The information for spatial contexts will be retrieved via
/// SpatialContextReader instance returned.
/// NOTE:
/// Serialize() method of SpatialContextReader would be able to convert data returned from
/// SpatialContextReader to binary stream for transmission or XML.
/// </summary>
/// <param name="resource">Input
/// A resource identifier referring to connection string
/// </param>
/// <param name="bActiveOnly">Input
/// This flag would decide whether it should return all or active spatial context.
/// True - Only active spatial context
/// False - All spatial contexts
/// </param>
/// <returns>
/// SpatialContextReader pointer, a pointer to the actual instance reader returned from the
/// FdoProvider (or NULL).
///
/// This reader can be utilized to retrieve the following information
/// Name -             Spatial Context name
/// Description -      Brief information about spatial context
/// CoordinateSystem -
/// ExtentType -       Static/Dynamic
/// Extent -           AWKB/AWKT byte/text array representing the boundary
///                    of the extent.
/// XYTolerance -      Double
/// ZTolerance -       Double
/// IsActive           Boolean
/// </returns>
///
/// EXCEPTIONS:
/// MgInvalidResourceIdentifer
/// NOTE:
/// Subject to change in FDO R2
MgSpatialContextReader* MgServerFeatureService::GetSpatialContexts( MgResourceIdentifier* resource,
                                                                   bool bActiveOnly)
{
    MG_LOG_TRACE_ENTRY(L"MgServerFeatureService::GetSpatialContexts()");

    MgServerGetSpatialContexts msgsc;
    return msgsc.GetSpatialContexts(resource, bActiveOnly);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// This method executes the SELECT statement to return all available Long transactions
/// for the provider. The information for long transactions will be retrieved via
/// MgLongTransactionReader instance returned.
/// NOTE:
/// Serialize() method of SpatialContextReader would be able to convert data returned from
/// SpatialContextReader to XML.
/// </summary>
/// <param name="resource">Input
/// A resource identifier referring to connection string
/// </param>
/// <param name="bActiveOnly">Input
/// This flag would decide whether it should return all or active long transaction.
/// True - Only active spatial context
/// False - All spatial contexts
/// </param>
/// <returns>
/// MgLongTransactionReader pointer, a pointer to the actual instance reader returned from the
/// FdoProvider (or NULL).
///
/// This reader can be utilized to retrieve the following information
/// Name -             Spatial Context name
/// Description -      Brief information about spatial context
/// Owner       -      Owner of this long transaction
/// IsActive           Boolean
/// IsFrozen    -
/// </returns>
///
/// EXCEPTIONS:
/// MgInvalidResourceIdentifer
/// NOTE:
/// Subject to change in FDO R2
MgLongTransactionReader* MgServerFeatureService::GetLongTransactions(MgResourceIdentifier* resource, bool bActiveOnly)
{
    MG_LOG_TRACE_ENTRY(L"MgServerFeatureService::GetLongTransactions()");

    MgServerGetLongTransactions msglt;
    return msglt.GetLongTransactions(resource, bActiveOnly);
}

//////////////////////////////////////////////////////////////////
/// \brief
/// Retrieves schema information about a set of feature classes for a given feature source.
///
/// <!-- Syntax in .Net, Java, and PHP -->
/// \htmlinclude DotNetSyntaxTop.html
/// virtual MgByteReader CreateFeatureSource(MgResourceIdentifier featureSourceId, MgStringCollection featureClasses);
/// \htmlinclude SyntaxBottom.html
/// \htmlinclude JavaSyntaxTop.html
/// virtual MgByteReader CreateFeatureSource(MgResourceIdentifier featureSourceId, MgStringCollection featureClasses);
/// \htmlinclude SyntaxBottom.html
/// \htmlinclude PHPSyntaxTop.html
/// virtual MgByteReader CreateFeatureSource(MgResourceIdentifier featureSourceId, MgStringCollection featureClasses);
/// \htmlinclude SyntaxBottom.html
///
/// \param featureSourceId (MgResourceIdentifier)
/// The resource identifier defining the
/// location of the feature source in
/// the repository.
/// \param featureClasses (MgStringCollection)
/// A collection of strings identifying the feature classes for which to
/// retrieve schema information. If this collection is null or empty, information
/// is returned for all feature classes.
///
/// \return
/// Returns an MgByteReader containing the XML schema.
///
MgByteReader* MgServerFeatureService::DescribeWfsFeatureType(MgResourceIdentifier* featureSourceId, MgStringCollection* featureClasses)
{
    Ptr<MgFeatureSchemaCollection> fsc = DescribeSchema(featureSourceId, L"");

    //TODO: need code to pick out only the classes that caller asked for from the
    //schemas
    /*
    //TODO: for now, pick the first schema from the collection
    Ptr<MgFeatureSchema> schema = fsc->GetItem(0);

    //create a destination schema collection with just the
    //required schemas
    Ptr<MgFeatureSchemaCollection> dfsc = new MgFeatureSchemaCollection();
    dfsc->Add(schema);


    Ptr<MgClassDefinitionCollection> classes = schema->GetClasses();

    if (featureClasses)
    {
        //TODO clean out classes we don't need
    }
    */

    STRING sch = SchemaToXml(fsc);

    string utfsch = MgUtil::WideCharToMultiByte(sch);

    Ptr<MgByteSource> bs = new MgByteSource((unsigned char*)utfsch.c_str(), (INT32)utfsch.length());
    bs->SetMimeType(MgMimeType::Xml);

    return bs->GetReader();
}


//////////////////////////////////////////////////////////////////
// Helper private method
void MgServerFeatureService::FeatureSourceToString(MgResourceIdentifier* resource, string& resourceContent)
{
    CHECKNULL(resource, L"MgServerFeatureService.FeatureSourceToString");

    MgServiceManager* serviceMan = MgServiceManager::GetInstance();
    assert(NULL != serviceMan);

    // Get the service from service manager
    Ptr<MgResourceService> resourceService = dynamic_cast<MgResourceService*>(
        serviceMan->RequestService(MgServiceType::ResourceService));
    assert(resourceService != NULL);

    // Get the feature source contents
    Ptr<MgByteReader> byteReader = resourceService->GetResourceContent(resource, MgResourcePreProcessingType::Substitution);

    Ptr<MgByteSink> byteSink = new MgByteSink((MgByteReader*)byteReader);
    byteSink->ToStringUtf8(resourceContent);
}


//////////////////////////////////////////////////////////////////
MgBatchPropertyCollection* MgServerFeatureService::GetFeatures(INT32 featureReaderId)
{
    MgServerGetFeatures* featId = (MgServerGetFeatures*)featureReaderId;

    MgServerFeatureReaderIdentifierPool* featPool = MgServerFeatureReaderIdentifierPool::GetInstance();
    CHECKNULL(featPool, L"MgServerFeatureService.GetFeatures");

    if (!featPool->Contains(featId))
    {
        STRING buffer;
        MgUtil::Int32ToString(featureReaderId, buffer);

        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(buffer);

        throw new MgInvalidArgumentException(L"MgServerFeatureService.GetFeatures",
            __LINE__, __WFILE__, &arguments, L"MgFeatureReaderIdNotFound", NULL);
    }

    // Find out the counts from Configuration
    INT32 count = 1;
    MgConfiguration* config = MgConfiguration::GetInstance();

    config->GetIntValue(MgConfigProperties::FeatureServicePropertiesSection,
                        MgConfigProperties::FeatureServicePropertyDataCacheSize,
                        count,
                        MgConfigProperties::DefaultFeatureServicePropertyDataCacheSize);

    Ptr<MgFeatureSet> featSet = featId->GetFeatures(count);
    CHECKNULL((MgFeatureSet*)featSet, L"MgServerFeatureService.GetFeatures");

    Ptr<MgBatchPropertyCollection> bpCol = featSet->GetFeatures();
    CHECKNULL((MgBatchPropertyCollection*)bpCol, L"MgServerFeatureService.GetFeatures");

    return (bpCol->GetCount() > 0)? bpCol.Detach() : NULL;
}


//////////////////////////////////////////////////////////////////
bool MgServerFeatureService::CloseFeatureReader(INT32 featureReaderId)
{
    bool retVal = false;
    MgServerGetFeatures* featId = (MgServerGetFeatures*)featureReaderId;

    MgServerFeatureReaderIdentifierPool* featPool = MgServerFeatureReaderIdentifierPool::GetInstance();
    CHECKNULL(featPool, L"MgServerFeatureService.CloseFeatureReader");

    if (featPool->Contains(featId))
    {
        featPool->Remove(featId);
        retVal = true;
    }

    return retVal;
}


//////////////////////////////////////////////////////////////////
MgStringCollection* MgServerFeatureService::GetSchemas( MgResourceIdentifier* resource )
{
    MgServerDescribeSchema msds;
    return msds.GetSchemas(resource);
}


//////////////////////////////////////////////////////////////////
MgStringCollection* MgServerFeatureService::GetClasses( MgResourceIdentifier* resource, CREFSTRING schemaName )
{
    MgServerDescribeSchema msds;
    return msds.GetClasses(resource, schemaName);
}


//////////////////////////////////////////////////////////////////
MgClassDefinition* MgServerFeatureService::GetClassDefinition(  MgResourceIdentifier* resource,
                                                                CREFSTRING schemaName,
                                                                CREFSTRING className)
{
    MgServerDescribeSchema msds;
    return msds.GetClassDefinition(resource, schemaName, className);
}


//////////////////////////////////////////////////////////////////
STRING MgServerFeatureService::SchemaToXml(MgFeatureSchemaCollection* schema)
{
    MgServerDescribeSchema msds;
    return msds.SchemaToXml(schema);
}


//////////////////////////////////////////////////////////////////
MgFeatureSchemaCollection* MgServerFeatureService::XmlToSchema(CREFSTRING xml)
{
    MgServerDescribeSchema msds;
    return msds.XmlToSchema(xml);
}


//////////////////////////////////////////////////////////////////
MgByteReader* MgServerFeatureService::GetRaster(INT32 readerId, INT32 xSize, INT32 ySize, STRING propName)
{
    MgServerFeatureProcessor* featureProcessor = (MgServerFeatureProcessor*)readerId;

    Ptr<MgByteReader> byteReader;

    if (featureProcessor != NULL)
    {
        bool isValid = MgServerFeatureUtil::ValidatePoolEntry(readerId);
        if (!isValid)
        {
            // No entry in pool is found.
            STRING buffer;
            MgUtil::Int32ToString(readerId, buffer);

            MgStringCollection arguments;
            arguments.Add(L"1");
            arguments.Add(buffer);

            throw new MgInvalidArgumentException(L"MgServerFeatureService.GetRaster",
                __LINE__, __WFILE__, &arguments, L"MgReaderIdNotFound", NULL);
        }
        // Entry found and therefore get the raster
        byteReader = featureProcessor->GetRaster(xSize, ySize, propName);
    }

    return byteReader.Detach();
}


//////////////////////////////////////////////////////////////////
MgBatchPropertyCollection* MgServerFeatureService::GetSqlRows(INT32 lsqlReader)
{
    MgServerSqlProcessor* sqlReader = (MgServerSqlProcessor*)lsqlReader;

    MgServerSqlDataReaderPool* sqlDataReaderPool = MgServerSqlDataReaderPool::GetInstance();
    CHECKNULL(sqlDataReaderPool, L"MgServerFeatureService.GetSqlRows");

    if (!sqlDataReaderPool->Contains(sqlReader))
    {
        STRING buffer;
        MgUtil::Int32ToString(lsqlReader, buffer);

        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(buffer);

        throw new MgInvalidArgumentException(L"MgServerFeatureService.GetSqlRows",
            __LINE__, __WFILE__, &arguments, L"MgSqlReaderIdNotFound", NULL);
    }

    // Find out the counts from Configuration
    INT32 count = 1;
    MgConfiguration* config = MgConfiguration::GetInstance();

    config->GetIntValue(MgConfigProperties::FeatureServicePropertiesSection,
                        MgConfigProperties::FeatureServicePropertyDataCacheSize,
                        count,
                        MgConfigProperties::DefaultFeatureServicePropertyDataCacheSize);

    Ptr<MgBatchPropertyCollection> bpCol = sqlReader->GetRows(count);
    CHECKNULL((MgBatchPropertyCollection*)bpCol, L"MgServerFeatureService.GetSqlRows");

    return (bpCol->GetCount() > 0)? bpCol.Detach() : NULL;
}


//////////////////////////////////////////////////////////////////
bool MgServerFeatureService::CloseSqlReader(INT32 lsqlReader)
{
    bool retVal = false;
    MgServerSqlProcessor* sqlReader = (MgServerSqlProcessor*)lsqlReader;

    MgServerSqlDataReaderPool* sqlDataReaderPool = MgServerSqlDataReaderPool::GetInstance();
    CHECKNULL(sqlDataReaderPool, L"MgServerFeatureService.CloseSqlReader");

    if (sqlDataReaderPool->Contains(sqlReader))
    {
        sqlDataReaderPool->Remove(sqlReader);
        retVal = true;
    }

    return retVal;
}


//////////////////////////////////////////////////////////////////
MgBatchPropertyCollection* MgServerFeatureService::GetDataRows(INT32 ldataReader)
{
    MgServerDataProcessor* dataReader = (MgServerDataProcessor*)ldataReader;

    MgServerDataReaderPool* dataReaderPool = MgServerDataReaderPool::GetInstance();
    CHECKNULL(dataReaderPool, L"MgServerFeatureService.GetDataRows");

    if (!dataReaderPool->Contains(dataReader))
    {
        STRING buffer;
        MgUtil::Int32ToString(ldataReader, buffer);

        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(buffer);

        throw new MgInvalidArgumentException(L"MgServerFeatureService.GetDataRows",
             __LINE__, __WFILE__, &arguments, L"MgDataReaderIdNotFound", NULL);
    }

    // Find out the counts from Configuration
    INT32 count = 1;
    MgConfiguration* config = MgConfiguration::GetInstance();

    config->GetIntValue(MgConfigProperties::FeatureServicePropertiesSection,
                        MgConfigProperties::FeatureServicePropertyDataCacheSize,
                        count,
                        MgConfigProperties::DefaultFeatureServicePropertyDataCacheSize);

    Ptr<MgBatchPropertyCollection> bpCol = dataReader->GetRows(count);
    CHECKNULL((MgBatchPropertyCollection*)bpCol, L"MgServerFeatureService.GetDataRows");

    return (bpCol->GetCount() > 0)? bpCol.Detach() : NULL;
}


//////////////////////////////////////////////////////////////////
bool MgServerFeatureService::CloseDataReader(INT32 ldataReader)
{
    bool retVal = false;

    MgServerDataProcessor* dataReader = (MgServerDataProcessor*)ldataReader;

    // At this point we have the following:
    //   - the data processor is in the pool
    //   - the data processor references the data reader
    //   - the data reader references the data processor
    // The last two result in a circular reference which we have to break.

    // release the processor's reference to the reader - this causes the
    // reader to be destroyed and release its reference to the processor
    if (dataReader != NULL)
        dataReader->ClearDataReader();

    // now remove the processor from the pool - this should be the final
    // reference to the processor and will cause it to be destroyed
    MgServerDataReaderPool* dataReaderPool = MgServerDataReaderPool::GetInstance();
    CHECKNULL(dataReaderPool, L"MgServerFeatureService.CloseDataReader");

    if (dataReaderPool->Contains(dataReader))
    {
        dataReaderPool->Remove(dataReader);
        retVal = true;
    }

    return retVal;
}


//////////////////////////////////////////////////////////////////
void MgServerFeatureService::CreateFeatureSource(MgResourceIdentifier* resource, MgFeatureSourceParams* sourceParams)
{
    MgServerCreateFeatureSource msds;
    return msds.CreateFeatureSource(resource, sourceParams);
}


//////////////////////////////////////////////////////////////////
// Returns the collection of identity properties for the specified class.
// If schemaName is empty, then className needs to be fully qualified.
MgPropertyDefinitionCollection* MgServerFeatureService::GetIdentityProperties(MgResourceIdentifier* resource,
                                                                              CREFSTRING schemaName,
                                                                              CREFSTRING className)
{
    MgServerDescribeSchema msds;
    return msds.GetIdentityProperties(resource, schemaName, className);
}



//////////////////////////////////////////////////////////////////
/// \brief
/// Retrieves feature information based on the supplied criteria.
///
/// <!-- Syntax in .Net, Java, and PHP -->
/// \htmlinclude DotNetSyntaxTop.html
/// virtual MgByteReader GetWfsFeature(MgResourceIdentifier featureSourceId, string featureClass, MgStringCollection requiredProperties, string srs, string filter, int maxFeatures);
/// \htmlinclude SyntaxBottom.html
/// \htmlinclude JavaSyntaxTop.html
/// virtual MgByteReader GetWfsFeature(MgResourceIdentifier featureSourceId, String featureClass, MgStringCollection requiredProperties, String srs, String filter, int maxFeatures);
/// \htmlinclude SyntaxBottom.html
/// \htmlinclude PHPSyntaxTop.html
/// virtual MgByteReader GetWfsFeature(MgResourceIdentifier featureSourceId, string featureClass, MgStringCollection requiredProperties, string srs, string filter, int maxFeatures);
/// \htmlinclude SyntaxBottom.html
///
/// \param featureSourceId (MgResourceIdentifier)
/// The resource identifier defining the
/// location of the feature source in
/// the repository.
/// \param featureClass (String/string)
/// The feature class containing the features to retrieve.
/// \param requiredProperties (MgStringCollection)
/// The collection of properties to retrieve for each feature. If the
/// collection is null or empty, all properties will be retrieved.
/// \param srs (String/string)
/// The spatial reference system in which to return feature geometries
/// \param filter (String/string)
/// An XML string containing the definition for an OGC filter
/// \param filter (int)
/// The maximum number of features to retrieve. If the value is less
/// than or equal to zero, all features will be retrieved.
///
/// \return
/// Returns an MgByteReader containing the requested feature information.
///
MgByteReader* MgServerFeatureService::GetWfsFeature(MgResourceIdentifier* fs,
                                                     CREFSTRING featureName,
                                                     MgStringCollection* propNames,
                                                     CREFSTRING srs,
                                                     CREFSTRING wfsFilter,
                                                     INT32 maxFeatures)
{
    MgCoordinateSystemFactory fact;
    Ptr<MgCoordinateSystem> mapCs = (srs.empty()) ? NULL : fact.Create(srs);

    //get the coordinate system of the data --> we need this
    //so that we can convert the input geometry from mapping space
    //to layer's space

    // Need to determine the name of the spatial context for this layer
    // Parse the feature name for the schema and class
    STRING::size_type nDelimiter = featureName.find(L":");
    STRING schemaName;
    STRING className;

    if(STRING::npos == nDelimiter)
    {
        schemaName = L"";
        className = featureName;
    }
    else
    {
        schemaName = featureName.substr(0, nDelimiter);
        className = featureName.substr(nDelimiter + 1);
    }

    STRING spatialContextAssociation = L"";

    // Get the class definition so that we can find the spatial context association
    Ptr<MgClassDefinition> classDef = GetClassDefinition(fs, schemaName, className);
    Ptr<MgPropertyDefinitionCollection> propDefCol = classDef->GetProperties();

    // Find the spatial context for the geometric property. Use the first one if there are many defined.
    for(int index=0;index<propDefCol->GetCount();index++)
    {
        Ptr<MgPropertyDefinition> propDef = propDefCol->GetItem(index);
        if (propDef->GetPropertyType () == MgFeaturePropertyType::GeometricProperty)
        {
            // We found the geometric property
            MgGeometricPropertyDefinition* geomProp = static_cast<MgGeometricPropertyDefinition*>(propDef.p);
            spatialContextAssociation = geomProp->GetSpatialContextAssociation();
            break;
        }
    }

    // We want all of the spatial contexts
    Ptr<MgSpatialContextReader> csrdr = GetSpatialContexts(fs, false);

    // This is the strategy we use for picking the spatial context
    // Find the 1st spatial context that satisfies one of the following: (Processed in order)
    // 1) Matches the association spatial context name
    // 2) The 1st spatial context returned
    // 3) FAIL - none of the above could be satisfied

    Ptr<MgCoordinateSystem> layerCs = (MgCoordinateSystem*)NULL;

    if (mapCs)
    {
        STRING srcwkt = L"";
        STRING csrName = L"";
        bool bHaveFirstSpatialContext = false;

        while(csrdr->ReadNext())
        {
            csrName = csrdr->GetName();
            if((!spatialContextAssociation.empty()) && (csrName == spatialContextAssociation))
            {
                // Match found for the association)
                srcwkt = csrdr->GetCoordinateSystemWkt();
                break;
            }
            else if(!bHaveFirstSpatialContext)
            {
                // This is the 1st spatial context returned
                // This will be overwritten if we find the association
                srcwkt = csrdr->GetCoordinateSystemWkt();
                bHaveFirstSpatialContext = true;
            }
        }

        // Create coordinate system transformer

        // If the WKT is not defined, attempt to resolve it from the name.
        // This is a work around for MG298: WKT not set for WMS and 
        // WFS spatial contexts.
        if (srcwkt.empty())
        {
            try
            {
                Ptr<MgCoordinateSystem> csPtr = new MgCoordinateSystem();
                srcwkt = csPtr->ConvertCoordinateSystemCodeToWkt(csrName);
            }
            catch (MgException* e)
            {
                SAFE_RELEASE(e);
            }
            catch(...)
            {
                // Just use the empty WKT. 
            }
        }

        layerCs = (srcwkt.empty()) ? NULL : fact.Create(srcwkt);
    }
    else
    {
        // No coordinate system!!! 
        // We fail here and do not use a default
    }

    //we want to transform query geometry from mapping space to feature space
    Ptr<MgCoordinateSystemTransform> trans;

    if (mapCs && layerCs)
    {
        trans = new MgCoordinateSystemTransform(mapCs, layerCs);
    }

    //now get the schema -- we need to the feature class to get
    Ptr<MgFeatureSchemaCollection> fsc = DescribeSchema(fs, L"");

    assert(fsc->GetCount() > 0);

    //TODO:
    //get the first schema in the collection
    Ptr<MgFeatureSchema> schema = fsc->GetItem(0);

    Ptr<MgClassDefinitionCollection> classes = schema->GetClasses();

    //find the needed class definition
    Ptr<MgClassDefinition> fc;

    for (int i=0; i<classes->GetCount(); i++)
    {
        Ptr<MgClassDefinition> t = classes->GetItem(i);
        if (t->GetName() == featureName)
        {
            fc = t;
            break;
        }
    }

    assert(fc != NULL);
    if (fc == NULL)
        return NULL;

    STRING geomPropName = fc->GetDefaultGeometryPropertyName();

    //execute the spatial query
    Ptr<MgFeatureQueryOptions> options = new MgFeatureQueryOptions();

    //add the properties we need from FDO
    if (propNames)
    {
        for (int i=0; i<propNames->GetCount(); i++)
            options->AddFeatureProperty(propNames->GetItem(i));
    }

    //convert the WFS filter to an FDO filter
    //and set it to the FDO feature query
    if (!wfsFilter.empty())
    {
        MgOgcFilterUtil u;
        options->SetFilter(u.Ogc2FdoFilter(wfsFilter, trans));
    }

    // TODO: can FeatureName be an extension name rather than a FeatureClass?
    Ptr<MgFeatureReader> rdr = SelectFeatures(fs, featureName, options);

    //get underlying FDO feature reader
    GisPtr<FdoIFeatureReader> fdoRdr = ((MgServerFeatureReader*)(rdr.p))->GetInternalReader();

    //generate a file name to serialize wfs data
    STRING fileName = MgFileUtil::GenerateTempFileName(false, L"wfs", L"xml");

    GisPtr<FdoXmlFeatureFlags> flags = FdoXmlFeatureFlags::Create();
    flags->SetWriteCollection(true);
    flags->SetWriteMember(true);
    flags->SetCollectionUri(L"http://www.opengis.net/wfs");
    flags->SetCollectionName(L"FeatureCollection");
    flags->SetMemberName(L"featureMember");
    flags->SetMemberUri(L"http://www.opengis.net/gml");

    // set schemaLocations
    // gml schema location
    flags->SetSchemaLocation(L"http://www.opengis.net/gml", L"http://schemas.opengis.net/gml/2.1.2/feature.xsd");
    // wfs schema location
    flags->SetSchemaLocation(L"http://www.opengis.net/wfs", L"http://schemas.opengis.net/wfs/1.0.0/WFS-basic.xsd");
    // default namespace schema location
    flags->SetSchemaLocation(L"http://www.mynamespace.com/myns", L"http://www.mynamespace.com/myns/myns.xsd");
    // set the default namespace
    flags->SetDefaultNamespace(L"http://www.mynamespace.com/myns");

    //create the FDO xml serializer stack and write out the features
    GisPtr<GisXmlWriter> xmlWriter = GisXmlWriter::Create(fileName.c_str(), false);
    GisPtr<FdoXmlFeaturePropertyWriter> propWriter = FdoXmlFeaturePropertyWriter::Create(xmlWriter);
    GisPtr<FdoXmlFeatureWriter> featWriter = FdoXmlFeatureWriter::Create(propWriter, flags);

    FdoXmlFeatureSerializer::XmlSerialize(fdoRdr, featWriter, flags);

    flags = NULL;
    featWriter = NULL;
    propWriter = NULL;
    xmlWriter = NULL; //flush and close file

    //return the file
    Ptr<MgByteSource> src = new MgByteSource(fileName, true);
    src->SetMimeType(MgMimeType::Xml);

    return src->GetReader();
}


//////////////////////////////////////////////////////////////////
bool MgServerFeatureService::CloseGwsFeatureReader(INT32 gwsFeatureReader)
{
    bool retVal = false;

    // Is there anything to do here?  If nothing, then just return true.
    retVal = true;

    return retVal;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief
/// This method enumerates all the providers and if they are FDO enabled for 
/// the specified provider and partial connection string.
///
/// \param providerName (String/string)
/// The name of the Fdo feature provider.
/// \param partialConnString (String/string)
/// The partial connection string to the Fdo provider.
///
/// \returns
/// Returns the list of data stores.
MgByteReader* MgServerFeatureService::EnumerateDataStores(CREFSTRING providerName, CREFSTRING partialConnString)
{
    MG_LOG_TRACE_ENTRY(L"MgServerFeatureService::EnumerateDataStores()");

    MgServerEnumerateDataStores mseds;
    return mseds.EnumerateDataStores(providerName, partialConnString);
}

////////////////////////////////////////////////////////////////////////////////
/// \brief
/// This method returns all of the logical to physical schema mappings for 
/// the specified provider and partial connection string.
///
/// \param providerName (String/string)
/// The name of the Fdo feature provider.
/// \param partialConnString (String/string)
/// The partial connection string to the Fdo provider.
///
/// \returns
/// Returns the schema mapping.
MgByteReader* MgServerFeatureService::GetSchemaMapping(CREFSTRING providerName, CREFSTRING partialConnString)
{
    MG_LOG_TRACE_ENTRY(L"MgServerFeatureService::GetSchemaMapping()");

    MgServerGetSchemaMapping msgsm;
    return msgsm.GetSchemaMapping(providerName, partialConnString);
}

void MgServerFeatureService::UpdateFeatureServiceCache()
{
    MgFeatureServiceCache* featureServiceCache = MgFeatureServiceCache::GetInstance();
    if(featureServiceCache)
    {
        featureServiceCache->RemoveExpiredEntries();
    }
}

void MgServerFeatureService::RemoveFeatureServiceCacheEntries(MgSerializableCollection* changedResources)
{
    MgFeatureServiceCache* featureServiceCache = MgFeatureServiceCache::GetInstance();
    if(featureServiceCache)
    {
        if(changedResources)
        {
            // We have a list of changed resources
            INT32 numResources = changedResources->GetCount();
            for (INT32 i = 0; i < numResources; ++i)
            {
                Ptr<MgSerializable> serializableObj = changedResources->GetItem(i);
                MgResourceIdentifier* resource = dynamic_cast<MgResourceIdentifier*>(serializableObj.p);
                featureServiceCache->RemoveEntry(resource);
            }
        }
    }
}

void MgServerFeatureService::RemoveFeatureServiceCacheEntry(MgResourceIdentifier* resource)
{
    MgFeatureServiceCache* featureServiceCache = MgFeatureServiceCache::GetInstance();
    if(featureServiceCache)
    {
        featureServiceCache->RemoveEntry(resource);
    }
}

void MgServerFeatureService::ClearFeatureServiceCache()
{
    MgFeatureServiceCache* featureServiceCache = MgFeatureServiceCache::GetInstance();
    if(featureServiceCache)
    {
        featureServiceCache->ClearCache();
    }
}

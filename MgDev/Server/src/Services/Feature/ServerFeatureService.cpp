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

#include "ServerFeatureServiceDefs.h"
#include "ServerFeatureService.h"
#include "ServerGetFeatureProviders.h"
#include "ServerGetProviderCapabilities.h"
#include "ServerFeatureConnection.h"
#include "ServerApplySchema.h"
#include "ServerDescribeSchema.h"
#include "ServerSelectFeatures.h"
#include "ServerFeatureReaderPool.h"
#include "ServiceManager.h"
#include "ServerGetConnectionPropertyValues.h"
#include "ServerGetSpatialContexts.h"
#include "ServerGetLongTransactions.h"
#include "ServerSqlDataReaderPool.h"
#include "ServerSqlCommand.h"
#include "FeatureDistribution.h"
#include "ServerDataReaderPool.h"
#include "ServerFeatureUtil.h"
#include "ServerUpdateFeatures.h"
#include "ServerCreateFeatureSource.h"
#include "ServerFeatureReader.h"
#include "ServerFdoFeatureReader.h"
#include "ServerEnumerateDataStores.h"
#include "ServerGetSchemaMapping.h"
#include "FilterUtil.h"
#include "LongTransactionManager.h"
#include "TransformCache.h"
#include "CacheManager.h"
#include "LogDetail.h"
#include "ServerDataReader.h"
#include "ServerSqlDataReader.h"
#include "ServerFeatureTransaction.h"

#include <Fdo/Xml/FeatureSerializer.h>
#include <Fdo/Xml/FeatureWriter.h>
#include <Fdo/Xml/FeaturePropertyWriter.h>
#include <Fdo/Xml/FeatureFlags.h>

IMPLEMENT_CREATE_SERVICE(MgServerFeatureService)


//////////////////////////////////////////////////////////////////
/// <summary>
/// Default constructor - needed to prevent a compile error under GCC 3
/// </summary>
MgServerFeatureService::MgServerFeatureService() : MgFeatureService()
{
    // Get data cache size
    MgConfiguration* config = MgConfiguration::GetInstance();
    if(config)
    {
        config->GetIntValue(MgConfigProperties::FeatureServicePropertiesSection,
                            MgConfigProperties::FeatureServicePropertyDataCacheSize,
                            m_nDataCacheSize,
                            MgConfigProperties::DefaultFeatureServicePropertyDataCacheSize);
    }
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Destructor
/// </summary>
MgServerFeatureService::~MgServerFeatureService()
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
MgStringCollection* MgServerFeatureService::GetConnectionPropertyValues(CREFSTRING providerName,
                                                                        CREFSTRING propertyName,
                                                                        CREFSTRING partialConnString)
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

    bool result = false;

    MG_FEATURE_SERVICE_TRY()

    MgServerFeatureConnection msfc(providerName, connectionString);
    result = msfc.IsConnectionOpen();

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerFeatureService.TestConnection")

    return result;
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
bool MgServerFeatureService::TestConnection(MgResourceIdentifier* resource)
{
    MG_LOG_TRACE_ENTRY(L"MgServerFeatureService::TestConnection()");

    bool result = false;

    MG_FEATURE_SERVICE_TRY()

    MgServerFeatureConnection msfc(resource);
    result = msfc.IsConnectionOpen();

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerFeatureService.TestConnection")

    return result;
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
MgByteReader* MgServerFeatureService::GetCapabilities(CREFSTRING providerName)
{
    MG_LOG_TRACE_ENTRY(L"MgServerFeatureService::GetCapabilities()");

    Ptr<MgUserInformation> userInfo =  MgUserInformation::GetCurrentUserInfo();
    if (userInfo->GetApiVersion() != MG_API_VERSION(1,0,0) && userInfo->GetApiVersion() != MG_API_VERSION(2,0,0))
    {
        throw new MgInvalidOperationVersionException(
            L"MgServerFeatureService.GetCapabilities", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    STRING connectionString = L"";
    MgServerGetProviderCapabilities msgpc(providerName, connectionString);
    return msgpc.GetProviderCapabilities();
}


///////////////////////////////////////////////////////////////////////////
MgByteReader* MgServerFeatureService::GetCapabilities(CREFSTRING providerName, CREFSTRING connectionString)
{
    MG_LOG_TRACE_ENTRY(L"MgServerFeatureService::GetCapabilities()");

    MgServerGetProviderCapabilities msgpc(providerName, connectionString);
    return msgpc.GetProviderCapabilities();
}


///////////////////////////////////////////////////////////////////////////
/// \brief
/// Creates or updates a feature schema within the specified feature source.
///
void MgServerFeatureService::ApplySchema(MgResourceIdentifier* resource, MgFeatureSchema* schema)
{
    MG_LOG_TRACE_ENTRY(L"MgServerFeatureService::ApplySchema()");

    MgServerApplySchema msas;
    msas.ApplySchema(resource, schema);
}


///////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the definitions of one or more schemas contained in the
/// feature source for particular classes. If the specified schema name or
/// a class name does not exist, this method will throw an exception.
///
MgFeatureSchemaCollection* MgServerFeatureService::DescribeSchema(MgResourceIdentifier* resource,
                                                                  CREFSTRING schemaName,
                                                                  MgStringCollection* classNames)
{
    Ptr<MgFeatureSchemaCollection> schemas;

    MG_FEATURE_SERVICE_TRY()

    if (NULL == resource)
    {
        throw new MgNullArgumentException(
            L"MgServerFeatureService.DescribeSchema", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MgLogDetail logDetail(MgServiceType::FeatureService, MgLogDetail::Trace, L"MgServerFeatureService.DescribeSchema", mgStackParams);
    logDetail.AddResourceIdentifier(L"Resource", resource);
    logDetail.AddString(L"SchemaName", schemaName);
    logDetail.AddObject(L"ClassNames", classNames);
    logDetail.Create();

    MgServerDescribeSchema msds;
    schemas = msds.DescribeSchema(resource, schemaName, classNames);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerFeatureService.DescribeSchema");

    return schemas.Detach();
}


///////////////////////////////////////////////////////////////////////////////
/// This method has been deprecated. Use the above method.
///
MgFeatureSchemaCollection* MgServerFeatureService::DescribeSchema(MgResourceIdentifier* resource,
                                                                  CREFSTRING schemaName)
{
    return DescribeSchema(resource, schemaName, NULL);
}


///////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the definitions in XML format of one or more schemas contained in the
/// feature source for particular classes. If the specified schema name or
/// a class name does not exist, this method will throw an exception.
///
STRING MgServerFeatureService::DescribeSchemaAsXml(MgResourceIdentifier* resource,
                                                   CREFSTRING schemaName,
                                                   MgStringCollection* classNames)
{
    STRING schemaXml;

    MG_FEATURE_SERVICE_TRY()

    if (NULL == resource)
    {
        throw new MgNullArgumentException(
            L"MgServerFeatureService.DescribeSchemaAsXml", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MgLogDetail logDetail(MgServiceType::FeatureService, MgLogDetail::Trace, L"MgServerFeatureService.DescribeSchemaAsXml", mgStackParams);
    logDetail.AddResourceIdentifier(L"Resource", resource);
    logDetail.AddString(L"SchemaName", schemaName);
    logDetail.AddObject(L"ClassNames", classNames);
    logDetail.Create();

    MgServerDescribeSchema msds;
    schemaXml = msds.DescribeSchemaAsXml(resource, schemaName, classNames);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerFeatureService.DescribeSchemaAsXml");

    return schemaXml;
}


///////////////////////////////////////////////////////////////////////////////
/// This method has been deprecated. Use the above method.
///
STRING MgServerFeatureService::DescribeSchemaAsXml(MgResourceIdentifier* resource,
                                                   CREFSTRING schemaName)
{
    return DescribeSchemaAsXml(resource, schemaName, NULL);
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
MgFeatureReader* MgServerFeatureService::SelectFeatures(MgResourceIdentifier* resource,
                                                        CREFSTRING className,
                                                        MgFeatureQueryOptions* options)
{
    MG_LOG_TRACE_ENTRY(L"MgServerFeatureService::SelectFeatures()");

    Ptr<MgFeatureReader> reader;

    MG_FEATURE_SERVICE_TRY()

    if (NULL == resource)
    {
        throw new MgNullArgumentException(
            L"MgServerFeatureService.SelectFeatures", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MgLogDetail logDetail(MgServiceType::FeatureService, MgLogDetail::Trace, L"MgServerFeatureService.SelectFeatures",mgStackParams);
    logDetail.AddResourceIdentifier(L"Resource", resource);
    logDetail.AddString(L"ClassName", className);
    logDetail.AddObject(L"Options", options);
    logDetail.Create();

    MgServerSelectFeatures mssf;
    reader = (MgFeatureReader*)mssf.SelectFeatures(resource, className, options, false);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerFeatureService.SelectFeatures")

    return reader.Detach();
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Select features returns feature information from an Fdo Provider.  An MgFeatureReader
/// object provides one way, read-only iteration over the features.  The various properties
/// of a feature class are accessed by name using methods of the MgFeatureReader.
///
/// The desired feature class is specified by name. A subset of the feature properties
/// can be specified using the properties string collection.  Futhermore, an Fdo filter text
/// can be applied to limit the result set. The expected expected coordinate system name can
/// be specified.
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
/// <param name="coordinateSystem">Input
/// A valid coordinate system name. Retrieved features will be in the specified coordinate system.
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
MgFeatureReader* MgServerFeatureService::SelectFeatures(MgResourceIdentifier* resource,
                                                        CREFSTRING className,
                                                        MgFeatureQueryOptions* options,
                                                        CREFSTRING coordinateSystem)
{
    throw new MgNotImplementedException(
        L"MgServerFeatureService::SelectFeatures",
        __LINE__, __WFILE__, NULL, L"", NULL);

    return NULL; // to make some compiler happy
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
MgDataReader* MgServerFeatureService::SelectAggregate(MgResourceIdentifier* resource,
                                                      CREFSTRING className,
                                                      MgFeatureAggregateOptions* options)
{
    MG_LOG_TRACE_ENTRY(L"MgServerFeatureService::SelectAggregate()");

    Ptr<MgDataReader> reader;

    MG_FEATURE_SERVICE_TRY()

    MgServerSelectFeatures mssf;
    reader = (MgDataReader*)mssf.SelectFeatures(resource, className, options, true);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerFeatureService.SelectAggregate");

    return reader.Detach();
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
MgPropertyCollection* MgServerFeatureService::UpdateFeatures(MgResourceIdentifier* resource,
                                                             MgFeatureCommandCollection* commands,
                                                             bool useTransaction)
{
    MG_LOG_TRACE_ENTRY(L"MgServerFeatureService::UpdateFeatures()");

    MgServerUpdateFeatures asuf;
    return asuf.Execute(resource, commands, useTransaction);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// It executes all the commands specified in command collection
/// within the given transaction.
/// </summary>
/// <param name="resource">Input
/// A resource identifier referring to connection string
/// </param>
/// <param name="commands">Input
/// Collection of commands to be executed
/// </param>
/// <param name="transaction">Input
/// The MgTransaction instance on which the commands
/// will be executed.
/// </param>
/// <returns>
/// Integer collection referring to result for each command
/// Index of commandCollection and index of IntCollection would match the result.
/// </returns>
///
/// EXCEPTIONS:
/// MgInvalidResourceIdentifer
MgPropertyCollection* MgServerFeatureService::UpdateFeatures(MgResourceIdentifier* resource,
                                                             MgFeatureCommandCollection* commands,
                                                             MgTransaction* transaction)
{
    MG_LOG_TRACE_ENTRY(L"MgServerFeatureService::UpdateFeatures()");

    MgServerUpdateFeatures asuf;
    return asuf.Execute(resource, commands, transaction);
}


////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the locked features
/// </summary>
///
/// <param name="resource">Input
/// A resource identifier for the feature source.
/// </param>
/// <param name="className">Input
/// The name of the feature class on which
/// the select operation is performed.
/// </param>
/// <param name="options">Input
/// An MgFeatureAggregateOptions instance
/// containing all the criteria and filters
/// required for this select operation.
/// <param>
/// <returns>
/// Returns an MgFeatureReader containing the locked features.
/// </returns>
///
/// EXCEPTIONS:
/// MgFeatureServiceException
/// MgInvalidArgumentException
MgFeatureReader* MgServerFeatureService::GetLockedFeatures(MgResourceIdentifier* resource,
                                                           CREFSTRING className,
                                                           MgFeatureQueryOptions* options)
{
    throw new MgNotImplementedException(
        L"MgServerFeatureService::GetLockedFeatures",
        __LINE__, __WFILE__, NULL, L"", NULL);

    return NULL; // to make some compiler happy
}


////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Starts a transaction on the specified feature source
///
/// NOTE:
/// The returned MgTransaction instance has to be used along with ExecuteSqlQuery()
/// or ExecuteSqlNonQuery()
/// </summary>
/// <param name="resource">Input
/// A resource identifier referring to connection string
/// </param>
/// <returns>
/// Returns an MgTransaction instance (or NULL).
/// </returns>
///
/// EXCEPTIONS:
/// MgFeatureServiceException
/// MgInvalidArgumentException
/// MgInvalidOperationException
/// MgFdoException
MgTransaction* MgServerFeatureService::BeginTransaction(MgResourceIdentifier* resource)
{
    MG_LOG_TRACE_ENTRY(L"MgServerFeatureService::BeginTransaction()");

    Ptr<MgTransaction> transaction;

    MG_FEATURE_SERVICE_TRY()

    MgServerFeatureTransactionPool* transactionPool = MgServerFeatureTransactionPool::GetInstance();
    CHECKNULL(transactionPool, L"MgServerFeatureService.BeginTransaction");

    transaction = transactionPool->CreateTransaction(resource);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerFeatureService.BeginTransaction")

    return transaction.Detach();
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
MgSqlDataReader* MgServerFeatureService::ExecuteSqlQuery(MgResourceIdentifier* resource,
                                                         CREFSTRING sqlStatement)
{
    MG_LOG_TRACE_ENTRY(L"MgServerFeatureService::ExecuteSqlQuery()");

    return ExecuteSqlQuery(resource, sqlStatement, NULL, NULL);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// This method executes the SELECT SQL statement specified within a given transaction
/// and returns a pointer to SqlDataReader instance. This instance can be used to
/// retrieve column information and related values.
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
/// <param name="params">InputOutput
/// Parameters binded to the SQL statement.
/// </param>
/// <param name="transaction">Input
/// The MgTransaction instance on which the sql statement will be executed.
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
MgSqlDataReader* MgServerFeatureService::ExecuteSqlQuery(MgResourceIdentifier* resource,
                                                         CREFSTRING sqlStatement,
                                                         MgParameterCollection* params,
                                                         MgTransaction* transaction)
{
    MG_LOG_TRACE_ENTRY(L"MgServerFeatureService::ExecuteSqlQuery()");

    return ExecuteSqlQuery(resource, sqlStatement, params, transaction, m_nDataCacheSize);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// This method executes the SELECT SQL statement specified within a given transaction
/// and returns a pointer to SqlDataReader instance. This instance can be used to
/// retrieve column information and related values.
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
/// <param name="params">InputOutput
/// Parameters binded to the SQL statement.
/// </param>
/// <param name="transaction">Input
/// The MgTransaction instance on which the sql statement will be executed.
/// </param>
/// <param name="fetchSize">
/// The fetch size of query. This method returns all data of query if
/// setting the fetch size to 0.
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
MgSqlDataReader* MgServerFeatureService::ExecuteSqlQuery(MgResourceIdentifier* resource,
                                                         CREFSTRING sqlStatement,
                                                         MgParameterCollection* params,
                                                         MgTransaction* transaction,
                                                         INT32 fetchSize)
{
    MG_LOG_TRACE_ENTRY(L"MgServerFeatureService::ExecuteSqlQuery()");

    MgServerSqlCommand sqlCommand;
    return sqlCommand.ExecuteQuery(resource, sqlStatement, params, transaction, fetchSize);
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
INT32 MgServerFeatureService::ExecuteSqlNonQuery(MgResourceIdentifier* resource,
                                                 CREFSTRING sqlNonSelectStatement)
{
    MG_LOG_TRACE_ENTRY(L"MgServerFeatureService::ExecuteSqlNonQuery()");

    MgServerSqlCommand sqlCommand;
    return sqlCommand.ExecuteNonQuery(resource, sqlNonSelectStatement, NULL, NULL);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// This method executes all SQL statements supported by providers except SELECT
/// within a given transaction.
/// </summary>
/// <param name="resource">Input
/// A resource identifier referring to connection string
/// </param>
/// <param name="sqlNonSelectStatement">Input
/// This would allow users to specify free format SQL statement like INSERT/UPDATE/DELETE/CREATE
/// </param>
/// </param>
/// <param name="params">InputOutput
/// Parameters binded to the SQL statement.
/// </param>
/// <param name="transaction">Input
/// The MgTransaction instance on which the sql statement will be executed.
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
INT32 MgServerFeatureService::ExecuteSqlNonQuery(MgResourceIdentifier* resource,
                                                 CREFSTRING sqlNonSelectStatement,
                                                 MgParameterCollection* params,
                                                 MgTransaction* transaction)
{
    MG_LOG_TRACE_ENTRY(L"MgServerFeatureService::ExecuteSqlNonQuery()");

    MgServerSqlCommand sqlCommand;
    return sqlCommand.ExecuteNonQuery(resource, sqlNonSelectStatement, params, transaction);
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
/// This flag is obsolete and no longer used.
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
MgSpatialContextReader* MgServerFeatureService::GetSpatialContexts(MgResourceIdentifier* resource,
                                                                   bool bActiveOnly)
{
    Ptr<MgSpatialContextReader> reader;

    MG_FEATURE_SERVICE_TRY()

    if (NULL == resource)
    {
        throw new MgNullArgumentException(
            L"MgServerFeatureService.GetSpatialContexts", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MgLogDetail logDetail(MgServiceType::FeatureService, MgLogDetail::Trace, L"MgServerFeatureService.GetSpatialContexts", mgStackParams);
    logDetail.AddResourceIdentifier(L"Id", resource);
    logDetail.AddBool(L"ActiveOnly", bActiveOnly);
    logDetail.Create();

    MgServerGetSpatialContexts msgsc;
    reader = msgsc.GetSpatialContexts(resource);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerFeatureService.GetSpatialContexts")

    return reader.Detach();
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
MgLongTransactionReader* MgServerFeatureService::GetLongTransactions(MgResourceIdentifier* resource,
                                                                     bool bActiveOnly)
{
    MG_LOG_TRACE_ENTRY(L"MgServerFeatureService::GetLongTransactions()");

    MgServerGetLongTransactions msglt;
    return msglt.GetLongTransactions(resource, bActiveOnly);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Set the active long transaction name for a feature source.
///
/// The long transaction name is associated with the caller's session.  If
/// no session is set then the method throws an MgSessionNotFoundException.
/// </summary>
/// <param name="featureSourceId">Input
/// A resource identifier identifying a feature source in the repository.
/// </param>
/// <param name="longTransactionName">Input
/// The long transaction name to set.
/// </param>
/// <returns>
/// Returns true if the name was successfully set; otherwise returns false.
/// </returns>
///
/// EXCEPTIONS:
/// MgNullArgumentException
/// MgInvalidResourceTypeException
/// MgSessionNotFoundException
bool MgServerFeatureService::SetLongTransaction(MgResourceIdentifier* featureSourceId,
                                                CREFSTRING longTransactionName)
{
    MG_LOG_TRACE_ENTRY(L"MgServerFeatureService::SetLongTransaction()");

    CHECKARGUMENTNULL(featureSourceId, L"MgServerFeatureService.SetLongTransaction");

    if (featureSourceId->GetResourceType() != MgResourceType::FeatureSource)
        throw new MgInvalidResourceTypeException(L"MgServerFeatureService.SetLongTransaction", __LINE__, __WFILE__, NULL, L"", NULL);

    MG_FEATURE_SERVICE_TRY()

    // we require a session ID
    STRING sessionId;
    Ptr<MgUserInformation> userInfo = MgUserInformation::GetCurrentUserInfo();
    if (userInfo != NULL)
        sessionId = userInfo->GetMgSessionId();

    if (sessionId.empty())
        throw new MgSessionNotFoundException(L"MgServerFeatureService.SetLongTransaction", __LINE__, __WFILE__, NULL, L"", NULL);

    // set the name in the manager
    MgLongTransactionManager::SetLongTransactionName(sessionId, featureSourceId, longTransactionName);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerFeatureService.SetLongTransaction")

    return true;
}


///////////////////////////////////////////////////////////////////////////
// Just a simple hashing algorithm
unsigned MgServerFeatureService::StringHasher(FdoString* pszString)
{
  size_t i = 0;
  size_t iLen = wcslen(pszString);
  unsigned x = 0;
  unsigned uRet = 0;

  for(i = 0; i<iLen; i++) {
    uRet = (uRet << 4) + pszString[i];
    x = (uRet & 0xF0000000);
    if(x != 0)
      uRet = uRet ^ (x >> 24);
      uRet = uRet & (~ x);
  }
  return uRet;
}


///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Find a class definition from the specified schemas.
///
void MgServerFeatureService::FindClassDefinition(Ptr<MgFeatureSchemaCollection>& schemas,
    CREFSTRING schemaName, CREFSTRING className, REFSTRING schemaHash,
    Ptr<MgFeatureSchema>& schemaFound, Ptr<MgClassDefinition>& classFound)
{
    if (NULL == schemas.p)
    {
        throw new MgNullArgumentException(L"MgServerFeatureService.FindClassDefinition",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    INT32 schemaCount = schemas->GetCount();
    bool hashed = (0 == schemaName.find_first_of(
        MG_SCHEMA_NAME_HASH_PREFIX.c_str(), 0, MG_SCHEMA_NAME_HASH_PREFIX.length()));

    schemaHash = L"";
    schemaFound = NULL;
    classFound = NULL;

    for (INT32 i = 0; i < schemaCount; ++i)
    {
        Ptr<MgFeatureSchema> currSchema = schemas->GetItem(i);
        STRING currSchemaName = currSchema->GetName();

        if (!schemaName.empty())
        {
            if (hashed)
            {
                MgUtil::Int32ToString(StringHasher(currSchemaName.c_str()), schemaHash);
                schemaHash = MG_SCHEMA_NAME_HASH_PREFIX + schemaHash;

                if (schemaName != schemaHash)
                {
                    continue;
                }
            }
            else if (schemaName != currSchemaName)
            {
                continue;
            }
        }

        Ptr<MgClassDefinitionCollection> classes = currSchema->GetClasses();
        INT32 classCount = classes->GetCount();

        for (INT32 j = 0; j < classCount; ++j)
        {
            Ptr<MgClassDefinition> currClass = classes->GetItem(j);

            if (className == currClass->GetName())
            {
                schemaFound = currSchema;
                classFound = currClass;
                break;
            }
        }

        if (NULL != classFound.p)
        {
            break;
        }
    }
}


//////////////////////////////////////////////////////////////////
/// \brief
/// Retrieves schema information about a set of feature classes for a given feature source with specified namespace prefix and url.
///
/// <!-- Syntax in .Net, Java, and PHP -->
/// \htmlinclude DotNetSyntaxTop.html
/// virtual MgByteReader DescribeWfsFeatureType(MgResourceIdentifier featureSourceId, MgStringCollection featureClasses, string namespacePrefix, string namespaceUrl);
/// \htmlinclude SyntaxBottom.html
/// \htmlinclude JavaSyntaxTop.html
/// virtual MgByteReader DescribeWfsFeatureType(MgResourceIdentifier featureSourceId, MgStringCollection featureClasses, string namespacePrefix, string namespaceUrl);
/// \htmlinclude SyntaxBottom.html
/// \htmlinclude PHPSyntaxTop.html
/// virtual MgByteReader DescribeWfsFeatureType(MgResourceIdentifier featureSourceId, MgStringCollection featureClasses, string namespacePrefix, string namespaceUrl);
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
/// \param namespacePrefix (String/string)
/// A string identifying the namespace prefix in the output xml
/// \param namespaceUrl (String/string)
/// A string idenyifying the namespace url in the output xml
///
/// \return
/// Returns an MgByteReader containing the XML schema.
///
MgByteReader* MgServerFeatureService::DescribeWfsFeatureType(MgResourceIdentifier* featureSourceId,
                                                             MgStringCollection* featureClasses,
                                                             CREFSTRING namespacePrefix,
                                                             CREFSTRING namespaceUrl)
{
    MG_LOG_TRACE_ENTRY(L"MgServerFeatureService::DescribeWfsFeatureType()");

    Ptr<MgByteReader> byteReader;

    MG_FEATURE_SERVICE_TRY()

    STRING schemaName;
    Ptr<MgStringCollection> schemaNames = new MgStringCollection();
    Ptr<MgStringCollection> classNames = new MgStringCollection();

    if (NULL != featureClasses)
    {
        bool singleSchemaUsed = true;
        STRING::size_type hashPrefixLength = MG_SCHEMA_NAME_HASH_PREFIX.length();
        INT32 count = featureClasses->GetCount();

        for (INT32 i = 0; i < count; ++i)
        {
            STRING currSchemaName, currClassName;
            MgUtil::ParseQualifiedClassName(featureClasses->GetItem(i), currSchemaName, currClassName);

            if (!currClassName.empty())
            {
                schemaNames->Add(currSchemaName);
                classNames->Add(currClassName);
            }

            if (singleSchemaUsed)
            {
                if (0 == currSchemaName.find_first_of(MG_SCHEMA_NAME_HASH_PREFIX.c_str(), 0, hashPrefixLength))
                {
                    singleSchemaUsed = false;
                    schemaName = L"";
                }
                else if (0 == i)
                {
                    schemaName = currSchemaName;
                }
                else if (currSchemaName != schemaName)
                {
                    singleSchemaUsed = false;
                    schemaName = L"";
                }
            }
        }
    }

    Ptr<MgFeatureSchemaCollection> fsc = DescribeSchema(featureSourceId, schemaName, classNames);
    INT32 count = classNames->GetCount();

    if (count > 0)
    {
        Ptr<MgFeatureSchemaCollection> dfsc = new MgFeatureSchemaCollection();

        for (INT32 i = 0; i < count; ++i)
        {
            Ptr<MgFeatureSchema> schemaFound;
            Ptr<MgClassDefinition> classFound;
            STRING schemaHash;
            STRING currSchemaName = schemaNames->GetItem(i);
            STRING currClassName = classNames->GetItem(i);

            FindClassDefinition(fsc, currSchemaName, currClassName, schemaHash, schemaFound, classFound);

            if (NULL == classFound.p)
            {
                throw new MgObjectNotFoundException(L"DescribeWfsFeatureType",
                    __LINE__, __WFILE__, NULL, L"", NULL);
            }
            else
            {
                STRING featureSourceHash;
                bool bSchemaFound = false;

                MgUtil::Int32ToString(StringHasher(featureSourceId->ToString().c_str()),featureSourceHash);
                featureSourceHash = MG_FEATURE_SOURCE_HASH_PREFIX + featureSourceHash + schemaHash;

                Ptr<MgFeatureSchema> schemaToAdd = new MgFeatureSchema(featureSourceHash, schemaFound->GetDescription());
                
                for(int i =0; i<dfsc->GetCount(); i++)
                {
                    Ptr<MgFeatureSchema> fSchema = dfsc->GetItem(i);
                    if(fSchema->GetName() == schemaToAdd->GetName())
                    {
                        bSchemaFound = true;
                        Ptr<MgClassDefinitionCollection> classes = fSchema->GetClasses();
                        classes->Add(classFound);
                        break;
                    }
                }

                if(!bSchemaFound)
                {
                    Ptr<MgClassDefinitionCollection> classes = schemaToAdd->GetClasses();

                    classes->Add(classFound);
                    dfsc->Add(schemaToAdd);
                }
            }
        }

        fsc = dfsc;
    }

    STRING sch = SchemaToXml(fsc, namespacePrefix, namespaceUrl);

    string utfsch = MgUtil::WideCharToMultiByte(sch);

    Ptr<MgByteSource> bs = new MgByteSource((unsigned char*)utfsch.c_str(), (INT32)utfsch.length());
    bs->SetMimeType(MgMimeType::Xml);

    byteReader = bs->GetReader();

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerFeatureService.DescribeWfsFeatureType")

    return byteReader.Detach();
}

///////////////////////////////////////////////////////////////////////////
/// This method has been deprecated. Use the above method.
///
MgByteReader* MgServerFeatureService::DescribeWfsFeatureType(MgResourceIdentifier* featureSourceId,
                                                             MgStringCollection* featureClasses)
{
    return DescribeWfsFeatureType(featureSourceId, featureClasses, L"", L"");
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
MgBatchPropertyCollection* MgServerFeatureService::GetFeatures(CREFSTRING featureReader)
{
    MG_LOG_TRACE_ENTRY(L"MgServerFeatureService::GetFeatures()");

    Ptr<MgBatchPropertyCollection> bpCol;

    MG_FEATURE_SERVICE_TRY()

    MgServerFeatureReaderPool* featPool = MgServerFeatureReaderPool::GetInstance();
    CHECKNULL(featPool, L"MgServerFeatureService.GetFeatures");

    Ptr<MgFeatureReader> reader = featPool->GetReader(featureReader);
    if (NULL == reader.p)
    {
        STRING buffer;

        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(featureReader);

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

    Ptr<MgFeatureSet> featSet;
    featSet = reader->GetFeatures(count);

    CHECKNULL((MgFeatureSet*)featSet, L"MgServerFeatureService.GetFeatures");

    bpCol = featSet->GetFeatures();
    CHECKNULL((MgBatchPropertyCollection*)bpCol, L"MgServerFeatureService.GetFeatures");

    MG_FEATURE_SERVICE_CATCH(L"MgServerFeatureService.GetFeatures")
    if(NULL != mgException.p)
    {
        // Cleanup the reader as an exception has occured
        MG_FEATURE_SERVICE_TRY()
        CloseFeatureReader(featureReader);
        MG_FEATURE_SERVICE_CATCH(L"MgServerFeatureService.GetFeatures")
    }

    MG_FEATURE_SERVICE_THROW()

    return (bpCol->GetCount() > 0)? bpCol.Detach() : NULL;
}


//////////////////////////////////////////////////////////////////
bool MgServerFeatureService::CloseFeatureReader(CREFSTRING featureReader)
{
    MG_LOG_TRACE_ENTRY(L"MgServerFeatureService::CloseFeatureReader()");

    bool retVal = false;

    MG_FEATURE_SERVICE_TRY()

    MgServerFeatureReaderPool* featPool = MgServerFeatureReaderPool::GetInstance();
    CHECKNULL(featPool, L"MgServerFeatureService.CloseFeatureReader");

    Ptr<MgFeatureReader> reader = featPool->GetReader(featureReader);
    if (NULL != reader.p)
    {
        reader->Close();
    }

    retVal = featPool->Remove(featureReader);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerFeatureService.CloseFeatureReader")

    return retVal;
}


//////////////////////////////////////////////////////////////////
MgStringCollection* MgServerFeatureService::GetSchemas( MgResourceIdentifier* resource )
{
    MG_LOG_TRACE_ENTRY(L"MgServerFeatureService::GetSchemas()");

    MgServerDescribeSchema msds;
    return msds.GetSchemas(resource);
}


//////////////////////////////////////////////////////////////////
MgStringCollection* MgServerFeatureService::GetClasses( MgResourceIdentifier* resource, CREFSTRING schemaName )
{
    MG_LOG_TRACE_ENTRY(L"MgServerFeatureService::GetClasses()");

    MgServerDescribeSchema msds;
    return msds.GetClasses(resource, schemaName);
}


//////////////////////////////////////////////////////////////////
MgClassDefinition* MgServerFeatureService::GetClassDefinition(MgResourceIdentifier* resource,
                                                              CREFSTRING schemaName,
                                                              CREFSTRING className)
{
    MG_LOG_TRACE_ENTRY(L"MgServerFeatureService::GetClassDefinition()");

    MgServerDescribeSchema msds;
    return msds.GetClassDefinition(resource, schemaName, className, true);
}


//////////////////////////////////////////////////////////////////
MgClassDefinition* MgServerFeatureService::GetClassDefinition(MgResourceIdentifier* resource,
                                                              CREFSTRING schemaName,
                                                              CREFSTRING className,
                                                              bool serialize)
{
    MG_LOG_TRACE_ENTRY(L"MgServerFeatureService::GetClassDefinition()");

    MgServerDescribeSchema msds;
    return msds.GetClassDefinition(resource, schemaName, className, serialize);
}


//////////////////////////////////////////////////////////////////
STRING MgServerFeatureService::SchemaToXml(MgFeatureSchemaCollection* schema)
{
    MG_LOG_TRACE_ENTRY(L"MgServerFeatureService::SchemaToXml()");

    MgServerDescribeSchema msds;
    return msds.SchemaToXml(schema);
}

//////////////////////////////////////////////////////////////////
STRING MgServerFeatureService::SchemaToXml(MgFeatureSchemaCollection* schema, CREFSTRING namespacePrefix, CREFSTRING namespaceUrl)
{
    MG_LOG_TRACE_ENTRY(L"MgServerFeatureService::SchemaToXml()");

    MgServerDescribeSchema msds;
    return msds.SchemaToXml(schema, namespacePrefix, namespaceUrl);
}

//////////////////////////////////////////////////////////////////
MgFeatureSchemaCollection* MgServerFeatureService::XmlToSchema(CREFSTRING xml)
{
    MG_LOG_TRACE_ENTRY(L"MgServerFeatureService::XmlToSchema()");

    MgServerDescribeSchema msds;
    return msds.XmlToSchema(xml);
}


//////////////////////////////////////////////////////////////////
MgByteReader* MgServerFeatureService::GetRaster(CREFSTRING reader, INT32 xSize, INT32 ySize, STRING propName)
{
    MG_LOG_TRACE_ENTRY(L"MgServerFeatureService::GetRaster()");

    Ptr<MgByteReader> byteReader;

    MG_FEATURE_SERVICE_TRY()

    MgServerFeatureReaderPool* featPool = MgServerFeatureReaderPool::GetInstance();
    CHECKNULL(featPool, L"MgServerFeatureService.GetRaster");

    Ptr<MgFeatureReader> featureReader = featPool->GetReader(reader);
    if (NULL != featureReader.p)
    {
        // Entry found and therefore get the raster
        MgServerFeatureReader* sfr = dynamic_cast<MgServerFeatureReader*>(featureReader.p);
        if(NULL != sfr)
        {
            byteReader = sfr->GetRaster(propName, xSize, ySize);
        }
    }
    else
    {
        // No entry in pool is found.
        STRING buffer;

        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(reader);

        throw new MgInvalidArgumentException(L"MgServerFeatureService.GetRaster",
            __LINE__, __WFILE__, &arguments, L"MgReaderIdNotFound", NULL);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerFeatureService.GetRaster")

    return byteReader.Detach();
}


//////////////////////////////////////////////////////////////////
MgBatchPropertyCollection* MgServerFeatureService::GetSqlRows(CREFSTRING sqlReader)
{
    MG_LOG_TRACE_ENTRY(L"MgServerFeatureService::GetSqlRows()");

    Ptr<MgBatchPropertyCollection> bpCol;

    MG_FEATURE_SERVICE_TRY()

    MgServerSqlDataReaderPool* sqlDataReaderPool = MgServerSqlDataReaderPool::GetInstance();
    CHECKNULL(sqlDataReaderPool, L"MgServerFeatureService.GetSqlRows");

    Ptr<MgServerSqlDataReader> sqlDataReader = sqlDataReaderPool->GetReader(sqlReader);
    if (NULL == sqlDataReader.p)
    {
        STRING buffer;

        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(sqlReader);

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

    bpCol = sqlDataReader->GetRows(count);
    CHECKNULL((MgBatchPropertyCollection*)bpCol, L"MgServerFeatureService.GetSqlRows");

    MG_FEATURE_SERVICE_CATCH(L"MgServerFeatureService.GetSqlRows")
    if(NULL != mgException.p)
    {
        // Cleanup the reader as an exception has occured
        MG_FEATURE_SERVICE_TRY()
        CloseSqlReader(sqlReader);
        MG_FEATURE_SERVICE_CATCH(L"MgServerFeatureService.GetSqlRows")
    }

    MG_FEATURE_SERVICE_THROW()

    return (bpCol->GetCount() > 0)? bpCol.Detach() : NULL;
}


//////////////////////////////////////////////////////////////////
bool MgServerFeatureService::CloseSqlReader(CREFSTRING sqlReader)
{
    MG_LOG_TRACE_ENTRY(L"MgServerFeatureService::CloseSqlReader()");

    bool retVal = false;

    MG_FEATURE_SERVICE_TRY()

    MgServerSqlDataReaderPool* sqlDataReaderPool = MgServerSqlDataReaderPool::GetInstance();
    CHECKNULL(sqlDataReaderPool, L"MgServerFeatureService.CloseSqlReader");

    Ptr<MgServerSqlDataReader> reader = sqlDataReaderPool->GetReader(sqlReader);
    if (NULL != reader.p)
    {
        reader->Close();
    }

    retVal = sqlDataReaderPool->Remove(sqlReader);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerFeatureService.CloseSqlReader")

    return retVal;
}


//////////////////////////////////////////////////////////////////
MgBatchPropertyCollection* MgServerFeatureService::GetDataRows(CREFSTRING dataReader)
{
    MG_LOG_TRACE_ENTRY(L"MgServerFeatureService::GetDataRows()");

    Ptr<MgBatchPropertyCollection> bpCol;

    MG_FEATURE_SERVICE_TRY()

    MgServerDataReaderPool* dataReaderPool = MgServerDataReaderPool::GetInstance();
    CHECKNULL(dataReaderPool, L"MgServerFeatureService.GetDataRows");

    Ptr<MgServerDataReader> reader = dataReaderPool->GetReader(dataReader);
    if (NULL == reader.p)
    {
        STRING buffer;

        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(dataReader);

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

    bpCol = reader->GetRows(count);
    CHECKNULL((MgBatchPropertyCollection*)bpCol, L"MgServerFeatureService.GetDataRows");

    MG_FEATURE_SERVICE_CATCH(L"MgServerFeatureService.GetDataRows")
    if(NULL != mgException.p)
    {
        // Cleanup the reader as an exception has occured
        MG_FEATURE_SERVICE_TRY()
        CloseDataReader(dataReader);
        MG_FEATURE_SERVICE_CATCH(L"MgServerFeatureService.GetDataRows")
    }

    MG_FEATURE_SERVICE_THROW()

    return (bpCol->GetCount() > 0)? bpCol.Detach() : NULL;
}


//////////////////////////////////////////////////////////////////
bool MgServerFeatureService::CloseDataReader(CREFSTRING dataReader)
{
    MG_LOG_TRACE_ENTRY(L"MgServerFeatureService::CloseDataReader()");

    bool retVal = false;

    MG_FEATURE_SERVICE_TRY()

    MgServerDataReaderPool* dataReaderPool = MgServerDataReaderPool::GetInstance();
    CHECKNULL(dataReaderPool, L"MgServerFeatureService.CloseDataReader");

    Ptr<MgServerDataReader> reader = dataReaderPool->GetReader(dataReader);
    if(NULL != reader.p)
    {
        reader->Close();
    }

    retVal = dataReaderPool->Remove(dataReader);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerFeatureService.CloseDataReader")

    return retVal;
}


//////////////////////////////////////////////////////////////////
void MgServerFeatureService::CreateFeatureSource(MgResourceIdentifier* resource, MgFeatureSourceParams* sourceParams)
{
    MG_LOG_TRACE_ENTRY(L"MgServerFeatureService::CreateFeatureSource()");

    MgServerCreateFeatureSource msds;
    msds.CreateFeatureSource(resource, sourceParams);
}


//////////////////////////////////////////////////////////////////
// Returns the collection of identity properties for the specified class.
// If schemaName is empty, then className needs to be fully qualified.
MgClassDefinitionCollection* MgServerFeatureService::GetIdentityProperties(MgResourceIdentifier* resource,
                                                                           CREFSTRING schemaName,
                                                                           MgStringCollection* classNames)
{
    MG_LOG_TRACE_ENTRY(L"MgServerFeatureService::GetIdentityProperties()");

    MgServerDescribeSchema msds;
    return msds.GetIdentityProperties(resource, schemaName, classNames);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief
/// Retrieves feature information based on the supplied criteria with specified format.
///
///
/// <!-- Syntax in .Net, Java, and PHP -->
/// \htmlinclude DotNetSyntaxTop.html
/// virtual MgByteReader GetWfsFeature(MgResourceIdentifier featureSourceId, string featureClass, MgStringCollection requiredProperties, string srs, string filter, int maxFeatures, string wfsVersion, string outputFormat, string sortCriteria, string namespacePrefix, string namespaceUrl);
/// \htmlinclude SyntaxBottom.html
/// \htmlinclude JavaSyntaxTop.html
/// virtual MgByteReader GetWfsFeature(MgResourceIdentifier featureSourceId, string featureClass, MgStringCollection requiredProperties, string srs, string filter, int maxFeatures, string wfsVersion, string outputFormat, string sortCriteria, string namespacePrefix, string namespaceUrl);
/// \htmlinclude SyntaxBottom.html
/// \htmlinclude PHPSyntaxTop.html
/// virtual MgByteReader GetWfsFeature(MgResourceIdentifier featureSourceId, string featureClass, MgStringCollection requiredProperties, string srs, string filter, int maxFeatures, string wfsVersion, string outputFormat, string sortCriteria, string namespacePrefix, string namespaceUrl);
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
/// \param maxFeatures (int)
/// The maximum number of features to retrieve. If the value is less
/// than or equal to zero, all features will be retrieved.
/// \param wfsVersion (String/string)
/// A string identifying the wfs version
/// \param outputFormat (String/string)
/// A string identifying the output format of
/// the retrieved feature information.
/// The supported values of output format are specified in OpenGIS Web Feature Service (WFS) Implementation Specification - section 9.2
/// http://portal.opengeospatial.org/files/?artifact_id=8339
/// \param sortCriteria (String/string)
/// A string identifying the sort criteria
/// \param namespacePrefix (String/string)
/// A string identifying the namespace prefix in the output xml
/// \param namespaceUrl (String/string)
/// A string idenyifying the namespace url in the output xml
///
/// \return
/// Returns an MgByteReader containing the requested feature information.
///
/// EXCEPTIONS:
/// MgInvalidArgumentException
MgByteReader* MgServerFeatureService::GetWfsFeature(MgResourceIdentifier* fs,
                                                    CREFSTRING featureClass,
                                                    MgStringCollection* propNames,
                                                    CREFSTRING srs,
                                                    CREFSTRING wfsFilter,
                                                    INT32 maxFeatures,
                                                    CREFSTRING wfsVersion,
                                                    CREFSTRING outputFormat,
                                                    CREFSTRING sortCriteria,
                                                    CREFSTRING namespacePrefix,
                                                    CREFSTRING namespaceUrl)
{
    MG_LOG_TRACE_ENTRY(L"MgServerFeatureService::GetWfsFeature()");

    Ptr<MgByteReader> byteReader;
    TransformCacheMap transformCache;

    MG_FEATURE_SERVICE_TRY()

    STRING lfeatureName = featureClass;
    STRING schemaName, className;
    MgUtil::ParseQualifiedClassName(lfeatureName, schemaName, className);

    bool hashed = (0 == schemaName.find_first_of(
        MG_SCHEMA_NAME_HASH_PREFIX.c_str(), 0, MG_SCHEMA_NAME_HASH_PREFIX.length()));
    Ptr<MgStringCollection> classNames = new MgStringCollection();

    if (!className.empty())
    {
        classNames->Add(className);
    }

    // Find the needed class definition.
    Ptr<MgFeatureSchemaCollection> fsc = DescribeSchema(
        fs, hashed ? L"" : schemaName, classNames);
    Ptr<MgFeatureSchema> schema;
    Ptr<MgClassDefinition> fc;
    STRING schemaHash;

    FindClassDefinition(fsc, schemaName, className, schemaHash, schema, fc);

    if (hashed && NULL != schema.p)
    {
        MgUtil::FormatQualifiedClassName(schema->GetName(), className, lfeatureName);
    }

    MgCoordinateSystemFactory fact;
    Ptr<MgCoordinateSystem> mapCs = (srs.empty()) ? NULL : fact.Create(srs);

    //get a transform from feature space to mapping space
    TransformCache* item = TransformCache::GetLayerToMapTransform(transformCache, lfeatureName, fs, mapCs, &fact, this);
    Ptr<MgCoordinateSystemTransform> trans = item? item->GetMgTransform() : NULL;

    assert(fc != NULL);
    if (fc == NULL)
        return NULL;

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
        STRING GEOM_PROP_TAG = L"%MG_GEOM_PROP%"; //NOXLATE
        STRING fdoFilterString = L""; //NOXLATE

        Ptr<MgPropertyDefinitionCollection> properties = fc->GetProperties();
        MgOgcFilterUtil u;
        
        for(int i = 0; i<properties->GetCount(); i++)
        {
            Ptr<MgPropertyDefinition> prop = properties->GetItem(i);
            if(prop->GetPropertyType() == MgFeaturePropertyType::GeometricProperty)
            {
                STRING ogcFilter = wfsFilter;

                if(wfsFilter.find(GEOM_PROP_TAG, 0) != STRING::npos)
                {
                    ogcFilter = MgUtil::ReplaceString(wfsFilter,GEOM_PROP_TAG.c_str(),prop->GetName().c_str());
                }
                if(!fdoFilterString.empty())
                {
                    fdoFilterString += L" OR "; //NOXLATE
                }

                fdoFilterString += u.Ogc2FdoFilter(ogcFilter, trans, prop->GetName(), properties);
            }
        }

        options->SetFilter(fdoFilterString);
    }

    if(!sortCriteria.empty())
    {
        Ptr<MgStringCollection> orderByProperties = new MgStringCollection();
        int orderOption = MgOrderingOption::Ascending;

        STRING sSortCriteria = sortCriteria;
        STRING::size_type pos = sSortCriteria.find_last_of(L" ");
        if(pos != STRING::npos)
        {
            STRING sSortByProperty = sSortCriteria.substr(0, pos);
            orderByProperties->Add(sSortByProperty);

            STRING sSortOption  = MgUtil::ToUpper(sSortCriteria.substr(pos+1));
            if(sSortOption == L"D")
            {
                orderOption = MgOrderingOption::Descending;
            }
        }
        else
        {
            orderByProperties->Add(sortCriteria);
        }

        options->SetOrderingFilter(orderByProperties,orderOption);
    }
    // TODO: can FeatureName be an extension name rather than a FeatureClass?
    Ptr<MgFeatureReader> mgfReader = SelectFeatures(fs, lfeatureName, options);

    //get underlying FDO feature reader
    FdoPtr<FdoIFeatureReader> fdoReader = (NULL == mgfReader.p) ? NULL : ((MgServerFeatureReader*)(mgfReader.p))->GetInternalReader();

    //wrap the reader to enforce maxfeatures
    MgServerFdoFeatureReader maxFeatureReader(fdoReader);
    maxFeatureReader.SetMaxFeatures(maxFeatures);


    //generate a file name to serialize wfs data
    STRING fileName = MgFileUtil::GenerateTempFileName(false, L"wfs", L"xml");

    FdoPtr<FdoXmlFeatureFlags> flags = FdoXmlFeatureFlags::Create();
    flags->SetWriteCollection(true);
    flags->SetWriteMember(true);
    flags->SetCollectionUri(L"http://www.opengis.net/wfs");
    flags->SetCollectionName(L"FeatureCollection");
    flags->SetMemberName(L"featureMember");
    flags->SetMemberUri(L"http://www.opengis.net/gml");

    // gml schema location and version
    if(L"text/xml; subtype=gml/2.1.2" == outputFormat)
    {
        flags->SetSchemaLocation(L"http://www.opengis.net/gml", L"http://schemas.opengis.net/gml/2.1.2/feature.xsd");
        flags->SetGmlVersion(FdoGmlVersion_212);
    }
    else if(L"text/xml; subtype=gml/3.1.1" == outputFormat)
    {
        flags->SetSchemaLocation(L"http://www.opengis.net/gml", L"http://schemas.opengis.net/gml/3.1.1/base/feature.xsd");
        flags->SetGmlVersion(FdoGmlVersion_311);
    }
    else
    {
        throw new MgInvalidArgumentException(L"MgServerFeatureService.GetWfsFeature",
        __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // For WFS CITE certification
    // gml_id, gml_name and gml_description properties must be serialized with gml namespace.
    Ptr<MgPropertyDefinitionCollection> properties = fc->GetProperties();
    for(int i = 0; i<properties->GetCount(); i++)
    {
        Ptr<MgPropertyDefinition> prop = properties->GetItem(i);
        STRING propName = prop->GetName();
        
        if(L"GML_ID" == MgUtil::ToUpper(propName))
        {
             FdoPtr<FdoStringCollection> gmlIDRelatePropertyNames = FdoStringCollection::Create();
             gmlIDRelatePropertyNames->Add(propName.c_str());
             flags->SetGmlIDRelatePropertyNames(gmlIDRelatePropertyNames);
        }
        else if(L"GML_NAME" == MgUtil::ToUpper(propName))
        {
            flags->SetGmlNameRelatePropertyName(propName.c_str()); 
        }
        else if(L"GML_DESCRIPTION" == MgUtil::ToUpper(propName))
        {
            flags->SetGmlDescriptionRelatePropertyName(propName.c_str()); 
        }
    }

    // wfs schema location
    if(L"1.0.0" == wfsVersion)
    {
        flags->SetSchemaLocation(L"http://www.opengis.net/wfs", L"http://schemas.opengis.net/wfs/1.0.0/WFS-basic.xsd");
    }
    else if(L"1.1.0" == wfsVersion)
    {
        flags->SetSchemaLocation(L"http://www.opengis.net/wfs", L"http://schemas.opengis.net/wfs/1.1.0/wfs.xsd");
    }
    else
    {
        throw new MgInvalidArgumentException(L"MgServerFeatureService.GetWfsFeature",
        __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // set the default namespace prefix
    if(!namespacePrefix.empty())
    {
        flags->SetDefaultNamespacePrefix(namespacePrefix.c_str());
    }

    // set the default namespace
    if(!namespaceUrl.empty())
    {
        flags->SetDefaultNamespace(namespaceUrl.c_str());
    }
    else
    {
        STRING defaultNamespace(L"http://fdo.osgeo.org/schemas/feature/");
        defaultNamespace += namespacePrefix;
        flags->SetDefaultNamespace(defaultNamespace.c_str());
    }

    //create the FDO xml serializer stack and write out the features
    FdoPtr<FdoXmlWriter> xmlWriter = FdoXmlWriter::Create(fileName.c_str(), false);
    FdoPtr<FdoXmlFeaturePropertyWriter> propWriter = FdoXmlFeaturePropertyWriter::Create(xmlWriter,flags);
    FdoPtr<FdoXmlFeatureWriter> featWriter = FdoXmlFeatureWriter::Create(propWriter, flags);

    FdoXmlFeatureSerializer::XmlSerialize(&maxFeatureReader, featWriter, flags);

    mgfReader->Close();
    flags = NULL;
    featWriter = NULL;
    propWriter = NULL;
    xmlWriter = NULL; //flush and close file

    //return the file
    Ptr<MgByteSource> src = new MgByteSource(fileName, true);
    src->SetMimeType(MgMimeType::Xml);
    byteReader = src->GetReader();

    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH(fs, L"MgServerFeatureService.GetWfsFeature")

    TransformCache::Clear(transformCache);

    MG_FEATURE_SERVICE_THROW()

    return byteReader.Detach();
}


///////////////////////////////////////////////////////////////////////////
/// This method has been deprecated. Use the above method.
///
MgByteReader* MgServerFeatureService::GetWfsFeature(MgResourceIdentifier* fs,
                                                    CREFSTRING featureClass,
                                                    MgStringCollection* propNames,
                                                    CREFSTRING srs,
                                                    CREFSTRING wfsFilter,
                                                    INT32 maxFeatures)
{
    return GetWfsFeature(fs, featureClass, propNames, srs, wfsFilter, maxFeatures, L"1.0.0", L"text/xml; subtype=gml/2.1.2", L"", L"", L"");
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


////////////////////////////////////////////////////////////////////////////////
void MgServerFeatureService::SetConnectionProperties(MgConnectionProperties*)
{
    // Do nothing.  No connection properties are required for Server-side service objects.
}


///////////////////////////////////////////////////////////////////////////
bool MgServerFeatureService::CommitTransaction(CREFSTRING transactionId)
{
    MG_LOG_TRACE_ENTRY(L"MgServerFeatureService::CommitTransaction()");

    bool bResult = false;

    MG_FEATURE_SERVICE_TRY()

    MgServerFeatureTransactionPool* transactionPool = MgServerFeatureTransactionPool::GetInstance();
    CHECKNULL(transactionPool, L"MgServerFeatureService.CommitTransaction");

    bResult = transactionPool->CommitTransaction(transactionId);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerFeatureService.CommitTransaction")

    return bResult;
}


///////////////////////////////////////////////////////////////////////////
bool MgServerFeatureService::RollbackTransaction(CREFSTRING transactionId)
{
    MG_LOG_TRACE_ENTRY(L"MgServerFeatureService::RollbackTransaction()");

    bool bResult = false;

    MG_FEATURE_SERVICE_TRY()

    MgServerFeatureTransactionPool* transactionPool = MgServerFeatureTransactionPool::GetInstance();
    CHECKNULL(transactionPool, L"MgServerFeatureService.RollbackTransaction");

    bResult = transactionPool->RollbackTransaction(transactionId);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerFeatureService.RollbackTransaction")

    return bResult;
}

///////////////////////////////////////////////////////////////////////////
STRING MgServerFeatureService::AddSavePoint(CREFSTRING transactionId, CREFSTRING suggestName)
{
    MG_LOG_TRACE_ENTRY(L"MgServerFeatureService::AddSavePoint()");

    STRING resultName;

    MG_FEATURE_SERVICE_TRY()

    MgServerFeatureTransactionPool* transactionPool = MgServerFeatureTransactionPool::GetInstance();
    CHECKNULL(transactionPool, L"MgServerFeatureService.AddSavePoint");

    resultName = transactionPool->AddSavePoint(transactionId, suggestName);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerFeatureService.AddSavePoint")

    return resultName;
}

bool MgServerFeatureService::RollbackSavePoint(CREFSTRING transactionId, CREFSTRING savePointName)
{
    MG_LOG_TRACE_ENTRY(L"MgServerFeatureService::RollbackSavePoint()");

    bool bResult = false;

    MG_FEATURE_SERVICE_TRY()

    MgServerFeatureTransactionPool* transactionPool = MgServerFeatureTransactionPool::GetInstance();
    CHECKNULL(transactionPool, L"MgServerFeatureService.RollbackSavePoint");

    bResult = transactionPool->RollbackSavePoint(transactionId, savePointName);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerFeatureService.RollbackSavePoint")

    return bResult;
}

///////////////////////////////////////////////////////////////////////////
bool MgServerFeatureService::ReleaseSavePoint(CREFSTRING transactionId, CREFSTRING savePointName)
{
   MG_LOG_TRACE_ENTRY(L"MgServerFeatureService::ReleaseSavePoint()");

    bool bResult = false;

    MG_FEATURE_SERVICE_TRY()

    MgServerFeatureTransactionPool* transactionPool = MgServerFeatureTransactionPool::GetInstance();
    CHECKNULL(transactionPool, L"MgServerFeatureService.ReleaseSavePoint");

    bResult = transactionPool->ReleaseSavePoint(transactionId, savePointName);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerFeatureService.ReleaseSavePoint")

    return bResult;
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the FDO cache information for the server
/// </summary>
/// <returns>
/// The FDO cache information
/// </returns>
STRING MgServerFeatureService::GetFdoCacheInfo()
{
    MG_LOG_TRACE_ENTRY(L"MgServerFeatureService::GetFdoCacheInfo()");

    STRING info = L"";

    MgFdoConnectionManager* fdoConnMgr = MgFdoConnectionManager::GetInstance();
    if(fdoConnMgr)
    {
        info = fdoConnMgr->GetFdoCacheInfo();
    }

    return info;
}


///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Handle the Resource Change Notification event.
/// Remove the cached FDO connections and Feature Service cache entries
/// associated with the changed feature sources.
///
bool MgServerFeatureService::NotifyResourcesChanged(const set<STRING>& resources, bool strict)
{
    bool success = true;

    if (!resources.empty())
    {
        MgCacheManager* cacheManager = MgCacheManager::GetInstance();

        for (set<STRING>::const_iterator i = resources.begin();
            i != resources.end(); ++i)
        {
            MG_FEATURE_SERVICE_TRY()

            cacheManager->NotifyResourceChanged(*i);

            MG_FEATURE_SERVICE_CATCH(L"MgServerFeatureService.NotifyResourcesChanged")

            if (NULL != mgException.p)
            {
                success = false;

                if (strict)
                {
                    MG_FEATURE_SERVICE_THROW();
                }
                else
                {
                    MgLogManager* logManager = MgLogManager::GetInstance();
                    ACE_ASSERT(NULL != logManager);
                    logManager->LogSystemErrorEntry(mgException.p);
                }
            }
        }
    }

    return success;
}


///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Handle the Resource Change Notification event.
/// Remove the cached FDO connections and Feature Service cache entries
/// associated with the changed feature sources.
///
bool MgServerFeatureService::NotifyResourcesChanged(MgSerializableCollection* resources, bool strict)
{
    bool success = true;

    if (NULL != resources)
    {
        INT32 numResources = resources->GetCount();

        if (numResources > 0)
        {
            MgCacheManager* cacheManager = MgCacheManager::GetInstance();

            for (INT32 i = 0; i < numResources; ++i)
            {
                Ptr<MgSerializable> serializableObj = resources->GetItem(i);
                MgResourceIdentifier* resource =
                    dynamic_cast<MgResourceIdentifier*>(serializableObj.p);
                ACE_ASSERT(NULL != resource);

                MG_FEATURE_SERVICE_TRY()

                cacheManager->NotifyResourceChanged(resource);

                MG_FEATURE_SERVICE_CATCH(L"MgServerFeatureService.NotifyResourcesChanged")

                if (NULL != mgException.p)
                {
                    success = false;

                    if (strict)
                    {
                        MG_FEATURE_SERVICE_THROW();
                    }
                    else
                    {
                        MgLogManager* logManager = MgLogManager::GetInstance();
                        ACE_ASSERT(NULL != logManager);
                        logManager->LogSystemErrorEntry(mgException.p);
                    }
                }
            }
        }
    }

    return success;
}

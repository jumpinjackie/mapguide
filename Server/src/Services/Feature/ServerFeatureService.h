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

#ifndef _MG_SERVER_FEATURE_SERVICE_H_
#define _MG_SERVER_FEATURE_SERVICE_H_

#include "ServerFeatureDllExport.h"
#include "Services/FeatureService.h"

#include <set>

class MG_SERVER_FEATURE_API MgServerFeatureService : public MgFeatureService
{
    DECLARE_CLASSNAME(MgServerFeatureService)

public:

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Construct an MgServerFeatureService object
    /// </summary>
    MgServerFeatureService();

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Destructor
    /// </summary>
    ~MgServerFeatureService();

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
    MgByteReader* GetFeatureProviders();

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
    /// UserName=test;Password=test;IPAddress=144.111.1.1
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
    /// UserName=test;Password=test;IPAddress=144.111.1.1;ServiceName=Canada
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
    /// UserName=test;Password=test;IPAddress=144.111.1.1;ServiceName=Canada;DatabaseName=Alberta
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
    MgStringCollection* GetConnectionPropertyValues( CREFSTRING providerName,
                                                     CREFSTRING propertyName,
                                                     CREFSTRING partialConnString );

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
    bool TestConnection(CREFSTRING providerName,CREFSTRING connectionString);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Attempts to connect to the Feature Provider given by the connection string
    /// </summary>
    /// <param name="resource">
    /// Resource identifier referring to feature source
    /// </param>
    /// <returns>
    /// True if connection was successful
    /// </returns>
    bool TestConnection(MgResourceIdentifier* resource);

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
    /// <param name="providerName">Input
    /// Name of provider for which capabilities are being requested
    /// </param>
    /// <returns>
    /// Byte array representing XML (or NULL)
    /// </returns>
    ///
    /// EXCEPTIONS:
    /// MgInvalidProviderNameException
    MgByteReader* GetCapabilities(CREFSTRING providerName);

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
    /// <param name="providerName">Input
    /// Name of provider for which capabilities are being requested
    /// </param>
    /// <returns>
    /// <param name="connectionString">Input
    /// The connection string to use
    /// </param>
    /// Byte array representing XML (or NULL)
    /// </returns>
    ///
    /// EXCEPTIONS:
    /// MgInvalidProviderNameException
    MgByteReader* GetCapabilities(CREFSTRING providerName, CREFSTRING connectionString);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// This method returns list of ALL schemas names available with
    /// with the provider
    /// </summary>
    /// <param name="resource">Input
    /// A resource identifier referring to connection string
    /// </param>
    /// <returns>
    /// StringCollection listing all schema names
    /// </returns>
    ///
    /// EXCEPTIONS:
    /// MgInvalidResourceIdentifer
    MgStringCollection* GetSchemas( MgResourceIdentifier* resource );

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// This method returns list of ALL class names available within
    /// a specified schema
    /// </summary>
    /// <param name="resource">Input
    /// A resource identifier referring to connection string
    /// </param>
    /// <param name="schemaName">Input
    /// A schema name or NULL to retrieve all available classes
    /// </param>
    /// <returns>
    /// StringCollection listing all class names
    /// </returns>
    ///
    /// EXCEPTIONS:
    /// MgInvalidResourceIdentifer
    MgStringCollection* GetClasses( MgResourceIdentifier* resource, CREFSTRING schemaName );

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// This method returns class definition for the specified class
    /// </summary>
    /// <param name="resource">Input
    /// A resource identifier referring to connection string
    /// </param>
    /// <param name="schemaName">Input
    /// A schema name
    /// </param>
    /// <param name="className">Input
    /// A class name
    /// </param>
    /// <returns>
    /// MgClassDefinition instance for the mentioned classname
    /// </returns>
    ///
    /// EXCEPTIONS:
    /// MgInvalidResourceIdentifer
    MgClassDefinition* GetClassDefinition(  MgResourceIdentifier* resource,
                                            CREFSTRING schemaName,
                                            CREFSTRING className);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// This method returns class definition for the specified class
    /// </summary>
    /// <param name="resource">Input
    /// A resource identifier referring to connection string
    /// </param>
    /// <param name="schemaName">Input
    /// A schema name
    /// </param>
    /// <param name="className">Input
    /// A class name
    /// </param>
    /// <returns>
    /// MgClassDefinition instance for the mentioned classname
    /// </returns>
    ///
    /// EXCEPTIONS:
    /// MgInvalidResourceIdentifer
    MgClassDefinition* GetClassDefinition(  MgResourceIdentifier* resource,
                                            CREFSTRING schemaName,
                                            CREFSTRING className,
                                            bool serialize);

    /////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates or updates a feature schema within specified feature source.
    ///
    void ApplySchema(MgResourceIdentifier* resource, MgFeatureSchema* schema);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the definitions of one or more schemas contained in the
    /// feature source for particular classes. If the specified schema name or
    /// a class name does not exist, this method will throw an exception.
    ///
    MgFeatureSchemaCollection* DescribeSchema(MgResourceIdentifier* resource,
        CREFSTRING schemaName, MgStringCollection* classNames);

    ///////////////////////////////////////////////////////////////////////////
    /// This method has been deprecated. Use the above method.
    ///
    MgFeatureSchemaCollection* DescribeSchema(MgResourceIdentifier* resource,
        CREFSTRING schemaName);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the definitions in XML format of one or more schemas contained in the
    /// feature source for particular classes. If the specified schema name or
    /// a class name does not exist, this method will throw an exception.
    ///
    STRING DescribeSchemaAsXml(MgResourceIdentifier* resource,
        CREFSTRING schemaName, MgStringCollection* classNames);

    ///////////////////////////////////////////////////////////////////////////
    /// This method has been deprecated. Use the above method.
    ///
    STRING DescribeSchemaAsXml(MgResourceIdentifier* resource,
        CREFSTRING schemaName);

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
    MgFeatureReader*  SelectFeatures(   MgResourceIdentifier* resource,
                                        CREFSTRING className,
                                        MgFeatureQueryOptions* options );

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
    MgFeatureReader*  SelectFeatures(   MgResourceIdentifier* resource,
                                        CREFSTRING className,
                                        MgFeatureQueryOptions* options,
                                        CREFSTRING coordinateSystem );

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
    MgDataReader*  SelectAggregate( MgResourceIdentifier* resource,
                                    CREFSTRING className,
                                    MgFeatureAggregateOptions* options );

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
    MgPropertyCollection* UpdateFeatures( MgResourceIdentifier* resource,
                                          MgFeatureCommandCollection* commands,
                                          bool useTransaction );

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
    MgPropertyCollection* UpdateFeatures( MgResourceIdentifier* resource,
                                          MgFeatureCommandCollection* commands,
                                          MgTransaction* transaction );

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
    MgFeatureReader* GetLockedFeatures( MgResourceIdentifier* resource,
                                        CREFSTRING className,
                                        MgFeatureQueryOptions* options );

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
    MgTransaction* BeginTransaction( MgResourceIdentifier* resource );

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
    MgSqlDataReader* ExecuteSqlQuery( MgResourceIdentifier* resource,
                                      CREFSTRING sqlStatement );

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// This method executes the SELECT SQL statement specified and returns a pointer to
    /// SqlDataReader instance within the given transaction. This instance can be used to
    /// retrieve column information and related values.
    ///
    /// NOTE:
    /// Serialize() method of SqlDataReader would be able to convert data returned
    /// to AWKFF or XML stream.
    /// </summary>
    /// <remarks>
    /// If passing in NULL for transaction, it will work the same
    /// as before within no explicit transaction.
    /// </remarks>
    /// <param name="resource">Input
    /// A resource identifier referring to connection string
    /// </param>
    /// <param name="sqlStatement">Input
    /// This would allow users to specify free format SQL SELECT statement like
    /// SELECT * FROM CLASSNAME WHERE COLOR = RED. This would return all rows
    /// from "CLASSNAME" where COLOR column has value RED.
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
    MgSqlDataReader* ExecuteSqlQuery( MgResourceIdentifier* resource,
                                      CREFSTRING sqlStatement,
                                      MgParameterCollection* params,
                                      MgTransaction* transaction );

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// This method executes the SELECT SQL statement specified and returns a pointer to
    /// SqlDataReader instance within the given transaction. This instance can be used to
    /// retrieve column information and related values.
    ///
    /// NOTE:
    /// Serialize() method of SqlDataReader would be able to convert data returned
    /// to AWKFF or XML stream.
    /// </summary>
    /// <remarks>
    /// If passing in NULL for transaction, it will work the same
    /// as before within no explicit transaction.
    /// </remarks>
    /// <param name="resource">Input
    /// A resource identifier referring to connection string
    /// </param>
    /// <param name="sqlStatement">Input
    /// This would allow users to specify free format SQL SELECT statement like
    /// SELECT * FROM CLASSNAME WHERE COLOR = RED. This would return all rows
    /// from "CLASSNAME" where COLOR column has value RED.
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
    MgSqlDataReader* ExecuteSqlQuery( MgResourceIdentifier* resource,
                                      CREFSTRING sqlStatement,
                                      MgParameterCollection* params,
                                      MgTransaction* transaction,
                                      INT32 fetchSize );

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
    INT32 ExecuteSqlNonQuery( MgResourceIdentifier* resource,
                              CREFSTRING sqlNonSelectStatement );

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// This method executes all SQL statements supported by providers except SELECT
    /// within the given transaction.
    /// </summary>
    /// <remarks>
    /// If passing in NULL for transaction, it will work the same
    /// as before within no explicit transaction.
    /// </remarks>
    /// <param name="resource">Input
    /// A resource identifier referring to connection string
    /// </param>
    /// <param name="sqlNonSelectStatement">Input
    /// This would allow users to specify free format SQL statement like INSERT/UPDATE/DELETE/CREATE
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
    INT32 ExecuteSqlNonQuery( MgResourceIdentifier* resource,
                              CREFSTRING sqlNonSelectStatement,
                              MgParameterCollection* params,
                              MgTransaction* transaction );

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
    MgSpatialContextReader* GetSpatialContexts( MgResourceIdentifier* resource,
                                                bool bActiveOnly);

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
    MgLongTransactionReader* GetLongTransactions( MgResourceIdentifier* resource,
                                                  bool bActiveOnly);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Set the active long transaction name for a feature source.
    /// The long transaction name is associated with the caller's session. If
    /// no session is set then the method throws an MgSessionNotFoundException.
    /// </summary>
    /// <param name="featureSourceId">Input
    /// A resource identifier identifying a feature source in the repository.
    /// </param>
    /// <param name="longTransactionName">Input
    /// The long transaction name to set.
    /// </param>
    /// <returns>
    /// Returns true if the name was successfully set; otherwise
    /// returns false.
    /// </returns>
    ///
    /// EXCEPTIONS:
    /// MgNullArgumentException
    /// MgInvalidResourceTypeException
    /// MgSessionNotFoundException
    ///
    bool SetLongTransaction( MgResourceIdentifier* featureSourceId,
                             CREFSTRING longTransactionName);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Retrieves schema information about a set of feature classes for a given feature source.
    /// </summary>
    /// <param name="featureSourceId">Input
    /// The resource identifier defining the
    /// location of the feature source in
    /// the repository.
    /// </param>
    /// <param name="featureClasses">Input
    /// A collection of strings identifying the feature classes for which to
    /// retrieve schema information. If this collection is null or empty, information
    /// is returned for all feature classes.
    /// </param>
    /// <returns>
    /// Returns an MgByteReader containing the XML schema.
    /// </returns>
    ///
    MgByteReader* DescribeWfsFeatureType(MgResourceIdentifier* featureSourceId, MgStringCollection* featureClasses);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Retrieves schema information about a set of feature classes for a given feature source with specified output format.
    /// </summary>
    /// <param name="featureSourceId">Input
    /// The resource identifier defining the
    /// location of the feature source in
    /// the repository.
    /// </param>
    /// <param name="featureClasses">Input
    /// A collection of strings identifying the feature classes for which to
    /// retrieve schema information. If this collection is null or empty, information
    /// is returned for all feature classes.
    /// </param>
    /// <param name="outputFormat">Input
    /// A string identifying the output format of the retrieved schema information.
	/// The supported values of output format are specified in OpenGIS Web Feature Service (WFS) Implementation Specification - section 8.2
	/// http://portal.opengeospatial.org/files/?artifact_id=8339
    /// </param>
    /// <returns>
    /// Returns an MgByteReader containing the XML schema.
    /// </returns>
    ///
    MgByteReader* DescribeWfsFeatureType(MgResourceIdentifier* featureSourceId, MgStringCollection* featureClasses, CREFSTRING outputFormat);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Retrieves feature information based on the supplied criteria.
    /// </summary>
    /// <param name="featureSourceId">Input
    /// The resource identifier defining the
    /// location of the feature source in
    /// the repository.
    /// </param>
    /// <param name="featureClass">Input
    /// The feature class containing the features to retrieve.
    /// </param>
    /// <param name="requiredProperties">Input
    /// The collection of properties to retrieve for each feature. If the
    /// collection is null or empty, all properties will be retrieved.
    /// </param>
    /// <param name="srs">Input
    /// The spatial reference system in which to return feature geometries.
    /// </param>
    /// <param name="filter">Input
    /// An XML string containing the definition for an OGC filter.
    /// </param>
    /// <param name="maxFeatures">Input
    /// The maximum number of features to retrieve. If the value is less
    /// than or equal to zero, all features will be retrieved.
    /// </param>
    /// <returns>
    /// Returns an MgByteReader containing the requested feature information.
    /// </returns>
    ///
    MgByteReader* GetWfsFeature(MgResourceIdentifier* featureSourceId, CREFSTRING featureClass,
        MgStringCollection* requiredProperties, CREFSTRING srs, CREFSTRING filter, INT32 maxFeatures);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Retrieves feature information based on the supplied criteria with specified output format.
    /// </summary>
    /// <param name="featureSourceId">Input
    /// The resource identifier defining the
    /// location of the feature source in
    /// the repository.
    /// </param>
    /// <param name="featureClass">Input
    /// The feature class containing the features to retrieve.
    /// </param>
    /// <param name="requiredProperties">Input
    /// The collection of properties to retrieve for each feature. If the
    /// collection is null or empty, all properties will be retrieved.
    /// </param>
    /// <param name="srs">Input
    /// The spatial reference system in which to return feature geometries.
    /// </param>
    /// <param name="filter">Input
    /// An XML string containing the definition for an OGC filter.
    /// </param>
    /// <param name="maxFeatures">Input
    /// The maximum number of features to retrieve. If the value is less
    /// than or equal to zero, all features will be retrieved.
    /// </param>
    /// <param name="outputFormat">Input
    /// A string identifying the output format of the retrieved feature information.
    /// The supported values of output format are specified in OpenGIS Web Feature Service (WFS) Implementation Specification - section 9.2
    /// http://portal.opengeospatial.org/files/?artifact_id=8339
    /// </param>
    /// <returns>
    /// Returns an MgByteReader containing the requested feature information.
    /// </returns>
    ///
    MgByteReader* GetWfsFeature(MgResourceIdentifier* featureSourceId, CREFSTRING featureClass,
        MgStringCollection* requiredProperties, CREFSTRING srs, CREFSTRING filter, INT32 maxFeatures, CREFSTRING outputFormat);

    ////////////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// This method enumerates all the providers and if they are FDO enabled for
    /// the specified provider and partial connection string.
    /// </summary>
    /// <param name="providerName">Input
    /// The name of the Fdo feature provider.
    /// </param>
    /// <param name="partialConnString">Input
    /// The partial connection string to the Fdo provider.
    /// </param>
    /// <returns>
    /// Returns the list of data stores.
    /// </returns>
    ///
    MgByteReader* EnumerateDataStores(CREFSTRING providerName, CREFSTRING partialConnString);

    ////////////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// This method returns all of the logical to physical schema mappings for
    /// the specified provider and partial connection string.
    /// </summary>
    /// <param name="providerName">Input
    /// The name of the Fdo feature provider.
    /// </param>
    /// <param name="partialConnString">Input
    /// The partial connection string to the Fdo provider.
    /// </param>
    /// <returns>
    /// Returns the schema mapping.
    /// </returns>
    ///
    MgByteReader* GetSchemaMapping(CREFSTRING providerName, CREFSTRING partialConnString);

    ////////////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Create a feature source using the given feature source parameters and identified by the
    /// specified resource identifier.
    /// </summary>
    /// <param name="featureSourceId">Input
    /// The resource identifier defining the location of the new feature source in the repository.
    /// </param>
    /// <param name="sourceParams">Input
    /// The feature source parameters (feature schema and spatial context definitions).
    /// </param>
    /// <returns>
    /// Returns nothing.
    /// </returns>
    ///
    void CreateFeatureSource(MgResourceIdentifier* resource, MgFeatureSourceParams* sourceParams);

    // Feature
    MgBatchPropertyCollection* GetFeatures(CREFSTRING featureReader);
    bool CloseFeatureReader(CREFSTRING featureReader);

    // SQL
    MgBatchPropertyCollection* GetSqlRows(CREFSTRING sqlReader);
    bool CloseSqlReader(CREFSTRING sqlReader);

    // Data
    MgBatchPropertyCollection* GetDataRows(CREFSTRING dataReader);
    bool CloseDataReader(CREFSTRING dataReader);

    // Raster
    MgByteReader* GetRaster(CREFSTRING reader, INT32 xSize, INT32 ySize, STRING propName);


    STRING SchemaToXml(MgFeatureSchemaCollection* schema);
    MgFeatureSchemaCollection* XmlToSchema(CREFSTRING xml);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns a class definition collection containing identity properties for the specified classes.
    /// The class definition only contains the identity properties.
    /// If schemaName is empty, then class names must be fully qualified.
    virtual MgClassDefinitionCollection* GetIdentityProperties(MgResourceIdentifier* resource,
                                                               CREFSTRING schemaName,
                                                               MgStringCollection* classNames);

    virtual bool NotifyResourcesChanged(const set<STRING>& resources,
        bool strict = true);
    virtual bool NotifyResourcesChanged(MgSerializableCollection* resources,
        bool strict = true);

    STRING GetFdoCacheInfo();

    DECLARE_CREATE_SERVICE()

    void SetConnectionProperties(MgConnectionProperties* connProp);

    // Commit the transaction specified by the transaction id.
    bool CommitTransaction(CREFSTRING transactionId);

    // Rollback the transaction specified by the transaction id.
    bool RollbackTransaction(CREFSTRING transactionId);

private:

    unsigned StringHasher(FdoString* pszString);
    void FindClassDefinition(Ptr<MgFeatureSchemaCollection>& schemas,
        CREFSTRING schemaName, CREFSTRING className, REFSTRING schemaHash,
        Ptr<MgFeatureSchema>& schemaFound, Ptr<MgClassDefinition>& classFound);
    void FeatureSourceToString(MgResourceIdentifier* resource, string& resourceContent);
};

#endif

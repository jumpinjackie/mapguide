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

#ifndef _MG_PROXY_FEATURE_SERVICE_H
#define _MG_PROXY_FEATURE_SERVICE_H


class MgIntCollection;

/// \cond INTERNAL
/////////////////////////////////////////////////////////////////
/// \brief
/// The purpose of FeatureService is to provide access to all available FdoProviders
/// in a uniform way to MapGuide Server clients. Most of the required Fdo APIs are
/// wrapped into a smaller set of classes so that it can be utlized in client/server
/// environment.
class MG_MAPGUIDE_API MgProxyFeatureService : public MgFeatureService
{
    DECLARE_CLASSNAME(MgProxyFeatureService)

INTERNAL_API:

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// This method returns list of all providers, their connection properties
    /// and (default or enumerated) values for these properties, if any, in XML
    /// format.
    ///
    /// \remarks
    /// Schema Definition: FeatureProviders.xsd
    /// \n
    /// Sample XML:        FeatureProviders.xml
    ///
    /// \return
    /// Byte array representing XML (or error status)
    ///
    MgByteReader* GetFeatureProviders();

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// This method returns list of all values for the connection property
    /// specified based on partial connection string.
    ///
    /// \remarks
    /// To understand this, let
    /// us take an example:
    /// \n
    /// Oracle Provider for Fdo has 3 services running on each Machine A, Machine B
    /// and Machine C.
    /// \n
    /// <table>
    /// Machine     IP Address    ServiceName
    /// ---------   -----------   ----------------------
    /// Machine A   144.111.1.1   Canada, Europe, Russia
    /// Machine B   192.111.1.2   Japan, China, India
    /// Machine C   159.111.2.1   USA, Mexico, Australia
    /// </table>
    ///
    /// List of properties required for connection:
    /// <table border="1" class="RuledTable">
    ///    <tr>
    ///        <th>Propertyline</th>
    ///        <th>Enumerable</th>
    ///        <th>Values</th>
    ///    </tr>
    ///    <tr>
    ///        <td>Userline</td>
    ///        <td>No</td>
    ///        <td>&nbsp;</td>
    ///    </tr>
    ///    <tr>
    ///        <td>Password</td>
    ///        <td>No</td>
    ///        <td>&nbsp;</td>
    ///    </tr>
    ///    <tr>
    ///        <td>IPAddress</td>
    ///        <td>Yes</td>
    ///        <td>144.111.1.1<br>192.111.1.2<br>159.111.2.1</td>
    ///    </tr>
    ///    <tr>
    ///        <td>Serviceline</td>
    ///        <td>Yes</td>
    ///        <td>(This value is dependent on IPAddress and therefore value is unknown)</td>
    ///    </tr>
    ///    <tr>
    ///        <td>Databaseline</td>
    ///        <td>Yes</td>
    ///        <td>(This value is dependent on IPAddress+Serviceline and therefore value is unknown)</td>
    ///    </tr>
    /// </table>
    /// \n
    /// User builds partial connection string as
    /// UserName=test;Password=test;IPAddress=144.111.1.1
    /// \n
    /// Using this partial connection string user requests values for ServiceName property.
    /// \n
    /// Internally Fdo provider, if it has some dependency on a certain property, it needs
    /// to look into property dictionary and find out if a dependent property values are already
    /// set. In this case it is an IPAddress only but there can be more than one property.
    /// \n
    /// If all dependent values are set, it will return the list of values for the dependent
    /// property values. The result for the above connectionString would be
    /// <ul>
    ///    <li>Canada,</li>
    ///    <li>Europe,</li>
    ///    <li>Russia</li>
    /// </ul>
    /// In case of SQLServer, we might have more than one database per service. Therefore
    /// once the list of services are retrieved, user builds another connectionString as
    /// \n
    /// UserName=test;Password=test;IPAddress=144.111.1.1;ServiceName=Canada
    /// \n
    /// and requests property values for "DatabaseName". This would result in list of
    /// databases available to the users e.g.
    /// <ul>
    ///    <li>Alberta</li>
    ///    <li>British Columbia</li>
    ///    <li>Manitoba</li>
    /// </ul>
    /// The final connectionString for Open would be
    /// \n
    /// UserName=test;Password=test;IPAddress=144.111.1.1;ServiceName=Canada;DatabaseName=Alberta
    ///
    /// \param propertyName
    /// Input
    /// Name of property for which values need to be retrieved. (Can not be NULL)
    /// \param partialConnString
    /// Input
    /// Partial connection string on which property request is dependent.
    /// NULL value indicates no dependency.
    ///
    /// \return
    /// String Collection or NULL if nothing is found
    ///
    MgStringCollection* GetConnectionPropertyValues( CREFSTRING providerName,
                                                     CREFSTRING propertyName,
                                                     CREFSTRING partialConnString );

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Attempts to connect to the Feature Provider given by the connection string
    ///
    /// \param connectionString
    /// Connection string for Feature Provider
    ///
    /// \return
    /// True if connection was successful
    ///
    bool TestConnection(CREFSTRING providerName,CREFSTRING connectionString);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Attempts to connect to the Feature Provider given by the connection string
    ///
    /// \param resource
    /// Resource identifier referring to feature source
    ///
    /// \return
    /// True if connection was successful
    ///
    bool TestConnection(MgResourceIdentifier* resource);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// This method returns capabilities of the provider applicable for the
    /// connection.
    ///
    /// \remarks
    /// This would provide details on the following
    /// capabilities:
    /// <ol>
    /// <li>Connection</li>
    /// <li>Schema</li>
    /// <li>Command</li>
    /// <li>Filter</li>
    /// <li>Expression</li>
    /// </ol>
    /// \n
    /// Schema Definition: FeatureProviderCapabilities.xsd
    /// Sample XML:        FeatureProviderCapabilities.xml
    ///
    /// \param providerName
    /// Input
    /// Name of provider for which capabilities are requested
    ///
    /// \return
    /// Byte array representing XML (or NULL)
    ///
    /// \exception MgInvalidProviderNameException
    ///
    MgByteReader* GetCapabilities(CREFSTRING providerName);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// This method returns list of ALL schemas names available with
    /// with the provider
    ///
    /// \param resource
    /// Input
    /// A resource identifier referring to connection string
    ///
    /// \return
    /// StringCollection listing all schema names
    ///
    /// \exception MgInvalidResourceIdentifier
    ///
    MgStringCollection* GetSchemas( MgResourceIdentifier* resource );

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// This method returns list of ALL class names available within
    /// a specified schema
    ///
    /// \param resource
    /// Input
    /// A resource identifier referring to connection string
    /// \param schemaName
    /// Input
    /// A schema name or NULL to retrieve all available classes
    ///
    /// \return
    /// StringCollection listing all class names
    ///
    /// \exception MgInvalidResourceIdentifier
    ///
    MgStringCollection* GetClasses( MgResourceIdentifier* resource, CREFSTRING schemaName );

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// This method returns class definition for the specified class
    ///
    /// \param resource
    /// Input
    /// A resource identifier referring to connection string
    /// \param schemaName
    /// Input
    /// A schema name
    /// \param className
    /// Input
    /// A class name
    ///
    /// \return
    /// MgClassDefinition instance for the mentioned classname
    ///
    /// \exception MgInvalidResourceIdentifier
    ///
    MgClassDefinition* GetClassDefinition(  MgResourceIdentifier* resource,
                                            CREFSTRING schemaName,
                                            CREFSTRING className);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the definitions of one or more schemas contained in the
    /// feature source for particular classes. If the specified schema name or
    /// a class name does not exist, this method will throw an exception.
    /// See \link Feature_Schema_Module Feature Schema \endlink.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgFeatureSchemaCollection DescribeSchema(MgResourceIdentifier resource, string schemaName, MgStringCollection classNames);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgFeatureSchemaCollection DescribeSchema(MgResourceIdentifier resource, String schemaName, MgStringCollection classNames);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgFeatureSchemaCollection DescribeSchema(MgResourceIdentifier resource, string schemaName, MgStringCollection classNames);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param resource (MgResourceIdentifier)
    /// A resource identifier for the feature source.
    /// \param schemaName (String/string)
    /// The name of the schema definition to retrieve. If this string is empty,
    /// execution of the command will describe requested classes using the
    /// qualified class name. If the class name is not qualified, the requested
    /// class from all schemas will be described.
    /// \param classNames (MgStringCollection)
    /// A collection of strings identifying the feature classes for which to
    /// retrieve schema information. If this collection is null or empty,
    /// execution of the command will describe all classes.
    /// If the class name is not qualified, and the schema name is not specified,
    /// the requested class from all schemas will be described.
    /// The class names specified serve only as a hint. Use of the hint
    /// during command execution is provider dependent. Providers that
    /// will not use the hint will describe the schema for all classes.
    ///
    /// \return
    /// Returns the schema collection representing the schema containing
    /// the class collection for the specified classes.
    ///
    /// \exception MgFeatureServiceException
    /// \exception MgInvalidArgumentException
    /// \exception MgFdoException
    ///
    /// \note
    /// Please refer to Overview section of this document or Fdo
    /// provider documents for details on connection properties,
    /// schema and classes.
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
    /// See \link Feature_Schema_Module Feature Schema \endlink.
    ///
    /// \remarks
    /// The XML representation of the schema definitions conforms to
    /// FDO XML schema, which are based on OGC GML schema. How to
    /// specify a schema definition in XML is discussed in the
    /// Autodesk FDO API Developer's Guide.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual string DescribeSchemaAsXml(MgResourceIdentifier resource, string schemaName, MgStringCollection classNames);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual String DescribeSchemaAsXml(MgResourceIdentifier resource, String schemaName, MgStringCollection classNames);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual string DescribeSchemaAsXml(MgResourceIdentifier resource, string schemaName, MgStringCollection classNames);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param resource (MgResourceIdentifier)
    /// A resource identifier for the feature source.
    /// \param schemaName (String/string)
    /// The name of the schema definition to retrieve. If this string is empty,
    /// execution of the command will describe requested classes using the
    /// qualified class name. If the class name is not qualified, the requested
    /// class from all schemas will be described.
    /// \param classNames (MgStringCollection)
    /// A collection of strings identifying the feature classes for which to
    /// retrieve schema information. If this collection is null or empty,
    /// execution of the command will describe all classes.
    /// If the class name is not qualified, and the schema name is not specified,
    /// the requested class from all schemas will be described.
    /// The class names specified serve only as a hint. Use of the hint
    /// during command execution is provider dependent. Providers that
    /// will not use the hint will describe the schema for all classes.
    ///
    /// \return
    /// Returns a string containing the FDO schema in XML format.
    ///
    /// \exception MgFeatureServiceException
    /// \exception MgInvalidArgumentException
    /// \exception MgFdoException
    ///
    STRING DescribeSchemaAsXml(MgResourceIdentifier* resource,
        CREFSTRING schemaName, MgStringCollection* classNames);

    ///////////////////////////////////////////////////////////////////////////
    /// This method has been deprecated. Use the above method.
    ///
    STRING DescribeSchemaAsXml(MgResourceIdentifier* resource,
        CREFSTRING schemaName);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// This method returns the Fdo schema information in XML format
    /// for the MgFeatureSchemaCollection object provided
    ///
    /// \param schema
    /// Input
    /// A resource identifier referring to connection string
    ///
    /// \return
    /// STRING representing XML
    ///
    /// \exception MgNullArgumentException
    ///
    STRING SchemaToXml(MgFeatureSchemaCollection* schema);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// This method creates MgFeatureSchemaCollection using
    /// XML schema provided
    ///
    /// \param xml
    /// Input
    /// A string containing xml
    ///
    /// \return
    /// MgFeatureSchemaCollection
    ///
    /// \exception MgNullArgumentException
    ///
    MgFeatureSchemaCollection* XmlToSchema(CREFSTRING xml);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Select features returns feature information from an Fdo Provider.  An MgFeatureReader
    /// object provides one way, read-only iteration over the features.  The various properties
    /// of a feature class are accessed by name using methods of the MgFeatureReader.
    /// \n
    /// The desired feature class is specified by name. A subset of the feature properties
    /// can be specified using the properties string collection.  Futhermore, an Fdo filter text
    /// can be applied to limit the result set.
    ///
    /// \param resource
    /// Input
    /// A resource identifier referring to feature source
    /// \param className
    /// Input
    /// A valid class name on which select operation needs to be executed. It should not be NULL.
    /// \param options
    /// Input
    /// MgFeatureQueryOptions instance containing all required filters for this select operation
    ///
    /// \return
    /// MgFeatureReader which operates on the instance of actual reader returned from the
    /// FdoProvider.
    ///
    /// \exception MgFeatureServiceException
    /// \exception MgInvalidArgumentException
    /// \exception MgFdoException
    ///
    /// \note
    /// 1. Please refer to Overview section of this document or Fdo provider documents
    /// for details on connection properties, schema classes, class properties and
    /// filter text.
    /// \n
    /// 2. Interrogation of class definition would allow to determine properties of classes
    /// which can be used for filter text.
    ///
    MgFeatureReader*  SelectFeatures(   MgResourceIdentifier* resource,
                                        CREFSTRING className,
                                        MgFeatureQueryOptions* options );

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Selects features from a feature source according to the
    /// criteria set in the MgFeatureQueryOptions argument The
    /// criteria are applied to all of the features in the feature
    /// source. Use the coordinateSystem argument to set the target
    /// coordinate system if you want to transform.
    /// If you want to apply the criteria to a subset of the
    /// features, use the \link MgFeatureService::SelectAggregate MgFeatureService::SelectAggregate Method \endlink.
    /// See \link FiltersAndExpressions Filters and expressions \endlink.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgFeatureReader SelectFeatures(MgResourceIdentifier resource, string className, MgFeatureQueryOptions options, string coordinateSystem);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgFeatureReader SelectFeatures(MgResourceIdentifier resource, String className, MgFeatureQueryOptions options, String coordinateSystem);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgFeatureReader SelectFeatures(MgResourceIdentifier resource, string className, MgFeatureQueryOptions options, string coordinateSystem);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param resource (MgResourceIdentifier)
    /// A resource identifier for the feature
    /// source.
    /// \param className (String/string)
    /// The name of the feature class from which
    /// the properties of interest are selected.
    /// \param options (MgFeatureQueryOptions)
    /// MgFeatureQueryOptions instance
    /// containing all required filters for this
    /// select operation.
    /// \param coordinateSystem (String/string)
    /// The name of the coordinate system to transform to.
    ///
    /// \return
    /// Returns an MgFeatureReader containing the set of selected
    /// features.
    ///
    /// \exception MgFeatureServiceException
    /// \exception MgInvalidArgumentException
    /// \exception MgFdoException
    ///
    virtual MgFeatureReader*  SelectFeatures(   MgResourceIdentifier* resource,
                                                CREFSTRING className,
                                                MgFeatureQueryOptions* options,
                                                CREFSTRING coordinateSystem );

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// This operation allows user to select features and then combine them into a group (means aggregate them).
    /// Once the features are grouped together into a single set of data, they do not represent features, in other
    /// words, they do not corresponds to underlying database schema. Therefore, this operation returns MgDataReader.
    /// Computed properties applied in SelectFeatures would only be applicable per feature. For example, a polygon feature
    /// can compute its area while retriveal. The area would be computed property for a specific feature instance.
    /// In SelectAggregate operation, user might want to group all area for a particular zone and sum them to
    /// find total area based on certain criteria. The total area of a particular zone does not represent a feature instance (row).
    /// \n
    /// To find about aggregate functionalities available, please refer to GetCapabilities document returned by GetCapabilities
    /// operation.
    ///
    /// \param resource
    /// Input
    /// A resource identifier referring to feature source
    /// \param className
    /// Input
    /// A valid class name on which select operation needs to be executed. It should not be NULL.
    /// \param options
    /// Input
    /// MgFeatureAggregateOptions instance containing all required filters for this select operation
    ///
    /// \return
    /// MgDataReader pointer which operates on the instance of actual reader returned from the
    /// FdoProvider ( OR NULL ).
    ///
    /// \exception MgFeatureServiceException
    /// \exception MgInvalidArgumentException
    /// \exception MgFdoException
    ///
    /// \note
    /// 1. Please refer to Overview section of this document or Fdo provider documents
    /// for details on connection properties, schema, classes, class properties,
    /// filter text and spatial operations.
    /// \n
    /// 2. Please refer to Geometry API documentation for Geometry details.
    /// \n
    /// 3. Interrogation of provider capabilities will inform list of operations supported
    /// \n
    /// 4. Interrogation of class definition would allow to determine properties of classes
    /// which can be used for filter text.
    ///
    MgDataReader*  SelectAggregate( MgResourceIdentifier* resource,
                                    CREFSTRING className,
                                    MgFeatureAggregateOptions* options );

    /////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// It executes all the commands specified in command collection.
    ///
    /// \param resource
    /// Input
    /// A resource identifier referring to connection string
    /// \param commands
    /// Input
    /// Collection of commands to be executed
    /// \param useTransaction
    /// Input
    /// true  -  all commands will be executed in transaction
    /// false -  not to use transaction
    ///
    /// \return
    /// Integer collection referring to result for each command
    /// Index of commandCollection and index of IntCollection would match the result.
    ///
    /// \exception MgInvalidResourceIdentifier
    ///
    MgPropertyCollection* UpdateFeatures( MgResourceIdentifier* resource,
                                     MgFeatureCommandCollection* commands,
                                     bool useTransaction );

    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the locked features
    ///
    /// \remarks
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgFeatureReader GetLockedFeatures(MgResourceIdentifier resource, string className, MgFeatureAggregateOptions options);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgFeatureReader GetLockedFeatures(MgResourceIdentifier resource, String className, MgFeatureAggregateOptions options);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgFeatureReader GetLockedFeatures(MgResourceIdentifier resource, string className, MgFeatureAggregateOptions options);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param resource (MgResourceIdentifier)
    /// A resource identifier for the feature
    /// source.
    /// \param className (String/string)
    /// The name of the feature class on which
    /// the select operation is performed.
    /// \param options (MgFeatureAggregateOptions)
    /// An MgFeatureAggregateOptions instance
    /// containing all the criteria and filters
    /// required for this select operation.
    ///
    /// \return
    /// Returns an MgFeatureReader containing the locked features.
    ///
    /// \exception MgFeatureServiceException
    /// \exception MgInvalidArgumentException
    /// \exception ...
    ///
    virtual MgFeatureReader* GetLockedFeatures( MgResourceIdentifier* resource,
                                                CREFSTRING className,
                                                MgFeatureQueryOptions* options );

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// This method executes the SELECT SQL statement specified and returns a pointer to
    /// SqlDataReader instance. This instance can be used to retrieve column information
    /// and related values.
    ///
    /// \note
    /// Serialize() method of SqlDataReader would be able to convert data returned
    /// to AWKFF or XML stream.
    ///
    /// \param resource
    /// Input
    /// A resource identifier referring to connection string
    /// \param sqlStatement
    /// Input
    /// This would allow users to specify free format SQL SELECT statement like
    /// SELECT * FROM CLASSNAME WHERE COLOR = RED. This would return all rows
    /// from "CLASSNAME" where COLOR column has value RED.
    ///
    /// \return
    /// SqlDataReader pointer, an instance of reader pointing to the actual reader
    /// from FdoProvider (or NULL).
    /// If any statement other than SELECT is passed to this method, it would return failure.
    ///
    /// \exception MgInvalidResourceIdentifier
    /// \exception MgInvalidSqlStatement
    /// \exception MgSqlNotSupported
    ///
    MgSqlDataReader* ExecuteSqlQuery( MgResourceIdentifier* resource,
                                      CREFSTRING sqlStatement );

    /////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// This method executes all SQL statements supported by providers except SELECT.
    ///
    /// \param resource
    /// Input
    /// A resource identifier referring to connection string
    /// \param sqlNonSelectStatement
    /// Input
    /// This would allow users to specify free format SQL statement like INSERT/UPDATE/DELETE/CREATE
    ///
    /// \return
    /// An positive integer value indicating how many instances (rows) have been affected.
    ///
    /// \note
    /// It is possible that only few rows were affected than actually expected.
    /// In this scenario, warning object will contain the details on the failed records.
    ///
    /// \exception MgInvalidResourceIdentifier
    ///
    INT32 ExecuteSqlNonQuery( MgResourceIdentifier* resource,
                              CREFSTRING sqlNonSelectStatement );

    /////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// This method executes the SELECT statement to return all available spatial context
    /// for the provider. The information for spatial contexts will be retrieved via
    /// SpatialContextReader instance returned.
    ///
    /// \note
    /// Serialize() method of SpatialContextReader would be able to convert data returned from
    /// SpatialContextReader to binary stream for transmission or XML.
    ///
    /// \param resource
    /// Input
    /// A resource identifier referring to connection string
    /// \param bActiveOnly
    /// Input
    /// This flag is obsolete and no longer used.
    ///
    /// \return
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
    ///
    /// \exception MgInvalidResourceIdentifier
    ///
    /// \note
    /// Subject to change in FDO R2
    ///
    MgSpatialContextReader* GetSpatialContexts( MgResourceIdentifier* resource,
                                                bool bActiveOnly);

    /////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// This method executes the SELECT statement to return all available Long transactions
    /// for the provider. The information for long transactions will be retrieved via
    /// MgLongTransactionReader instance returned.
    ///
    /// \note
    /// Serialize() method of SpatialContextReader would be able to convert data returned from
    /// SpatialContextReader to XML.
    ///
    /// \param resource
    /// Input
    /// A resource identifier referring to connection string
    /// \param bActiveOnly
    /// Input
    /// This flag would decide whether it should return all or active long transaction.
    /// True - Only active spatial context
    /// False - All spatial contexts
    ///
    /// \return
    /// MgLongTransactionReader pointer, a pointer to the actual instance reader returned from the
    /// FdoProvider (or NULL).
    ///
    /// This reader can be utilized to retrieve the following information
    /// Name -             Spatial Context name
    /// Description -      Brief information about spatial context
    /// Owner       -      Owner of this long transaction
    /// IsActive           Boolean
    /// IsFrozen    -
    ///
    /// \exception MgInvalidResourceIdentifier
    ///
    /// \note
    /// Subject to change in FDO R2
    ///
    MgLongTransactionReader* GetLongTransactions( MgResourceIdentifier* resource,
                                                  bool bActiveOnly);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Set the active long transaction name for a feature source.
    ///
    /// \remarks
    /// The long transaction name is associated with the caller's session.  If
    /// no session is set then the method throws an MgSessionNotFoundException.
    ///
    /// \param featureSourceId (MgResourceIdentifier)
    /// A resource identifier identifying a feature source in the repository.
    /// \param longTransactionName (String/string)
    /// The long transaction name to set.
    ///
    /// \return
    /// Returns true if the name was successfully set; otherwise
    /// returns false.
    ///
    /// \exception MgNullArgumentException
    /// \exception MgInvalidResourceTypeException
    /// \exception MgSessionNotFoundException
    ///
    bool SetLongTransaction( MgResourceIdentifier* featureSourceId,
                             CREFSTRING longTransactionName);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Create a feature source using the given feature source parameters and identified by the
    /// specified resource identifier.
    ///
    void CreateFeatureSource(MgResourceIdentifier* resource, MgFeatureSourceParams* sourceParams);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
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
    MgByteReader* DescribeWfsFeatureType(MgResourceIdentifier* featureSourceId, MgStringCollection* featureClasses);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
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
    MgByteReader* GetWfsFeature(MgResourceIdentifier* featureSourceId, CREFSTRING featureClass,
        MgStringCollection* requiredProperties, CREFSTRING srs, CREFSTRING filter, INT32 maxFeatures);

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
    MgByteReader* EnumerateDataStores(CREFSTRING providerName, CREFSTRING partialConnString);

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
    MgByteReader* GetSchemaMapping(CREFSTRING providerName, CREFSTRING partialConnString);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct an MgProxyFeatureService object.
    ///
    ///
    MgProxyFeatureService();

    DECLARE_CREATE_SERVICE()

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the connection properties for the Proxy Service.  This
    /// information tells the proxy object where to connect.
    ///
    /// \param connProp
    /// Connection properties for server
    ///
    void SetConnectionProperties(MgConnectionProperties* connProp);

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

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns a class definition collection containing identity properties for the specified classes.
    /// The class definition only contains the identity properties.
    /// If schemaName is empty, then class names must be fully qualified.
    MgClassDefinitionCollection* GetIdentityProperties(MgResourceIdentifier* resource,
                                                       CREFSTRING schemaName,
                                                       MgStringCollection* classNames);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Get the FDO cache information.
    /// </summary>
    /// <returns>
    /// The FDO cache information.
    /// </returns>
    STRING GetFdoCacheInfo();

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// This method returns class definition for the specified class
    ///
    /// \param resource
    /// Input
    /// A resource identifier referring to connection string
    /// \param schemaName
    /// Input
    /// A schema name
    /// \param className
    /// Input
    /// A class name
    /// \param serialize
    /// Input
    /// Specifies whether the class definition is to be serialized
    ///
    /// \return
    /// MgClassDefinition instance for the mentioned classname
    ///
    /// \exception MgInvalidResourceIdentifier
    ///
    MgClassDefinition* GetClassDefinition(  MgResourceIdentifier* resource,
                                            CREFSTRING schemaName,
                                            CREFSTRING className,
                                            bool serialize);


protected:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Dispose this object
    ///
    /// \return
    /// Nothing
    ///
    virtual void Dispose();

private:

    void SetWarning(MgWarnings* warning);

    Ptr<MgConnectionProperties> m_connProp;

};
/// \endcond

#endif

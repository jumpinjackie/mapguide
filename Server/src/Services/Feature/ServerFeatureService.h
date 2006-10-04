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

#ifndef _MG_SERVER_FEATURE_SERVICE_H_
#define _MG_SERVER_FEATURE_SERVICE_H_

#include "ServerFeatureDllExport.h"
#include "Services/FeatureService.h"

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
    ~MgServerFeatureService(void);

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
    MgFeatureSchemaCollection* DescribeSchema( MgResourceIdentifier* resource,
                                  CREFSTRING schemaName );

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
    STRING DescribeSchemaAsXml( MgResourceIdentifier* resource,
                                  CREFSTRING schemaName );

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
    MgSpatialContextReader* GetSpatialContexts( MgResourceIdentifier* resource,
                                                bool bActiveOnly = false);

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
                                                  bool bActiveOnly = false);

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
    MgByteReader* DescribeWfsFeatureType(MgResourceIdentifier* featureSourceId, MgStringCollection* featureClasses);

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

    MgBatchPropertyCollection* GetFeatures(INT32 featureReaderId);
    bool CloseFeatureReader(INT32 featureReaderId);
    MgBatchPropertyCollection* GetSqlRows(INT32 sqlReader);
    bool CloseSqlReader(INT32 sqlReader);
    MgByteReader* GetRaster(INT32 featureReaderId, INT32 xSize, INT32 ySize, STRING propName);
    MgBatchPropertyCollection* GetDataRows(INT32 dataReader);
    bool CloseDataReader(INT32 dataReader);

    STRING SchemaToXml(MgFeatureSchemaCollection* schema);
    MgFeatureSchemaCollection* XmlToSchema(CREFSTRING xml);

    void CreateFeatureSource(MgResourceIdentifier* resource, MgFeatureSourceParams* sourceParams);

    //////////////////////////////////////////////////////////////////
    // Returns the collection of identity properties for the specified class.
    // If schemaName is empty, then className needs to be fully qualified.
    MgPropertyDefinitionCollection* GetIdentityProperties(MgResourceIdentifier* resource,
                                                          CREFSTRING schemaName,
                                                          CREFSTRING className);

    bool CloseGwsFeatureReader(INT32 gwsFeatureReader);

    // Feature Service cache methods
    virtual void UpdateFeatureServiceCache();
    virtual void RemoveFeatureServiceCacheEntries(MgSerializableCollection* changedResources);
    virtual void RemoveFeatureServiceCacheEntry(MgResourceIdentifier* resource);
    virtual void ClearFeatureServiceCache();

    DECLARE_CREATE_SERVICE()

    void SetConnectionProperties(MgConnectionProperties* connProp);

private:

    void FeatureSourceToString(MgResourceIdentifier* resource, string& resourceContent);
};

#endif

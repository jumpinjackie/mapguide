#ifndef GwsQueryEngine_h
#define GwsQueryEngine_h
// Copyright (C) 2004-2009 by Autodesk, Inc.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of version 2.1 of the GNU Lesser
// General Public License as published by the Free Software Foundation.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

#ifdef _WIN32
#ifdef GWSQUERYENGINE_EXPORTS
#define GWS_QUERYENGINE_API __declspec(dllexport)
#else
#define GWS_QUERYENGINE_API __declspec(dllimport)
#endif
#else
#define GWS_QUERYENGINE_API
#endif

#include <Fdo.h>
#include <GwsCommon.h>

#ifdef _WIN32
#pragma warning(disable: 4505) // ignore 'function' : unreferenced local function has been removed
#endif

// forward definitions
class IGWSFeatureQueryDefinition;
class IGWSFeatureIterator;
typedef IGWSFeatureIterator IGWSScrollableIterator;

/// <summary>
/// Types of supported query definitions
/// </summary>
typedef enum _EGwsQueryType
{
     eGwsQueryFeature
    ,eGwsQueryLeftOuterJoin
    ,eGwsQueryEqualJoin
} EGwsQueryType;

/// <summary>
/// This interface should be exposed by the Fdo connection pooler object.
/// Fdo Connection pool is used by the query engine to get connection by
/// name.
/// Name could be also a zero length string. In such case it is
/// up to the pooler implementaiton how to treat it.
/// </summary>
class IGWSConnectionPool : public IGWSObject
{
public:
    /// <summary>
    /// Given name, return Fdo connection.
    /// Must throw FdoException in case if name cannot be resolved.
    /// </summary>
    virtual FdoIConnection * GetConnection (FdoString * name) = 0;
};




/// <summary>
/// This interface represents query definition.
/// It is the base interface for all feature definitions.
///
/// </summary>
class IGWSQueryDefinition : public IGWSObject,
                            public FdoXmlSaxHandler

{
public:

    /// <summary>
    /// Creates query definition from the xml stream.
    /// </summary>
    /// <param name="elementName">Xml Element name for the query.</param>
    /// <param name="reader">Xml reader.</param>
    /// <returns>Returns nothing.</returns>
    GWS_QUERYENGINE_API
    static IGWSQueryDefinition * FromXmlReader (FdoString    * elementName,
                                                FdoXmlReader * reader);

    /// <summary>
    /// Creates empty query definition from the xml attributes collection.
    /// Returned pointer could be dynamicaly casted to the FdoXmlSaxHandler *.
    /// If this handler returned from the FdoXmlSaxHandler::XmlStartElement
    /// it will be initialized with the Gws Query
    /// </summary>
    /// <param name="attrs">Xml attributes.</param>
    /// <returns>Returns nothing.</returns>
    GWS_QUERYENGINE_API
    static IGWSQueryDefinition * FromXmlAttributes (
                                          FdoXmlAttributeCollection* attrs);


    /// <summary>
    /// Writes query definition to the Xml stream.
    /// </summary>
    /// <param name="elementName">Xml Element name for the query.</param>
    /// <param name="writer">Xml writer.</param>
    /// <returns>Returns nothing.</returns>
    virtual void                ToXmlWriter ( FdoString * elementName,
                                              FdoXmlWriter * writer) = 0;

    /// <summary>
    /// Returns type of the query.
    /// </summary>
    /// <returns>Returns nothing.</returns>
    virtual EGwsQueryType       Type () const = 0;

    /// <summary>
    /// Select list. Attribute names in result.
    /// </summary>
    /// <returns>Returns nothing.</returns>
    virtual FdoIdentifierCollection * SelectList () = 0;

    /// <summary>
    /// Returns class names referenced by the query definition
    /// </summary>
    /// <returns>Returns nothing.</returns>
    virtual IGWSQualifiedNames *QualifiedNames () = 0;

    /// <summary>
    /// Returns feature source names explicitly referenced in the query
    /// definition.
    /// The return collection may be empty, in case when
    /// all classes are referenced implicitly
    /// </summary>
    /// <returns>Returns nothing.</returns>
    virtual FdoStringCollection* FeatureSourceNames () = 0;


    /// <summary>
    /// Returns the filter of the query defintion.
    /// For the joined query definition this method returns the
    /// filter of the primary feature query
    /// (the left-hand feature query defintion filter)
    /// </summary>
    /// <returns>Returns nothing.</returns>
    virtual FdoFilter          * Filter    () = 0;


    /// <summary>
    /// Returns the primary feature query definition of the query definition.
    /// (the left-hand feature query defintion)
    /// </summary>
    /// <returns>Returns nothing.</returns>
    virtual IGWSFeatureQueryDefinition *
                                 GetPrimaryQueryDefinition () = 0;

    /// <summary>
    /// Returns order by columns
    /// </summary>
    /// <returns>Returns column collection.</returns>
    virtual FdoStringCollection* GetOrderBy () = 0;

    /// <summary>
    /// Sets order by columns
    /// </summary>
    /// <returns>Returns nothing.</returns>
    virtual void                 SetOrderBy (FdoStringCollection* orderByList) = 0;

    /// <summary>
    /// Gets whether the sort order is ascending or descending.
    /// </summary>
    /// <returns>Returns the fdo ordering option.</returns>
    virtual FdoOrderingOption    GetOrderingOption () = 0;

    /// <summary>
    /// Sets whether the sort order is ascending or descending.
    /// </summary>
    /// <returns>Returns nothing.</returns>
    virtual void                 SetOrderingOption (FdoOrderingOption orderingOption) = 0;
};

/// <summary>
/// This interface represents feature query definition .
/// </summary>
class IGWSFeatureQueryDefinition: public IGWSQueryDefinition

{
public:
    /// <summary>
    /// Constructs and return feature query definition
    /// </summary>
    /// <param name="selList">Input property name.</param>
    /// <param name="classname">Input qualified name.</param>
    /// <param name="propertyName">Input filter.</param>
    /// <returns>Returns the query definition.</returns>
    static GWS_QUERYENGINE_API IGWSFeatureQueryDefinition * Create (
                    FdoIdentifierCollection *    selList,
                    const GWSQualifiedName & classname,
                    FdoFilter              * filter);

    /// <summary>
    /// Returns constant reference to a class name
    /// </summary>
    /// <returns>Returns qualified name.</returns>
    virtual const GWSQualifiedName & ClassName () const= 0;
};


/// <summary>
/// This interface represents join query definition.
/// </summary>
class IGWSJoinQueryDefinition: public IGWSQueryDefinition
{
public:
    /// <summary>
    /// Left query definition
    /// </summary>
    /// <returns>Returns the query definition.</returns>
    virtual IGWSQueryDefinition * LeftQueryDefinition () = 0;

    /// <summary>
    /// Right query definition
    /// </summary>
    /// <returns>Returns the query definition.</returns>
    virtual IGWSQueryDefinition * RightQueryDefinition () = 0;

    /// <summary>
    /// Left join attributes
    /// </summary>
    /// <returns>Returns the query definition.</returns>
    virtual FdoStringCollection * LeftJoinAttributes () = 0;

    /// <summary>
    /// Right join attributes
    /// </summary>
    /// <returns>Returns the attribute collection.</returns>
    virtual FdoStringCollection * RightJoinAttributes () = 0;

    /// </summary>
    /// Returns the join name.
    /// </summary>
    /// <returns>Returns the join name.</returns>
    virtual const FdoString* JoinName() = 0;

    /// </summary>
    /// Returns the join delimiter.
    /// </summary>
    /// <returns>Returns the join delimiter.</returns>
    virtual const FdoString* JoinDelimiter() = 0;

    /// </summary>
    /// Returns the force one-to-one flag.
    /// </summary>
    /// <returns>Returns the force one-to-one flag.</returns>
    virtual bool ForceOneToOne() = 0;
};

/// <summary>
/// This interface represents left outer join query definition.
/// Left outer join is a join in which all instances from the left query are
//// preserved. However, only matching instances from the right query are
/// preserved. Any instances from the right query that do match are not joined
/// </summary>
class IGWSLeftJoinQueryDefinition: public IGWSJoinQueryDefinition
{
public:
    /// <summary>
    /// Constructs left join query definition
    /// <param name="left_qdef">Left query defintion.</param>
    /// <param name="right_qdef">Right query defintion.</param>
    /// <param name="left_attrs">Attributes from the left query definition
    /// </param>
    /// <param name="right_attrs">Attributes from the right query definition
    /// </param>
    /// </summary>
    static GWS_QUERYENGINE_API IGWSLeftJoinQueryDefinition * Create (
                    const FdoString     * joinName,
                    const FdoString     * joinDelimiter,
                    bool                  forceOneToOne,
                    IGWSQueryDefinition *    left_qdef,
                    IGWSQueryDefinition *    right_qdef,
                    FdoStringCollection    * left_attrs,
                    FdoStringCollection    * right_attrs);
};

/// <summary>
/// This interface represents equal inner join query definition.
/// Inner join is a join in which matching instances of two feature
/// classes are joined. If the value from the left attributes matches
/// the corresponding value in the right attribute, the instances are joined.
/// Any instances from the left or right feature queries that do not match
/// are not joined.
/// </summary>
class IGWSEqualJoinQueryDefinition: public IGWSJoinQueryDefinition
{
public:
    /// <summary>
    /// Constructs equal join query definition
    /// <param name="left_qdef">Left query defintion.</param>
    /// <param name="right_qdef">Right query defintion.</param>
    /// <param name="left_attrs">Attributes from the left query definition
    /// </param>
    /// <param name="right_attrs">Attributes from the right query definition
    /// </param>
    /// </summary>
    static GWS_QUERYENGINE_API IGWSEqualJoinQueryDefinition * Create (
                    const FdoString     * joinName,
                    const FdoString     * joinDelimiter,
                    bool                  forceOneToOne,
                    IGWSQueryDefinition *    left_qdef,
                    IGWSQueryDefinition *    right_qdef,
                    FdoStringCollection    * left_attrs,
                    FdoStringCollection    * right_attrs);
};


/// <summary>
/// This inerface represents query results descriptor.
/// It is designed to describe results of the joined query.
/// ClassName and ClassDefinition represent fully qualified name
/// and class definition of the primary query feature class.
///
/// </summary>
class IGWSExtendedFeatureDescription : public IGWSObject
{
public:
    /// <summary>
    /// fully qualified primary class name referenced in the query
    /// </summary>
    /// <returns>Returns qualified name.</returns>
    virtual const GWSQualifiedName  & ClassName () = 0;

    /// <summary>
    /// FDO Class Definition of the primary feature class.
    /// </summary>
    /// <returns>Returns qualified name.</returns>
    virtual FdoClassDefinition      * ClassDefinition () = 0;


    /// <summary>
    /// Get coordinate system of the geometric property
    /// </summary>
    /// <returns>GWSCoordinateSystem.</returns>
    virtual const GWSCoordinateSystem  & GetCoordinateSystem () = 0;

    /// <summary>
    /// Property names exposed by the query, these are the full
    /// set of the query. The order number of these properties
    /// can be used to get property values by
    /// int id. See IGWSFeature.
    /// </summary>
    /// <returns>Returns qualified name.</returns>
    virtual FdoStringCollection    *  PropertyNames () = 0;

    /// <summary>
    /// number of extended feature descriptions of the joined classes,
    /// there will be one for each right join part; 0 for none joined case
    /// </summary>
    /// <returns>Returns qualified name.</returns>
    virtual int                       GetCount        () = 0;

    /// <summary>
    /// Get joined feature description descriptor
    /// </summary>
    /// <param name="i">Input index of the description</param>
    /// <returns>Returns extended feature description.</returns>
    virtual IGWSExtendedFeatureDescription*  GetItem  (int i) = 0 ;


    /// <summary>
    /// Get extended feature description by name if it exists in the
    /// this feature description
    /// </summary>
    /// <param name="name">Input name for the feature.</param>
    /// <returns>Returns extended feature description.</returns>
    virtual IGWSExtendedFeatureDescription * GetItem  (const GWSQualifiedName & name) = 0;

    /// </summary>
    /// Returns the property index of the qualified property name. -1 if
    /// the propety name is not in the list.
    /// </summary>
    /// <param name="propertyName">Input property Name</param>
    /// <returns>0-based index of the property name</returns>
    virtual int                       Contains (FdoString* propertyName) = 0;

    /// </summary>
    /// Returns the property defintion by the property name. NULL if
    /// property was not found
    /// </summary>
    /// <param name="propertyName">Input property name to find.</param>
    /// <returns>Returns property definition of the property.</returns>
    virtual FdoPropertyDefinition  *  FindPropertyDefinition (FdoString* propertyName) = 0;

    /// </summary>
    /// Returns the join name. NULL not a join.
    /// </summary>
    /// <returns>Returns the join name.</returns>
    virtual const FdoString  *  JoinName () = 0;

    /// </summary>
    /// Returns the join delimiter. NULL not a join.
    /// </summary>
    /// <returns>Returns the join delimiter.</returns>
    virtual const FdoString  *  JoinDelimiter () = 0;

    /// </summary>
    /// Returns the force one to one flag. This only applies to joined data.
    /// </summary>
    /// <returns>Returns the force one to one flag.</returns>
    virtual bool ForceOneToOneJoin() = 0;
};



/// <summary>
/// This interface represents runtime query objects.  These functions throw
/// FdoException * on error.
///
/// </summary>
class IGWSQuery : public IGWSObject
{
public:
    /// <summary>
    /// Creates query by passing the connection pool manager and query
    /// definition.
    /// Connection pool is used by the query object to resolve connnection
    /// names referenced in the query definition when preparing and
    /// exeuting the query.
    /// </summary>
    /// <param name="pool">Connection pool manager.</param>
    /// <param name="qdef">Query defintion.</param>
    /// <param name="owner">Object that ownes the query. NULL if no owner.</param>
    /// <returns>Returns nothing.</returns>
    static GWS_QUERYENGINE_API IGWSQuery * Create (IGWSConnectionPool * pool,
                                       IGWSQueryDefinition * qdef,
                                       IGWSObject         * owner);

    /// <summary>
    /// Returns query defintion
    /// </summary>
    /// <param name="ppQdef">Output query definition.</param>
    /// <returns>Returns nothing.</returns>
    virtual void        GetQueryDefinition (IGWSQueryDefinition ** ppQdef) = 0;

    /// <summary>
    /// Describe the query results. A Feature description is available after
    /// query has been prepared
    /// </summary>
    /// <param name="ppResDesc">Output feature description for this feature
    /// type.</param>
    /// <returns>Returns nothing.</returns>
    virtual void        DescribeResults (IGWSExtendedFeatureDescription ** ppResDesc) = 0;

    /// <summary>
    /// Set filter for the query. The filter will override the filter from the
    /// query definition of the primary query (left hand query in the join
    /// definition).
    /// </summary>
    /// <param name="filter">Input filter for the query.</param>
    /// <returns>Returns nothing.</returns>
    virtual void        SetFilter (FdoFilter * filter) = 0;


    /// <summary>
    /// Prepares query
    /// </summary>
    /// <returns>Returns nothing.</returns>
    virtual void        Prepare () = 0;


    /// <summary>
    /// Set the target coordinate system and coordnate system type.
    /// All resulting geometries will be converted to this
    /// target.
    /// </summary>
    /// <param name="csname">Coordinate system name.</param>
    /// <returns>Returns nothing.</returns>
    virtual void        SetDestinationCS (const GWSCoordinateSystem & csname) = 0;

    /// <summary>
    /// Get the destination coordinate system name.
    /// </summary>
    /// <returns>String.</returns>
    virtual const GWSCoordinateSystem & GetDestinationCS () = 0;

    /// <summary>
    /// Set the source coordinate system override.
    /// All resulting geometries will be treated in this coordinate system.
    /// </summary>
    /// <param name="csname">Coordinate system name.</param>
    /// <returns>Returns nothing.</returns>
    virtual void        SetSourceCS  (const GWSCoordinateSystem & csname) = 0;

    /// <summary>
    /// Get the source coordinate system override.
    /// </summary>
    /// <param name="csname">Coordinate system name.</param>
    /// <returns>Returns nothing.</returns>
    virtual const GWSCoordinateSystem &  GetSourceCS () = 0;

    /// <summary>
    /// Set coordinate system converter factory. Query processor uses it
    /// to create converters.
    /// </summary>
    /// <param name="csfactory">Coordinate system converter factory.</param>
    virtual void        SetCSFactory (IGWSCoordinateSystemConverterFactory * csfactory) = 0;


    /// <summary>
    /// Get the coordinate system converter factory.
    /// </summary>
    /// <returns>IGWSCoordinateSystemConverterFactory *</returns>
    virtual IGWSCoordinateSystemConverterFactory * GetCSFactory () = 0;

    /// <summary>
    /// Executes query and returns query results
    /// </summary>
    /// <param name="results">Output feature set iterator.</param>
    /// <param name="bScrollable">Specify if a scrollable reader is desired.</param>
    /// <returns>Returns nothing.</returns>
    virtual void        Execute (IGWSFeatureIterator ** results, bool bScrollable = false) = 0;

};

/// <summary>
/// This interface represents read only feature. It also includes the feature
/// status in the GWS> Please note: The IGWSFeature inteface is really a wrapper
/// around the internal FdoFeatureReader. As such the feature data accessed
/// through this object is only valid until the next feature is iterated i.e.
/// until IGWSFeatureIterator::NextFeature() is called.
/// Therefore, if you need to cache a local copy of the feature data, create
/// a mutable feature from IGWSFeature using IGWSMutableFeature::Create.
/// </summary>
class IGWSFeature : public IGWSObject

{
public:
    /// <summary>
    /// Method is used to describe the set of properties this feature
    /// is composed of, taking into account joined feature descriptions.
    /// </summary>
    /// <returns>Returns nothing.</returns>
    virtual void                DescribeFeature(IGWSExtendedFeatureDescription ** ppResDesc) = 0;


    /// <summary>
    /// Methods returns feature id in cache. 0 indicates a features not
    /// cached feature
    /// </summary>
    /// <returns>Returns cache id.</returns>
    virtual FdoInt32            GetCacheId () = 0;

    /// <summary>
    /// Method returns the name of the layer source to which the feature
    /// belongs. Returns NULL if the feature has not been added to a layer.
    /// </summary>
    /// <returns>Returns layer source name</returns>
    virtual FdoString*          GetLayerSource () = 0;

    /// <summary>
    /// Methods returns true if the cached feature is a newly added feature
    /// </summary>
    /// <returns>Returns whether feature is new.</returns>
    virtual bool                IsNew () = 0;

    /// <summary>
    /// Methods returns true if the cached feature is a modified feature
    /// </summary>
    /// <returns>Returns whether feature is modified.</returns>
    virtual bool                IsModified () = 0;

    /// <summary>
    /// Methods returns true if the cached feature is a deleted feature
    /// </summary>
    /// <returns>Returns whether feature is deleted.</returns>
    virtual bool                IsDeleted () = 0;

    /// <summary>
    /// Gets the feature lock type in the cache
    /// </summary>
    /// <remarks>
    /// The following lock types are possible:
    ///   eGwsLockNolock            // feature not locked
    ///   eGwsLockPersistent        // persistent lock
    ///   eGwsLockOffline           // offline lock
    /// if lockType is eGwsLockPersistent or eGwsLockOffline, then the feature is
    /// an edit set memberr.
    /// </remarks>
    /// <returns>Returns a lock status.</returns>
    virtual EGwsLockType        GetCacheLockType () = 0;

    /// <summary>
    /// Methods returns revision number of the feature in the underlying Feature Source.
    /// If feature doesn't have revision number -1 is returned
    /// </summary>
    /// <returns>Returns feature revision number.</returns>
    virtual FdoInt32            GetRevisionNumber () = 0;

    /// <summary>
    /// Methods returns feature id of the feature in the underlying Feature Source.
    /// If feature id cannot be obtained (i.e. feature doesn't have identity)
    /// NULL is returned
    /// </summary>
    /// <returns>Returns feature id.</returns>
    virtual GWSFeatureId        GetFeatureId () = 0;

    /// <summary>
    /// If the feature is an instance of an FDO FeatureClass, this helper method returns
    /// the FDO geometry type of the primary geometry value. May throw exception.
    /// </summary>
    virtual FdoGeometryType     GetGeometryType(FdoByteArray* pArray) = 0;

    /// <summary>
    /// If the feature is an instance of an FDO FeatureClass, this helper method returns
    /// the name of the primary geometry value. May throw exception.
    /// </summary>
    virtual FdoString*          GetPrimaryGeometryName() = 0;

    /// <summary>
    /// Getters by property name
    /// </summary>
    /// <param name="propertyName">Input property name.</param>
    /// <returns>Returns as indicated.</returns>
    virtual bool                IsNull      (FdoString * propertyName) = 0;
    virtual FdoString   *       GetString   (FdoString * propertyName) = 0;
    virtual bool                GetBoolean  (FdoString * propertyName) = 0;
    virtual FdoByte             GetByte     (FdoString * propertyName) = 0;
    virtual FdoDateTime         GetDateTime (FdoString * propertyName) = 0;
    virtual double              GetDouble   (FdoString * propertyName) = 0;
    virtual FdoInt16            GetInt16    (FdoString * propertyName) = 0;
    virtual FdoInt32            GetInt32    (FdoString * propertyName) = 0;
    virtual FdoInt64            GetInt64    (FdoString * propertyName) = 0;
    virtual float               GetSingle   (FdoString * propertyName) = 0;
    virtual FdoLOBValue*        GetLOB      (FdoString * propertyName) = 0;
    virtual FdoIStreamReader*   GetLOBStreamReader (FdoString * propertyName) = 0;
    virtual FdoIRaster*         GetRaster   (FdoString * propertyName) = 0;
    virtual const FdoByte *     GetGeometry (FdoString * propertyName, FdoInt32 * count) = 0;
    virtual FdoByteArray*       GetGeometry (FdoString * propertyName) = 0;
    virtual FdoIFeatureReader*  GetFeatureObject (FdoString * propertyName) = 0;

    /// <summary>
    /// Returns the coordinate system name of the geometry properties
    /// </summary>
    /// <returns>FdoString.</returns>
    virtual const GWSCoordinateSystem &  GetCSName () = 0;

    /// <summary>
    /// Get data property value. NULL if propertyName is not a data property
    /// </summary>
    /// <param name="propertyName">Input property name.</param>
    /// <returns>Returns data value.</returns>
    virtual FdoDataValue *      GetDataValue (FdoString* propertyName) = 0;

    /// <summary>
    /// Get data property values specified by the property names.
    /// if property name in collection doesn't exist,
    /// null is placed in the collection
    /// </summary>
    /// <param name="propertyNames">Input property name collection.</param>
    /// <returns>Returns data value collection.</returns>
    virtual FdoDataValueCollection *
                                GetDataValues (FdoStringCollection* propertyNames) = 0;

    /// <summary>
    /// Geting string representaion of the property value
    /// Doesn't throw an exception
    /// </summary>
    /// <param name="propName">Input property name.</param>
    /// <param name="buff">Input/output string.</param>
    /// <param name="iProp">Input length of buffer.</param>
    /// <returns>Returns nothing.</returns>
    virtual void                ToString    (FdoString * propName, wchar_t * buff, int len) = 0;


    /// <summary>
    /// Returns a read-only binary representation of this feature. Note
    //  that this buffer must be copied and is only valid until the next
    /// feature is iterated i.e. until IGWSFeatureIterator::NextFeature()
    /// is called.
    /// </summary>
    /// <param name="bufLen">Output length of buffer.</param>
    /// <returns>Pointer to binary feature data.</returns>
    virtual unsigned char*      ToBuffer    (int& bufLen) = 0;
};


/// <summary>
/// This interface represents non specific feature iterator.
//  A read-only cursor returned as a result of feature or layer
/// query execution.
/// </summary>
class IGWSFeatureIterator : public IGWSFeature,
                            public FdoIFeatureReader
{
public:
    /// <summary>
    /// disposable interface methods. To avoid ambiquity
    /// </summary>
    virtual FdoInt32                AddRef() = 0;
    virtual FdoInt32                Release() = 0;

    // declarations below are to avoids ambiquity (methods are defined both
    // in IGWSFeature and FdoIFeatureReader)
    virtual bool                IsNull (FdoString * propName) = 0;
    virtual FdoString   *       GetString   (FdoString * propertyName) = 0;
    virtual bool                GetBoolean  (FdoString * propertyName) = 0;
    virtual FdoByte             GetByte     (FdoString * propertyName) = 0;
    virtual FdoDateTime         GetDateTime (FdoString * propertyName) = 0;
    virtual double              GetDouble   (FdoString * propertyName) = 0;
    virtual FdoInt16            GetInt16    (FdoString * propertyName) = 0;
    virtual FdoInt32            GetInt32    (FdoString * propertyName) = 0;
    virtual FdoInt64            GetInt64    (FdoString * propertyName) = 0;
    virtual float               GetSingle   (FdoString * propertyName) = 0;
    virtual FdoLOBValue*        GetLOB      (FdoString * propertyName) = 0;
    virtual FdoIStreamReader*   GetLOBStreamReader (FdoString * propertyName) = 0;
    virtual FdoIRaster*         GetRaster   (FdoString * propertyName) = 0;
    virtual const FdoByte *     GetGeometry (FdoString * propertyName, FdoInt32 * count) = 0;
    virtual FdoByteArray*       GetGeometry (FdoString * propertyName) = 0;
    virtual FdoIFeatureReader*  GetFeatureObject (FdoString * propertyName) = 0;

    /// <summary>
    /// moves the current position on the next feture and returns it
    /// </summary>
    virtual bool                NextFeature (IGWSFeature ** feature) = 0;

    /// <summary>
    /// Get features joined with the given one. The index range is
    /// from 0 to the number of joined features - 1 in the
    /// extended feature description;
    /// </summary>
    /// <param name="i">Input count of features to return.</param>
    /// <returns>Returns a feature iterator.</returns>
    virtual IGWSFeatureIterator* GetJoinedFeatures (int i) = 0;

    /// <summary>
    /// returns true if this iterator is a valid, scrollable iterator
    /// </summary>
    virtual bool Scrollable() = 0;

    //scrollable iterator interface
    virtual int                 Count() = 0;
    virtual bool                ReadFirst() = 0;
    virtual bool                ReadLast() = 0;
    virtual bool                ReadPrevious() = 0;
    virtual bool                ReadAt(FdoPropertyValueCollection* key) = 0;
    virtual bool                ReadAtIndex( unsigned int recordindex ) = 0;
    virtual unsigned int        IndexOf(FdoPropertyValueCollection* key) = 0;

    /// <summary>
    /// Gets the original (unconverted) geometry value of the specified
    /// property as a byte array.
    /// </summary>
    virtual FdoByteArray*       GetOriginalGeometry(FdoString* propertyName) = 0;
};

/// <summary>
/// This interface represents mutable(writable) feature for the purpose of
/// inserting or updating features in the layer source or for creating a
/// local copy of feature data.
/// </summary>
class IGWSMutableFeature: public IGWSFeature
{
public:
    /// <summary>
    /// Creates a mutable feature from a regular (read-only) feature. This
    /// method copies both meta-data and feature property values to the
    /// to the mutable feature. Use this method to create a local copy of
    /// an IGWSFeature.
    /// </summary>
    /// <param name="pFrom">Input feature to create.</param>
    /// <param name="propertyName">Output new feature.</param>
    /// <returns>Returns status.</returns>
    static GWS_QUERYENGINE_API EGwsStatus
                            Create(IGWSFeature        * pFrom,
                                   IGWSMutableFeature** pNewFeature);

    static GWS_QUERYENGINE_API EGwsStatus
                            Create(IGWSFeature        * pFrom,
                                   IGWSObject         * owner,
                                   IGWSMutableFeature** pNewFeature);

    static GWS_QUERYENGINE_API EGwsStatus
                            CreatePrimary(IGWSFeature        * pFrom,
                                          IGWSMutableFeature** pNewFeature);

    static GWS_QUERYENGINE_API EGwsStatus
                            Create(IGWSExtendedFeatureDescription * pefdsc,
                                   IGWSObject         * owner,
                                   IGWSMutableFeature** pNewFeature);

    static GWS_QUERYENGINE_API EGwsStatus
                            Create(IGWSExtendedFeatureDescription * pefdsc,
                                   unsigned char      * pBuf,
                                   int                  len,
                                   IGWSMutableFeature** pNewFeature);

    virtual const     GWSQualifiedName &    ClassName () const = 0;


    /// <summary>
    /// Returns true if property value has been set.
    /// Mutable feature getters throw exception if the property
    /// value is not set.
    /// </summary>
    /// <param name="propertyName">Input property name.</param>
    /// <returns>boolean.</returns>
    virtual bool      IsPropertyValueSet (FdoString* propertyName) = 0;

    /// <summary>
    /// Sets the values for property name.  Exceptions may be thrown on
    /// failure.
    /// </summary>
    /// <param name="propertyName">Input property name.</param>
    /// <param name="value">Input value.</param>
    /// <returns>Returns nothing.</returns>
    virtual void      SetNull(FdoString* propertyName) = 0;
    virtual void      SetString(FdoString* propertyName, FdoString* pValue) = 0;
    virtual void      SetBoolean(FdoString* propertyName, bool value) = 0;
    virtual void      SetByte(FdoString* propertyName, FdoByte value) = 0;
    virtual void      SetDateTime(FdoString* propertyName, FdoDateTime value) = 0;
    virtual void      SetDouble(FdoString* propertyName, double value) = 0;
    virtual void      SetInt16(FdoString* propertyName, FdoInt16 value) = 0;
    virtual void      SetInt32(FdoString* propertyName, FdoInt32 value) = 0;
    virtual void      SetInt64(FdoString* propertyName, FdoInt64 value) = 0;
    virtual void      SetSingle(FdoString* propertyName, float value) = 0;
    virtual void      SetLOB(FdoString* propertyName, FdoLOBValue* pValue) = 0;
    virtual void      SetGeometry (FdoString* propertyName, FdoByteArray * pValue) = 0;
    virtual void      SetValue (FdoString* propertyName, FdoValueExpression* pVal) = 0;

    /// <summary>
    /// Sets the coordinate system name and type of feature geometry
    /// properties.
    /// </summary>
    /// <param name="csname">Coordinate system name.</param>
    /// <returns>Returns nothing.</returns>
    virtual void      SetCSName (const GWSCoordinateSystem & csname) = 0;


    /// <summary>
    /// Sets the source coordinate system override.
    /// If set, feature geometry will be converted to it when saving feature
    /// in the feature source
    /// </summary>
    /// <param name="csname">Coordinate system name.</param>
    /// <returns>Returns nothing.</returns>
    virtual void      SetSourceCSName (const GWSCoordinateSystem & csname) = 0;

    /// <summary>
    /// Gets the source coordinate system override.
    /// </summary>
    /// <returns>Coordinate system name.</returns>
    virtual const GWSCoordinateSystem & GetSourceCSName () = 0;


    /// <summary>
    /// Get the property values
    /// </summary>
    virtual FdoPropertyValueCollection*  GetPropertyValues() = 0;

    /// <summary>
    /// Set property values
    /// </summary>
    virtual void      SetPropertyValues(FdoPropertyValueCollection * vals) = 0;

    /// <summary>
    /// The methods below are indended for internal use.
    /// </summary>

    /// <summary>
    /// Set feature id.
    /// </summary>
    virtual void      SetFeatureId  (const GWSFeatureId & fid) = 0;

    /// <summary>
    /// Set feature revision number.
    /// </summary>
    virtual void      SetRevisionNumber (FdoInt32 revnum) = 0;

    /// <summary>
    /// Set cache lock type.
    /// </summary>
    virtual void      SetCacheLockType (EGwsLockType locktype) = 0;

     /// <summary>
    /// Set cache status.
    /// </summary>
    virtual void      SetCacheStatus (long status) = 0;

    /// <summary>
    /// Set locg cache id.
    /// </summary>
    virtual void      SetCacheId (FdoInt32 cacheId) = 0;

    /// <summary>
    /// Sets the layer source of the mutable feature
    /// </summary>
    /// <returns>nothing</returns>
    virtual void      SetLayerSource (FdoString *) = 0;

};

namespace GwsQueryHelpers
{


    /// <summary>
    /// Creates the "flat" Fdo Feature reader from the Gws Feature iterator
    /// If there join iterator yield multiple features, only the first
    /// feature from such iterator will be present in the "flat" results
    /// </summary>
    /// <returns>nothing</returns>
    GWS_QUERYENGINE_API
    void  CreateFlatFdoReader (IGWSFeatureIterator * pFrom,
                               FdoIFeatureReader  ** pFdoReader);

    /// <summary>
    /// Creates the "flat" Gws Feature iterator from the Gws Feature iterator
    /// If there join iterator yield multiple features, only the first
    /// feature from such iterator will be present in the "flat" results
    /// </summary>
    /// <returns>nothing</returns>
    GWS_QUERYENGINE_API
    void  CreateFlatGwsIterator (IGWSFeatureIterator *  pFrom,
                                 IGWSFeatureIterator ** pGwsIterator);

    /// <summary>
    /// Clones property definition and assignes new name to the
    /// output definition
    /// If newName is null, the exact copy is made
    /// </summary>
    /// <returns>FdoPropertyDefinition *</returns>

    GWS_QUERYENGINE_API
    FdoPropertyDefinition * ClonePropertyDefinition (
                               FdoString * newName,
                               FdoPropertyDefinition * pDefIn,
                               bool forceCloneReadOnly = false);

    GWS_QUERYENGINE_API
    void BuildFlatDescription(IGWSQueryDefinition* pQryDef,
                              IGWSExtendedFeatureDescription* pJoinDesc,
                              IGWSExtendedFeatureDescription** ppResDesc);
};

#endif /* GWSQUERYENGINE_EXPORTS */

#ifndef GWSCommon_H
#define GWSCommon_H
// Copyright (C) 2004-2010 by Autodesk, Inc.
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
#ifdef GWSCOMMON_EXPORTS
#define GWS_COMMON_API __declspec(dllexport)
#else
#define GWS_COMMON_API __declspec(dllimport)
#endif
#else
#define GWS_COMMON_API
#endif

#include <Fdo.h>

#ifdef GWS_MULTHREADED_CLIENT
#define GWSThreadModel GWSMultiThreadModel
#else
#define GWSThreadModel GWSSingleThreadModel
#endif

// string related definitions
#ifndef _WIN32
    #define _wcsicmp  wcscasecmp
    #define _wcsnicmp wcsncasecmp

    #define _stricmp  strcasecmp
    #define _strnicmp strncasecmp
#endif

typedef std::wstring        WSTR;
typedef std::vector<WSTR>   WSTRARRAY;

// case insensitive comparison for wstrings to be used in stl map
// templates
typedef struct less_ignore_case_fast : std::less<const std::wstring>
{
    bool operator()(const std::wstring& s1, const std::wstring& s2) const
    {
        return _wcsicmp (s1.c_str(), s2.c_str()) < 0;
    };
} GwsIgnoreCase;

// String to String case insensitive map
typedef std::map<WSTR,WSTR,GwsIgnoreCase> GwsStrToStrMap;

// Diagnostics parameters
typedef GwsStrToStrMap GwsDiagParameters;

// statuses enumerator
typedef enum EGwsStatus {
     // Good
     eGwsOk      = 0

     // warnings
    ,eGwsWarningStorageAlreadyLoaded
    ,eGwsWarningFailedToLockSomeFeatures
    ,eGwsWarningFailedToUnlockSomeFeatures
    ,eGwsWarningFailedToConnectSomeFeatureSources
    ,eGwsWarningFeatureSourceSessionNotStarted
    ,eGwsWarningNotCaching              // cache is not used.
    ,eGwsWarningStillLoadingCache       // cache is being loaded
    ,eGwsWarningCacheLoadingStopped     // cache loading stopped
    ,eGwsOperationIsNotRunning          // operation is not running
    ,eGwsOperationCanceled              // operation has been canceled
    ,eGwsWarningSomePropertyValuesAreUninitialized // some property values are uninitilaized
    ,eGwsWarningFeatureSourceClassIsNotVersionEnabled
    ,eGwsWarningExtentsNotYetAvailable // extents are not available
    ,eGwsWarningExtentsAlreadyLoaded
    ,eGwsWarningTooManyExtentsInProcess


    // Failures
    ,eGwsFailed
    ,eGwsNullPointer
    ,eGwsOutOfMemory
    ,eGwsNotSupported
    ,eGwsOperationInProgress            // operation is still running
    ,eGwsDeleteCommandNotSupported
    ,eGwsUpdateCommandNotSupported
    ,eGwsInsertCommandNotSupported
    ,eGwsFdoProviderError
    ,eGwsAlreadyOpened
    ,eGwsClosed
    ,eGwsDisconnected
    ,eGwsConnected
    ,eGwsConnectionNotInitialized
    ,eGwsFailedToGetLockInformation
    ,eGwsOutstandingSessions
    ,eFailedToStartFeatureSourceSession
    ,eFailedToStartLayerSourceSession
    ,eFailedToStartTransaction
    ,eGwsNoTransactionStarted
    ,eGwsFailedToCommitCache
    ,eGwsFailedToRollbackCache
    ,eGwsFailedToCommitTransaction
    ,eGwsFailedToRollbackTransaction
    ,eGwsFailedToCommitSomeTransactions
    ,eGwsFailedToRollbackSomeTransactions
    ,eGwsFailedToTerminateSession
    ,eGwsSessionNotFound
    ,eGwsFailedToFlushChanges
    ,eGwsFeatureSourceDoesnotSupportLongIdentity // Cache feature source must support long autoincreament identity
                                                 // otherwise it is not good for GWS
    ,eGwsVersionNotFound
    ,eGwsVersionResolutionFailed
    ,eGwsGwsAlreadyExists
    ,eGwsFailedToDesribeClass
    ,eGwsFeatureStorageDoesNotExist
    ,eGwsFeatureStorageAlreadyCreated
    ,eGwsFailedToPrepareQuery
    ,eGwsFailedToCreateQuery
    ,eGwsFailedToExecuteQuery
    ,eGwsDoesNotSupportRightJoin
    ,eGwsDoesNotSupportLeftJoin
    ,eGwsNotAFeatureClass
    ,eGwsNoGeometryProperty
    ,eGwsNoExplicitFeatureSourceInLayerQuery
    ,eGwsDataSourceNotFound
    ,eGwsDataSourceAlreadyExists
    ,eGwsDataSourceAlreadyAdded
    ,eGwsConnectionParameterNotFound
    ,eGwsFailedToInitializeFdoConnection
    ,eGwsFailedToOpenFdoConnection
    ,eGwsFailedToCloseFdoConnection
    ,eGwsFailedToReadGws
    ,eGwsFailedToWriteGws
    ,eGwsLayerSourceNotPartOfGws
    ,eGwsFailedToDropLayerStorage
    ,eGwsFailedToCreateLayerStorage
    ,eGwsFailedToAddPropertyDefinition
    ,eGwsFailedToAddIdentityPropertyDefinition
    ,eGwsFailedToCreateGwsFeatureDefintion
    ,eGwsFailedToCacheFeatures
    ,eGwsFailedToDeleteFeature
    ,eGwsFailedToDeleteLayerFeature
    ,eGwsGetCachedFeaturesByIdsFailed
    ,eGwsStorageNotLoaded
    ,eGwsInvalidParameter
    ,eGwsObjectsRemainInEditSet
    ,eGwsItemAlreadyAdded
    ,eGwsItemNotFound
    ,eGwsFdoReadOnlyProperty
    ,eGwsFdoQueryIsNotPrepared
    ,eGwsFdoCommandIsNotPrepared
    ,eGwsFdoLockConflict
    ,eGwsFdoVersionConflict
    ,eGwsFailedToExecuteCommand
    ,eGwsFailedToPrepareCommand
    ,eGwsFailedToFetchResults
    ,eGwsFailedToGetProperty
    ,eGwsFdoInvalidPropertyType
    ,eGwsFdoUnsupportedPropertyType
    ,eGwsFeatureDoesNotExist
    ,eGwsFeatureHasBeenDeleted
    ,eGwsRevisionNumbersConflict
    ,eGwsInvalidPropertyName
    ,eGwsFailedToLockFeatures
    ,eGwsFailedToUnlockFeatures
    ,eGwsCannotRemoveNewFeatureFromEditSet
    ,eGwsJoinPropertiesCardinalityViolation
    ,eGwsFailedToCreateThread
    ,eGwsClosedFeatureIterator
    ,eGwsInvalidGwsName
    ,eGwsIsReadOnly
    ,eGwsFileNotFound
    ,eGwsCannotCopyGwsCacheFiles
    ,eGwsGwsCacheCorrupted
    ,eGwsInvalidCacheId
    ,eGwsFailedToInsertFeature
    ,eGwsFailedToInsertLayerFeature
    ,eGwsFailedToUpdateFeature
    ,eGwsFailedToUpdateLayerFeature
    ,eGwsIndexOutOfBounds
    ,eGwsIdNotUnique
    ,eGwsEditSetUnavailable
    ,eGwsFailedToSaveGws
    ,eGwsFailedToDescribeLayer
    ,eGwsCannotGetPropertyValueOffline
    ,eGwsFeatureNotFoundOnline
    ,eGwsFailedToCommitLayerSourceTransaction
    ,eGwsFailedToCreateMutableFeature
    ,eGwsFailedToPrepareLayerSourceQuery
    ,eGwsClassNotReferencedByLayer
    ,eGwsWrongLayerFeatureId
    ,eGwsFailedToSaveEditSet
    ,eGwsMissingLeftJoinAttributes
    ,eGwsInvalidJoinAttributeType
    ,eGwsJoinAttributesAreNotComparable
    ,eGwsInvalidFeatureSourceName

    ,eGwsFailedToTransformFeature
    ,eGwsFailedToParseFeature
    ,eGwsSCNotFound
    ,eGwsFailedToHandleSourceCS
    ,eGwsFailedToHandleDestinationCS
    ,eGwsFailedToInitializeCSC

    ,eGwsFeatureSourceSessionTerminated
    ,eGwsLayerSourceSessionTerminated
    ,eGwsCannotMakeUniqueGwsName
    ,eGwsCannotSetActiveVersion
    ,eGwsNotAllFeatureSourcesConnected
    ,eGwsFailedToUpdateLayerSourceVersion
    ,eGwsProviderNotRegistered
    ,eGwsFailedToRefreshFeatures
    ,eGwsNoLayers
    ,eGwsFeatureSourceIsReadOnly
    ,eGwsFeatureClassHasNoIdentity
    ,eGwsFdoInvalidDataType

} EGwsStatus;


/// <summary>
/// Feature status returned in the feature report.
/// </summary>
/// <param name="eGwsSucceeded">
///  Operation was successul for the feature.
/// </param>
/// </summary>
/// <param name="eGwsRevisionNumberConflict">
///  Revision number conflict for the feature.
/// </param>
/// <param name="eGwsLockRejected">
///  Persistent lock has been rejected for the feature.
/// </param>
/// <param name="eGwsUnlockRejected">
///  Persistent lock has not been released for the feature.
/// </param>
/// <param name="eGwsLockedOffline">
///  Feature has been added to the edit set offline.
/// </param>
/// <param name="eGwsCannotUnlockOffline">
/// Feature that has persistent lock cannot be removed from the edit set
/// offline.
/// </param>
/// <param name="eGwsFeatureAlreadyInEditSet">
/// Feature was not added to the edit set because it is already there.
/// </param>
/// <param name="eGwsSkippedNewFeature">
/// New feature in the edit set has been ignored by the operation.
/// </param>
/// <param name="eGwsSkippedNotLockedFeature">
/// Feature is not in the edit set and was skipped by the operation.
/// </param>
/// <param name="eGwsFeatureNotSaved">
/// Feature was not saved in the target feature source.
/// </param>
/// <param name="eGwsNotProcessed">
/// Feature was not processed because operation has been terminated prematurely.
/// </param>
/// <param name="eGwsRefreshFailedToReloadEditSetFeature">
/// Feature was not refreshed because it is part of the edit set.
/// </param>
/// <param name="eGwsRefreshCacheFeatureIsUpTodate">
/// Feature is up-to-date.
/// </param>
/// <param name="eGwsRefreshDeletedFeatureInCache">
/// Feature has been deleted from the cache during refresh.
/// </param>
/// <param name="eGwsRefreshReloadedFeatureInCache">
/// Feature has been refreshed in the cache.
/// </param>
/// <param name="eGwsRefreshFeatureNotFound">
/// Feature was not found in the feature source and in the cache.
/// </param>

typedef enum _EGwsFeatureStatus{
     eGwsSucceeded
    ,eGwsRevisionNumberConflict
    ,eGwsLockRejected
    ,eGwsLockedOffline
    ,eGwsUnlockRejected
    ,eGwsCannotUnlockOffline
    ,eGwsFeatureAlreadyInEditSet
    ,eGwsSkippedNewFeature
    ,eGwsSkippedNotLockedFeature
    ,eGwsFeatureNotSaved
    ,eGwsCannotRemoveModifedFeatureOffline
    ,eGwsNewFeatureCannotBeRemoved
    ,eGwsNewFeatureRemoved
    ,eGwsNotProcessed
    ,eGwsRefreshFailedToReloadEditSetFeature
    ,eGwsRefreshCacheFeatureIsUpTodate
    ,eGwsRefreshDeletedFeatureInCache
    ,eGwsRefreshReloadedFeatureInCache
    ,eGwsRefreshFeatureNotFound
} EGwsFeatureStatus;

/// <summary>
/// The feature lock status.
/// </summary>
typedef enum _EGwsLockType
{
     eGwsLockNolock     = 0     // not locked
    ,eGwsLockPersistent = 1     // Persistent lock
    ,eGwsLockOffline    = 2     // Offline lock  (no persistent lock but feature is treated as locked)
} EGwsLockType;

/// <summary>
/// Coordinate system type.
/// </summary>
/// <param name="eGwsCSWkt">WKT type.</param>
/// <param name="eGwsCSAbbr">Abbreviated type.</param>
typedef enum _EGwsCSType
{
    eGwsCSWkt  = 1,
    eGwsCSAbbr = 2
} EGwsCSType;

/// <summary>
/// Qualified name. Fully qualified feature class or relational table name
/// Fully qualified name consists of :
///   FeatureSource name (optional.)
///   Schema name (may be optional for relational sources)
///   Class or table name
/// <summary>
class GWSQualifiedName
{
friend class GWSClassName;
public:
    GWS_COMMON_API
                      GWSQualifiedName ();
    GWS_COMMON_API
                      GWSQualifiedName (FdoString * datasource, FdoString * schema, FdoString * name);
    GWS_COMMON_API
                      GWSQualifiedName (FdoString * schema, FdoString * name);
    GWS_COMMON_API
                      GWSQualifiedName (FdoString * name);
    GWS_COMMON_API
                      GWSQualifiedName (const GWSQualifiedName & other);
    GWS_COMMON_API
    virtual           ~GWSQualifiedName();

    GWS_COMMON_API
    FdoString *       FeatureSource () const;
    GWS_COMMON_API
    FdoString *       Schema () const;
    GWS_COMMON_API
    FdoString *       Name () const;

    GWS_COMMON_API
    void              operator= (const GWSQualifiedName & other);
    GWS_COMMON_API
    bool              operator==(const GWSQualifiedName & other) const;
    GWS_COMMON_API
    bool              operator< (const GWSQualifiedName & other) const;
    GWS_COMMON_API
    bool              operator>(const GWSQualifiedName & other) const;

    /// <summary>Returns an FDO compatible qualified class name
    /// of the form <Schema>:<Class>
    /// </summary>
    /// <param name="str">Input a buffer big enough to hold the name</param>
    /// <param name="numchars">Input the number of UNICODE chars including the terminting null.</param>
    /// <returns>Number of characters written.</returns>
    GWS_COMMON_API
    int               ToString  (wchar_t * str, int numchars) const;

    /// <summary>Returns the fully qualified name, including the feature source name:
    /// <Schema>:<Class>@<FeatureSource>
    /// </summary>
    /// <param name="str">Input a buffer big enough to hold the name</param>
    /// <param name="numchars">Input the number of UNICODE chars including the terminting null.</param>
    /// <returns>Number of characters written.</returns>
    GWS_COMMON_API
    int               ToFullyQualifedString  (wchar_t * str, int numchars) const;

private:
    FdoStringP        m_featuresource;
    FdoStringP        m_schema;
    FdoStringP        m_name;
};


/// <summary>
/// Feature Id. A collection of Fdo Data Values
/// <summary>
class GWSFeatureId
{
public:
    /// <summary>Default constructor of a Feature identifier</summary>
    /// <returns>Returns nothing.</returns>
    GWS_COMMON_API                 GWSFeatureId ();


    /// <summary>Construct s a Feature identifier</summary>
    /// <param name="values">Input the identity value collection</param>
    /// <returns>Returns nothing.</returns>
    GWS_COMMON_API                 GWSFeatureId (FdoDataValueCollection * values);

    ///<summary>Constructs a Feature identifier from the single long</summary>
    /// <param name="id">Input feature id as long</param>
    /// <returns></returns>
    GWS_COMMON_API                 GWSFeatureId (int id);


    ///<summary>Constructs a Feature identifier</summary>
    /// <param name="src">Input the source collection</param>
    /// <returns>Returns nothing.</returns>
    GWS_COMMON_API                 GWSFeatureId (const GWSFeatureId& src);


    /// <summary>Destructor</summary>
    /// <returns>Returns nothing.</returns>
    GWS_COMMON_API virtual         ~GWSFeatureId();


    ///<summary>Creates fdo data value collection</summary>
    /// <returns>Data value collection</returns>
    static GWS_COMMON_API FdoDataValueCollection
                            *      CreateDataValueCollection ();


    /// <summary>Gets the number of values in the identifier.</summary>
    /// <returns>Returns number of items in the collection</returns>
    GWS_COMMON_API  FdoInt32       GetCount() const;

    /// <summary>Gets the item in the collection at the specified index. Throws an
    /// invalid argument exception if the index is out of range.</summary>
    /// <param name="index">Input index</param>
    /// <returns>Returns the item in the collection at the specified index</returns>
    GWS_COMMON_API FdoDataValue *  GetItem(FdoInt32 index) const;


    /// <summary>Less than operator.  Note this operation compares
    /// fields in order so ordering of fields is important. </summary>
    /// <param name="other">Input object for comparison</param>
    /// <returns>Returns whether this object is less than the input
    /// object.</returns>
    GWS_COMMON_API bool            operator< (const GWSFeatureId & other) const;

    ///<summary>equals operator</summary>
    /// <param name="other">Input object for comparison</param>
    /// <returns>Returns whether this object is equal to the input object.</returns>
    GWS_COMMON_API bool            operator== (const GWSFeatureId & other) const ;

    /// <summary>assignement  operator</summary>
    /// <param name="other">Input object for assignment</param>
    /// <returns>Returns nothing.</returns>
    GWS_COMMON_API GWSFeatureId &  operator= (const GWSFeatureId & other);

    /// <summary>assign property values</summary>
    /// <param name="other">Input array for assignment</param>
    /// <returns>Returns nothing.</returns>
    GWS_COMMON_API GWSFeatureId &  operator= (FdoDataValueCollection * vals);

    ///<summary>compares feature ids</summary>
    /// <returns>
    /// -1 this < other
    /// 0 this == other
    /// 1 this > other
    /// -2 this is null other is not null
    /// 2 this is not null other is null
    ///</returns>
    GWS_COMMON_API int             Compare (const GWSFeatureId & other) const;

    ///<summary>verified nullability of the the ley values</summary>
    /// <returns>
    /// return true if at least one propertry value is null
    ///</returns>
    GWS_COMMON_API bool            IsNull () const;

    ///<summary>returns string representation of the feature id</summary>
    /// <returns>
    /// the number of characters in the resuling buffer
    ///</returns>
    GWS_COMMON_API int             ToString  (wchar_t * str, int numchars) const;


private:
    FdoPtr<FdoDataValueCollection> m_values;
};

/// <summary>
/// Extended Feature Id. Fully qualified class name and a collection
/// of Fdo Data Values
/// <summary>
class GWSExtendedFeatureId
{
    public:
    /// <summary>Default constructor of am extended Feature identifier</summary>
    /// <returns>Returns nothing.</returns>
    GWS_COMMON_API             GWSExtendedFeatureId ();


    /// <summary>Constructs an Extended Feature identifier.</summary>
    /// <param name="qname">Input the qualified name for the feature.</param>
    /// <param name="values">Input the feature id for the feature.</param>
    /// <returns>Returns nothing.</returns>
    GWS_COMMON_API             GWSExtendedFeatureId (
                                      const GWSQualifiedName & qname,
                                      const GWSFeatureId     & id);

    /// <summary>Copy Constructor for an extended Feature identifier.</summary>
    /// <param name="src">Input the source feature id.</param>
    /// <returns>Returns nothing.</returns>
    GWS_COMMON_API             GWSExtendedFeatureId (const GWSExtendedFeatureId & src);


    /// <summary>Destructor</summary>
    GWS_COMMON_API virtual     ~GWSExtendedFeatureId();

    /// <summary>Gets the class name of the extended feature id.</summary>
    /// <returns>Returns class name </returns>
    GWS_COMMON_API const GWSQualifiedName & ClassName () const;

    /// <summary>Gets simple feature id.</summary>
    /// <returns>Returns feature id</returns>
    GWS_COMMON_API const GWSFeatureId &FeatureId () const;


    /// <summary>Gets the number of values in the identifier.</summary>
    /// <returns>Returns number of items in the collection</returns>
    GWS_COMMON_API  FdoInt32   GetCount() const;

    /// <summary>Gets the item in the collection at the specified index. Throws an
    /// invalid argument exception if the index is out of range.</summary>
    /// <param name="index">Input index</param>
    /// <returns>Returns the item in the collection at the specified index</returns>
    GWS_COMMON_API FdoDataValue * GetItem(FdoInt32 index) const;


    /// <summary>less than operator</summary>
    /// <param name="other">Input feature id to compare.</param>
    /// <returns>Returns whether this object is less than the input
    /// object.</returns>
    GWS_COMMON_API bool            operator< (const GWSExtendedFeatureId & other) const;

    /// <summary>equals operator</summary>
    /// <param name="other">Input feature id to compare.</param>
    /// <returns>Returns whether this object is equal to the input
    /// object.</returns>
    GWS_COMMON_API bool            operator== (const GWSExtendedFeatureId & other) const ;

    /// <summary>assignement  operator</summary>
    /// <returns>Returns nothing.</returns>
    GWS_COMMON_API void            operator= (const GWSExtendedFeatureId & other);


    /// <summary>Returns string representation of the extended feature id</summary>
    /// <param name="str">Input/output buffer for the result string.</param>
    /// <param name="numchars">Input size of the buffer.</param>
    /// <returns>
    /// the number of characters in the resulting buffer
    /// </returns>
    GWS_COMMON_API int             ToString  (wchar_t * str, int numchars) const;

private:
    GWSQualifiedName        m_qname;
    GWSFeatureId            m_fid;
};


/// <summary>
/// Coordinate system name. Also includes type
/// <summary>
class GWSCoordinateSystem
{
public:
    /// <summary>Default constructor</summary>
    GWS_COMMON_API          GWSCoordinateSystem ();

    /// <summary>Constructs wkt coordinate system name</summary>
    /// <param name="wkncsname">coordinate system name.</param>
    GWS_COMMON_API          GWSCoordinateSystem (FdoString * wkncsname);

    /// <summary>Constructs coordinate system name</summary>
    /// <param name="csname">coordinate system name.</param>
    /// <param name="cstype">coordinate system type.</param>
    GWS_COMMON_API          GWSCoordinateSystem (FdoString * csname,
                                                 EGwsCSType cstype);

    /// <summary>Copy constructor.</summary>
    /// <param name="other">other coordinate system name.</param>
    GWS_COMMON_API          GWSCoordinateSystem (const GWSCoordinateSystem & other);

    /// <summary>Destructor</summary>
    GWS_COMMON_API virtual  ~GWSCoordinateSystem ();

    /// <summary>Get coordinate system name.</summary>
    GWS_COMMON_API FdoString* Name () const;

    /// <summary>Get coordinate system type.</summary>
    GWS_COMMON_API EGwsCSType Type () const;

    /// <summary>Assignment operator.</summary>
    GWS_COMMON_API
    const GWSCoordinateSystem & operator= (const GWSCoordinateSystem & other);

    /// <summary>Casting name to string.</summary>
    GWS_COMMON_API             operator FdoString * () const;

    /// <summary>Equal operator.</summary>
    GWS_COMMON_API
    bool                       operator== (const GWSCoordinateSystem & other) const;

    /// <summary>Verifies that coordinate system name is empty.</summary>
    GWS_COMMON_API
    bool                       IsEmpty () const;

private:
    FdoStringP          m_csname;
    EGwsCSType          m_cstype;
};

/// <summary>
/// This class describes the problem description class.  When a
/// function makes changes to a number of entities as a group, then
/// many errors may be created.  This interface provides a mechanism
/// for the user to display the errors.
/// </summary>
class GWSStatus
{
public:
    /// <summary>
    /// Gets the type of error.
    /// </summary>
    /// <returns>Returns the type of error. </returns>
    virtual EGwsStatus      GetStatus() const = 0;

    /// <summary>
    /// Gets the message associated with the error if any.
    /// </summary>
    /// <returns>Returns the error message.</returns>
    virtual FdoString*      GetMessage() const = 0;

    /// <summary>
    /// Gets the name value pair at index. These values may be the name of the
    /// Feature source which could not be committed, or other useful parameter.
    /// </summary>
    /// <param name="index">Input index for the name value pair.</param>
    /// <param name="name">Output name of the parameter.</param>
    /// <param name="value">Output value of the parameter.</param>
    /// <returns>Returns false if there is no pair at this index.</returns>
    virtual bool            GetParameterAt(unsigned int index, FdoString*& name, FdoString*& value) const = 0;

    /// <summary>
    /// Gets number of parameters
    /// </summary>
    /// <returns>Returns the number of parameters.</returns>
    virtual int             GetParameterCount () const = 0;

};

/// <summary>
/// All GWS objects are reference counted.
/// </summary>
class IGWSDisposable
{
public:
    /// <summary>
    /// Increases the reference count on the object.
    /// </summary>
    /// <returns>Returns nothing.</returns>
    virtual FdoInt32 AddRef() = 0;
    /// <summary>
    /// Decreases the reference count on the object and disposes of it
    /// when the count is 0.
    /// </summary>
    /// <returns>Returns nothing.</returns>
    virtual FdoInt32 Release() = 0;

protected:
    /// <summary>
    /// Called to allow the object to release any resources it may
    /// be keeping. Default implementation calls 'delete this'.
    /// </summary>
    /// <returns>The new reference count</returns>
    virtual void Dispose() = 0;
};

/// <summary>
/// GWS objects can be owned by other GWS objects. Ownership does not
/// necessarily imply the owner controls the lifetime of the ownee. Typically
/// GWS objects are owned by the object they are created from.
/// </summary>
class IGWSObject : public IGWSDisposable
{
public:
    /// <summary>
    /// Gets the owner of this object.
    /// </summary>
    /// <returns>IGWSObject*</returns>
    virtual IGWSObject* GetOwner() = 0;

protected:
    /// <summary>
    /// Sets the owner of this object.
    /// <param name="pObject">Input the object's owner.</param>
    /// </summary>
    /// <returns>Returns nothing.</returns>
    virtual void SetOwner(IGWSObject* pObject) = 0;
};

/// <summary>
/// This interface describes the exception object thrown by GWS functions.
/// </summary>
class IGWSException : public FdoException

{
public:
    GWS_COMMON_API
    static  IGWSException       * Create();

    GWS_COMMON_API
    static IGWSException        * Create (EGwsStatus stat);
    GWS_COMMON_API
    static IGWSException        * Create (FdoString* message,
                                          EGwsStatus stat);
    GWS_COMMON_API
    static IGWSException        * Create (FdoString* message,
                                          EGwsStatus stat,
                                          FdoException* cause);
    GWS_COMMON_API
    static IGWSException        * Create  (GWSStatus * stat);

    GWS_COMMON_API
    static IGWSException        * Create  (EGwsStatus    fes,
                                           FdoException* pCause);

    GWS_COMMON_API
    static IGWSException        * Create  (EGwsStatus type,
                                           GWSStatus * stat);

    // the list must be explicitly terminated with a NULL
    GWS_COMMON_API
    static IGWSException        * Create(EGwsStatus type,
                                         FdoException* pCause,
                                         FdoString* name,
                                         FdoString* value,
                                         ...);   // for other name value pairs, odd number means empty value.

    /// <summary>Gets the message associated with this exception. If no
    /// message and no cause are associated with this exception then null is
    /// returned.</summary>
    /// <returns>Returns the error message</returns>
    virtual FdoString*     GetExceptionMessage() = 0;

    /// <summary>Gets the message associated with this exception. If no
    /// message is associated with this exception then null is
    /// returned. The returned string should be deleted by the caller.
    /// </summary>
    /// <returns>Returns the error message</returns>
    virtual FdoString*     ToString() = 0;

    /// <summary>Gets the Gws Status associated with this exception.
    /// </summary>
    /// <returns>Returns the Gws status associated with the exception.</returns>
    virtual EGwsStatus     GetStatus () const  = 0;

    /// <summary>
    /// Gets the name value pair at index. These values may be the name of the
    /// Feature source which could not be committed, or other useful parameter.
    /// </summary>
    /// <param name="index">Input index for the name value pair.</param>
    /// <param name="name">Output name of the parameter.</param>
    /// <param name="value">Output value of the parameter.</param>
    /// <returns>Returns false if there is no pair at this index.</returns>
    virtual bool            GetParameterAt(unsigned int index,
                                           FdoString*& name,
                                           FdoString*& value) const = 0;

    /// <summary>
    /// Gets number of parameters
    /// </summary>
    /// <returns>Returns the number of parameters.</returns>
    virtual int             GetParameterCount () const = 0;

    /// <summary>
    /// Sets the diagnostics parameter value.
    /// </summary>
    /// <param name="name">Output name of the parameter.</param>
    /// <param name="value">Value of the parameter.</param>
    /// <returns>Returns nothing.</returns>
    virtual void            SetParameter (FdoString *pName, FdoString *pValue) = 0;


    /// <summary>Checks whether EGwsStatus indicates failure condition.
    /// </summary>
    /// <param name="stat">Input status.</param>
    /// <returns>Returns true if stat is an error.</returns>
    static GWS_COMMON_API bool     IsError (EGwsStatus stat);

    /// <summary>Checks whether EGwsStatus indicates warning condition.
    /// </summary>
    /// <param name="stat">Input status.</param>
    /// <returns>Returns true if stat is a warning.</returns>
    static GWS_COMMON_API bool     IsWarning (EGwsStatus stat);

    /// <summary>Checks whether EGwsStatus indicates success.
    /// </summary>
    /// <param name="stat">Input status.</param>
    /// <returns>Returns true if stat is a success.</returns>
    static GWS_COMMON_API bool     IsSuccess (EGwsStatus stat);

    /// <summary>Gets status message. Caller provides buffer
    /// that is filled by the method.
    /// len specified the length of the caller buffer.
    /// </summary>
    /// <param name="stat">Input status.</param>
    /// <param name="msg">Input/output buffer for storing message.</param>
    /// <param name="stat">Input length of the buffer.</param>
    /// <returns>Returns pointer to the buffer. It is the
    /// same as input msg paramter.</returns>
    static GWS_COMMON_API wchar_t *GetStatusMessage (EGwsStatus stat,
                                              wchar_t * msg,
                                              int len);

protected:
    /// <summary>Necessary to initialize the base class properly.</summary>
    IGWSException(){}
    virtual ~IGWSException() {}

    virtual void Dispose(){}
};




/// <summary>
/// This interface represents a set of qualified names.
/// </summary>
class IGWSQualifiedNames : public IGWSObject
{
public:
    /// <summary>
    /// Create a new, empty collection of qualified names
    /// </summary>
    /// <returns>Returns a new empty collection.</returns>
    static GWS_COMMON_API IGWSQualifiedNames * Create ();

    /// <summary>
    /// Returns index of a qualified name in the collection
    /// Returns -1 if the qualified name was not found
    /// </summary>
    /// <returns>Index.</returns>
    virtual int IndexOf (const GWSQualifiedName & qname) const = 0;

    /// <summary>
    /// Insert a qualified name
    /// </summary>
    /// <param name="qname">Input qualified name to insert.</param>
    /// <returns>Returns true if successful otherwise returns false.</returns>
    virtual bool Insert(const GWSQualifiedName & qname) = 0;

    /// <summary>
    /// Get a qualified name by index
    /// </summary>
    /// <param name="index">Input index.</param>
    /// <returns>Qualified Name at index, or empty qualified name if not found</returns>
    virtual const GWSQualifiedName & Get (int index) const = 0;

    /// <summary>
    /// Remove a qualifed name
    /// </summary>
    /// <param name="qname">Input qualified name to remove.</param>
    /// <returns>true if successful</returns>
    virtual bool Remove(const GWSQualifiedName & qname) = 0;

    /// <summary>
    /// Remove a qualifed name by its index
    /// </summary>
    /// <param name="index">Input index.</param>
    /// <returns>true if successful</returns>

    virtual bool Remove(int index) = 0;
    /// <summary>
    /// Returns the number of names.
    /// </summary>
    /// <returns>Returns count of qualified names in the collection.</returns>

    virtual int Count() const = 0;
    /// <summary>
    /// Removes all qualified names
    /// </summary>
    /// <returns>Returns nothing.</returns>
    virtual void Clear() = 0;
};

/// <summary>
/// This interface represents a set of unique cache ids. It supports O(log(n))
/// performance for Contains() and Add(). Remove is O(log(n)) from the
/// end, O(n) in general. The Id() method executes in constant, O(1) time.
/// </summary>
class IGWSLongFeatureIdSet : public IGWSObject
{
public:
    /// <summary>
    /// Create a new, id set from a C++ array of longs.
    /// </summary>
    /// <param name="cacheIds">Input array of ids.</param>
    /// <param name="count">Input size of array of ids.</param>
    /// <returns>Returns new id set.</returns>
    static GWS_COMMON_API IGWSLongFeatureIdSet* Create(long* cacheIds, long count);

    /// <summary>
    /// Create a new, empty id set.
    /// </summary>
    /// <returns>Returns new id set.</returns>
    static GWS_COMMON_API IGWSLongFeatureIdSet* Create();

    /// <summary>
    /// Returns the size of the set.
    /// </summary>
    /// <returns>Returns size of the id set.</returns>
    virtual FdoInt32            Size() const = 0;

    /// <summary>
    /// Returns a boolean indicating whether or not the value is contained
    /// in the set.
    /// </summary>
    /// <param name="id">Input id.</param>
    /// <returns>Returns true if id is in the set.</returns>
    virtual bool                Contains(const long& id) const = 0;

    /// <summary>
    /// Adds a value to the set.
    /// </summary>
    /// <param name="id">Input id to add.</param>
    /// <returns>A boolean indicating the id was added to the set.</returns>
    virtual bool                Add(const long& id) = 0;

    /// <summary>
    /// Removes a value to the set.
    /// </summary>
    /// <param name="id">Input id to remove.</param>
    /// <returns>A boolean indicating the id was removed from the set.</returns>
    virtual bool                Remove(const long& id) = 0;

    /// <summary>
    /// Returns a value from the set give the specified 0-based index. May throw
    /// an exception if the index is out of the bounds of 0 to Size().
    /// </summary>
    /// <param name="index">Input index.</param>
    /// <returns>The value at that index.</returns>
    virtual const long&         Id(FdoInt32 index) = 0;

    /// <summary>
    /// Clears the contents of the set.
    /// </summary>
    /// <returns>Returns nothing.</returns>
    virtual void                Clear() = 0;
};


/// <summary>
/// This interface represents a set of unique feature ids. It supports O(log(n))
/// performance for Contains() and Add(). Remove is O(log(n)) from the
/// end, O(n) in general. The Id() method executes in constant, O(1) time.
/// </summary>
class IGWSFeatureIdSet : public IGWSObject
{
public:
    /// <summary>
    /// Create a new, empty id set.
    /// </summary>
    /// <returns>Returns new id set.</returns>
    static GWS_COMMON_API IGWSFeatureIdSet* Create();

    /// <summary>
    /// Returns the size of the set.
    /// </summary>
    /// <returns>Returns size of the id set.</returns>
    virtual FdoInt32            Size() const = 0;

    /// <summary>
    /// Returns a boolean indicating whether or not the value is contained
    /// in the set.
    /// </summary>
    /// <param name="id">Input id.</param>
    /// <returns>Returns true if id is in the set.</returns>
    virtual bool                Contains(const GWSFeatureId& id) const = 0;

    /// <summary>
    /// Adds a value to the set.
    /// </summary>
    /// <param name="id">Input id to add.</param>
    /// <returns>A boolean indicating the id was added to the set.</returns>
    virtual bool                Add(const GWSFeatureId& id) = 0;

    /// <summary>
    /// Removes a value to the set.
    /// </summary>
    /// <param name="id">Input id to remove.</param>
    /// <returns>A boolean indicating the id was removed from the set.</returns>
    virtual bool                Remove(const GWSFeatureId& id) = 0;

    /// <summary>
    /// Returns a value from the set give the specified 0-based index. May throw
    /// an exception if the index is out of the bounds of 0 to Size().
    /// </summary>
    /// <param name="index">Input index.</param>
    /// <returns>The value at that index.</returns>
    virtual const GWSFeatureId& Id(FdoInt32 index) = 0;

    /// <summary>
    /// Clears the contents of the set.
    /// </summary>
    /// <returns>Returns nothing.</returns>
    virtual void                Clear() = 0;
};


/// <summary>
/// This interface represents a coordinate system converter.
/// Object implementing this class must provide conversion
/// of the FdoGeometry specified as byte array from the source
/// coordinate system to the destination one.
/// </summary>
class IGWSCoordinateSystemConverter: public IGWSObject
{
public:
    /// <summary>
    /// Set the destination coordinate system name and type.
    /// </summary>
    /// <param name="dcsname">Destination coordinate system name.</param>
    /// <returns>Status</returns>
    virtual EGwsStatus    SetDestinationCS (const GWSCoordinateSystem & dcsname) = 0;

    /// <summary>
    /// Set the source coordinate system name and type.
    /// </summary>
    /// <param name="scsname">Source coordinate system name.</param>
    /// <returns>Status</returns>
    virtual EGwsStatus    SetSourceCS (const GWSCoordinateSystem & scsname) = 0;

    /// <summary>
    /// Get the destination coordinate system name.
    /// </summary>
    /// <returns>String</returns>
    virtual const GWSCoordinateSystem & DestinationCS    () = 0;

    /// <summary>
    /// Get the source coordinate system name.
    /// </summary>
    /// <returns>String</returns>
    virtual const GWSCoordinateSystem & SourceCS ()= 0;

    /// <summary>
    /// Convert geometry byte array from the source cs to the
    /// destination cs.
    /// Does not throw an exception but return error
    /// status if gemetry was not converted.
    /// </summary>
    /// <returns>EGwsStatus</returns>
    virtual EGwsStatus     ConvertForward(FdoByteArray *pGeom) = 0;


    /// <summary>
    /// Convert geometry byte bugger from the source cs to the
    /// destination cs.
    /// Does not throw an exception but return error
    /// status if gemetry was not converted.
    /// </summary>
    /// <returns>EGwsStatus</returns>
    virtual EGwsStatus     ConvertForward(FdoByte *pGeom, int nCount) = 0;


    /// <summary>
    /// Convert geometry byte array  from the destination
    /// cs to the source cs.
    /// Does not throw an exception but return error
    /// status if gemetry was not converted.
    /// </summary>
    /// <returns>EGwsStatus</returns>
    virtual EGwsStatus     ConvertBackward(FdoByteArray *pGeom) = 0;

    /// <summary>
    /// Convert geometry byte buffer  from the destination
    /// cs to the source cs.
    /// Does not throw an exception but return error
    /// status if gemetry was not converted.
    /// </summary>
    /// <returns>EGwsStatus</returns>
    virtual EGwsStatus     ConvertBackward(FdoByte *pGeom, int nCount) = 0;

    /// <summary>
    /// Checks whether coordinate system converter is initialized.
    /// Returns true if converter is initialized and false otherwise
    /// </summary>
    /// <returns>boolean</returns>
    virtual bool           IsInitialized () = 0;

#ifdef _CS_DEBUG
    virtual void           Clear () = 0;
    virtual long           GetNumPoints () = 0;

#endif

};


/// <summary>
/// This interface represents a coordinate system converter
/// factory.
/// The responsibility of the factory object is to create
/// coordinate system converter
/// </summary>
class IGWSCoordinateSystemConverterFactory: public IGWSObject
{
public:
    /// <summary>
    /// Create uninitilalized coordinate system converter.
    /// </summary>
    /// <param name="conveter">Resulting converter.</param>
    /// <returns>EGwsStatus</returns>
    virtual EGwsStatus      Create (IGWSCoordinateSystemConverter ** converter) = 0;

    /// <summary>
    /// Creates and initilalizes coordinate system converter.
    /// </summary>
    /// <param name="scsname">Source coordinate system name.</param>
    /// <param name="dcsname">Destination coordinate system name.</param>
    /// <param name="conveter">Resulting converter.</param>
    /// <returns>EGwsStatus</returns>
    virtual EGwsStatus      Create (const GWSCoordinateSystem & scsname,
                                    const GWSCoordinateSystem & dcsname,
                                    IGWSCoordinateSystemConverter ** converter) = 0;

    /// <summary>
    /// Converts the specified WKT string definition to an equivalent abbreviated name
    /// </summary>
    /// <param name="pWkt">WKT CS definition string.</param>
    /// <param name="pAbbr">Abbreviated CS name. In/Out parameter</param>
    /// <param name="abbrLen">Number of characters in the buffer pointed to by pAbbr</param>
    /// <returns>EGwsStatus</returns>
    virtual EGwsStatus      WktToAbbrev(FdoString* pWkt, wchar_t* pAbbr, int abbrLen) = 0;
};

/// <summary>
/// This object is returned when the client executes a certain GWS operations.
/// </summary>
class IGWSOperationHandler : public IGWSObject
{
public:

    /// <summary>
    /// Cancel the operation in progress. GWS may execute some operations in the
    /// working thread. In such case operation may be canceled.
    /// In some cases cancelation doesn't mean undoing incomplete results of the
    /// operation. Operation may be simply stopped and all incomplete results are
    /// preserved.
    /// </summary>
    /// <returns>Returns gws status. eGwsOk indicates that cancelation
    /// request has been accepted. In case if operation already
    /// completed, eGwsOperationInProgress is returned.
    /// </returns>
    virtual EGwsStatus    Cancel () = 0;

    /// <summary>
    /// Returns true if operation is in progress.
    /// This method returns false afte completion of the operation.
    /// </summary>
    /// <returns>boolean status.</returns>
    virtual bool          IsInProgress () = 0;


    /// <summary>
    /// Returns true if operation has been canceled via Cancel ().
    /// </summary>
    /// <returns>boolean cancel status.</returns>
    virtual bool          IsCanceled () = 0;


    /// <summary>
    /// Get the message indicating the progress of the operation
    /// processing.
    /// Caller must provider the pointer to a buffer and its length.
    /// Operation handler fills the buffer and returns pointer to it.
    /// </summary>
    /// <param name="msgbuff">Pointer to the buffer.</param>
    /// <param name="len">Length of the buffer.</param>
    /// <returns>Pointer to the message buffer.</returns>
    virtual wchar_t *     GetProgressMessage (wchar_t * msgbuff, int len) = 0;


    /// <summary>
    /// Returns the measure associated with the current progress indication
    /// returned by the GetProgressMessage () method.
    /// For example, if the progress message is "Writing features",
    /// the measure is the index of the feature being currently
    /// processed.
    /// 0 is returned when there is no measure or measure is unknown.
    /// </summary>
    /// <returns>long operation measure.</returns>
    virtual long          GetMeasure () = 0;

    /// <summary>
    /// Returns the scale associated with the current progress indication
    /// returned by the GetProgressMessage () method.
    /// For example, if the progress message is "Writing features",
    /// the scale is the number of features that will be written.
    /// 0 is returned when there is no scale or scale is unknown.
    /// </summary>
    /// <returns>long operation scale.</returns>
    virtual long          GetScale () = 0;

    /// <summary>
    /// Status may be checked after operation has been completed. Operation is
    /// completed when IsInProgress () returns false.
    /// The exception output parameter provides diagnostics in case there is a
    /// failure or a warning.
    /// </summary>
    /// <param name="exception">Output diagnostic exception if any</param>
    /// <returns>Returns completion status.eGwsOperationInProgress is
    /// returned if operation is still active.</returns>
    virtual EGwsStatus    GetCompletionStatus (IGWSException ** exception) = 0;


    /// <summary>
    /// Set the progress message. Operation handler sets the progress
    /// message. Client may set its own/
    /// </summary>
    virtual void          SetProgressMessage (const wchar_t * msgbuff) = 0;

    /// <summary>
    /// Set the measure. Operation handler sets the measure.
    /// Client may set its own.
    /// </summary>
    virtual void          SetMeasure         (long measure) = 0;

    /// <summary>
    /// Set the scale. Operation handler sets the scale.
    /// Client may set its own.
    /// </summary>
    virtual void          SetScale           (long scale) = 0;

};


/// <summary>
// Useful Fdo related methods
/// </summary>
namespace GWSFdoUtilities
{
    /// <summary>
    /// Compares Fdo Data values. Returns negative value if the
    /// first value is less than the second one, 0, if they are equal,
    /// and positive value otherwise
    /// </summary>
    GWS_COMMON_API
    int                     CompareDataValues (FdoDataValue * val1,
                                               FdoDataValue *val2);

    /// <summary>
    // returns true if data types are comparable
    /// </summary>
    GWS_COMMON_API
    bool                    IsComparableDataTypes (
                                               FdoDataPropertyDefinition * val1,
                                               FdoDataPropertyDefinition * val2);
};





#endif /* GWSCOMMON_EXPORTS */

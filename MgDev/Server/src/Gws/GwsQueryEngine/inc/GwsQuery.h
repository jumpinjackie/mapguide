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

#ifndef CGwsQuery_h
#define CGwsQuery_h


///////////////////////////////////////////////////////////////////////////////
//
// forward declarations
//
///////////////////////////////////////////////////////////////////////////////

class FdoClassDefinition;
class GwsBinaryFeatureWriter;

// query class hierachy
class CGwsFeatureSourceQuery;

class CGwsPreparedQuery;
    class CGwsPreparedFeatureQuery;
    class CGwsPreparedLeftJoinQuery;
    class CGwsPreparedEqualJoinQuery;

// query results (iterators) class hierarchy
class CGwsFeatureIterator;
    class CGwsJoinQueryResults;
        class CGwsSortMergeJoinQueryResults;
    class CGwsRightJoinQueryResults;
        class CGwsRightSortedJoinQueryResults;
    class CGwsMultiSelectIterator; // could wrap all of the above


///////////////////////////////////////////////////////////////////////////////
//
// typedefs
//
///////////////////////////////////////////////////////////////////////////////

/// <summary>
/// feature status in cache
/// <summary>
typedef enum __GwsCacheFeatureStatus
{
    eGwsUnknownFeature  = 0x00,
    eGwsNewFeature      = 0x01,
    eGwsModifiedFeature = 0x02,
    eGwsDeletedFeature  = 0x04
} EGwsCacheFeatureStatus;


typedef enum _EGwsCursorPosition
{
    eBeforeFirstRow,
    eOnJoinRow,
    eAfterJoinRow,
    eAfterLastRow
} EGwsCursorPosition;


// join results type
typedef enum _EJoinResultsType
{
    kGwsSortMergeResults = 1,
    kGwsNestedLoopSortBlockResults,
    kGwsNestedLoopResults,
    kGwsBatchSortBlockResults,
    kGwsHashResults
} EJoinResultsType;


// join methods
typedef enum _EGwsJoinMethod
{
    eGwsSortMerge = 1,
    eGwsNestedLoops,
    eGwsNestedLoopSortedBlock,
    eGwsBatchSortedBlock,
    eGwsHash
} EGwsJoinMethod;


typedef enum _EGwsIteratorType
{
    eGwsFeatureIterator = 1,
    eGwsSortMergeJoinIterator,
    eGwsNestedLoopsIterator,
    eGwsNestedLoopSortedBlockIterator,
    eGwsBatchSortedBlockIterator,
    eGwsRightSortMergeJoinIterator,
    eGwsRightNestedLoopsIterator,
    eGwsRightNestedLoopSortedBlockIterator,
    eGwsRightBatchSortedBlockIterator
} EGwsFeatureIteratorType;


// prepared query type. In can be either against cache or
// underlying feature source
typedef enum _EGwsPreparedQueryType
{
    eGwsPreparedFeatureQuery = 1,
    eGwsPreparedCacheQuery
} EGwsPreparedQueryType;


///////////////////////////////////////////////////////////////////////////////
//
// Property descriptor. The simplified representation of
// FdoPropertyDefintion
//
///////////////////////////////////////////////////////////////////////////////
class GWS_QUERYENGINE_API CGwsPropertyDesc
{
public:
    CGwsPropertyDesc (
                    const WSTR &    name,
                    FdoPropertyType ptype,
                    FdoDataType     dataprop,
                    bool            nullable,
                    int             length,
                    int             precision,
                    int             scale,
                    bool            haselevation,
                    bool            hasmeasure,
                    int             geometrytypes,
                    bool            bReadOnly,
                    bool            bIsComputedIdentifier,
                    const WSTR  &   spatialcontext);

    CGwsPropertyDesc (FdoPropertyDefinition * propdef);

    CGwsPropertyDesc (const CGwsPropertyDesc & other) ;

    void operator= (const CGwsPropertyDesc & other);

public:
    WSTR            m_name;     // unqualified name
    FdoPropertyType m_ptype;
    FdoDataType     m_dataprop;
    bool            m_nullable;
    int             m_length;
    int             m_precision;
    int             m_scale;
    bool            m_haselevation;
    bool            m_hasmeasure;
    int             m_geometrytypes;
    bool            m_bReadOnly;
    bool            m_bIsComputedIdentifier;
    WSTR            m_spatialcontext;
};


///////////////////////////////////////////////////////////////////////////////
//
// implementation of IGWSQueryResultDescriptors
//
///////////////////////////////////////////////////////////////////////////////
class CGwsQueryResultDescriptors : public GWSObject<IGWSExtendedFeatureDescription>
{
friend class CGwsQueryResultDescriptor;
public:
    GWS_QUERYENGINE_API             CGwsQueryResultDescriptors ();
    GWS_QUERYENGINE_API             CGwsQueryResultDescriptors (
                                           FdoClassDefinition     * classDef,
                                            const GWSQualifiedName & classname,
                                            const FdoString        * joinName,
                                            const FdoString        * joinDelimiter,
                                            bool                     forceOneToOne,
                                            FdoIdentifierCollection    * propnames,
                                            bool                   leftOuterJoin = false);
    GWS_QUERYENGINE_API             CGwsQueryResultDescriptors (const CGwsQueryResultDescriptors & other);
    GWS_QUERYENGINE_API
    virtual                         ~CGwsQueryResultDescriptors () throw();

    // interface implementation
    GWS_QUERYENGINE_API
    virtual const GWSQualifiedName & ClassName ();
    GWS_QUERYENGINE_API
    virtual FdoClassDefinition     * ClassDefinition ();
    GWS_QUERYENGINE_API
    virtual const GWSCoordinateSystem & GetCoordinateSystem ();
    GWS_QUERYENGINE_API
    virtual int                      GetCount  ();
    GWS_QUERYENGINE_API
    virtual IGWSExtendedFeatureDescription * GetItem (int i);
    GWS_QUERYENGINE_API
    virtual IGWSExtendedFeatureDescription * GetItem (const GWSQualifiedName & name);
    GWS_QUERYENGINE_API
    virtual FdoStringCollection    * PropertyNames ();
    GWS_QUERYENGINE_API
    virtual int                      Contains (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual FdoPropertyDefinition  * FindPropertyDefinition (FdoString* propertyName);

    // adding descriptor
    GWS_QUERYENGINE_API
    void                             Add (IGWSExtendedFeatureDescription * pDesc);

    GWS_QUERYENGINE_API
    const CGwsPropertyDesc &         GetPropertyDescriptor (int idx);
    GWS_QUERYENGINE_API
    const CGwsPropertyDesc &         GetPropertyDescriptor (FdoString * name);
    GWS_QUERYENGINE_API
    const CGwsPropertyDesc &         GetGeometryPropertyDescriptor ();

    GWS_QUERYENGINE_API
    const std::vector<CGwsPropertyDesc> & GetPropertyDescriptors ();
    GWS_QUERYENGINE_API
    FdoDataPropertyDefinitionCollection *GetIdentityProperties ();

    GWS_QUERYENGINE_API
    const WSTR &                     GetSuffix () const;
    GWS_QUERYENGINE_API
    void                             SetSuffix (const WSTR & suffix);
    GWS_QUERYENGINE_API
    void                             SetCSName (const GWSCoordinateSystem & csname);

    virtual bool                    ForceOneToOneJoin() { return m_forceOneToOne; }
    virtual bool                    LeftOuterJoin() { return m_leftOuterJoin; };

protected:
     void                           appendPropertyNames (
                                        FdoIdentifierCollection     * propnamestoadd,
                                        FdoClassDefinition          * fdoclass,
                                        FdoStringCollection         * propnames,
                                        std::vector<CGwsPropertyDesc> & propdsc);
    void                            pushPropDefinition (
                                        FdoPropertyDefinition       * propdef,
                                        FdoStringCollection         * propnames,
                                        std::vector<CGwsPropertyDesc> & propdsc);
    virtual const FdoString*        JoinName() { return m_joinName.empty() ? NULL : m_joinName.c_str(); }
    virtual const FdoString*        JoinDelimiter() { return m_joinDelimiter.empty() ? NULL : m_joinDelimiter.c_str(); }

protected:
    // primary feature description
    FdoPtr<FdoClassDefinition>          m_classDef;
    GWSQualifiedName                    m_classname;
    std::vector<CGwsPropertyDesc>       m_propdsc;
    FdoPtr<FdoStringCollection>         m_propertynames;
    FdoPtr<FdoDataPropertyDefinitionCollection> m_identprops;

    // join name
    WSTR                                m_joinName;
    WSTR                                m_joinDelimiter;
    bool                                m_forceOneToOne;
    bool                                m_leftOuterJoin;

    // joined properties
    std::vector<IGWSExtendedFeatureDescription*>
                                        m_descriptors;
    // query results. It is used to generate cached data class name
    WSTR                                m_suffix;

    // coordinate system of the geometric property
    GWSCoordinateSystem                 m_csname;
};

typedef enum _GwsExtendedSelProvider { eSDF, eSHP, eCGFI } EGwsExtendedSelProvider;


///////////////////////////////////////////////////////////////////////////////
//
// class CGwsPreparedQuery
//
///////////////////////////////////////////////////////////////////////////////
class GWS_QUERYENGINE_API CGwsPreparedQuery
{
public:
                            CGwsPreparedQuery ();
                            CGwsPreparedQuery (bool bSdfExtendedQuery);
    virtual                 ~CGwsPreparedQuery ();
    virtual EGwsQueryType   QueryType () const = 0;
    virtual void            DescribeResults (IGWSExtendedFeatureDescription ** ppResDesc);

    virtual EGwsStatus      Execute (IGWSFeatureIterator   ** results, bool bScrollable = false) = 0;

    virtual EGwsStatus      Execute (FdoFilter * filter,
                                     IGWSFeatureIterator ** results,
                                     bool bScrollable = false) = 0;

    virtual EGwsStatus      Execute (const GWSFeatureId & featid,
                                     IGWSFeatureIterator ** results) = 0;

    virtual EGwsStatus      Execute (const GwsFeaturesIdVector & featids,
                                     int lbound,
                                     int ubound,
                                     IGWSFeatureIterator ** results,
                                     bool bScrollable = false) = 0;

    virtual EGwsStatus      SetFilter (FdoFilter * filter) = 0;
    virtual FdoFilter *     GetFilter () = 0;

    // returns primary prepared query (left hand query for the join and
    // itself in feature query case)
    virtual CGwsPreparedFeatureQuery *
                            GetPrimaryQuery () = 0;
    // returns identity properties for the primary query
    virtual FdoDataPropertyDefinitionCollection * GetIdentityProperties () = 0;

    virtual CGwsFeatureIterator *
                            CreateFeatureIterator (EGwsFeatureIteratorType);

    virtual EGwsPreparedQueryType
                            GetPreparedQueryType ();

public:
    int                     GetPathLength () {return m_pathlength;}
    void                    GetQueryDefinition (IGWSQueryDefinition ** ppQdef);

protected:
    CGwsQueryResultDescriptors * m_resultDescriptor;
    CGwsFeatureSourceQuery     * m_pQuery;

    // path length to the primary join feature
    int                          m_pathlength;

    // boolean to determine if the query is using the sdf extended scrollable
    // reader
    bool                         m_bExtendedQuerySupported;
    EGwsExtendedSelProvider      mExSelProv;
};


///////////////////////////////////////////////////////////////////////////////
//
// class CGwsPreparedFeatureQuery
//
///////////////////////////////////////////////////////////////////////////////
class  CGwsPreparedFeatureQuery: public CGwsPreparedQuery,
                                 public CGwsFdoCommand
{
friend class CGwsFeatureIterator;
friend class CGwsFeatureSourceQuery;
public:
    GWS_QUERYENGINE_API     CGwsPreparedFeatureQuery (
                                                      FdoIConnection         * session,
                                                      const GWSQualifiedName & classname,
                                                      CGwsFeatureSourceQuery * pQuery);
    GWS_QUERYENGINE_API
    virtual                 ~CGwsPreparedFeatureQuery ();

    GWS_QUERYENGINE_API
    virtual CGwsFdoCommand::EFdoCommandType Type () const
    {
        return CGwsFdoCommand::eSelect;
    }

    GWS_QUERYENGINE_API
    virtual EGwsQueryType   QueryType () const {
        return eGwsQueryFeature;
    }
    GWS_QUERYENGINE_API
    EGwsStatus              Init (FdoIdentifierCollection * sellist,
                                  FdoStringCollection * orderBy,
                                  FdoOrderingOption     orderingOption,
                                  FdoFilter           * filter);

    GWS_QUERYENGINE_API
    virtual void            PrepareInternal ();

    GWS_QUERYENGINE_API
    virtual void            SetFilterInternal (FdoFilter * filter);


    GWS_QUERYENGINE_API
    virtual EGwsStatus      Execute (IGWSFeatureIterator ** results, bool bScrollable = false);

    GWS_QUERYENGINE_API
    virtual EGwsStatus      Execute (FdoFilter * filter,
                                     IGWSFeatureIterator ** results,
                                     bool bScrollable = false);
    GWS_QUERYENGINE_API
    virtual EGwsStatus      Execute (const GWSFeatureId & featid,
                                     IGWSFeatureIterator ** results);
    GWS_QUERYENGINE_API
    virtual EGwsStatus      Execute (const GwsFeaturesIdVector & featids,
                                     int lbound,
                                     int ubound,
                                     IGWSFeatureIterator ** results,
                                     bool bScrollable = false);
    GWS_QUERYENGINE_API
    virtual EGwsStatus      SetFilter (FdoFilter * filter);
    GWS_QUERYENGINE_API
    virtual FdoFilter    *  GetFilter ();

    GWS_QUERYENGINE_API
    virtual void            SetRealignRectangleFilter (bool bRealign) { m_bIsAxisAlignedRectangleFilter = bRealign; }

    GWS_QUERYENGINE_API
    virtual CGwsPreparedFeatureQuery *  GetPrimaryQuery () ;
    GWS_QUERYENGINE_API
    virtual FdoDataPropertyDefinitionCollection * GetIdentityProperties ();
    GWS_QUERYENGINE_API
    virtual FdoStringCollection *  GetOrderBy ();
    GWS_QUERYENGINE_API
    virtual FdoOrderingOption      GetOrderingOption ();

protected:
    FdoPtr<FdoIdentifierCollection> m_selectList;
    bool                    m_bIsSelectDistinct;
    bool                    m_bIsAxisAlignedRectangleFilter;
};



///////////////////////////////////////////////////////////////////////////////
//
// class CGwsPreparedJoinQuery
//
///////////////////////////////////////////////////////////////////////////////
class CGwsPreparedJoinQuery: public CGwsPreparedQuery,
                             public CGwsObject
{
public:
    GWS_QUERYENGINE_API      CGwsPreparedJoinQuery (EGwsJoinMethod             joinmethod,
                                                   CGwsPreparedQuery        * lpq,
                                                   CGwsPreparedQuery        * rpq,
                                                   FdoStringCollection      * lcols,
                                                   FdoStringCollection      * rcols,
                                                   CGwsFeatureSourceQuery   * query);

    GWS_QUERYENGINE_API
    virtual                 ~CGwsPreparedJoinQuery ();

    CGwsPreparedQuery   *   LeftQuery () {return m_leftquery;}
    CGwsPreparedQuery   *   RightQuery () {return m_rightquery;}

    FdoStringCollection *   LeftProperties ()
    {
        if (m_leftCols != NULL)
            m_leftCols.p->AddRef ();
        return m_leftCols;
    }
    FdoStringCollection *   RightProperties ()
    {
        if (m_rightCols != NULL)
            m_rightCols.p->AddRef ();
        return m_rightCols;
    }

    GWS_QUERYENGINE_API
    virtual EGwsJoinMethod  JoinMethod () const { return m_joinmethod; }

    GWS_QUERYENGINE_API
    virtual EGwsStatus      Execute (IGWSFeatureIterator ** results, bool bScrollable = false);

    GWS_QUERYENGINE_API
    virtual EGwsStatus      Execute (FdoFilter * filter,
                                     IGWSFeatureIterator ** results,
                                     bool bScrollable = false);
    GWS_QUERYENGINE_API
    virtual EGwsStatus      Execute (const GWSFeatureId & featid,
                                     IGWSFeatureIterator ** results);
    GWS_QUERYENGINE_API
    virtual EGwsStatus      Execute (const GwsFeaturesIdVector & featids,
                                     int lbound,
                                     int ubound,
                                     IGWSFeatureIterator ** results,
                                     bool bScrollable = false);

    GWS_QUERYENGINE_API
    virtual EGwsStatus      SetFilter (FdoFilter * filter);
    GWS_QUERYENGINE_API
    virtual FdoFilter *     GetFilter ();

    GWS_QUERYENGINE_API
    EGwsStatus              Init     ();
    GWS_QUERYENGINE_API
    virtual CGwsPreparedFeatureQuery * GetPrimaryQuery () ;

    GWS_QUERYENGINE_API
    virtual FdoDataPropertyDefinitionCollection *
                            GetIdentityProperties ();

protected:
    CGwsPreparedQuery         * m_leftquery;
    CGwsPreparedQuery         * m_rightquery;
    FdoPtr<FdoStringCollection> m_leftCols;
    FdoPtr<FdoStringCollection> m_rightCols;
    EGwsJoinMethod              m_joinmethod;
};


///////////////////////////////////////////////////////////////////////////////
//
// class CGwsPreparedLeftJoinQuery
//
///////////////////////////////////////////////////////////////////////////////
class CGwsPreparedLeftJoinQuery: public CGwsPreparedJoinQuery
{
public:
    GWS_QUERYENGINE_API     CGwsPreparedLeftJoinQuery (EGwsJoinMethod             joinmethod,
                                                       CGwsPreparedQuery        * lpq,
                                                       CGwsPreparedQuery        * rpq,
                                                       FdoStringCollection      * lcols,
                                                       FdoStringCollection      * rcols,
                                                       CGwsFeatureSourceQuery   * query);

    GWS_QUERYENGINE_API
    virtual                 ~CGwsPreparedLeftJoinQuery ();

    GWS_QUERYENGINE_API
    virtual EGwsQueryType   QueryType () const
    {
        return eGwsQueryLeftOuterJoin;
    }
};


///////////////////////////////////////////////////////////////////////////////
//
// class CGwsPreparedEqualJoinQuery
//
///////////////////////////////////////////////////////////////////////////////
class CGwsPreparedEqualJoinQuery: public CGwsPreparedJoinQuery
{
public:
    GWS_QUERYENGINE_API     CGwsPreparedEqualJoinQuery (EGwsJoinMethod             joinmethod,
                                                       CGwsPreparedQuery        * lpq,
                                                       CGwsPreparedQuery        * rpq,
                                                       FdoStringCollection      * lcols,
                                                       FdoStringCollection      * rcols,
                                                       CGwsFeatureSourceQuery   * query);

    GWS_QUERYENGINE_API
    virtual                 ~CGwsPreparedEqualJoinQuery ();

    GWS_QUERYENGINE_API
    virtual EGwsQueryType   QueryType () const
    {
        return eGwsQueryEqualJoin;
    }
};


///////////////////////////////////////////////////////////////////////////////
//
// class CGwsFeatureSourceQuery
// implementation of IGWSQuery for the feature source
//
///////////////////////////////////////////////////////////////////////////////
class CGwsFeatureSourceQuery : public GWSObject<IGWSQuery>,
                               public CGwsObject
{
public:
    GWS_QUERYENGINE_API     CGwsFeatureSourceQuery ();
    GWS_QUERYENGINE_API
    virtual                 ~CGwsFeatureSourceQuery () throw();

    // IGWSQuery implementation
    GWS_QUERYENGINE_API
    virtual void            GetQueryDefinition (IGWSQueryDefinition ** ppQdef);
    GWS_QUERYENGINE_API
    virtual void            DescribeResults (IGWSExtendedFeatureDescription ** ppResDesc);
    GWS_QUERYENGINE_API
    virtual void            SetFilter (FdoFilter * filter);
    GWS_QUERYENGINE_API
    virtual FdoFilter *     GetFilter ();
    GWS_QUERYENGINE_API
    virtual void            Prepare ();
    GWS_QUERYENGINE_API
    virtual void            Execute (IGWSFeatureIterator ** results, bool bScrollable = false);
    GWS_QUERYENGINE_API
    virtual void            Execute (FdoFilter * filter,
                                     IGWSFeatureIterator ** results);
    GWS_QUERYENGINE_API
    virtual void            Execute (const GWSFeatureId & featid,
                                     IGWSFeatureIterator ** results);
    GWS_QUERYENGINE_API
    virtual void            Execute (const GwsFeaturesIdVector & featids,
                                     int lbound,
                                     int ubound,
                                     IGWSFeatureIterator ** results);

    GWS_QUERYENGINE_API
    virtual const WSTR   &  GetRevisionPropertyName ();

    GWS_QUERYENGINE_API
    virtual void            SetDestinationCS (const GWSCoordinateSystem & csname);

    GWS_QUERYENGINE_API
    virtual const GWSCoordinateSystem & GetDestinationCS ();

    GWS_QUERYENGINE_API
    virtual void            SetSourceCS (const GWSCoordinateSystem & csname);

    GWS_QUERYENGINE_API
    virtual const GWSCoordinateSystem &  GetSourceCS ();

    GWS_QUERYENGINE_API
    virtual void            SetCSFactory (IGWSCoordinateSystemConverterFactory * csfactory);
    GWS_QUERYENGINE_API
    virtual IGWSCoordinateSystemConverterFactory * GetCSFactory ();

public:
    GWS_QUERYENGINE_API
    virtual void            Initialize (IGWSConnectionPool  * pool,
                                         IGWSQueryDefinition * qrydef);

    IGWSConnectionPool*     GetConnectionPool ();

    GWS_QUERYENGINE_API
    virtual CGwsPreparedFeatureQuery * CreatePreparedFeatureQuery (
                                            FdoIConnection         * sess,
                                            const GWSQualifiedName & classname);

    GWS_QUERYENGINE_API
    virtual CGwsPreparedJoinQuery    * CreatePreparedJoinQuery(
                        EGwsQueryType              jtype,
                        EGwsJoinMethod             joinmethod,
                        CGwsPreparedQuery        * lpq,
                        CGwsPreparedQuery        * rpq,
                        FdoStringCollection      * lcols,
                        FdoStringCollection      * rcols);

    GWS_QUERYENGINE_API
    virtual CGwsPreparedQuery * Prepare (
                        IGWSQueryDefinition * pQryDef,
                        const WSTR          & suffix);
    GWS_QUERYENGINE_API
    virtual CGwsPreparedFeatureQuery * PrepareFeatureQuery (
                        IGWSFeatureQueryDefinition * pQryDef,
                        FdoStringCollection        * orderCols,
                        FdoOrderingOption            orderingOption,
                        const WSTR                 & suffix,
                        FdoClassDefinition         * pClassDef = NULL);
    GWS_QUERYENGINE_API
    virtual CGwsPreparedJoinQuery * PrepareJoinQuery (
                                            IGWSJoinQueryDefinition   * pQryDef,
                                            const WSTR                  & suffix);

    // validate join attributes
    void                               ValidateJoinAttributes (
                                            FdoPropertyDefinition * lpdef,
                                            FdoPropertyDefinition * rpdef,
                                            FdoString             * lname,
                                            FdoString             * rname);

    // returns the primary (left hand) prepared query
    GWS_QUERYENGINE_API
    CGwsPreparedFeatureQuery *          GetPrimaryPreparedQuery ();

    // return prepared query
    CGwsPreparedQuery *                 GetPreparedQuery () {return m_pQuery; }

    GWS_QUERYENGINE_API
    void                                SetClassDefinition(FdoClassDefinition* pClassDef);

protected:
    FdoPtr<IGWSConnectionPool>   m_connectionpool;
    FdoPtr<IGWSQueryDefinition>  m_qrydef;
    CGwsPreparedQuery        *   m_pQuery;   // prepared query
    WSTR                         m_revprop;

    // coordinate systems

    // factory
    FdoPtr<IGWSCoordinateSystemConverterFactory> m_csfactory;
    // destination coordinate system
    GWSCoordinateSystem          m_csname;
    // source coordinate system override
    GWSCoordinateSystem          m_srccsname;
    // primary class definition - optimization
    FdoPtr<FdoClassDefinition>  m_classDef;
};


///////////////////////////////////////////////////////////////////////////////
//
// class CGwsFeatureIterator
// implementation of IGWSFeatureIterator for the feature source
//
///////////////////////////////////////////////////////////////////////////////
class CGwsFeatureIterator : public GWSObject<IGWSFeatureIterator>,
                            public CGwsObject
{
public:
    GWS_QUERYENGINE_API     CGwsFeatureIterator ();
    GWS_QUERYENGINE_API
    virtual                 ~CGwsFeatureIterator () throw();

    GWS_QUERYENGINE_API
    virtual EGwsStatus      InitializeReader (
                                    FdoIFeatureReader               * pReader,
                                    IGWSQuery                       * fquery,
                                    CGwsPreparedFeatureQuery        * prepquery,
                                    bool                            bIsScrollable);

    // this method is used in the non-cached case in order to expose
    // integer feature ids as cached id
    GWS_QUERYENGINE_API
    virtual void            SetExposeFeatureIdAsCacheId (bool bFlag);

    GWS_QUERYENGINE_API
    virtual void            DescribeFeature(IGWSExtendedFeatureDescription ** ppResDesc);
    GWS_QUERYENGINE_API
    virtual bool            NextFeature (IGWSFeature ** feature);
    GWS_QUERYENGINE_API
    virtual bool            ReadNext();
    GWS_QUERYENGINE_API
    virtual void            Close ();
    GWS_QUERYENGINE_API
    virtual FdoInt32        GetCacheId ();
    GWS_QUERYENGINE_API
    virtual FdoInt32        GetRevisionNumber ();
    GWS_QUERYENGINE_API
    virtual GWSFeatureId    GetFeatureId ();
    GWS_QUERYENGINE_API
    virtual bool            IsNew ();
    GWS_QUERYENGINE_API
    virtual bool            IsModified ();
    GWS_QUERYENGINE_API
    virtual bool            IsDeleted ();
    GWS_QUERYENGINE_API
    virtual EGwsLockType    GetCacheLockType ();
    GWS_QUERYENGINE_API
    virtual FdoString*      GetLayerSource (){ return m_strLayerSource.c_str(); }
    GWS_QUERYENGINE_API
    virtual void            SetLayerSource (FdoString* strLayerSource){ m_strLayerSource = strLayerSource; }
    GWS_QUERYENGINE_API
    virtual bool            ExposeFeatureIdAsCacheId() { return m_bExposeFeatureIdAsCacheId; }

    GWS_QUERYENGINE_API
    virtual IGWSFeatureIterator* GetJoinedFeatures (int i);

    // getters by the property name
    GWS_QUERYENGINE_API
    virtual bool            IsNull      (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual FdoString   *   GetString   (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual bool            GetBoolean  (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual FdoByte         GetByte     (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual FdoDateTime     GetDateTime (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual double          GetDouble   (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual FdoInt16        GetInt16    (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual FdoInt32        GetInt32    (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual FdoInt64        GetInt64    (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual float           GetSingle   (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual FdoLOBValue*    GetLOB      (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual FdoIStreamReader* GetLOBStreamReader(const wchar_t* propertyName );
    GWS_QUERYENGINE_API
    virtual FdoIRaster*     GetRaster   (FdoString* propertyName);

    GWS_QUERYENGINE_API
    virtual FdoInt32        GetDepth    ();
    GWS_QUERYENGINE_API
    virtual const FdoByte * GetGeometry (FdoString* propertyName, FdoInt32 * count) ;
    GWS_QUERYENGINE_API
    virtual FdoByteArray*   GetGeometry (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual FdoIFeatureReader* GetFeatureObject(FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual FdoDataValue *  GetDataValue (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual FdoDataValueCollection *
        GetDataValues (FdoStringCollection* propertyNames);

    // getters by the property index
    GWS_QUERYENGINE_API
    virtual bool            IsNull      (FdoInt32 index);
    GWS_QUERYENGINE_API
    virtual FdoString   *   GetString   (FdoInt32 index);
    GWS_QUERYENGINE_API
    virtual bool            GetBoolean  (FdoInt32 index);
    GWS_QUERYENGINE_API
    virtual FdoByte         GetByte     (FdoInt32 index);
    GWS_QUERYENGINE_API
    virtual FdoDateTime     GetDateTime (FdoInt32 index);
    GWS_QUERYENGINE_API
    virtual double          GetDouble   (FdoInt32 index);
    GWS_QUERYENGINE_API
    virtual FdoInt16        GetInt16    (FdoInt32 index);
    GWS_QUERYENGINE_API
    virtual FdoInt32        GetInt32    (FdoInt32 index);
    GWS_QUERYENGINE_API
    virtual FdoInt64        GetInt64    (FdoInt32 index);
    GWS_QUERYENGINE_API
    virtual float           GetSingle   (FdoInt32 index);
    GWS_QUERYENGINE_API
    virtual FdoLOBValue*    GetLOB      (FdoInt32 index);
    GWS_QUERYENGINE_API
    virtual FdoIStreamReader* GetLOBStreamReader(FdoInt32 index);
    GWS_QUERYENGINE_API
    virtual FdoIRaster*     GetRaster   (FdoInt32 index);
    GWS_QUERYENGINE_API
    virtual const FdoByte * GetGeometry (FdoInt32 index, FdoInt32 * count) ;
    GWS_QUERYENGINE_API
    virtual FdoByteArray*   GetGeometry (FdoInt32 index);
    GWS_QUERYENGINE_API
    virtual FdoIFeatureReader* GetFeatureObject(FdoInt32 index);

    // newly added methods to FdoIFeatureReader
    GWS_QUERYENGINE_API
    virtual FdoString*      GetPropertyName(FdoInt32 index);
    GWS_QUERYENGINE_API
    virtual FdoInt32        GetPropertyIndex(FdoString* propertyName);

    // returns unconverted geometry. Coordinate system transaformations are not applied
    GWS_QUERYENGINE_API
    virtual FdoByteArray*   GetOriginalGeometry (FdoString* propertyName);

    // returns true if geometry id being converted
    GWS_QUERYENGINE_API
    virtual bool            ConvertingGeometry ();

    GWS_QUERYENGINE_API
    virtual void            ToString    (FdoString* propertyName, wchar_t * buff, int len);
    GWS_QUERYENGINE_API
    virtual unsigned char*  ToBuffer(int& bufLen);

    GWS_QUERYENGINE_API
    virtual const GWSCoordinateSystem & GetCSName ();
    GWS_QUERYENGINE_API
    virtual FdoClassDefinition* GetClassDefinition();
    GWS_QUERYENGINE_API
    virtual FdoGeometryType GetGeometryType(FdoByteArray* pArray);
    GWS_QUERYENGINE_API
    virtual FdoString*      GetPrimaryGeometryName();
    // get data property value
    GWS_QUERYENGINE_API
    virtual FdoDataValue *  GetPropertyValue (const CGwsPropertyDesc &);
    GWS_QUERYENGINE_API
    virtual IGWSMutableFeature *    GetSimpleFeature ();
    GWS_QUERYENGINE_API
    virtual bool            InitializeMutableFeature ();

    GWS_QUERYENGINE_API
    virtual CGwsPreparedQuery* PrepQuery() { return m_prepquery; }

    // SdfIScrollableFeatureReader implementation
    GWS_QUERYENGINE_API
    virtual int             Count();
    GWS_QUERYENGINE_API
    virtual bool            ReadFirst();
    GWS_QUERYENGINE_API
    virtual bool            ReadLast();
    GWS_QUERYENGINE_API
    virtual bool            ReadPrevious();
    GWS_QUERYENGINE_API
    virtual bool            ReadAt(FdoPropertyValueCollection* key);
    GWS_QUERYENGINE_API
    virtual bool            ReadAtIndex( unsigned int recordindex );
    GWS_QUERYENGINE_API
    virtual unsigned int    IndexOf(FdoPropertyValueCollection* key);

    GWS_QUERYENGINE_API
    virtual bool            Scrollable();

    GWS_QUERYENGINE_API
    virtual FdoInt32        ResolveJoinIdVector(std::vector<FdoInt32>& idVec) { return GetCacheId(); }

    GWS_QUERYENGINE_API
        virtual bool                ReadAtCacheId(FdoInt32 cacheId) { return false; }
    GWS_QUERYENGINE_API
    virtual std::vector<FdoInt32>* GetJoinIdVector(FdoInt32 jCacheId) { return NULL; }

public:
    GWS_QUERYENGINE_API
    virtual const CGwsPropertyDesc &
                            GetPropertyDescriptor (int iProp);
    GWS_QUERYENGINE_API
    virtual const CGwsPropertyDesc &
                            GetPropertyDescriptor (FdoString * propname);
    GWS_QUERYENGINE_API
    virtual IGWSFeatureIterator*    GetPrimaryIterator();

protected:
    // throw an exception if m_reader is null. m_reader maybe
    // null in some join cases
    void                    CheckReader ();

protected:
    FdoPtr<FdoIFeatureReader>              m_reader;

    // tracks whether the reader is scrollable or not
    bool                                   m_bScrollableReader;

    // reference to query
    FdoPtr<IGWSQuery>                      m_query;

    // reference to prepared query that
    // is used to create iterator
    CGwsPreparedQuery                   *  m_prepquery;

    // this guy lives in prepared query, but I want to keep
    // it here. This will save us some steps, since
    // conversion should only happen at the lowest level,
    // when geometry is gotten from Fdo.
    // In the join case, CGwsFeatureIterator wraps another
    // CGwsFeatureIterator and we don't need to do conversion
    // in this case.
    FdoPtr<IGWSCoordinateSystemConverter>  m_converter;

    FdoPtr<IGWSMutableFeature>             m_mutableFeature;
    bool                                   m_bMutableFeatureSet;
    std::wstring                           m_strLayerSource;
    bool                                   m_bExposeFeatureIdAsCacheId;
    FdoDataType                            m_cacheIdType;
    std::wstring                           m_idname;

    // flag indicating that geometry has been already converted
    bool                                   m_bGeometryConverted;
    GwsBinaryFeatureWriter*                m_pBinaryWriter;
    EGwsExtendedSelProvider                m_extProviderType;
};


///////////////////////////////////////////////////////////////////////////////
//
// class CGwsJoinQueryResults
// Join results iterator. (left part)
//
///////////////////////////////////////////////////////////////////////////////
class CGwsJoinQueryResults : public CGwsFeatureIterator
{
public:
    GWS_QUERYENGINE_API         CGwsJoinQueryResults   ();
    GWS_QUERYENGINE_API
    virtual                     ~CGwsJoinQueryResults  () throw();

    GWS_QUERYENGINE_API
    virtual EGwsStatus          InitializeReader (
                                    FdoStringCollection   * leftjoincols,
                                    IGWSQuery             * query,
                                    CGwsPreparedQuery     * prepquery,
                                    bool                  bScrollable);

    GWS_QUERYENGINE_API
    virtual EGwsStatus          InitializeReader (
                                    IGWSQuery             * query,
                                    CGwsPreparedJoinQuery * prepquery,
                                    bool bScrollable = false) = 0;

    GWS_QUERYENGINE_API
    virtual bool                ReadNext ();

    GWS_QUERYENGINE_API
    virtual IGWSFeatureIterator *  GetJoinedFeatures (int i);

    // get right hand side joined features
    IGWSFeatureIterator *       GetJoinedFeatures ();

    GWS_QUERYENGINE_API
    virtual FdoInt32            GetRevisionNumber ();
    GWS_QUERYENGINE_API
    virtual GWSFeatureId        GetFeatureId      ();
    GWS_QUERYENGINE_API
    virtual void                Close ();

    GWS_QUERYENGINE_API
    virtual EJoinResultsType    Type () const = 0;

    virtual FdoDataValueCollection *    GetJoinValues ();

    GWS_QUERYENGINE_API
    bool                        CacheReadNext ();

    GWS_QUERYENGINE_API
    virtual FdoDataValueCollection * GetDataValues (FdoStringCollection* propertyNames);

    // SdfIScrollableFeatureReader implementation
    GWS_QUERYENGINE_API
    virtual int             Count();
    GWS_QUERYENGINE_API
    virtual bool            ReadAtIndex( unsigned int recordindex );
    GWS_QUERYENGINE_API
    virtual bool            Scrollable();

protected:
    CGwsRightJoinQueryResults             * m_right;
    FdoPtr<FdoStringCollection>             m_leftcols;
    GWSFeatureId                            m_leftJoinVals;
    bool                                    m_bLeftJoinValuesSet;
    bool                                    m_started;
    bool                                    m_forceOneToOne;
    bool    SetupRightSide(bool leftResult);
};


///////////////////////////////////////////////////////////////////////////////
//
// class CGwsRightJoinQueryResults
// Right side iterator
//
///////////////////////////////////////////////////////////////////////////////
class CGwsRightJoinQueryResults : public CGwsFeatureIterator
{
public:

    GWS_QUERYENGINE_API          CGwsRightJoinQueryResults   ();
    GWS_QUERYENGINE_API
    virtual                     ~CGwsRightJoinQueryResults  () throw();

    GWS_QUERYENGINE_API
    virtual EGwsStatus          InitializeReader (
                                    IGWSQuery           * query,
                                    FdoIFeatureReader   * reader,
                                    FdoStringCollection * joincols,
                                    bool                bScrollable);
    GWS_QUERYENGINE_API
    virtual EGwsStatus          InitializeReader (
                                    IGWSQuery           * query,
                                    CGwsPreparedQuery   * prepquery,
                                    FdoStringCollection * joincols,
                                    bool bScrollable);

    GWS_QUERYENGINE_API
    void                        SetNeverUsePooling ();

    GWS_QUERYENGINE_API
    virtual EGwsStatus          SetRelatedValues (const GWSFeatureId & vals);
    GWS_QUERYENGINE_API
    virtual FdoInt32            GetRevisionNumber ();
    GWS_QUERYENGINE_API
    virtual GWSFeatureId        GetFeatureId ();
    GWS_QUERYENGINE_API
    virtual void                DescribeFeature(IGWSExtendedFeatureDescription ** ppResDesc);
    GWS_QUERYENGINE_API
    virtual IGWSFeatureIterator* GetJoinedFeatures (int i);

    // getters by the property name
    GWS_QUERYENGINE_API
    virtual bool                IsNull      (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual FdoString   *       GetString   (FdoString * propname);
    GWS_QUERYENGINE_API
    virtual bool                GetBoolean  (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual FdoByte             GetByte     (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual FdoDateTime         GetDateTime (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual double              GetDouble   (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual FdoInt16            GetInt16    (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual FdoInt32            GetInt32    (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual FdoInt64            GetInt64    (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual float               GetSingle   (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual FdoLOBValue*        GetLOB      (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual FdoIStreamReader*   GetLOBStreamReader(const wchar_t* propertyName );
    GWS_QUERYENGINE_API
    virtual FdoIRaster*         GetRaster   (FdoString* propertyName);

    GWS_QUERYENGINE_API
    virtual const FdoByte *     GetGeometry (FdoString* propertyName, FdoInt32 * count) ;
    GWS_QUERYENGINE_API
    virtual FdoByteArray*       GetGeometry (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual FdoByteArray *      GetOriginalGeometry (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual bool                ConvertingGeometry ();
    GWS_QUERYENGINE_API
    virtual FdoIFeatureReader*  GetFeatureObject(FdoString* propertyName);

    GWS_QUERYENGINE_API
    virtual FdoDataValue *      GetDataValue (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual FdoDataValueCollection * GetDataValues (FdoStringCollection* propertyNames);

    GWS_QUERYENGINE_API
    virtual void                ToString    (FdoString* propertyName, wchar_t * buff, int len);

    GWS_QUERYENGINE_API
    virtual FdoStringCollection* GetJoinColumns();

protected:
    IGWSFeature *               GetPooledFeature ();

protected:
    FdoPtr<FdoStringCollection>
                            m_joincols;
    GWSFeatureId            m_joinkeys;
    GWSFeatureId            m_joinvals;

    // pooled features for a current join key
    CGwsFeaturesPool    *   m_pool;
    int                     m_poolpos;
    bool                    m_usepool;

    bool                    m_neverusepooling;
};


///////////////////////////////////////////////////////////////////////////////
//
// class CGwsSortMergeJoinQueryResults
// Left join results iterator. (Sort merge)
//
///////////////////////////////////////////////////////////////////////////////
class CGwsSortMergeJoinQueryResults : public CGwsJoinQueryResults
{
friend class CGwsPreparedFeatureQuery;
friend class CGwsPreparedLeftJoinQuery;
friend class CGwsPreparedEqualJoinQuery;
public:
    GWS_QUERYENGINE_API         CGwsSortMergeJoinQueryResults  ();
    GWS_QUERYENGINE_API
    virtual                 ~CGwsSortMergeJoinQueryResults () throw();

    GWS_QUERYENGINE_API
    virtual EGwsStatus      InitializeReader (
                                    IGWSQuery                       * query,
                                    CGwsPreparedJoinQuery           * prepquery,
                                    bool                            bScrollable = false);

    GWS_QUERYENGINE_API
    virtual EJoinResultsType Type () const
    {
        return kGwsSortMergeResults;
    }
};


///////////////////////////////////////////////////////////////////////////////
//
// class CGwsNestedLoopsJoinQueryResults
// Left join results iterator. (nested loops)
//
///////////////////////////////////////////////////////////////////////////////
class CGwsNestedLoopsJoinQueryResults : public CGwsJoinQueryResults
{
friend class CGwsPreparedFeatureQuery;
friend class CGwsPreparedLeftJoinQuery;
friend class CGwsPreparedEqualJoinQuery;
public:
    GWS_QUERYENGINE_API         CGwsNestedLoopsJoinQueryResults  ();
    GWS_QUERYENGINE_API
    virtual                     ~CGwsNestedLoopsJoinQueryResults () throw();

    GWS_QUERYENGINE_API
    virtual EGwsStatus          InitializeReader (
                IGWSQuery                       * query,
                CGwsPreparedJoinQuery           * prepquery,
                bool                            bScrollable = false);
    GWS_QUERYENGINE_API
    virtual EJoinResultsType    Type () const
    {
        return kGwsNestedLoopResults;
    }
};


///////////////////////////////////////////////////////////////////////////////
//
// class CGwsNestedLoopSortedBlock
// Left join results iterator. (nested loops sorted block)
//
///////////////////////////////////////////////////////////////////////////////
class CGwsNestedLoopSortedBlockJoinQueryResults : public CGwsJoinQueryResults
{
friend class CGwsPreparedFeatureQuery;
friend class CGwsPreparedLeftJoinQuery;
friend class CGwsPreparedEqualJoinQuery;
friend class CGwsPreparedJoinQuery;
public:
    GWS_QUERYENGINE_API     CGwsNestedLoopSortedBlockJoinQueryResults  ();
    GWS_QUERYENGINE_API
    virtual                 ~CGwsNestedLoopSortedBlockJoinQueryResults () throw();

    GWS_QUERYENGINE_API
    virtual EGwsStatus      InitializeReader (
                                IGWSQuery                       * query,
                                CGwsPreparedJoinQuery           * prepquery,
                                bool                            bScrollable = false);
    GWS_QUERYENGINE_API
    virtual EJoinResultsType Type () const
    {
        return kGwsNestedLoopSortBlockResults;
    }
};


typedef struct
{
    FdoPtr<FdoByteArray> geometry;
    FdoPtr<FdoDataValue> dataValue;
} PropertyCacheEntry;

typedef std::vector< CGwsPropertyDesc* > PropertyDescriptionCollection;
typedef std::vector< PropertyCacheEntry* > PropertyCollection;

typedef struct
{
    PropertyCollection propertyCollection;
    FdoPtr<FdoDataValue> primaryKey;
    bool bUsed;
} PrimaryCacheEntry;


///////////////////////////////////////////////////////////////////////////////
//
// class CGwsBatchSortedBlock
// Left join results iterator. (Batch sorted block)
//
///////////////////////////////////////////////////////////////////////////////
class CGwsBatchSortedBlockJoinQueryResults : public CGwsJoinQueryResults
{
friend class CGwsPreparedFeatureQuery;
friend class CGwsPreparedLeftJoinQuery;
friend class CGwsPreparedEqualJoinQuery;
friend class CGwsPreparedJoinQuery;
public:
    GWS_QUERYENGINE_API     CGwsBatchSortedBlockJoinQueryResults  ();
    GWS_QUERYENGINE_API
    virtual                 ~CGwsBatchSortedBlockJoinQueryResults () throw();

    GWS_QUERYENGINE_API
    virtual EGwsStatus      InitializeReader (
                                IGWSQuery                       * query,
                                CGwsPreparedJoinQuery           * prepquery,
                                bool                            bScrollable = false);
    GWS_QUERYENGINE_API
    virtual EJoinResultsType Type () const
    {
        return kGwsBatchSortBlockResults;
    }

    GWS_QUERYENGINE_API
    virtual bool                ReadNext ();

    GWS_QUERYENGINE_API
    virtual IGWSFeatureIterator *  GetJoinedFeatures (int i);

    // get right hand side joined features
    IGWSFeatureIterator *       GetJoinedFeatures ();

    virtual FdoDataValueCollection *    GetJoinValues ();

    // Getters by the property name - this will come from the cache
    GWS_QUERYENGINE_API
    virtual bool            IsNull      (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual FdoString   *   GetString   (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual bool            GetBoolean  (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual FdoByte         GetByte     (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual FdoDateTime     GetDateTime (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual double          GetDouble   (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual FdoInt16        GetInt16    (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual FdoInt32        GetInt32    (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual FdoInt64        GetInt64    (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual float           GetSingle   (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual FdoLOBValue*    GetLOB      (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual FdoIStreamReader* GetLOBStreamReader(const wchar_t* propertyName );
    GWS_QUERYENGINE_API
    virtual FdoIRaster*     GetRaster   (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual const FdoByte * GetGeometry (FdoString* propertyName, FdoInt32 * count) ;
    GWS_QUERYENGINE_API
    virtual FdoByteArray*   GetGeometry (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual FdoIFeatureReader* GetFeatureObject(FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual FdoDataValue *  GetDataValue (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual FdoDataValueCollection *
        GetDataValues (FdoStringCollection* propertyNames);

    // returns unconverted geometry. Coordinate system transaformations are not applied
    GWS_QUERYENGINE_API
    virtual FdoByteArray*   GetOriginalGeometry (FdoString* propertyName);

    // get data property value
    GWS_QUERYENGINE_API
    virtual FdoDataValue *  GetPropertyValue (const CGwsPropertyDesc &);

    GWS_QUERYENGINE_API
    static int sm_nBatchSize;

protected:
    bool    SetupBatchRightSide(bool leftResult);
    void    ClearIteratorCache();
    void    ResetCache();
    void QuickSort(std::vector<PrimaryCacheEntry*>& cache, FdoInt32 left, FdoInt32 right);
    bool QuickSortCompare(PrimaryCacheEntry* compareA, PrimaryCacheEntry* compareB);
    void ShowPrimaryCache();

    IGWSFeatureIterator* m_primaryFeatureIterator;

    FdoPtr<FdoStringCollection> m_propertyNames;
    PropertyDescriptionCollection m_propertyDescriptionCollection;

    // Primary iterator cache
    std::vector<PrimaryCacheEntry*> m_pPrimaryCache;
    size_t m_primaryCacheIndex;
    size_t m_nEntriesUpdated;
};


///////////////////////////////////////////////////////////////////////////////
//
// class CGwsRightSortedJoinQueryResults
// ordered right side of the joined query results
//
///////////////////////////////////////////////////////////////////////////////
class CGwsRightSortedJoinQueryResults : public CGwsRightJoinQueryResults
{
friend class CGwsPreparedFeatureQuery;
friend class CGwsPreparedLeftJoinQuery;
friend class CGwsPreparedEqualJoinQuery;
friend class CGwsPreparedJoinQuery;
public:
    GWS_QUERYENGINE_API         CGwsRightSortedJoinQueryResults ();
    GWS_QUERYENGINE_API
    virtual                     ~CGwsRightSortedJoinQueryResults () throw();

    GWS_QUERYENGINE_API
    virtual EGwsStatus          InitializeReader (
                                    IGWSQuery           * query,
                                    FdoIFeatureReader   * reader,
                                    FdoStringCollection * joincols,
                                    bool                bScrollable);

    GWS_QUERYENGINE_API
    virtual bool                ReadNext();
    GWS_QUERYENGINE_API
    virtual void                Close ();
    GWS_QUERYENGINE_API
    virtual EGwsStatus          SetRelatedValues (const GWSFeatureId & vals);

protected:
    EGwsCursorPosition      m_pos;
};


///////////////////////////////////////////////////////////////////////////////
//
// class CGwsRightNestedLoopJoinQueryResults
//
// join nested loop right side
//
///////////////////////////////////////////////////////////////////////////////
class CGwsRightNestedLoopJoinQueryResults : public CGwsRightJoinQueryResults
{
friend class CGwsPreparedFeatureQuery;
friend class CGwsPreparedLeftJoinQuery;
friend class CGwsPreparedEqualJoinQuery;
friend class CGwsPreparedJoinQuery;
public:
    GWS_QUERYENGINE_API         CGwsRightNestedLoopJoinQueryResults ();
    GWS_QUERYENGINE_API
    virtual                     ~CGwsRightNestedLoopJoinQueryResults () throw();

    GWS_QUERYENGINE_API
    virtual EGwsStatus          InitializeReader  (
                                    IGWSQuery                  * query,
                                    CGwsPreparedQuery          * prepquery,
                                    FdoStringCollection        * joincols,
                                    bool                        bScrollable);

    GWS_QUERYENGINE_API
    virtual bool                ReadNext();
    GWS_QUERYENGINE_API
    virtual void                Close ();
    GWS_QUERYENGINE_API
    virtual EGwsStatus          SetRelatedValues (const GWSFeatureId & vals);

protected:
    bool                    m_bClosed; // closed flag
};


///////////////////////////////////////////////////////////////////////////////
//
// class CGwsRightNestedLoopSortedBlockJoinQueryResults
//
// join nested loop sorted block right side
//
///////////////////////////////////////////////////////////////////////////////
class CGwsRightNestedLoopSortedBlockJoinQueryResults
                    : public CGwsRightNestedLoopJoinQueryResults
{
friend class CGwsPreparedFeatureQuery;
friend class CGwsPreparedLeftJoinQuery;
friend class CGwsPreparedEqualJoinQuery;
friend class CGwsPreparedJoinQuery;
public:
    GWS_QUERYENGINE_API         CGwsRightNestedLoopSortedBlockJoinQueryResults ();
    GWS_QUERYENGINE_API
    virtual                     ~CGwsRightNestedLoopSortedBlockJoinQueryResults () throw();


    GWS_QUERYENGINE_API
    virtual EGwsStatus          InitializeReader (
                                    IGWSQuery                  * query,
                                    CGwsPreparedQuery          * prepquery,
                                    FdoStringCollection        * joincols,
                                    bool                        bScrollable);

    GWS_QUERYENGINE_API
    virtual bool                ReadNext();
    GWS_QUERYENGINE_API
    virtual void                Close ();
    GWS_QUERYENGINE_API
    virtual EGwsStatus          SetRelatedValues (const GWSFeatureId & vals);
};


///////////////////////////////////////////////////////////////////////////////
//
// class CGwsRightBatchSortedBlockJoinQueryResults
//
// join batch sorted block right side
//
///////////////////////////////////////////////////////////////////////////////
class CGwsRightBatchSortedBlockJoinQueryResults
                    : public CGwsRightNestedLoopJoinQueryResults
{
friend class CGwsPreparedFeatureQuery;
friend class CGwsPreparedLeftJoinQuery;
friend class CGwsPreparedEqualJoinQuery;
friend class CGwsPreparedJoinQuery;
public:
    GWS_QUERYENGINE_API         CGwsRightBatchSortedBlockJoinQueryResults ();
    GWS_QUERYENGINE_API
    virtual                     ~CGwsRightBatchSortedBlockJoinQueryResults () throw();


    GWS_QUERYENGINE_API
    virtual EGwsStatus          InitializeReader (
                                    IGWSQuery                  * query,
                                    CGwsPreparedQuery          * prepquery,
                                    FdoStringCollection        * joincols,
                                    bool                        bScrollable);

    GWS_QUERYENGINE_API
    virtual bool                ReadNext();
    GWS_QUERYENGINE_API
    virtual void                Close ();
    GWS_QUERYENGINE_API
    virtual EGwsStatus          SetRelatedValues (const GWSFeatureId & vals);

    GWS_QUERYENGINE_API
    virtual bool            IsNull      (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual FdoString   *   GetString   (FdoString * propname);
    GWS_QUERYENGINE_API
    virtual bool            GetBoolean  (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual FdoByte         GetByte     (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual FdoDateTime     GetDateTime (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual double          GetDouble   (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual FdoInt16        GetInt16    (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual FdoInt32        GetInt32    (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual FdoInt64        GetInt64    (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual float           GetSingle   (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual FdoLOBValue*    GetLOB      (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual FdoIStreamReader* GetLOBStreamReader(const wchar_t* propertyName );
    GWS_QUERYENGINE_API
    virtual FdoIRaster*     GetRaster   (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual const FdoByte * GetGeometry (FdoString* propertyName, FdoInt32 * count) ;
    GWS_QUERYENGINE_API
    virtual FdoByteArray*   GetGeometry (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual FdoIFeatureReader* GetFeatureObject(FdoString* propertyName);

    GWS_QUERYENGINE_API
    virtual bool            IsNull      (FdoInt32 index);
    GWS_QUERYENGINE_API
    virtual FdoString   *   GetString   (FdoInt32 index);
    GWS_QUERYENGINE_API
    virtual bool            GetBoolean  (FdoInt32 index);
    GWS_QUERYENGINE_API
    virtual FdoByte         GetByte     (FdoInt32 index);
    GWS_QUERYENGINE_API
    virtual FdoDateTime     GetDateTime (FdoInt32 index);
    GWS_QUERYENGINE_API
    virtual double          GetDouble   (FdoInt32 index);
    GWS_QUERYENGINE_API
    virtual FdoInt16        GetInt16    (FdoInt32 index);
    GWS_QUERYENGINE_API
    virtual FdoInt32        GetInt32    (FdoInt32 index);
    GWS_QUERYENGINE_API
    virtual FdoInt64        GetInt64    (FdoInt32 index);
    GWS_QUERYENGINE_API
    virtual float           GetSingle   (FdoInt32 index);
    GWS_QUERYENGINE_API
    virtual FdoLOBValue*    GetLOB      (FdoInt32 index);
    GWS_QUERYENGINE_API
    virtual FdoIStreamReader* GetLOBStreamReader(FdoInt32 index);
    GWS_QUERYENGINE_API
    virtual FdoIRaster*     GetRaster   (FdoInt32 index);
    GWS_QUERYENGINE_API
    virtual const FdoByte * GetGeometry (FdoInt32 index, FdoInt32 * count) ;
    GWS_QUERYENGINE_API
    virtual FdoByteArray*   GetGeometry (FdoInt32 index);
    GWS_QUERYENGINE_API
    virtual FdoIFeatureReader* GetFeatureObject(FdoInt32 index);

    GWS_QUERYENGINE_API
    virtual FdoDataValue *  GetDataValue (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual FdoByteArray*   GetOriginalGeometry (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual FdoClassDefinition* GetClassDefinition();

    void                        IncrementKeyIndex()     { m_joinKeyIndex++; }
    void                        ResetKeyIndex()         { m_joinKeyIndex=0; }
    void                        SetPrimaryKeyNull(bool bValue)  { m_bPrimaryKeyNull=bValue; }

protected:
    FdoDataValue*               GetSecondaryDataValue(FdoDataType dtSecondary, FdoString* propname);

    EGwsCursorPosition      m_pos;
    int                     m_joinKeyIndex;
    bool                    m_bMoreData;
    bool                    m_bNullEntry;
    bool                    m_bPrimaryKeyNull;
    EGwsQueryType           m_queryType;
    bool                    m_bOneToOneJoin;
    FdoDataType             m_dtSecondary;
    FdoString*              m_propName;
    bool                    m_bFirstReadNext;
};


///////////////////////////////////////////////////////////////////////////////
//
// class CGwsMultiSelectIterator
// Wrapper, implemeting multiple selects during iteration
// Must be able to wrap all iterator types
//
///////////////////////////////////////////////////////////////////////////////
class CGwsMultiSelectIterator : public CGwsFeatureIterator
{
public:
    GWS_QUERYENGINE_API     CGwsMultiSelectIterator ();
    GWS_QUERYENGINE_API
    virtual                 ~CGwsMultiSelectIterator () throw();

    // Unexposed and asserting. Shouldn't be called
    //
    virtual EGwsStatus      InitializeReader (
                                    FdoIFeatureReader               * pReader,
                                    IGWSQuery                       * fquery,
                                    CGwsPreparedFeatureQuery        * prepquery,
                                    bool                            bScrollable);

    // This one must be used instead
    //
    GWS_QUERYENGINE_API
    EGwsStatus              InitializeReader (
                                    IGWSQuery                       * fquery,
                                    FdoFilter                       * pLayerFilter,
                                    const std::vector<FdoPtr<FdoFilter> > & filterSet);

    // Call member iterator, and if it is finished, execute next query
    //
    GWS_QUERYENGINE_API
    virtual bool            ReadNext();
    GWS_QUERYENGINE_API
    virtual bool            NextFeature (IGWSFeature ** feature);

    // Overloaded CGwsFeatureIterator methods, changing members and propagating them
    // to the member iterator
    //
    GWS_QUERYENGINE_API
    virtual void            SetExposeFeatureIdAsCacheId (bool bFlag);
    GWS_QUERYENGINE_API
    virtual void            SetLayerSource (FdoString* strLayerSource);

    // Only non-overloaded method. Must be OK as it is
    //
//    GWS_QUERYENGINE_API
//    virtual FdoString*      GetLayerSource (){ return m_strLayerSource.c_str(); }

    // Overloaded CGwsFeatureIterator methods, simply readdressing the calls to member iterator
    //
    GWS_QUERYENGINE_API
    virtual void            DescribeFeature(IGWSExtendedFeatureDescription ** ppResDesc);
    GWS_QUERYENGINE_API
    virtual void            Close ();
    GWS_QUERYENGINE_API
    virtual FdoInt32        GetCacheId ();
    GWS_QUERYENGINE_API
    virtual FdoInt32        GetRevisionNumber ();
    GWS_QUERYENGINE_API
    virtual GWSFeatureId    GetFeatureId ();
    GWS_QUERYENGINE_API
    virtual bool            IsNew ();
    GWS_QUERYENGINE_API
    virtual bool            IsModified ();
    GWS_QUERYENGINE_API
    virtual bool            IsDeleted ();
    GWS_QUERYENGINE_API
    virtual EGwsLockType    GetCacheLockType ();
    GWS_QUERYENGINE_API
    virtual IGWSFeatureIterator* GetJoinedFeatures (int i);

    GWS_QUERYENGINE_API
    virtual FdoString*      GetPropertyName(FdoInt32 index);
    GWS_QUERYENGINE_API
    virtual FdoInt32        GetPropertyIndex(FdoString* propertyName);

    GWS_QUERYENGINE_API
    virtual bool            IsNull      (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual FdoString   *   GetString   (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual bool            GetBoolean  (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual FdoByte         GetByte     (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual FdoDateTime     GetDateTime (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual double          GetDouble   (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual FdoInt16        GetInt16    (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual FdoInt32        GetInt32    (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual FdoInt64        GetInt64    (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual float           GetSingle   (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual FdoLOBValue*    GetLOB      (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual FdoIStreamReader* GetLOBStreamReader(const wchar_t* propertyName );
    GWS_QUERYENGINE_API
    virtual FdoIRaster*     GetRaster   (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual FdoInt32        GetDepth    ();
    GWS_QUERYENGINE_API
    virtual const FdoByte * GetGeometry (FdoString* propertyName, FdoInt32 * count) ;
    GWS_QUERYENGINE_API
    virtual FdoByteArray*   GetGeometry (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual FdoIFeatureReader* GetFeatureObject(FdoString* propertyName);

    GWS_QUERYENGINE_API
    virtual bool            IsNull      (FdoInt32 index);
    GWS_QUERYENGINE_API
    virtual FdoString   *   GetString   (FdoInt32 index);
    GWS_QUERYENGINE_API
    virtual bool            GetBoolean  (FdoInt32 index);
    GWS_QUERYENGINE_API
    virtual FdoByte         GetByte     (FdoInt32 index);
    GWS_QUERYENGINE_API
    virtual FdoDateTime     GetDateTime (FdoInt32 index);
    GWS_QUERYENGINE_API
    virtual double          GetDouble   (FdoInt32 index);
    GWS_QUERYENGINE_API
    virtual FdoInt16        GetInt16    (FdoInt32 index);
    GWS_QUERYENGINE_API
    virtual FdoInt32        GetInt32    (FdoInt32 index);
    GWS_QUERYENGINE_API
    virtual FdoInt64        GetInt64    (FdoInt32 index);
    GWS_QUERYENGINE_API
    virtual float           GetSingle   (FdoInt32 index);
    GWS_QUERYENGINE_API
    virtual FdoLOBValue*    GetLOB      (FdoInt32 index);
    GWS_QUERYENGINE_API
    virtual FdoIStreamReader* GetLOBStreamReader(FdoInt32 index);
    GWS_QUERYENGINE_API
    virtual FdoIRaster*     GetRaster   (FdoInt32 index);
    GWS_QUERYENGINE_API
    virtual const FdoByte * GetGeometry (FdoInt32 index, FdoInt32 * count) ;
    GWS_QUERYENGINE_API
    virtual FdoByteArray*   GetGeometry (FdoInt32 index);
    GWS_QUERYENGINE_API
    virtual FdoIFeatureReader* GetFeatureObject(FdoInt32 index);

    GWS_QUERYENGINE_API
    virtual FdoDataValue *  GetDataValue (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual FdoDataValueCollection * GetDataValues (FdoStringCollection* propertyNames);
    GWS_QUERYENGINE_API
    virtual FdoByteArray*   GetOriginalGeometry (FdoString* propertyName);
    GWS_QUERYENGINE_API
    virtual bool            ConvertingGeometry ();
    GWS_QUERYENGINE_API
    virtual void            ToString    (FdoString* propertyName, wchar_t * buff, int len);
    GWS_QUERYENGINE_API
    virtual unsigned char*  ToBuffer(int& bufLen);
    GWS_QUERYENGINE_API
    virtual const GWSCoordinateSystem & GetCSName ();
    GWS_QUERYENGINE_API
    virtual FdoClassDefinition* GetClassDefinition();
    GWS_QUERYENGINE_API
    virtual FdoGeometryType GetGeometryType(FdoByteArray* pArray);
    GWS_QUERYENGINE_API
    virtual FdoString*      GetPrimaryGeometryName();
    GWS_QUERYENGINE_API
    virtual FdoDataValue *  GetPropertyValue (const CGwsPropertyDesc &);
    GWS_QUERYENGINE_API
    virtual bool            InitializeMutableFeature ();
    GWS_QUERYENGINE_API
    virtual const CGwsPropertyDesc &
                            GetPropertyDescriptor (int iProp);
    GWS_QUERYENGINE_API
    virtual const CGwsPropertyDesc &
                            GetPropertyDescriptor (FdoString * propname);

    GWS_QUERYENGINE_API
    virtual IGWSMutableFeature *    GetSimpleFeature ();

    GWS_QUERYENGINE_API
    virtual int             Count();
    GWS_QUERYENGINE_API
    virtual bool            ReadFirst();
    GWS_QUERYENGINE_API
    virtual bool            ReadLast();
    GWS_QUERYENGINE_API
    virtual bool            ReadPrevious();
    GWS_QUERYENGINE_API
    virtual bool            ReadAt(FdoPropertyValueCollection* key);
    GWS_QUERYENGINE_API
    virtual bool            ReadAtIndex( unsigned int recordindex );
    GWS_QUERYENGINE_API
    virtual unsigned int    IndexOf(FdoPropertyValueCollection* key);

    GWS_QUERYENGINE_API
    virtual bool            Scrollable();

    GWS_QUERYENGINE_API
    virtual IGWSFeatureIterator*    GetPrimaryIterator();

protected:
    // throws an exception if m_iter is null.
    void                    CheckIterator ();

    // Executes next query
    EGwsStatus              Execute ();

protected:
    FdoPtr<CGwsFeatureIterator>            m_iter;
    unsigned int                           m_queryIdx;
    FdoPtr<FdoFilter>                      m_layerFilter;
    std::vector<FdoPtr<FdoFilter> >        m_filterSet;
};

#endif

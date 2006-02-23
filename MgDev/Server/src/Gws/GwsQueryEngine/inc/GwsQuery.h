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

#ifndef CGwsQuery_h
#define CGwsQuery_h

/////////////////////////////////////////////////////////////////////////////////
//
// forward declarations
//
///////////////////////////////////////////////////////////////////////////////
class FdoClassDefinition;

// query class hierachy
class CGwsFeatureSourceQuery;

class CGwsPreparedQuery;
    class CGwsPreparedFeatureQuery;
    class CGwsPreparedLeftJoinQuery;
    class CGwsPreparedEqualJoinQuery;

// query reults (iterators) class hierarchy
class CGwsFeatureIterator;
    class CGwsJoinQueryResults;
        class CGwsSortMergeJoinQueryResults;
    class CGwsRightJoinQueryResults;
        class CGwsRightSortedJoinQueryResults;


/////////////////////////////////////////////////////////////////////////////////
//
// typedefs
//
///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// feature status in cache
/// <summary>
typedef enum __GwsCacheFeatureStatus {
    eGwsUnknownFeature  = 0x00,
    eGwsNewFeature      = 0x01,
    eGwsModifiedFeature = 0x02,
    eGwsDeletedFeature  = 0x04
} EGwsCacheFeatureStatus;



/////////////////////////////////////////////////////////////////////////////////
//
// defines
//
///////////////////////////////////////////////////////////////////////////////


// join results type
typedef enum _EJoinResultsType
{
    kGwsSortMergeResults = 1,
    kGwsNestedLoopSortBlockResults,
    kGwsNestedLoopResults,
    kGwsHashResults
} EJoinResultsType;

     // join methods
typedef enum _EGwsJoinMethod {
    eGwsSortMerge = 1,
    eGwsNestedLoops,
    eGwsNestedLoopSortedBlock,
    eGwsHash
} EGwsJoinMethod;

typedef enum _EGwsIteratorType
{
    eGwsFeatureIterator = 1,
    eGwsSortMergeJoinIterator,
    eGwsNestedLoopsIterator,
    eGwsNestedLoopSortedBlockIterator,
    eGwsRightSortMergeJoinIterator,
    eGwsRightNestedLoopsIterator,
    eGwsRightNestedLoopSortedBlockIterator

} EGwsFeatureIteratorType;

// prepared query type. In can be either against cache or
// underlying feature source
typedef enum _EGwsPreparedQueryType {
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
    WSTR            m_spatialcontext;
};


///////////////////////////////////////////////////////////////////////////////
//
// implelemtation of IGWSQueryResultDescriptors
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
                                            GisStringCollection    * propnames);
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
    virtual GisStringCollection    * PropertyNames ();
    GWS_QUERYENGINE_API
    virtual int                      Contains (GisString* propertyName);
    GWS_QUERYENGINE_API
    virtual FdoPropertyDefinition  * FindPropertyDefinition (GisString* propertyName);

    // adding descriptor
    GWS_QUERYENGINE_API
    void                             Add (IGWSExtendedFeatureDescription * pDesc);

    GWS_QUERYENGINE_API
    const CGwsPropertyDesc &         GetPropertyDescriptor (int idx);
    GWS_QUERYENGINE_API
    const CGwsPropertyDesc &         GetPropertyDescriptor (GisString * name);
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

protected:
     void                           appendPropertyNames (
                                        GisStringCollection         * propnamestoadd,
                                        FdoClassDefinition          * fdoclass,
                                        GisStringCollection         * propnames,
                                        std::vector<CGwsPropertyDesc> & propdsc);
    void                            pushPropDefinition (
                                        FdoPropertyDefinition       * propdef,
                                        GisStringCollection         * propnames,
                                        std::vector<CGwsPropertyDesc> & propdsc);

protected:
    // primary feature description
    GisPtr<FdoClassDefinition>          m_classDef;
    GWSQualifiedName                    m_classname;
    std::vector<CGwsPropertyDesc>       m_propdsc;
    GisPtr<GisStringCollection>         m_propertynames;
    GisPtr<FdoDataPropertyDefinitionCollection> m_identprops;

    // joined properties
    std::vector<IGWSExtendedFeatureDescription*>
                                        m_descriptors;
    // suffix defines the position of the description in the joined
    // query results. It is used to genratate cached data class name
    WSTR                                m_suffix;

    // coordinate system of the geometric property
    GWSCoordinateSystem                 m_csname;
};



///////////////////////////////////////////////////////////////////////////////
//
// class CGwsPreparedQuery
//
///////////////////////////////////////////////////////////////////////////////
class GWS_QUERYENGINE_API CGwsPreparedQuery
{
public:
                            CGwsPreparedQuery ();
    virtual                 ~CGwsPreparedQuery ();
    virtual EGwsQueryType   QueryType () const = 0;
    virtual void            DescribeResults (IGWSExtendedFeatureDescription ** ppResDesc);
    virtual EGwsStatus      Execute (IGWSFeatureIterator   ** results) = 0;
    virtual EGwsStatus      Execute (FdoFilter * filter,
                                     IGWSFeatureIterator ** results) = 0;
    virtual EGwsStatus      Execute (const GWSFeatureId & featid,
                                     IGWSFeatureIterator ** results) = 0;
    virtual EGwsStatus      Execute (const GwsFeaturesIdVector & featids,
                                     int lbound,
                                     int ubound,
                                     IGWSFeatureIterator ** results) = 0;
    virtual EGwsStatus      SetFilter (FdoFilter * filter) = 0;
    // returns primary prepared query (left hand query for the join and
    // inself in feature query case)
    virtual CGwsPreparedFeatureQuery *
                            GetPrimaryQuery () = 0;
    // returns identity propertrties for the primary query
    virtual FdoDataPropertyDefinitionCollection * GetIdentityProperties () = 0;

    virtual CGwsFeatureIterator *
                            CreateFeatureIterator (EGwsFeatureIteratorType);

    virtual EGwsPreparedQueryType
                            GetPreparedQueryType ();
public:
    int                     GetPathLength () {return m_pathlength;}

protected:
    CGwsQueryResultDescriptors * m_resultDescriptor;
    CGwsFeatureSourceQuery    *  m_pQuery;

    // path length to the primary join feature
    int                          m_pathlength;
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
    EGwsStatus              Init (GisStringCollection * sellist,
                                  GisStringCollection * orderBy,
                                  FdoFilter           * filter);

    GWS_QUERYENGINE_API
    virtual void            PrepareInternal ();

    GWS_QUERYENGINE_API
    virtual void            SetFilterInternal (FdoFilter * filter);


    GWS_QUERYENGINE_API
    virtual EGwsStatus      Execute (IGWSFeatureIterator ** results);
    GWS_QUERYENGINE_API
    virtual EGwsStatus      Execute (FdoFilter * filter,
                                     IGWSFeatureIterator ** results);
    GWS_QUERYENGINE_API
    virtual EGwsStatus      Execute (const GWSFeatureId & featid,
                                     IGWSFeatureIterator ** results);
    GWS_QUERYENGINE_API
    virtual EGwsStatus      Execute (const GwsFeaturesIdVector & featids,
                                     int lbound,
                                     int ubound,
                                     IGWSFeatureIterator ** results);
    GWS_QUERYENGINE_API
    virtual EGwsStatus      SetFilter (FdoFilter * filter);

    GWS_QUERYENGINE_API
    virtual CGwsPreparedFeatureQuery *  GetPrimaryQuery () ;
    GWS_QUERYENGINE_API
    virtual FdoDataPropertyDefinitionCollection * GetIdentityProperties ();
    GWS_QUERYENGINE_API
    virtual GisStringCollection *  GetOrderBy ();

protected:
    WSTRARRAY               m_selectList;
    bool                    m_bIsSelectDistinct;
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
                                                   GisStringCollection      * lcols,
                                                   GisStringCollection      * rcols,
                                                   CGwsFeatureSourceQuery   * query);

    GWS_QUERYENGINE_API
    virtual                 ~CGwsPreparedJoinQuery ();

    CGwsPreparedQuery   *   LeftQuery () {return m_leftquery;}
    CGwsPreparedQuery   *   RightQuery () {return m_rightquery;}

    GisStringCollection *   LeftProperties ()
    {
        if (m_leftCols != NULL)
            m_leftCols.p->AddRef ();
        return m_leftCols;
    }
    GisStringCollection *   RightProperties ()
    {
        if (m_rightCols != NULL)
            m_rightCols.p->AddRef ();
        return m_rightCols;
    }


    GWS_QUERYENGINE_API
    virtual EGwsJoinMethod  JoinMethod () const { return m_joinmethod; }

    GWS_QUERYENGINE_API
    virtual EGwsStatus      Execute (IGWSFeatureIterator ** results);
    GWS_QUERYENGINE_API
    virtual EGwsStatus      Execute (FdoFilter * filter,
                 IGWSFeatureIterator ** results);
    GWS_QUERYENGINE_API
    virtual EGwsStatus      Execute (const GWSFeatureId & featid,
                 IGWSFeatureIterator ** results);
    GWS_QUERYENGINE_API
    virtual EGwsStatus      Execute (const GwsFeaturesIdVector & featids,
                                     int lbound,
                                     int ubound,
                                     IGWSFeatureIterator ** results);
    GWS_QUERYENGINE_API
    virtual EGwsStatus      SetFilter (FdoFilter * filter);

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
    GisPtr<GisStringCollection> m_leftCols;
    GisPtr<GisStringCollection> m_rightCols;
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
                                                       GisStringCollection      * lcols,
                                                       GisStringCollection      * rcols,
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
                                                       GisStringCollection      * lcols,
                                                       GisStringCollection      * rcols,
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
    virtual void            Prepare ();
    GWS_QUERYENGINE_API
    virtual void            Execute (IGWSFeatureIterator ** results);
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
    virtual void            SetCSFactory (IGWSCoordinateSystemConverterFactory * csfactory);

    GWS_QUERYENGINE_API
    virtual const GWSCoordinateSystem & GetDestinationCS ();

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
                        GisStringCollection      * lcols,
                        GisStringCollection      * rcols);

    GWS_QUERYENGINE_API
    virtual CGwsPreparedQuery * Prepare (
                        IGWSQueryDefinition * pQryDef,
                        const WSTR          & suffix);
    GWS_QUERYENGINE_API
    virtual CGwsPreparedFeatureQuery * PrepareFeatureQuery (
                        IGWSFeatureQueryDefinition * pQryDef,
                        GisStringCollection        * orderCols,
                        const WSTR                 & suffix);
    GWS_QUERYENGINE_API
    virtual CGwsPreparedJoinQuery * PrepareJoinQuery (
                                            IGWSJoinQueryDefinition   * pQryDef,
                                            const WSTR                  & suffix);

    // validate join attributes
    void                               ValidateJoinAttributes (
                                            FdoPropertyDefinition * lpdef,
                                            FdoPropertyDefinition * rpdef,
                                            GisString             * lname,
                                            GisString             * rname);

    // returns the primary (left hand) prepared query
    GWS_QUERYENGINE_API
    CGwsPreparedFeatureQuery *          GetPrimaryPreparedQuery ();

    // return prepared query
    CGwsPreparedQuery *                 GetPreparedQuery () {return m_pQuery; }

protected:
    GisPtr<IGWSConnectionPool>   m_connectionpool;
    GisPtr<IGWSQueryDefinition>  m_qrydef;
    CGwsPreparedQuery        *   m_pQuery;   // prepared query
    WSTR                         m_revprop;
    GisPtr<IGWSCoordinateSystemConverterFactory> m_csfactory;
    GWSCoordinateSystem          m_csname;
};


class GwsBinaryFeatureWriter;
///////////////////////////////////////////////////////////////////////////////
//
// class CGwsFeatureIterator
// implementation of IGWSFeatureIterator for the feature source
//
///////////////////////////////////////////////////////////////////////////////
class CGwsFeatureIterator : public GWSObject<IGWSFeatureIterator>,
                            public CGwsObject
{
friend class CGwsPreparedFeatureQuery;
friend class CGwsRightSortedJoinQueryResults;
friend class CGwsRightJoinQueryResults;
public:
    GWS_QUERYENGINE_API     CGwsFeatureIterator ();
    GWS_QUERYENGINE_API
    virtual                 ~CGwsFeatureIterator () throw();

    GWS_QUERYENGINE_API
    virtual EGwsStatus      InitializeReader (
                                    FdoIFeatureReader               * pReader,
                                    IGWSQuery                       * fquery,
                                    CGwsPreparedFeatureQuery        * prepquery);

    // this method is used in the non -cache case in order to expose
    // integer feature ids as cached id
    GWS_QUERYENGINE_API
    void                    SetExposeFeatureIdAsCacheId (bool bFlag);

    GWS_QUERYENGINE_API
    virtual void            DescribeFeature(IGWSExtendedFeatureDescription ** ppResDesc);
    GWS_QUERYENGINE_API
    virtual bool            NextFeature (IGWSFeature ** feature);
    GWS_QUERYENGINE_API
    virtual bool            ReadNext();
    GWS_QUERYENGINE_API
    virtual void            Close ();
    GWS_QUERYENGINE_API
    virtual GisInt32        GetCacheId ();
    GWS_QUERYENGINE_API
    virtual GisInt32        GetRevisionNumber ();
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
    virtual GisString*      GetLayerSource (){ return m_strLayerSource.c_str(); }
    GWS_QUERYENGINE_API
    virtual void            SetLayerSource (GisString* strLayerSource){ m_strLayerSource = strLayerSource; }

    GWS_QUERYENGINE_API
    virtual IGWSFeatureIterator* GetJoinedFeatures (int i);

    // getters by the prperty name
    GWS_QUERYENGINE_API
    virtual bool            IsNull      (GisString* propertyName);
    GWS_QUERYENGINE_API
    virtual GisString   *   GetString   (GisString * propname);
    GWS_QUERYENGINE_API
    virtual bool            GetBoolean  (GisString* propertyName);
    GWS_QUERYENGINE_API
    virtual GisByte         GetByte     (GisString* propertyName);
    GWS_QUERYENGINE_API
    virtual GisDateTime     GetDateTime (GisString* propertyName);
    GWS_QUERYENGINE_API
    virtual double          GetDouble   (GisString* propertyName);
    GWS_QUERYENGINE_API
    virtual GisInt16        GetInt16    (GisString* propertyName);
    GWS_QUERYENGINE_API
    virtual GisInt32        GetInt32    (GisString* propertyName);
    GWS_QUERYENGINE_API
    virtual GisInt64        GetInt64    (GisString* propertyName);
    GWS_QUERYENGINE_API
    virtual float           GetSingle   (GisString* propertyName);
    GWS_QUERYENGINE_API
    virtual FdoLOBValue*    GetLOB      (GisString* propertyName);
    GWS_QUERYENGINE_API
    virtual GisIStreamReader* GetLOBStreamReader(const wchar_t* propertyName );
    GWS_QUERYENGINE_API
    virtual FdoIRaster*     GetRaster   (GisString* propertyName);
    GWS_QUERYENGINE_API
    virtual GisInt32        GetDepth    ();
    GWS_QUERYENGINE_API
    virtual const GisByte * GetGeometry (GisString* propertyName, GisInt32 * count) ;
    GWS_QUERYENGINE_API
    virtual GisByteArray*   GetGeometry (GisString* propertyName);
    GWS_QUERYENGINE_API
    virtual FdoIFeatureReader* GetFeatureObject(GisString* propertyName);
    GWS_QUERYENGINE_API
    virtual FdoDataValue *  GetDataValue (GisString* propertyName);
    GWS_QUERYENGINE_API
    virtual FdoDataValueCollection *
        GetDataValues (GisStringCollection* propertyNames);
/*
    // Getter by the property index
    GWS_QUERYENGINE_API
    virtual bool            IsNull      (GisInt32 iProp);
    GWS_QUERYENGINE_API
    virtual GisString   *   GetString   (GisInt32 iProp);
    GWS_QUERYENGINE_API
    virtual bool            GetBoolean  (GisInt32 iProp);
    GWS_QUERYENGINE_API
    virtual GisByte         GetByte     (GisInt32 iProp);
    GWS_QUERYENGINE_API
    virtual GisDateTime     GetDateTime (GisInt32 iProp);
    GWS_QUERYENGINE_API
    virtual double          GetDouble   (GisInt32 iProp);
    GWS_QUERYENGINE_API
    virtual GisInt16        GetInt16    (GisInt32 iProp);
    GWS_QUERYENGINE_API
    virtual GisInt32        GetInt32    (GisInt32 iProp);
    GWS_QUERYENGINE_API
    virtual GisInt64        GetInt64    (GisInt32 iProp);
    GWS_QUERYENGINE_API
    virtual float           GetSingle   (GisInt32 iProp);
    GWS_QUERYENGINE_API
    virtual FdoLOBValue*    GetLOB      (GisInt32 iProp);
    GWS_QUERYENGINE_API
    virtual GisIStreamReader* GetLOBStreamReader (GisInt32 iProp);
    GWS_QUERYENGINE_API
    virtual FdoIRaster*     GetRaster   (GisInt32 iProp);
    GWS_QUERYENGINE_API
    virtual const GisByte * GetGeometry (GisInt32 iProp, GisInt32 * count);
    GWS_QUERYENGINE_API
    virtual GisByteArray*   GetGeometry (GisInt32 iProp);
    GWS_QUERYENGINE_API
    virtual FdoIFeatureReader* GetFeatureObject (GisInt32 iProp);

    GWS_QUERYENGINE_API
    virtual void            ToString    (GisInt32 iProp, wchar_t * buff, int len);
*/
    GWS_QUERYENGINE_API
    virtual void            ToString    (GisString* propertyName, wchar_t * buff, int len);
    GWS_QUERYENGINE_API
    virtual unsigned char*  ToBuffer(int& bufLen);

    GWS_QUERYENGINE_API
    virtual const GWSCoordinateSystem & GetCSName ();
    GWS_QUERYENGINE_API
    virtual FdoClassDefinition* GetClassDefinition();
    GWS_QUERYENGINE_API
    virtual GisGeometryType GetGeometryType(GisByteArray* pArray);
    GWS_QUERYENGINE_API
    virtual GisString*      GetPrimaryGeometryName();
    // get data property value
    GWS_QUERYENGINE_API
    virtual FdoDataValue *  GetPropertyValue (const CGwsPropertyDesc &);
    GWS_QUERYENGINE_API
    IGWSMutableFeature *    GetSimpleFeature ();
    GWS_QUERYENGINE_API
    virtual bool            InitializeMutableFeature ();

public:
    GWS_QUERYENGINE_API
    virtual const CGwsPropertyDesc &
                            GetPropertyDescriptor (int iProp);
    //NOTE! This reader is not AddRef'd so don't call release on it, or
    //put it in a GisPtr<>.
    FdoIFeatureReader*      GetFdoReader(void) { return m_reader; }

    GWS_QUERYENGINE_API
    virtual const CGwsPropertyDesc &
                            GetPropertyDescriptor (GisString * propname);

protected:
    // throw an exception if m_reader is null. m_reader maybe
    // null in some join cases
    void                    CheckReader ();

protected:
    GisPtr<FdoIFeatureReader>              m_reader;

    // reference to query
    GisPtr<IGWSQuery>                      m_query;

    // reference to prepared query that
    // is used to creted iterator
    CGwsPreparedQuery                   *  m_prepquery;

    // this guy lives in prepared query, but I want to keep
    // it here. This will safe us some steps, since
    // convertion should only happend at the lowest level,
    // when geometry is gotten from Fdo.
    // In the join case, CGwsFeatureIterator wraps another
    // CGwsFeatureIterator and we don't need to do convertion
    // in this case.
    GisPtr<IGWSCoordinateSystemConverter>  m_converter;

    GisPtr<IGWSMutableFeature>             m_mutableFeature;
    bool                                   m_bMutableFeatureSet;
    std::wstring                           m_strLayerSource;
    bool                                   m_bExposeFeatureIdAsCacheId;

    // flag indicating that geometry has been already converted
    bool                                   m_bGeometryConverted;
    GwsBinaryFeatureWriter*                m_pBinaryWriter;
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
                                    GisStringCollection   * leftjoincols,
                                    IGWSQuery             * query,
                                    CGwsPreparedQuery     * prepquery);

    GWS_QUERYENGINE_API
    virtual EGwsStatus          InitializeReader (
                                    IGWSQuery             * query,
                                    CGwsPreparedJoinQuery * prepquery) = 0;

    GWS_QUERYENGINE_API
    virtual bool                ReadNext ();

    GWS_QUERYENGINE_API
    virtual IGWSFeatureIterator *  GetJoinedFeatures (int i);

    // get right hade side joined features
    IGWSFeatureIterator *       GetJoinedFeatures ();

    GWS_QUERYENGINE_API
    virtual GisInt32            GetRevisionNumber ();
    GWS_QUERYENGINE_API
    virtual GWSFeatureId        GetFeatureId      ();
    GWS_QUERYENGINE_API
    virtual void                Close ();

    GWS_QUERYENGINE_API
    virtual EJoinResultsType    Type () const = 0;

    FdoDataValueCollection *    GetJoinValues ();

protected:
    CGwsRightJoinQueryResults             * m_right;
    GisPtr<GisStringCollection>             m_leftcols;
    GWSFeatureId                            m_leftJoinVals;
    bool                                    m_bLeftJoinValuesSet;
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
                                    GisStringCollection * joincols);
    GWS_QUERYENGINE_API
    virtual EGwsStatus          InitializeReader (
                                    IGWSQuery           * query,
                                    CGwsPreparedQuery   * prepquery,
                                    GisStringCollection * joincols);

    GWS_QUERYENGINE_API
    void                        SetNeverUsePooling ();

    GWS_QUERYENGINE_API
    virtual EGwsStatus          SetRelatedValues (const GWSFeatureId & vals);
    GWS_QUERYENGINE_API
    virtual GisInt32            GetRevisionNumber ();
    GWS_QUERYENGINE_API
    virtual GWSFeatureId        GetFeatureId ();
    GWS_QUERYENGINE_API
    virtual void                DescribeFeature(IGWSExtendedFeatureDescription ** ppResDesc);
    GWS_QUERYENGINE_API
    virtual IGWSFeatureIterator* GetJoinedFeatures (int i);

    // getters by the property name
    GWS_QUERYENGINE_API
    virtual bool                IsNull      (GisString* propertyName);
    GWS_QUERYENGINE_API
    virtual GisString   *       GetString   (GisString * propname);
    GWS_QUERYENGINE_API
    virtual bool                GetBoolean  (GisString* propertyName);
    GWS_QUERYENGINE_API
    virtual GisByte             GetByte     (GisString* propertyName);
    GWS_QUERYENGINE_API
    virtual GisDateTime         GetDateTime (GisString* propertyName);
    GWS_QUERYENGINE_API
    virtual double              GetDouble   (GisString* propertyName);
    GWS_QUERYENGINE_API
    virtual GisInt16            GetInt16    (GisString* propertyName);
    GWS_QUERYENGINE_API
    virtual GisInt32            GetInt32    (GisString* propertyName);
    GWS_QUERYENGINE_API
    virtual GisInt64            GetInt64    (GisString* propertyName);
    GWS_QUERYENGINE_API
    virtual float               GetSingle   (GisString* propertyName);
    GWS_QUERYENGINE_API
    virtual FdoLOBValue*        GetLOB      (GisString* propertyName);
    GWS_QUERYENGINE_API
    virtual GisIStreamReader*   GetLOBStreamReader(const wchar_t* propertyName );
    GWS_QUERYENGINE_API
    virtual FdoIRaster*         GetRaster   (GisString* propertyName);

    GWS_QUERYENGINE_API
    virtual const GisByte *     GetGeometry (GisString* propertyName, GisInt32 * count) ;
    GWS_QUERYENGINE_API
    virtual GisByteArray*       GetGeometry (GisString* propertyName);
    GWS_QUERYENGINE_API
    virtual FdoIFeatureReader*  GetFeatureObject(GisString* propertyName);

    GWS_QUERYENGINE_API
    virtual FdoDataValue *      GetDataValue (GisString* propertyName);
    GWS_QUERYENGINE_API
    virtual FdoDataValueCollection * GetDataValues (GisStringCollection* propertyNames);
/*
    // Getter by the property index
    GWS_QUERYENGINE_API
    virtual bool                IsNull      (GisInt32 iProp);
    GWS_QUERYENGINE_API
    virtual GisString   *       GetString   (GisInt32 iProp);
    GWS_QUERYENGINE_API
    virtual bool                GetBoolean  (GisInt32 iProp);
    GWS_QUERYENGINE_API
    virtual GisByte             GetByte     (GisInt32 iProp);
    GWS_QUERYENGINE_API
    virtual GisDateTime         GetDateTime (GisInt32 iProp);
    GWS_QUERYENGINE_API
    virtual double              GetDouble   (GisInt32 iProp);
    GWS_QUERYENGINE_API
    virtual GisInt16            GetInt16    (GisInt32 iProp);
    GWS_QUERYENGINE_API
    virtual GisInt32            GetInt32    (GisInt32 iProp);
    GWS_QUERYENGINE_API
    virtual GisInt64            GetInt64    (GisInt32 iProp);
    GWS_QUERYENGINE_API
    virtual float               GetSingle   (GisInt32 iProp);
    GWS_QUERYENGINE_API
    virtual FdoLOBValue*        GetLOB      (GisInt32 iProp);
    GWS_QUERYENGINE_API
    virtual GisIStreamReader*   GetLOBStreamReader (GisInt32 iProp);
    GWS_QUERYENGINE_API
    virtual FdoIRaster*         GetRaster   (GisInt32 iProp);
    GWS_QUERYENGINE_API
    virtual const GisByte *     GetGeometry (GisInt32 iProp, GisInt32 * count);
    GWS_QUERYENGINE_API
    virtual GisByteArray*       GetGeometry (GisInt32 iProp);
    GWS_QUERYENGINE_API
    virtual FdoIFeatureReader*  GetFeatureObject (GisInt32 iProp);

    GWS_QUERYENGINE_API
    virtual void                ToString    (GisInt32 iProp, wchar_t * buff, int len);
*/
    GWS_QUERYENGINE_API
    virtual void                ToString    (GisString* propertyName, wchar_t * buff, int len);

protected:
    IGWSFeature *               GetPooledFeature ();

protected:
    GisPtr<GisStringCollection>
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
                                    CGwsPreparedJoinQuery           * prepquery);

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
                CGwsPreparedJoinQuery           * prepquery);
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
                                CGwsPreparedJoinQuery           * prepquery);
    GWS_QUERYENGINE_API
    virtual EJoinResultsType Type () const
    {
        return kGwsNestedLoopSortBlockResults;
    }
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
                                    GisStringCollection * joincols);

    GWS_QUERYENGINE_API
    virtual bool                ReadNext();
    GWS_QUERYENGINE_API
    virtual void                Close ();
    GWS_QUERYENGINE_API
    virtual EGwsStatus          SetRelatedValues (const GWSFeatureId & vals);

protected:
    typedef enum _EGwsCursorPosition {
        eBeforeFirstRow,
        eOnJoinRow,
        eAfterJoinRow,
        eAfterLastRow
    } EGwsCursorPosition;

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
                                    GisStringCollection        * joincols);

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
                                    GisStringCollection        * joincols);

    GWS_QUERYENGINE_API
    virtual bool                ReadNext();
    GWS_QUERYENGINE_API
    virtual void                Close ();
    GWS_QUERYENGINE_API
    virtual EGwsStatus          SetRelatedValues (const GWSFeatureId & vals);
};

#endif

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

#ifndef _GWSFLATFDOREADER_H
#define _GWSFLATFDOREADER_H

///////////////////////////////////////////////////////////////////////////////
class PropertyDefItem
{
public:
    PropertyDefItem (
        int                              nItem,
        std::vector<int>&                aPath,
        WSTR&                            aKey,
        FdoString*                       pubname,
        FdoPropertyDefinition*           pdef,
        IGWSExtendedFeatureDescription*  fdsc,
        bool                             forceReadOnly
    )
    {
        m_nItem = nItem;
        m_pDef  = pdef;
        if (pdef != NULL)
            pdef->AddRef ();
        m_fdsc = fdsc;
        if (fdsc != NULL)
            fdsc->AddRef ();

        m_pFlatDef = GwsQueryHelpers::ClonePropertyDefinition (pubname, pdef, forceReadOnly);
        m_aPath = aPath;
        m_aKey = aKey;
    }

    PropertyDefItem (const PropertyDefItem & other)
    {
        m_nItem    = other.m_nItem;
        m_pDef     = other.m_pDef;
        m_fdsc     = other.m_fdsc;
        m_pFlatDef = other.m_pFlatDef;
        m_aPath    = other.m_aPath;
        m_aKey     = other.m_aKey;
    }

    virtual ~PropertyDefItem ()
    {
    }

    static WSTR MakeAKey (const std::vector<int> & aPath, int len)
    {
        WSTR wstr;
        for (size_t i = 0; i < (size_t) len; i ++) {
            if (i > 0)
                wstr.append (/*NOXLATE*/L"-");
            wchar_t wbuff [32];
            swprintf (wbuff, 32, /*NOXLATE*/L"%ld", aPath[i]);
            wstr.append (wbuff);
        }
        return wstr;
    }

    const std::vector<int> & GetAccessPath ()
    {
        return m_aPath;
    }

    const WSTR & GetAccessKey ()
    {
        return m_aKey;
    }


    FdoString * GetOriginalName ()
    {
        if (m_pDef != NULL)
            return m_pDef->GetName ();
        return NULL;
    }

    FdoString * GetFlatName ()
    {
        if (m_pFlatDef != NULL)
            return m_pFlatDef->GetName ();
        return NULL;
    }


    FdoPropertyDefinition * GetPropertyDefinition ()
    {
        if (m_pDef != NULL)
            m_pDef.p->AddRef ();
        return m_pDef;
    }

    FdoPropertyDefinition * GetFlatPropertyDefinition ()
    {
        if (m_pFlatDef != NULL)
            m_pFlatDef.p->AddRef ();
        return m_pFlatDef;
    }

    IGWSExtendedFeatureDescription * GetExtendedFeatureDescription ()
    {
        if (m_fdsc != NULL)
            m_fdsc.p->AddRef ();
        return m_fdsc;
    }
    int GetNItem ()
    {
        return m_nItem;
    }

protected:
    PropertyDefItem () {}

private:
    int                                     m_nItem;
    FdoPtr<FdoPropertyDefinition>           m_pDef;
    FdoPtr<FdoPropertyDefinition>           m_pFlatDef;
    FdoPtr<IGWSExtendedFeatureDescription>  m_fdsc;
    std::vector<int>                        m_aPath;
    WSTR                                    m_aKey;
};


///////////////////////////////////////////////////////////////////////////////
struct sortKey
{
    FdoInt32    joinCacheId;
    FdoInt32    sortPropId;
};


///////////////////////////////////////////////////////////////////////////////
class CGwsFlatFdoReader: public FdoIFeatureReader
{
public:
                                CGwsFlatFdoReader (IGWSFeatureIterator * pIterator);
    virtual                     ~CGwsFlatFdoReader ();

    virtual void                Dispose();

    virtual FdoClassDefinition* GetClassDefinition();
    virtual FdoInt32            GetDepth();
    virtual const FdoByte *     GetGeometry(FdoString* propertyName, FdoInt32 * count);
    virtual FdoByteArray*       GetGeometry(FdoString* propertyName);
    virtual FdoIFeatureReader*  GetFeatureObject(FdoString* propertyName);

    virtual bool                GetBoolean(FdoString* propertyName);
    virtual FdoByte             GetByte(FdoString* propertyName);
    virtual FdoDateTime         GetDateTime(FdoString* propertyName);
    virtual double              GetDouble(FdoString* propertyName);
    virtual FdoInt16            GetInt16(FdoString* propertyName);
    virtual FdoInt32            GetInt32(FdoString* propertyName);
    virtual FdoInt64            GetInt64(FdoString* propertyName);
    virtual float               GetSingle(FdoString* propertyName);
    virtual FdoString*          GetString(FdoString* propertyName);
    virtual FdoLOBValue*        GetLOB(FdoString* propertyName);
    virtual FdoIStreamReader*   GetLOBStreamReader(const wchar_t* propertyName );
    virtual bool                IsNull(FdoString* propertyName);
    virtual FdoIRaster*         GetRaster(FdoString* propertyName);

    virtual const FdoByte *     GetGeometry(FdoInt32 index, FdoInt32 * count);
    virtual FdoByteArray*       GetGeometry(FdoInt32 index);
    virtual FdoIFeatureReader*  GetFeatureObject(FdoInt32 index);

    virtual bool                GetBoolean(FdoInt32 index);
    virtual FdoByte             GetByte(FdoInt32 index);
    virtual FdoDateTime         GetDateTime(FdoInt32 index);
    virtual double              GetDouble(FdoInt32 index);
    virtual FdoInt16            GetInt16(FdoInt32 index);
    virtual FdoInt32            GetInt32(FdoInt32 index);
    virtual FdoInt64            GetInt64(FdoInt32 index);
    virtual float               GetSingle(FdoInt32 index);
    virtual FdoString*          GetString(FdoInt32 index);
    virtual FdoLOBValue*        GetLOB(FdoInt32 index);
    virtual FdoIStreamReader*   GetLOBStreamReader(FdoInt32 index);
    virtual bool                IsNull(FdoInt32 index);
    virtual FdoIRaster*         GetRaster(FdoInt32 index);

    virtual FdoString*          GetPropertyName(FdoInt32 index);
    virtual FdoInt32            GetPropertyIndex(FdoString* propertyName);

    virtual bool                ReadNext();
    virtual void                Close();
    virtual FdoInt32            GetCacheId();

    IGWSFeatureIterator   *     GetOriginalIterator ();

    int                         Count();
    bool                        Scrollable();
    bool                        ReadFirst();
    bool                        ReadLast();
    bool                        ReadPrevious();
    bool                        ReadAt(FdoPropertyValueCollection* key);
    bool                        ReadAtIndex( unsigned int recordindex );
    unsigned int                IndexOf(FdoPropertyValueCollection* key);

    FdoByteArray*               GetOriginalGeometry(FdoString* propertyName);

protected:
    PropertyDefItem     *       GetPropertyDefItem   (FdoString * pname);
    CGwsFeatureIterator *       GetReaderForProperty (FdoString * pname,
                                                      WSTR&                                     origname);
    CGwsFeatureIterator *       FindReader (
                                    const WSTR                & aKey,
                                    const std::vector<int>    & aPath);
    void                        LoadAllReaders();
    void                        ReleaseJoinReaders ();
    static void                 CollectPropertyNames (
                                    FdoStringCollection                     * names,
                                    std::map<std::wstring, PropertyDefItem> & types,
                                    IGWSExtendedFeatureDescription         *  pDescription,
                                    FdoStringCollection*                      keyProps,
                                    int                                       nItem,
                                    std::vector<int>                          iPath,
                                    std::vector<std::vector<int> >*           pPaths = NULL,
                                    std::vector<WSTR>*                        pKeys = NULL);
private:
    bool                        PositionPrimaryReaderAtCacheId(FdoInt32 cacheId);
    bool                        BuildCidIndexMap(); // map cache id to scrollable index (LHS)
    void                        SortResults();      // sort the results

public:
    static void BuildFlatDescription(IGWSQueryDefinition* pQryDef,
                                    IGWSExtendedFeatureDescription* pJoinDesc,
                                    IGWSExtendedFeatureDescription** ppResDesc);
protected:
    FdoPtr<CGwsFeatureIterator> m_gwsReader;
    bool                        m_bJoin;
    FdoPtr<FdoClassDefinition>  m_classDef;
    FdoPtr<FdoStringCollection> m_names;
    std::map<WSTR, PropertyDefItem> m_PropDefs;
    std::map<WSTR, CGwsFeatureIterator *> m_pJoinReaders;
    std::vector<std::vector<int> > m_readerPaths;
    std::vector<WSTR>              m_readerKeys;
    bool                        m_bReadersLoaded;
    int                         m_tableSize;
    int                         m_curCacheId;
    int                         m_curIndex;
    bool                        m_curIndexResult;
    std::map<FdoInt32,FdoInt32> m_left_cid_map;         // map of m_gwsReader cacheid to index
    bool                        m_forceOneToOne;
    FdoPtr<FdoStringCollection> m_orderBy;
    FdoOrderingOption           m_orderOpt;
    std::vector<sortKey>        m_orderedCIDs;
};


///////////////////////////////////////////////////////////////////////////////
class CGwsFlatGwsIterator: public CGwsFeatureIterator
{
public:
                                CGwsFlatGwsIterator (IGWSFeatureIterator * iter);
    virtual                     ~CGwsFlatGwsIterator () throw();

    // FdoIFeatureReader
    virtual FdoClassDefinition* GetClassDefinition();
    virtual FdoInt32            GetDepth();
    virtual const FdoByte *     GetGeometry(FdoString* propertyName, FdoInt32 * count);
    virtual FdoByteArray*       GetGeometry(FdoString* propertyName);
    virtual FdoIFeatureReader*  GetFeatureObject(FdoString* propertyName);

    virtual bool                GetBoolean(FdoString* propertyName);
    virtual FdoByte             GetByte(FdoString* propertyName);
    virtual FdoDateTime         GetDateTime(FdoString* propertyName);
    virtual double              GetDouble(FdoString* propertyName);
    virtual FdoInt16            GetInt16(FdoString* propertyName);
    virtual FdoInt32            GetInt32(FdoString* propertyName);
    virtual FdoInt64            GetInt64(FdoString* propertyName);
    virtual float               GetSingle(FdoString* propertyName);
    virtual FdoString*          GetString(FdoString* propertyName);
    virtual FdoLOBValue*        GetLOB(FdoString* propertyName);
    virtual FdoIStreamReader*   GetLOBStreamReader(const wchar_t* propertyName );
    virtual bool                IsNull(FdoString* propertyName);
    virtual FdoIRaster*         GetRaster(FdoString* propertyName);
    virtual bool                ReadNext();
    virtual void                Close();

    // IGWSFeatureIterator
    virtual bool                NextFeature (IGWSFeature ** feature);
    virtual IGWSFeatureIterator* GetJoinedFeatures (int i);

    virtual void                DescribeFeature(IGWSExtendedFeatureDescription ** ppResDesc);
    virtual FdoInt32            GetCacheId ();
    virtual FdoString*          GetLayerSource ();
    virtual bool                IsNew ();
    virtual bool                IsModified ();
    virtual bool                IsDeleted ();
    virtual EGwsLockType        GetCacheLockType ();
    virtual FdoInt32            GetRevisionNumber ();
    virtual GWSFeatureId        GetFeatureId ();
    virtual FdoGeometryType     GetGeometryType(FdoByteArray* pArray);
    virtual FdoString*          GetPrimaryGeometryName();

    virtual const GWSCoordinateSystem &  GetCSName ();
    virtual FdoDataValue *      GetDataValue (FdoString* propertyName);
    virtual FdoDataValueCollection *
                                GetDataValues (FdoStringCollection* propertyNames);
    virtual void                ToString    (FdoString * propName, wchar_t * buff, int len);
    virtual unsigned char*      ToBuffer    (int& bufLen);

    virtual bool                Scrollable();
    //IGWSFeatureIterator::SdfIScrollableFeatureReader implementation.
    virtual int                 Count();
    virtual bool                ReadFirst();
    virtual bool                ReadLast();
    virtual bool                ReadPrevious();
    virtual bool                ReadAt(FdoPropertyValueCollection* key);
    virtual bool                ReadAtIndex( unsigned int recordindex );
    virtual unsigned int        IndexOf(FdoPropertyValueCollection* key);

    virtual FdoByteArray*       GetOriginalGeometry(FdoString* propertyName);
    GWS_QUERYENGINE_API IGWSFeatureIterator*  GetPrimaryIterator() { return m_pFlatFdoReader->GetOriginalIterator(); }

    virtual FdoInt32            ResolveJoinIdVector(std::vector<FdoInt32>& idVec);
    virtual std::vector<FdoInt32>* GetJoinIdVector(FdoInt32 jCacheId);
    virtual bool                ReadAtCacheId(FdoInt32 cacheId);

protected:
    FdoDataValue *              GetPropertyValue (const CGwsPropertyDesc & desc);
    const CGwsPropertyDesc &    GetPropertyDescriptor (FdoString * propname);

protected:
    CGwsFlatFdoReader       *   m_pFlatFdoReader;
    FdoPtr<IGWSExtendedFeatureDescription> m_fdsc;
};

#endif /* _GWSFLATFDOREADER_H */

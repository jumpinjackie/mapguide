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

#ifndef GWS_MUTABLEFEATURE_H
#define GWS_MUTABLEFEATURE_H

// forward declarations
class CGwsPropertyDesc;

///////////////////////////////////////////////////////////////////////////////
//
// class CGwsMutableFeature
//
///////////////////////////////////////////////////////////////////////////////
class CGwsMutableFeature : public GWSObject<IGWSMutableFeature>,
                           public CGwsObject

{
friend class IGWSMutableFeature;
friend class CGwsFeatureIterator;
friend class CGwsFdoInsertCommand;

protected:
    virtual                     ~CGwsMutableFeature() throw() {}

public:
                                CGwsMutableFeature();

    void                        Initialize (IGWSExtendedFeatureDescription * pFeatDesc);

    virtual void                DescribeFeature(IGWSExtendedFeatureDescription ** ppResDesc);
    virtual GisInt32            GetCacheId ();
    virtual GisInt32            GetRevisionNumber ();
    virtual GWSFeatureId        GetFeatureId ();
    virtual bool                IsNew ();
    virtual bool                IsModified ();
    virtual bool                IsDeleted ();
    virtual EGwsLockType        GetCacheLockType ();

    virtual void                SetLayerSource(GisString* strLayerSource) { m_strLayerSource = strLayerSource; }
    virtual GisString*          GetLayerSource() { return m_strLayerSource.c_str(); }
    virtual GisGeometryType     GetGeometryType(GisByteArray* pArray);
    virtual GisString*          GetPrimaryGeometryName();

    virtual bool                IsPropertyValueSet (GisString* propertyName);

    virtual bool                IsNull      (GisString * propertyName);
    virtual GisString   *       GetString   (GisString * propertyName);
    virtual bool                GetBoolean  (GisString * propertyName);
    virtual GisByte             GetByte     (GisString * propertyName);
    virtual GisDateTime         GetDateTime (GisString * propertyName);
    virtual double              GetDouble   (GisString * propertyName);
    virtual GisInt16            GetInt16    (GisString * propertyName);
    virtual GisInt32            GetInt32    (GisString * propertyName);
    virtual GisInt64            GetInt64    (GisString * propertyName);
    virtual float               GetSingle   (GisString * propertyName);
    virtual FdoLOBValue*        GetLOB      (GisString * propertyName);
    virtual GisIStreamReader*   GetLOBStreamReader (GisString * propertyName);
    virtual FdoIRaster*         GetRaster   (GisString * propertyName);
    virtual const GisByte *     GetGeometry (GisString * propertyName, GisInt32 * count);
    virtual GisByteArray*       GetGeometry (GisString * propertyName);
    virtual FdoIFeatureReader*  GetFeatureObject (GisString * propertyName);

    virtual void                ToString    (GisString* propertyName, wchar_t * buff, int len);
    virtual unsigned char*      ToBuffer(int& bufLen);

    virtual FdoDataValue *      GetDataValue (GisString* propertyName);
    virtual FdoDataValueCollection *
                                GetDataValues (GisStringCollection* propertyNames);

    virtual void                SetNull(GisString* propertyName);
    virtual void                SetString(GisString* propertyName, GisString* pValue);
    virtual void                SetBoolean(GisString* propertyName, bool value);
    virtual void                SetByte(GisString* propertyName, GisByte value);
    virtual void                SetDateTime(GisString* propertyName, GisDateTime value);
    virtual void                SetDouble(GisString* propertyName, double value);
    virtual void                SetInt16(GisString* propertyName, GisInt16 value);
    virtual void                SetInt32(GisString* propertyName, GisInt32 value);
    virtual void                SetInt64(GisString* propertyName, GisInt64 value);
    virtual void                SetSingle(GisString* propertyName, float value);
    virtual void                SetLOB(GisString* propertyName, FdoLOBValue* pValue);
    virtual void                SetGeometry (GisString* propertyName, GisByteArray * pValue);
    virtual void                SetValue (GisString* propertyName, FdoValueExpression* pVal);

    //virtual void SetLOBStreamReader(const wchar_t* propertyName, GisIStreamReader* pValue );
    //virtual void SetRaster(GisString* propertyName, FdoIRaster* pValue);

    virtual void                SetCSName (const GWSCoordinateSystem & scsname);
    virtual const GWSCoordinateSystem &  GetCSName ();

    // These are for internal use only
    virtual FdoPropertyValueCollection* GetPropertyValues()
    {
        if (m_pProperties != NULL)
            m_pProperties.p->AddRef();
        return m_pProperties;
    }

    virtual void                SetPropertyValues(FdoPropertyValueCollection * vals)
    {
        m_pProperties = vals;
        if (vals != NULL)
            vals->AddRef ();
    }

    void                        InitializePropertyValues ();

    void                        SetPropertyValues (IGWSFeature * feature);

    // returns the number of properties
    virtual int                 GetCount ();

    // get property value
    virtual FdoPropertyValue *  GetPropertyValue (int idx);
    virtual const GWSQualifiedName &
                                ClassName () const;

    // extract individual feature from the extended feature
    // (for internal use only)
    IGWSMutableFeature        * GetSimpleFeature ();

    // simple feature shortcuts. In case of extended feature
    // asserts in debug mode and returns the first item data
    virtual void                SetFeatureId  (const GWSFeatureId & fid);
    virtual void                SetRevisionNumber (GisInt32 revnum);
    virtual void                SetCacheLockType (EGwsLockType locktype);
    virtual void                SetCacheStatus (long status);
    virtual void                SetCacheId (GisInt32 cacheId);

protected:
    // ValidataPropertyName methods throw exeption
    // soft property validation. has to be a valid feature property name
    void                        ValidatePropertyName(
                                    GisString         * propName,
                                    const CGwsPropertyDesc ** propdesc);

    // strong property validation. has to be a valid feature property name
    // the specified type
    void                        ValidatePropertyName(
                                    GisString                * propName,
                                    FdoPropertyType            ptype,
                                    FdoDataType                datatype,
                                    const CGwsPropertyDesc  ** propdesc);
    // validates name and property type
    void                        ValidatePropertyName(
                                    GisString                * propName,
                                    FdoPropertyType            ptype,
                                    const CGwsPropertyDesc  ** propdesc);

    const CGwsPropertyDesc &    GetPropertyDescriptor (int iProp);
    const CGwsPropertyDesc &    GetPropertyDescriptor (GisString * propertyName);
    FdoPropertyValue        *   ConstructPropertyValue (const CGwsPropertyDesc & desc);
    FdoValueExpression      *   ConstructValueExpression (const CGwsPropertyDesc & desc);

protected:
    GisPtr<FdoPropertyValueCollection>        m_pProperties;
    GisPtr<IGWSExtendedFeatureDescription>    m_pFeatDesc;
    GisInt32                                  m_cacheId;
    GisInt32                                  m_revisionnumber;
    GWSFeatureId                              m_featureId;
    EGwsLockType                              m_cacheLockType;
    long                                      m_cacheStatus;
    WSTR                                      m_strLayerSource;
    GWSCoordinateSystem                       m_csname;
};

///////////////////////////////////////////////////////////////////////////////
//
// class CGwsBinaryFeature
//
///////////////////////////////////////////////////////////////////////////////
class GwsBinaryFeatureReader;
class CGwsBinaryFeature: public CGwsMutableFeature
{
public:
                                CGwsBinaryFeature (IGWSExtendedFeatureDescription * pFeatDesc);
    virtual                     ~CGwsBinaryFeature () throw();
    //copies the binary feature data into the base class's FdoPropertyValueCollection
    void                        Set (unsigned char* buf, int len);
private:
    GwsBinaryFeatureReader*     m_pBinaryReader;
};


///////////////////////////////////////////////////////////////////////////////
//
// class CGwsPooledFeature
//
///////////////////////////////////////////////////////////////////////////////
class CGwsPooledFeature: public CGwsMutableFeature
{
public:
                                CGwsPooledFeature (IGWSExtendedFeatureDescription * pFeatDesc);
    virtual                     ~CGwsPooledFeature () throw();

    virtual GisInt32            GetRevisionNumber ();
    virtual GWSFeatureId        GetFeatureId ();
    // sets pooled feature from the feature
    void                        Set (IGWSFeature * feature);
};

///////////////////////////////////////////////////////////////////////////////
//
// class CGwsFeaturesPool
//
// pool of mutable features of the specified type
//
///////////////////////////////////////////////////////////////////////////////
class CGwsFeaturesPool: public CGwsObject
{
public:
                                CGwsFeaturesPool    (IGWSExtendedFeatureDescription * pFeatDesc);
    virtual                     ~CGwsFeaturesPool    ();

    // returns number of feaures in the pool
    int                         GetCount ();
    // returns pool size
    int                         GetSize ();

    // returns feature from the pool
    IGWSFeature         *       GetFeature (int i);

    // sets number of used features in the pool to zero
    void                        Reset ();

    // adds feature to the pool (actually feature is being copied to the pool)
    void                        AddFeature (IGWSFeature *);

protected:
    std::vector<CGwsPooledFeature *> m_pool;
    int                              m_used;
    GisPtr<IGWSExtendedFeatureDescription>
                                     m_pFeatDesc;
};

#endif

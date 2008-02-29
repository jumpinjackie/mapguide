//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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
    virtual FdoInt32            GetCacheId ();
    virtual FdoInt32            GetRevisionNumber ();
    virtual GWSFeatureId        GetFeatureId ();
    virtual bool                IsNew ();
    virtual bool                IsModified ();
    virtual bool                IsDeleted ();
    virtual EGwsLockType        GetCacheLockType ();

    virtual void                SetLayerSource(FdoString* strLayerSource) { m_strLayerSource = strLayerSource; }
    virtual FdoString*          GetLayerSource() { return m_strLayerSource.c_str(); }
    virtual FdoGeometryType     GetGeometryType(FdoByteArray* pArray);
    virtual FdoString*          GetPrimaryGeometryName();

    virtual bool                IsPropertyValueSet (FdoString* propertyName);

    virtual bool                IsNull      (FdoString * propertyName);
    virtual FdoString   *       GetString   (FdoString * propertyName);
    virtual bool                GetBoolean  (FdoString * propertyName);
    virtual FdoByte             GetByte     (FdoString * propertyName);
    virtual FdoDateTime         GetDateTime (FdoString * propertyName);
    virtual double              GetDouble   (FdoString * propertyName);
    virtual FdoInt16            GetInt16    (FdoString * propertyName);
    virtual FdoInt32            GetInt32    (FdoString * propertyName);
    virtual FdoInt64            GetInt64    (FdoString * propertyName);
    virtual float               GetSingle   (FdoString * propertyName);
    virtual FdoLOBValue*        GetLOB      (FdoString * propertyName);
    virtual FdoIStreamReader*   GetLOBStreamReader (FdoString * propertyName);
    virtual FdoIRaster*         GetRaster   (FdoString * propertyName);
    virtual const FdoByte *     GetGeometry (FdoString * propertyName, FdoInt32 * count);
    virtual FdoByteArray*       GetGeometry (FdoString * propertyName);
    virtual FdoIFeatureReader*  GetFeatureObject (FdoString * propertyName);

    virtual void                ToString    (FdoString* propertyName, wchar_t * buff, int len);
    virtual unsigned char*      ToBuffer(int& bufLen);

    virtual FdoDataValue *      GetDataValue (FdoString* propertyName);
    virtual FdoDataValueCollection *
                                GetDataValues (FdoStringCollection* propertyNames);

    virtual void                SetNull(FdoString* propertyName);
    virtual void                SetString(FdoString* propertyName, FdoString* pValue);
    virtual void                SetBoolean(FdoString* propertyName, bool value);
    virtual void                SetByte(FdoString* propertyName, FdoByte value);
    virtual void                SetDateTime(FdoString* propertyName, FdoDateTime value);
    virtual void                SetDouble(FdoString* propertyName, double value);
    virtual void                SetInt16(FdoString* propertyName, FdoInt16 value);
    virtual void                SetInt32(FdoString* propertyName, FdoInt32 value);
    virtual void                SetInt64(FdoString* propertyName, FdoInt64 value);
    virtual void                SetSingle(FdoString* propertyName, float value);
    virtual void                SetLOB(FdoString* propertyName, FdoLOBValue* pValue);
    virtual void                SetGeometry (FdoString* propertyName, FdoByteArray * pValue);
    virtual void                SetValue (FdoString* propertyName, FdoValueExpression* pVal);

    //virtual void SetLOBStreamReader(const wchar_t* propertyName, FdoIStreamReader* pValue );
    //virtual void SetRaster(FdoString* propertyName, FdoIRaster* pValue);

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

    GWS_QUERYENGINE_API void                InitializePropertyValues ();

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
    virtual void                SetRevisionNumber (FdoInt32 revnum);
    virtual void                SetCacheLockType (EGwsLockType locktype);
    virtual void                SetCacheStatus (long status);
    virtual void                SetCacheId (FdoInt32 cacheId);

    void                        SetSourceCSName (const GWSCoordinateSystem & csname);
    const GWSCoordinateSystem & GetSourceCSName ();

protected:
    // ValidataPropertyName methods throw exeption
    // soft property validation. has to be a valid feature property name
    void                        ValidatePropertyName(
                                    FdoString         * propName,
                                    const CGwsPropertyDesc ** propdesc);

    // strong property validation. has to be a valid feature property name
    // the specified type
    void                        ValidatePropertyName(
                                    FdoString                * propName,
                                    FdoPropertyType            ptype,
                                    FdoDataType                datatype,
                                    const CGwsPropertyDesc  ** propdesc);
    // validates name and property type
    void                        ValidatePropertyName(
                                    FdoString                * propName,
                                    FdoPropertyType            ptype,
                                    const CGwsPropertyDesc  ** propdesc);

    const CGwsPropertyDesc &    GetPropertyDescriptor (int iProp);
    const CGwsPropertyDesc &    GetPropertyDescriptor (FdoString * propertyName);
    FdoPropertyValue        *   ConstructPropertyValue (const CGwsPropertyDesc & desc);
    FdoValueExpression      *   ConstructValueExpression (const CGwsPropertyDesc & desc);

protected:
    FdoPtr<FdoPropertyValueCollection>        m_pProperties;
    FdoPtr<IGWSExtendedFeatureDescription>    m_pFeatDesc;
    FdoInt32                                  m_cacheId;
    FdoInt32                                  m_revisionnumber;
    GWSFeatureId                              m_featureId;
    EGwsLockType                              m_cacheLockType;
    long                                      m_cacheStatus;
    WSTR                                      m_strLayerSource;

    // coordinate systems
    GWSCoordinateSystem                       m_csname; // coordinate system
    GWSCoordinateSystem                       m_srccsname; // coordinate system override
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

    virtual FdoInt32            GetRevisionNumber ();
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
    FdoPtr<IGWSExtendedFeatureDescription>
                                     m_pFeatDesc;
};


#endif

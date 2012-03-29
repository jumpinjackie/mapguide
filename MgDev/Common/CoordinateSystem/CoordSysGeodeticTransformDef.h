//
//  Copyright (C) 2004-2011 by Autodesk, Inc.
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

#ifndef _CCOORDINATESYSTEMGEODETICTRANSFORMDEF_H_
#define _CCOORDINATESYSTEMGEODETICTRANSFORMDEF_H_


namespace CSLibrary
{

class CCoordinateSystemGeodeticTransformDef : public MgCoordinateSystemGeodeticTransformDef
{
public:

    CCoordinateSystemGeodeticTransformDef(MgCoordinateSystemCatalog* pCatalog);
    virtual ~CCoordinateSystemGeodeticTransformDef();

    virtual void Dispose();

    void Reset(INT32 transformationDefType);
    void Initialize(const cs_GeodeticTransform_& transformDef);

    virtual MgCoordinateSystemGeodeticTransformDef* CreateClone();

    //begin properties
    virtual STRING GetTransformName();
    virtual void SetTransformName(CREFSTRING name);

    virtual STRING GetSourceDatum();
    virtual void SetSourceDatum(CREFSTRING datumKey);

    virtual STRING GetTargetDatum();
    virtual void SetTargetDatum(CREFSTRING datumKey);

    virtual STRING GetGroup();
    virtual void SetGroup(CREFSTRING group);

    virtual STRING GetDescription();
    virtual void SetDescription(CREFSTRING description);

    virtual STRING GetSource();
    virtual void SetSource(CREFSTRING source);

    virtual INT32 GetEpsgCode();
    virtual void SetEpsgCode(INT32 epsgCode);

    virtual INT32 GetEpsgVariation();
    virtual void SetEpsgVariation(INT32 epsgVariation);

    virtual bool GetInverseSupported();
    virtual void SetInverseSupported(bool inverseSupported);

    virtual INT32 GetMaxIterations();
    virtual void SetMaxIterations(INT32 maxIterations);

    virtual double GetConvergenceValue();
    virtual void SetConvergenceValue(double convergenceValue);

    virtual double GetErrorValue();
    virtual void SetErrorValue(double errorValue);

    virtual double GetAccuracy();
    virtual void SetAccuracy(double accuracy);

    virtual double GetRangeMinLongitude();
    virtual void SetRangeMinLongitude(double minLong);

    virtual double GetRangeMaxLongitude();
    virtual void SetRangeMaxLongitude(double maxLong);

    virtual double GetRangeMinLatitude();
    virtual void SetRangeMinLatitude(double minLat);

    virtual double GetRangeMaxLatitude();
    virtual void SetRangeMaxLatitude(double maxLat);

    virtual MgCoordinateSystemGeodeticTransformDefParams* GetParameters();
    virtual void SetParameters(MgCoordinateSystemGeodeticTransformDefParams* parameters);
    //end properties

    virtual INT32 GetTransformDefType();

    MgCoordinateSystemGeodeticTransformation* CreateTransformation(bool createInverse);

    virtual UINT8* SerializeFrom(UINT8* pStream);
    virtual UINT8* SerializeTo(UINT8* pStream);
    virtual UINT32 GetSizeSerialized();

    //helper - don't delete
    virtual bool IsEncrypted();

    virtual bool IsProtected();
    virtual bool IsValid();

    void CopyTo(cs_GeodeticTransform_& transformDef) const;

private:
    void ReleaseInstance();
    INT32 GetTransformationDefType(INT32 methodCode);

private:
    INT32 transformationDefType;
    cs_GeodeticTransform_* transformDefinition;
    Ptr<MgCoordinateSystemCatalog> catalog;
    
    enum CCoordinateSystemGeodeticPathObjectVersions
    {
        kGxRelease0  = 0   // Initial Release
    };
};

} //namespace CSLibrary

#endif //_CCOORDINATESYSTEMGEODETICTRANSFORMDEF_H_

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

#ifndef _CCOORDINATESYSTEMELLIPSOID_H_
#define _CCOORDINATESYSTEMELLISPOID_H_

namespace CSLibrary
{

class CCoordinateSystemEllipsoid : public MgCoordinateSystemEllipsoid
{
public:
    CCoordinateSystemEllipsoid();
    virtual ~CCoordinateSystemEllipsoid();

    //MgCoordinateSystemEllipsoid
    virtual STRING GetCode();  /// __get
    virtual void SetCode(CREFSTRING sCode);
    virtual bool IsLegalCode(CREFSTRING sCode);
    virtual bool IsValid();
    virtual bool IsUsable();
    virtual bool IsSameAs(MgGuardDisposable *pDef);
    virtual MgCoordinateSystemEllipsoid* CreateClone();
    virtual STRING GetDescription();  /// __get
    virtual void SetDescription(CREFSTRING sDesc);
    virtual bool IsLegalDescription(CREFSTRING sDesc);
    virtual STRING GetGroup();
    virtual void SetGroup(CREFSTRING sGroup);
    virtual bool IsLegalGroup(CREFSTRING sGroup);
    virtual STRING GetSource();
    virtual void SetSource(CREFSTRING sSource);
    virtual bool IsLegalSource(CREFSTRING sSource);
    virtual bool IsProtected();
    virtual INT16 GetAge();
    virtual void SetProtectMode(bool bIsProtected);
    virtual bool IsEncrypted();
    virtual void SetEncryptMode(bool bIsEncrypted);
    virtual double GetEquatorialRadius();
    virtual double GetPolarRadius();
    virtual double GetFlatteningRatio();
    virtual double GetEccentricity();
    virtual void SetRadii(double dEquatorialRadius, double dPolarRadius);
    virtual double FlatteningRatioToEccentricity(double dFlat);
    virtual double EccentricityToFlatteningRatio(double dEccent);
    virtual double EquatorialRadiusFromPolarRadiusFlatteningRatio(double dPolarRadius, double dFlatteningRatio);
    virtual double PolarRadiusFromEquatorialRadiusFlatteningRatio(double dEquatorialRadius, double dFlatteningRatio);
    virtual double GetGreatCircleDistance(double dLongitude1, double dLatitude1, double dLongitude2, double dLatitude2);
    virtual double GetGreatCircleAzimuth(double dLongitude1, double dLatitude1, double dLongitude2, double dLatitude2);
    virtual double GetRadiusMin();
    virtual double GetRadiusMax();
    virtual bool IsLegalRadius(double dRadius);
    virtual double GetFlatteningRatioMin();
    virtual double GetFlatteningRatioMax();
    virtual bool IsLegalFlatteningRatio(double dFlat);
    virtual double FlatteningRatioFromRadii(double dEquatorialRadius, double dPolarRadius);
    virtual MgCoordinateSystemCatalog* GetCatalog();

    virtual UINT8* SerializeFrom(UINT8* pStream);
    virtual UINT8* SerializeTo(UINT8* pStream);
    virtual UINT32 GetSizeSerialized();

protected:
    //from MgDisposable
    virtual void Dispose();

protected:
    //Data members
};

} // End of namespace

#endif //_CCOORDINATESYSTEMELLISPOID_H_

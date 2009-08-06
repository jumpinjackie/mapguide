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

#ifndef _CCOORDINATESYSTEMMGRS_H_
#define _CCOORDINATESYSTEMMGRS_H_

namespace CSLibrary
{

class CCoordinateSystemMgrs : public MgCoordinateSystemMgrs
{
public:
    CCoordinateSystemMgrs(INT8 nLetteringScheme, bool bSetExceptionsOn);
    CCoordinateSystemMgrs(MgCoordinateSystem* pTragetCs,INT8 nLetteringScheme, bool bSetExceptionsOn);
    virtual ~CCoordinateSystemMgrs();

    void Init(double dEquatorialRadius, double dEccentricity);
    void InitFromEllipsoid(CREFSTRING sEllipsoidCode);
    void InitFromDatum(CREFSTRING sDatumCode);

PUBLISHED_API:
    STRING ConvertFromLonLat(double dLongitude, double dLatitude, int nPrecision);
    STRING ConvertFromLonLat(MgCoordinate* pLonLat, int nPrecision);
    MgCoordinate* ConvertToLonLat(CREFSTRING sMgrs);
    INT8 GetLetteringScheme();

    INT32 GetSpecializationType ();
    void SetBoundary (MgCoordinateSystemGridBoundary* pGridBoundary);
    MgCoordinateSystemGridBoundary* GetBoundary(void);
    MgCoordinateSystemGridLineCollection* GetGridLines (MgCoordinateSystemGridSpecification* specification);
    MgCoordinateSystemGridRegionCollection* GetGridRegions (MgCoordinateSystemGridSpecification* specification);
    MgCoordinateSystemGridTickCollection* GetGridTicks (MgCoordinateSystemGridSpecification* specification);

    double GetConvergenceAngle (MgCoordinate* location);
    double GetProjectiveGridScale (MgCoordinate* location);

    INT32 GetLastError();
    void ResetLastError();
    bool AreExceptionsOn();
    void SetExceptionsOn(bool bOn);

INTERNAL_API:
    INT32 ConvertFromLonLat(double dLongitude, double dLatitude, int nPrecision, REFSTRING sMgrs);
    INT32 ConvertFromLonLat(MgCoordinate* pLonLat, int nPrecision, REFSTRING sMgrs);
    INT32 ConvertToLonLat(CREFSTRING sMgrs, MgCoordinate* pLonLat);
    INT32 ConvertToLonLat(CREFSTRING sMgrs, double& dLongitude, double& dLatitude);

protected:
    //from MgDisposable
    void Dispose();

private:
    short GetBesselFromLetteringScheme(INT8 nLetteringScheme);

protected:
    INT8 m_nLetteringScheme;
    bool m_bExceptionsOn;
    INT32 m_nLastError;
    Ptr<MgCoordinateSystem> m_pCsTarget;
    struct cs_Mgrs_* m_pCsMgrs;
    Ptr<MgCoordinateSystemGridBoundary> m_GridBoundary;

private:
    CCoordinateSystemMgrs();
    CCoordinateSystemMgrs(const CCoordinateSystemMgrs&);
    CCoordinateSystemMgrs& operator=(const CCoordinateSystemMgrs&);
};

} // End of namespace

#endif //_CCOORDINATESYSTEMMGRS_H_

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
    //struct CCoordinateSystemUtmCodeMap
    //{
    //    INT32 ZoneNbr;
    //    wchar_t ZoneCode [14];
    //};
    //static const CCordinateSystemUtmCodeMap UtmCodeMap [];
 
struct CCoordinateSystemMgrsSeries
{
	wchar_t easting [9];
	wchar_t northing [21];
};

public:
    // Static Constants, Variables (hopefully not), and Functions.
    static const CCoordinateSystemMgrsSeries MgrsSeriesNormal [6];
    static const CCoordinateSystemMgrsSeries MgrsSeriesBessel [6];
    static const wchar_t MgrsSeriesPolarSouth [2][25];
    static const wchar_t MgrsSeriesPolarNorth [2][15];
    static const wchar_t MgrsGridZoneDesignation [25];

    // The following static functions are implemented in order to achieve
    // a design goal of having all MGRS specific knowledge exist in this
    // object.
    static STRING ZoneNbrToUtmCs (INT32 zoneNbr);
    static void InitMgrsSpecification (MgCoordinateSystemGridSpecification* pSpecification,
                                       INT32 mgrsGridLevel);
    static INT32 GridZoneDesignationIndex (double latitude,double longitude = 1.0);
    static wchar_t GridZoneDesignationLetter (INT32 index);
    static STRING GridSquareDesignation (INT32 utmZoneNbr,double easting,
                                                          double northing,
                                                          INT8 letteringScheme = MgCoordinateSystemMgrsLetteringScheme::Normal);

    // Construction / Destruction
    CCoordinateSystemMgrs(INT8 nLetteringScheme, bool bSetExceptionsOn);
    CCoordinateSystemMgrs(MgCoordinateSystem* pTragetCs,INT8 nLetteringScheme, bool bSetExceptionsOn);
    virtual ~CCoordinateSystemMgrs();
    // Copy and assignment constructors are not implemented.

    // These functions, in conjuction with the first constructor provided above,
    // are used when grids are not required; that is, when only conversion between
    // geographic coordinates and MGRS strings is required.
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
    INT32 ConvertFromLonLat(double dLongitude, double dLatitude, INT32 nPrecision, REFSTRING sMgrs);
    INT32 ConvertFromLonLat(MgCoordinate* pLonLat, INT32 nPrecision, REFSTRING sMgrs);
    INT32 ConvertToLonLat(CREFSTRING sMgrs, MgCoordinate* pLonLat);
    INT32 ConvertToLonLat(CREFSTRING sMgrs, double& dLongitude, double& dLatitude);

protected:          // Still INTERNAL API only.
    // Given a frame/viewport boundary, and the coordinate system thereof,
    // this function will return a collection of CCoordinateSystemOneGrid
    // objects, each of which represents a single UTM zone.  Taken together,
    // all such zones will provide complete coverage of the provided
    // frame boundary.  Typically, however, there is only one.
    CCoordinateSystemMgrsZoneCollection* FrameBoundaryToZones (MgCoordinateSystemGridBoundary* frameBoundary,
                                                               MgCoordinateSystem* frameCS,
                                                               bool useFrameDatum);

    // Fulfillment of virtual member of MgDisposable
    void Dispose();

private:
    short GetBesselFromLetteringScheme(INT8 nLetteringScheme);

protected:
    INT8 m_nLetteringScheme;
    bool m_bExceptionsOn;
    bool m_bUseFrameDatum;
    INT32 m_nLastError;
    Ptr<MgCoordinateSystem> m_pCsTarget;
    struct cs_Mgrs_* m_pCsMgrs;
    Ptr<MgCoordinateSystemGridBoundary> m_GridBoundary;
    Ptr<CCoordinateSystemMgrsZoneCollection> m_ZoneCollection;

private:
    CCoordinateSystemMgrs();
    CCoordinateSystemMgrs(const CCoordinateSystemMgrs&);
    CCoordinateSystemMgrs& operator=(const CCoordinateSystemMgrs&);
};

} // End of namespace

#endif //_CCOORDINATESYSTEMMGRS_H_

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

#ifndef _CCOORDINATESYSTEM_H_
#define _CCOORDINATESYSTEM_H_

#ifdef _WIN32
#pragma warning( disable: 4251 )
#else
#define _wcsnicmp wcsncasecmp
#define _wcsicmp wcscasecmp
#define _stricmp  strcasecmp
#define _strnicmp strncasecmp
#endif

struct cs_Csdef_;

namespace CSLibrary
{

class CCoordinateSystem : public MgCoordinateSystem
{
public:
    CCoordinateSystem(MgCoordinateSystemCatalog* pCatalog);
    virtual ~CCoordinateSystem();

    bool InitArbitrary(const cs_Csprm_ &csprm);
    void InitFromCatalog(const cs_Csdef_& def,        
        const map<STRING, Ptr<MgDisposable> >* const ellipsoidMap = NULL,
        const map<STRING, Ptr<MgDisposable> >* const datumMap = NULL);
    void GetMentorDef(cs_Csdef_& def){def=m_csprm.csdef;};

    //from MgCoordinateSystem
    virtual INT32 GetType();  /// __get
    virtual MgCoordinate* ConvertFromLonLat(MgCoordinate* lonLat);
    virtual MgCoordinate* ConvertToLonLat(MgCoordinate* coordinate);
    virtual double ConvertCoordinateSystemUnitsToMeters(double units);
    virtual double ConvertMetersToCoordinateSystemUnits(double meters);
    virtual double MeasureEuclideanDistance(MgCoordinate* coord1, MgCoordinate* coord2);
    virtual double MeasureEuclideanDistance(double x1, double y1, double x2, double y2);
    virtual double MeasureGreatCircleDistance(MgCoordinate* coord1, MgCoordinate* coord2);
    virtual double MeasureGreatCircleDistance(double x1, double y1, double x2, double y2);
    virtual double GetAzimuth(MgCoordinate* coord1, MgCoordinate* coord2);
    virtual double GetAzimuth(double x1, double y1, double x2, double y2);
    virtual MgCoordinate* GetCoordinate(MgCoordinate* coord, double azimuth, double distance);
    virtual MgCoordinate* GetCoordinate(double xStart, double yStart, double azimuth, double distance);
    virtual STRING ToString();
    virtual STRING GetUnits();  /// __get
    virtual double GetUnitScale();  /// __get
    virtual double GetMinX();  /// __get
    virtual double GetMinY();  /// __get
    virtual double GetMaxX();  /// __get
    virtual double GetMaxY();  /// __get
    virtual STRING GetCsCode();  /// __get
    virtual STRING GetDescription();  /// __get
    virtual STRING GetProjection();  /// __get
    virtual STRING GetProjectionDescription();  /// __get
    virtual STRING GetDatum();  /// __get
    virtual STRING GetDatumDescription();  /// __get
    virtual STRING GetEllipsoid();  /// __get
    virtual STRING GetEllipsoidDescription();  /// __get

    virtual INT32 GetEpsgCode (void);  /// __get
    virtual INT32 GetSridCode (void);  /// __get
    virtual INT16 GetEpsgQuadrant (void);   /// __get

    virtual MgCoordinateSystemMeasure* GetMeasure();
    virtual MgStringCollection* GetCategories();
    virtual MgCoordinateSystemCatalog* GetCatalog();
    virtual void SetCatalog(MgCoordinateSystemCatalog* pCatalog);
    virtual void SetCsCode(CREFSTRING sCode);
    virtual bool IsLegalCsCode(CREFSTRING sCode);
    virtual bool IsValid();
    virtual bool IsUsable(MgCoordinateSystemCatalog *pCatalog);
    virtual bool IsSameAs(MgGuardDisposable *pDef);
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
    virtual MgCoordinateSystemEnumInteger32* GetErrors();
    virtual STRING GetLocation();
    virtual void SetLocation(CREFSTRING sLoc);
    virtual bool IsLegalLocation(CREFSTRING sLoc);
    virtual STRING GetCountryOrState();
    virtual void SetCountryOrState(CREFSTRING sCountryOrState);
    virtual bool IsLegalCountryOrState(CREFSTRING sCountryOrState);
    virtual bool IsGeodetic();
    virtual INT32 GetProjectionCode();
    virtual void SetProjectionCode(INT32  prjType);
    virtual INT32 GetUnitCode();
    virtual void SetUnitCode(INT32 unit);
    virtual INT32 GetProjectionParameterCount();
    virtual double GetProjectionParameter(INT32 nIndex);
    virtual void SetProjectionParameter(INT32 nIndex, double dValue);
    virtual double GetOriginLongitude();
    virtual void SetOriginLongitude(double dOrgLng);
    virtual double GetOriginLatitude();
    virtual void SetOriginLatitude(double dOrgLat);
    virtual double GetOffsetX();
    virtual double GetOffsetY();
    virtual void SetOffsets(double dXOffset, double dYOffset);
    virtual double GetScaleReduction();
    virtual void SetScaleReduction(double dSclRed);
    virtual double GetMapScale();
    virtual void SetMapScale(double dMapScale);
    virtual double GetZeroX();
    virtual double GetZeroY();
    virtual void SetZeroes(double dXZero, double dYZero);
    virtual double GetLonMin();
    virtual double GetLonMax();
    virtual double GetLatMin();
    virtual double GetLatMax();
    virtual void SetLonLatBounds(double dLonMin, double dLatMin, double dLonMax, double dLatMax);
    virtual void CancelLonLatBounds();
    virtual void SetXYBounds(double dXMin, double dYMin, double dXMax, double dYMax);
    virtual void CancelXYBounds();
    virtual INT16 GetQuadrant();
    virtual void SetQuadrant(INT16 sQuad);
    virtual MgCoordinateSystemDatum* GetDatumDefinition();
    virtual void SetDatumDefinition(MgCoordinateSystemDatum *pDatum);
    virtual MgCoordinateSystemEllipsoid* GetEllipsoidDefinition();
    virtual void SetEllipsoidDefinition(MgCoordinateSystemEllipsoid *pEllipsoid);
    virtual bool IsValidXY(double dX, double dY);
    virtual bool IsValidLonLat(double dLongitude, double dLatitude);

    virtual void ConvertCoordinateFromLonLat(MgCoordinate* lonLatToCoordinate);
    virtual void ConvertCoordinateToLonLat(MgCoordinate* coordinateToLonLat);
    virtual MgCoordinate* ConvertToLonLat(double dX, double dY);
    virtual MgCoordinate* ConvertToLonLat(double dX, double dY, double dZ);
    virtual MgCoordinate* ConvertFromLonLat(double dLongitude, double dLatitude);
    virtual MgCoordinate* ConvertFromLonLat(double dLongitude, double dLatitude, double dZ);

    virtual double GetConvergence(double dLongitude, double dLatitude);
    virtual double GetScale(double dLongitude, double dLatitude);
    virtual double GetScaleH(double dLongitude, double dLatitude);
    virtual double GetScaleK(double dLongitude, double dLatitude);
    virtual MgCoordinateSystem* CreateClone();

    virtual UINT8* SerializeFrom(UINT8* pStream);
    virtual UINT8* SerializeTo(UINT8* pStream);
    virtual UINT32 GetSizeSerialized();

INTERNAL_API:
    virtual void ConvertFromLonLat(double lon, double lat, double& x, double& y);
    virtual void ConvertFromLonLat(double lon[], double lat[], double x[], double y[], int arraySize);
    virtual void ConvertToLonLat(double x, double y, double& lon, double& lat);
    virtual void ConvertToLonLat(double x[], double y[], double lon[], double lat[], int arraySize);

public:
    /// Coordinate system information properties
    static const STRING CoordinateSystemCode;
    static const STRING CoordinateSystemDescription;
    static const STRING CoordinateSystemProjection;
    static const STRING CoordinateSystemProjectionDescription;
    static const STRING CoordinateSystemDatum;
    static const STRING CoordinateSystemDatumDescription;
    static const STRING CoordinateSystemEllipsoid;
    static const STRING CoordinateSystemEllipsoidDescription;

    static const STRING BaseLibrary;

protected:
    //from MgDisposable
    virtual void Dispose();

    //Typedefs
    typedef int (*chkfunc)(const struct cs_Csprm_ *, int, const double [][3]);
    typedef int (*llcsfunc)(const struct cs_Csprm_ *, double [3], const double [3]);
    typedef double (*doublefunc)(const struct cs_Csprm_ *, const double [2]);

    //Private member functions
    bool IsInitialized();
    void Uninitialize();
    bool IsValidCoord(chkfunc check, double dCoord1, double dCoord2);
    void ConvertPoint(chkfunc check, llcsfunc convert, double dSourceCoord1, double dSourceCoord2, double dSourceCoord3, double *pdDestCoord1, double *pdDestCoord2, double *pdDestCoord3);
    double GetDouble(doublefunc func, double dLongitude, double dLatitude);

    void SetString(CREFSTRING sSrc, char *pDest, UINT32 nMaxSize);
    bool Protected();
    void SetProjectionDefaults();

protected:
    //Data members
    bool m_bEncrypted;

    //Data members
    cs_Csprm_ m_csprm;

    Ptr<MgCoordinateSystemDatum> m_pDatum;
    Ptr<MgCoordinateSystemEllipsoid> m_pEllipsoid;
    Ptr<MgCoordinateSystemCatalog> m_pCatalog;

    enum CCoordinateSystemObjectVersions {
        kCsRelease0  = 0   // Initial Release
    };

private:
    CCoordinateSystem();
    CCoordinateSystem(const CCoordinateSystem&);
    CCoordinateSystem& operator=(const CCoordinateSystem&);
};

} // End of namespace

#endif //_CCOORDINATESYSTEM_H_

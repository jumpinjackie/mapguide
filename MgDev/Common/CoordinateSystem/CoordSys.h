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

#ifndef _CCOORDINATESYSTEM_H_
#define _CCOORDINATESYSTEM_H_

#ifdef _WIN32
#pragma warning( disable: 4251 )
#else
#define _wcsnicmp wcsncasecmp
#define _wcsicmp wcscasecmp
#endif

class OGRSpatialReference;
class OGRCoordinateTransformation;

namespace CSLibrary
{
class CCoordinateSystemCatalog;
class CCoordinateSystemCategory;

enum ErcWktFlavor { wktFlvrNone = 0,
                    wktFlvrOgc,
                    wktFlvrGeoTiff,
                    wktFlvrEsri,
                    wktFlvrOracle,
                    wktFlvrGeoTools,
                    wktFlvrEpsg,
                    wktFlvrUnknown,
                    wktFlvrAppAlt,
                    wktFlvrLclAlt
                  };

///////////////////////////////////////////////////////////////////////////////
///<summary>
/// The CCoordinateSystem class supplies operations for working with
/// coordinates in that coordinate system, including the ability to convert
/// coordinates to and from longitude latitude values, work with linear units,
/// and compute distances.
///</summary>

class CCoordinateSystem : public MgCoordinateSystem
{
public:
    CCoordinateSystem(MgCoordinateSystemCatalog* pCatalog);
    CCoordinateSystem(CREFSTRING ogcWkt);
    virtual ~CCoordinateSystem();

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Returns the base coordinate system library being used.
    ///</summary>
    static STRING GetBaseLibrary();

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Returns a copy of the coordinate system.
    ///</summary>
    MgCoordinateSystem* CreateClone();

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Returns the type of the coordinate system, arbitrary, geographic, or
    /// projected.
    ///</summary>
    ///<returns>
    /// One of the values defined in CCoordinateSystemType.
    ///</returns>
    virtual INT32 GetType();

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Converts the specified value in coordinate system units to
    /// meters.
    ///</summary>
    ///<param name="double units">
    /// The value in coordinate system units to convert.
    ///</param>
    ///<returns>
    /// The value in meters.
    ///</returns>
    virtual double ConvertCoordinateSystemUnitsToMeters(double units);

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Converts the value in meters to coordinate system units.
    ///</summary>
    ///<param name="double meters">
    /// The value in meters to convert.
    ///</param>
    ///<returns>
    /// The value in coordinate system units.
    ///</returns>
    virtual double ConvertMetersToCoordinateSystemUnits(double meters);

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Measures the euclidian distance between two coordinates.
    ///</summary>
    ///<param name="double x1">
    /// The x value that defines the starting coordinate.
    ///</param>
    ///<param name="double y1">
    /// The y value that defines the starting coordinate.
    ///</param>
    ///<param name="double x2">
    /// The x value that defines the ending coordinate.
    ///</param>
    ///<param name="double y2">
    /// The y value that defines the ending coordinate.
    ///</param>
    ///<returns>
    /// The euclidean distance from coord1 to coord2 in coordinate system units.
    ///</returns>
    virtual double MeasureEuclideanDistance(double x1, double y1, double x2, double y2);

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Measures the distance between two coordinates.
    /// This function uses the coordinate systems ellipsoid
    /// definition and a great circle algorithm to produce the result.
    ///</summary>
    ///<param name="double x1">
    /// The x value that defines the starting coordinate.
    ///</param>
    ///<param name="double y1">
    /// The y value that defines the starting coordinate.
    ///</param>
    ///<param name="double x2">
    /// The x value that defines the ending coordinate.
    ///</param>
    ///<param name="double y2">
    /// The y value that defines the ending coordinate.
    ///</param>
    ///<returns>
    /// The great circle distance from coord1 to coord2 in coordinate system units.
    ///</returns>
    virtual double MeasureGreatCircleDistance(double x1, double y1, double x2, double y2);

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Gets the angle with respect to the north of a vector formed by two
    /// coordinates.
    ///</summary>
    ///<param name="double x1">
    /// The x value that defines the first coordinate.
    ///</param>
    ///<param name="double y1">
    /// The y value that defines the first coordinate.
    ///</param>
    ///<param name="double x2">
    /// The x value that defines the second coordinate.
    ///</param>
    ///<param name="double y2">
    /// The y value that defines the second coordinate.
    ///</param>
    ///<returns>
    /// The azimuth (Angle with respect to the North) of the vector formed by
    /// the 2 specified coordinates.
    ///</returns>
    virtual double GetAzimuth(double x1, double y1, double x2, double y2);

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Computes a coordinate a given distance along a vector that is defined
    /// by a starting coordinate and an azimuth (Angle with respect to the
    /// North).
    ///</summary>
    ///<param name="double xStart">
    /// The x value that represents the start of the vector.
    ///</param>
    ///<param name="double yStart">
    /// The y value that represents the start of the vector.
    ///</param>
    ///<param name="double azimuth">
    /// An azimuth (Angle with respect to the North) that defines the direction
    /// of the vector.
    ///</param>
    ///<param name="double distance">
    /// The distance along the vector of the desired coordinate in coordinate system units.
    ///</param>
    ///<param name="double& x">
    /// The x value that represents the resultant coordinate.
    ///</param>
    ///<param name="double& y">
    /// The y value that represents the resultant coordinate.
    ///</param>
    ///<returns>
    /// Nothing.
    ///</returns>
    virtual void GetCoordinate(double xStart, double yStart, double azimuth, double distance, double& x, double& y);

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Returns the OGC WKT string representation of the coordinate system.
    ///</summary>
    ///<returns>
    /// OGC WKT string.
    ///</returns>
    virtual STRING ToString();

    /////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the coordinate system units.
    /// </summary>
    ///<returns>
    /// coordinate system units string
    ///</returns>
    STRING GetUnits();

    /////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the coordinate system units scale.
    /// </summary>
    ///<returns>
    /// coordinate system unit scale
    ///</returns>
    double GetUnitScale();

    /////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the minimum X value.
    /// </summary>
    ///<returns>
    /// coordinate system minimum X value
    ///</returns>
    double GetMinX();

    /////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the minimum Y value.
    /// </summary>
    ///<returns>
    /// coordinate system minimum Y value
    ///</returns>
    double GetMinY();

    /////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the maximum X value.
    /// </summary>
    ///<returns>
    /// coordinate system maximum X value
    ///</returns>
    double GetMaxX();

    /////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the maximum Y value.
    /// </summary>
    ///<returns>
    /// coordinate system maximum Y value
    ///</returns>
    double GetMaxY();

    /////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the coordinate system code.
    /// </summary>
    ///<returns>
    /// coordinate system code string
    ///</returns>
    STRING GetCode();

    /////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the coordinate system description.
    /// </summary>
    ///<returns>
    /// coordinate system description string
    ///</returns>
    STRING GetDescription();

    /////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the coordinate system projection.
    /// </summary>
    ///<returns>
    /// coordinate system projection string
    ///</returns>
    STRING GetProjection();

    /////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the coordinate system projection description.
    /// </summary>
    ///<returns>
    /// coordinate system projection description string
    ///</returns>
    STRING GetProjectionDescription();

    /////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the coordinate system datum.
    /// </summary>
    ///<returns>
    /// coordinate system datum string
    ///</returns>
    STRING GetDatum();

    /////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the coordinate system datum description.
    /// </summary>
    ///<returns>
    /// coordinate system datum description string
    ///</returns>
    STRING GetDatumDescription();

    /////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the coordinate system ellipsoid.
    /// </summary>
    ///<returns>
    /// coordinate system ellipsoid string
    ///</returns>
    STRING GetEllipsoid();

    /////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the coordinate system ellipsoid description.
    /// </summary>
    ///<returns>
    /// coordinate system ellipsoid description string
    ///</returns>
    STRING GetEllipsoidDescription();

    /////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the internal coordinate system.
    /// </summary>
    OGRSpatialReference* GetInternalCoordinateSystem() { return m_ogrSrs; }

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Converts the specified longitude latitude to a coordinate in this
    /// coordinate system. Note that no datum conversion is applied in
    /// this conversion. Use an instance of CCoordinateSystemTransform if
    /// datum conversion is desired. An exception is thrown if the conversion
    /// from longitude latitude to this coordinate system is not supported.
    /// The longitude and latitude coordinate system used is based on WGS84.
    ///</summary>
    ///<param name="double lon">
    /// The longitude value to convert.
    ///</param>
    ///<param name="double lat">
    /// The latitude value to convert.
    ///</param>
    ///<param name="double& x">
    /// The converted longitude value.
    ///</param>
    ///<param name="double& y">
    /// The converted latitude value.
    ///</param>
    ///<returns>
    /// Nothing.
    ///</returns>
    virtual void ConvertFromLonLat(double lon, double lat, double& x, double& y);

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Converts the specified longitude latitude to a coordinate in this
    /// coordinate system. Note that no datum conversion is applied in
    /// this conversion. Use an instance of CCoordinateSystemTransform if
    /// datum conversion is desired. An exception is thrown if the conversion
    /// from longitude latitude to this coordinate system is not supported.
    /// The longitude and latitude coordinate system used is based on WGS84.
    ///</summary>
    ///<param name="double lon[]">
    /// The longitude values to convert.
    ///</param>
    ///<param name="double lat[]">
    /// The latitude values to convert.
    ///</param>
    ///<param name="double x[]">
    /// The converted longitude values.
    ///</param>
    ///<param name="double y[]">
    /// The converted latitude values.
    ///</param>
    ///<param name="int arraySize">
    /// The size of the arrays. All the arrays must be the same size.
    ///</param>
    ///<returns>
    /// Nothing.
    ///</returns>
    virtual void ConvertFromLonLat(double lon[], double lat[], double x[], double y[], int arraySize);

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Converts the specified coordinate in this coordinate system to a
    /// longitude latitude value. Note that no datum conversion is applied in
    /// this conversion. Use an instance of CCoordinateSystemTransform if
    /// datum conversion is desired. An exception is thrown if the conversion
    /// from this coordinate system to longitude latitude is not supported.
    /// The longitude and latitude coordinate system used is based on WGS84.
    ///</summary>
    ///<param name="double x">
    /// The value to convert to a longitude value.
    ///</param>
    ///<param name="double y">
    /// The value to convert to a latitude value.
    ///</param>
    ///<param name="double& lon">
    /// The longitude value.
    ///</param>
    ///<param name="double& lat">
    /// The latitude value.
    ///</param>
    ///<returns>
    /// Nothing.
    ///</returns>
    virtual void ConvertToLonLat(double x, double y, double& lon, double& lat);

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Converts the specified coordinate in this coordinate system to a
    /// longitude latitude value. Note that no datum conversion is applied in
    /// this conversion. Use an instance of CCoordinateSystemTransform if
    /// datum conversion is desired. An exception is thrown if the conversion
    /// from this coordinate system to longitude latitude is not supported.
    /// The longitude and latitude coordinate system used is based on WGS84.
    ///</summary>
    ///<param name="double x[]">
    /// The values to convert to a longitude values.
    ///</param>
    ///<param name="double y[]">
    /// The values to convert to a latitude values.
    ///</param>
    ///<param name="double lon[]">
    /// The longitude values.
    ///</param>
    ///<param name="double lat[]">
    /// The latitude values.
    ///</param>
    ///<param name="int arraySize">
    /// The size of the arrays. All the arrays must be the same size.
    ///</param>
    ///<returns>
    /// Nothing.
    ///</returns>
    virtual void ConvertToLonLat(double x[], double y[], double lon[], double lat[], int arraySize);

    virtual MgCoordinate* ConvertFromLonLat(MgCoordinate* lonLat);
    virtual MgCoordinate* ConvertToLonLat(MgCoordinate* coordinate);
    virtual double MeasureEuclideanDistance(MgCoordinate* coord1, MgCoordinate* coord2);
    virtual double MeasureGreatCircleDistance(MgCoordinate* coord1, MgCoordinate* coord2);
    virtual double GetAzimuth(MgCoordinate* coord1, MgCoordinate* coord2);
    virtual MgCoordinate* GetCoordinate(MgCoordinate* coord, double azimuth, double distance);
    virtual MgCoordinate* GetCoordinate(double xStart, double yStart, double azimuth, double distance);

    virtual MgCoordinateSystemMeasure* GetMeasure();
    virtual MgStringCollection* GetCategories();
    virtual MgCoordinateSystemCatalog* GetCatalog();
    virtual void SetCode(CREFSTRING sCode);
    virtual bool IsLegalCode(CREFSTRING sCode);
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

    virtual UINT8* SerializeFrom(UINT8* pStream);
    virtual UINT8* SerializeTo(UINT8* pStream);
    virtual UINT32 GetSizeSerialized();

    // Coordinate system information properties
    static const STRING CoordinateSystemCode;
    static const STRING CoordinateSystemDescription;
    static const STRING CoordinateSystemProjection;
    static const STRING CoordinateSystemProjectionDescription;
    static const STRING CoordinateSystemDatum;
    static const STRING CoordinateSystemDatumDescription;
    static const STRING CoordinateSystemEllipsoid;
    static const STRING CoordinateSystemEllipsoidDescription;

    static const STRING LatLonWkt;

    static const STRING BaseLibrary;

    static STRING ConvertWktToCoordinateSystemCode(CREFSTRING ogcWkt);
    static STRING ConvertCoordinateSystemCodeToWkt(CREFSTRING csCode);
    static STRING ConvertEpsgCodeToWkt(long code);
    static long ConvertWktToEpsgCode(CREFSTRING wkt);

protected:
    //from MgDisposable
    virtual void Dispose();

protected:
    //Data members
    STRING m_ogcWkt;
    int m_coordinateSystemType;
    OGRSpatialReference* m_ogrSrs;
    OGRSpatialReference* m_latLonSrs;
    OGRCoordinateTransformation* m_transformForward;
    OGRCoordinateTransformation* m_transformInverse;

    STRING m_units;
    double m_unitScale;
    double m_minX;
    double m_minY;
    double m_maxX;
    double m_maxY;
    STRING m_code;
    STRING m_description;
    STRING m_projection;
    STRING m_projectionDescription;
    STRING m_datum;
    STRING m_datumDescription;
    STRING m_ellipsoid;
    STRING m_ellipsoidDescription;
    STRING m_category;

private:
    CCoordinateSystem();
    void Cleanup();
};

} // End of namespace

#endif //_CCOORDINATESYSTEM_H_

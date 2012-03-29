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

#ifndef _MGCOORDINATESYSTEM_H_
#define _MGCOORDINATESYSTEM_H_

class MgCoordinateSystemCatalog;
class MgCoordinateSystemDatum;
class MgCoordinateSystemEllipsoid;
class MgCoordinateSystemMeasure;
class MgCoordinateSystemEnumInteger32;

/// \defgroup MgCoordinateSystem MgCoordinateSystem
/// \ingroup Coordinate_System_classes
/// \{
class MgCoordinateSystem : public MgGuardDisposable
{
    DECLARE_CLASSNAME(MgCoordinateSystem)

PUBLISHED_API:
    virtual INT32 GetType()=0;  /// __get
    virtual MgCoordinate* ConvertFromLonLat(MgCoordinate* lonLat)=0;
    virtual MgCoordinate* ConvertToLonLat(MgCoordinate* coordinate)=0;
    virtual double ConvertCoordinateSystemUnitsToMeters(double units)=0;
    virtual double ConvertMetersToCoordinateSystemUnits(double meters)=0;
    virtual double MeasureEuclideanDistance(MgCoordinate* coord1, MgCoordinate* coord2)=0;
    virtual double MeasureEuclideanDistance(double x1, double y1, double x2, double y2)=0;
    virtual double MeasureGreatCircleDistance(MgCoordinate* coord1, MgCoordinate* coord2)=0;
    virtual double MeasureGreatCircleDistance(double x1, double y1, double x2, double y2)=0;
    virtual double GetAzimuth(MgCoordinate* coord1, MgCoordinate* coord2)=0;
    virtual double GetAzimuth(double x1, double y1, double x2, double y2)=0;
    virtual MgCoordinate* GetCoordinate(MgCoordinate* coord, double azimuth, double distance)=0;
    virtual MgCoordinate* GetCoordinate(double xStart, double yStart, double azimuth, double distance)=0;
    virtual STRING ToString()=0;
    virtual STRING GetUnits()=0;  /// __get
    virtual double GetUnitScale()=0;  /// __get
    virtual double GetMinX()=0;  /// __get
    virtual double GetMinY()=0;  /// __get
    virtual double GetMaxX()=0;  /// __get
    virtual double GetMaxY()=0;  /// __get
    virtual STRING GetCsCode()=0;  /// __get
    virtual STRING GetDescription()=0;  /// __get
    virtual STRING GetProjection()=0;  /// __get
    virtual STRING GetProjectionDescription()=0;  /// __get
    virtual STRING GetDatum()=0;  /// __get
    virtual STRING GetDatumDescription()=0;  /// __get
    virtual STRING GetEllipsoid()=0;  /// __get
    virtual STRING GetEllipsoidDescription()=0;  /// __get

    virtual INT32 GetEpsgCode (void)=0;  /// __get
    virtual INT32 GetSridCode (void)=0;  /// __get
    virtual INT16 GetEpsgQuadrant (void)=0;   /// __get

    virtual MgCoordinateSystemMeasure* GetMeasure()=0;
    virtual MgStringCollection* GetCategories()=0;
    virtual MgCoordinateSystemCatalog* GetCatalog()=0;
    virtual void SetCsCode(CREFSTRING sCode)=0;
    virtual bool IsLegalCsCode(CREFSTRING sCode)=0;
    virtual bool IsValid()=0;
    virtual bool IsUsable(MgCoordinateSystemCatalog *pCatalog)=0;
    virtual bool IsSameAs(MgGuardDisposable *pDef)=0;
    virtual void SetDescription(CREFSTRING sDesc)=0;
    virtual bool IsLegalDescription(CREFSTRING sDesc)=0;
    virtual STRING GetGroup()=0;
    virtual void SetGroup(CREFSTRING sGroup)=0;
    virtual bool IsLegalGroup(CREFSTRING sGroup)=0;
    virtual STRING GetSource()=0;
    virtual void SetSource(CREFSTRING sSource)=0;
    virtual bool IsLegalSource(CREFSTRING sSource)=0;
    virtual bool IsProtected()=0;
    virtual INT16 GetAge()=0;
    virtual void SetProtectMode(bool bIsProtected)=0;
    virtual bool IsEncrypted()=0;
    virtual void SetEncryptMode(bool bIsEncrypted)=0;
    virtual MgCoordinateSystemEnumInteger32* GetErrors()=0;
    virtual STRING GetLocation()=0;
    virtual void SetLocation(CREFSTRING sLoc)=0;
    virtual bool IsLegalLocation(CREFSTRING sLoc)=0;
    virtual STRING GetCountryOrState()=0;
    virtual void SetCountryOrState(CREFSTRING sCountryOrState)=0;
    virtual bool IsLegalCountryOrState(CREFSTRING sCountryOrState)=0;
    virtual bool IsGeodetic()=0;
    virtual INT32 GetProjectionCode()=0;
    virtual void SetProjectionCode(INT32  prjType)=0;
    virtual INT32 GetUnitCode()=0;
    virtual void SetUnitCode(INT32 unit)=0;
    virtual INT32 GetProjectionParameterCount()=0;
    virtual double GetProjectionParameter(INT32 nIndex)=0;
    virtual void SetProjectionParameter(INT32 nIndex, double dValue)=0;
    virtual double GetOriginLongitude()=0;
    virtual void SetOriginLongitude(double dOrgLng)=0;
    virtual double GetOriginLatitude()=0;
    virtual void SetOriginLatitude(double dOrgLat)=0;
    virtual double GetOffsetX()=0;
    virtual double GetOffsetY()=0;
    virtual void SetOffsets(double dXOffset, double dYOffset)=0;
    virtual double GetScaleReduction()=0;
    virtual void SetScaleReduction(double dSclRed)=0;
    virtual double GetMapScale()=0;
    virtual void SetMapScale(double dMapScale)=0;
    virtual double GetZeroX()=0;
    virtual double GetZeroY()=0;
    virtual void SetZeroes(double dXZero, double dYZero)=0;
    
    
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns this system's minimum longitude value in degrees. If not set in the dictionary, 
    /// the value returned here has been calculated by the underlying coordinate system
    /// transformation engine. 
    /// \return
    /// Returns this system's minimum longitude value in degrees. Only returns 0 (if it's not the
    /// actual value) if explicitely set via SetLonLatBounds or CancelLonLatBounds.
    virtual double GetLonMin()=0;
    
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns this system's maximum longitude value in degrees. If not set in the dictionary, 
    /// the value returned here has been calculated by the underlying coordinate system
    /// transformation engine. 
    /// \return
    /// Returns this system's minimum longitude value in degrees. Only returns 0 (if it's not the
    /// actual value) if explicitely set via SetLonLatBounds or CancelLonLatBounds.
    virtual double GetLonMax()=0;
    
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns this system's minimum latitude value in degrees. If not set in the dictionary, 
    /// the value returned here has been calculated by the underlying coordinate system
    /// transformation engine. 
    /// \return
    /// Returns this system's minimum longitude value in degrees. Only returns 0 (if it's not the
    /// actual value) if explicitely set via SetLonLatBounds or CancelLonLatBounds.
    virtual double GetLatMin()=0;
    
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns this system's maximum latitude value in degrees. If not set in the dictionary, 
    /// the value returned here has been calculated by the underlying coordinate system
    /// transformation engine. 
    /// \return
    /// Returns this system's minimum longitude value in degrees. Only returns 0 (if it's not the
    /// actual value) if explicitely set via SetLonLatBounds or CancelLonLatBounds.
    virtual double GetLatMax()=0;
    
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Allows to set the min/max longitude and latitude values in degrees this coordinate system is valid in.
    /// This method must only be called on systems that are not protected.
    /// When setting these values, the coordinate system API will perform the following actions:
    ///
    /// \li (Re-)enable valid range checking: When converting coordinates, the values are checked to fall
    ///     in the valid range as specified here
    /// \li Save the values for later storage: When eventually writing this coordinate system to the
    ///     dictionary, the values will be written, too.
    ///
    /// \param dLonMin
    /// The minimum longitude value
    /// \param dLatMin
    /// The minimum latitude value
    /// \param dLonMax
    /// The maximum longitude value
    /// \param dLatMax
    /// The maximum latitude value
    ///    
    /// \return
    /// Nothing
    virtual void SetLonLatBounds(double dLonMin, double dLatMin, double dLonMax, double dLatMax)=0;
    
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Cancels only the range checking for coordinate transformations, if it has been enabled
    /// via SetLonLatBounds before. Specifically, calling this method does not reset the
    /// min/max values on the coordinate system definition itself. That is, when eventually writing
    /// this object to the dictionary, the min/max values will be written, too.
    ///
    /// \return
    /// Nothing
    virtual void CancelLonLatBounds()=0;
    
    virtual void SetXYBounds(double dXMin, double dYMin, double dXMax, double dYMax)=0;
    virtual void CancelXYBounds()=0;
    virtual INT16 GetQuadrant()=0;
    virtual void SetQuadrant(INT16 sQuad)=0;
    virtual MgCoordinateSystemDatum* GetDatumDefinition()=0;
    virtual void SetDatumDefinition(MgCoordinateSystemDatum *pDatum)=0;
    virtual MgCoordinateSystemEllipsoid* GetEllipsoidDefinition()=0;
    virtual void SetEllipsoidDefinition(MgCoordinateSystemEllipsoid *pEllipsoid)=0;
    virtual bool IsValidXY(double dX, double dY)=0;
    virtual bool IsValidLonLat(double dLongitude, double dLatitude)=0;

    virtual void ConvertCoordinateFromLonLat(MgCoordinate* lonLatToCoordinate)=0;
    virtual void ConvertCoordinateToLonLat(MgCoordinate* coordinateToLonLat)=0;

    virtual MgCoordinate* ConvertToLonLat(double dX, double dY)=0;
    virtual MgCoordinate* ConvertToLonLat(double dX, double dY, double dZ)=0;
    virtual MgCoordinate* ConvertFromLonLat(double dLongitude, double dLatitude)=0;
    virtual MgCoordinate* ConvertFromLonLat(double dLongitude, double dLatitude, double dZ)=0;

    virtual double GetConvergence(double dLongitude, double dLatitude)=0;
    virtual double GetScale(double dLongitude, double dLatitude)=0;
    virtual double GetScaleH(double dLongitude, double dLatitude)=0;
    virtual double GetScaleK(double dLongitude, double dLatitude)=0;
    virtual MgCoordinateSystem* CreateClone()=0;

INTERNAL_API:
    virtual UINT8* SerializeFrom(UINT8* pStream)=0;
    virtual UINT8* SerializeTo(UINT8* pStream)=0;
    virtual UINT32 GetSizeSerialized()=0;

    ///////////////////////////////////////////////////////////////////////////
    /// \internal
    /// \brief
    /// INTERNAL METHOD ONLY
    /// Converts the specified longitude latitude to a coordinate in this
    /// coordinate system. Note that no datum conversion is applied in
    /// this conversion. Use an instance of MgCoordinateSystemTransform if
    /// datum conversion is desired. An exception is thrown if the conversion
    /// from longitude latitude to this coordinate system is not supported.
    /// The longitude and latitude coordinate system used is based on WGS84.
    ///
    /// \param lon
    /// The longitude value to convert.
    /// \param lat
    /// The latitude value to convert.
    /// \param x
    /// The converted longitude value.
    /// \param y
    /// The converted latitude value.
    ///
    /// \return
    /// Nothing.
    ///
    virtual void ConvertFromLonLat(double lon, double lat, double& x, double& y)=0;

    ///////////////////////////////////////////////////////////////////////////
    /// \internal
    /// \brief
    /// INTERNAL METHOD ONLY
    /// Converts the specified longitude latitude to a coordinate in this
    /// coordinate system. Note that no datum conversion is applied in
    /// this conversion. Use an instance of MgCoordinateSystemTransform if
    /// datum conversion is desired. An exception is thrown if the conversion
    /// from longitude latitude to this coordinate system is not supported.
    /// The longitude and latitude coordinate system used is based on WGS84.
    ///
    /// \param lon
    /// The longitude values to convert.
    /// \param lat
    /// The latitude values to convert.
    /// \param x
    /// The converted longitude values.
    /// \param y
    /// The converted latitude values.
    /// \param arraySize
    /// The size of the arrays. All the arrays must be the same size.
    ///
    /// \return
    /// Nothing.
    ///
    virtual void ConvertFromLonLat(double lon[], double lat[], double x[], double y[], int arraySize)=0;

    ///////////////////////////////////////////////////////////////////////////
    /// \internal
    /// \brief
    /// INTERNAL METHOD ONLY
    /// Converts the specified coordinate in this coordinate system to a
    /// longitude latitude value. Note that no datum conversion is applied in
    /// this conversion. Use an instance of MgCoordinateSystemTransform if
    /// datum conversion is desired. An exception is thrown if the conversion
    /// from this coordinate system to longitude latitude is not supported.
    /// The longitude and latitude coordinate system used is based on WGS84.
    ///
    /// \param x
    /// The value to convert to a longitude value.
    /// \param y
    /// The value to convert to a latitude value.
    /// \param lon
    /// The longitude value.
    /// \param lat
    /// The latitude value.
    ///
    /// \return
    /// Nothing.
    ///
    virtual void ConvertToLonLat(double x, double y, double& lon, double& lat)=0;

    ///////////////////////////////////////////////////////////////////////////
    /// \internal
    /// \brief
    /// INTERNAL METHOD ONLY
    /// Converts the specified coordinate in this coordinate system to a
    /// longitude latitude value. Note that no datum conversion is applied in
    /// this conversion. Use an instance of MgCoordinateSystemTransform if
    /// datum conversion is desired. An exception is thrown if the conversion
    /// from this coordinate system to longitude latitude is not supported.
    /// The longitude and latitude coordinate system used is based on WGS84.
    ///
    /// \param x
    /// The values to convert to a longitude values.
    /// \param y
    /// The values to convert to a latitude values.
    /// \param lon
    /// The longitude values.
    /// \param lat
    /// The latitude values.
    /// \param arraySize
    /// The size of the arrays. All the arrays must be the same size.
    ///
    /// \return
    /// Nothing.
    ///
    virtual void ConvertToLonLat(double x[], double y[], double lon[], double lat[], int arraySize)=0;

protected:
    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the class Id
    ///
    /// \return
    /// The integer value
    ///
    INT32 GetClassId(){return m_cls_id;};

CLASS_ID:
    static const INT32 m_cls_id = CoordinateSystem_CoordinateSystem;
};

/// \}

#endif //_MGCOORDINATESYSTEM_H_

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

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the type of the coordinate system: arbitrary, geographic, or projected. 
    ///
    /// \return
    /// One of the values defined in MgCoordinateSystemType. 
    ///
    virtual INT32 GetType()=0;  /// __get

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Converts the specified MgCoordinate in lon/lat units to coordinate system units
    ///
    /// \param lonLat (MgCoordinate)
    /// The MgCoordinate in lon/lat units
    ///
    /// \return
    /// The converted MgCoordinate in coordinate system units
    ///
    virtual MgCoordinate* ConvertFromLonLat(MgCoordinate* lonLat)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Converts the specified MgCoordinate in coordinate system units to lon/lat units
    ///
    /// \param coordinate (MgCoordinate)
    /// The MgCoordinate in coordinate system units
    ///
    /// \return
    /// The converted MgCoordinate in lon/lat units
    ///
    virtual MgCoordinate* ConvertToLonLat(MgCoordinate* coordinate)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Converts the given value in coordinate system units to meters
    ///
    /// \param units (double)
    /// The value in coordinate system units
    ///
    /// \return
    /// The converted value in meters
    ///
    virtual double ConvertCoordinateSystemUnitsToMeters(double units)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Converts the given value in meters to coordinate system units
    ///
    /// \param meters (double)
    /// The value in meters
    ///
    /// \return
    /// The converted value in coordinate system unit
    ///
    virtual double ConvertMetersToCoordinateSystemUnits(double meters)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Measures the euclidean distance between two coordinates in coordinate system units. 
    ///
    /// \param coord1 (MgCoordinate)
    /// The first coordinate
    /// \param coord2 (MgCoordinate)
    /// The second coordinate
    ///
    /// \return
    /// The euclidean distance from coord1 to coord2 in coordinate system units.
    ///
    virtual double MeasureEuclideanDistance(MgCoordinate* coord1, MgCoordinate* coord2)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Measures the euclidean distance between (x1, y1) and (x2, y2) in coordinate system units. 
    ///
    /// \param x1
    /// The first X coordinate
    /// \param y1
    /// The first Y coordinate
    /// \param x2
    /// The second X coordinate
    /// \param y2
    /// The second Y coordinate
    ///
    /// \return
    /// The euclidean distance from (x1, y1) to (x2, y2) in coordinate system units.
    ///
    virtual double MeasureEuclideanDistance(double x1, double y1, double x2, double y2)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Measures the great circle distance between two coordinates in coordinate system units
    ///
    /// \param coord1 (MgCoordinate)
    /// The first coordinate
    /// \param coord2 (MgCoordinate)
    /// The second coordinate
    ///
    /// \return
    /// The great circle distance from coord1 to coord2 in coordinate system units
    ///
    virtual double MeasureGreatCircleDistance(MgCoordinate* coord1, MgCoordinate* coord2)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Measures the distance between two coordinates (x1, y1) and (x2, y2) in coordinate system units
    ///
    /// \param x1
    /// The first X coordinate
    /// \param y1
    /// The first Y coordinate
    /// \param x2
    /// The second X coordinate
    /// \param y2
    /// The second Y coordinate
    ///
    /// \return
    /// The great circle distance from (x1, y1) to (x2, y2) in coordinate system units
    ///
    virtual double MeasureGreatCircleDistance(double x1, double y1, double x2, double y2)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the angle with respect to North of a vector formed by two coordinates
    ///
    /// \param coord1 (MgCoordinate)
    /// The first coordinate
    /// \param coord2 (MgCoordinate)
    /// The second coordinate
    ///
    /// \return
    /// The azimuth (Angle with respect to North) of the vector formed by coord1 and coord2
    ///
    /// \remarks
    /// This method is specific to a geographic coordinate system. The angle is positive moving clockwise from North and negative moving counterclockwise from North. If the first coordinate is above and to the left of the second, or below and to the left, the movement is clockwise. If the first coordinate is above and to the right, or below and to the right, the movement is counterclockwise
    ///
    virtual double GetAzimuth(MgCoordinate* coord1, MgCoordinate* coord2)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the angle with respect to North of a vector formed by two coordinate
    ///
    /// \param x1
    /// The first X coordinate
    /// \param y1
    /// The first Y coordinate
    /// \param x2
    /// The second X coordinate
    /// \param y2
    /// The second Y coordinate
    ///
    /// \return
    /// The azimuth (Angle with respect to the North) of the vector formed by (x1, y1) and (x2, y2)
    ///
    /// \remarks
    /// This method is specific to a geographic coordinate system. The angle is positive moving clockwise from North and negative moving counterclockwise from North. If the first coordinate is above and to the left of the second, or below and to the left, the movement is clockwise. If the first coordinate is above and to the right, or below and to the right, the movement is counterclockwise
    ///
    virtual double GetAzimuth(double x1, double y1, double x2, double y2)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Computes a coordinate a given distance along a vector that is defined by a starting coordinate and an azimuth (Angle with respect to North). 
    ///
    /// \param coord (MgCoordinate)
    /// An MgCoordinate object in coordinate system
    /// units that represents the start of the
    /// vector.
    /// \param azimuth (double)
    /// An azimuth (Angle with respect to the North)
    /// that defines the direction of the vector.
    /// \param distance (double)
    /// The distance along the vector to the desired
    /// coordinate in meters.
    ///
    /// \return
    /// An MgCoordinate that lies the given distance along the vector.
    ///
    /// \remarks
    /// This method is specific to a geographic coordinate system. The values contained in the given and returned coordinates are in units native to the coordinate system as specified in the <name> parameter in the UNIT[] clause of the coordinate system WKT. The azimuth is always in decimal degrees, and the distance is always in meters
    ///
    virtual MgCoordinate* GetCoordinate(MgCoordinate* coord, double azimuth, double distance)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Computes a coordinate a given distance along a vector that is defined by a starting (x, y) coordinate and an azimuth (Angle with respect to North). 
    ///
    /// \param xStart (double)
    /// The x value in coordinate system units of the
    /// starting coordinate.
    /// \param yStart (double)
    /// The y value in coordinate system units of the
    /// starting coordinate.
    /// \param azimuth (double)
    /// An azimuth (Angle with respect to the North)
    /// that defines the direction of the vector.
    /// \param distance (double)
    /// The distance along the vector to the desired
    /// coordinate in meters.
    ///
    /// \return
    /// An MgCoordinate that lies the given distance along the vector.
    ///
    /// \remarks
    /// This method is specific to a geographic coordinate system. The values contained in the given and returned coordinates are in units native to the coordinate system as specified in the <name> parameter in the UNIT[] clause of the coordinate system WKT. The azimuth is always in decimal degrees, and the distance is always in meters
    ///
    virtual MgCoordinate* GetCoordinate(double xStart, double yStart, double azimuth, double distance)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the OGC WKT string representation of the coordinate system. 
    ///
    /// \return
    /// The OGC WKT string. 
    ///
    virtual STRING ToString()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the coordinate system units
    ///
    /// \return
    /// The coordinate system units string
    ///
    virtual STRING GetUnits()=0;  /// __get

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the factor used to convert coordinate system units to meters
    ///
    /// \return
    /// The factor used to convert coordinate system units to meters 
    ///
    virtual double GetUnitScale()=0;  /// __get

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the minimum X value. 
    ///
    /// \return
    /// The coordinate system minimum X value
    ///
    virtual double GetMinX()=0;  /// __get

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the minimum Y value. 
    ///
    /// \return
    /// The coordinate system minimum Y value
    ///
    virtual double GetMinY()=0;  /// __get

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the maximum X value
    ///
    /// \return
    /// The coordinate system maximum X value
    ///
    virtual double GetMaxX()=0;  /// __get

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the maximum Y value
    ///
    /// \return
    /// The coordinate system maximum Y value
    ///
    virtual double GetMaxY()=0;  /// __get

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the CS-Map coordinate system code
    ///
    /// \return
    /// The CS-Map coordinate system code
    ///
    /// \remarks
    /// This method used to be called GetCode prior to MapGuide Open Source 2.2 / MapGuide Enterprise 2011 / AutoCAD Map3D 2011
    ///
    virtual STRING GetCsCode()=0;  /// __get

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the description of this coordinate system
    ///
    /// \return
    /// The description of this coordinate system
    ///
    virtual STRING GetDescription()=0;  /// __get

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the projection of this coordinate system
    ///
    /// \return
    /// The projection of this coordinate system
    ///
    virtual STRING GetProjection()=0;  /// __get

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the projection description of this coordinate system
    ///
    /// \return
    /// The projection description of this coordinate system
    ///
    virtual STRING GetProjectionDescription()=0;  /// __get

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the datum of this coordinate system
    ///
    /// \remarks
    /// The returned value comes from the first parameter (<name>) of the DATUM[] clause in the coordinate system WKT. See \ref WKTofSRS .
    ///
    /// \return
    /// The coordinate system datum string
    ///
    virtual STRING GetDatum()=0;  /// __get

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the datum description of this coordinate system
    ///
    /// \return
    /// The datum description string of this coordinate system
    ///
    virtual STRING GetDatumDescription()=0;  /// __get

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the ellipsoid of this coordinate system
    ///
    /// \return
    /// The ellipsoid string of this coordinate system
    ///
    virtual STRING GetEllipsoid()=0;  /// __get

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the ellipsoid description of this coordinate system
    ///
    /// \return
    /// The ellipsoid description string of this coordinate system
    ///
    virtual STRING GetEllipsoidDescription()=0;  /// __get

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the EPSG code of this coordinate system
    ///
    /// \return
    /// The EPSG code of this coordinate system
    ///
    /// \since 2.3
    ///
    virtual INT32 GetEpsgCode (void)=0;  /// __get

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the SRID code of this coordinate system
    ///
    /// \return
    /// The SRID code of this coordinate system
    ///
    /// \since 2.3
    ///
    virtual INT32 GetSridCode (void)=0;  /// __get

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the EPSG quadrant of this coordinate system
    ///
    /// \return
    /// The EPSG quadrant of this coordinate system
    ///
    /// \since 2.3
    ///
    virtual INT16 GetEpsgQuadrant (void)=0;   /// __get

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the measure object of this coordinate system
    ///
    /// \return
    /// The MgCoordinateSystemMeasure object of this coordinate system
    ///
    /// \since 2.0
    ///
    virtual MgCoordinateSystemMeasure* GetMeasure()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the categories of this coordinate system
    ///
    /// \return
    /// The MgStringCollection containing the cateogories for this coordinate system
    ///
    virtual MgStringCollection* GetCategories()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the associated coordinate system catalog for this coordinate system
    ///
    /// \return
    /// The associated MgCoordinateSystemCatalog object
    ///
    virtual MgCoordinateSystemCatalog* GetCatalog()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the CS-Map coordinate system code for this coordinate system
    ///
    /// \param sCode (String/string)
    /// The CS-Map coordinate system code
    ///
    /// \remarks
    /// This method used to be called SetCode prior to MapGuide Open Source 2.2 / MapGuide Enterprise 2011 / AutoCAD Map3D 2011
    ///
    virtual void SetCsCode(CREFSTRING sCode)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Checks whether the given CS-Map coordinate system code is a legal one
    ///
    /// \return
    /// true if the given code is a legal one, false otherwise
    ///
    /// \remarks
    /// This method used to be called IsLegalCode prior to MapGuide Open Source 2.2 / MapGuide Enterprise 2011 / AutoCAD Map3D 2011
    ///
    virtual bool IsLegalCsCode(CREFSTRING sCode)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Checks whether this coordinate system is a valid one
    ///
    /// \return
    /// true if this coordinate system is a valid one. false otherwise
    ///
    virtual bool IsValid()=0;
    virtual bool IsUsable(MgCoordinateSystemCatalog *pCatalog)=0;
    virtual bool IsSameAs(MgGuardDisposable *pDef)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the description of this coordinate system
    ///
    /// \param sDesc (String/string)
    /// The description of this coordinate system
    ///
    virtual void SetDescription(CREFSTRING sDesc)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Checks whether this description is legal for this coordinate system
    ///
    /// \param sDesc (String/string)
    /// The description to check
    ///
    /// \return
    /// true if the given description is a legal one, false otherwise
    ///
    virtual bool IsLegalDescription(CREFSTRING sDesc)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the group of this coordinate system
    ///
    /// \return
    /// The group of this coordinate system
    ///
    virtual STRING GetGroup()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the group of this coordinate system
    ///
    /// \param sGroup (String/string)
    /// The group
    ///
    virtual void SetGroup(CREFSTRING sGroup)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Checks whether this group is legal for this coordinate system
    ///
    /// \param sGroup (String/string)
    /// The group to check
    ///
    /// \return
    /// true if the given group is a legal one, false otherwise
    ///
    virtual bool IsLegalGroup(CREFSTRING sGroup)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the source of this coordinate system
    ///
    /// \return
    /// The source of this coordinate system
    ///
    virtual STRING GetSource()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the source of this coordinate system
    ///
    /// \param sSource (String/string)
    /// The source
    ///
    virtual void SetSource(CREFSTRING sSource)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Checks whether this source is legal for this coordinate system
    ///
    /// \param sSource (String/string)
    /// The source to check
    ///
    /// \return
    /// true if the given source is a legal one, false otherwise
    ///
    virtual bool IsLegalSource(CREFSTRING sSource)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Checks whether this coordinate system is protected
    ///
    /// \return
    /// true if this coordinate system is protected. false otherwise
    ///
    virtual bool IsProtected()=0;
    virtual INT16 GetAge()=0;
    virtual void SetProtectMode(bool bIsProtected)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Checks whether this coordinate system is encrypted
    ///
    /// \return
    /// true if this coordinate system is encrypted. false otherwise
    ///
    virtual bool IsEncrypted()=0;
    virtual void SetEncryptMode(bool bIsEncrypted)=0;
    virtual MgCoordinateSystemEnumInteger32* GetErrors()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the location of this coordinate system
    ///
    /// \return
    /// The location of this coordinate system
    ///
    virtual STRING GetLocation()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the location of this coordinate system
    ///
    /// \param sLoc (String/string)
    /// The location
    ///
    virtual void SetLocation(CREFSTRING sLoc)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Checks whether this location is legal for this coordinate system
    ///
    /// \param sLoc (String/string)
    /// The location to check
    ///
    /// \return
    /// true if the given location is a legal one, false otherwise
    ///
    virtual bool IsLegalLocation(CREFSTRING sLoc)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the country or state of this coordinate system
    ///
    /// \return
    /// The country or state of this coordinate system
    ///
    virtual STRING GetCountryOrState()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the country or state of this coordinate system
    ///
    /// \param sCountryOrState (String/string)
    /// The country or state
    ///
    virtual void SetCountryOrState(CREFSTRING sCountryOrState)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Checks whether this country or state is legal for this coordinate system
    ///
    /// \param sCountryOrState (String/string)
    /// The country or state to check
    ///
    /// \return
    /// true if the given country or state is a legal one, false otherwise
    ///
    virtual bool IsLegalCountryOrState(CREFSTRING sCountryOrState)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets whether this coordinate system is geodetic
    ///
    /// \return
    /// true if this coordinate system is geodetic. false otherwise
    ///
    virtual bool IsGeodetic()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the projection code of this coordinate system (see MgCoordinateSystemProjectionCode)
    ///
    /// \return
    /// The projection code of this coordinate system
    ///
    virtual INT32 GetProjectionCode()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the projection code for this coordinate system
    ///
    /// \param prjType (int)
    /// The projection code (see MgCoordinateSystemProjectionCode)
    ///
    virtual void SetProjectionCode(INT32  prjType)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the unit code of this coordinate system
    ///
    /// \return
    /// The unit code of this coordinate system
    ///
    virtual INT32 GetUnitCode()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the unit code for this coordinate system
    ///
    /// \param unit (int)
    /// The unit code
    ///
    virtual void SetUnitCode(INT32 unit)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the projection parameter count of this coordinate system
    ///
    /// \return
    /// The projection parameter count of this coordinate system
    ///
    virtual INT32 GetProjectionParameterCount()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the projection parameter count of this coordinate system at the specified index
    ///
    /// \param nIndex (int)
    /// The index at which to retrieve the projection parameter
    ///
    /// \return
    /// The projection parameter at the given index
    ///
    virtual double GetProjectionParameter(INT32 nIndex)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the projection parameter of this coordinate system at the specified index
    ///
    /// \param nIndex (int)
    /// The index at which to set the projection parameter
    /// \param dValue (double)
    /// The projection parameter value
    ///
    virtual void SetProjectionParameter(INT32 nIndex, double dValue)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the origin longitude of this coordinate system
    ///
    /// \return
    /// The origin longitude of this coordinate system
    ///
    virtual double GetOriginLongitude()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the origin longitude for this coordinate system
    ///
    /// \param dOrgLng (double)
    /// The origin longitude
    ///
    virtual void SetOriginLongitude(double dOrgLng)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the origin latitude of this coordinate system
    ///
    /// \return
    /// The origin latitude of this coordinate system
    ///
    virtual double GetOriginLatitude()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the origin latitude for this coordinate system
    ///
    /// \param dOrgLat (double)
    /// The origin latitude
    ///
    virtual void SetOriginLatitude(double dOrgLat)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the X offset of this coordinate system
    ///
    /// \return
    /// The X offset of this coordinate system
    ///
    virtual double GetOffsetX()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the Y offset of this coordinate system
    ///
    /// \return
    /// The Y offset of this coordinate system
    ///
    virtual double GetOffsetY()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the X and Y offsets for this coordinate system
    ///
    /// \param dXOffset (double)
    /// The X offset
    /// \param dYOffset (double)
    /// The Y offset
    ///
    virtual void SetOffsets(double dXOffset, double dYOffset)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the scale reduction of this coordinate system
    ///
    /// \return
    /// The scale reduction of this coordinate system
    ///
    virtual double GetScaleReduction()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the scale reduction for this coordinate system
    ///
    /// \param dSclRed (double)
    /// The scale reduction
    ///
    virtual void SetScaleReduction(double dSclRed)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the map scale for this coordinate system
    ///
    /// \return
    /// The map scale for this coordinate system
    ///
    virtual double GetMapScale()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the map scale for this coordinate system
    ///
    /// \param dMapScale (double)
    /// The map scale
    ///
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
    /// <ol>
    /// <li>(Re-)enable valid range checking: When converting coordinates, the values are checked to fall
    ///     in the valid range as specified here</li>
    /// <li>Save the values for later storage: When eventually writing this coordinate system to the
    ///     dictionary, the values will be written, too.</li>
    /// </ol>
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
    
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Allows to set the min/max X and Y values in the coordinate system's units that this coordinate system is valid in.
    /// This method must only be called on systems that are not protected.
    /// When setting these values, the coordinate system API will perform the following actions:
    ///
    /// <ol>
    /// <li>(Re-)enable valid range checking: When converting coordinates, the values are checked to fall
    ///     in the valid range as specified here</li>
    /// <li>Save the values for later storage: When eventually writing this coordinate system to the
    ///     dictionary, the values will be written, too.</li>
    /// </ol>
    ///
    /// \param dXMin
    /// The minimum X value
    /// \param dYMin
    /// The minimum Y value
    /// \param dXMax
    /// The maximum X value
    /// \param dYMax
    /// The maximum Y value
    ///    
    /// \return
    /// Nothing
    virtual void SetXYBounds(double dXMin, double dYMin, double dXMax, double dYMax)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Cancels only the range checking for coordinate transformations, if it has been enabled via
    /// SetXYBounds
    ///
    virtual void CancelXYBounds()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the quadrant for this coordinate system
    ///
    /// \return
    /// The quadrant for this coordinate system
    ///
    virtual INT16 GetQuadrant()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the quadrant for this coordinate system
    ///
    /// \param sQuad (short/int)
    /// The quadrant
    ///
    virtual void SetQuadrant(INT16 sQuad)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the datum definition for this coordinate system
    ///
    /// \return
    /// The datum definition for this coordinate system
    ///
    virtual MgCoordinateSystemDatum* GetDatumDefinition()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the datum for this coordinate system
    ///
    /// \param pDatum (MgCoordinateSystemDatum)
    /// The datum definition
    ///
    virtual void SetDatumDefinition(MgCoordinateSystemDatum* pDatum)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the ellipsoid for this coordinate system
    ///
    /// \return
    /// The ellipsoid for this coordinate system
    ///
    virtual MgCoordinateSystemEllipsoid* GetEllipsoidDefinition()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the ellipsoid definition for this coordinate system
    ///
    /// \param pEllipsoid (MgCoordinateSystemEllipsoid)
    /// The ellipsoid definition
    ///
    virtual void SetEllipsoidDefinition(MgCoordinateSystemEllipsoid* pEllipsoid)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the given X and Y coordinates are valid within this coordinate system
    ///
    /// \param dX (double)
    /// The X coordinate
    /// \param dY (double)
    /// The Y coordinate
    ///
    /// \return
    /// true if the coordinates are valid. false otherwise
    ///
    virtual bool IsValidXY(double dX, double dY)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the given longitude and latitude coordinates are valid within this coordinate system
    ///
    /// \param dLongitude (double)
    /// The longitude coordinate
    /// \param dLatitude (double)
    /// The latitude coordinate
    ///
    /// \return
    /// true if the coordinates are valid. false otherwise
    ///
    virtual bool IsValidLonLat(double dLongitude, double dLatitude)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Converts the specified MgCoordinate in lon/lat units to coordinate system units
    ///
    /// \param lonLatToCoordinate (MgCoordinate)
    /// The MgCoordinate in lon/lat units that will be converted
    ///
    virtual void ConvertCoordinateFromLonLat(MgCoordinate* lonLatToCoordinate)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Converts the specified MgCoordinate in coordinate system units to lon/lat units
    ///
    /// \param coordinateToLonLat (MgCoordinate)
    /// The MgCoordinate in coordinate system units that will be converted
    ///
    virtual void ConvertCoordinateToLonLat(MgCoordinate* coordinateToLonLat)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Converts the specified coordinates in coordinate system units to lon/lat units
    ///
    /// \param dX (double)
    /// The X coordinate
    /// \param dY (double)
    /// The Y coordinate
    ///
    /// \return
    /// The MgCoordinate in lon/lat units
    ///
    virtual MgCoordinate* ConvertToLonLat(double dX, double dY)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Converts the specified coordinates in coordinate system units to lon/lat units
    ///
    /// \param dX (double)
    /// The X coordinate
    /// \param dY (double)
    /// The Y coordinate
    /// \param dZ (double)
    /// The Z coordinate
    ///
    /// \return
    /// The MgCoordinate in lon/lat units
    ///
    virtual MgCoordinate* ConvertToLonLat(double dX, double dY, double dZ)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Converts the specified coordinates in lon/lat units to coordinate system units
    ///
    /// \param dLongitude (double)
    /// The longitude coordinate
    /// \param dLatitude (double)
    /// The latitude coordinate
    ///
    /// \return
    /// The MgCoordinate in coordinate system units
    ///
    virtual MgCoordinate* ConvertFromLonLat(double dLongitude, double dLatitude)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Converts the specified coordinates in lon/lat units to coordinate system units
    ///
    /// \param dLongitude (double)
    /// The longitude coordinate
    /// \param dLatitude (double)
    /// The latitude coordinate
    /// \param dZ (double)
    /// The Z coordinate
    ///
    /// \return
    /// The MgCoordinate in coordinate system units
    ///
    virtual MgCoordinate* ConvertFromLonLat(double dLongitude, double dLatitude, double dZ)=0;

    virtual double GetConvergence(double dLongitude, double dLatitude)=0;
    virtual double GetScale(double dLongitude, double dLatitude)=0;
    virtual double GetScaleH(double dLongitude, double dLatitude)=0;
    virtual double GetScaleK(double dLongitude, double dLatitude)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets a cloned copy of this coordinate system
    ///
    /// \return
    /// The cloned copy of this coordinate system
    ///
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

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

#ifndef _CCOORDINATESYSTEMGRIDS_H_
#define _CCOORDINATESYSTEMGRIDS_H_

namespace CSLibrary
{

//=============================================================================
// Parameters necessary to specify the various aspects of a grid or graticule
// lumped into a single object for convenience.
class CCoordinateSystemGridSpecification : public MgCoordinateSystemGridSpecification
{
public:
    CCoordinateSystemGridSpecification (void);
    CCoordinateSystemGridSpecification (const CCoordinateSystemGridSpecification& source);
    ~CCoordinateSystemGridSpecification (void);
    CCoordinateSystemGridSpecification& operator= (const CCoordinateSystemGridSpecification& rhs);

    const static double m_DefaultPrecision;
    double GetEastingBase(void);
    double GetNorthingBase(void);
    double GetEastingIncrement(void);
    double GetNorthingIncrement(void);
    double GetTickEastingIncrement(void);
    double GetTickNorthingIncrement(void);
    INT32 GetUnitType(void);
    INT32 GetUnitCode(void);
    bool IsSameAs(MgCoordinateSystemGridSpecification* specification);
    double GetCurvePrecision(void);
    INT32 GetMaxCurvePoints(void);

    void SetGridBase(double eastingBase,double northingBase);
    void SetGridIncrement(double eastingIncrement,double northingIncrement);
    void SetTickIncrements(double eastingIncrement,double northingIncrement);
    void SetUnits (INT32 unitCode,INT32 unitType = MgCoordinateSystemUnitType::Linear);
    void SetCurvePrecision (double curvePrecision);
    void SetMaxCurvePoints (INT32 maxCurvePoints);

    // The following function identically to the published version, with the
    // exception that the value returned is always converted to the units
    // specified by the provided units code.
    double GetEastingBase (INT32 resultUnitCode);
    double GetNorthingBase (INT32 resultUnitCode);
    double GetEastingIncrement (INT32 resultUnitCode);
    double GetNorthingIncrement (INT32 resultUnitCode);
    double GetTickEastingIncrement (INT32 resultUnitCode);
    double GetTickNorthingIncrement (INT32 resultUnitCode);

    // These functions will generate a suitable value if the host application
    // left the curve precision unspecified.
    double GetCurvePrecision(MgCoordinateSystem* gridCS);
    double GetCurvePrecision(MgCoordinateSystemGridBoundary* gridBoundary,MgCoordinateSystem* gridCS);

    // This function will generate a suitable value if the host application
    // left the max curve points unspecified.
    double GetMaxCurvePoints (MgCoordinateSystemGridBoundary* frameBoundary,MgCoordinateSystem* frameCS);

    // The following is used by the CoordinateSystemFactory object to verify
    // that all of the information provided to it is consistent with the
    // requirements of the object.
    bool IsConsistent (void);

protected:
    void Dispose (void);
    double ConvertUnitsOfValue (double value,INT32 trgUnitCode);

private:
    double m_EastingBase;
    double m_NorthingBase;
    double m_EastingIncrement;
    double m_NorthingIncrement;
    double m_CurvePrecision;
    double m_TickEastingIncrement;
    double m_TickNorthingIncrement;
    INT32 m_UnitType;
    INT32 m_UnitCode;
    INT32 m_MaxCurvePoints;
};

//=============================================================================
// Carries the boundary of the grid.
//
// In the case where the coordinates are geographic, the values of the
// coordinates in the MgPolygon may exceed the expected limits of +/- 180
// and +/- 90 to support wrap around.
//
class CCoordinateSystemGridBoundary : public MgCoordinateSystemGridBoundary
{
public:
   ////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Default value representing the limit at which curve generation will
    /// always stop so as to reduce the possibility of an infinite loop.  The
    /// limit is applied after each generation cycle, so the actual number of
    /// points in a line may be as much as twice this value.  All GridBoundary
    /// objects will have there m_MaxCurvePoints member variable initialized
    /// to this value.
    static const INT32 MaxCurvePoints;

    ////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Default Constructor.  Use one of the two SetBoundary overlaods defined
    /// below in order to create a valid GridBoundary object after using this
    /// default constructor.
    CCoordinateSystemGridBoundary (void);

    ////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Primary Constructor.  Contrary to other envelope objects, this
    /// particular overload specifically requires that the southwest argument
    /// indeed be southwest of the northeast argument.  (The assumption here is
    /// that the X coordinate is the easting and the Y coordinate is the
    /// northing.)  This is necessary to support geographic coordinate systems
    /// (i.e. +/- 180).  To support future development plans, the boundary is
    /// maintained as an arbitrary MgPolygon object.  In the initial release,
    /// only rectangular boundaries will be tested and officially supported.
    /// \param southwest
    /// Southwest corner of the boundary in which a grid is to be drawn.  This
    /// point must be southwest of the northeast location.
    /// \param northeast
    /// Northeast corner of the boundary in which a grid is to be drawn.
    /// This point must be northeast of the southwest location.
    /// \remarks
    /// Externally, parameters are typically in frame coordinates; internally,
    /// may be in internal coordinates (such as UTM when used internal to the
    /// MGRS object).
    CCoordinateSystemGridBoundary (MgCoordinate* southwest,MgCoordinate* northeast);

    ////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Secondary Constructor.  This particular overload simply adds a reference
    /// to provided polygon as the boundary.
    CCoordinateSystemGridBoundary (MgPolygon* boundary);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Destructor, no surprises here.
    ~CCoordinateSystemGridBoundary (void);

    ////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Resets the grid boundary to that specified by the two arguments.
    /// \param southwest
    /// Southwest corner of the boundary in which a grid is to be drawn.  This
    /// point must be southwest of the northeast location.
    /// \param northeast
    /// Northeast corner of the boundary in which a grid is to be drawn.
    /// This point must be northeast of the southwest location.
    void SetBoundaryExtents (MgCoordinate* southwest,MgCoordinate* northeast);

    ////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Resets the grid boundary to that specified by the polygon argument.
    /// \param boundary
    /// Polygon which defines the new grid boundary.
    void SetBoundaryExtents (MgPolygon* boundary);

    ////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the current grid boundary in the form of an MgPolygon object.
    /// \remarks
    /// While initially, grid boundaries are simple rectangels, during grid
    /// generation a boundary may be converted from one coordinate system
    /// to another.  In so doing, the straight lines of the initial rectangle
    /// a often replaced with a complex curve representing the boundary in
    /// the new coordinate system.
    MgPolygon* GetBoundary (void) const;

    ////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets a limit on the number of points generated by the complex curve
    /// approximation algorithm.
    /// \param maxPoints
    /// The complex curve approximation algorithm cleanly terminates after this
    /// number of points has been generated.
    /// /remarks
    /// The actual number of points in a complex curve approximation may contain
    /// as many as twice this number of points.  It is when this number of
    /// points is exceeded that the algorithms starts a clean termination.<p>
    /// This value is used in the "GetBoundary" function to reduce the
    /// probability of a runaway calculation.
    void SetMaxCurvePoints (INT32 maxPoints);

    ////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the extents of the grid boundary.
    /// \param eastMin
    /// The minimum easting (i.e. X) ordinate value is returned here.
    /// \param eastMax
    /// The maximum easting (i.e. X) ordinate value is returned here.
    /// \param northMin
    /// The minimum northing (i.e. Y) ordinate value is returned here.
    /// \param northMax
    /// The maximum northing (i.e. Y) ordinate value is returned here.
    /// \remarks
    /// Especially useful after the internal boundary is converted to a
    /// different coordinate system.
    void GetBoundaryExtents (double& eastMin,double& eastMax,double& northMin,double& northMax) const;

    ////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the internal grid boundary, in the form of an MgPolygon object,
    /// after the boundary has been converted using the provided coordinate
    /// system transform.
    /// \param transform
    /// The transformation used to convert the grid boundary.
    /// \param precision
    /// A value which indicates, in target coordinate system units, how
    /// precisely any complex curve approximation generated must match the
    /// the true complex curve.
    /// \remarks
    /// The transformation must be a coordinate system transformation.  The
    /// existing grid boundary is assumed to be defined in terms of the
    /// transformation's source coordinate system, and is converted to the
    /// transformation's target coordinate system.  In the conversion process,
    /// a straight line segment is usually replaced with a multi-segment
    /// approximation.  The maximum distance (in target system units) will
    /// not exceed the value specified by the precision parameter.  Regardless
    /// of the precision specification, the curve generation process shuts down
    /// when the number of segments exceeds the value specified by the
    /// m_MaxCurvePoints member variable.
    MgPolygon* GetBoundary (MgCoordinateSystemTransform* transform,double precision = 1.0);

    ////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the result of clipping the provided line string to the internal
    /// region (exterior ring) of the grid boundary.
    /// \param lineString
    /// A line string in the same coordinate system as the grid boundary.
    /// \remarks
    /// The result is a <b>collection</b> of line strings, thus supporting the
    /// proper clipping of a line string which enters and leaves the grid
    /// boundary more than once.  The algorithm is <b>slow</b> as it is written
    /// to handle arbitrary polygons.  The algoirthm is designed to support
    /// concave polygons, but this has not been tested.  Interior rings in the
    /// internal polygon object are ignored.
    MgLineStringCollection* ClipLineString (MgLineString* lineString) const;

protected:
    ////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Satifies the pure virtual function of the MgDispose base class from
    /// which this object is derived.  Essentially causes the destructor to be
    /// called.
    void Dispose (void);
private:
    // The transform used in the following function is more sophisicated than the
    // standard Transform function.
    MgLinearRing* TransformLinearRing (MgLinearRing* linearString,
                                       MgCoordinateSystemTransform* transformation,
                                       double curvePrecision);
    bool m_Large;
    INT32 m_MaxCurvePoints;
    Ptr<MgPolygon> m_GridBoundary;
};

// Implementation class which adds an orientation and a value (real) to
// the base MgLineStringCollection.  Typically, the line string carries
// a single line; the multiple line feature is necessary to support
// clipping to the grid boundary.
class CCoordinateSystemGridLine : public MgCoordinateSystemGridLine
{
public:
    CCoordinateSystemGridLine (INT32 orientation,double value);
    ~CCoordinateSystemGridLine (void);

    INT32 GetGridOrientation(void);
    double GetRealValue(void);
    INT32 GetCount (void);
    MgLineString* GetSegment (INT32 index);
    MgLineStringCollection* GetSegmentCollection (void);

    void SetGridOrientation (INT32 orientation);
    void SetRealValue (double realValue);
    void ClearSegments (void);
    void AddSegment (MgLineString* newSegment);
    void SetSegmentCollection (MgLineStringCollection* segmentCollection);
protected:
    void Dispose (void);

    INT32 m_Orientation;
    double m_Value;
    Ptr<MgLineStringCollection> m_LineSegments;

private:            // Not implemented
    CCoordinateSystemGridLine (const CCoordinateSystemGridLine& source);
};

//=============================================================================
// CCoordinateSystemGridregion  --  Implements MgCoordinateSystemGridRegion
//
// This API has been found to be problematical in the initial release.  It
// will be deprecated in the next release.
//
class CCoordinateSystemGridRegion : public MgCoordinateSystemGridRegion
{
public:
    CCoordinateSystemGridRegion (STRING label);
    CCoordinateSystemGridRegion (STRING label,MgCoordinateSystemGridBoundary* frameBoundary,
                                              MgCoordinateSystemTransform* gridToFrame,
                                              MgCoordinate* southwest,
                                              MgCoordinate* northeast,
                                              double curvePrecision,
                                              INT32 maxPoints);

    STRING GetLabel ();
    MgCoordinate* GetRegionCenter (void);
    MgPolygon* GetRegionBoundary (void);

    // Returns from the following are clipped, thus they are collections of
    // line strings due to the rare but possible case the indicated line
    // enters and leaves the frame boundary more than once.
    MgLineStringCollection* GetSouthLine (void);
    MgLineStringCollection* GetEastLine (void);
    MgLineStringCollection* GetNorthLine (void);
    MgLineStringCollection* GetWestLine (void);

    void SetRegionCenter (MgCoordinate* regionCenter);
    void SetRegionBoundary (MgPolygon* boundary);
    void SetSouthLine (MgLineStringCollection* southLine);
    void SetEastLine (MgLineStringCollection* eastLine);
    void SetNorthLine (MgLineStringCollection* northLine);
    void SetWestLine (MgLineStringCollection* westLine);

protected:
    void Dispose (void);
    STRING m_RegionLabel;
    Ptr<MgCoordinate> m_RegionCenter;
    Ptr<MgPolygon> m_RegionBoundary;
    Ptr<MgLineStringCollection> m_SouthLine;
    Ptr<MgLineStringCollection> m_EastLine;
    Ptr<MgLineStringCollection> m_NorthLine;
    Ptr<MgLineStringCollection> m_WestLine;

private:
    // Not implemented
    CCoordinateSystemGridRegion (void);
    CCoordinateSystemGridRegion (const CCoordinateSystemGridRegion& source);
};

//=============================================================================
// CCoordinateSystemGridTick  --  Implements MgCoordinateSystemGridTick
//
//
class CCoordinateSystemGridTick : public MgCoordinateSystemGridTick
{
public:
    CCoordinateSystemGridTick (INT32 orientation,double value);
    ~CCoordinateSystemGridTick (void);

    void SetOnGridLine (bool isOnGridLine);
    void SetOrientation (INT32 orientation);
    void SetValue (double value);
    void SetPosition (MgCoordinate* position);
    void SetDirection (MgCoordinate* direction);

    bool GetIsOnGridLine (void);
    INT32 GetTickOrientation (void);
    double GetValue (void);
    MgCoordinate* GetPosition (void);
    MgCoordinate* GetDirectionVector (void);

protected:
    void Dispose (void);

    bool m_OnGridLine;
    INT32 m_Orientation;
    double m_Value;
    Ptr<MgCoordinate> m_Position;
    Ptr<MgCoordinate> m_Direction;

private:            // Not implemented
    CCoordinateSystemGridTick (const CCoordinateSystemGridTick& source);
    CCoordinateSystemGridTick& operator= (const CCoordinateSystemGridTick& rhs);
};

//=============================================================================
// An CCoordinateSystemGridLineCollection is collection of
// MgCoordinateSystemGridLine objects.  As manufactured, grid horizontal lines
// appear first in ascending order by grid value (northing in this case);
// followed by grid vertical lines in ascending order by grid value (easting
// in this case).
class CCoordinateSystemGridLineCollection : public MgCoordinateSystemGridLineCollection
{
public:
    CCoordinateSystemGridLineCollection (INT64 memoryThreshold);
    ~CCoordinateSystemGridLineCollection(void);

    INT32 GetCount () const;
    MgCoordinateSystemGridLine* GetItem (INT32 index) const;
    INT32 IndexOf (INT32 gridOrientation,double gridValue) const;
    void RemoveAt (INT32 index);
    void Clear();

    void SetItem (INT32 index,MgCoordinateSystemGridLine* value);
    void Add (MgCoordinateSystemGridLine* value);
    void AddCollection (MgCoordinateSystemGridLineCollection* aGridLineCollection);

protected:
    void Dispose(void);
    INT64 m_MemoryThreshold;
    Ptr<MgDisposableCollection> m_GridLineCollection;

private:
    // Not Implemented
    CCoordinateSystemGridLineCollection (const CCoordinateSystemGridLineCollection& source);
    CCoordinateSystemGridLineCollection& operator= (const CCoordinateSystemGridLineCollection& rhs);
};

//=============================================================================
// A CCoordinateSystemGridRegionCollection is collection of
// CCoordinateSystemGridRegion objects.
class CCoordinateSystemGridRegionCollection : public MgCoordinateSystemGridRegionCollection
{
public:
    CCoordinateSystemGridRegionCollection (INT64 memoryThreshold);
    ~CCoordinateSystemGridRegionCollection (void);

    INT32 GetCount () const;
    MgCoordinateSystemGridRegion* GetItem (INT32 index) const;
    void RemoveAt (INT32 index);
    void Clear();

    void SetItem (INT32 index, MgCoordinateSystemGridRegion* value);
    void Add (MgCoordinateSystemGridRegion* value);
    void AddCollection (MgCoordinateSystemGridRegionCollection* aGridRegionCollection);

protected:
    void Dispose (void);
    INT64 m_MemoryThreshold;
    Ptr<MgDisposableCollection> m_GridRegionCollection;

private:
    // Not Implemented
    CCoordinateSystemGridRegionCollection (const CCoordinateSystemGridRegionCollection& source);
    CCoordinateSystemGridRegionCollection& operator= (const CCoordinateSystemGridRegionCollection& rhs);
};

//=============================================================================
// A CCoordinateSystemGridTickCollection is collection of
// CCoordinateSystemGridTick objects.  CCoordinateSystemGridTickCollection
// objects will contain MgCoordinateSystemGridTick objects for the entire
// boundary or for an individual grid line.
class CCoordinateSystemGridTickCollection : public MgCoordinateSystemGridTickCollection
{
public:
    CCoordinateSystemGridTickCollection (INT64 memoryThreshold);
    ~CCoordinateSystemGridTickCollection (void);

    INT32 GetCount () const;
    MgCoordinateSystemGridTick* GetItem (INT32 index) const;
    void RemoveAt (INT32 index);
    void Clear();
    void SetItem (INT32 index, MgCoordinateSystemGridTick* value);
    void Add (MgCoordinateSystemGridTick* value);
    void AddCollection (MgCoordinateSystemGridTickCollection* aGridTickCollection);

protected:
    void Dispose (void);
    INT64 m_MemoryThreshold;
    Ptr<MgDisposableCollection> m_GridTickCollection;

private:
    // Not Implemented
    CCoordinateSystemGridTickCollection (const CCoordinateSystemGridTickCollection& source);
    CCoordinateSystemGridTickCollection& operator= (const CCoordinateSystemGridTickCollection& rhs);
};

}   /* namespace CSLibrary */

#endif /* _CCOORDINATESYSTEMGRIDS_H_ */

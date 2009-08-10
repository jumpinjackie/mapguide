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
PUBLISHED_API:
    double GetEastingBase(void);
    double GetNorthingBase(void);
    double GetEastingIncrement(void);
    double GetNorthingIncrement(void);
    INT32 TickEastingSubdivisions(void);
    INT32 TickNorthingSubdivisions(void);
    INT32 GetUnitType(void);
    INT32 GetUnitCode(void);
    bool IsSameAs(MgCoordinateSystemGridSpecification* specification);
    double GetCurvePrecision(void);

    void SetGridBase(double eastingBase,double northingBase = 0.0);
    void SetGridIncrement(double eastingIncrement,double northingIncrement = 0.0);
    void SetTickSubdivisions(INT32 eastingSubdivisions,INT32 northingSubdivisions = 0);
    void SetUnits (INT32 unitCode,INT32 unitType = MgCoordinateSystemUnitType::Linear);
    void SetCurvePrecision (double curvePrecision);

INTERNAL_API:
    // The following function identically to the published version, with the
    // exception that the value returned are always converted to the units of
    // the provided coordinate system.
    double GetEastingBase (MgCoordinateSystem* gridCS);
    double GetNorthingBase (MgCoordinateSystem* gridCS);
    double GetEastingIncrement(MgCoordinateSystem* gridCS);
    double GetNorthingIncrement(MgCoordinateSystem* gridCS);
    double GetCurvePrecision(MgCoordinateSystem* gridCS);

protected:
    void Dispose (void);

private:

    double m_EastingBase;
    double m_NorthingBase;
    double m_EastingIncrement;
    double m_NorthingIncrement;
    double m_CurvePrecision;
    INT32 m_EastingTickSubdivisions;
    INT32 m_NorthingTickSubdivisions;
    INT32 m_UnitType;
    INT32 m_UnitCode;
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
    // Contrary to other envelope objects, the first of these overloads specifically
    // requires that the southwest argument indeed be southwest of the northeast
    // argument.  Necessary to support geographic coordinate systems (i.e. +/- 180).
    CCoordinateSystemGridBoundary (MgCoordinate* southwest,MgCoordinate* northeast);
    ~CCoordinateSystemGridBoundary (void);

    void SetBoundaryExtents (MgCoordinate* southwest,MgCoordinate* northeast);
    void SetBoundaryExtents (MgPolygon* boundary);
    MgPolygon* GetBoundary (void) const;
    CCoordinateSystemGridBoundary (void);

    void GetBoundaryExtents (double& eastMin,double& eastMax,double& northMin,double& northMax) const;
    MgPolygon* GetBoundary (MgCoordinateSystemTransform* transform,double precision = 1.0);
    MgLineStringCollection* ClipLineString (MgLineString* lineString) const;
protected:
    void Dispose (void);
private:
    // The transform used in the folloing function is more sophisicated than the
    // standard Transform function.
    MgLinearRing* TransformLinearRing (MgLinearRing* linearString,
                                       MgCoordinateSystemTransform* transformation,
                                       double curvePrecision); 
    bool m_Large;
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
    CCoordinateSystemGridLine (const CCoordinateSystemGridLine& source);
    ~CCoordinateSystemGridLine (void);
    
    virtual INT32 GetGridOrientation(void);
    virtual double GetRealValue(void);
    virtual INT32 GetCount (void);
    virtual MgLineString* GetSegment (INT32 index);
    virtual MgLineStringCollection* GetSegmentCollection (void);

    virtual void SetGridOrientation (INT32 orientation);
    virtual void SetRealValue (double realValue);
    virtual void ClearSegments (void);
    virtual void AddSegment (MgLineString* newSegment);
    virtual void SetSegmentCollection (MgLineStringCollection* segmentCollection);
protected:
    void Dispose (void);

    INT32 m_Orientation;
    double m_Value;
    Ptr<MgLineStringCollection> m_LineSegments;
};

//=============================================================================
// CCoordinateSystemGridregion  --  Implements MgCoordinateSystemGridregion
//
class CCoordinateSystemGridRegion : public MgCoordinateSystemGridRegion
{
    CCoordinateSystemGridRegion (STRING label,MgPolygon* polygon);
    
    virtual STRING GetLabel ();
    virtual MgPolygon* GetPolygon ();

protected:
    void Dispose (void);
    STRING m_RegionLabel;
    Ptr<MgPolygon> m_Polygon;

private:
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
    CCoordinateSystemGridTick (const CCoordinateSystemGridTick& source);
    ~CCoordinateSystemGridTick (void);
    CCoordinateSystemGridTick& operator= (const CCoordinateSystemGridTick& rhs);

    virtual void SetOrientation (INT32 orientation);
    virtual void SetValue (double value);
    virtual void SetPosition (MgCoordinate* position);
    virtual void SetDirection (MgCoordinate* direction);

    virtual INT32 GetTickOrientation (void);
    virtual double GetValue (void);
    virtual MgCoordinate* GetPosition (void);
    virtual MgCoordinate* GetDirectionVector (void);

protected:
    void Dispose (void);

    INT32 m_Orientation;
    double m_Value;
    Ptr<MgCoordinate> m_Position;
    Ptr<MgCoordinate> m_Direction;
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
    CCoordinateSystemGridLineCollection (void);
    ~CCoordinateSystemGridLineCollection(void);

    INT32 GetCount () const;
    MgCoordinateSystemGridLine* GetItem (INT32 index) const;
    INT32 IndexOf (INT32 gridOrientation,double gridValue) const;
    void RemoveAt (INT32 index);
    void Clear();

    void SetItem (INT32 index,MgCoordinateSystemGridLine* value);
    void Add (MgCoordinateSystemGridLine* value);
protected:
    void Dispose(void);
    Ptr<MgDisposableCollection> m_GridLineCollection;
private:
    CCoordinateSystemGridLineCollection (const CCoordinateSystemGridLineCollection& source);
    CCoordinateSystemGridLineCollection& operator= (const CCoordinateSystemGridLineCollection& rhs);
};

//=============================================================================
// A CCoordinateSystemGridRegionCollection is collection of
// CCoordinateSystemGridRegion objects.
class CCoordinateSystemGridRegionCollection : public MgCoordinateSystemGridRegionCollection
{
    CCoordinateSystemGridRegionCollection (void);
    ~CCoordinateSystemGridRegionCollection (void);

    INT32 GetCount () const;
    MgCoordinateSystemGridRegion* GetItem (INT32 index) const;
    void RemoveAt (INT32 index);
    void Clear();

    void SetItem (INT32 index, MgCoordinateSystemGridRegion* value);
    void Add (MgCoordinateSystemGridRegion* value);

protected:
    void Dispose (void);
    Ptr<MgDisposableCollection> m_GridRegionCollection;
private:
    CCoordinateSystemGridRegionCollection (const CCoordinateSystemGridRegionCollection& source);
    CCoordinateSystemGridRegionCollection& operator= (const CCoordinateSystemGridRegionCollection& rhs);
};

//=============================================================================
// A CCoordinateSystemGridTickCollection is collection of
// CCoordinateSystemGridTick objects.  CCoordinateSystemGridTickCollection
// objects will contain MgCoordinateSystemGridTick objects for the entire
// boundary or for an individual grid line.
class MG_GEOMETRY_API CCoordinateSystemGridTickCollection : public MgCoordinateSystemGridTickCollection 
{
    CCoordinateSystemGridTickCollection (void);
    ~CCoordinateSystemGridTickCollection (void);

    INT32 GetCount () const;
    MgCoordinateSystemGridTick* GetItem (INT32 index) const;
    void RemoveAt (INT32 index);
    void Clear()=0;
    void SetItem (INT32 index, MgCoordinateSystemGridTick* value);
    void Add (MgCoordinateSystemGridTick* value);
protected:
    void Dispose (void);
    Ptr<MgDisposableCollection> m_GridTickCollection;

private:
    CCoordinateSystemGridTickCollection (const CCoordinateSystemGridTickCollection& source);
    CCoordinateSystemGridTickCollection& operator= (const CCoordinateSystemGridTickCollection& rhs);  
};

}   /* namespace CSLibrary */

#endif /* _CCOORDINATESYSTEMGRIDS_H_ */

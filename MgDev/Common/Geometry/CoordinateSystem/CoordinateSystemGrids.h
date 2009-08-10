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

//=============================================================================
// Class declararations
class MgCoordinateSystemGridBoundary;           // MgPolygon based extents of the grid;
                                                // currently only rectangles are
                                                // supported.
class MgCoordinateSystemGridBase;               // abstract interface between grid generator
                                                // and grid consumer
class MgCoordinateSystemGridGeneric;            // derives from MgCoordinateSystemGridBase
                                                // and generates generic (i.e. non-specialized)
                                                // grids (SPCS,UTM, etc.)
class MgCoordinateSystemGridMgrs;               // derives from MgCoordinateSystemGridBase and contains
                                                // (most) all MGRS related intelligence
class MgCoordinateSystemGridUsng;               // derives from MgCoordinateSystemGridBase and contains
                                                // (most) all USNG related intelligence
class MgCoordinateSystemGridSpecification;      // conveys all grid generation parameters
                                                // between objects, generator and consumer
                                                // alike
class MgCoordinateSystemGridLineCollection;     // a collection of MgCoordinateSystemGridLine objects,
                                                // whatever they turn out to be
class MgCoordinateSystemGridRegionCollection;   // a collection of MgCoordinateSystemGridRegion objects,
                                                // whatever they turn out to be
class MgCoordinateSystemGridTickCollection;     // a collection of MgCoordinateSystemGridTick objects,
                                                // whatever they turn out to be
class MgCoordinateSystemGridLine;               // a grid value and a collection of
                                                // MgCoordinateSystemGridLineSegment objects
                                                // (commonly just one)
class MgCoordinateSystemGridLineSegment;        // a segment of a grid line resulting from clipping
                                                // a complete grid line to grid boundary
class MgCoordinateSystemGridRegion;             // a label and a MgPolygon which defines the boundary
                                                // of the region
class MgCoordinateSystemGridTick;               // a position in viewport coordinates and a grid value
                                                // as a double


//=============================================================================
// An enumeration of the various types of specialized grids/graticules
// currently supported.  A specialized grid is one for which specific standards
// exist to which the results of this feature are compliant with.
class MgCoordinateSystemGridSpecializationType
{
PUBLISHED_API:
    static const INT32 None              = 0;       // not specified as yet; initialize to this value
    static const INT32 Generic           = 0x01;    // Generic grid of a specified coordinate system;
                                                    // may be geographic or projected
    static const INT32 MGRS              = 0x11;    // Military Grid Reference System
    static const INT32 USNG              = 0x12;    // United States National Grid
    static const INT32 Unknown           = 0x8000;  // indicates a failure of an algorithm
};

//=============================================================================
// An enumeration of the supported values for the 'm_Orientation' member of
// several objects related to grids and graticules.
//
// This value is used to qualify objects which are of the "iso" type.  I.e. a
// grid line is referred to as an isoline as it is the locus of points which
// have a specific value for either the easting or the northing.  It is this
// value which indicates which.  Thus, a grid line which is classified as
// having an "EastWest" orientation will be a isoline which is the locus of
// points which sahre a common easting value, and the "m_Value" element of
// that object will be an easting value.
//
class MgCoordinateSystemGridOrientation
{
PUBLISHED_API:
    static const INT8 None = 0;             // not specified as yet; initialize to this value
    static const INT8 EastWest = 1;         // generally indicates a vertical grid line
    static const INT8 NorthSouth = 2;       // generally indicates a horizontal grid line
    static const INT8 Unknown = 3;          // indicates a failure of an algorithm
};

//=============================================================================
// MgCoordinateSystemGridSpecification
//
// This object is used to convey all the parameters necessary for the
// generation of a grid/graticule lumped into a single object for convenience.
// Thus, adding a parameter determined to be necessary does not alter a lot of
// calling sequences.
class MG_GEOMETRY_API MgCoordinateSystemGridSpecification : public MgGuardDisposable
{
PUBLISHED_API:
    virtual double GetEastingBase (void)=0;             // the base value from which easting grid line values are computed.
    virtual double GetNorthingBase (void)=0;            // the base value from which northing grid line values are computed.
    virtual double GetEastingIncrement(void)=0;         // the interval between easting grid lines
    virtual double GetNorthingIncrement(void)=0;        // the interval between northing grid lines
    virtual INT32 TickEastingSubdivisions(void)=0;      // number of tic subdivisions between grid increments in the east/west direction
    virtual INT32 TickNorthingSubdivisions(void)=0;     // number of tic subdivisions between grid increments in the north/south direction
    virtual INT32 GetUnitType(void)=0;                  // a value from MgCoordinateSystemUnitType indicating the type of units of the
                                                        // base and interval specifications
    virtual INT32 GetUnitCode(void)=0;                  // a value from MgCoordinateSystemUnitCode indicating the units of the
                                                        // base and interval specifications
    virtual bool IsSameAs(MgCoordinateSystemGridSpecification* specification)=0;
    virtual double GetCurvePrecision(void)=0;           // the maximum difference between true complex curve and the polyline
                                                        // approximation thereof, in the same units used to specify the
                                                        // base and increment values (i.e. UnitCode).
                                                        // If left unspecified (or set to zero) a value is automatically selected to be,
                                                        // approximately, the equivalent of 1 meter in target system units.

    virtual void SetGridBase(double eastingBase,double northingBase = 0.0) = 0;
    virtual void SetGridIncrement(double eastingIncrement,double northingIncrement = 0.0) = 0;
    virtual void SetTickSubdivisions(INT32 eastingSubdivisions,INT32 northingSubdivisions = 0) = 0;
    virtual void SetUnits (INT32 unitCode,INT32 unitType = MgCoordinateSystemUnitType::Linear) = 0;
    virtual void SetCurvePrecision (double curvePrecision) = 0;

INTERNAL_API:
    // The following function identically to the published version, with the
    // exception that the value returned is converted to the units of the
    // provided coordinate system.  Typically, the provided coordinate system
    // is either the grid coordinate system OR the frame coordinate system.
    virtual double GetEastingBase (MgCoordinateSystem* gridCS)=0;
    virtual double GetNorthingBase (MgCoordinateSystem* gridCS)=0;
    virtual double GetEastingIncrement (MgCoordinateSystem* gridCS)=0;
    virtual double GetNorthingIncrement (MgCoordinateSystem* gridCS)=0;
    virtual double GetCurvePrecision (MgCoordinateSystem* gridCS)=0;
protected:
    INT32 GetClassId(){return m_cls_id;};
CLASS_ID:
    static const INT32 m_cls_id = CoordinateSystem_CoordinateSystemGridSpecification;
};

//=============================================================================
// External to this interface, boundary objects are always in the viewport
// (i.e. target) coordinate system.
class MG_GEOMETRY_API MgCoordinateSystemGridBoundary : public MgGuardDisposable
{
PUBLISHED_API:
    // Contrary to other envelope objects, the first of these overloads specifically
    // requires that the southwest argument indeed be southwest of the northeast
    // argument.  Necessary to support geographic coordinate systems (i.e. +/- 180).
    virtual void SetBoundaryExtents (MgCoordinate* southwest,MgCoordinate* northeast)=0;
    virtual void SetBoundaryExtents (MgPolygon* boundary)=0;
    virtual MgPolygon* GetBoundary (void) const=0;
INTERNAL_API:
    virtual void GetBoundaryExtents (double& eastMin,double& eastMax,double& northMin,double& northMax) const = 0;
    virtual MgPolygon* GetBoundary (MgCoordinateSystemTransform* transformation,double precision = 1.0) = 0;
    virtual MgLineStringCollection* ClipLineString (MgLineString* lineString) const=0;
protected:
    INT32 GetClassId(){return m_cls_id;};
CLASS_ID:
    static const INT32 m_cls_id = CoordinateSystem_CoordinateSystemGridBoundary;
};

//=============================================================================
// Abstract base class for all grid generation objects (generic and
// specialized).
class MG_GEOMETRY_API MgCoordinateSystemGridBase : public MgGuardDisposable
{
PUBLISHED_API:
    // The following may be somewhat redendant in view of the fact that all grid
    // specialization objects will have their own ClassID value.  Nevertheless,
    // the specialization type enumeration and this function are provided to
    // support exposure of this interface to other environments in a standard
    // pre-determined manner.  In this manner, therefore. this module will
    // continue to control the ID associated with any given specialization.
    virtual INT32 GetSpecializationType ()=0;

    virtual void SetBoundary (MgCoordinateSystemGridBoundary* pGridBoundary) = 0;
    virtual MgCoordinateSystemGridBoundary* GetBoundary(void)=0;
    virtual MgCoordinateSystemGridLineCollection* GetGridLines (MgCoordinateSystemGridSpecification* specification)=0;
    virtual MgCoordinateSystemGridRegionCollection* GetGridRegions (MgCoordinateSystemGridSpecification* specification)=0;
    // The following generates grid ticks for the grid boundary.
    virtual MgCoordinateSystemGridTickCollection* GetGridTicks (MgCoordinateSystemGridSpecification* specification)=0;

    // The following can be of value for rendering and stylization objects
    // which need to render north arrows, scale bars, etc.  Location
    // parameter is always in viewport/frame coordinates.
    virtual double GetConvergenceAngle (MgCoordinate* location) = 0;
    virtual double GetProjectiveGridScale (MgCoordinate* location) = 0;

    virtual INT32 GetLastError() = 0;
    virtual void ResetLastError()= 0;
    virtual bool AreExceptionsOn() = 0;
    virtual void SetExceptionsOn(bool bOn) = 0;
INTERNAL_API:
protected:
};

//=============================================================================
// An MgCoordinateSystemGridLine object is a grid value (as a double) and a
// collection of line strings, where each individual line string being
// considered a grid line segment.  All line strings in the collection are in
// the viewport coordinate system.  The value returned by GetGridOrientation
// indicates the nature of the grid line in grid coordinates (i.e. easting or
// northing).
class MG_GEOMETRY_API MgCoordinateSystemGridLine : public MgGuardDisposable
{
PUBLISHED_API:
    virtual INT32 GetGridOrientation(void)= 0;
    virtual double GetRealValue(void)=0;
    virtual INT32 GetCount (void) = 0;
    virtual MgLineString* GetSegment (INT32 index)=0;
    virtual MgLineStringCollection* GetSegmentCollection(void)= 0;
    virtual void SetSegmentCollection (MgLineStringCollection* segmentCollection) = 0;
INTERNAL_API:
protected:
    INT32 GetClassId(){return m_cls_id;};
CLASS_ID:
    static const INT32 m_cls_id = CoordinateSystem_CoordinateSystemGridLine;
};

//=============================================================================
// An MgCoordinateSystemGridRegion object consists of a label and a polygon.
// The polygon is always in the viewport coordinate system.  Such an object
// is used, for example, to label and delineate an MGRS 100Km UTM grid zone.
class MG_GEOMETRY_API MgCoordinateSystemGridRegion : public MgGuardDisposable
{
PUBLISHED_API:
    virtual STRING GetLabel () = 0;
    virtual MgPolygon* GetPolygon() = 0;
INTERNAL_API:
protected:
    INT32 GetClassId(){return m_cls_id;};
CLASS_ID:
    static const INT32 m_cls_id = CoordinateSystem_CoordinateSystemGridRegion;
};

//=============================================================================
// An MgCoordinateSystemGridTick object consists of a grid value, a tick
// location point in viewport coordinates, and a 2D unit direction vector.
// Position is the position of the tick mark on a grid or boundary line.  The
// 2D unit direction vector is the direction of the line at the position given.
// In the case of a tick mark on a boundary line, since boundary lines are
// always maintained in the counterclockwise direction, the interior of the
// viewport will always be to the left of the direction vector.
class MG_GEOMETRY_API MgCoordinateSystemGridTick : public MgGuardDisposable
{
PUBLISHED_API:
    virtual INT32 GetTickOrientation () const= 0;
    virtual double GetValue () const = 0;
    virtual MgCoordinate* GetPosition () const = 0;
    virtual MgCoordinate* GetDirectionVector () const = 0;
INTERNAL_API:
protected:
    INT32 GetClassId(){return m_cls_id;};
CLASS_ID:
    static const INT32 m_cls_id = CoordinateSystem_CoordinateSystemGridTick;
};

//=============================================================================
// An MgCoordinateSystemGridLineCollection is collection of
// MgCoordinateSystemGridLine objects.  As manufactured, grid horizontal lines
// appear first in ascending order by grid value (easting in this case);
// followed by grid vertical lines in ascending order by grid value (northing
// in this case).
class MG_GEOMETRY_API MgCoordinateSystemGridLineCollection : public MgGuardDisposable
{
PUBLISHED_API:
    virtual INT32 GetCount () const=0;
    virtual MgCoordinateSystemGridLine* GetItem (INT32 index) const=0;
    virtual INT32 IndexOf (INT32 gridOrientation,double gridValue) const=0;
    virtual void RemoveAt (INT32 index)=0;
    virtual void Clear()=0;
INTERNAL_API:
    virtual void SetItem (INT32 index, MgCoordinateSystemGridLine* value)=0;
    virtual void Add (MgCoordinateSystemGridLine* value)=0;

protected:
    INT32 GetClassId(){return m_cls_id;};
CLASS_ID:
    static const INT32 m_cls_id = CoordinateSystem_CoordinateSystemGridLineCollection;
};

//=============================================================================
// An MgCoordinateSystemGridRegionCollection is collection of
// MgCoordinateSystemGridRegion objects.
class MG_GEOMETRY_API MgCoordinateSystemGridRegionCollection : public MgGuardDisposable
{
PUBLISHED_API:
    virtual INT32 GetCount () const=0;
    virtual MgCoordinateSystemGridRegion* GetItem (INT32 index) const=0;
    virtual void RemoveAt (INT32 index)=0;
    virtual void Clear()=0;
INTERNAL_API:
    virtual void SetItem (INT32 index, MgCoordinateSystemGridRegion* value)=0;
    virtual void Add (MgCoordinateSystemGridRegion* value)=0;
protected:
    INT32 GetClassId(){return m_cls_id;};
CLASS_ID:
    static const INT32 m_cls_id = CoordinateSystem_CoordinateSystemGridRegionCollection;
};

//=============================================================================
// An MgCoordinateSystemGridTickCollection is collection of
// MgCoordinateSystemGridTick objects.  MgCoordinateSystemGridTickCollection
// objects will contain MgCoordinateSystemGridTick objects for the entire
// boundary, or for an individual grid line.
class MG_GEOMETRY_API MgCoordinateSystemGridTickCollection : public MgGuardDisposable
{
PUBLISHED_API:
    virtual INT32 GetCount () const=0;
    virtual MgCoordinateSystemGridTick* GetItem (INT32 index) const=0;
    virtual void RemoveAt (INT32 index)=0;
    virtual void Clear()=0;
INTERNAL_API:
    virtual void SetItem (INT32 index, MgCoordinateSystemGridTick* value)=0;
    virtual void Add (MgCoordinateSystemGridTick* value)=0;
protected:
    INT32 GetClassId(){return m_cls_id;};
CLASS_ID:
    static const INT32 m_cls_id = CoordinateSystem_CoordinateSystemGridTickCollection;
};

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

#ifndef _MGCOORDINATESYSTEMGRIDS_H_
#define _MGCOORDINATESYSTEMGRIDS_H_

//=============================================================================
// Class declararations
class MgCoordinateSystemGridBoundary;           // MgPolygon based extents of the grid
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

/// \defgroup MgCoordinateSystemGridSpecification MgCoordinateSystemGridSpecification
/// \ingroup Coordinate_System_classes
/// \{

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// MgCoordinateSystemGridSpecification is an object that is used to
/// convey all the parameters necessary for the generation of a grid/graticule
/// lumped into a single object for convenience.  Thus, adding a parameter
/// determined to be necessary at a later time does not alter a lot of calling
/// sequences.
///
/// Note that this interface is an abstract interface.  There is code
/// associated with the implementation of this interface which is used to
/// handle the conversion of parameters between the various unit systems.
/// Thus, one obtains a MgCoordinateSystemGridSpecification from the
/// MgCoordinateSystemFactory object.
///
/// Note that all values are provided in the units specified within the object.
/// The units used in this object do <b>not</b> need to be the same as any
/// coordinate system involved in the generation of a grid, although the
/// <b>type</b> of unit must be consistent with the type of grid being drawn.
/// That is, specifying a Angular unit type when drawing a UTM grid will cause
/// an exception at the time the request for (say) grid lines is issued.
///
/// \since 2.2
class MG_GEOMETRY_API MgCoordinateSystemGridSpecification : public MgGuardDisposable
{
PUBLISHED_API:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the easting base value.
    /// 
    /// \return
    /// Returns the current value of the easting base member.
    /// 
    /// \remarks
    /// The base value is the base upon which grid values are determined.
    /// Thus, a grid with and increment of, say, two degrees, can actually
    /// start at one (the base) yielding grid lines at 1, 3, 5, 7, etc.
    /// 
    virtual double GetEastingBase (void)=0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the northing base value.
    /// 
    /// \return
    /// Returns the current value of the northing base member.
    /// 
    /// \remarks
    /// The base value is the base upon which grid values are determined.
    /// Thus, a grid with and increment of, say, two degrees, can actually
    /// start at one (the base) yielding grid lines at 1, 3, 5, 7, etc.
    /// 
    virtual double GetNorthingBase (void)=0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the easting increment value (the interval between easting grid lines).
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual double GetEastingIncrement ();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual double GetEastingIncrement ();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual double GetEastingIncrement ();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the current value of the easting increment member.
    ///
    /// \remarks
    /// The distance between grid lines representing easting values.
    ///
    virtual double GetEastingIncrement(void)=0;         // the interval between easting grid lines

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the northing increment value.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual double GetNorthingIncrement ();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual double GetNorthingIncrement ();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual double GetNorthingIncrement ();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the current value of the northing increment member.
    ///
    /// \remarks
    /// The distance between grid lines representing northing values.
    ///
    virtual double GetNorthingIncrement(void)=0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the tick easting increment value.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual double GetTickEastingIncrement ();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual double GetTickEastingIncrement ();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual double GetTickEastingIncrement ();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the current value of the tick easting increment member.
    ///
    /// \remarks
    /// The distance between ticks representing specific easting values.
    ///
    virtual double GetTickEastingIncrement(void)=0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the tick northing increment value.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual double GetTickNorthingIncrement ();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual double GetTickNorthingIncrement ();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual double GetTickNorthingIncrement ();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the current value of the tick northing increment member.
    ///
    /// \remarks
    /// The distance between ticks representing specific northing values.
    ///
    virtual double GetTickNorthingIncrement(void)=0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the current unit type setting.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual int GetUnitType();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual int GetUnitType();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual int GetUnitType();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the current unit type setting.
    ///
    /// \remarks
    /// The value returned is one of the values defined in the
    /// MgCoordinateSystemUnitType object and essentially specifies if the unit
    /// specification is of the linear or angular type.
    ///
    virtual INT32 GetUnitType(void)=0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the current unit code setting.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual int GetUnitCode();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual int GetUnitCode();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual int GetUnitCode();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the current unit code setting.
    ///
    /// \remarks
    /// The value returned is one of the values defined in the
    /// MgCoordinateSystemUnitCode object and essentially specifies the unit in
    /// which the base, increment (line and tick), and curve precision values
    /// are specified.
    ///
    virtual INT32 GetUnitCode(void)=0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Determines if two MgCoordinateSystemGridSpecification objects are the
    /// same.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual bool IsSameAs (MgCoordinateSystenGridSpecification secondObject);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual bool IsSameAs (MgCoordinateSystenGridSpecification secondObject);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual bool IsSameAs (MgCoordinateSystenGridSpecification secondObject);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param specification (MgCoordinateSystemGridSpecification)
    /// The second object to which the host object is to be compared to.
    ///
    /// \return
    /// Returns true if the objects are identical.
    ///
    /// \remarks
    /// Objects with a different unit settings are automatically considered
    /// different.  Therefore the intelligence necessary to compare values which
    /// are identical in different units is <b>not</b> present.
    ///
    virtual bool IsSameAs(MgCoordinateSystemGridSpecification* specification)=0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the current curve precision value.
    /// same.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual double GetCurvePrecision ();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual double GetCurvePrecision ();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual double GetCurvePrecision ();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the current curve precision value.
    ///
    /// \remarks
    /// The curve precision value specifies the desired maximum distance between
    /// a complex curve approximation and the actual complex curve.  This value
    /// may be left at zero, and a suitable default value will be choosen by the
    /// grid generation engine at grid line generation time.
    ///
    virtual double GetCurvePrecision(void)=0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the easting and northing base values.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual void SetGridBase (double eastingBase, double northingBase);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual void SetGridBase (double eastingBase, double northingBase);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual void SetGridBase (double eastingBase, double northingBase);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param eastingBase (double)
    /// The base value to be used in calculating the easting grid and tick
    /// values.
    ///
    /// \param northingBase (double)
    /// The base value to be used in calculating the northing grid and tick
    /// values.
    ///
    /// \remarks
    /// While the increment values determine the distance between grid lines
    /// and ticks, the base value specifies the base value at which the
    /// initial grid line/tick is positioned.  Actually, the base for any
    /// grid is the algebraically highest value which is less than the
    /// grid boundary minimum and which, when mod'ed with base, produces zero.
    ///
    /// That is, for example, an increment of 2 and a base of 1 will produce
    /// grid values of 1, 3, 5, 7, etc.
    ///
    virtual void SetGridBase(double eastingBase,double northingBase) = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the easting and northing grid line increment values.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual void SetGridIncrement (double eastingIncrement, double northingIncrement);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual void SetGridIncrement (double eastingIncrement, double northingIncrement);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual void SetGridIncrement (double eastingIncrement, double northingIncrement);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param eastingIncrement (double)
    /// The distance between easting grid lines.
    ///
    /// \param northingIncrement (double)
    /// The distance between northing grid lines.
    ///
    /// \remarks
    /// In this context, easting refers to grid lines which represent a specific
    /// easting value; the resulting lines usually being vertical (i.e. south to
    /// north) grid lines.  Northing refers to grid lines which represent a
    /// specific northing value; the resulting lines usually being horizontal
    /// (i.e. west to east).
    ///
    /// \bug
    /// This function should be named SetGridIncrements to be consistent with the
    /// rest of the interface.
    ///
    virtual void SetGridIncrement(double eastingIncrement,double northingIncrement) = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the easting and northing tick increment values.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual void SetTickIncrements (double eastingIncrement, double northingIncrement);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual void SetTickIncrements (double eastingIncrement, double northingIncrement);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual void SetTickIncrements (double eastingIncrement, double northingIncrement);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param eastingIncrement (double)
    /// The distance between easting tick marks.
    ///
    /// \param northingIncrement (double)
    /// The distance between northing tick marks.
    ///
    virtual void SetTickIncrements(double eastingIncrement,double northingIncrement) = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the units used to define the desired grid.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual void SetUnits (int unitCode, int unitType);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual void SetUnits (int unitCode, int unitType);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual void SetUnits (int unitCode, int unitType);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param unitCode (int)
    /// One of the integer values defined in the MgCoordinateSystemUnitCode
    /// object which indicates the units used to define the grid parameters.
    ///
    /// \param unitType (int)
    /// One of the integer values defined in the MgCoordinateSystemUnitType
    /// object which indicates the type of units (linear vs angular) used to
    /// define the grid parameters.
    ///
    /// \remarks
    /// The unitCode and unitType parameters must be consistent with each other
    /// or an exception is thrown.  Also, the unit tyupe must be consistent
    /// with the type of grid being requested or an exception will be thrown.
    /// That is, you may not define a graticule (i.e. a grid of geographic
    /// coordinates on a projected coordinate system map) using linear units.
    /// SImilarly, you cannot use angular units to define a grid of a projected
    /// coordinate system on a projected coordinate system map.
    ///
    virtual void SetUnits (INT32 unitCode,INT32 unitType) = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the desired precision of complex curve approximations.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual void SetCurvePrecision (double curvePrecision);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual void SetCurvePrecision (double curvePrecision);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual void SetCurvePrecision (double curvePrecision);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param curvePrecision (double)
    /// The desired maximum distance between an approximation of a complex curve
    /// and the true curve.
    ///
    /// \remarks
    /// Please note the term desired in this description.  The generation of a
    /// complex curve approximation is also controlled by a variable usually
    /// called the MaximumCurvePoints.  Thus, if this criteria is exceeded in
    /// the curve generation process, the curve precision can and will often be
    /// less than that requested.  This provision is implemented to reduce the
    /// probability of a runaway grid which consumes the entire machine.
    ///
    /// \bug
    /// The maximum curve point variable should be a part of this object.
    ///
    virtual void SetCurvePrecision (double curvePrecision) = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the maximum number of points to generate for a complex curve
    /// approximation.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual void SetMaxCurvePoints (double curvePrecision);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual void SetMaxCurvePoints (double curvePrecision);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual void SetMaxCurvePoints (double curvePrecision);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param maxCurvePoints (INT32)
    /// The desired maximum number of points to be generated for a complex
    /// curve approximation.  Default is 511.
    ///
    /// \remarks
    /// Generating a grid line of one coordinate system in the model space
    /// based on another coordinate system requires that an approximation
    /// of the complex curve be generated by a series of straight line
    /// segments.  Use this value to limit the number of points generated
    /// to a specific value.  The traditional, and default, value is 511.
    /// Large values will significantly increase memeory usage and reduce
    /// performance.  Using a smaller value is an easy way to reduce
    /// memory consumption.
    ///
    virtual void SetMaxCurvePoints (INT32 maxCurvePoints) = 0;


    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Checks the internal contents of the object for consistency.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual bool IsConsistent ();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual bool IsConsistent ();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual bool IsConsistent ();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// returns true if the values set are consistent with a valid
    /// specification block.
    ///
    /// \remarks
    /// Will not throw.
    virtual bool IsConsistent (void) = 0;

INTERNAL_API:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the easting base value after conversion from the units
    /// of this specification object to the units requested by the argument.
    ///
    /// \param resultUnitCode
    /// The return value is converted to the units indicated by this unit
    /// code.
    ///
    /// \return
    /// Easting base value in units indicated by the resultUnitCode
    /// parameter.
    ///
    /// \exception
    /// Will throw if the unit types of this specification and the provided
    /// argument are not the same.
    ///
    virtual double GetEastingBase (INT32 resultUnitCode)=0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the northing base value after conversion from the units
    /// of this specification object to the units requested by the argument.
    ///
    /// \param resultUnitCode
    /// The return value is converted to the units indicated by this unit
    /// code.
    ///
    /// \return
    /// Northing base value in units indicated by the resultUnitCode
    /// parameter.
    ///
    /// \exception
    /// Will throw if the unit types of this specification and the provided
    /// argument are not the same.
    ///
    virtual double GetNorthingBase (INT32 resultUnitCode)=0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the easting increment value after conversion from the units
    /// of this specification object to the units requested by the argument.
    ///
    /// \param resultUnitCode
    /// The return value is converted to the units indicated by this unit
    /// code.
    ///
    /// \return
    /// Easting increment value in units indicated by the resultUnitCode
    /// parameter.
    ///
    /// \exception
    /// Will throw if the unit types of this specification and the provided
    /// argument are not the same.
    ///
    virtual double GetEastingIncrement (INT32 resultUnitCode)=0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the northing increment value after conversion from the units
    /// of this specification object to the units requested by the argument.
    ///
    /// \param resultUnitCode
    /// The return value is converted to the units indicated by this unit
    /// code.
    ///
    /// \return
    /// Northing increment value in units indicated by the resultUnitCode
    /// parameter.
    ///
    /// \exception
    /// Will throw if the unit types of this specification and the provided
    /// argument are not the same.
    ///
    virtual double GetNorthingIncrement (INT32 resultUnitCode)=0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the tick easting increment value after conversion from the
    /// units of this specification object to the units requested by the
    /// argument.
    ///
    /// \param resultUnitCode
    /// The return value is converted to the units indicated by this unit
    /// code.
    ///
    /// \return
    /// Tick easting increment value in units indicated by the
    /// resultUnitCode parameter.
    ///
    /// \exception
    /// Will throw if the unit types of this specification and the provided
    /// argument are not the same.
    ///
    virtual double GetTickEastingIncrement (INT32 resultUnitCode)=0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the tick northing increment value after conversion from the
    /// units of this specification object to the units requested by the
    /// argument.
    ///
    /// \param resultUnitCode
    /// The return value is converted to the units indicated by this unit
    /// code.
    ///
    /// \return
    /// Tick northing increment value in units indicated by the
    /// resultUnitCode parameter.
    ///
    /// \exception
    /// Will throw if the unit types of this specification and the provided
    /// argument are not the same.
    ///
    virtual double GetTickNorthingIncrement (INT32 resultUnitCode)=0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the curve precision value after conversion from the units
    /// of this specification object to the units of the provided coordinate
    /// system.
    ///
    /// \param gridCRS
    /// The return value is converted to the units of this coordinate system.
    ///
    /// \return
    /// Curve precision value in units of the provided coordinate system.
    ///
    /// \exception
    /// Will throw if the unit types of this specification and the provided
    /// coordinate system are not the same.
    ///
    /// \remarks
    /// If this specification object's curve precision value has not been set
    /// or is otherwise set to zero, this function will manufacture a curve
    /// precision value consistent with the coordinate system provided.  Note
    /// that this default value will be more liberal than conservative.
    ///
    /// \bugs
    /// The automatically calculated curve precision value should be a function
    /// of the Grid Boundary object which known how much geography is to be
    /// included in the grid and is thus in a better position to determine
    /// what the default value should be.
    ///
    virtual double GetCurvePrecision (MgCoordinateSystem* gridCRS)=0;

protected:
     INT32 GetClassId(){return m_cls_id;};
CLASS_ID:
    static const INT32 m_cls_id = CoordinateSystem_CoordinateSystemGridSpecification;
};
/// \}

/// \defgroup MgCoordinateSystemGridBoundary MgCoordinateSystemGridBoundary
/// \ingroup Coordinate_System_classes
/// \{

//=============================================================================
// External to this interface, boundary objects are always in the viewport
// (i.e. target) coordinate system.
///////////////////////////////////////////////////////////////////////////
/// \brief
/// This object is used to maintain the definition of the boundary of a
/// specific grid or graticule.  Externally, a
/// MgCoordinateSystemGridBoundary object will be in viewport
/// coordinates.  Internally, objects of this type are often used to the
/// carry grid boundaries in grid coordinates, and also greographic
/// coordinates.
///
/// Grid boundaries iusually start out as rectangles, but are often converted
/// to a series of complex curves approximated by multi-segment lines (i.e.
/// line strings).
///
/// \since 2.2
class MG_GEOMETRY_API MgCoordinateSystemGridBoundary : public MgGuardDisposable
{
PUBLISHED_API:
    // Contrary to other envelope objects, the first of these overloads specifically
    // requires that the southwest argument indeed be southwest of the northeast
    // argument.  Necessary to support geographic coordinate systems (i.e. +/- 180).

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the extents of this grid boundary
    ///
    /// \param southwest (MgCoordinate)
    /// The southwest coordinate of the extents
    /// \param northeast (MgCoordinate) 
    /// The northeast coordinate of the extents
    ///
    /// \remarks
    /// Contrary to other envelope objects, this method specifically
    /// requires that the southwest argument indeed be southwest of the northeast
    /// argument. Necessary to support geographic coordinate systems (i.e. +/- 180).
    ///
    virtual void SetBoundaryExtents (MgCoordinate* southwest, MgCoordinate* northeast)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the extents of this grid boundary
    ///
    /// \param boundary (MgPolygon)
    /// The extents of this grid boundary
    ///
    virtual void SetBoundaryExtents (MgPolygon* boundary)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the geometry representing the grid boundary
    ///
    /// \return
    /// The polygon geometry representing the grid boundary
    ///
    virtual MgPolygon* GetBoundary (void) const=0;
INTERNAL_API:

    virtual void SetMaxCurvePoints (INT32 maxPoints) = 0;
    virtual void GetBoundaryExtents (double& eastMin,double& eastMax,double& northMin,double& northMax) const = 0;
    virtual MgPolygon* GetBoundary (MgCoordinateSystemTransform* transformation,double precision) = 0;
    virtual MgLineStringCollection* ClipLineString (MgLineString* lineString) const=0;
protected:
    INT32 GetClassId(){return m_cls_id;};
CLASS_ID:
    static const INT32 m_cls_id = CoordinateSystem_CoordinateSystemGridBoundary;
};

/// \}

/// \defgroup MgCoordinateSystemGridBase MgCoordinateSystemGridBase
/// \ingroup Coordinate_System_classes
/// \{

////////////////////////////////////////////////////////////////
/// \brief
/// Abstract base class for all grid generation objects (generic and
/// specialized).
/// \since 2.2
class MG_GEOMETRY_API MgCoordinateSystemGridBase : public MgGuardDisposable
{
PUBLISHED_API:
    // The following may be somewhat redundant in view of the fact that all grid
    // specialization objects will have their own ClassID value.  Nevertheless,
    // the specialization type enumeration and this function are provided to
    // support exposure of this interface to other environments in a standard
    // pre-determined manner.  In this manner, therefore. this module will
    // continue to control the ID associated with any given specialization.

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the specialized type of this grid instance. Value can be any value from MgCoordinateSystemGridSpecializationType
    ///
    /// \return
    /// The specialized type of this grid instance (MgCoordinateSystemGridSpecializationType)
    ///
    virtual INT32 GetSpecializationType ()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the boundary of this grid instance
    ///
    /// \param pGridBoundary (MgCoordinateSystemGridBoundary)
    /// The boundary
    ///
    virtual void SetBoundary (MgCoordinateSystemGridBoundary* pGridBoundary) = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the boundary of this grid instance
    ///
    /// \return
    /// The boundary of this grid instance
    ///
    virtual MgCoordinateSystemGridBoundary* GetBoundary(void)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the grid lines for this grid instance for the given grid specification
    ///
    /// \param specification (MgCoordinateSystemGridSpecification)
    /// The grid specification
    ///
    /// \return
    /// The collection of grid lines
    ///
    virtual MgCoordinateSystemGridLineCollection* GetGridLines (MgCoordinateSystemGridSpecification* specification)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the grid regions for this grid instance for the given grid specification
    ///
    /// \param specification (MgCoordinateSystemGridSpecification)
    /// The grid specification
    ///
    /// \return
    /// The collection of grid regions
    ///
    virtual MgCoordinateSystemGridRegionCollection* GetGridRegions (MgCoordinateSystemGridSpecification* specification)=0;
    
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Generates grid ticks for the grid boundary.
    ///
    /// \param specification (MgCoordinateSystemGridSpecification)
    /// The grid specification
    ///
    /// \return
    /// The collection of grid ticks for the grid boundary
    ///
    virtual MgCoordinateSystemGridTickCollection* GetGridTicks (MgCoordinateSystemGridSpecification* specification)=0;

    // The following can be of value for rendering and stylization objects
    // which need to render north arrows, scale bars, etc.  Location
    // parameter is always in viewport/frame coordinates.

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the convergence angle for the given location
    ///
    /// \param location (MgCoordinate)
    /// The location
    ///
    /// \return
    /// The convergence angle for the given location
    ///
    /// \remarks
    /// The following can be of value for rendering and stylization objects
    /// which need to render north arrows, scale bars, etc.  Location
    /// parameter is always in viewport/frame coordinates.
    ///
    virtual double GetConvergenceAngle (MgCoordinate* location) = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the projective grid scale for the given location
    ///
    /// \param location (MgCoordinate)
    /// The location
    ///
    /// \return
    /// The projective grid scale for the given location
    ///
    /// \remarks
    /// The following can be of value for rendering and stylization objects
    /// which need to render north arrows, scale bars, etc.  Location
    /// parameter is always in viewport/frame coordinates.
    ///
    virtual double GetProjectiveGridScale (MgCoordinate* location) = 0;

    // The following may be used to determine if a specific grid generation
    // should be attempted.  The returned values are approximate and is in bytes.
    // The proposed grid boundary for the grid object hosting this interface must
    // set prior to calling this function.  Failure to do so will cause a -1
    // return value.

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Calculates the approximate amount of memory required to generated grid lines for this instance
    ///
    /// \param specification (MgCoordinateSystemGridSpecification)
    /// The grid specification
    ///
    /// \return
    /// The approximate memory usage in bytes
    ///
    /// \remarks
    /// This method may be used to determine if a specific grid generation
    /// should be attempted. The returned values are approximate and is in bytes.
    /// The proposed grid boundary for the grid object hosting this interface must
    /// set prior to calling this function.  Failure to do so will cause a -1
    /// return value.
    ///
    virtual INT32 ApproxGridLineMemoryUsage (MgCoordinateSystemGridSpecification* specification) = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Calculates the approximate amount of memory required to generated grid regions for this instance
    ///
    /// \param specification (MgCoordinateSystemGridSpecification)
    /// The grid specification
    ///
    /// \return
    /// The approximate memory usage in bytes
    ///
    /// \remarks
    /// This method may be used to determine if a specific grid generation
    /// should be attempted. The returned values are approximate and is in bytes.
    /// The proposed grid boundary for the grid object hosting this interface must
    /// set prior to calling this function.  Failure to do so will cause a -1
    /// return value.
    ///
    virtual INT32 ApproxGridRegionMemoryUsage (MgCoordinateSystemGridSpecification* specification) = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Calculates the approximate amount of memory required to generated grid ticks for this instance
    ///
    /// \param specification (MgCoordinateSystemGridSpecification)
    /// The grid specification
    ///
    /// \return
    /// The approximate memory usage in bytes
    ///
    /// \remarks
    /// This method may be used to determine if a specific grid generation
    /// should be attempted. The returned values are approximate and is in bytes.
    /// The proposed grid boundary for the grid object hosting this interface must
    /// set prior to calling this function.  Failure to do so will cause a -1
    /// return value.
    ///
    virtual INT32 ApproxGridTickMemoryUsage (MgCoordinateSystemGridSpecification* specification) = 0;

    // Each grid object establishes a hard coded value and will throw an exception
    // before generating a grid which, accordinag to the grid object estimates, will
    // cause memory usage to exceed the hard coded value.  Use the following functions
    // to modify the threshold for the exception.  Such modification remains in effect
    // only within the existence of the modified object.
    // These functions return the previous value and make no changes if the argument
    // value is less than 10MB.  The memoryUseMax argument is always in bytes.

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the amount of memory at which an exception will be thrown if generating a grid line 
    /// collection that would exceed the specified value in memory usage
    ///
    /// \param memoryUseMax (int)
    /// The memory limit in bytes
    ///
    /// \return
    /// The new memory limit
    ///
    /// \remarks
    /// Each grid object establishes a hard coded value and will throw an exception
    /// before generating a grid which, accordinag to the grid object estimates, will
    /// cause memory usage to exceed the hard coded value. This method can be used to
    /// to modify the threshold for the exception.  Such modification remains in effect
    /// only within the existence of the modified object.
    /// These functions return the previous value and make no changes if the argument
    /// value is less than 10MB.  The memoryUseMax argument is always in bytes.
    ///
    virtual INT32 SetGridLineExceptionLevel (INT32 memoryUseMax) = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the amount of memory at which an exception will be thrown if generating a grid region 
    /// collection that would exceed the specified value in memory usage
    ///
    /// \param memoryUseMax (int)
    /// The memory limit in bytes
    ///
    /// \return
    /// The new memory limit
    ///
    /// \remarks
    /// Each grid object establishes a hard coded value and will throw an exception
    /// before generating a grid which, accordinag to the grid object estimates, will
    /// cause memory usage to exceed the hard coded value. This method can be used to
    /// to modify the threshold for the exception.  Such modification remains in effect
    /// only within the existence of the modified object.
    /// These functions return the previous value and make no changes if the argument
    /// value is less than 10MB.  The memoryUseMax argument is always in bytes.
    ///
    virtual INT32 SetGridRegionExceptionLevel (INT32 memoryUseMax) = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the amount of memory at which an exception will be thrown if generating a grid tick 
    /// collection that would exceed the specified value in memory usage
    ///
    /// \param memoryUseMax (int)
    /// The memory limit in bytes
    ///
    /// \return
    /// The new memory limit
    ///
    /// \remarks
    /// Each grid object establishes a hard coded value and will throw an exception
    /// before generating a grid which, accordinag to the grid object estimates, will
    /// cause memory usage to exceed the hard coded value. This method can be used to
    /// to modify the threshold for the exception.  Such modification remains in effect
    /// only within the existence of the modified object.
    /// These functions return the previous value and make no changes if the argument
    /// value is less than 10MB.  The memoryUseMax argument is always in bytes.
    ///
    virtual INT32 SetGridTickExceptionLevel (INT32 memoryUseMax) = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the last error code
    ///
    /// \return
    /// The last error code
    ///
    virtual INT32 GetLastError() = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Resets the last error code
    /// 
    virtual void ResetLastError()= 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets whether exceptions are enabled for this instance
    ///
    /// \return
    /// true if exceptions are enabled. false otherwise
    ///
    virtual bool AreExceptionsOn() = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Enables or disables exceptions for this instance
    ///
    /// \param bOn (boolean/bool)
    /// true to enable exceptions. false to disable exceptions
    ///
    virtual void SetExceptionsOn(bool bOn) = 0;
INTERNAL_API:
protected:
CLASS_ID:
    static const INT32 m_cls_id = CoordinateSystem_CoordinateSystemGridBase;
};
/// \}

/// \defgroup MgCoordinateSystemGridLine MgCoordinateSystemGridLine
/// \ingroup Coordinate_System_classes
/// \{

////////////////////////////////////////////////////////////////
/// \brief
/// An MgCoordinateSystemGridLine object is a grid value (as a double) and a
/// collection of line strings, where each individual line string being
/// considered a grid line segment.  All line strings in the collection are in
/// the viewport coordinate system.  The value returned by GetGridOrientation
/// indicates the nature of the grid line in grid coordinates (i.e. easting or
/// northing).
///
/// \since 2.2
class MG_GEOMETRY_API MgCoordinateSystemGridLine : public MgGuardDisposable
{
PUBLISHED_API:
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the grid orientation for this instance (see MgCoordinateSystemGridOrientation)
    ///
    /// \return
    /// The grid orientation for this instance (see MgCoordinateSystemGridOrientation)
    ///
    virtual INT32 GetGridOrientation(void)= 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the real value for this instance
    ///
    /// \return
    /// The real value for this instance
    ///
    virtual double GetRealValue(void)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the number of line segments in this instance
    ///
    /// \return
    /// The number of line segments in this instance
    ///
    virtual INT32 GetCount (void) = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the line segment at the specified index
    ///
    /// \param index (int)
    /// The index at which to retrieve the line segment
    ///
    /// \return
    /// The MgLineString at the specified index
    ///
    virtual MgLineString* GetSegment (INT32 index)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the entire line segment collection
    ///
    /// \return
    /// The MgLineStringCollection containing the line segments
    ///
    virtual MgLineStringCollection* GetSegmentCollection(void)= 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the entire line segment collection
    ///
    /// \param segmentCollection (MgLineStringCollection)
    /// The line segment collection
    ///
    virtual void SetSegmentCollection (MgLineStringCollection* segmentCollection) = 0;
INTERNAL_API:
protected:
    INT32 GetClassId(){return m_cls_id;};
CLASS_ID:
    static const INT32 m_cls_id = CoordinateSystem_CoordinateSystemGridLine;
};
/// \}

/// \defgroup MgCoordinateSystemGridRegion MgCoordinateSystemGridRegion
/// \ingroup Coordinate_System_classes
/// \{

//=============================================================================
// An MgCoordinateSystemGridRegion object consists of a label and a polygon.
// The polygon is always in the viewport coordinate system.  Such an object
// is used, for example, to label and delineate an MGRS 100Km UTM grid zone.

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// This API has been found to be problematical in the initial release.  It   //
// will be deprecated in the next release.                                   //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// An MgCoordinateSystemGridRegion object consists of a label and a polygon.
/// The polygon is always in the viewport coordinate system.  Such an object
/// is used, for example, to label and delineate an MGRS 100Km UTM grid zone.
///
/// \since 2.2
class MG_GEOMETRY_API MgCoordinateSystemGridRegion : public MgGuardDisposable
{
PUBLISHED_API:
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the label for this region
    ///
    /// \return
    /// The label for this region
    ///
    virtual STRING GetLabel () = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the center of this region
    ///
    /// \return
    /// The center (MgCoordinate) of this region
    ///
    virtual MgCoordinate* GetRegionCenter (void) = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the boundary geometry of this region
    ///
    /// \return
    /// The boundary geometry (MgPolygon) of this region
    ///
    virtual MgPolygon* GetRegionBoundary (void) = 0;

    // The returns from the following members are clipped to the frame boundary
    // of the grid object from which the region object was obtained.  Since it
    // is possible (rare, but possible) that a region boundary line enters and
    // leaves the frame boundary more than once.

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the south line of this region
    ///
    /// \return
    /// The south line (MgLineStringCollection) of this region
    ///
    /// \remarks
    /// The return value is clipped to the frame boundary
    /// of the grid object from which the region object was obtained.  Since it
    /// is possible (rare, but possible) that a region boundary line enters and
    /// leaves the frame boundary more than once.
    ///
    virtual MgLineStringCollection* GetSouthLine (void) = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the east line of this region
    ///
    /// \return
    /// The east line (MgLineStringCollection) of this region
    ///
    /// \remarks
    /// The return value is clipped to the frame boundary
    /// of the grid object from which the region object was obtained.  Since it
    /// is possible (rare, but possible) that a region boundary line enters and
    /// leaves the frame boundary more than once.
    ///
    virtual MgLineStringCollection* GetEastLine (void) = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the north line of this region
    ///
    /// \return
    /// The north line (MgLineStringCollection) of this region
    ///
    /// \remarks
    /// The return value is clipped to the frame boundary
    /// of the grid object from which the region object was obtained.  Since it
    /// is possible (rare, but possible) that a region boundary line enters and
    /// leaves the frame boundary more than once.
    ///
    virtual MgLineStringCollection* GetNorthLine (void) = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the west line of this region
    ///
    /// \return
    /// The west line (MgLineStringCollection) of this region
    ///
    /// \remarks
    /// The return value is clipped to the frame boundary
    /// of the grid object from which the region object was obtained.  Since it
    /// is possible (rare, but possible) that a region boundary line enters and
    /// leaves the frame boundary more than once.
    ///
    virtual MgLineStringCollection* GetWestLine (void) = 0;

INTERNAL_API:
protected:
    INT32 GetClassId(){return m_cls_id;};
CLASS_ID:
    static const INT32 m_cls_id = CoordinateSystem_CoordinateSystemGridRegion;
};
/// \}


/// \defgroup MgCoordinateSystemGridTick MgCoordinateSystemGridTick
/// \ingroup Coordinate_System_classes
/// \{
///////////////////////////////////////////////////////////////////////////////
/// \brief
/// An MgCoordinateSystemGridTick object consists of a grid value, a tick
/// location point in viewport coordinates, and a 2D unit direction vector.
/// Position is the position of the tick mark on a grid or boundary line.  The
/// 2D unit direction vector is the direction of the line at the position given.
/// In the case of a tick mark on a boundary line, since boundary lines are
/// always maintained in the counterclockwise direction, the interior of the
/// viewport will always be to the left of the direction vector.
///
/// \since 2.2
class MG_GEOMETRY_API MgCoordinateSystemGridTick : public MgGuardDisposable
{
PUBLISHED_API:
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets whether this grid tick is on a grid line
    ///
    /// \return
    /// true if this grid tick is on a grid line. false otherwise
    ///
    virtual bool GetIsOnGridLine () = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the tick orientation (see MgCoordinateSystemGridOrientation for valid values)
    ///
    /// \return
    /// The tick orientation
    ///
    virtual INT32 GetTickOrientation () = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the tick value
    ///
    /// \return
    /// The tick value
    ///
    virtual double GetValue () = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the tick position
    ///
    /// \return
    /// The tick position
    ///
    virtual MgCoordinate* GetPosition () = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the tick's direction vector
    ///
    /// \return
    /// The tick's direction vector
    ///
    virtual MgCoordinate* GetDirectionVector () = 0;
INTERNAL_API:
protected:
    INT32 GetClassId(){return m_cls_id;};
CLASS_ID:
    static const INT32 m_cls_id = CoordinateSystem_CoordinateSystemGridTick;
};
/// \}

/// \defgroup MgCoordinateSystemGridLineCollection MgCoordinateSystemGridLineCollection
/// \ingroup Coordinate_System_classes
/// \{
///////////////////////////////////////////////////////////////////////////////
/// \brief
/// An MgCoordinateSystemGridLineCollection is collection of
/// MgCoordinateSystemGridLine objects.  As manufactured, grid horizontal lines
/// appear first in ascending order by grid value (easting in this case);
/// followed by grid vertical lines in ascending order by grid value (northing
/// in this case).
///
/// \since 2.2
class MG_GEOMETRY_API MgCoordinateSystemGridLineCollection : public MgGuardDisposable
{
PUBLISHED_API:
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the number of grid lines in this collection
    ///
    /// \return
    /// The number of grid lines in this collection
    ///
    virtual INT32 GetCount () const=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the grid line at the specified index
    ///
    /// \param index (int)
    /// The index
    ///
    /// \return
    /// The grid line (MgCoordinateSystemGridLine) at the specified index
    ///
    virtual MgCoordinateSystemGridLine* GetItem (INT32 index) const=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the index of the given grid orientation and value
    ///
    /// \param gridOrientation (int)
    /// The grid orientation
    /// \param gridValue (double)
    /// The grid value
    ///
    /// \return
    /// The index of the given grid orientation and value
    ///
    virtual INT32 IndexOf (INT32 gridOrientation,double gridValue) const=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Removes the grid line at the specified index
    ///
    /// \param index (int)
    /// The index to remove
    ///
    virtual void RemoveAt (INT32 index)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Removes all grid lines from this collection
    ///
    virtual void Clear()=0;
INTERNAL_API:
    virtual void SetItem (INT32 index, MgCoordinateSystemGridLine* value)=0;
    virtual void Add (MgCoordinateSystemGridLine* value)=0;
    virtual void AddCollection (MgCoordinateSystemGridLineCollection* aGridLineCollection)=0;

protected:
    INT32 GetClassId(){return m_cls_id;};
CLASS_ID:
    static const INT32 m_cls_id = CoordinateSystem_CoordinateSystemGridLineCollection;
};
/// \}

/// \defgroup MgCoordinateSystemGridRegionCollection MgCoordinateSystemGridRegionCollection
/// \ingroup Coordinate_System_classes
/// \{
///////////////////////////////////////////////////////////////////////////////
/// \brief
/// An MgCoordinateSystemGridRegionCollection is collection of
/// MgCoordinateSystemGridRegion objects.
/// \since 2.2
class MG_GEOMETRY_API MgCoordinateSystemGridRegionCollection : public MgGuardDisposable
{
PUBLISHED_API:
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the number of grid regions in this collection
    ///
    /// \return
    /// The number of grid regions in this collection
    ///
    virtual INT32 GetCount () const=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the grid region at the specified index
    ///
    /// \param index (int)
    /// The index
    ///
    /// \return
    /// The grid region (MgCoordinateSystemGridRegion) at the specified index
    ///
    virtual MgCoordinateSystemGridRegion* GetItem (INT32 index) const=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Removes the grid region at the specified index
    ///
    /// \param index (int)
    /// The index to remove
    ///
    virtual void RemoveAt (INT32 index)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Removes all grid regions from this collection
    ///
    virtual void Clear()=0;
INTERNAL_API:
    virtual void SetItem (INT32 index, MgCoordinateSystemGridRegion* value)=0;
    virtual void Add (MgCoordinateSystemGridRegion* value)=0;

protected:
    INT32 GetClassId(){return m_cls_id;};
CLASS_ID:
    static const INT32 m_cls_id = CoordinateSystem_CoordinateSystemGridRegionCollection;
};
/// \}

/// \defgroup MgCoordinateSystemGridTickCollection MgCoordinateSystemGridTickCollection
/// \ingroup Coordinate_System_classes
/// \{
///////////////////////////////////////////////////////////////////////////////
/// \brief
/// An MgCoordinateSystemGridTickCollection is collection of
/// MgCoordinateSystemGridTick objects.  MgCoordinateSystemGridTickCollection
/// objects will contain MgCoordinateSystemGridTick objects for the entire
/// boundary, or for an individual grid line.
/// \since 2.2
class MG_GEOMETRY_API MgCoordinateSystemGridTickCollection : public MgGuardDisposable
{
PUBLISHED_API:
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the number of grid ticks in this collection
    ///
    /// \return
    /// The number of grid ticks in this collection
    ///
    virtual INT32 GetCount () const=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the grid tick at the specified index
    ///
    /// \param index (int)
    /// The index
    ///
    /// \return
    /// The grid tick (MgCoordinateSystemGridTick) at the specified index
    ///
    virtual MgCoordinateSystemGridTick* GetItem (INT32 index) const=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Removes the grid tick at the specified index
    ///
    /// \param index (int)
    /// The index to remove
    ///
    virtual void RemoveAt (INT32 index)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Removes all grid ticks from this collection
    ///
    virtual void Clear()=0;
INTERNAL_API:
    virtual void SetItem (INT32 index, MgCoordinateSystemGridTick* value)=0;
    virtual void Add (MgCoordinateSystemGridTick* value)=0;

protected:
    INT32 GetClassId(){return m_cls_id;};
CLASS_ID:
    static const INT32 m_cls_id = CoordinateSystem_CoordinateSystemGridTickCollection;
};
/// \}

#endif //_MGCOORDINATESYSTEMGRIDS_H_

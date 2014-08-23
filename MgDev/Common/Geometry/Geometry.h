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

#ifndef _MGGEOMETRY_H_
#define _MGGEOMETRY_H_

class MgGeometry;
template class MG_GEOMETRY_API Ptr<MgGeometry>;

/// \defgroup MgGeometry MgGeometry
/// \ingroup Geometry_Module_classes
/// \{

////////////////////////////////////////////////////////////////
/// \brief
/// MgGeometry is an abstract base class for all of the geometry
/// types.
///
/// \remarks
/// All of the geometries are constructed by calling
/// non-static methods on an instance of MgGeometryFactory and
/// are immutable once constructed.
///
/// \note
/// The following analysis functions would throw
/// MgGeometryException if called on an instance of
/// MgMultiGeometry:
/// <ol>
///   <li>Intersection</li>
///   <li>Union</li>
///   <li>Difference</li>
///   <li>SymetricDifference</li>
///   <li>Disjoint</li>
///   <li>Touches</li>
///   <li>Intersects</li>
///   <li>Crosses</li>
///   <li>Within</li>
///   <li>Contains</li>
///   <li>Overlaps</li>
///  <li>Equals</li>
/// </ol>
///

class MG_GEOMETRY_API MgGeometry : public MgGeometricEntity
{
PUBLISHED_API:
    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the type of this geometry.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual int GetGeometryType();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual int GetGeometryType();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual int GetGeometryType();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the value defined in MgGeometryType corresponding to
    /// this geometry.
    ///
    virtual INT32 GetGeometryType() = 0;  /// __get

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// This is a convenience method. Given 2 geometries a and b,
    /// a.Contains(b) is true if and only if b.MgGeometry::Within(a)
    /// is true.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual bool Contains(MgGeometry other);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual boolean Contains(MgGeometry other);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual bool Contains(MgGeometry other);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param other (MgGeometry)
    /// The MgGeometry to test against this one.
    ///
    /// \return
    /// True if the other geometry is within this geometry, false
    /// otherwise.
    ///
    virtual bool Contains(MgGeometry* other);

    //////////////////////////////////////////////////////////////////////
    /// \brief
    /// Given 2 geometries a and b, a.Crosses(b) is true if and only
    /// if the dimension of the intersection of the interior of a and
    /// the interior of b is less than the greater of the dimension
    /// of the interior of a and the dimension of the interior of b
    /// and the intersection of a and b is neither a nor b.
    ///
    /// \remarks
    /// A Point cannot cross another geometry because the
    /// intersection of the Point with the other geometry is the
    /// Point.
    /// \n
    /// Two MultiPoint geometries cannot cross one another because
    /// the dimension of the intersection of their interiors, namely
    /// the 0-dimension, is not less than the greater of the
    /// dimensions of their interiors, namely the 0-dimension.
    /// \n
    /// [\link OGC99049 OGC99-049 \endlink] implicitly excludes a Crosses
    /// relationship between 2 polygons. According to the definition,
    /// the possibility of such a relationship would require that the
    /// intersection of the interior of one polygon with that of
    /// another be a Point or Line.
    /// <h2>Illustration</h2>
    /// d, e and f are MultiPoint geometries.
    /// \n
    /// \image html crosses.png
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual bool Crosses(MgGeometry other);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual boolean Crosses(MgGeometry other);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual bool Crosses(MgGeometry other);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param other (MgGeometry)
    /// The MgGeometry to test against this
    /// one.
    ///
    /// \return
    /// True if this geometry spatially crosses the other geometry,
    /// false otherwise.
    ///
    virtual bool Crosses(MgGeometry* other);

    /////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Given 2 geometries a and b, a.Disjoint(b)is true if and only
    /// if the intersection of a and b is empty.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual bool Disjoint(MgGeometry other);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual boolean Disjoint(MgGeometry other);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual bool Disjoint(MgGeometry other);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param other (MgGeometry)
    /// The MgGeometry to test against this one.
    ///
    /// \return
    /// True if this geometry is spatially disjoint from the other
    /// geometry, false otherwise.
    ///
    virtual bool Disjoint(MgGeometry* other);

    /////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Given 2 geometries a and b, a.Equals(b) is true if the 2
    /// geometries are spatially equal
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual bool Equals(MgGeometry other);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual boolean Equals(MgGeometry other);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual bool Equals(MgGeometry other);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param other (MgGeometry)
    /// The MgGeometry to test against this one.
    ///
    /// \return
    /// True if this geometry is spatially equal to the other
    /// geometry, false otherwise.
    ///
    virtual bool Equals(MgGeometry* other);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// This is a convenience method. Given 2 geometries a and b,
    /// a.Intersects(b) is true if and only if a.\link MgGeometry::Disjoint Disjoint \endlink(b)
    /// is false.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual bool Intersects(MgGeometry other);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual boolean Intersects(MgGeometry other);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual bool Intersects(MgGeometry other);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param other (MgGeometry)
    /// The MgGeometry to test against this one.
    ///
    /// \return
    /// True if this geometry is not disjoint with respect to the
    /// other geometry, false otherwise.
    ///
    virtual bool Intersects(MgGeometry* other);

    /////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Given 2 geometries a and b, a.Overlaps(b) is true if and only
    /// if the dimension of the interior of a equals the dimension of
    /// the interior of b equals the dimension of the intersection of
    /// the interior of a and the interior of b and the intersection
    /// of a and b is neither a nor b.
    ///
    /// \remarks
    /// A Point cannot overlap a Point, and a MultiPoint cannot
    /// overlap a Point but a MultiPoint can overlap a MultiPoint.
    /// <h2>Illustration</h2>
    /// c and d are MultiPoint geometries.
    /// \image html overlaps.png
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual bool Overlaps(MgGeometry other);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual boolean Overlaps(MgGeometry other);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual bool Overlaps(MgGeometry other);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param other (MgGeometry)
    /// The MgGeometry to test against this one.
    ///
    /// \return
    /// True if this geometry spatially overlaps the other geometry,
    /// false otherwise.
    ///
    virtual bool Overlaps(MgGeometry* other);

    //////////////////////////////////////////////////////////////////////
    /// \brief
    /// Given 2 geometries a and b, a.Touches(b) is true if and only
    /// if the intersection of the interior of a and the interior of
    /// b is empty and the intersection of a and b is not empty.
    ///
    /// \remarks
    /// A Point cannot touch a Point because a Point has no boundary
    /// and so the intersection of the interiors of the two
    /// geometries is not empty.
    /// \n
    /// A Point can touch a non-closed Line at one of the end points
    /// of the Line.
    /// \n
    /// A Point cannot touch a closed Line because all of the points
    /// in the Line are interior to it.
    /// <h2>Illustration</h2>
    /// e are MultiPoint geometries and f is a LineString.
    /// \image html touches.png
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual bool Touches(MgGeometry other);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual boolean Touches(MgGeometry other);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual bool Touches(MgGeometry other);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param other (MgGeometry)
    /// The MgGeometry to test against this
    /// one.
    ///
    /// \return
    /// True if this geometry spatially touches the other geometry,
    /// false otherwise.
    ///
    virtual bool Touches(MgGeometry* other);

    //////////////////////////////////////////////////////////////////////
    /// \brief
    /// Given 2 geometries a and b, a.Within(b) is true if and only
    /// if the intersection of a and b is a and the intersection of
    /// the interior of a and the interior of b is not empty.
    ///
    /// \remarks
    /// If the entire point-set of a geometry intersects the boundary
    /// of another geometry, the former is not within the latter.
    /// <h2>Illustration</h2>
    /// The end point of d and the end point of e intersect. a, b, i,
    /// j, k, and m are MultiPoints. The concentric circles represent
    /// intersecting points. The polygon n1n2n3n4 is within the
    /// polygon p1p2p3p4 and vice versa. The LineString q1q2 is
    /// within the LineString r1r2 and vice versa. The MultiPoint j
    /// is within the MultiPoint k and vice versa. The Point f is
    /// within the point g and vice versa.
    /// \n
    /// \image html within.png
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual bool Within(MgGeometry other);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual boolean Within(MgGeometry other);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual bool Within(MgGeometry other);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param other (MgGeometry)
    /// The MgGeometry to test against this
    /// one.
    ///
    /// \return
    /// True if this geometry is spatially within the other geometry,
    /// false otherwise.
    ///
    virtual bool Within(MgGeometry* other);

    /////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \remarks
    /// <TABLE border="1" class="RuledTable">
    ///   <tr>
    ///     <th>Class</th>
    ///     <th>Boundary</th>
    ///   </tr>
    ///   <tr>
    ///     <td>MgPoint, MgMultiPoint</td>
    ///     <td>Empty set.</td>
    ///   </tr>
    ///   <tr>
    ///     <td>MgLineString, MgCurveString</td>
    ///     <td>If closed: empty set. If not closed: 2 end points.</td>
    ///   </tr>
    ///   <tr>
    ///     <td>MgMultiLineString, MgMultiCurveString</td>
    ///     <td>Points that are in the boundaries of the element curves after application of the 'Mod 2' union rule (see below).</td>
    ///   </tr>
    ///   <tr>
    ///     <td>MgPolygon, MgCurvePolygon</td>
    ///     <td>Its set of Rings.</td>
    ///   </tr>
    ///   <tr>
    ///     <td>MgMultiPolygon, MgMultiCurvePolygon</td>
    ///     <td>The set of Rings of its polygons.</td>
    ///   </tr>
    ///   <tr>
    ///     <td>MgMultiGeometry</td>
    ///     <td>Points that are in the boundaries of the heterogenous element geometries after application of the 'Mod 2' union rule (see below).</td>
    ///   </tr>
    /// </TABLE>
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgGeometry Boundary();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgGeometry Boundary();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgGeometry Boundary();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// An MgGeometry that represents this geometric entity's
    /// boundary.
    ///
    /// <h2>'Mod 2' Union Rule</h2>
    /// [\link OGC99049 OGC99-049 \endlink] references [\link OGC96015R1 OGC96-015R1 \endlink]
    /// for the definition of this rule. In section 3.12.3.2
    /// [OGC96-015R1] states the following:
    /// \n
    /// "For complex geometries, we take the 'mod 2' union of the
    /// components. That means that a point is in the boundary of a
    /// complex object (represented as disjoint representational
    /// geometries) if it is in an odd number of the boundaries of
    /// its component simple geometries."
    ///
    /// The following illustrations are inspired by the discussion in
    /// section 8.2.5 of [\link JTS14 JTS1.4 \endlink].
    /// <TABLE border="1" class="RuledTable">
    ///   <tr>
    ///     <td>Case</td>
    ///     <td>Boundary</td>
    ///   </tr>
    ///   <tr>
    ///     <td>1</td>
    ///     <td>a, b, c, d</td>
    ///   </tr>
    ///   <tr>
    ///     <td>2</td>
    ///     <td>a, b, c, d</td>
    ///   </tr>
    ///   <tr>
    ///     <td>3</td>
    ///     <td>a, d</td>
    ///   </tr>
    ///   <tr>
    ///     <td>4</td>
    ///     <td>a, b, e, d</td>
    ///   </tr>
    ///   <tr>
    ///     <td>5</td>
    ///     <td>b, c, polygon((d, e, f, g))</td>
    ///   </tr>
    ///   <tr>
    ///     <td>6</td>
    ///     <td>b, (polygon((d, e, f, g)) - c)</td>
    ///   </tr>
    /// </TABLE>
    ///
    /// \image html mod2Rule.png
    ///
    virtual MgGeometry* Boundary();

    ////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns a geometry that represents all points whose distance
    /// from this geometric entity is less than or equal to a given
    /// distance. By default, the distance computation is done using
    /// a linear algorithm unless a custom measure is supplied.
    ///
    /// \remarks
    /// This method returns NULL when using a negative offset with
    /// points, lines, and curves.  When using a negative offset with
    /// polygons, the method returns NULL if the offset causes the
    /// buffered polygon to shrink to zero area.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgGeometry Buffer(double distance, MgMeasure measure);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgGeometry Buffer(double distance, MgMeasure measure);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgGeometry Buffer(double distance, MgMeasure measure);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param distance (double)
    /// The distance from the geometry in units.  This value cannot be zero.
    /// \param measure (MgMeasure)
    /// The MgMeasure instance to use when computing the buffer. If null,
    /// a linear distance algorithm is used.
    ///
    /// \return
    /// An MgGeometry that represents the geometric entity's buffer, or
    /// NULL if the geometry cannot be buffered.
    ///
    virtual MgGeometry* Buffer(double distance, MgMeasure* measure);

    /////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \remarks
    /// In section 12.4.2 [\link JTS14 JTS1.4 \endlink] states:
    /// "The convex hull of a Geometry is the smallest convex Polygon
    /// that contains all the points in the Geometry. If the convex
    /// hull contains fewer than 3 points, a lower dimension Geometry
    /// is returned, specified as follows:
    /// <TABLE border="1" class="RuledTable">
    ///     <tr>
    ///       <th>Points in Convex Hull</th>
    ///       <th>Geometry of Result</th>
    ///     </tr>
    ///     <tr>
    ///       <td>0</td>
    ///       <td>empty GeometryCollection</td>
    ///     </tr>
    ///     <tr>
    ///       <td>1</td>
    ///       <td>Point</td>
    ///     </tr>
    ///     <tr>
    ///       <td>2</td>
    ///       <td>LineString</td>
    ///     </tr>
    ///     <tr>
    ///       <td>3 or more</td>
    ///       <td>Polygon</td>
    ///     </tr>
    /// </TABLE>
    /// JTS will return a Geometry with the minimal number of points
    /// needed to represent the convex hull. In particular, no more
    /// than two consecutive points will be collinear."
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgGeometry ConvexHull();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgGeometry ConvexHull();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgGeometry ConvexHull();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// An MgGeometry that represents the convex hull of the
    /// geometric entity.
    ///
    virtual MgGeometry* ConvexHull();

    //////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns a geometry that represents a point set difference
    /// between this geometric entity and another.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgGeometry Difference(MgGeometry other);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgGeometry Difference(MgGeometry other);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgGeometry Difference(MgGeometry other);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param other (MgGeometry)
    /// The MgGeometry to subtract from this one.
    ///
    /// \return
    /// An MgGeometry representing the difference.
    ///
    /// <h2>Illustration</h2>
    /// if (Geom1.Disjoint(Geom2) == True) then
    /// Geom1.Difference(Geom2) = Geom1.
    /// \n
    /// if (Geom1.Touches(Geom2) == True) then
    /// Geom1.Difference(Geom2) = Geom1.
    /// \n
    /// if (Geom1.Crosses(Geom2) == True) then
    /// \image html diffWhenCrosses.png
    /// \n
    /// if (Geom1.Within(Geom2) == True) then
    /// \image html diffWhenWithin.png
    ///
    /// if (A.Overlaps(B) == True) then
    /// \image html diffWhenOverlaps.png
    ///
    virtual MgGeometry* Difference(MgGeometry* other);

    /////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the shortest distance between this geometry and
    /// another. By default distance computation is done using a
    /// linear algorithm unless a custom measure is supplied.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual double Distance(MgGeometry other, MgMeasure measure);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual double Distance(MgGeometry other, MgMeasure measure);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual double Distance(MgGeometry other, MgMeasure measure);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param other (MgGeometry)
    /// The MgGeometry to determine the distance to.
    /// \param measure (MgMeasure)
    /// The MgMeasure instance to use when computing the
    /// distance. If null, a linear distance algorithm
    /// is used.
    ///
    /// \return
    /// A double value representing the shortest distance between two
    /// geometries.
    ///
    virtual double Distance(MgGeometry* other, MgMeasure* measure);

    //////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns a geometry that represents the point set
    /// intersection of this geometry and another.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgGeometry Intersection(MgGeometry other);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgGeometry Intersection(MgGeometry other);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgGeometry Intersection(MgGeometry other);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param other (MgGeometry)
    /// The MgGeometry to compute the intersection with
    /// this one.
    ///
    /// \return
    /// An MgGeometry that represents the intersection.
    ///
    virtual MgGeometry* Intersection(MgGeometry* other);

    ////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns a geometry that represents the point set \link symmetric_difference symmetric difference \endlink
    /// of this geometry with another.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgGeometry SymetricDifference(MgGeometry other);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgGeometry SymetricDifference(MgGeometry other);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgGeometry SymetricDifference(MgGeometry other);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param other (MgGeometry)
    /// The MgGeometry to subtract from this one.
    ///
    /// \remarks 
    /// As of 3.0 the resulting geometry, if it is an aggregate geometry will have its sub-geometries ordered based on the order  
    /// of your operand geometries. Take note of this if your code is concerned about the order of geometries in the symmetric difference 
    /// result 
    /// 
    /// \return
    /// An MgGeometry representing the symmetric difference.
    ///
    /// <h2>Illustration</h2>
    /// if (Geom1.Disjoint(Geom2) == True) then
    /// <TABLE border="1" class="RuledTable">
    ///   <tr>
    ///     <th>A</th>
    ///     <th>B</th>
    ///     <th>SymDiff</th>
    ///   </tr>
    ///   <tr>
    ///     <td>Point | MultiPoint</td>
    ///     <td>Point | MultiPoint</td>
    ///     <td>MultiPoint</td>
    ///   </tr>
    ///   <tr>
    ///     <td>Point | MultiPoint</td>
    ///     <td>LineString | MultiLineString</td>
    ///     <td>Geometry Collection (MultiLineString Point+, LineString+)</td>
    ///   </tr>
    ///   <tr>
    ///     <td>Point | MultiPoint</td>
    ///     <td>Polygon | MultiPolygon</td>
    ///     <td>Geometry Collection (Point+, Polygon+)</td>
    ///   </tr>
    ///   <tr>
    ///     <td>LineString | MultiLineString</td>
    ///     <td>LineString | MultiLineString</td>
    ///     <td>MultiLineString</td>
    ///   </tr>
    ///   <tr>
    ///     <td>LineString |MultiLineString</td>
    ///     <td>Polygon | MultiPolygon</td>
    ///     <td>Geometry Collection (LineString+, Polygon+)</td>
    ///   </tr>
    ///   <tr>
    ///     <td>Polygon | MultiPolygon</td>
    ///     <td>Polygon | MultiPolygon</td>
    ///     <td>MultiPolygon</td>
    ///   </tr>
    /// </TABLE>
    /// \n
    /// if (Geom1.Touches(Geom2) == True) then
    /// \image html symmetricDiffWhenTouches.png
    /// \n
    /// if (Geom1.Crosses(Geom2) == True) then
    /// \image html symmetricDiffWhenCrosses.png
    /// \n
    /// if (Geom1.Within(Geom2) == True) then
    /// \image html symmetricDiffWhenWithin.png
    /// \n
    /// if (Geom1.Overlaps(Geom2) == True) then
    /// \image html symmetricDiffWhenOverlaps.png
    ///
    virtual MgGeometry* SymetricDifference(MgGeometry* other);

    ///////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns a geometry that represents the point set union of
    /// this geometry with another.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgGeometry Union(MgGeometry other);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgGeometry Union(MgGeometry other);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgGeometry Union(MgGeometry other);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param other (MgGeometry)
    /// The MgGeometry to union with this one.
    ///
    /// \return
    /// An MgGeometry representing the union of the two geometries.
    ///
    virtual MgGeometry* Union(MgGeometry* other);

INTERNAL_API:

    virtual INT32 GetEntityType();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the unique identifier for the class
    ///
    /// \return
    /// Class Identifider.
    ///
    virtual INT32 GetClassId();

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns an iterator over the coordinates included in this
    /// geometric entity.
    ///
    /// \return
    /// The coordinates of this geometric entity
    ///
    virtual MgCoordinateIterator* GetCoordinates() = 0;

protected:

    //////////////////////////////////////////////
    /// \brief
    /// Dispose this object.
    ///
    virtual void Dispose();

CLASS_ID:
    static const INT32 m_cls_id = Geometry_Geometry;
};
/// \}

#endif //_MGGEOMETRY_H_

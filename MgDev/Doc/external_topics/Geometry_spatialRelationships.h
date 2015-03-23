/**
\addtogroup SpatialRelationships

<p>
  The spatial relationship predicates are implemented in this
  API in two ways:
</p>
<ul>
  <li>as methods on the MgGeometry class and</li>
  <li>as an element in a spatial filter passed to either the
    MgFeatureService::SelectFeatures() or the
    MgFeatureService::SelectAggregate() method.</li>
</ul>
<p>
  MgGeometry is the base class for all of the geometry classes
  in this API. The call pattern is &quot;$bool =
  $geometry1-&gt;predicate($geometry2)&quot;. You can use the methods
  on this class to analyze geometries locally.
</p>
<p>
  The MgFeatureService::SelectFeatures() and
  MgFeatureService::SelectAggregate() methods select features
  from a data store somewhere in the network. The underlying
  implementation of this functionality is the Autodesk
  \link FDO FDO\endlink.
  The filter elements are class name, geometry property name, a
  geometry argument, and a spatial relationship predicate name.
  The filter says &quot;Select the geometries stored in &lt;geometry
  property name&gt; in &lt;class name&gt; which are related to the
  &lt;geometry argument&gt; by the &lt;spatial relationship
  predicate&gt;. That is, &quot;&lt;geometry property&gt;.&lt;spatial
  relationship predicate&gt;(&lt;geometry argument&gt;)&quot; is true.
</p>
<h2>
  Egenhofer Spatial Relations
</h2>
<p>
  The Oracle Spatial Users Guide and Reference (\link OSUGR [OSUGR]\endlink) bases its definitions of
  spatial operators on \link Egenhofer91 [Egenhofer91]\endlink. It defines the 
  operators as follows:
</p>
<TABLE class="RuledTable">
  <tr>
    <th><b>Operator</b></th>
    <th><b>Definition</b></th>
  </tr>
  <tr>
    <td>disjoint</td>
    <td>The boundaries and interiors do not intersect.</td>
  </tr>
  <tr>
    <td>touch</td>
    <td>The boundaries intersect but the interiors do not intersect.</td>
  </tr>
  <tr>
    <td>overlapbdydisjoint</td>
    <td>the interior of one object intersects the boundary and exterior of the other object but the two boundaries do not intersect.</td>
  </tr>
  <tr>
    <td>overlapbdyintersect</td>
    <td>The boundaries and interiors of the two objects
      intersect.</td>
  </tr>
  <tr>
    <td>equal</td>
    <td>The two objects have the same boundary and interior.</td>
  </tr>
  <tr>
    <td>contains</td>
    <td>The interior and boundary of one object is completely contained in the interior of the other object.</td>
  </tr>
  <tr>
    <td>covers</td>
    <td>The interior of one objects is completely contained in the interior or boundary of the other object and their boundaries intersect.</td>
  </tr>
  <tr>
    <td>inside</td>
    <td>The opposite of contains. A inside B implies B contains A.</td>
  </tr>
  <tr>
    <td>coveredby</td>
    <td>The opposite of covers. A converedby B implies B covers A.</td>
  </tr>
</TABLE>
<h2>
  Base Definitions
</h2>
<p>
  The definitions of the spatial relationship predicates are
  implicitly or explicitly based on the definitions of the
  interior, boundary, and exterior of a geometry. In section
  2.1.13.1, 
  \link OGC99049 [OGC99-049]\endlink
  states:
</p>
<p>
  &quot;The domain of geometric objects considered is those that are spatial relationships. The
  interior of a geometry consists of those points that are left
  when the boundary points are removed. The exterior of a
  geometry consists of points not in the interior or boundary.&quot;
</p>
<p>
  See the description of the
  \link MgGeometry::Boundary MgGeometry::Boundary Method\endlink
  for a specification of the boundary for each geometry type.
</p>
<h2>
  The Common Predicates
</h2>
<p>
  Section 2.1.13 of 
  \link OGC99049 [OGC99-049]\endlink
  defines 8 spatial
  relationship predicates:
  \link MgGeometry::Contains Contains\endlink,
  \link MgGeometry::Crosses Crosses\endlink,
  \link MgGeometry::Disjoint Disjoint\endlink,
  \link MgGeometry::Equals Equals\endlink,
  \link MgGeometry::Intersects Intersects\endlink,
  \link MgGeometry::Overlaps Overlaps\endlink,
  \link MgGeometry::Touches Touches\endlink,
  and
  \link MgGeometry::Within Within\endlink. Click on
  the predicate names to see the description of the MgGeometry
  method, which includes the definition.
</p>
<p>
  You can use the Disjoint, Equals, and Intersects predicates
  on any pair of geometries in any order and you may get a
  return value of true as well as false.
</p>
<p>
  The following table identifies the cases where the Contains,
  Overlaps, Touches, and Crosses predicates always return false. Blank cells in the table indicate that
  it is possible for the predicate to return a true value.
</p>
<p>
  In the table, <strong>P</strong> stands for Point, <strong>MP</strong> for MultiPoint, <strong>L</strong> for
  1-dimensional geometries (LineString, CurveString,
  MultiLineString and MultiCurveString), and <strong>A</strong> for
  2-dimensional geometries (Polygon, MultiPolygon,
  CurvePolygon, and MultiCurvePolygon).
</p>
<p>
  Asterisks indicate cases where switching the order of the geometries produces a result of false
  for the Within predicate, but may produce a result of true for the Contains predicate.
</p>
<p>
	For example, a point cannot contain a line: P.Contains(L) == false. A line, however, can contain a point. Also, a 
	point may be within a line, but a line cannot be within a point.
</p>
<TABLE class="RuledTable">
  <tr>
    <th><b>&nbsp;</b></th>
    <th><b>P/P</b></th>
    <th><b>MP/MP</b></th>
    <th><b>P/L*</b></th>
    <th><b>MP/L*</b></th>
    <th><b>P/A*</b></th>
    <th><b>MP/A*</b></th>
    <th><b>L/L</b></th>
    <th><b>L/A*</b></th>
    <th><b>A/A</b></th>
  </tr>
  <tr>
    <td class="shaded"><strong>Contains</strong></td>
    <td></td>
    <td></td>
    <td>F</td>
    <td>F</td>
    <td>F</td>
    <td>F</td>
    <td></td>
    <td>F</td>
    <td></td>
  </tr>
  <tr>
    <td class="shaded"><strong>Overlaps</strong></td>
    <td>F</td>
    <td></td>
    <td>F</td>
    <td>F</td>
    <td>F</td>
    <td>F</td>
    <td></td>
    <td>F</td>
    <td></td>
  </tr>
  <tr>
    <td class="shaded"><strong>Touches</strong></td>
    <td>F</td>
    <td>F</td>
    <td></td>
    <td></td>
    <td></td>
    <td></td>
    <td></td>
    <td></td>
    <td></td>
  </tr>
  <tr>
    <td class="shaded"><strong>Within</strong></td>
    <td></td>
    <td></td>
    <td></td>
    <td></td>
    <td></td>
    <td></td>
    <td></td>
    <td></td>
    <td></td>
  </tr>
  <tr>
    <td class="shaded"><strong>Crosses</strong></td>
    <td>F</td>
    <td>F</td>
    <td>F</td>
    <td></td>
    <td>F</td>
    <td></td>
    <td></td>
    <td></td>
    <td>F</td>
  </tr>
</TABLE>
<h2>
  Predicates Specific to MgGeometry
</h2>
<p>
  These are
  \link MgGeometry::Boundary Boundary\endlink,
  \link MgGeometry::Buffer Buffer\endlink,
  \link MgGeometry::ConvexHull ConvexHull\endlink,
  \link MgGeometry::Distance Distance\endlink,
  \link MgGeometry::Intersection Intersection\endlink,
  \link MgGeometry::SymetricDifference Symmetric Difference\endlink,
  and
  \link MgGeometry::Union Union\endlink. Click on the
  predicate to see the method description, which includes a
  definition.
</p>
<p>
<h2>
  Predicates Specific to MgFeatureService Select Methods
</h2>
These predicates are \link CoveredBy Covered By\endlink, \link Inside Inside\endlink,
and \link EnvelopeIntersects Envelope Intersects\endlink. Click on the predicate to see
the definition.
</p>
**/
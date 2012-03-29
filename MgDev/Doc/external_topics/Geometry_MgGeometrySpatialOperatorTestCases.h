/** 
\addtogroup MgGeometrySpatialOperatorTestCases
<p>
  These tests target the MgGeometry methods: Contains,Crosses,
  Disjoint, Equals, Intersects, Overlaps, Touches, and Within.
  The CoveredBy, EnvelopeIntersects, and Inside predicates are
  simulated using other MgGeometry methods. See the PHP code
  sections of 
  \link CoveredBy CoveredBy\endlink, 
  \link EnvelopeIntersects EnvelopeIntersects \endlink
   , 
   and \link Inside Inside \endlink.
  All of the tests should return true.
</p>

<p>
  For those cases using the predicates CoveredBy,
  EnvelopeIntersects, and Inside, convert the AGF text
  specifications to objects of type MgGeometry and call the
  appropriate function passing the geometries in as arguments.
</p>
<pre>
$wktReaderWriter = new MgWktReaderWriter();
$geom1 = $wktReaderWriter-&gt;Read(&quot;POINT(1 1)&quot;);
$geom2 = $wktReaderWriter-&gt;Read(&quot;POINT(1 1)&quot;);
$boolResult = EnvelopeIntersects($geom1, $geom2); 
</pre>
<p>
  For the other cases, convert the AGF text specifications to
  objects of type MgGeometry and call the appropriate method
  with the second MgGeometry object as the argument.
</p>
<code> $boolResult = $geom1-&gt;Contains($geom2); </code>
<h2>
  Contains
</h2>
<tt> POINT(1 1):Contains:POINT(1 1) <br>
LINESTRING(0 1, 2 1):Contains:POINT(1 1) <br>
POLYGON((0 0, 2 0, 2 2, 0 2, 0 0)):Contains:POINT(1 1) <br>
MULTIPOINT(1 1, 2 1):Contains:POINT(1 1) <br>
MULTIPOINT(2 1, 3 1, 5 1):Contains:MULTIPOINT(2 1, 3 1, 5 1) <br>
LINESTRING(2 1, 6 1):Contains:MULTIPOINT(2 1, 3 1, 5 1) <br>
POLYGON((1 0, 6 0, 6 2, 1 2, 1 0)):Contains:MULTIPOINT(2 1, 3 1, 5 1) <br>
POLYGON((2 0, 6 0, 6 2, 2 2, 2 0)):Contains:MULTIPOINT(2 1, 3 1, 5 1) <br>
LINESTRING(4 1, 7 1):Contains:LINESTRING(4 1, 7 1) <br>
LINESTRING(3 1, 8 1):Contains:LINESTRING(4 1, 7 1) <br>
LINESTRING(4 1, 8 1):Contains:LINESTRING(4 1, 7 1) <br>
POLYGON((4 0, 8 0, 8 2, 4 2, 4 0)):Contains:LINESTRING(4 1, 7 1) <br>
POLYGON((3 0, 8 0, 8 2, 3 2, 3 0)):Contains:LINESTRING(4 1, 7 1) <br>
POLYGON((1 1, 4 1, 4 4, 1 4, 1 1)):Contains:POLYGON((1 1, 4 1, 4 4, 1 4, 1 1)) <br>
POLYGON((0 0, 5 0, 5 5, 0 5, 0 0)):Contains:POLYGON((1 1, 4 1, 4 4, 1 4, 1 1)) <br>
POLYGON((1 0, 5 0, 5 5, 1 5, 1 0)):Contains:POLYGON((1 1, 4 1, 4 4, 1 4, 1 1)) </tt>
<h2>
  CoveredBy
</h2>
<tt> LINESTRING(4 1, 7 1):CoveredBy:LINESTRING(4 1, 7 1) <br>
LINESTRING(4 1, 7 1):CoveredBy:LINESTRING(4 1, 8 1) <br>
LINESTRING(4 1, 7 1):CoveredBy:POLYGON((4 0, 8 0, 8 2, 4 2, 4 0)) <br>
POLYGON((1 1, 4 1, 4 4, 1 4, 1 1)):CoveredBy:POLYGON((1 0, 5 0, 5 5, 1 5, 1 0)) <br>
POLYGON((1 1, 4 1, 4 4, 1 4, 1 1)):CoveredBy:POLYGON((1 1, 4 1, 4 4, 1 4, 1 1)) </tt>
<h2>
  Crosses
</h2>
<tt> MULTIPOINT(2 1, 3 1, 5 1):Crosses:LINESTRING(4 1, 7 1) <br>
MULTIPOINT(2 1, 3 1, 5 1):Crosses:POLYGON((1 0, 4 0, 4 2, 1 2, 1 0)) <br>
LINESTRING(4 1, 7 1):Crosses:LINESTRING(5 0, 5 2) <br>
LINESTRING(4 1, 7 1):Crosses:POLYGON((4 0, 6 0, 6 2, 4 2, 4 0)) <br>
LINESTRING(4 1, 7 1):Crosses:POLYGON((5 0, 6 0, 6 2, 5 2, 5 0)) <br>
LINESTRING(4 1, 7 1):Crosses:POLYGON((5 0, 8 0, 8 2, 5 2, 5 0)) </tt>
<h2>
  Disjoint
</h2>
<tt> POINT(1 1):Disjoint:POINT(2 1) <br>
POINT(1 1):Disjoint:MULTIPOINT(2 1, 3 1, 5 1) <br>
POINT(1 1):Disjoint:LINESTRING(4 1, 7 1) <br>
POINT(1 1):Disjoint:POLYGON((2 1, 3 1, 3 2, 2 2, 2 1)) <br>
MULTIPOINT(2 1, 3 1, 5 1):Disjoint:MULTIPOINT(4 0, 7 0) <br>
MULTIPOINT(2 1, 3 1, 5 1):Disjoint:LINESTRING(4 0, 7 0) <br>
MULTIPOINT(2 1, 3 1, 5 1):Disjoint:POLYGON((0 0, 1 0, 1 1, 0 1, 0 0)) <br>
LINESTRING(4 1, 7 1):Disjoint:LINESTRING(4 0, 7 0) <br>
LINESTRING(4 1, 7 1):Disjoint:POLYGON((0 0, 1 0, 1 1, 0 1, 0 0)) <br>
POLYGON((1 1, 4 1, 4 4, 1 4, 1 1)):Disjoint:POLYGON((5 1, 6 1, 6 2, 5 2, 5 1))
  * Equals *
  POINT(1 1):Equals:POINT(1 1) <br>
MULTIPOINT(2 1, 3 1, 5 1):Equals:MULTIPOINT(2 1, 3 1, 5 1) <br>
LINESTRING(4 1, 7 1):Equals:LINESTRING(4 1, 7 1) <br>
POLYGON((1 1, 4 1, 4 4, 1 4, 1 1)):Equals:POLYGON((1 1, 4 1, 4 4, 1 4, 1 1)) </tt>
<h2>
  EnvelopeIntersects
</h2>
<tt> POINT(1 1):EnvelopeIntersects:POINT(1 1) <br>
MULTIPOINT(2 1, 3 1, 5 1):EnvelopeIntersects:MULTIPOINT(2 1, 3 1, 5 1) <br>
LINESTRING(4 1, 7 1):EnvelopeIntersects:LINESTRING(4 1, 7 1) <br>
POLYGON((1 1, 4 1, 4 4, 1 4, 1
  1)):EnvelopeIntersects:POLYGON((1 1, 4 1, 4 4, 1 4, 1 1)) </tt>
<h2>
  Inside
</h2>
<tt> POINT(1 1):Inside:POINT(1 1) <br>
POINT(1 1):Inside:MULTIPOINT(1 1, 2 1) <br>
POINT(1 1):Inside:LINESTRING(0 1, 2 1) <br>
POINT(1 1):Inside:POLYGON((0 0, 2 0, 2 2, 0 2, 0 0)) <br>
MULTIPOINT(2 1, 3 1, 5 1):Inside:MULTIPOINT(2 1, 3 1, 5 1) <br>
MULTIPOINT(2 1, 3 1, 5 1):Inside:LINESTRING(1 1, 6 1) <br>
MULTIPOINT(2 1, 3 1, 5 1):Inside:POLYGON((1 0, 6 0, 6 2, 1 2, 1 0)) <br>
LINESTRING(4 1, 7 1):Inside:LINESTRING(3 1, 8 1) <br>
LINESTRING(4 1, 7 1):Inside:POLYGON((3 0, 8 0, 8 2, 3 2, 3 0)) <br>
POLYGON((1 1, 4 1, 4 4, 1 4, 1 1)):Inside:POLYGON((0 0, 5 0, 5 5, 0 5, 0 0)) </tt>
<h2>
  Intersects
</h2>
<tt> POINT(1 1):Intersects:POINT(1 1) <br>
MULTIPOINT(2 1, 3 1, 5 1):Intersects:MULTIPOINT(2 1, 3 1, 5 1) <br>
LINESTRING(4 1, 7 1):Intersects:LINESTRING(4 1, 7 1) <br>
POLYGON((1 1, 4 1, 4 4, 1 4, 1 1)):Intersects:POLYGON((1 1, 4 1, 4 4, 1 4, 1 1)) </tt>
<h2>
  Overlaps
</h2>
<tt> MULTIPOINT(2 1, 3 1):Overlaps:MULTIPOINT(3 1, 5 1) <br>
LINESTRING(4 1, 7 1):Overlaps:LINESTRING(6 1, 8 1) <br>
POLYGON((1 1, 4 1, 4 4, 1 4, 1 1)):Overlaps:POLYGON((0 2, 5 2, 5 3, 0 3, 0 2)) <br>
POLYGON((1 1, 4 1, 4 4, 1 4, 1 1)):Overlaps:POLYGON((0 2, 3 2, 3 3, 0 3, 0 2)) </tt>
<h2>
  Touches
</h2>
<tt> POINT(1 1):Touches:LINESTRING(1 1, 2 1) <br>
POINT(1 1):Touches:POLYGON((0 0, 1 0, 1 1, 0 1, 0 0)) <br>
MULTIPOINT(2 1, 3 1):Touches:LINESTRING(3 1, 4 1) <br>
MULTIPOINT(1 1, 2 1):Touches:POLYGON((0 0, 1 0, 1 1, 0 1, 0 0)) <br>
LINESTRING(4 1, 7 1):Touches:LINESTRING(5 1, 5 2) <br>
LINESTRING(4 1, 7 1):Touches:LINESTRING(4 1, 4 2) <br>
LINESTRING(1 1, 2 1):Touches:POLYGON((0 0, 1 0, 1 1, 0 1, 0 0)) <br>
POLYGON((1 1, 4 1, 4 4, 1 4, 1 1)):Touches:POLYGON((4 4, 5 5, 4 6, 3 5, 4 4)) <br>
POLYGON((1 1, 4 1, 4 4, 1 4, 1 1)):Touches:POLYGON((4 2, 5 2, 5 3, 4 3, 4 2)) </tt>
<h2>
  Within
</h2>
<tt> POINT(1 1):Within:POINT(1 1) <br>
POINT(1 1):Within:MULTIPOINT(1 1, 2 1) <br>
POINT(1 1):Within:LINESTRING(0 1, 2 1) <br>
POINT(1 1):Within:POLYGON((0 0, 2 0, 2 2, 0 2, 0 0)) <br>
MULTIPOINT(2 1, 3 1, 5 1):Within:MULTIPOINT(2 1, 3 1, 5 1) <br>
MULTIPOINT(2 1, 3 1, 5 1):Within:LINESTRING(1 1, 6 1) <br>
MULTIPOINT(2 1, 3 1, 5 1):Within:LINESTRING(2 1, 6 1) <br>
MULTIPOINT(2 1, 3 1, 5 1):Within:POLYGON((1 0, 6 0, 6 2, 1 2, 1 0)) <br>
MULTIPOINT(2 1, 3 1, 5 1):Within:POLYGON((2 0, 6 0, 6 2, 2 2, 2 0)) <br>
LINESTRING(4 1, 7 1):Within:LINESTRING(4 1, 7 1) <br>
LINESTRING(4 1, 7 1):Within:LINESTRING(3 1, 8 1) <br>
LINESTRING(4 1, 7 1):Within:LINESTRING(4 1, 8 1) <br>
LINESTRING(4 1, 7 1):Within:POLYGON((4 0, 8 0, 8 2, 4 2, 4 0)) <br>
LINESTRING(4 1, 7 1):Within:POLYGON((3 0, 8 0, 8 2, 3 2, 3 0)) <br>
POLYGON((1 1, 4 1, 4 4, 1 4, 1 1)):Within:POLYGON((1 1, 4 1, 4 4, 1 4, 1 1)) <br>
POLYGON((1 1, 4 1, 4 4, 1 4, 1 1)):Within:POLYGON((0 0, 5 0, 5 5, 0 5, 0 0)) <br>
POLYGON((1 1, 4 1, 4 4, 1 4, 1 1)):Within:POLYGON((1 0, 5 0, 5 5, 1 5, 1 0)) </tt> 

**/
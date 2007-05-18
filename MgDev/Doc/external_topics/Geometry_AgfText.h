/**
\addtogroup AgfText



<h2>Summary</h2>
<p>
  A geometry can be specified in a string using a grammar
  called AGF Text, which is a superset of the Open GIS
  Consortium's (\link OGC OGC \endlink) Well-Known-Text (\link WKT \endlink) grammar. The OGC WKT
  grammar is defined in the OpenGIS Project Document 99-049,
  &quot;OpenGIS Simple Features Specification For SQL&quot;, Revision
  1.1.
</p>
<p> AGF Text is a superset of WKT. AGF Text adds definitions for
  CurveString, MultiCurveString, CurvePolygon, and
  MultiCurvePolygon and dimensionality attribute for each
  definition.</p>
<h2> Notes regarding the AGF Text Grammar</h2>
<ul>
  <li>
    <p>Productions in this grammar do not contain tabs or newlines. </p>
  </li>
  <li>
    <p> If the &lt;Dimensionality&gt; is XY, it may be omitted. </p>
  </li>
  <li>
    <p> A DOUBLE is a double precision floating point value. In the
      input an integer may be substituted for a DOUBLE, but the
      lexical analyzer will convert the integer to a DOUBLE. </p>
  </li>
  <li>
    <p> A &lt;LineString&gt; is closed if the first and last &lt;Point&gt;s
      in its &lt;PointCollection&gt; are identical. </p>
  </li>
  <li>
    <p> The first &lt;LineString&gt; in the &lt;LineStringCollection&gt; of a
      &lt;Polygon&gt; is the exterior boundary of the polygon, and
      subsequent &lt;LineString&gt;s in the collection are inside of
      it. All of the &lt;LineString&gt;s in the collection are closed. </p>
  </li>
  <li>
    <p> The initial &lt;Point&gt; of a &lt;CurveSegment&gt; is the initial
      &lt;Point&gt; of a &lt;CurveString&gt; or the last &lt;Point&gt; of a
      &lt;CurveSegment&gt; preceding it in a &lt;CurveSegmentCollection&gt;. </p>
  </li>
  <li>
    <p>
       A &lt;CurveString&gt; is closed if the initial &lt;Point&gt; is
      identical to the last &lt;Point&gt; in the last &lt;CurveSegment&gt;
      of the &lt;CurveSegmentCollection&gt;. 
    </p>
  </li>
  <li>
    <p>
       The first &lt;CurveString&gt; in the &lt;CurveStringCollection&gt; of
      a &lt;CurvePolygon&gt; is the exterior boundary of the polygon,
      and subsequent &lt;CurveString&gt;s in the collection are inside
      of it. All of the &lt;CurveString&gt;s in the collection are
      closed. 
    </p>
  </li>
</ul>
<h2>AGF Text Grammar</h2>
<p><tt> &lt;AGF Text&gt; ::= POINT &lt;Dimensionality&gt; &lt;PointEntity&gt; <br>
  | LINESTRING &lt;Dimensionality&gt; &lt;LineString&gt; <br>
  | POLYGON &lt;Dimensionality&gt; &lt;Polygon&gt; <br>
  | CURVESTRING &lt;Dimensionality&gt; &lt;CurveString&gt; <br>
  | CURVEPOLYGON &lt;Dimensionality&gt; &lt;CurvePolygon&gt; <br>
  | MULTIPOINT &lt;Dimensionality&gt; &lt;MultiPoint&gt; <br>
  | MULTILINESTRING &lt;Dimensionality&gt; &lt;MultiLineString&gt; <br>
  | MULTIPOLYGON &lt;Dimensionality&gt; &lt;MultiPolygon&gt; <br>
  | MULTICURVESTRING &lt;Dimensionality&gt; &lt;MultiCurveString&gt; <br>
  | MULTICURVEPOLYGON &lt;Dimensionality&gt; &lt;MultiCurvePolygon&gt; <br>
  | GEOMETRYCOLLECTION &lt;GeometryCollection&gt; <br>
  <br>
  &lt;PointEntity&gt; ::= '(' &lt;Point&gt; ')' <br>
  <br>
  &lt;LineString&gt; ::= '(' &lt;PointCollection&gt; ')' <br>
  <br>
  &lt;Polygon&gt; ::= '(' &lt;LineStringCollection&gt; ')' <br>
  <br>
  &lt;MultiPoint&gt; ::= '(' &lt;PointCollection&gt; ')' <br>
  <br>
  &lt;MultiLineString&gt; ::= '(' &lt;LineStringCollection&gt; ')' <br>
  <br>
  &lt;MultiPolygon&gt; ::= '(' &lt;PolygonCollection&gt; ')' <br>
  <br>
  &lt;GeometryCollection : '(' &lt;AGF Collection Text&gt; ')' <br>
  <br>
  &lt;CurveString&gt; ::= '(' &lt;StartPoint&gt; '('<br>
  <br>
  &lt;CurveSegmentCollection&gt; ')' ')' <br>
  <br>
  &lt;CurvePolygon&gt; ::= '(' &lt;CurveStringCollection&gt; ')' <br>
  <br>
  &lt;MultiCurveString&gt; ::= '(' &lt;CurveStringCollection&gt; ')' <br>
  <br>
  &lt;MultiCurvePolygon&gt; ::= '(' &lt;CurvePolygonCollection&gt; ')' <br>
  <br>
  &lt;Dimensionality&gt; ::= // default to XY <br>
  | XY <br>
  | XYZ <br>
  | XYM <br>
  | XYZM <br>
  <br>
  &lt;Point&gt; ::= DOUBLE DOUBLE <br>
  | DOUBLE DOUBLE DOUBLE <br>
  | DOUBLE DOUBLE DOUBLE DOUBLE <br>
  <br>
  &lt;StartPoint&gt; ::= &lt;Point&gt; <br>
  <br>
  &lt;2ndPoint&gt; ::= &lt;Point&gt; <br>
  <br>
  &lt;EndAndStartPointNextSeg&gt; ::= &lt;Point&gt; <br>
  <br>
  &lt;PointCollection&gt; ::= &lt;Point&gt; <br>
  | &lt;PointCollection ',' &lt;Point&gt; <br>
  <br>
  &lt;LineStringCollection&gt; ::= &lt;LineString&gt; <br>
  | &lt;LineStringCollection&gt; ',' &lt;LineString&gt; <br>
  <br>
  &lt;PolygonCollection&gt; ::= &lt;Polygon&gt; <br>
  | &lt;PolygonCollection&gt; ',' &lt;Polygon&gt; <br>
  <br>
  &lt;AGF Collection Text&gt; ::= &lt;AGF Text&gt; <br>
  | &lt;AGF Collection Text&gt; ',' &lt;AGF Text&gt; <br>
  <br>
  &lt;CurveSegment&gt; ::= CIRCULARARCSEGMENT '(' &lt;2ndPoint&gt; ','<br>
  <br>
  &lt;EndAndStartPointNextSeg&gt; ')' <br>
  | LINESTRINGSEGMENT '(' &lt;LineSegmentPointCollection&gt; ')' <br>
  <br>
  &lt;LineSegmentPointCollection&gt; ::= &lt;EndAndStartPointNextSeg&gt; <br>
  | &lt;2ndPoint&gt;, &lt;EndAndStartPointNextSeg&gt; <br>
  | &lt;2ndPoint&gt;, &lt;PointCollection&gt;,<br>
  <br>
  &lt;EndAndStartPointNextSeg&gt; <br>
  <br>
  &lt;CurveSegmentCollection&gt; ::= &lt;CurveSegment&gt; <br>
  | &lt;CurveSegmentCollection&gt; ',' &lt;CurveSegment&gt; <br>
  <br>
  &lt;CurveStringCollection&gt; ::= &lt;CurveString&gt; <br>
  | &lt;CurveStringCollection&gt; ',' &lt;CurveString&gt; <br>
  <br>
  &lt;CurvePolygonCollection&gt; ::= &lt;CurvePolygon&gt; <br>
  | &lt;CurvePolygonCollection&gt; ',' &lt;CurvePolygon&gt; </tt></p>
<h2> Examples</h2>
<p>Each example is shown in two formats. The first format
  conforms to the grammar, and the second format contains tabs
  and newlines to make the structure of the production more
  comprehensible.</p>
<p class="row_1">POINT XY (0 0)</p>
<pre class="row_2">
POINT XY (
   0 0
)
</pre>
<p class="row_1">MULTIPOINT XY (0 0 , 1 1)</p>
<pre class="row_2">
MULTIPOINT XY (
   0 0 , 1 1
)
</pre>
<p class="row_1">LINESTRING XY (0 0 , 1 -1)</p>
<pre class="row_2">
LINESTRING XY(
   0 0 , 1 -1
)
</pre>
<p class="row_1">MULTILINESTRING XY ((3 -2, 4 -2), (5 -2, 6 -2))</p>
<pre class="row_2">
MULTILINESTRING XY (
   ( 3 -2, 4 -2 ),
   ( 5 -2, 6 -2 )
)
</pre>
<p class="row_1">POLYGON XY ((1 -3, 4 -3, 4 -6, 1 -6, 1 -3), (2 -4, 3 -4, 3
  -5, 2 -5, 2 -4))</p>
<pre class="row_2">
POLYGON XY (
   (1 -3, 4 -3, 4 -6, 1 -6, 1 -3),
   (2 -4, 3 -4, 3 -5, 2 -5, 2 -4)
)
</pre>
<p class="row_1">MULTIPOLYGON XY (((5 -3, 8 -3, 8 -6, 5 -6, 5 -3), (6 -4, 7
  -4, 7 -5, 6 -5, 6 -4)), ((9 -3, 12 -3, 12 -6, 9 -6, 9 -3),
  (10 -4, 11 -4, 11 -5, 10 -5, 10 -4)))</p>
<pre class="row_2">
MULTIPOLYGON XY (
   (
      (5 -3, 8 -3, 8 -6, 5 -6, 5 -3),
      (6 -4, 7 -4, 7 -5, 6 -5, 6 -4)
   ),
   (
      (9 -3, 12 -3, 12 -6, 9 -6, 9 -3),
      (10 -4, 11 -4, 11 -5, 10 -5, 10 -4)
   )
)
</pre>
<p class="row_1">CURVESTRING XY (0 -9 (CIRCULARARCSEGMENT(2 -7, 4 -9),
  LINESTRINGSEGMENT(2 5, 6 9, 4 2)))</p>
<pre class="row_2">
CURVESTRING XY (
   0 -9 (
      CIRCULARARCSEGMENT(2 -7, 4 -9),
      LINESTRINGSEGMENT(2 5, 6 9, 4 2)
   )
)
</pre>
<p class="row_1">MULTICURVESTRING XY (( 5 -9 ( LINESTRINGSEGMENT(7 -7, 9 -9),
  LINESTRINGSEGMENT(11 -7, 13 -9))), ( 10 -9 (
  CIRCULARARCSEGMENT(12 -7, 14 -9) CIRCULARARCSEGMENT(16 -4, 17
  -8))))</p>
<pre class="row_2">
MULTICURVESTRING XY (
   (
      5 -9 (
         LINESTRINGSEGMENT(7 -7, 9 -9),
         LINESTRINGSEGMENT(11 -7, 13 -9)
      )
   ),
   (
      10 -9 (
         CIRCULARARCSEGMENT(12 -7, 14 -9)
         CIRCULARARCSEGMENT(16 -4, 17 -8)
      )
   )
)
</pre>
<p class="row_1">CURVEPOLYGON XY (( 0 -13 (CIRCULARARCSEGMENT( 2 -11, 4 -13),
  CIRCULARARCSEGMENT( 2 -15, 0 -13))), ( 1 -13 (
  CIRCULARARCSEGMENT( 2 -12, 3 -13), CIRCULARARCSEGMENT( 2 -14,
  1 -13))))</p>
<pre class="row_2">
CURVEPOLYGON XY (
   (
      0 -13 (
         CIRCULARARCSEGMENT( 2 -11, 4 -13),
         CIRCULARARCSEGMENT( 2 -15, 0 -13)
      )
   ),
   (
      1 -13 (
         CIRCULARARCSEGMENT( 2 -12, 3 -13),
         CIRCULARARCSEGMENT( 2 -14, 1 -13)
      )
   )
)
</pre>
<p class="row_1">MULTICURVEPOLYGON XY ((( 5 -13 ( CIRCULARARCSEGMENT( 7 -11, 9
  -13), CIRCULARARCSEGMENT( 7 -15, 5 -13))), (6 -13 (
  CIRCULARARCSEGMENT( 7 -12, 8 -13), CIRCULARARCSEGMENT( 7 -14,
  6 -13)))), (( 10 -13 ( CIRCULARARCSEGMENT( 12 -11, 14 -13),
  CIRCULARARCSEGMENT( 12 -15, 10 -13))), (11 -13 (
  CIRCULARARCSEGMENT( 12 -12, 13 -13), CIRCULARARCSEGMENT( 12
  -14, 11 -13)))))</p>
<pre class="row_2">
MULTICURVEPOLYGON XY (
   (
      (
         5 -13 (
            CIRCULARARCSEGMENT( 7 -11, 9 -13),
            CIRCULARARCSEGMENT( 7 -15, 5 -13)
         )
      ),
      (
         6 -13 (
            CIRCULARARCSEGMENT( 7 -12, 8 -13),
            CIRCULARARCSEGMENT( 7 -14, 6 -13)
         )
      )
   ),
   (
      (
         10 -13 (
            CIRCULARARCSEGMENT( 12 -11, 14 -13),
            CIRCULARARCSEGMENT( 12 -15, 10 -13)
         )
      ),
      (
         11 -13 (
            CIRCULARARCSEGMENT( 12 -12, 13 -13),
            CIRCULARARCSEGMENT( 12 -14, 11 -13)
         )
      )
   )
)
</pre>
**/
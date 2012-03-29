/** 
\addtogroup Crosses

<h2>
  OGC Definition
</h2>
\link OGC99049 [OGC99-049] \endlink defines crosses as:
<p>
  A.Crosses(B) iff ( dim( I(A) intersect I(B) ) &lt; max( dim(
  I(A) ), dim( I(B) ))) and ( A intersect B is not A) and ( A
  intersect B is not B) where dim(...) is dimension of ... and
  I(A) is interior of A.
</p>
<p>
  Expressed in terms of the 
  \link DE9IM DE-9IM \endlink
  :
</p>
<p class="indent_1">
  Case A is a Point and B is a LineString:
<br>
  Case A is a Point and B is a Polygon:
<br>
  Case A is a LineString and B is a Polygon:
</p>
<p class="indent_2">
  A.Crosses(B) iff ( I(A) intersect I(B) is not empty) and (
  I(A) intersect E(B) is not empty) iff A.Relate( B,
  &quot;T*T******&quot;) 
  <br>
  where E(B) is exterior of B and &amp;	T*T******&quot; is
</p>
<p class="indent_3">
  dim( I(A) intersect I(B) ) belongs to { 0, 1, 2 }
<br>
  dim( I(A) intersects B(B) ) does not matter
<br>
  dim( I(A) intersects E(B) ) belongs to { 0, 1, 2 }
<br>
  dim( B(A) intersects I(B) ) does not matter
<br>
  dim( B(A) intersects B(B) ) does not matter
<br>
  dim( B(A) intersects E(B) ) does not matter
<br>
  dim( E(A) intersects I(B) ) does not matter
<br>
  dim( E(A) intersects B(B) ) does not matter
<br>
  dim( E(A) intersects E(B) ) does not matter
</p>
<p class="indent_1">
  Case A is a LineString and B is a LineString:
</p>
<p class="indent_2">
  A.Crosses(B) iff dim( I(A) intersect I(B) is zero iff
  A.Relate(B, &quot;0********&quot;)
</p>
<p class="indent_3">
  dim( I(A) intersect I(B) ) is 0
<br>
  dim( I(A) intersects B(B) ) does not matter
<br>
  dim( I(A) intersects E(B) ) does not matter
<br>
  dim( B(A) intersects I(B) ) does not matter
<br>
  dim( B(A) intersects B(B) ) does not matter
<br>
  dim( B(A) intersects E(B) ) does not matter
<br>
  dim( E(A) intersects I(B) ) does not matter
<br>
  dim( E(A) intersects B(B) ) does not matter
<br>
  dim( E(A) intersects E(B) ) does not matter
</p>
<h2>
  Oracle Spatial Definition
</h2>
<p>
  There is none. The FDO Oracle Provider maps this predicate to
  the overlapbdydisjiont operator. Oracle defines this operator
  as follows. The interior of one object intersects the
  boundary and interior of the other object, but the two
  boundaries do not intersect.
</p>
<p>
  crosses = overlapbdydisjoint as used in the third argument to
  the sdo_relate spatial operator, for example,
  sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2002, NULL, NULL,
  MDSYS.SDO_ELEM_INFO_ARRAY(1,2,1),
  MDSYS.SDO_ORDINATE_ARRAY(4,1, 7,1)),
  'mask=overlapbdydisjoint').
</p>
<h2>
  Illustration
</h2>
\image html crosses.png
**/
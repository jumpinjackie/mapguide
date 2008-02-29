/**
\addtogroup Overlaps

<h2>
  OGC Definition
</h2>
<p>
  \link OGC99049 [OGC99-049] \endlink
  defines overlaps as:
</p>
<p class="indent_1">
  A.Overlaps(B) iff ( dim( I(A) ) = dim( I(B) ) = dim( I(A)
  intersect I(B) ) ) and (A intersect B is not A) and (A
  intersect B is not B) where dim(...) is dimension of ... and
  I(A) is interior of A
</p>
<p>
  Expressed in terms of the
  \link DE9IM DE-9IM \endlink
  :
</p>
<p class="indent_1">
  Case A is a Point and B is a Point: <br>
  Case A is Polygon and B is a Polygon:
</p>
<p class="indent_2">
  A.Overlaps(B) iff ( I(A) intersect I(B) is not empty) and (
  I(A) intersect E(B) is not empty) and ( E(A) intersect I(B)
  is not empty) iff A.Relate(B, &quot;T*T***T**&quot;) 
  where E(B) is exterior of B and &quot;T*T***T**&quot; means
</p>
<p class="indent_3">
  dim( I(A) intersect I(B) ) belongs to { 0, 1, 2 } <br>
  dim ( I(A) intersect B(B) ) does not matter <br>
  dim ( I(A) intersect E(B) ) belongs to { 0, 1, 2 } <br>
  dim ( B(A) intersect I(B) ) does not matter <br>
  dim ( B(A) intersect B(B) ) does not matter <br>
  dim ( B(A) intersect E(B) ) does not matter <br>
  dim ( E(A) intersect I(B) ) belongs to { 0, 1, 2 } <br>
  dim ( E(A) intersect B(B) ) does not matter <br>
  dim ( E(A) intersect E(B) ) does not matter <br>
</p>
<p class="indent_1">
  Case A is a LineString and B is a LineString:
</p>
<p class="indent_2">
  A.Overlaps(B) iff ( I(A) intersect I(B) is 1) and ( I(A)
  intersect E(B) is not empty) and ( E(A) intersect I(B) is not
  empty) iff A.Relate(B, &quot;1*T***T**&quot;) 
  where &quot;1*T***T**&quot; means
</p>
<p class="indent_3">
  dim( I(A) intersect I(B) ) is 1 <br>
  dim ( I(A) intersect B(B) ) does not matter <br>
  dim ( I(A) intersect E(B) ) belongs to { 0, 1, 2 } <br>
  dim ( B(A) intersect I(B) ) does not matter <br>
  dim ( B(A) intersect B(B) ) does not matter <br>
  dim ( B(A) intersect E(B) ) does not matter <br>
  dim ( E(A) intersect I(B) ) belongs to { 0, 1, 2 } <br>
  dim ( E(A) intersect B(B) ) does not matter <br>
  dim ( E(A) intersect E(B) ) does not matter
</p>
<h2>
  Oracle Spatial Definition
</h2>
<p>
There is none.
</p>
<p>
  The FDO Oracle Provider maps the OGC overlaps operator to the
  Oracle spatial operator overlapbdyintersect.
</p>
<p>
  \link OSUGR [OSUGR] \endlink
  defines overlapbdyintersect as follows.
  &quot;The boundaries and interiors of the two objects intersect.&quot;
</p>
<p>
  overlaps = overlapbdyintersect as used in the third argument
  to the sdo_relate spatial operator, for example,
  sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2005, NULL, NULL,
  MDSYS.SDO_ELEM_INFO_ARRAY(1,1,2),
  MDSYS.SDO_ORDINATE_ARRAY(3,1, 5,1)),
  'mask=overlapbdyintersect').
</p>
<h2>
  Illustration
</h2>
\image html overlaps.png

**/

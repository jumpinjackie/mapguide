/**
\addtogroup Touches

<h2>
  OGC Definition
</h2>
<p>
  \link OGC99049 [OGC99-049] \endlink
  defines Touches as follows:
</p>
<p class="indent_1">
  A.Touches(B) iff ( I(A) intersect I(B) is empty) and ( A
  intersect B is not empty) where I(A) means interior of A.
</p>
<p>
  Expressed in terms of the
  \link DE9IM DE-9IM \endlink
  :
</p>
<p class="indent_1">
  A.Touches(B) iff ( I(A) intersect I(B) is empty) and ( ( B(A)
  intersect I(B) is not empty) or ( I(A) intersect B(B) is not
  empty) or ( B(A) intersect B(B) is not empty) ) iff
  A.Relate(B, &quot;FT*******&quot;) or A.Relate(B, &quot;F**T*****&quot;) or
  A.Relate(B, &quot;F***T****&quot;) 
  <br>where B(A) means boundary of A and 
  &quot;FT*******&quot; means
</p>
<p class="indent_2">
  dim( I(A) intersect I(B) ) is empty <br>
  dim ( I(A) intersect B(B) ) belongs to { 0, 1, 2 } <br>
  dim ( I(A) intersect E(B) ) does not matter <br>
  dim ( B(A) intersect I(B) ) does not matter <br>
  dim ( B(A) intersect B(B) ) does not matter <br>
  dim ( B(A) intersect E(B) ) does not matter <br>
  dim ( E(A) intersect I(B) ) does not matter <br>
  dim ( E(A) intersect B(B) ) does not matter <br>
  dim ( E(A) intersect E(B) ) does not matter <br>
</p>
<p class="indent_2">
  Examples of the foregoing are a point or multipoint object
  intersecting the boundary of a polygon or line or the
  interior of a line intersecting the boundary of a line.
</p>
<p class="indent_1">
  &quot;F**T*****&quot; means
</p>
<p class="indent_2">
  dim( I(A) intersect I(B) ) is empty <br>
  dim ( I(A) intersect B(B) ) does not matter <br>
  dim ( I(A) intersect E(B) ) does not matter <br>
  dim ( B(A) intersect I(B) ) belongs to { 0, 1, 2 } <br>
  dim ( B(A) intersect B(B) ) does not matter <br>
  dim ( B(A) intersect E(B) ) does not matter <br>
  dim ( E(A) intersect I(B) ) does not matter <br>
  dim ( E(A) intersect B(B) ) does not matter <br>
  dim ( E(A) intersect E(B) ) does not matter
</p>
<p class="indent_2">
  Examples of the foregoing are the boundary of a line or a
  polygon intersecting the boundary of a line or a polygon.
</p>
<p class="indent_1">
  &quot;F***T****&quot; means
</p>
<p class="indent_2">
  dim( I(A) intersect I(B) ) is empty <br>
  dim ( I(A) intersect B(B) ) does not matter <br>
  dim ( I(A) intersect E(B) ) does not matter <br>
  dim ( B(A) intersect I(B) ) does not matter <br>
  dim ( B(A) intersect B(B) ) belongs to { 0, 1, 2 } <br>
  dim ( B(A) intersect E(B) ) does not matter <br>
  dim ( E(A) intersect I(B) ) does not matter <br>
  dim ( E(A) intersect B(B) ) does not matter <br>
  dim ( E(A) intersect E(B) ) does not matter <br>
</p>
<p class="indent_2">
  Examples of the foregoing are the boundary of a line or
  polygon intersecting a point or multipoint object or the
  boundary of a line intersecting the interior of a line.
</p>
<h2>
  Oracle Spatial Definition
</h2>
<p>
  \link OSUGR [OSUGR] \endlink
  defines Touch as follows. &quot;The
  boundaries intersect but the interiors do not intersect.&quot; The
  FDO Oracle Provider implements Touches using the Oracle touch
  spatial operator.
</p>
<p>
  touches = touch as used in the third argument to the
  sdo_relate spatial operator, for example,
  <br>
  <tt>
  sdo_relate(a.GEOEMTRY, MDSYS.SDO_GEOMETRY(2003, NULL, NULL,
  MDSYS.SDO_ELEM_INFO_ARRAY(1,1003,1),
  MDSYS.SDO_ORDINATE_ARRAY(4,2, 5,2, 5,3, 4,3, 4,2)),
  'mask=touch')
  </tt>.
</p>
<h2>
  Illustration
</h2>
\image html touches.png
**/
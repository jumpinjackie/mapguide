/**
\addtogroup Within

<h2>
  OGC Definition
</h2>
<p>
  \link OGC99049 [OGC99-049] \endlink
  defines within as:
</p>
<p class="indent_2">
  A.Within(B) iff ( A intersect B = A) and ( I(A) intersect
  I(B) is not empty)
</p>
<p class="indent_1">
  where I(A) means the
  interior
  of A.
</p>
<p>
  In terms of the
  \link DE9IM DE-9IM \endlink
  \link OGC99049 [OGC99-049] \endlink
  defines within as:
</p>
<p class="indent_2">
  A.Within(B) iff 
  ( I(A) intersect I(B) is not empty) and ( I(A) intersect E(B)
  is empty) and ( B(A) intersect E(B) is empty) iff 
  A.Relate(B, 'T*F**F***')
</p>
<p class="indent_1">
  where E(B) means the exterior of B and the B(A) means the
  boundary of A and
  'T*F**F***' means
</p>
<p class="indent_2">
  dim( I(A) intersect I(B) ) belongs to { 0, 1, 2 } <br>
  dim ( I(A) intersect B(B) ) does not matter <br>
  dim ( I(A) intersect E(B) ) is empty <br>
  dim ( B(A) intersect I(B) ) does not matter <br>
  dim ( B(A) intersect B(B) ) does not matter <br>
  dim ( B(A) intersect E(B) ) is empty <br>
  dim ( E(A) intersect I(B) ) does not matter <br>
  dim ( E(A) intersect B(B) ) does not matter <br>
  dim ( E(A) intersect E(B) ) does not matter <br>
  where dim(...) means the dimension of ...
</p>
<h2>
  Oracle Spatial Definition
</h2>
<p>
  There is none. The FDO Oracle Provider implements the Within
  operator using the OR'd combination of the inside and
  coveredby spatial operators.
</p>
<p>
  within =
  \link Inside Inside \endlink
  +
  \link CoveredBy CoveredBy \endlink
  as
  used in the third argument to the sdo_relate spatial
  operator, for example, <br><tt>sdo_relate(a.geometry,
  mdsys.sdo_geometry(2001, NULL,
  mdsys.sdo_point_type(1,1,NULL), NULL, NULL),
  'mask=inside+coveredby')</tt>.
</p>
<h2>
  Illustration
</h2>
\image html within.png

**/
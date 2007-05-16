/**
\addtogroup Disjoint
<h2>
  OGC Definition
</h2>
<p>
  \link OGC99049 [OGC99-049] \endlink defines disjoint as:
</p>
<p>
  A.Disjoint(B) iff A intersect B is empty.
</p>
<p>
  Expressed in terms of the \link DE9IM DE-9IM \endlink:
</p>
<p>
  A.Disjoint(B) iff ( I(A) intersect I(B) is empty ) and ( I(A)
  intersect B(B) is empty ) and ( B(A) intersect I(B) is empty
  ) and ( B(A) intersect B(B) is empty ) iff A.Relate(B,
  &quot;FF*FF****&quot;) where I(A) means interior of A, B(A) means
  boundary of A and &quot;FAA*FF****&quot; means:
</p>
<p>
dim( I(A) intersect I(B) ) is empty <br>
dim( I(A) intersects B(B) ) is empty <br>
dim( I(A) intersects E(B) ) does not matter <br>
dim( B(A) intersects I(B) ) is empty <br>
dim( B(A) intersects B(B) ) is empty <br>
dim( B(A) intersects E(B) ) does not matter <br>
dim( E(A) intersects I(B) ) does not matter <br>
dim( E(A) intersects B(B) ) does not matter <br>
dim( E(A) intersects E(B) ) does not matter
</p>
<h2>
  Oracle Spatial Definition
</h2>
<p>
  \link OSUGR [OSUGR] \endlink defines disjoint as follows. &quot;The
  boundaries and interiors do not intersect.&quot; The FDO Oracle
  Provider implements this predicate using the disjoint
  operator.
</p>
<p>
  disjiont = disjiont as used in the third argument to the
  sdo_relate spatial operator, for example,
  sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2005, NULL, NULL,
  MDSYS.SDO_ELEM_INFO_ARRAY(1,1,2),
  MDSYS.SDO_ORDINATE_ARRAY(4,0, 7,0)), 'mask=disjoint').
</p>
**/
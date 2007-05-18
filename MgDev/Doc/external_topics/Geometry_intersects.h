/**
\addtogroup Intersects

<h2>
  OGC Definition
</h2>
<p>
  \link OGC99049 [OGC99-049] \endlink
  defines Intersects as follows:
</p>
<p>
  A.Intersects(B) iff !A.Disjoint(B).
</p>
<h2>
  Oracle Spatial Definition
</h2>
<p>
  The equivalent Oracle spatial operator is ANYINTERACT, which
  is defined as follows. &quot;The objects are non-disjoint.&quot; The
  FDO Oracle Provider implements the intersects operator using
  the spatial operator ANYINTERACT.
</p>
<p>
  intersects = anyinteract as used in the third argument to the
  sdo_relate spatial operator, for example,<br><tt>
  sdo_relate(a.TUXUNIVERSALGEOM, MDSYS.SDO_GEOMETRY(2002, NULL,
  NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,2,1),
  MDSYS.SDO_ORDINATE_ARRAY(4,1, 7,1)), 'mask=anyinteract')</tt>
</p>

**/

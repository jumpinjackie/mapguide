/**
\addtogroup Equals
<h2>
  OGC Definition
</h2>
\link OGC99049 [OGC99-049] \endlink simply defines the Equals
method as follows. &quot;Equals(anotherGeometry:Geometry):Integer
 Returns 1 (TRUE) if this Geometry is 'spatially equal' to
anotherGeometry.&quot;
<h2>
  Oracle Spatial Definition
</h2>
<p>
  \link OSUGR [OSUGR] \endlink defines the EQUAL spatial operator as
  follows. &quot;The two objects have the same boundary and
  interior.&quot; The FDO Oracle Provider implements equals using
  the equal spatial operator.
</p>
<p>
  equals = equal as used in the third argument to the
  sdo_relate spatial operator, for example, <tt>sdo_relate(a.geometry, mdsys.sdo_geometry(2001, NULL,
  mdsys.sdo_point_type(1,1,NULL), NULL, NULL), 'mask=equal').</tt>
</p>
**/
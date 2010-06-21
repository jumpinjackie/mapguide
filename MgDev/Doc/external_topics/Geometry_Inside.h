/**
\addtogroup Inside
<h2>
  OGC Definition
</h2>
<p>There is none.</p>
<h2>
  Oracle Spatial Definition
</h2>
<p>
\link OSUGR [OSUGR] \endlink defines Inside as follows. &quot;The
opposite of CONTAINS. A INSIDE B implies B CONTAINS A. See \link Contains Contains \endlink.
The FDO Oracle Provider implements Inside using the inside
spatial operator.
</p>
<p>
  inside = inside as used in the third argument to the
  sdo_relate spatial operator, for example,<br>
  <tt>
  sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2002, NULL, NULL,
  MDSYS.SDO_ELEM_INFO_ARRAY(1,2,1),
  MDSYS.SDO_ORDINATE_ARRAY(1,1, 6,1)), 'mask=inside')
  </tt>
  </p>
<h2>
  PHP Code Implementation
</h2>
The equivalent of the Inside predicate can be expressed using
MgGeometry methods as follows.
<pre>
&lt;?php
function Inside ($geom1, $geom2) {
   $withinBool = $geom1-&gt;Within($geom2);
   if (!$withinBool) return $withinBool;
   $boundary1 = $geom1-&gt;Boundary();
   $boundary2 = $geom2-&gt;Boundary();
   $intersectBool = $boundary1-&gt;Intersects($boundary2);
   return !$intersectBool;
}
?&gt;
</pre>
<h2>
  Illustration
</h2>
<p>The concentric circles represent intersecting points. a, e,
g, h, and j are MultiPoint geometries.
</p>
\image html inside.png

**/ 
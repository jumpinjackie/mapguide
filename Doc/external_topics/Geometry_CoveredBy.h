/** 
\addtogroup CoveredBy

<h2>
  OGC Definition
</h2>
<p>
  There is none.
</p>

<h2>
  Oracle Spatial Definition
</h2>
<p>
  \link OSUGR [OSUGR] \endlink defines CoveredBy as follows. &quot;The opposite of
  COVERS. A.COVEREDBY B implies B COVERS A.&quot;
</p>
<p>
  The FDO Oracle Provider implements this definition:
</p>
<p>
  CoveredBy = coveredby as used in the third argument to the
  sdo_relate spatial operator, for example, <br>
  <tt> sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2002, NULL, NULL,
  MDSYS.SDO_ELEM_INFO_ARRAY(1,2,1),
  MDSYS.SDO_ORDINATE_ARRAY(4,1, 8,1)), 'mask=coveredby')</tt>.
</p>

<h2>
  PHP Code Implementation
</h2>
<p>
  The equivalent of the CoveredBy predicate can be expressed
  using MgGeometry methods as follows.
</p>
<pre>
&lt;?php
function CoveredBy ($geom1, $geom2) {
   $withinBool = $geom1-&gt;Within($geom2);
   if (!$withinBool) return $withinBool;
   $boundary1 = $geom1-&gt;Boundary();
   $boundary2 = $geom2-&gt;Boundary();
   $intersectBool = $boundary1-&gt;Intersects($boundary2);
   return $intersectBool;
}
?&gt;
</pre>
<p>
  A Point and MultiPoint cannot be covered because they do not
  have boundaries.
</p>

<h2>
  Illustration
</h2>
<p>
  An endpoint of d and an endpoint of e intersect.
</p>

\image html coveredBy.png

**/
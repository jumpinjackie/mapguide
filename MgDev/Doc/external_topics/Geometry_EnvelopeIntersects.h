/**
\addtogroup EnvelopeIntersects
<h2>
  Definition
</h2>
<p>
  <tt>geom1.EnvelopeIntersects(geom2)</tt> returns true if the envelope
  of <tt>geom1</tt> intersects the envelope of <tt>geom2</tt>.
</p>
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
  There is none. The FDO Oracle Provider implements it using
  the spatial operator sdo_filter, for example, <tt>sdo_filter(a.TUXUNIVERSALGEOM, MDSYS.SDO_GEOMETRY(2003, NULL,
  NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,1003,1),
  MDSYS.SDO_ORDINATE_ARRAY(1,1, 4,1, 4,4, 1,4, 1,1)),
  'querytype = WINDOW')</tt>
</p>
<h2>
  PHP Code Implementation
</h2>
<p>
  The equivalent of the EnvelopeIntersects predicate can be
  expressed using MgGeometricEntity and MgEnvelope methods as
  follows.
</p>
<pre>
&lt;?php
function EnvelopeIntersects ($geom1, $geom2) {
   $envelope1 = $geom1-&gt;Envelope();
   $envelope2 = $geom2-&gt;Envelope();
   $envIntersectBool = $envelope1-&gt;Intersects($envelope2);
   return $envIntersectBool;
}
?&gt;
</pre>
**/ 
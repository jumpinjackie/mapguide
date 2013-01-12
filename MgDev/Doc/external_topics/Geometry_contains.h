/**
\addtogroup Contains

<h2>
  OGC Definition
</h2>
<p>
  \link OGC99049 [OGC99-049] \endlink defines contains as:
</p>
<p>
  A.Contains(B) iff B.Within(A). See \link Within Within \endlink.
</p>

<h2>
  Oracle Spatial Definition
</h2>
<p>
  \link OSUGR [OSUGR] \endlink defines Contains as follows. &quot;The
  interior and boundary of one object is completely contained
  in the interior of the other object.&quot; This is not the same as
  the OGC definition, and so the FDO Oracle Provider maps the
  OGC definition of contains to an OR'd combination of the
  Oracle contains and covers operators.
</p>
<p>
  \link OSUGR [OSUGR] \endlink defines covers as follows. &quot;The
  interior of one object is completely contained in the
  interior or boundary of the other object and their boundaries
  intersect.&quot;
</p>
<p>
  contains = contains+covers as used in the third argument to
  the sdo_relate spatial operator, for example, <br>
  <tt> sdo_relate(a.geometry, mdsys.sdo_geometry(2001, NULL,
  mdsys.sdo_point_type(1,1,NULL), NULL, NULL),
  'mask=contains+covers')</tt>.
</p>

**/
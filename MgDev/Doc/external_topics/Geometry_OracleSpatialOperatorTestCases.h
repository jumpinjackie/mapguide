/**
\addtogroup OracleSpatialOperatorTestCases

<p>
  These tests parallel the ones defined for the MgGeometry
  methods. With one set of exceptions, each test selects one
  geometry from the datastore and relates it to the geometry
  being passed into the sdo_relate function according to the
  mask argument. Each test should return the row referenced in
  the select statement. The set of exceptions concern the
  EnvelopeIntersects predicate. This set uses the sdo_filter
  function.
</p>
<p>
  The mask specifies one or more Oracle spatial operators OR'd
  together. The meaning of these spatial operators is
  reproduced for your convenience in the
  \link SpatialRelationships spatial relationships \endlink
  topic.
</p>
<p>
  Before running the sql commands batch file, you must insert
  the following set of geometries into the Oracle datastore.
  The assumption is that you have applied a schema which
  defines an integer field for storing a feature id and a
  geometry field for storing points, lines, and polygons. It
  also assumes that you named the feature id field FEATID, the
  geometry field GEOMETRY, and the class FEATCLASS.
</p>
<TABLE class="RuledTable">
  <tr>
    <th>FeatId</th>
    <th>Geometry</th>
  </tr>
  <tr>
    <td>0</td>
    <td>POINT(1 1)</td>
  </tr>
  <tr>
    <td>1</td>
    <td>LINESTRING(0 1, 3 1)</td>
  </tr>
  <tr>
    <td>2</td>
    <td>POLYGON((0 0, 2 0, 2 2, 0 2, 0 0))</td>
  </tr>
  <tr>
    <td>3</td>
    <td>MULTIPOINT(1 1, 2 1)</td>
  </tr>
  <tr>
    <td>4</td>
    <td>MULTIPOINT(2 1, 3 1, 5 1)</td>
  </tr>
  <tr>
    <td>5</td>
    <td>LINESTRING(2 1, 6 1)</td>
  </tr>
  <tr>
    <td>6</td>
    <td>POLYGON((1 0, 6 0, 6 2, 1 2, 1 0))</td>
  </tr>
  <tr>
    <td>7</td>
    <td>POLYGON((2 0, 6 0, 6 2, 2 2, 2 0))</td>
  </tr>
  <tr>
    <td>8</td>
    <td>LINESTRING(4 1, 7 1)</td>
  </tr>
  <tr>
    <td>9</td>
    <td>LINESTRING(3 1, 8 1)</td>
  </tr>
  <tr>
    <td>10</td>
    <td>LINESTRING(4 1, 8 1)</td>
  </tr>
  <tr>
    <td>11</td>
    <td>POLYGON((4 0, 8 0, 8 2, 4 2, 4 0))</td>
  </tr>
  <tr>
    <td>12</td>
    <td>POLYGON((4 0, 8 0, 8 2, 3 2, 3 0))</td>
  </tr>
  <tr>
    <td>13</td>
    <td>POLYGON(1 1, 4 1, 4 4, 1 4, 1 1))</td>
  </tr>
  <tr>
    <td>14</td>
    <td>POLYGON((0 0, 5 0, 5 5, 0 5, 0 0))</td>
  </tr>
  <tr>
    <td>15</td>
    <td>POLYGON((1 0, 5 0, 5 5, 1 5, 1 0))</td>
  </tr>
  <tr>
    <td>16</td>
    <td>MULTIPOINT(2 1, 3 1)</td>
  </tr>
  <tr>
    <td>17</td>
    <td>LINESTRING(1 1, 2 1)</td>
  </tr>
</TABLE>
<h2>
  Results
</h2>
<p>
  There are 89 tests. All were expected to return true. The
  following 31 tests returned false:
</p>
<pre>POINT(1 1):Contains:POINT(1 1)
MULTIPOINT(1 1, 2 1):Contains:POINT(1 1)
MULTIPOINT(2 1, 3 1, 5 1):Contains:MULTIPOINT(2 1, 3 1, 5 1)
LINESTRING(4 1, 7 1):Contains:LINESTRING(4 1, 7 1)
POLYGON((1 1, 4 1, 4 4, 1 4, 1 1)):Contains:POLYGON((1 1, 4 1, 4 4, 1 4, 1 1))
LINESTRING(4 1, 7 1):CoveredBy:LINESTRING(4 1, 7 1)
POLYGON((1 1, 4 1, 4 4, 1 4, 1 1)):CoveredBy:POLYGON((1 1, 4 1, 4 4, 1 4, 1 1))
LINESTRING(4 1, 7 1):Crosses:POLYGON((4 0, 6 0, 6 2, 4 2, 4 0))
POINT(1 1):Disjoint:POINT(2 1)
POINT(1 1):Disjoint:MULTIPOINT(2 1, 3 1, 5 1)
POINT(1 1):Disjoint:LINESTRING(4 1, 7 1)
POINT(1 1):Disjoint:POLYGON((2 1, 3 1, 3 2, 2 2, 2 1))
MULTIPOINT(2 1, 3 1, 5 1):Disjoint:MULTIPOINT(4 0, 7 0)
MULTIPOINT(2 1, 3 1, 5 1):Disjoint:LINESTRING(4 0, 7 0)
MULTIPOINT(2 1, 3 1, 5 1):Disjoint:POLYGON((0 0, 1 0, 1 1, 0 1, 0 0))
LINESTRING(4 1, 7 1):Disjoint:LINESTRING(4 0, 7 0)
LINESTRING(4 1, 7 1):Disjoint:POLYGON((0 0, 1 0, 1 1, 0 1, 0 0))
POLYGON((1 1, 4 1, 4 4, 1 4, 1 1)):Disjoint:POLYGON((5 1, 6 1, 6 2, 5 2, 5 1))
POINT(1 1):Inside:POINT(1 1)
POINT(1 1):Inside:MULTIPOINT(1 1, 2 1)
POINT(1 1):Inside:LINESTRING(0 1, 2 1)
MULTIPOINT(2 1, 3 1, 5 1):Inside:MULTIPOINT(2 1, 3 1, 5 1)
MULTIPOINT(2 1, 3 1, 5 1):Inside:LINESTRING(1 1, 6 1)
LINESTRING(4 1, 7 1):Inside:LINESTRING(3 1, 8 1)
MULTIPOINT(2 1, 3 1):Overlaps:MULTIPOINT(3 1, 5 1)
LINESTRING(4 1, 7 1):Overlaps:LINESTRING(6 1, 8 1)
POINT(1 1):Within:POINT(1 1)
POINT(1 1):Within:MULTIPOINT(1 1, 2 1)
MULTIPOINT(2 1, 3 1, 5 1):Within:MULTIPOINT(2 1, 3 1, 5 1)
LINESTRING(4 1, 7 1):Within:LINESTRING(4 1, 7 1)
POLYGON((1 1, 4 1, 4 4, 1 4, 1 1)):Within:POLYGON((1 1, 4 1, 4 4, 1 4, 1 1))
</pre>
<h2>
  Contains
</h2>
<pre>REM POINT(1 1):Contains:POINT(1 1)
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 0 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2001, NULL, MDSYS.SDO_POINT_TYPE(1,1,NULL), NULL, NULL), 'mask=contains+covers') = 'TRUE';
REM LINSTRING(0 1, 2 1):Contains:POINT(1 1)
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 1 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2001, NULL, MDSYS.SDO_POINT_TYPE(1,1,NULL), NULL, NULL), 'mask=contains+covers') = 'TRUE';
REM POLYGON((0 0, 2 0, 2 2, 0 2, 0 0)):Contains:POINT(1 1)
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 2 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2001, NULL, MDSYS.SDO_POINT_TYPE(1,1,NULL), NULL, NULL), 'mask=contains+covers') = 'TRUE';
REM MULTIPOINT(1 1, 2 1):Contains:POINT(1 1)
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 3 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2001, NULL, MDSYS.SDO_POINT_TYPE(1,1,NULL), NULL, NULL), 'mask=contains+covers') = 'TRUE';
REM MULTIPOINT(2 1, 3 1, 5 1):Contains:MULTIPOINT(2 1, 3 1, 5 1)
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 4 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2005, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,1,3), MDSYS.SDO_ORDINATE_ARRAY(2,1, 3,1, 5,1)), 'mask=contains+covers') = 'TRUE';
REM LINESTRING(2 1, 6 1):Contains:MULTIPOINT(2 1, 3 1, 5 1)
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 5 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2005, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,1,3), MDSYS.SDO_ORDINATE_ARRAY(2,1, 3,1, 5,1)), 'mask=contains+covers') = 'TRUE';
REM POLYGON((1 0, 6 0, 6 2, 1 2, 1 0)):Contains:MULTIPOINT(2 1, 3 1, 5 1)
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 6 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2005, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,1,3), MDSYS.SDO_ORDINATE_ARRAY(2,1, 3,1, 5,1)), 'mask=contains+covers') = 'TRUE';
REM POLYGON((2 0, 6 0, 6 2, 2 2, 2 0)):Contains:MULTIPOINT(2 1, 3 1, 5 1)
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 7 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2005, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,1,3), MDSYS.SDO_ORDINATE_ARRAY(2,1, 3,1, 5,1)), 'mask=contains+covers') = 'TRUE';
REM LINESTRING(4 1, 7 1):Contains:LINESTRING(4 1, 7 1)
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 8 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2002, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,2,1), MDSYS.SDO_ORDINATE_ARRAY(4,1, 7,1)), 'mask=contains+covers') = 'TRUE';
REM LINESTRING(3 1, 8 1):Contains:LINESTRING(4 1, 7 1)
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 9 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2002, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,2,1), MDSYS.SDO_ORDINATE_ARRAY(4,1, 7,1)), 'mask=contains+covers') = 'TRUE';
REM LINESTRING(4 1, 8 1):Contains:LINESTRING(4 1, 7 1)
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 10 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2002, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,2,1), MDSYS.SDO_ORDINATE_ARRAY(4,1, 7,1)), 'mask=contains+covers') = 'TRUE';
REM POLYGON((4 0, 8 0, 8 2, 4 2, 4 0)):Contains:LINESTRING(4 1, 7 1)
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 11 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2002, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,2,1), MDSYS.SDO_ORDINATE_ARRAY(4,1, 7,1)), 'mask=contains+covers') = 'TRUE';
REM POLYGON((3 0, 8 0, 8 2, 3 2, 3 0)):Contains:LINESTRING(4 1, 7 1)
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 12 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2002, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,2,1), MDSYS.SDO_ORDINATE_ARRAY(4,1, 7,1)), 'mask=contains+covers') = 'TRUE';
REM POLYGON((1 1, 4 1, 4 4, 1 4, 1 1)):Contains:POLYGON((1 1, 4 1, 4 4, 1 4, 1 1))
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 13 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2003, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,1003,1), MDSYS.SDO_ORDINATE_ARRAY(1,1, 4,1, 4,4, 1,4, 1,1)), 'mask=contains+covers') = 'TRUE';
REM POLYGON((0 0, 5 0, 5 5, 0 5, 0 0)):Contains:POLYGON((1 1, 4 1, 4 4, 1 4, 1 1))
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 14 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2003, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,1003,1), MDSYS.SDO_ORDINATE_ARRAY(1,1, 4,1, 4,4, 1,4, 1,1)), 'mask=contains+covers') = 'TRUE';
REM POLYGON((1 0, 5 0, 5 5, 1 5, 1 0)):Contains:POLYGON((1 1, 4 1, 4 4, 1 4, 1 1))
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 15 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2003, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,1003,1), MDSYS.SDO_ORDINATE_ARRAY(1,1, 4,1, 4,4, 1,4, 1,1)), 'mask=contains+covers') = 'TRUE'; </pre>
<h2>
  CoveredBy
</h2>
<pre>REM LINESTRING(4 1, 7 1):CoveredBy:LINESTRING(4 1, 7 1)
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 8 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2002, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,2,1), MDSYS.SDO_ORDINATE_ARRAY(4,1, 7,1)), 'mask=coveredby') = 'TRUE';
REM LINESTRING(4 1, 7 1):CoveredBy:LINESTRING(4 1, 8 1)
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 8 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2002, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,2,1), MDSYS.SDO_ORDINATE_ARRAY(4,1, 8,1)), 'mask=coveredby') = 'TRUE';
REM LINESTRING(4 1, 7 1):CoveredBy:POLYGON((4 0, 8 0, 8 2, 4 2, 4 0))
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 8 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2003, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,1003,1), MDSYS.SDO_ORDINATE_ARRAY(4,0, 8,0, 8,2, 4,2, 4,0)), 'mask=coveredby') = 'TRUE';
REM POLYGON((1 1, 4 1, 4 4, 1 4, 1 1)):CoveredBy:POLYGON((1 0, 5 0, 5 5, 1 5, 1 0))
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 13 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2003, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,1003,1), MDSYS.SDO_ORDINATE_ARRAY(1,0, 5,0, 5,5, 1,5, 1,0)), 'mask=coveredby') = 'TRUE';
REM POLYGON((1 1, 4 1, 4 4, 1 4, 1 1)):CoveredBy:POLYGON((1 1, 4 1, 4 4, 1 4, 1 1))
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 13 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2003, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,1003,1), MDSYS.SDO_ORDINATE_ARRAY(1,1, 4,1, 4,4, 1,4, 1,1)), 'mask=coveredby') = 'TRUE'; 
</pre>
<h2>
  Crosses
</h2>
<pre>REM MULTIPOINT(2 1, 3 1, 5 1):Crosses:LINESTRING(4 1, 7 1)
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 4 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2002, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,2,1), MDSYS.SDO_ORDINATE_ARRAY(4,1, 7,1)), 'mask=overlapbdydisjoint') = 'TRUE';
REM MULTIPOINT(2 1, 3 1, 5 1):Crosses:POLYGON((1 0, 4 0, 4 2, 1 2, 1 0))
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 4 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2003, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,1003,1), MDSYS.SDO_ORDINATE_ARRAY(1,0, 4,0, 4,2, 1,2, 1,0)), 'mask=overlapbdydisjoint') = 'TRUE';
REM LINESTRING(4 1, 7 1):Crosses:LINESTRING(5 0, 5 2)
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 8 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2002, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,2,1), MDSYS.SDO_ORDINATE_ARRAY(5,0, 5,2)), 'mask=overlapbdydisjoint') = 'TRUE';
REM LINESTRING(4 1, 7 1):Crosses:POLYGON((4 0, 6 0, 6 2, 4 2, 4 0))
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 8 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2003, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,1003,1), MDSYS.SDO_ORDINATE_ARRAY(4,0, 6,0, 6,2, 4,2, 4,0)), 'mask=overlapbdydisjoint') = 'TRUE';
REM LINESTRING(4 1, 7 1):Crosses:POLYGON((5 0, 6 0, 6 2, 5 2, 5 0))
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 8 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2003, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,1003,1), MDSYS.SDO_ORDINATE_ARRAY(5,0, 6,0, 6,2, 5,2, 5,0)), 'mask=overlapbdydisjoint') = 'TRUE';
REM LINESTRING(4 1, 7 1):Crosses:POLYGON((5 0, 8 0, 8 2, 5 2, 5 0))
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 8 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2003, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,1003,1), MDSYS.SDO_ORDINATE_ARRAY(5,0, 8,0, 8,2, 5,2, 5,0)), 'mask=overlapbdydisjoint') = 'TRUE'; </pre>
<h2>
  Disjoint
</h2>
<pre>REM POINT(1 1):Disjoint:POINT(2 1)
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 0 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2001, NULL, MDSYS.SDO_POINT_TYPE(2,1,NULL), NULL, NULL), 'mask=disjoint') = 'TRUE';
REM POINT(1 1):Disjoint:MULTIPOINT(2 1, 3 1, 5 1)
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 0 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2005, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,1,3), MDSYS.SDO_ORDINATE_ARRAY(2,1, 3,1, 5,1)), 'mask=disjoint') = 'TRUE';
REM POINT(1 1):Disjoint:LINESTRING(4 1, 7 1)
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 0 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2002, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,2,1), MDSYS.SDO_ORDINATE_ARRAY(4,1, 7,1)), 'mask=disjoint') = 'TRUE';
REM POINT(1 1):Disjoint:POLYGON((2 1, 3 1, 3 2, 2 2, 2 1))
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 0 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2003, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,1003,1), MDSYS.SDO_ORDINATE_ARRAY(2,1, 3,1, 3,2, 2,2, 2,1)), 'mask=disjoint') = 'TRUE';
REM MULTIPOINT(2 1, 3 1, 5 1):Disjoint:MULTIPOINT(4 0, 7 0)
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 4 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2005, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,1,2), MDSYS.SDO_ORDINATE_ARRAY(4,0, 7,0)), 'mask=disjoint') = 'TRUE';
REM MULTIPOINT(2 1, 3 1, 5 1):Disjoint:LINESTRING(4 0, 7 0)
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 4 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2002, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,2,1), MDSYS.SDO_ORDINATE_ARRAY(4,0, 7,0)), 'mask=disjoint') = 'TRUE';
REM MULTIPOINT(2 1, 3 1, 5 1):Disjoint:POLYGON((0 0, 1 0, 1 1, 0 1, 0 0))
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 4 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2003, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,1003,1), MDSYS.SDO_ORDINATE_ARRAY(0,0, 1,0, 1,1, 0,1, 0,0)), 'mask=disjoint') = 'TRUE';
REM LINESTRING(4 1, 7 1):Disjoint:LINESTRING(4 0, 7 0)
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 8 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2002, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,2,1), MDSYS.SDO_ORDINATE_ARRAY(4,0, 7,0)), 'mask=disjoint') = 'TRUE';
REM LINESTRING(4 1, 7 1):Disjoint:POLYGON((0 0, 1 0, 1 1, 0 1, 0 0))
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 8 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2003, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,1003,1), MDSYS.SDO_ORDINATE_ARRAY(0,0, 1,0, 1,1, 0,1, 0,0)), 'mask=disjoint') = 'TRUE';
REM POLYGON((1 1, 4 1, 4 4, 1 4, 1 1)):Disjoint:POLYGON((5 1, 6 1, 6 2, 5 2, 5 1))
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 13 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2003, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,1003,1), MDSYS.SDO_ORDINATE_ARRAY(5,1, 6,1, 6,2, 5,2, 5,1)), 'mask=disjoint') = 'TRUE'; 
</pre>
<h2>
  Equals
</h2>
<pre>REM POINT(1 1):Equals:POINT(1 1)
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 0 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2001, NULL, MDSYS.SDO_POINT_TYPE(1,1,NULL), NULL, NULL), 'mask=equal') = 'TRUE';
REM MULTIPOINT(2 1, 3 1, 5 1):Equals:MULTIPOINT(2 1, 3 1, 5 1)
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 4 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2005, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,1,3), MDSYS.SDO_ORDINATE_ARRAY(2,1, 3,1, 5,1)), 'mask=equal') = 'TRUE';
REM LINESTRING(4 1, 7 1):Equals:LINESTRING(4 1, 7 1)
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 8 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2002, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,2,1), MDSYS.SDO_ORDINATE_ARRAY(4,1, 7,1)), 'mask=equal') = 'TRUE';
REM POLYGON((1 1, 4 1, 4 4, 1 4, 1 1)):Equals:POLYGON((1 1, 4 1, 4 4, 1 4, 1 1))
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 13 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2003, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,1003,1), MDSYS.SDO_ORDINATE_ARRAY(1,1, 4,1, 4,4, 1,4, 1,1)), 'mask=equal') = 'TRUE'; 
</pre>
<h2>
  EnvelopeIntersects
</h2>
<pre>REM POINT(1 1):EnvelopeIntersects:POINT(1 1)
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 0 and sdo_filter(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2001, NULL, MDSYS.SDO_POINT_TYPE(1,1,NULL), NULL, NULL), 'querytype = WINDOW') = 'TRUE';
REM MULTIPOINT(2 1, 3 1, 5 1):EnvelopeIntersects:MULTIPOINT(2 1, 3 1, 5 1)
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 4 and sdo_filter(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2005, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,1,3), MDSYS.SDO_ORDINATE_ARRAY(2,1, 3,1, 5,1)), 'querytype = WINDOW') = 'TRUE';
REM LINESTRING(4 1, 7 1):EnvelopeIntersects:LINESTRING(4 1, 7 1)
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 8 and sdo_filter(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2002, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,2,1), MDSYS.SDO_ORDINATE_ARRAY(4,1, 7,1)), 'querytype = WINDOW') = 'TRUE';
REM POLYGON((1 1, 4 1, 4 4, 1 4, 1 1)):EnvelopeIntersects:POLYGON((1 1, 4 1, 4 4, 1 4, 1 1))
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 13 and sdo_filter(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2003, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,1003,1), MDSYS.SDO_ORDINATE_ARRAY(1,1, 4,1, 4,4, 1,4, 1,1)), 'querytype = WINDOW') = 'TRUE'; 
</pre>
<h2>
  Inside
</h2>
<pre>REM POINT(1 1):Inside:POINT(1 1)
select a.FEATID,a.GEOEMTRY from FEATCLASS a where a.FEATID = 0 and sdo_relate(a.GEOEMTRY, MDSYS.SDO_GEOMETRY(2001, NULL, MDSYS.SDO_POINT_TYPE(1,1,NULL), NULL, NULL), 'mask=inside') = 'TRUE';
REM POINT(1 1):Inside:MULTIPOINT(1 1, 2 1)
select a.FEATID,a.GEOEMTRY from FEATCLASS a where a.FEATID = 0 and sdo_relate(a.GEOEMTRY, MDSYS.SDO_GEOMETRY(2005, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,1,2), MDSYS.SDO_ORDINATE_ARRAY(1,1, 2,1)), 'mask=inside') = 'TRUE';
REM POINT(1 1):Inside:LINESTRING(0 1, 2 1)
select a.FEATID,a.GEOEMTRY from FEATCLASS a where a.FEATID = 0 and sdo_relate(a.GEOEMTRY, MDSYS.SDO_GEOMETRY(2002, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,2,1), MDSYS.SDO_ORDINATE_ARRAY(0,1, 2,1)), 'mask=inside') = 'TRUE';
REM POINT(1 1):Inside:POLYGON((0 0, 2 0, 2 2, 0 2, 0 0))
select a.FEATID,a.GEOEMTRY from FEATCLASS a where a.FEATID = 0 and sdo_relate(a.GEOEMTRY, MDSYS.SDO_GEOMETRY(2003, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,1003,1), MDSYS.SDO_ORDINATE_ARRAY(0,0, 2,0, 2,2, 0,2, 0,0)), 'mask=inside') = 'TRUE';
REM MULTIPOINT(2 1, 3 1, 5 1):Inside:MULTIPOINT(2 1, 3 1, 5 1)
select a.FEATID,a.GEOEMTRY from FEATCLASS a where a.FEATID = 4 and sdo_relate(a.GEOEMTRY, MDSYS.SDO_GEOMETRY(2005, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,1,3), MDSYS.SDO_ORDINATE_ARRAY(2,1, 3,1, 5,1)), 'mask=inside') = 'TRUE';
REM MULTIPOINT(2 1, 3 1, 5 1):Inside:LINESTRING(1 1, 6 1)
select a.FEATID,a.GEOEMTRY from FEATCLASS a where a.FEATID = 4 and sdo_relate(a.GEOEMTRY, MDSYS.SDO_GEOMETRY(2002, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,2,1), MDSYS.SDO_ORDINATE_ARRAY(1,1, 6,1)), 'mask=inside') = 'TRUE';
REM MULTIPOINT(2 1, 3 1, 5 1):Inside:POLYGON((1 0, 6 0, 6 2, 1 2, 1 0))
select a.FEATID,a.GEOEMTRY from FEATCLASS a where a.FEATID = 4 and sdo_relate(a.GEOEMTRY, MDSYS.SDO_GEOMETRY(2003, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,1003,1), MDSYS.SDO_ORDINATE_ARRAY(1,0, 6,0, 6,2, 1,2, 1,0)), 'mask=inside') = 'TRUE';
REM LINESTRING(4 1, 7 1):Inside:LINESTRING(3 1, 8 1)
select a.FEATID,a.GEOEMTRY from FEATCLASS a where a.FEATID = 8 and sdo_relate(a.GEOEMTRY, MDSYS.SDO_GEOMETRY(2002, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,2,1), MDSYS.SDO_ORDINATE_ARRAY(3,1, 8,1)), 'mask=inside') = 'TRUE';
REM LINESTRING(4 1, 7 1):Inside:POLYGON((3 0, 8 0, 8 2, 3 2, 3 0))
select a.FEATID,a.GEOEMTRY from FEATCLASS a where a.FEATID = 8 and sdo_relate(a.GEOEMTRY, MDSYS.SDO_GEOMETRY(2003, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,1003,1), MDSYS.SDO_ORDINATE_ARRAY(3,0, 8,0, 8,2, 3,2, 3,0)), 'mask=inside') = 'TRUE';
REM POLYGON((1 1, 4 1, 4 4, 1 4, 1 1)):Inside:POLYGON((0 0, 5 0, 5 5, 0 5, 0 0))
select a.FEATID,a.GEOEMTRY from FEATCLASS a where a.FEATID = 13 and sdo_relate(a.GEOEMTRY, MDSYS.SDO_GEOMETRY(2003, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,1003,1), MDSYS.SDO_ORDINATE_ARRAY(0,0, 5,0, 5,5, 0,5, 0,0)), 'mask=inside') = 'TRUE'; 
</pre>
<h2>
  Intersects
</h2>
<pre>REM POINT(1 1):Intersects:POINT(1 1)
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 0 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2001, NULL, MDSYS.SDO_POINT_TYPE(1,1,NULL), NULL, NULL), 'mask=anyinteract') = 'TRUE';
REM MULTIPOINT(2 1, 3 1, 5 1):Intersects:MULTIPOINT(2 1, 3 1, 5 1)
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 4 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2005, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,1,3), MDSYS.SDO_ORDINATE_ARRAY(2,1, 3,1, 5,1)), 'mask=anyinteract') = 'TRUE';
REM LINESTRING(4 1, 7 1):Intersects:LINESTRING(4 1, 7 1)
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 8 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2002, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,2,1), MDSYS.SDO_ORDINATE_ARRAY(4,1, 7,1)), 'mask=anyinteract') = 'TRUE';
REM POLYGON((1 1, 4 1, 4 4, 1 4, 1 1)):Intersects:POLYGON((1 1, 4 1, 4 4, 1 4, 1 1))
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 13 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2003, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,1003,1), MDSYS.SDO_ORDINATE_ARRAY(1,1, 4,1, 4,4, 1,4, 1,1)), 'mask=anyinteract') = 'TRUE'; 
</pre>
<h2>
  Overlaps
</h2>
<pre>REM MULTIPOINT(2 1, 3 1):Overlaps:MULTIPOINT(3 1, 5 1)
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 16 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2005, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,1,2), MDSYS.SDO_ORDINATE_ARRAY(3,1, 5,1)), 'mask=overlapbdyintersect') = 'TRUE';
REM LINESTRING(4 1, 7 1):Overlaps:LINESTRING(6 1, 8 1)
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 8 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2002, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,2,1), MDSYS.SDO_ORDINATE_ARRAY(6,1, 8,1)), 'mask=overlapbdyintersect') = 'TRUE';
REM POLYGON((1 1, 4 1, 4 4, 1 4, 1 1)):Overlaps:POLYGON((0 2, 5 2, 5 3, 0 3, 0 2))
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 13 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2003, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,1003,1), MDSYS.SDO_ORDINATE_ARRAY(0,2, 5,2, 5,3, 0,3, 0,2)), 'mask=overlapbdyintersect') = 'TRUE';
REM POLYGON((1 1, 4 1, 4 4, 1 4, 1 1)):Overlaps:POLYGON((0 2, 3 2, 3 3, 0 3, 0 2))
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 13 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2003, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,1003,1), MDSYS.SDO_ORDINATE_ARRAY(0,2, 3,2, 3,3, 0,3, 0,2)), 'mask=overlapbdyintersect') = 'TRUE'; 
</pre>
<h2>
  Touches
</h2>
<pre>REM POINT(1 1):Touches:LINESTRING(1 1, 2 1)
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 0 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2002, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,2,1), MDSYS.SDO_ORDINATE_ARRAY(1,1, 2,1)), 'mask=touch') = 'TRUE';
REM POINT(1 1):Touches:POLYGON((0 0, 1 0, 1 1, 0 1, 0 0))
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 0 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2003, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,1003,1), MDSYS.SDO_ORDINATE_ARRAY(0,0, 1,0, 1,1, 0,1, 0,0)), 'mask=touch') = 'TRUE';
REM MULTIPOINT(2 1, 3 1):Touches:LINESTRING(3 1, 4 1)
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 16 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2002, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,2,1), MDSYS.SDO_ORDINATE_ARRAY(3,1, 4,1)), 'mask=touch') = 'TRUE';
REM MULTIPOINT(1 1, 2 1):Touches:POLYGON((0 0, 1 0, 1 1, 0 1, 0 0))
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 3 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2003, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,1003,1), MDSYS.SDO_ORDINATE_ARRAY(0,0, 1,0, 1,1, 0,1, 0,0)), 'mask=touch') = 'TRUE';
REM LINESTRING(4 1, 7 1):Touches:LINESTRING(5 1, 5 2)
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 8 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2002, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,2,1), MDSYS.SDO_ORDINATE_ARRAY(5,1, 5,2)), 'mask=touch') = 'TRUE';
REM LINESTRING(4 1, 7 1):Touches:LINESTRING(4 1, 4 2)
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 8 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2002, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,2,1), MDSYS.SDO_ORDINATE_ARRAY(4,1, 4,2)), 'mask=touch') = 'TRUE';
REM LINESTRING(1 1, 2 1):Touches:POLYGON((0 0, 1 0, 1 1, 0 1, 0 0))
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 17 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2003, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,1003,1), MDSYS.SDO_ORDINATE_ARRAY(0,0, 1,0, 1,1, 0,1, 0,0)), 'mask=touch') = 'TRUE';
REM POLYGON((1 1, 4 1, 4 4, 1 4, 1 1)):Touches:POLYGON((4 4, 5 5, 4 6, 3 5, 4 4))
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 13 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2003, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,1003,1), MDSYS.SDO_ORDINATE_ARRAY(4,4, 5,5, 4,6, 3,5, 4,4)), 'mask=touch') = 'TRUE';
REM POLYGON((1 1, 4 1, 4 4, 1 4, 1 1)):Touches:POLYGON((4 2, 5 2, 5 3, 4 3, 4 2))
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 13 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2003, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,1003,1), MDSYS.SDO_ORDINATE_ARRAY(4,2, 5,2, 5,3, 4,3, 4,2)), 'mask=touch') = 'TRUE'; 
</pre>
<h2>
  Within
</h2>
<pre>REM POINT(1 1):Within:POINT(1 1)
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 0 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2001, NULL, MDSYS.SDO_POINT_TYPE(1,1,NULL), NULL, NULL), 'mask=inside+coveredby') = 'TRUE';
REM POINT(1 1):Within:MULTIPOINT(1 1, 2 1)
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 0 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2005, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,1,2), MDSYS.SDO_ORDINATE_ARRAY(1,1, 2,1)), 'mask=inside+coveredby') = 'TRUE';
REM POINT(1 1):Within:LINESTRING(0 1, 2 1)
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 0 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2002, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,2,1), MDSYS.SDO_ORDINATE_ARRAY(0,1, 2,1)), 'mask=inside+coveredby') = 'TRUE';
REM POINT(1 1):Within:POLYGON((0 0, 2 0, 2 2, 0 2, 0 0))
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 0 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2003, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,1003,1), MDSYS.SDO_ORDINATE_ARRAY(0,0, 2,0, 2,2, 0,2, 0,0)), 'mask=inside+coveredby') = 'TRUE';
REM MULTIPOINT(2 1, 3 1, 5 1):Within:MULTIPOINT(2 1, 3 1, 5 1)
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 4 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2005, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,1,3), MDSYS.SDO_ORDINATE_ARRAY(2,1, 3,1, 5,1)), 'mask=inside+coveredby') = 'TRUE';
REM MULTIPOINT(2 1, 3 1, 5 1):Within:LINESTRING(1 1, 6 1)
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 4 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2002, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,2,1), MDSYS.SDO_ORDINATE_ARRAY(1,1, 6,1)), 'mask=inside+coveredby') = 'TRUE';
REM MULTIPOINT(2 1, 3 1, 5 1):Within:LINESTRING(2 1, 6 1)
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 4 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2002, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,2,1), MDSYS.SDO_ORDINATE_ARRAY(2,1, 6,1)), 'mask=inside+coveredby') = 'TRUE';
REM MULTIPOINT(2 1, 3 1, 5 1):Within:POLYGON((1 0, 6 0, 6 2, 1 2, 1 0))
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 4 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2003, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,1003,1), MDSYS.SDO_ORDINATE_ARRAY(1,0, 6,0, 6,2, 1,2, 1,0)), 'mask=inside+coveredby') = 'TRUE';
REM MULTIPOINT(2 1, 3 1, 5 1):Within:POLYGON((2 0, 6 0, 6 2, 2 2, 2 0))
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 4 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2003, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,1003,1), MDSYS.SDO_ORDINATE_ARRAY(2,0, 6,0, 6,2, 2,2, 2,0)), 'mask=inside+coveredby') = 'TRUE';
REM LINESTRING(4 1, 7 1):Within:LINESTRING(4 1, 7 1)
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 8 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2002, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,2,1), MDSYS.SDO_ORDINATE_ARRAY(4,1, 7,1)), 'mask=inside+coveredby') = 'TRUE';
REM LINESTRING(4 1, 7 1):Within:LINESTRING(3 1, 8 1)
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 8 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2002, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,2,1), MDSYS.SDO_ORDINATE_ARRAY(3,1, 8,1)), 'mask=inside+coveredby') = 'TRUE';
REM LINESTRING(4 1, 7 1):Within:LINESTRING(4 1, 8 1)
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 8 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2002, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,2,1), MDSYS.SDO_ORDINATE_ARRAY(4,1, 8,1)), 'mask=inside+coveredby') = 'TRUE';
REM LINESTRING(4 1, 7 1):Within:POLYGON((4 0, 8 0, 8 2, 4 2, 4 0))
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 8 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2003, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,1003,1), MDSYS.SDO_ORDINATE_ARRAY(4,0, 8,0, 8,2, 4,2, 4,0)), 'mask=inside+coveredby') = 'TRUE';
REM LINESTRING(4 1, 7 1):Within:POLYGON((3 0, 8 0, 8 2, 3 2, 3 0))
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 8 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2003, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,1003,1), MDSYS.SDO_ORDINATE_ARRAY(3,0, 8,0, 8,2, 3,2, 3,0)), 'mask=inside+coveredby') = 'TRUE';
REM POLYGON((1 1, 4 1, 4 4, 1 4, 1 1)):Within:POLYGON((1 1, 4 1, 4 4, 1 4, 1 1))
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 13 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2003, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,1003,1), MDSYS.SDO_ORDINATE_ARRAY(1,1, 4,1, 4,4, 1,4, 1,1)), 'mask=inside+coveredby') = 'TRUE';
REM POLYGON((1 1, 4 1, 4 4, 1 4, 1 1)):Within:POLYGON((0 0, 5 0, 5 5, 0 5, 0 0))
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 13 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2003, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,1003,1), MDSYS.SDO_ORDINATE_ARRAY(0,0, 5,0, 5,5, 0,5, 0,0)), 'mask=inside+coveredby') = 'TRUE';
REM POLYGON((1 1, 4 1, 4 4, 1 4, 1 1)):Within:POLYGON((1 0, 5 0, 5 5, 1 5, 1 0))
select a.FEATID,a.GEOMETRY from FEATCLASS a where a.FEATID = 13 and sdo_relate(a.GEOMETRY, MDSYS.SDO_GEOMETRY(2003, NULL, NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1,1003,1), MDSYS.SDO_ORDINATE_ARRAY(1,0, 5,0, 5,5, 1,5, 1,0)), 'mask=inside+coveredby') = 'TRUE';
</pre>

**/
/**
\addtogroup FdoProviderSpatialOperatorTests
<p>
  These tests parallel the ones defined for the MgGeometry
  methods.
</p>
<p>
  Before running the sql commands batch file, you must insert
  the following set of geometries into the provider datastore.
  The assumption is that you have applied a schema which
  defines an integer field for storing a feature id and a
  geometry field for storing points, lines, and polygons. It
  also assumes that you named the feature id field FEATID, the
  geometry field GEOMETRY, and the class FEATCLASS.
</p>
<TABLE class="RuledTable">
  <tr>
    <th><b>FeatId</b></th>
    <th><b>Geometry</b></th>
  </tr>
  <tr>
    <td>0 </td>
    <td>POINT(1 1)</td>
  </tr>
  <tr>
    <td>1 </td>
    <td>LINESTRING(0 1, 3 1)</td>
  </tr>
  <tr>
    <td>2 </td>
    <td>POLYGON((0 0, 2 0, 2 2, 0 2, 0 0))</td>
  </tr>
  <tr>
    <td>3 </td>
    <td>MULTIPOINT(1 1, 2 1)</td>
  </tr>
  <tr>
    <td>4 </td>
    <td>MULTIPOINT(2 1, 3 1, 5 1)</td>
  </tr>
  <tr>
    <td>5 </td>
    <td>LINESTRING(2 1, 6 1)</td>
  </tr>
  <tr>
    <td>6 </td>
    <td>POLYGON((1 0, 6 0, 6 2, 1 2, 1 0))</td>
  </tr>
  <tr>
    <td>7 </td>
    <td>POLYGON((2 0, 6 0, 6 2, 2 2, 2 0))</td>
  </tr>
  <tr>
    <td>8 </td>
    <td>LINESTRING(4 1, 7 1)</td>
  </tr>
  <tr>
    <td>9 </td>
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
**/
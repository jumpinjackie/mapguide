/**
\addtogroup Geometry_Module

<p>
  The Geometry API supports the round-tripping of 2D and
  \link two_five_d 2.5D \endlink
  geometries as well as the analysis of 2D geometries.
</p>
<h2> Creating geometries </h2>
<p>
  Use an MgGeometryFactory object to create MgGeometry objects
  from coordinates. Click MgGeometryFactory to see a graphical 
  overview of geometry creation.
</p>
<p>
  Use an MgWktReaderWriter object to create MgGeometry objects
  from strings in
  \link AgfText AGF Text \endlink
  form.
</p>
<p>
  Use an MgAgfReaderWriter object to translate MgGeometry 
  objects into a form suitable for the creation of an
  MgGeometryProperty object for inserting into a datastore.
</p>
<p>
  Click on the geometry class name to view sample creation
  code. The supported geometries are:
</p>
<ul>
  <li>MgPoint and MgMultiPoint</li>
  <li>MgLineString and MgMultiLineString</li>
  <li>MgCurveString and MgMultiCurveString</li>
  <li>MgPolygon and MgMultiPolygon</li>
  <li>MgCurvePolygon and MgMultiCurvePolygon</li>
  <li>MgMultiGeometry (heterogenous collection of geometries)</li>
</ul>
**/
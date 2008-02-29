/**
\addtogroup Collections_Module
<p>
  One of the functions of collections is to serve as a repository for 
  intermediate results. In this role, a collection is a helper class. An 
  example is the construction of an MgLineString geometry. From the 
  perspective of construction, this geometry is a set of coordinates. The 
  sequence of construction is:
</p>
<ol>
  <li>create a coordinate</li>
  <li>add coordinate to coordinate collection</li>
  <li>do 1 and 2 until all coordinates created</li>
  <li>create the line string using coordinate collection</li>
</ol>
<p>
	See \link GeometryCollections Geometry Collections \endlink for the list of collections used in the construction of geometries.
</p>
**/
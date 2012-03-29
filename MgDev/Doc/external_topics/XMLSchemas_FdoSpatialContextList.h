/**
\addtogroup FdoSpatialContextList_schema
<p>
  This schema governs the output of
  MgContextSpatialReader::ToXml(). What follows is a
  representation of the element hierarchy in
  FdospatialContextList-1.0.0.xsd. Frequency of element
  occurrence is indicated by adding a quantifier symbol (* for
  0 or more, + for 1 or more, ? for 0 or 1 and nothing for 1)
  to the end of the element identifier.
</p>
<pre>
FdoSpatialContextList
   ProviderName
   SpatialContext+ IsActive=&lt;T|F&gt;
      Name
      Description
      CoordinaetSystemName
      CoordinateSystemWkt
      ExtentType is one of scStatic or scDynamic
      Extent
         LowerLeftCoordinate
            X
            Y
            Z?
            M?
         UpperRightCoordinate
            X
            Y
            Z?
            M?
</pre>
<h2>Example</h2>
<p>
  Here is output from the MgSpatialContextReader::ToXml method.
  The actual output has no newlines.
</p>
<pre>
&lt;?xml version="1.0" encoding="UTF-8" ?&gt;
&lt;FdoSpatialContextList&gt;
   &lt;ProviderName&gt;Autodesk.Oracle.3.0&lt;/ProviderName&gt;
   &lt;SpatialContext IsActive="true"&gt;
      &lt;Name&gt;SC_0&lt;/Name&gt;
      &lt;Description&gt;Default Database Spatial Context&lt;/Description&gt;
      &lt;CoordinateSystemName&gt;&lt;/CoordinateSystemName&gt;
      &lt;CoordinateSystemWkt&gt;&lt;/CoordinateSystemWkt&gt;
      &lt;ExtentType&gt;Static&lt;/ExtentType&gt;
      &lt;Extent&gt;
         &lt;LowerLeftCoordinate&gt;
            &lt;X&gt;-1000.0&lt;/X&gt;
            &lt;Y&gt;-1000.0&lt;/Y&gt;
         &lt;/LowerLeftCoordinate&gt;
         &lt;UpperRightCoordinate&gt;
            &lt;X&gt;1000.0&lt;/X&gt;
            &lt;Y&gt;1000.0&lt;/Y&gt;
         &lt;/UpperRightCoordinate&gt;
      &lt;/Extent&gt;
      &lt;XYTolerance&gt;0.001&lt;/XYTolerance&gt;
      &lt;ZTolerance&gt;0&lt;/ZTolerance&gt;
   &lt;/SpatialContext&gt;
&lt;/FdoSpatialContextList&gt;
</pre>

<a href="../../xml_schemas/FdoSpatialContextList/FdoSpatialContextList.html">Details</a>
**/
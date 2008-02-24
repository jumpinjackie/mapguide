/**
\addtogroup FeatureSource_schema

<p>
This schema governs the structure of a feature source xml
file. Before you can create a functional connection to an
FDO provider, you must create a feature source xml file
containing the connection parameter values for that provider
and add it to the repository.
</p>
<p>
What follows is a representation of the element hierarchy in
FeatureSource-1.0.0.xsd. Frequency of element occurrence is
indicated by adding a quantifier symbol (* for 0 or more, +
for 1 or more, ? for 0 or 1 and nothing for 1) to the end of
the element identifier.
</p>
<pre>
FeatureSource version=&lt;string&gt;
   Provider
   ConnectionProperties
      ConnectionProperty*
         Name
         Value
   Configuration?
   SpatialContext?
   LongTransaction?
</pre>

<a href="../../xml_schemas/FeatureSource/FeatureSource.htm">Details</a>
**/
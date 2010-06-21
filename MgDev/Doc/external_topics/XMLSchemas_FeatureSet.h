/**
\addtogroup FeatureSet_schema

<p>
This schema governs the XML formatting of data retrieved from
a feature source and output to an MgByteReader by an
MgReader::ToXml call.
</p>
<p>
What follows is a representation of the element hierarchy in
FeatureSource-1.0.0.xsd. Frequency of element occurrence is
indicated by adding a quantifier symbol (* for 0 or more, +
for 1 or more, ? for 0 or 1 and nothing for 1) to the end of
the element identifier.
</p>

<pre>
FeatureSet
   \<xs:schema\>...\</xs:schema\>
   Features
      Feature+
         Property+
            Name
            Value
</pre>

<a href="../../xml_schemas/FeatureSet/FeatureSet.html">Details</a>
**/
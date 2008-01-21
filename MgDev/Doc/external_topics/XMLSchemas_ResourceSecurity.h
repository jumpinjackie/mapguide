/**
\addtogroup ResourceSecurity_schema

<p>
This schema governs the structure of a complex type used in
the \link ResourceDocumentHeader_schema ResourceDocumentHeader \endlink schema. What follows is a
representation of the enumeration values in a simple type
&quot;PermissionsType&quot; and the element hierarchy in a complex type
&quot;ResourceSecurityType&quot; in ResourceSecurity-1.0.0.xsd.
Frequency of element occurrence is indicated by adding a
quantifier symbol (* for 0 or more, + for 1 or more, ? for 0
or 1 and nothing for 1) to the end of the element
identifier.
</p>
<pre>
PermissionsType
   enumerationValue=&quot;n&quot;
   enumerationValue=&quot;r&quot;
   enumerationValue=&quot;r,w&quot;
ResourceSecurityType
   Inherited
   Users?
      User+
         Name
         Permissions : PermissionsType
   Groups?
      Group+
         Name
         Permissions : PermissionsType
</pre>

<a href="../../xml_schemas/ResourceSecurity/ResourceSecurity.html">Details</a>

**/
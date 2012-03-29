/**
\addtogroup ResourceDocumentHeader_schema

<p>
This schema governs the structure of a resource document
header. When you add a document resource as opposed to a
folder resource to the repository, you provide a resource
document header as an argument to the call to
MgResourceService::SetResource.
</p>
<p>
What follows is a representation of the element hierarchy in
ResourceDocumentHeader-1.0.0.xsd. Frequency of element
occurrence is indicated by adding a quantifier symbol (* for
0 or more, + for 1 or more, ? for 0 or 1 and nothing for 1)
to the end of the element identifier.
</p>
<pre>
ResourceDocumentHeader
   Security // this is a complex type defined in ResourceSecurity-1.0.0.xsd
   IconName?
   Dependency?
      ResourceId
</pre>

<a href="../../xml_schemas/ResourceDocumentHeader/ResourceDocumentHeader.html">Details</a>
**/
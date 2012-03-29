/**
\addtogroup FeatureProviderRegistry_schema

<p>
This schema governs the output of
MgFeatureService::GetFeatureProviders(). What follows is a
representation of the element hierarchy in
FeatureProviderRegistry-1.0.0.xsd. Frequency of element
occurrence is indicated by adding a quantifier symbol (* for
0 or more, + for 1 or more, ? for 0 or 1 and nothing for 1)
to the end of the element identifier.
</p>
<pre>
FeatureProviderRegistry
   FeatureProvider*
      Name
      DisplayName
      Description
      Version
      FeatureDataObjectsVersion
      ConnectionProperties
         ConnectionProperty* Required=&lt;T|F&gt; Protected=&lt;T|F&gt; Enumerable=&lt;T|F&gt;
            Name
            LocalizedName?
            DefaultValue?
            Value*
</pre>
<a href="../../xml_schemas/FeatureProviderRegistry/FeatureProviderRegistry.html">Details</a>

**/
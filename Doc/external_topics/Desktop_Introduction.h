/**
\if DESKTOPAPI
\addtogroup Desktop_Module
<p>
	The Desktop Platform enables you work with the classes in the MapGuide API through desktop-specific subclasses
</p>
<p>
	<strong>Note:</strong> With the exception of MgServiceFactory and MgPlatform, all classes in the Desktop Platform are prefixed 
	with "Mgd" instead of "Mg" to distinguish them from the MapGuide core classes that most of these extend from. This
	is done to avoid the potential naming collisions of classes if you are developing applications that use both the
	official MapGuide API and mg-desktop.
</p>
<p>
	Any such classes can be up-casted to their "Mg" counterparts and can be used with any method that takes an "Mg" class.
</p>
<p>Some examples of Desktop Platform classes that extend their MapGuide core counterparts</p>
<ul>
	<li>MgdFeatureService -> MgFeatureService</li>
	<li>MgdResourceService -> MgResourceService</li>
	<li>MgdMap -> MgMapBase</li>
	<li>MgdLayer -> MgLayerBase</li>
	<li>MgdSelection -> MgSelectionBase</li>
</ul>
<p>
	<strong>Note:</strong> If you are using the Desktop Platform classes in conjunction with the official MapGuide API clases,
	you cannot cast Desktop classes to their MapGuide counterparts or vice versa. For example, such casts are not allowed:
</p>
<ul>
	<li>MgdMap <-> MgMap</li>
	<li>MgdLayer <-> MgLayer</li>
	<li>MgdSelection <-> MgSelection</li>
</ul>
\endif
**/

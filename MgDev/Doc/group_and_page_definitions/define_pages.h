/*! \mainpage Overview
<div class="mainpage">
<!-- This is the index.html page that the user first sees. -->

\section summary \ProdName API Reference
\if MAPGUIDEAPI
For detailed information about using the MapGuide API see the \DevGuide.
\n\n
\endif
Maps are made up of layers, where each layer displays features from a feature class. See
\ref Maps_and_Layers_Module "Maps and Layers" for more details.
\n\n
Much of the work with this API is done through the following services:

 \li \ref Resource_Service_Module "Resource Service": manipulates resources and repositories.
 \li \ref Feature_Service_Module "Feature Service": provides access to FDO providers.
\if DESKTOPAPI
 \li \ref Desktop_Module "Desktop Platform": Contains classes specific to mg-desktop and extensions to the core MapGuide classes to allow for use in Desktop Applications
\endif
\if MAPGUIDEAPI
 \li \ref Rendering_Service_Module "Rendering Service": renders a map into a bitmapped image.
 \li \ref Drawing_Service_Module "Drawing Service": allows low level access to drawing sources (DWF data).
 \li \ref Mapping_Service_Module "Mapping Service": generates eMaps and ePlots for viewing with the DWF Viewer.
 \li \ref Profiling_Service_Module "Profiling Service": Profiles map and layer rendering for performance data.
 \li \ref Site_Service_Module "Site Service": configures users, groups, and user sessions.
 \li \ref Tile_Service_Module "Tile Service": supports smooth pans and zooms in the AJAX Viewer.
\endif

Some other important areas of the API are:

 \li \ref Feature_Schema_Module "Feature Schema": defines the structure of data contained in a Feature Source
 \li \ref Coordinate_System_Module "Coordinate System": supports coordinate system transformations.
 \li \ref Geometry_Module "Geometry": manipulates geometric objects.
 \li \ref Collections_Module "Collections": classes to hold collections of objects.
 \li \ref Common_Module "Common": common utility classes.
 \li \ref Exceptions_Module "Exceptions"
 
The API uses XML data extensively. The schemas are documented in \ref XML_Schemas_Module "XML Schemas".
 
\htmlinclude copyright.html

</div>
*/






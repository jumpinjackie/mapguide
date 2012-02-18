Working With Feature Data
=========================

Overview of Features
--------------------

.. note::

    The Working With Feature Data sample, in the Developer's Guide samples,
    demonstrates concepts from this chapter.

Understanding features is fundamental to being able to use the MapGuide Web
API. Nearly every application will need to interact with feature data in one form
or another.

*Features* are map objects representing items like roads (polylines), lakes
(polygons), or locations (points).

A *feature source* is a resource that contains a set of related features, stored in a
file or database. Some common feature source types are SDF files, SHP files, or
data in a spatial database.

For example, you may have a feature source that contains data for roads. Feature
sources can be stored in the library repository or in a session repository. A feature
source identifier describes a complete path in the repository. For example,

``Library://Samples/Sheboygan/Data/RoadCenterLines.FeatureSource``

Within a single feature source there may be one or more *feature classes*. A feature
class describes a subset of the features in the feature source. In many cases, there
is one feature class for each feature source. For example, there may be a Roads
feature class in the RoadCenterLines feature source.

A feature class contains one or more features. Each feature has a geometry that
defines the spatial representation of the feature. Features will also generally
have one or more properties that provide additional information. For example,
a feature class containing road data may have properties for the road name
and the number of lanes. Feature properties can be of different types, like
strings, integers, and floating point numbers. Possible types are defined in the
class ``MgPropertyType``.

In some cases, a feature property will be another feature. For example, a Roads
feature might have a Sidewalk feature as one of its properties.

A map layer may contain the features from a feature class. The features are
rendered using the feature geometry.

The Web API Feature Service provides functions for querying and updating
feature data.

.. index::
    single: Features; Querying

.. _querying-feature-data:

Querying Feature Data
---------------------

In order to work with feature data, you must first select the features you are
interested in. This can be done with the Viewer or through Web API calls.

.. index::
    single: Feature Readers
    single: MgFeatureReader

Feature Readers
^^^^^^^^^^^^^^^

A *feature reader*, represented by an ``MgFeatureReader`` object, is used to iterate
through a list of features. Typically, the feature reader is created by selecting
features from a feature source.

To create a feature reader, use the ``MgFeatureService::SelectFeatures()``
method. See :ref:`selecting-with-web-api` details about selection.

To process the features in a feature reader, use the
``MgFeatureReader::ReadNext()`` method. You must call this method before
being able to read the first feature. Continue calling the method to process
the rest of the features.

The ``MgFeatureReader::GetPropertyCount()`` method returns the number of
properties available for the current feature. When you know the name and
type of the feature property, call one of the ``MgFeatureReader::GetPropertyType()``
methods (where PropertyType represents one of the available types) to retrieve the value. 
Otherwise, call ``MgFeatureReader::GetPropertyName()`` and
``MgFeatureReader::GetPropertyType()`` before retrieving the value.

.. index::
    single: MgFeatureReader; Proper Cleanup

.. note::
    You should always call ``Close()`` on your ``MgFeatureReader`` objects (or any object derived from ``MgReader`` for that matter)
    as memory leaks can happen because of unclosed reader objects. Another common pitfall is the ``Close()`` call not being called because
    the code before it threw an exception. For .net and Java, you can use the a ``try...finally`` block
    to ensure your reader will always be closed regardless of whether exceptions have been thrown or not. 

.. index::
    single: MgFeatureReader; Checking Null Values
   
.. note::
    Always check if the property whose value you are trying to retrive is null using the ``IsNull()`` method before attempting to access the property value. 
    Attempting to access a null property value will cause the reader to throw a ``MgNullPropertyValueException``

.. index::
    single: Selections

.. _selecting-with-web-api:

Selecting with the Web API
^^^^^^^^^^^^^^^^^^^^^^^^^^

Selections can be created programatically with the Web API. This is done by
querying data in a feature source, creating a feature reader that contains the
features, then converting the feature reader to a selection (``MgSelection`` object).

To create a feature reader, apply a selection filter to a feature class in the feature
source. A selection filter can be a basic filter, a spatial filter, or a combination
of the two. The filter is stored in an ``MgFeatureQueryOptions`` object.

Basic filters are used to select features based on the values of feature properties.
For example, you could use a basic filter to select all roads that have four or
more lanes.

Spatial filters are used to select features based on their geometry. For example,
you could use a spatial filter to select all roads that intersect a certain area.

.. index::
    single: Filters; Basic Filters
    single: MgFeatureQueryOptions; SetFilter
    
Basic Filters
"""""""""""""

Basic filters perform logical tests of feature properties. You can construct
complex queries by combining expressions. Expressions use the comparison
operators below:

.. index::
    single: Filters; Comparison Operators

+------------+------------------------------------------+
| Operator   | Meaning                                  |
+============+==========================================+
| =          | Equality                                 |
+------------+------------------------------------------+
| <>         | Not Equal                                |
+------------+------------------------------------------+
| <          | Less Than                                |
+------------+------------------------------------------+
| <=         | Less Than or Equal To                    |
+------------+------------------------------------------+
| >          | Greater Than                             |
+------------+------------------------------------------+
| >=         | Greater Than or Equal To                 |
+------------+------------------------------------------+
| ``LIKE``   | Used for string comparisons. The "%"     |
|            | wildcard represents any sequence of 0 or |
|            | more characters. The "_" wildcard        |
|            | represents any single character. For     |
|            | example, ``LIKE 'SCHMITT%'`` will begin  |
|            | search for any names beginning with      |
|            | ``SCHMITT``                              |
+------------+------------------------------------------+

The comparison operators can be used with numeric or string properties,
except for the ``LIKE`` operator, which can only be used with string properties.

Combine or modify expressions with the standard boolean operators ``AND``, ``OR``,
and ``NOT``.

**Examples**

These examples assume that the feature class you are querying has an integer
property named year and a string property named owner. To select all features
newer than 2001, create a filter like this:

**PHP**

.. highlight:: php
.. code-block:: php

    $queryOptions = new MgFeatureQueryOptions();
    $queryOptions->SetFilter('year > 2001');

**.net (C#)**

.. highlight:: csharp
.. code-block:: csharp

    //This code fragment assumes you have imported the OSGeo.MapGuide namespace
    MgFeatureQueryOptions queryOptions = new MgFeatureQueryOptions();
    queryOptions.SetFilter("year > 2001");

**Java**
    
.. highlight:: java
.. code-block:: java

    //This code fragment assumes you have imported the org.osgeo.mapguide namespace
    MgFeatureQueryOptions queryOptions = new MgFeatureQueryOptions();
    queryOptions.SetFilter("year > 2001");

To select all features built between 2001 and 2004, create a filter like this:

**PHP**

.. highlight:: php
.. code-block:: php

    $queryOptions = new MgFeatureQueryOptions();
    $queryOptions->SetFilter('year >= 2001 and year <= 2004');

**.net (C#)**

.. highlight:: csharp
.. code-block:: csharp

    //This code fragment assumes you have imported the OSGeo.MapGuide namespace
    MgFeatureQueryOptions queryOptions = new MgFeatureQueryOptions();
    queryOptions.SetFilter("year >= 2001 and year <= 2004");

**Java**
    
.. highlight:: java
.. code-block:: java

    //This code fragment assumes you have imported the org.osgeo.mapguide namespace
    MgFeatureQueryOptions queryOptions = new MgFeatureQueryOptions();
    queryOptions.SetFilter("year >= 2001 and year <= 2004");

To select all features owned by Davis or Davies, create a filter like this:

**PHP**

.. highlight:: php
.. code-block:: php

    $queryOptions = new MgFeatureQueryOptions();
    $queryOptions->SetFilter("owner LIKE 'Davi%s'");

**.net (C#)**

.. highlight:: csharp
.. code-block:: csharp

    //This code fragment assumes you have imported the OSGeo.MapGuide namespace
    MgFeatureQueryOptions queryOptions = new MgFeatureQueryOptions();
    queryOptions.SetFilter("owner LIKE 'Davi%s'");

**Java**
    
.. highlight:: java
.. code-block:: java

    //This code fragment assumes you have imported the org.osgeo.mapguide namespace
    MgFeatureQueryOptions queryOptions = new MgFeatureQueryOptions();
    queryOptions.SetFilter("owner LIKE 'Davi%s'");

.. index::
    single: Filters; Spatial
    single: MgFeatureQueryOptions; SetSpatialFilter

Spatial Filters
"""""""""""""""

With spatial filters, you can do comparisons using geometric properties. For
example, you can select all features that are inside an area on the map, or that
intersect an area.

.. note::

    For more information about geometry, see :ref:`representations-of-geometry`.

There are two ways of using spatial filters:

 * Create a separate spatial filter to apply to the feature source, using the
   ``MgFeatureQueryOptions::SetSpatialFilter()`` method.
 * Include spatial properties in a basic filter created with the ``MgFeatureQueryOptions::SetFilter()`` method.
 
The ``MgFeatureQueryOptions::SetSpatialFilter()`` method requires an
``MgGeometry`` object to define the geometry and a spatial operation to compare
the feature property and the geometry. The spatial operations are defined in
class ``MgFeatureSpatialOperations``.

.. index::
    single: Filters; Spatial Operations

To include spatial properties in a basic filter, define the geometry using WKT
format. Use the ``GEOMFROMTEXT()`` function in the basic filter, along with one
of the following spatial operations:

 * CONTAINS
 * COVEREDBY
 * CROSSES
 * DISJOINT
 * EQUALS
 * INTERSECTS
 * OVERLAPS
 * TOUCHES
 * WITHIN
 * INSIDE

.. note::

    Not all spatial operations can be used on all features. It depends on the
    capabilities of the FDO provider that supplies the data. This restriction applies to
    separate spatial filters and spatial properties that are used in a basic filter.

.. index::
    single: Features; Creating Geometry Objects
    single: MgAgfReaderWriter

**Creating Geometry Objects from Features**

You may want to use an existing feature as part of a spatial query. To retrieve
the feature's geometry and convert it into an appropriate format for a query,
perform the following steps:

 * Create a query that will select the feature.
 * Query the feature class containing the feature using the
   ``MgFeatureService::SelectFeatures()`` method.
 * Obtain the feature from the query using the ``MgFeatureReader::ReadNext()``
   method.
 * Get the geometry data from the feature using the
   ``MgFeatureReader::GetGeometry()`` method. This data is in AGF binary
   format.
 * Convert the AGF data to an ``MgGeometry`` object using the
   ``MgAgfReaderWriter::Read()`` method.

For example, the following sequence creates an ``MgGeometry`` object representing
the boundaries of District 1 in the Sheboygan sample data.

**PHP**

.. highlight:: php
.. code-block:: php

    $districtQuery = new MgFeatureQueryOptions();
    $districtQuery->SetFilter("Autogenerated_SDF_ID = 1");
    $layer = $map->GetLayers()->GetItem('Districts');
    $featureReader = $layer->SelectFeatures($districtQuery);
    $featureReader->ReadNext();
    $districtGeometryData = $featureReader->GetGeometry('Data');
    $agfReaderWriter = new MgAgfReaderWriter();
    $districtGeometry = $agfReaderWriter->Read($districtGeometryData);

**.net (C#)**

.. highlight:: csharp
.. code-block:: csharp

    //This code fragment assumes you have imported the OSGeo.MapGuide namespace
    MgFeatureQueryOptions districtQuery = new MgFeatureQueryOptions();
    districtQuery.SetFilter("Autogenerated_SDF_ID = 1");
    MgLayer layer = (MgLayer)map.GetLayers().GetItem("Districts");
    MgFeatureReader featureReader = layer.SelectFeatures(districtQuery);
    featureReader.ReadNext();
    MgByteReader districtGeometryData = featureReader.GetGeometry("Data");
    MgAgfReaderWriter agfReaderWriter = new MgAgfReaderWriter();
    MgGeometry districtGeometry = agfReaderWriter.Read(districtGeometryData);

**Java**
    
.. highlight:: java
.. code-block:: java

    //This code fragment assumes you have imported the org.osgeo.mapguide namespace
    MgFeatureQueryOptions districtQuery = new MgFeatureQueryOptions();
    districtQuery.SetFilter("Autogenerated_SDF_ID = 1");
    MgLayer layer = (MgLayer)map.GetLayers().GetItem("Districts");
    MgFeatureReader featureReader = layer.SelectFeatures(districtQuery);
    featureReader.ReadNext();
    MgByteReader districtGeometryData = featureReader.GetGeometry("Data");
    MgAgfReaderWriter agfReaderWriter = new MgAgfReaderWriter();
    MgGeometry districtGeometry = agfReaderWriter.Read(districtGeometryData);

.. index::
    single: MgWktReaderWriter

To convert an ``MgGeometry`` object into its WKT representation, use the ``MgWktReaderWriter::Write()`` method, as in the following example:

**PHP**

.. highlight:: php
.. code-block:: php

    $wktReaderWriter = new MgWktReaderWriter();
    $districtWkt = $wktReaderWriter->Write($districtGeometry);

**.net (C#)**

.. highlight:: csharp
.. code-block:: csharp

    //This code fragment assumes you have imported the OSGeo.MapGuide namespace
    MgWktReaderWriter wktReaderWriter = new MgWktReaderWriter();
    String districtWkt = wktReaderWriter.Write(districtGeometry);

**Java**
    
.. highlight:: java
.. code-block:: java

    //This code fragment assumes you have imported the org.osgeo.mapguide namespace
    MgWktReaderWriter wktReaderWriter = new MgWktReaderWriter();
    String districtWkt = wktReaderWriter.Write(districtGeometry);

**Examples**

The following examples assume that ``$testArea`` is an ``MgGeometry`` object
defining a polygon, and ``$testAreaWkt`` is a WKT description of the polygon.

To create a filter to find all properties owned by ``SCHMITT`` in the area, use
either of the following sequences:

**PHP**

.. highlight:: php
.. code-block:: php

    $queryOptions = new MgFeatureQueryOptions();
    $queryOptions->SetFilter("RNAME LIKE 'SCHMITT%'");
    $queryOptions->SetSpatialFilter('SHPGEOM', $testArea,
    MgFeatureSpatialOperations::Inside);

    $queryOptions = new MgFeatureQueryOptions();
    $queryOptions->SetFilter("RNAME LIKE 'SCHMITT%'
    AND SHPGEOM inside GEOMFROMTEXT('$testAreaWkt')";


**.net (C#)**

.. highlight:: csharp
.. code-block:: csharp

    //These code fragments assumes you have imported the OSGeo.MapGuide namespace
    MgFeatureQueryOptions queryOptions = new MgFeatureQueryOptions();
    queryOptions.SetFilter("RNAME LIKE 'SCHMITT%'");
    queryOptions.SetSpatialFilter("SHPGEOM", testArea, MgFeatureSpatialOperations.Inside);

    MgFeatureQueryOptions queryOptions = new MgFeatureQueryOptions();
    queryOptions.SetFilter("RNAME LIKE 'SCHMITT%' AND SHPGEOM inside GEOMFROMTEXT('" + testAreaWkt + "')";

**Java**
    
.. highlight:: java
.. code-block:: java

    //These code fragments assumes you have imported the org.osgeo.mapguide namespace
    MgFeatureQueryOptions queryOptions = new MgFeatureQueryOptions();
    queryOptions.SetFilter("RNAME LIKE 'SCHMITT%'");
    queryOptions.SetSpatialFilter("SHPGEOM", testArea, MgFeatureSpatialOperations.Inside);

    MgFeatureQueryOptions queryOptions = new MgFeatureQueryOptions();
    queryOptions.SetFilter("RNAME LIKE 'SCHMITT%' AND SHPGEOM inside GEOMFROMTEXT('" + testAreaWkt + "')";


.. index::
    single: Selections; Listing Properties

.. _example-selection:

Example: Selection
^^^^^^^^^^^^^^^^^^

The following example creates a selection, then lists properties from the
selected features. See the Working With Feature Data sample, in the Developer's
Guide samples, for the complete version.

It selects parcels within the boundaries of District 1 that are owned by
``SCHMITT``. This requires a spatial filter and a basic filter.

**PHP**

.. highlight:: php
.. code-block:: php

    $map = new MgMap($siteConnection);
    $map->Open($mapName);
     
    // Get the geometry for the boundaries of District 1
     
    $districtQuery = new MgFeatureQueryOptions();
    $districtQuery->SetFilter("Autogenerated_SDF_ID = 1");
     
    $layer = $map->GetLayers()->GetItem('Districts');
    $featureReader = $layer->SelectFeatures($districtQuery);
    $featureReader->ReadNext();
    $districtGeometryData = $featureReader->GetGeometry('Data');
    $featureReader->Close();
     
    // Convert the AGF binary data to MgGeometry.
     
    $agfReaderWriter = new MgAgfReaderWriter();
    $districtGeometry = $agfReaderWriter->
    Read($districtGeometryData);
     
    // Create a filter to select the desired features.
    // Combine a basic filter and a spatial filter.
     
    $queryOptions = new MgFeatureQueryOptions();
    $queryOptions->SetFilter("RNAME LIKE 'SCHMITT%'");
     
    $queryOptions->SetSpatialFilter('SHPGEOM', $districtGeometry, MgFeatureSpatialOperations::Inside);
     
    // Select the features.
     
    $layer = $map->GetLayers()->GetItem('Parcels');
    $featureReader = $layer->SelectFeatures($queryOptions);
     
    // For each selected feature, display the address.
     
    echo '<p>Properties owned by Schmitt ';
    echo 'in District 1</p><p>';
     
    while ($featureReader->ReadNext())
    {
        $val = $featureReader->GetString('RPROPAD');
        echo $val . '<br />';
    }
    $featureReader->Close();
    echo '</p>';

**.net (C#)**

.. highlight:: csharp
.. code-block:: csharp

    // This code fragment assumes you have imported the OSGeo.MapGuide namespace
    MgMap map = new MgMap(siteConnection);
    map.Open(mapName);
    
    // Get the geometry for the boundaries of District 1
    MgFeatureQueryOptions districtQuery = new MgFeatureQueryOptions();
    districtQuery.SetFilter("Autogenerated_SDF_ID = 1");
    
    MgLayer layer = (MgLayer)map.GetLayers().GetItem("Districts");
    MgFeatureReader featureReader = layer.SelectFeatures(districtQuery);
    featureReader.ReadNext();
    MgByteReader districtGeometryData = featureReader.GetGeometry("Data");
    featureReader.Close();
    
    // Convert the AGF binary data to MgGeometry
    MgAgfReaderWriter agfReaderWriter = new MgAgfReaderWriter();
    MgGeometry districtGeometry = agfReaderWriter.Read(districtGeometryData);
    
    // Create a filter to select the desired features.
    // Combine a basic filter and spatial filter.
    MgFeatureQueryOptions queryOptions = new MgFeatureQueryOptions();
    queryOptions.SetFilter("RNAME LIKE 'SCHMITT%'");
    
    queryOptions.SetSpatialFilter("SHPGEOM", districtGeometry, MgFeatureSpatialOperations.Inside);
    
    // Select the features
    layer = (MgLayer)map.GetLayers().GetItem("Parcels");
    featureReader = layer.SelectFeatures(queryOptions);
    
    // For each selected feature, display the address
    
    Response.Write("<p>Properties owned by Schmitt ");
    Response.Write("in District 1</p><p>");
    
    while (featureReader.ReadNext())
    {
        String val = featureReader.GetString("RPROPAD");
        Response.Write(val + "<br/>");
    }
    featureReader.Close();
    Response.Write("</p>");

**Java**
    
.. highlight:: java
.. code-block:: java

    //This code fragment assumes you have imported the org.osgeo.mapguide namespace
    MgMap map = new MgMap(siteConnection);
    map.Open(mapName);
    
    // Get the geometry for the boundaries of District 1
    MgFeatureQueryOptions districtQuery = new MgFeatureQueryOptions();
    districtQuery.SetFilter("Autogenerated_SDF_ID = 1");
    
    MgLayer layer = (MgLayer)map.GetLayers().GetItem("Districts");
    MgFeatureReader featureReader = layer.SelectFeatures(districtQuery);
    featureReader.ReadNext();
    MgByteReader districtGeometryData = featureReader.GetGeometry("Data");
    featureReader.Close();
    
    // Convert the AGF binary data to MgGeometry
    MgAgfReaderWriter agfReaderWriter = new MgAgfReaderWriter();
    MgGeometry districtGeometry = agfReaderWriter.Read(districtGeometryData);
    
    // Create a filter to select the desired features.
    // Combine a basic filter and spatial filter.
    MgFeatureQueryOptions queryOptions = new MgFeatureQueryOptions();
    queryOptions.SetFilter("RNAME LIKE 'SCHMITT%'");
    
    queryOptions.SetSpatialFilter("SHPGEOM", districtGeometry, MgFeatureSpatialOperations.Inside);
    
    // Select the features
    layer = (MgLayer)map.GetLayers().GetItem("Parcels");
    featureReader = layer.SelectFeatures(queryOptions);
    
    // For each selected feature, display the address
    
    response.getWriter().write("<p>Properties owned by Schmitt ");
    response.getWriter().write("in District 1</p><p>");
    
    while (featureReader.ReadNext())
    {
        String val = featureReader.GetString("RPROPAD");
        response.getWriter().write(val + "<br/>");
    }
    featureReader.Close();
    response.getWriter().write("</p>");

.. index::
    single: Active Selection; About

Active Selections
-----------------

A map may have an active selection, which is a list of features on the map
that have been selected and highlighted in the Viewer. The active selection
is part of the run-time map state, and is not stored with the map resource in
the repository.

The most direct method for creating an active selection is to use the interactive
selection tools in the Viewer. Applications can also create selections using the
Web API and apply them to a user's view of the map.

.. note::

    In the AJAX Viewer, any changes to the active selection require re-generation
    of the map image. Because of this, the Web server keeps information about
    the selection.

.. index::
    single: Selections; Selecting with the viewer

Selecting with the Viewer
^^^^^^^^^^^^^^^^^^^^^^^^^

In order for a feature to be selectable using the Viewer, the following criteria
must be met:

 * The layer containing the feature must be visible at the current map view scale.
 * The selectable property for the layer must be true. Change this property in the web layout or with the ``MgLayer::SetSelectable()`` method.
 
There are different selection tools available in the Viewer. They can be enabled
or disabled as part of the web layout. Each tool allows a user to select one or
more features on the map.

.. index::
    single: AJAX Viewer; Passing viewer information

Passing Viewer Information to the Web Server
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. todo::
    Talk about some parameters that are automatically passed for Invoke URL commands

The stateless nature of HTTP means that when the Viewer makes a request to a MapGuide Server Page
on the Web Server, it must often pass information as part of the request. Some
common methods for passing this information are:

 * as an additional parameter to an Invoke URL command in a web layout
 * through an Invoke Script command that executes the ``Submit`` method of the hidden ``formFrame``
 * through an ``onClick`` or other event that executes the ``Submit`` method of the hidden ``formFrame``
 
The best method to use depends on the requirements of the application. If
you are invoking the request from a command defined in a web layout, you
can pass the information either as an additional parameter to an Invoke URL
command or through an Invoke Script command. Invoke URL is simpler, but
it offers a restricted set of parameters. Invoke Script has complete access to all
the JavaScript calls in the Viewer API.

If you are invoking the request from a page in the task pane, you can execute
JavaScript as part of an ``onClick`` event or a form action.

.. index::
    single: Invoke URL Commands; Additional Parameters
    
Additional Parameters to an Invoke URL Command
""""""""""""""""""""""""""""""""""""""""""""""

With this release of MapGuide, the current selection is the only variable that
can be passed as part of an Invoke URL command.

To pass the current selection, edit the web layout. Define a new Invoke URL
command. On the Additional Parameters tab, enter a key and value. The key
must be a valid HTTP POST key. For the value, enter ``$CurrentSelection``. Add
the command to the toolbar, context menu, or task bar menu.

When the command is executed, the current selection is passed to the page,
along with the standard variables like ``SESSION`` and ``MAPNAME``.

For example, if you define the key ``SEL`` to have the value ``$CurrentSelection``,
then when the URL is invoked

**PHP**

.. highlight:: php
.. code-block:: php

    $selection = $_POST['SEL'];

**.net (C#)**

.. highlight:: csharp
.. code-block:: csharp

    String selection = Request.Form["SEL"];

**Java**
    
.. highlight:: java
.. code-block:: java

    String selection = request.getParameter("SEL");

gets the current selection, in XML format.

See :ref:`working-with-the-active-selection` for details about using the
XML data.

.. note::

    The AJAX automatically adds the following parameters for any Invoke URL command:
    
     * ``SESSION`` - The current session id
     * ``MAPNAME`` - The name of the current map. Use this for the ``MgMap::Open()`` method

.. index::
    single: Invoke Script Commands; Passing Parameters

Passing Parameters from an Invoke Script Command
""""""""""""""""""""""""""""""""""""""""""""""""

An Invoke Script command in a web layout can be used to pass custom
parameters to a page. The parameters can be any values that are available via
the Viewer API.

To pass parameters, edit the web layout. Define a new Invoke Script command.
On the Additional Parameters tab, enter the JavaScript code to retrieve the
values to be passed. Add the command to the toolbar, context menu, or task
bar menu.

The JavaScript code can call Viewer API functions or other functions to retrieve
values. To pass the parameters to a page, call the ``Submit`` method of the
``formFrame`` with the parameters, the page URL, and the name of the target
frame. Use ``taskPaneFrame`` or ``scriptFrame`` as the target frame, depending
whether the loaded page should be visible or not.

.. note::
    
    The parameters must include standard parameters like ``SESSION`` and ``MAPNAME``, if they are needed.

.. index::
    single: Task Pane; Passing Parameters

Passing Parameters from the Task Pane frame
"""""""""""""""""""""""""""""""""""""""""""

Passing parameters from the task pane frame is similar to passing them from
an Invoke Script command. Use the Viewer API to retrieve values and call the
``Submit`` method of the ``formFrame`` to pass the values to another page.

For example, the following function passes the map view scale and the center
point as parameters to a page that opens in a new window.

.. highlight:: javascript
.. code-block:: javascript

    function submitRequest(pageUrl)
    {
        xmlSel = parent.parent.mapFrame.GetSelectionXML();
        mapScale = parent.parent.mapFrame.GetScale();
        mapCenter = parent.parent.mapFrame.GetCenter();
        params = new Array(
            "SESSION", parent.parent.mapFrame.GetSessionId(),
            "MAPNAME", parent.parent.mapFrame.GetMapName(),
            "SELECTION", xmlSel,
            "SCALE", mapScale,
            "CENTERX", mapCenter.X,
            "CENTERY", mapCenter.Y
        );
        parent.parent.formFrame.Submit(pageUrl, params, "_blank");
    }

To call the function, execute it as part of an onClick event or as the action in
a form. For example, clicking the following link would execute the function:

.. highlight:: html
.. code-block:: html

    <a href="#" onClick="submitRequest('/mapguide/devguide/custom_output/property_report.php'); return false;">Click for report</a>

.. index::
    single: Active Selection; Manipulating the Active Selection

.. _working-with-the-active-selection:

Working with the Active Selection
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Whenever a selection is changed by the Viewer, the selection information is sent to the web server so the map can be re-generated.

To retrieve and manipulate the active selection for a map (AJAX Viewer only):

 1. Create an ``MgSelection`` object for the map. Initialize it to the active selection.
 2. Retrieve selected layers from the ``MgSelection`` object.
 3. For each layer, retrieve selected feature classes. There will normally be one feature class for the layer, so you can use the ``MgSelection::GetClass()`` method instead of the ``MgSelection::GetClasses()`` method.
 4. Call ``MgSelection::GenerateFilter()`` to create a selection filter that contains the selected features in the class.
 5. Call ``MgFeatureService::SelectFeatures()`` to create an MgFeatureReader object for the selected features.
 6. Process the ``MgFeatureReader`` object, retrieving each selected feature. 
 
.. index::
    single: Active Selection; Listing Selected Features
 
Example: Listing Selected Parcels
"""""""""""""""""""""""""""""""""

The steps for listing the selected parcels for the DWF Viewer are nearly the
same as for the AJAX Viewer. The major difference is you must pass the
selection information from the Viewer to your page.

One method to do this is to create a JavaScript function, then call this function
from the Viewer using an Invoke Script command or as a result of an onClick
event in the task pane. For example, the task pane of the Working With Feature
Data sample contains a JavaScript function executed by an onClick event.

.. todo::
    We removed the DWF sample and simplified the AJAX one. Make sure the actual devguide sample if it exists matches this

.. highlight:: javascript
.. code-block:: javascript

    function listSelection()
    {
        xmlSel = parent.parent.mapFrame.GetSelectionXML();
        params = new Array(
            "SESSION",parent.parent.mapFrame.GetSessionId(),
            "MAPNAME", parent.parent.mapFrame.GetMapName());
        pageUrl = "/mapguide/samplesphp/working_with_feature_data/listselection.php";
        parent.parent.formFrame.Submit(pageUrl, params, "taskPaneFrame");
    }

This submits a request to listselection.php, which contains the following:

.. todo::
    The APIs have been simplified since this was last written. This sample has been updated to use conveience APIs introduced with RFC33. Verify this checks out

**PHP (listselection.php)**

.. highlight:: php
.. code-block:: php

    $map = new MgMap($siteConnection);
    $map->Open($mapName);

    $selection = new MgSelection($map);
    $layers = $selection->GetLayers();

    if ($layers && $layers->GetCount() > 0)
    {
        for ($i = 0; $i < $layers->GetCount(); $i++)
        {
            $layer = $layers->GetItem($i);

            if ($layer && $layer->GetName() == 'Parcels')
            {
                $featureReader = $selection->GetSelectedFeatures($layer, $layer->GetFeatureClassName(), false);
                // Process each item in the MgFeatureReader,
                // displaying the owner name
                while ($featureReader->ReadNext())
                {
                    $val = $featureReader->GetString('NAME') .
                    '<br />&nbsp;&nbsp;' .
                    $featureReader->GetString('RPROPAD');
                    echo $val . '<br />';
                }
                $featureReader->Close();
            }
        }
    }
    else
    {
        echo 'No selected layers';
    }

**.net (C#)**

.. highlight:: csharp
.. code-block:: csharp

    //This code fragment assumes you have imported the OSGeo.MapGuide namespace
    MgMap map = new MgMap(siteConnetion);
    map.Open(mapName);
    
    MgSelection selection = new MgSelection(map);
    selection.Open(mapName);
    MgLayerCollection layers = selection.GetLayers();
    
    if (layers != null && layers.GetCount() > 0)
    {
        for (int i = 0; i < layers.GetCount(); i++)
        {
            MgLayer layer = (MgLayer)layers.GetItem(i);
            if (layer.GetName() == "Parcels")
            {
                MgFeatureReader featureReader = selection.GetSelectedFeatures(layer, layer.GetFeatureClassName(), false);
                while (featureReader.ReadNext())
                {
                    String val = featureReader.GetString("NAME") + "<br/>&nbsp;&nbsp;" + featureReader.GetString("RPROPAD");
                    Response.Write(val + "<br/>");
                }
                featureReader.Close();
            }
        }
    }
    else
    {
        Response.Write("No selected layers");
    }

**Java**
    
.. highlight:: java
.. code-block:: java

    //This code fragment assumes you have imported the org.osgeo.mapguide namespace
    MgMap map = new MgMap(siteConnetion);
    map.Open(mapName);
    
    MgSelection selection = new MgSelection(map);
    selection.Open(mapName);
    MgLayerCollection layers = selection.GetLayers();
    
    if (layers != null && layers.GetCount() > 0)
    {
        for (int i = 0; i < layers.GetCount(); i++)
        {
            MgLayer layer = (MgLayer)layers.GetItem(i);
            if (layer.GetName() == "Parcels")
            {
                MgFeatureReader featureReader = selection.GetSelectedFeatures(layer, layer.GetFeatureClassName(), false);
                while (featureReader.ReadNext())
                {
                    String val = featureReader.GetString("NAME") + "<br/>&nbsp;&nbsp;" + featureReader.GetString("RPROPAD");
                    response.getWriter().write(val + "<br/>");
                }
                featureReader.Close();
            }
        }
    }
    else
    {
        response.getWriter().write("No selected layers");
    }

.. index::
    single: Active Selection; Setting with Web API

Setting the Active Selection with the Web API
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To set the run-time map selection using a query, perform the following steps:

 * Create a selection as described in `selecting-with-the-web-api`. This creates a feature reader containing the selected features.
 * Create an ``MgSelection`` object to hold the features in the feature reader.
 * Send the selection to the Viewer, along with a call to the Viewer API function ``SetSelectionXML()``.

Example: Setting the Active Selection
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The following example combines the pieces needed to create a selection using
the Web API and pass it back to the Viewer where it becomes the active
selection for the map. It is an extension of the example shown :ref:`example-selection`.

The PHP code in this example creates the selection XML. Following that is a
JavaScript function that calls the ``SetSelectionXML()`` function with the
selection. This function is executed when the page loads.

**PHP**

.. highlight:: php
.. code-block:: php

    <body class="AppFrame" onLoad="OnPageLoad()">
     
        <h1 class="AppHeading">Select features</h1>

        <?php
        include '../common/common.php';

        $args = ($_SERVER['REQUEST_METHOD'] == "POST")? $_POST : $_GET;
        $sessionId = $args['SESSION'];
        $mapName = $args['MAPNAME'];

        try
        {

            // Initialize the Web Extensions and connect to the Server
            // using the Web Extensions session identifier

            MgInitializeWebTier ($webconfigFilePath);

            $userInfo = new MgUserInformation($sessionId);
            $siteConnection = new MgSiteConnection();
            $siteConnection->Open($userInfo);

            $map = new MgMap($siteConnection);
            $map->Open($mapName);

            // Get the geometry for the boundaries of District 1

            $districtQuery = new MgFeatureQueryOptions();
            $districtQuery->SetFilter("Autogenerated_SDF_ID = 1");

            $layer = $map->GetLayers()->GetItem('Districts');
            $featureReader = $layer->SelectFeatures($districtQuery);
            $featureReader->ReadNext();
            $districtGeometryData = $featureReader->GetGeometry('Data');

            // Convert the AGF binary data to MgGeometry.

            $agfReaderWriter = new MgAgfReaderWriter();
            $districtGeometry = $agfReaderWriter->Read($districtGeometryData);
            // Create a filter to select the desired features. Combine
            // a basic filter and a spatial filter.
            $queryOptions = new MgFeatureQueryOptions();
            $queryOptions->SetFilter("RNAME LIKE 'SCHMITT%'");
            $queryOptions->SetSpatialFilter('SHPGEOM', $districtGeometry, MgFeatureSpatialOperations::Inside);
            // Get the features from the feature source,
            // turn it into a selection, then save the selection as XML.
            $layer = $map->GetLayers()->GetItem('Parcels');
            $featureReader = $layer->SelectFeatures($queryOptions);
            $layer = $map->GetLayers()->GetItem('Parcels');
            $selection = new MgSelection($map);
            $selection->AddFeatures($layer, $featureReader, 0);
            $selectionXml = $selection->ToXml();
            echo 'Selecting parcels owned by Schmitt in District&nbsp;1';
        }
        catch (MgException $e)
        {
            echo $e->GetExceptionMessage();
            echo $e->GetDetails();
        }
        ?>
    </body>
    <script language="javascript">
        // Emit this function and assocate it with the onLoad event
        // for the page so that it gets executed when this page
        // loads in the browser. The function calls the
        // SetSelectionXML method on the Viewer Frame, which updates
        // the current selection on the viewer and the server.
        function OnPageLoad()
        {
            selectionXml = '<?php echo $selectionXml; ?>';
            parent.parent.SetSelectionXML(selectionXml);
        }
    </script>

**.net (C#)**

.. highlight:: csharp
.. code-block:: csharp

    <!-- This code fragment assumes you have imported the OSGeo.MapGuide namespace -->
    <body class="AppFrame" onLoad="OnPageLoad()">
     
        <h1 class="AppHeading">Select features</h1>

    <%

    sessionId = Request.Params["SESSION"];
    mapName = Request.Params["MAPNAME"];

    try
    {

      // Initialize the Web Extensions and connect to the Server using
      // the Web Extensions session identifier stored in PHP session state.

      MapGuideApi.MgInitializeWebTier (Constants.WebConfigPath);

      MgUserInformation userInfo = new MgUserInformation(sessionId);
      MgSiteConnection siteConnection = new MgSiteConnection();
      siteConnection.Open(userInfo);

      MgMap map = new MgMap(siteConnection);
      map.Open(mapName);

      // Get the geometry for the boundaries of District 1

      MgFeatureQueryOptions districtQuery = new MgFeatureQueryOptions();
      districtQuery.SetFilter("Autogenerated_SDF_ID = 1");

      MgLayerBase layer = map.GetLayers().GetItem("Districts");
      MgFeatureReader featureReader = layer.SelectFeatures(districtQuery);
      MgByteReader districtGeometryData = null;
      try {
        featureReader.ReadNext();
        districtGeometryData = featureReader.GetGeometry("Data");
      } finally { //Ensures the reader is closed regardless
        featureReader.Close();
      }

      // Convert the AGF binary data to MgGeometry.

      MgAgfReaderWriter agfReaderWriter = new MgAgfReaderWriter();
      MgGeometry districtGeometry = agfReaderWriter.Read(districtGeometryData);

      // Create a filter to select the desired features. Combine
      // a basic filter and a spatial filter.

      MgFeatureQueryOptions queryOptions = new MgFeatureQueryOptions();
      queryOptions.SetFilter("RNAME LIKE 'SCHMITT%'");
      queryOptions.SetSpatialFilter("SHPGEOM", districtGeometry, MgFeatureSpatialOperations.Inside);

      // Get the features from the feature source,
      // turn it into a selection, then save the selection as XML.

      layer = map.GetLayers().GetItem("Parcels");
      featureReader = layer.SelectFeatures(queryOptions);

      MgSelection selection = new MgSelection(map);
      selection.AddFeatures(layer, featureReader, 0);
      selectionXml = selection.ToXml();

      Response.Write("Selecting parcels owned by Schmitt in District&nbsp;1");
    }
    catch (MgException e)
    {
      Response.Write(e.GetExceptionMessage());
      Response.Write(e.GetDetails());
    }
    %>

  </body>

  <script language="javascript">

    <!-- Emit this function and assocate it with the onLoad event for the -.
    <!-- page so that it gets executed when this page loads in the        -.
    <!-- browser. The function calls the SetSelectionXML method on the    -.
    <!-- Viewer Frame, which updates the current selection on the viewer  -.
    <!-- and the server.                                                  -.

    function OnPageLoad()
    {
      selectionXml = '<%= selectionXml %>';
      parent.parent.SetSelectionXML(selectionXml);
    }

  </script>

**Java**
    
.. highlight:: java
.. code-block:: java

    //This code fragment assumes you have imported the org.osgeo.mapguide namespace
    
    //Code sample currently not available
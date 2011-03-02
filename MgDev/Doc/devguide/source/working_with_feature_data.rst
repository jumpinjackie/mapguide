.. index::
   single: feature; data

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


Querying Feature Data
---------------------

In order to work with feature data, you must first select the features you are
interested in. This can be done with the Viewer or through Web API calls.

Feature Readers
^^^^^^^^^^^^^^^

.. todo::
    Update page number reference with section link

A *feature reader*, represented by an ``MgFeatureReader`` object, is used to iterate
through a list of features. Typically, the feature reader is created by selecting
features from a feature source.

To create a feature reader, use the ``MgFeatureService::SelectFeatures()``
method. See Selecting with the Web API on page 39 for details about selection.

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

.. note::
    You should always call ``Close()`` on your ``MgFeatureReader`` objects (or any object derived from ``MgReader`` for that matter)
    as memory leaks can happen because of unclosed reader objects. Another common pitfall is the ``Close()`` call not being called because
    the code before it threw an exception. For .net and Java, you can use the a ``try...finally`` block
    to ensure your reader will always be closed regardless of whether exceptions have been thrown or not. 
    
.. note::
    Always check if the property whose value you are trying to retrive is null using the ``IsNull()`` method before attempting to access the property value. 
    Attempting to access a null property value will cause the reader to throw a ``MgNullPropertyValueException``

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

Basic Filters
"""""""""""""

Basic filters perform logical tests of feature properties. You can construct
complex queries by combining expressions. Expressions use the comparison
operators below:

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

    MgFeatureQueryOptions queryOptions = new MgFeatureQueryOptions();
    queryOptions.SetFilter("year > 2001");

**Java**
    
.. highlight:: java
.. code-block:: java

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

    MgFeatureQueryOptions queryOptions = new MgFeatureQueryOptions();
    queryOptions.SetFilter("year >= 2001 and year <= 2004");

**Java**
    
.. highlight:: java
.. code-block:: java

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

    MgFeatureQueryOptions queryOptions = new MgFeatureQueryOptions();
    queryOptions.SetFilter("owner LIKE 'Davi%s'");

**Java**
    
.. highlight:: java
.. code-block:: java

    MgFeatureQueryOptions queryOptions = new MgFeatureQueryOptions();
    queryOptions.SetFilter("owner LIKE 'Davi%s'");

Spatial Filters
"""""""""""""""

.. todo::
    Update page number reference with section link

With spatial filters, you can do comparisons using geometric properties. For
example, you can select all features that are inside an area on the map, or that
intersect an area.

.. note::

    For more information about geometry, see Representation of Geometry on page 73.

There are two ways of using spatial filters:

 * Create a separate spatial filter to apply to the feature source, using the
   ``MgFeatureQueryOptions::SetSpatialFilter()`` method.
 * Include spatial properties in a basic filter created with the ``MgFeatureQueryOptions::SetFilter()`` method.
 
The ``MgFeatureQueryOptions::SetSpatialFilter()`` method requires an
``MgGeometry`` object to define the geometry and a spatial operation to compare
the feature property and the geometry. The spatial operations are defined in
class ``MgFeatureSpatialOperations``.

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

Example: Selection
^^^^^^^^^^^^^^^^^^

**PHP**

.. highlight:: php
.. code-block:: php

**.net (C#)**

.. highlight:: csharp
.. code-block:: csharp

**Java**
    
.. highlight:: java
.. code-block:: java



Active Selections
-----------------

Selecting with the Viewer
^^^^^^^^^^^^^^^^^^^^^^^^^

Passing Viewer Information to the Web Server
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Additional Parameters to an Invoke URL Command
""""""""""""""""""""""""""""""""""""""""""""""

Passing Parameters from an Invoke Script Command
""""""""""""""""""""""""""""""""""""""""""""""""

Passing Parameters from the Task Pane frame
"""""""""""""""""""""""""""""""""""""""""""

Working with the Active Selection
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Example: Listing Selected Parcels
"""""""""""""""""""""""""""""""""

Setting the Active Selection with the Web API
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Example: Setting the Active Selection
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
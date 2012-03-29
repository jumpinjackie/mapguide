Analyzing Features
==================

Introduction
------------

.. note::

    The Analyzing Features sample, in the Developer's Guide samples, demonstrates concepts from this chapter.

MapGuide includes methods for analyzing map features, including comparing
the spatial relationships between features, measuring distances, and creating
buffer areas around features.

Analyzing features requires knowing how the features are represented and what
spatial reference systems are being used. If different spatial reference systems
are being used, it is important to be able to convert between them.

.. index::
    single: Geometry: Representations

.. _representations-of-geometry:

Representations of Geometry
---------------------------

MapGuide can represent geometric data in three different forms:

 * AGF text format, which is an extension of the Open Geospatial Consortium (OGC) Well Known Text (WKT) format. This is used to represent geometry as a character string.
 * Binary AGF format. This is used by the FDO technology supporting the Feature Service.
 * MapGuide internal representation, using ``MgGeometry`` and classes derived from it.

.. note::

    This guide and the Web API Reference will often use the term WKT to mean
    AGF text format. Be aware that AGF Text values do not always conform to the
    OGC WKT standard. See the Geometry module in the Web API Reference for
    details.
    
To convert between AGF text and the MapGuide internal representation, use
an ``MgWktReaderWriter`` object. Call ``MgWktReaderWriter.Read()`` to convert AGF
text to ``MgGeometry``. Call ``MgWktReaderWriter.Write()`` to convert ``MgGeometry``
to AGF text.

To convert between binary AGF and the MapGuide internal representation,
use an ``MgAgfReaderWriter`` object. Call ``MgAgfReaderWriter.Read()`` to convert
binary AGF to ``MgGeometry``. Call ``MgAgfReaderWriter.Write()`` to convert
``MgGeometry`` to binary AGF.

For example, if you have a WKT representation of the geometry, you could
create a geometry object as follows:

**PHP**

.. highlight:: php
.. code-block:: php

    $wktReaderWriter = new MgWktReaderWriter();
    $geometry = wktReaderWriter->Read($wktGeometry);

**.net (C#)**

.. highlight:: csharp
.. code-block:: csharp

    //This code fragment assumes you have imported the OSGeo.MapGuide namespace
    MgWktReaderWriter wktReaderWriter = new MgWktReaderWriter();
    MgGeometry geometry = wktReaderWriter.Read(wktGeometry);

**Java**
    
.. highlight:: java
.. code-block:: java

    //This code fragment assumes you have imported the org.osgeo.mapguide namespace
    MgWktReaderWriter wktReaderWriter = new MgWktReaderWriter();
    MgGeometry geometry = wktReaderWriter.Read(wktGeometry);

.. index::
    single: MgGeometry

Geometry Objects
^^^^^^^^^^^^^^^^

``MgGeometry`` is the base class for all the geometry types. The simple geometry
types are:

 * ``MgPoint`` - a single point
 * ``MgLineString`` - a series of connected line segments
 * ``MgCurveString`` - a series of connected curve segments
 * ``MgPolygon`` - a polygon with sides formed from line segments
 * ``MgCurvePolygon`` - a polygon with sides formed from curve segments

The curve segments are circular arcs, defined by a start point, an end point,
and a control point.

Complex types are formed by aggregating simple types. The complex types
are:

 * ``MgMultiPoint`` - a group of points
 * ``MgMultiLineString`` - a group of line strings
 * ``MgMultiCurveString`` - a group of curve strings
 * ``MgMultiPolygon`` - a group of polygons
 * ``MgMultiCurvePolygon`` - a group of curve polygons
 * ``MgMultiGeometry`` - a group of simple geometry objects of any type

.. index::
    single: Geometry; Comparison with other Geometries

Comparing Geometry Objects
^^^^^^^^^^^^^^^^^^^^^^^^^^

The ``MgGeometry`` class contains methods for comparing different geometry
objects. These are similar to the spatial filters described in :ref:`selecting-with-web-api`. Methods to test spatial relationships include:

 * ``Contains()``
 * ``Crosses()``
 * ``Disjoint()``
 * ``Equals()``
 * ``Intersects()``
 * ``Overlaps()``
 * ``Touches()``
 * ``Within()``

For example, if you have an ``MgLineString`` object ``$line`` and an ``MgPolygon``
object ``$polygon``, you can test if the line crosses the polygon with a call to

**PHP**

.. highlight:: php
.. code-block:: php

    $line->Crosses($polygon)

**.net (C#)**

.. highlight:: csharp
.. code-block:: csharp

    line.Crosses(polygon)

**Java**
    
.. highlight:: java
.. code-block:: java

    line.Crosses(polygon)

Methods to create new geometry objects from the point set of two other
geometries include:

 * ``Difference()``
 * ``Intersection()``
 * ``SymmetricDifference()``
 * ``Union()``

Complete details are in the Geometry module of the Web API reference, under
Spatial Relationships.

.. index::
    single: Coordinate Systems
    single: Coordinate Systems; Creating from MgMap
    single: MgCoordinateSystem

Coordinate Systems
------------------

A single map will often combine data from different sources, and the different
sources may use different coordinate systems. The map has its own coordinate
system, and any feature sources used in the map may have different coordinate
systems. It is important for display and analysis that all locations are
transformed to the same coordinate system.

.. note::

    A coordinate system can also be called a spatial reference system (SRS) or
    a coordinate reference system (CRS). This guide uses the abbreviation SRS.

MapGuide supports three different types of coordinate system:

 * Arbitrary X-Y
 * Geographic, or latitude/longitude
 * Projected

An ``MgCoordinateSystem`` object represents a coordinate system.

.. note::

    You cannot transform between arbitrary X-Y coordinates and either
    geographic or projected coordinates.
    
To create an ``MgCoordinateSystem`` object from an ``MgMap`` object,

 * Get the WKT representation of the map coordinate system, using ``MgMap::GetMapSRS()``.
 * Create an ``MgCoordinateSystem`` object, using ``MgCoordinateSystemFactory::Create()``.

To create an ``MgCoordinateSystem`` object from a map layer,

 * Get the feature source for the layer.
 * Get the active spatial context for the feature source.
 * Convert the spatial context to a WKT.
 * Create an ``MgCoordinateSystem`` object from the WKT.

To transform geometry from one coordinate system to another, create an
``MgCoordinateSystemTransform`` object using the two coordinate systems. Apply
this transform to the ``MgGeometry`` object.

For example, if you have geometry representing a feature on a layer that uses
one coordinate system, and you want to compare it to a feature on another
layer that uses a different coordinate system, perform the following steps:

**PHP**

.. highlight:: php
.. code-block:: php

    $featureSource1 = $layer1->GetFeatureSourceId();
    $contexts1 = $featureService->GetSpatialContexts($featureSource1, true);
    $contexts1->ReadNext();
    $srs1 = $contexts1->GetCoordinateSystemWkt();
    $contexts1->Close();
    $featureSource2 = $layer2->GetFeatureSourceId();
    $contexts2 = $featureService->GetSpatialContexts($featureSource2, true);
    $contexts2->ReadNext();
    $srs2 = $contexts2->GetCoordinateSystemWkt();
    $contexts2->Close();
    $csFactory = new MgCoordinateSystemFactory();
    $srcCs = $csFactory->Create($srs1);
    $dstCs = $csFactory->Create($srs2);
    $xform = $csFactory->GetTransform($srcCS, $dstCs);
    $geometry1xform = $geometry1->Transform($xform);

**.net (C#)**

.. highlight:: csharp
.. code-block:: csharp

    //This code fragment assumes you have imported the OSGeo.MapGuide namespace
    MgResourceIdentifier featureSource1 = layer1.GetFeatureSourceId();
    MgSpatialContextReader contexts1 = featureService.GetSpatialContexts(featureSource1, true);
    contexts1.ReadNext();
    String srs1 = contexts1.GetCoordinateSystemWkt();
    contexts1.Close();
    MgResourceIdentifier featureSource2 = layer2.GetFeatureSourceId();
    MgSpatialContextReader contexts2 = featureService.GetSpatialContexts(featureSource2, true);
    contexts2.ReadNext();
    String srs2 = contexts2.GetCoordinateSystemWkt();
    contexts2.Close();
    MgCoordinateSystemFactory csFactory = new MgCoordinateSystemFactory();
    MgCoordinateSystem srcCs = csFactory.Create(srs1);
    MgCoordinateSystem dstCs = csFactory.Create(srs2);
    MgCoordianteSystemTransform xform = csFactory.GetTransform(srcCs, dstCs);
    MgGeometry geometry1xform = geometry1.Transform(xform);

**Java**
    
.. highlight:: java
.. code-block:: java

    //This code fragment assumes you have imported the org.osgeo.mapguide namespace
    MgResourceIdentifier featureSource1 = layer1.GetFeatureSourceId();
    MgSpatialContextReader contexts1 = featureService.GetSpatialContexts(featureSource1, true);
    contexts1.ReadNext();
    String srs1 = contexts1.GetCoordinateSystemWkt();
    contexts1.Close();
    MgResourceIdentifier featureSource2 = layer2.GetFeatureSourceId();
    MgSpatialContextReader contexts2 = featureService.GetSpatialContexts(featureSource2, true);
    contexts2.ReadNext();
    String srs2 = contexts2.GetCoordinateSystemWkt();
    contexts2.Close();
    MgCoordinateSystemFactory csFactory = new MgCoordinateSystemFactory();
    MgCoordinateSystem srcCs = csFactory.Create(srs1);
    MgCoordinateSystem dstCs = csFactory.Create(srs2);
    MgCoordianteSystemTransform xform = csFactory.GetTransform(srcCs, dstCs);
    MgGeometry geometry1xform = geometry1.Transform(xform);

.. index::
    single: Coordinate Systems; Measure Units
    single: Measuring Distance

Measuring Distance
------------------

Measuring distance in geographic or projected coordinate systems requires
great circle calculations. Both ``MgGeometry::Buffer()`` and
``MgGeometry::Distance()`` accept a measurement parameter that defines the
great circle to be used. If the measurement parameter is null, the calculation
is done using a linear algorithm.

Create the measurement parameter, an ``MgCoordinateSystemMeasure`` object,
from the ``MgCoordinateSystem`` object.

Distance is calculated in the units of the SRS. ``MgCoordinateSystem`` includes
two methods, ``ConvertCoordinateSystemUnitsToMeters()`` and
``ConvertMetersToCoordinateSystemUnits()`` to convert to and from linear
distances.

For example, to calculate the distance between two ``MgGeometry`` objects ``$a`` and
``$b``, using the coordinate system ``$srs``, perform the following steps:

**PHP**

.. highlight:: php
.. code-block:: php

    $measure = $srs->GetMeasure();
    $distInMapUnits = $a->Distance($b, $measure);
    $distInMeters = $srs->ConvertCoordinateSystemUnitsToMeters($distInMapUnits);

**.net (C#)**

.. highlight:: csharp
.. code-block:: csharp

    //This code fragment assumes you have imported the OSGeo.MapGuide namespace
    MgCoordinateSystemMeasure measure = srs.GetMeasure();
    double distInMapUnits = a.Distance(b, measure);
    double distInMeters = srs.ConvertCoordinateSystemUnitsToMeters(distInMapUnits);

**Java**
    
.. highlight:: java
.. code-block:: java

    //This code fragment assumes you have imported the org.osgeo.mapguide namespace
    MgCoordinateSystemMeasure measure = srs.GetMeasure();
    double distInMapUnits = a.Distance(b, measure);
    double distInMeters = srs.ConvertCoordinateSystemUnitsToMeters(distInMapUnits);

Another way to calculate the distance is to use ``MgCoordinateSystemMeasure::GetDistance()``, as in the following:

**PHP**

.. highlight:: php
.. code-block:: php

    $distInMapUnits = $measure->GetDistance($a, $b);

**.net (C#)**

.. highlight:: csharp
.. code-block:: csharp

    double distInMapUnits = measure.GetDistance(a, b);

**Java**
    
.. highlight:: java
.. code-block:: java

    double distInMapUnits = measure.GetDistance(a, b);

.. index::
    single: Feature Source; Temporary Feature Sources
    single: MgClassDefinition
    single: MgFeatureSchema
    single: MgClassDefinition; Properties
    single: MgDataPropertyDefinition
    single: MgGeometryPropertyDefinition
    single: MgObjectPropertyDefinition
    single: MgRasterPropertyDefinition
    
Temporary Feature Sources
-------------------------

Many geometric analysis operations require creating new features and new
feature sources. For example, drawing a buffer around a point on a map requires
a layer to display the buffer polygon, and the layer requires a feature source.

To create a temporary feature source, perform the following steps:

 * Create a feature class definition.
 * Determine what properties you need to store for the features. Add the property definitions to the feature class definition.
 * Create a feature schema containing the feature class definition.
 * Determine the SRS for the feature source. This can be the same as the SRS used for the map.
 * Create a feature source using the schema and the SRS. The feature source can be stored in the session repository.

It is possible for a single feature source to contain more than one feature class.
A feature source that is to be used for temporary data, however, normally
contains one feature class.

A feature schema (``MgFeatureSchema`` object) contains class definitions
(``MgClassDefinition`` objects) for each feature class in the schema.

Each class definition contains property definitions for each property in the
feature class. The property definitions can be the following types:

 * ``MgDataPropertyDefinition``
 * ``MgGeometryPropertyDefinition``
 * ``MgObjectPropertyDefinition``
 * ``MgRasterPropertyDefinition``
 
.. note::

    Unlike FDO, MapGuide does not currently support Association Properties

``MgDataPropertyDefinition`` is used to define simple properties like numbers
or strings. ``MgGeometryPropertyDefinition`` is used to define geometric
properties. Most feature classes will have a geometric property to describe the
feature's location.

For example, the following creates a temporary feature source to hold buffer
features. The feature source contains a single feature class named ``BufferClass``.

Features in ``BufferClass`` have two properties. ``ID`` is an autogenerated unique
ID number, and ``BufferGeometry`` contains the geometry for the buffer polygon.

The FDO technology supporting the Feature Service allows for multiple spatial
reference systems within a single feature source. However, this capability is
dependent on the data provider, and does not apply to the SDF provider that
is used for creating feature sources within MapGuide. For temporary feature
sources, you must define a single default SRS for the feature source, and you
must set any geometry properties to use the same SRS. The name of the SRS
is user-defined.

**PHP**

.. highlight:: php
.. code-block:: php

    $bufferClass = new MgClassDefinition();
    $bufferClass->SetName('BufferClass');
    $properties = $bufferClass->GetProperties();
     
    $idProperty = new MgDataPropertyDefinition('ID');
    $idProperty->SetDataType(MgPropertyType::Int32);
    $idProperty->SetReadOnly(true);
    $idProperty->SetNullable(false);
    $idProperty->SetAutoGeneration(true);
    $properties->Add($idProperty);
     
    $polygonProperty = new MgGeometricPropertyDefinition('BufferGeometry');
    $polygonProperty->SetGeometryTypes(MgFeatureGeometricType::Surface);
    $polygonProperty->SetHasElevation(false);
    $polygonProperty->SetHasMeasure(false);
    $polygonProperty->SetReadOnly(false);
    $polygonProperty->SetSpatialContextAssociation('defaultSrs');
    $properties->Add($polygonProperty);
     
    $idProperties = $bufferClass->GetIdentityProperties();
    $idProperties->Add($idProperty);
     
    $bufferClass->SetDefaultGeometryPropertyName('BufferGeometry');
    $bufferSchema = new MgFeatureSchema('BufferLayerSchema', 'temporary schema to hold a buffer');
    $bufferSchema->GetClasses()->Add($bufferClass);
    $sdfParams = new MgCreateSdfParams('defaultSrs', $wkt, $bufferSchema);
    $featureService->CreateFeatureSource($bufferFeatureResId, $sdfParams);

**.net (C#)**

.. highlight:: csharp
.. code-block:: csharp

    //This code fragment assumes you have imported the OSGeo.MapGuide namespace
    MgClassDefinition bufferClass = new MgClassDefinition();
    bufferClass.SetName("BufferClass");
    MgPropertyDefinitionCollection properties = bufferClass.GetProperties();
     
    MgDataPropertyDefinition idProperty = new MgDataPropertyDefinition("ID");
    idProperty.SetDataType(MgPropertyType.Int32);
    idProperty.SetReadOnly(true);
    idProperty.SetNullable(false);
    idProperty.SetAutoGeneration(true);
    properties.Add(idProperty);
     
    MgGeometricPropertyDefinition polygonProperty = new MgGeometricPropertyDefinition("BufferGeometry");
    polygonProperty.SetGeometryTypes(MgFeatureGeometricType.Surface);
    polygonProperty.SetHasElevation(false);
    polygonProperty.SetHasMeasure(false);
    polygonProperty.SetReadOnly(false);
    polygonProperty.SetSpatialContextAssociation("defaultSrs");
    properties.Add(polygonProperty);
     
    MgPropertyDefinitionCollection idProperties = bufferClass.GetIdentityProperties();
    idProperties.Add(idProperty);
     
    bufferClass.SetDefaultGeometryPropertyName("BufferGeometry");
    MgFeatureSchema bufferSchema = new MgFeatureSchema("BufferLayerSchema", "temporary schema to hold a buffer");
    bufferSchema.GetClasses().Add(bufferClass);
    MgCreateSdfParams sdfParams = new MgCreateSdfParams("defaultSrs", wkt, bufferSchema);
    featureService.CreateFeatureSource(bufferFeatureResId, sdfParams);

**Java**
    
.. highlight:: java
.. code-block:: java

    //This code fragment assumes you have imported the org.osgeo.mapguide namespace
    MgClassDefinition bufferClass = new MgClassDefinition();
    bufferClass.SetName("BufferClass");
    MgPropertyDefinitionCollection properties = bufferClass.GetProperties();
     
    MgDataPropertyDefinition idProperty = new MgDataPropertyDefinition("ID");
    idProperty.SetDataType(MgPropertyType.Int32);
    idProperty.SetReadOnly(true);
    idProperty.SetNullable(false);
    idProperty.SetAutoGeneration(true);
    properties.Add(idProperty);
     
    MgGeometricPropertyDefinition polygonProperty = new MgGeometricPropertyDefinition("BufferGeometry");
    polygonProperty.SetGeometryTypes(MgFeatureGeometricType.Surface);
    polygonProperty.SetHasElevation(false);
    polygonProperty.SetHasMeasure(false);
    polygonProperty.SetReadOnly(false);
    polygonProperty.SetSpatialContextAssociation("defaultSrs");
    properties.Add(polygonProperty);
     
    MgPropertyDefinitionCollection idProperties = bufferClass.GetIdentityProperties();
    idProperties.Add(idProperty);
     
    bufferClass.SetDefaultGeometryPropertyName("BufferGeometry");
    MgFeatureSchema bufferSchema = new MgFeatureSchema("BufferLayerSchema", "temporary schema to hold a buffer");
    bufferSchema.GetClasses().Add(bufferClass);
    MgCreateSdfParams sdfParams = new MgCreateSdfParams("defaultSrs", wkt, bufferSchema);
    featureService.CreateFeatureSource(bufferFeatureResId, sdfParams);

To display features from a temporary feature source in a map, create a layer
definition that refers to the feature source. Use the techniques described in
:ref:`modifying-maps-and-layers`.

.. index::
    single: Features; Inserting Features
    single: Features; Deleting Features
    single: Features; Updating Features
    single: MgDeleteFeatures
    single: MgInsertFeatures
    single: MgUpdateFeatures
    single: MgFeatureService; UpdateFeatures

Inserting, Deleting and Updating Features
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To change data in a feature source, create an MgFeatureCommandCollection
object. This can contain commands to insert, delete, or update features in an
FDO data source. The commands are executed sequentially. For FDO providers
that support transaction processing, the commands can be treated as a single
transaction.

Feature commands can be one of the following:

 * ``MgDeleteFeatures``
 * ``MgInsertFeatures``
 * ``MgUpdateFeatures``

To execute the commands, call ``MgFeatureService::UpdateFeatures()``. The
feature class name and property names in any of the feature commands must
match the class name and property names in the feature source. If you want to execute 
feature commands against a ``MgLayer`` object, call ``MgLayer::UpdateFeatures()``. The
feature class name and feature source id of the ``MgLayer`` object is used

For example, to delete all features in a feature class with an identity property
``ID``, execute the following:

**PHP**

.. highlight:: php
.. code-block:: php

    $commands = new MgFeatureCommandCollection();
    $deleteCommand = new MgDeleteFeatures($className, "ID like '%'");
    $commands->Add($deleteCommand);
    $featureService->UpdateFeatures($featureSource, $commands, false);
    
    //You can do this instead if you have a MgLayer object and want to delete features from it
    $commands = new MgFeatureCommandCollection();
    $deleteCommand = new MgDeleteFeatures($className, "ID like '%'");
    $commands->Add($deleteCommand);
    $layer->UpdateFeatures($commands);

**.net (C#)**

.. highlight:: csharp
.. code-block:: csharp

    //This code fragment assumes you have imported the OSGeo.MapGuide namespace
    MgFeatureCommandCollection commands = new MgFeatureCommandCollection();
    MgDeleteFeature deleteCommand = new MgDeleteFeatures(className, "ID like '%'");
    commands.Add(deleteCommand);
    featureService.UpdateFeatures(featureSource, commands, false);
    
    //You can do this instead if you have a MgLayer object and want to delete features from it
    MgFeatureCommandCollection commands = new MgFeatureCommandCollection();
    MgDeleteFeature deleteCommand = new MgDeleteFeatures(className, "ID like '%'");
    commands.Add(deleteCommand);
    layer.UpdateFeatures(commands);

**Java**
    
.. highlight:: java
.. code-block:: java

    //This code fragment assumes you have imported the org.osgeo.mapguide namespace
    MgFeatureCommandCollection commands = new MgFeatureCommandCollection();
    MgDeleteFeature deleteCommand = new MgDeleteFeatures(className, "ID like '%'");
    commands.Add(deleteCommand);
    featureService.UpdateFeatures(featureSource, commands, false);
    
    //You can do this instead if you have a MgLayer object and want to delete features from it
    MgFeatureCommandCollection commands = new MgFeatureCommandCollection();
    MgDeleteFeature deleteCommand = new MgDeleteFeatures(className, "ID like '%'");
    commands.Add(deleteCommand);
    layer.UpdateFeatures(commands);

To insert features, create an ``MgPropertyCollection`` object that contains the
properties of the new feature. Create an ``MgInsertFeatures`` object and add
this to the ``MgFeatureCommandCollection`` object.

For example, to add a new feature with a single geometry property, execute
the following:

**PHP**

.. highlight:: php
.. code-block:: php

    $commands = new MgFeatureCommandCollection();
    $properties = new MgPropertyCollection();
    $agfByteStream = $agfReaderWriter->Write($geometry);
    $geometryProperty = new MgGeometryProperty($propertyName, $agfByteStream);
    $properties->Add($geometryProperty);
     
    $insertCommand = new MgInsertFeatures($className, $properties);
    $commands->Add($insertCommand);
     
    $featureService->UpdateFeatures($featureSource, $commands, false);
    
    //You can do this instead if you have a MgLayer object and want to update features in it
    $commands = new MgFeatureCommandCollection();
    $properties = new MgPropertyCollection();
    $agfByteStream = $agfReaderWriter->Write($geometry);
    $geometryProperty = new MgGeometryProperty($propertyName, $agfByteStream);
    $properties->Add($geometryProperty);
     
    $insertCommand = new MgInsertFeatures($className, $properties);
    $commands->Add($insertCommand);
     
    $layer->UpdateFeatures($commands);

**.net (C#)**

.. highlight:: csharp
.. code-block:: csharp

    //This code fragment assumes you have imported the OSGeo.MapGuide namespace
    MgFeatureCommandCollection commands = new MgFeatureCommandCollection();
    MgPropertyCollection properties = new MgPropertyCollection();
    MgByteReader agfByteStream = agfReaderWriter.Write(geometry);
    MgGeometryProperty geometryProperty = new MgGeometryProperty(propertyName, agfByteStream);
    properties.Add(geometryProperty);
    
    MgInsertFeatures insertCommand = new MgInsertFeatures(className, properties);
    commands.Add(insertCommand);
    
    featureService.UpdateFeatures(featureSource, commands, false);
    
    //You can do this instead if you have a MgLayer object and want to update features in it
    MgFeatureCommandCollection commands = new MgFeatureCommandCollection();
    MgPropertyCollection properties = new MgPropertyCollection();
    MgByteReader agfByteStream = agfReaderWriter.Write(geometry);
    MgGeometryProperty geometryProperty = new MgGeometryProperty(propertyName, agfByteStream);
    properties.Add(geometryProperty);
    
    MgInsertFeatures insertCommand = new MgInsertFeatures(className, properties);
    commands.Add(insertCommand);
    layer.UpdateFeatures(commands);

**Java**
    
.. highlight:: java
.. code-block:: java

    //This code fragment assumes you have imported the org.osgeo.mapguide namespace
    MgFeatureCommandCollection commands = new MgFeatureCommandCollection();
    MgPropertyCollection properties = new MgPropertyCollection();
    MgByteReader agfByteStream = agfReaderWriter.Write(geometry);
    MgGeometryProperty geometryProperty = new MgGeometryProperty(propertyName, agfByteStream);
    properties.Add(geometryProperty);
    
    MgInsertFeatures insertCommand = new MgInsertFeatures(className, properties);
    commands.Add(insertCommand);
    
    featureService.UpdateFeatures(featureSource, commands, false);
    
    //You can do this instead if you have a MgLayer object and want to update features in it
    MgFeatureCommandCollection commands = new MgFeatureCommandCollection();
    MgPropertyCollection properties = new MgPropertyCollection();
    MgByteReader agfByteStream = agfReaderWriter.Write(geometry);
    MgGeometryProperty geometryProperty = new MgGeometryProperty(propertyName, agfByteStream);
    properties.Add(geometryProperty);
    
    MgInsertFeatures insertCommand = new MgInsertFeatures(className, properties);
    commands.Add(insertCommand);
    layer.UpdateFeatures(commands);

To update existing features, create an MgPropertyCollection object that
contains the new values for the properties and a filter expression that selects
the correct feature or features. See :ref:`querying-feature-data` for details
about filter expressions.

.. index::
    single: Geometry; Buffering
    single: Buffer; Creating

Creating a Buffer
-----------------

To create a buffer around a feature, use the ``MgGeometry::Buffer()`` method.
This returns an ``MgGeometry`` object that you can use for further analysis. For
example, you could display the buffer by creating a feature in a temporary
feature source and adding a new layer to the map. You could also use the
buffer geometry as part of a spatial filter. For example, you might want to find
all the features within the buffer zone that match certain criteria, or you might
want to find all roads that cross the buffer zone.

To create a buffer, get the geometry of the feature to be buffered. If the feature
is being processed in an ``MgFeatureReader`` as part of a selection, this requires
getting the geometry data from the feature reader and converting it to an
``MgGeometry`` object. For example:

**PHP**

.. highlight:: php
.. code-block:: php

    $geometryData = $featureReader->GetGeometry($geometryName);
    $featureGeometry = $agfReaderWriter->Read($geometryData);

**.net (C#)**

.. highlight:: csharp
.. code-block:: csharp

    //This code fragment assumes you have imported the OSGeo.MapGuide namespace
    MgByteReader geometryData = featureReader.GetGeometry(geometryName);
    MgGeometry featureGeometry = agfReaderWriter.Read(geometryData);

**Java**
    
.. highlight:: java
.. code-block:: java

    //This code fragment assumes you have imported the org.osgeo.mapguide namespace
    MgByteReader geometryData = featureReader.GetGeometry(geometryName);
    MgGeometry featureGeometry = agfReaderWriter.Read(geometryData);

If the buffer is to be calculated using coordinate system units, create an
``MgCoordinateSystemMeasure`` object from the coordinate system for the map.
For example:

**PHP**

.. highlight:: php
.. code-block:: php

    $mapWktSrs = $currentMap->GetMapSRS();
    $coordSysFactory = new MgCoordinateSystemFactory();
    $srs = $coordSysFactory->Create($mapWktSrs);
    $srsMeasure = $srs->GetMeasure();

**.net (C#)**

.. highlight:: csharp
.. code-block:: csharp

    //This code fragment assumes you have imported the OSGeo.MapGuide namespace
    String mapWktSrs = currentMap.GetMapSRS();
    MgCoordinateSystemFactory coordSysFactory = new MgCoordinateSystemFactory();
    MgCoordianteSystem srs = coordSysFactory.Create(mapWktSrs);
    MgCoordinateSystemMeasure srsMeasure = srs.GetMeasure();

**Java**
    
.. highlight:: java
.. code-block:: java

    //This code fragment assumes you have imported the org.osgeo.mapguide namespace
    String mapWktSrs = currentMap.GetMapSRS();
    MgCoordinateSystemFactory coordSysFactory = new MgCoordinateSystemFactory();
    MgCoordianteSystem srs = coordSysFactory.Create(mapWktSrs);
    MgCoordinateSystemMeasure srsMeasure = srs.GetMeasure();

Use the coordinate system measure to determine the buffer size in the
coordinate system, and create the buffer object from the geometry to be
buffered.

**PHP**

.. highlight:: php
.. code-block:: php

    $srsDist = $srs->ConvertMetersToCoordinateSystemUnits($bufferDist);
    $bufferGeometry = $featureGeometry->Buffer($srsDist, $srsMeasure);

**.net (C#)**

.. highlight:: csharp
.. code-block:: csharp

    //This code fragment assumes you have imported the OSGeo.MapGuide namespace
    double srsDist = srs.ConvertMetersToCoordinateSystemUnits(bufferDist);
    MgGeometry bufferGeometry = featureGeometry.Buffer(srsDist, srsMeasure);

**Java**
    
.. highlight:: java
.. code-block:: java

    //This code fragment assumes you have imported the org.osgeo.mapguide namespace
    double srsDist = srs.ConvertMetersToCoordinateSystemUnits(bufferDist);
    MgGeometry bufferGeometry = featureGeometry.Buffer(srsDist, srsMeasure);

.. index::
    single: Buffer; Displaying in map

To display the buffer in the map, perform the following steps:

 * Create a feature source for the buffer.
 * Insert a buffer feature in the feature source.
 * Create a layer that references the feature source.
 * Add the layer to the map and make it visible.

To use the buffer as part of a query, create a spatial filter using the buffer
geometry, and use this in a call to ``MgFeatureService::SelectFeatures()`` or 
``MgLayer::SelectFeatures()``. For example, the following code selects parcels 
inside the buffer area that are of type "MFG". You can use the ``MgFeatureReader`` 
to perform tasks like generating a report of the parcels, or creating a new layer 
that puts point markers on each parcel.

**PHP**

.. highlight:: php
.. code-block:: php

    $queryOptions = new MgFeatureQueryOptions();
    $queryOptions->SetFilter("RTYPE = 'MFG'");
    $queryOptions->SetSpatialFilter('SHPGEOM', $bufferGeometry, MgFeatureSpatialOperations::Inside);
    /*
    // Old way, pre MapGuide OS 2.0. Kept here for reference
    $featureResId = new MgResourceIdentifier("Library://Samples/Sheboygan/Data/Parcels.FeatureSource");
    $featureReader = $featureService->SelectFeatures($featureResId, "Parcels", $queryOptions);
    */
    
    // New way, post MapGuide OS 2.0
    $featureReader = $layer->SelectFeatures($queryOptions);

**.net (C#)**

.. highlight:: csharp
.. code-block:: csharp

    //This code fragment assumes you have imported the OSGeo.MapGuide namespace
    MgFeatureQueryOptions queryOptions = new MgFeatureQueryOptions();
    queryOptions.SetFilter("RTYPE = 'MFG'");
    queryOptions.SetSpatialFilter("SHPGEOM", bufferGeometry, MgFeatureSpatialOperations.Inside);
    MgResourceIdentifier featureResId = new MgResourceIdentifier("Library://Samples/Sheboygan/Data/Parcels.FeatureSource");
    MgFeatureReader featureReader = featureService.SelectFeatures(featureResId, "Parcels", queryOptions);

**Java**
    
.. highlight:: java
.. code-block:: java

    //This code fragment assumes you have imported the org.osgeo.mapguide namespace
    MgFeatureQueryOptions queryOptions = new MgFeatureQueryOptions();
    queryOptions.SetFilter("RTYPE = 'MFG'");
    queryOptions.SetSpatialFilter("SHPGEOM", bufferGeometry, MgFeatureSpatialOperations.Inside);
    MgResourceIdentifier featureResId = new MgResourceIdentifier("Library://Samples/Sheboygan/Data/Parcels.FeatureSource");
    MgFeatureReader featureReader = featureService.SelectFeatures(featureResId, "Parcels", queryOptions);

Example
-------

This example builds on the example from :ref:`working-with-the-active-selection`. Instead of listing the parcels in the selection, it creates a series of
concentric buffers around the selection, showing increasing distance. The
code sections below contain the significant additions in this example. The
complete source code is available with the Developer's Guide samples.

Because this example modifies the map, it must refresh the map when it loads,
by executing a JavaScript function. Add the function to the page.

.. highlight:: javascript
.. code-block:: javascript

    <script language="javascript">
    function OnPageLoad()
    {
        parent.parent.Refresh();
    }
    </script>

Add an ``OnLoad`` command to the ``<body>`` element:

.. highlight:: html
.. code-block:: html

    <body onLoad="OnPageLoad()">

The example uses a temporary map layer named ``Buffer`` to store the buffer
feature. It creates a feature source and the layer if it does not exist. Otherwise,
it deletes any existing features before creating the new buffer. The functions
``CreateBufferFeatureSource()`` and ``CreateBufferLayer()`` are in
*bufferfunctions.php*, which is described below.

**PHP**

.. highlight:: php
.. code-block:: php

    include 'bufferfunctions.php';
    $bufferRingSize = 100; // measured in metres
    $bufferRingCount = 5;

    // Set up some objects for coordinate conversion

    $mapWktSrs = $map->GetMapSRS();
    $agfReaderWriter = new MgAgfReaderWriter();
    $wktReaderWriter = new MgWktReaderWriter();
    $coordinateSystemFactory = new MgCoordinateSystemFactory();
    $srs = $coordinateSystemFactory->Create($mapWktSrs);
    $srsMeasure = $srs->GetMeasure();

    // Check for a buffer layer. If it exists, delete
    // the current features.
    // If it does not exist, create a feature source and
    // a layer to hold the buffer.

    // This is how things can be done now
    $layerIndex = $map->GetLayers()->IndexOf('Buffer');
    if ($layerIndex < 0)
    {
        // The layer does not exist and must be created.

        $bufferFeatureResId = new MgResourceIdentifier("Session:" . $sessionId . "//Buffer.FeatureSource");
        CreateBufferFeatureSource($featureService, $mapWktSrs, $bufferFeatureResId);
        $bufferLayer = CreateBufferLayer($resourceService, $bufferFeatureResId, $sessionId);
        $map->GetLayers()->Insert(0, $bufferLayer);
    }
    else
    {
        $bufferLayer = $map->GetLayers()->GetItem($layerIndex);
        $commands = new MgFeatureCommandCollection();
        $commands->Add(new MgDeleteFeatures('BufferClass', "ID like '%'"));
        
        $bufferLayer->UpdateFeatures($commands);
    }
    
**.net (C#)**

.. highlight:: csharp
.. code-block:: csharp

    //This code fragment assumes you have imported the OSGeo.MapGuide namespace
    int bufferRingSize = 100; // measured in metres
    int bufferRingCount = 5;

    // Set up some objects for coordinate conversion

    String mapWktSrs = map.GetMapSRS();
    MgAgfReaderWriter agfReaderWriter = new MgAgfReaderWriter();
    MgWktReaderWriter wktReaderWriter = new MgWktReaderWriter();
    MgCoordinateSystemFactory coordinateSystemFactory = new MgCoordinateSystemFactory();
    MgCoordinateSystem srs = coordinateSystemFactory.Create(mapWktSrs);
    MgMeasure srsMeasure = srs.GetMeasure();

    BufferHelper helper = new BufferHelper(Server);

    // Check for a buffer layer. If it exists, delete
    // the current features.
    // If it does not exist, create a feature source and
    // a layer to hold the buffer.

    MgLayer bufferLayer = null;
    int layerIndex = map.GetLayers().IndexOf("Buffer");
    if (layerIndex < 0)
    {
        // The layer does not exist and must be created.

        MgResourceIdentifier bufferFeatureResId = new MgResourceIdentifier("Session:" + sessionId + "//Buffer.FeatureSource");
        helper.CreateBufferFeatureSource(featureService, mapWktSrs, bufferFeatureResId);
        bufferLayer = helper.CreateBufferLayer(resourceService, bufferFeatureResId, sessionId);
        map.GetLayers().Insert(0, bufferLayer);
    }
    else
    {
        bufferLayer = (MgLayer)map.GetLayers().GetItem(layerIndex);
        MgFeatureCommandCollection commands = new MgFeatureCommandCollection();
        commands.Add(new MgDeleteFeatures("BufferClass", "ID like '%'"));

        bufferLayer.UpdateFeatures(commands);
    }

**Java**
    
.. highlight:: java
.. code-block:: java

    //This code fragment assumes you have imported the org.osgeo.mapguide namespace

.. index::
    single: Geometry; Merging geometries

The geometries for the selected features are merged into a single
multi-geometry. Then a series of concentric buffers is created and added to
the feature source. The style for the layer, which is set when function
``CreateBufferLayer()`` processes ``bufferlayerdefinition.xml``, should define the
buffer features to be partly transparent. When they are drawn on the map,
the rings get progressively darker towards the center of the buffer area.

**PHP**

.. highlight:: php
.. code-block:: php

    // Process each item in the MgFeatureReader.
    // Merge them into a single feature.
 
    $inputGeometries = new MgGeometryCollection();
    while ($featureReader->ReadNext())
    {
        $featureGeometryData = $featureReader->GetGeometry('SHPGEOM');
        $featureGeometry = $agfReaderWriter->Read($featureGeometryData);

        $inputGeometries->Add($featureGeometry);
    }

    $geometryFactory = new MgGeometryFactory();
    $mergedFeatures = $geometryFactory->CreateMultiGeometry($inputGeometries);

    // Add buffer features to the temporary feature source.
    // Create multiple concentric buffers to show area.

    $commands = new MgFeatureCommandCollection();
    for ($bufferRing = 0; $bufferRing < $bufferRingCount; $bufferRing++)
    {
        $bufferDist = $srs->ConvertMetersToCoordinateSystemUnits($bufferRingSize * ($bufferRing + 1));
        $bufferGeometry = $mergedFeatures->Buffer($bufferDist, $srsMeasure);

        $properties = new MgPropertyCollection();
        $properties->Add(new MgGeometryProperty('BufferGeometry', $agfReaderWriter->Write($bufferGeometry)));

        $commands->Add(new MgInsertFeatures('BufferClass', $properties));
    }

    $results = $bufferLayer->UpdateFeatures($commands);

    $bufferLayer->SetVisible(true);
    $bufferLayer->ForceRefresh();
    $bufferLayer->SetDisplayInLegend(true);
    
    //If you created a MgMap with a MgSiteConnection
    $map->Save();

**.net (C#)**

.. highlight:: csharp
.. code-block:: csharp

    //This code fragment assumes you have imported the OSGeo.MapGuide namespace
    MgGeometryCollection inputGeometries = new MgGeometryCollection();
    while (featureReader.ReadNext())
    {
        MgByteReader featureGeometryData = featureReader.GetGeometry("SHPGEOM");
        MgGeometry featureGeometry = agfReaderWriter.Read(featureGeometryData);

        inputGeometries.Add(featureGeometry);
    }

    MgGeometryFactory geometryFactory = new MgGeometryFactory();
    MgGeometry mergedGeometries = geometryFactory.CreateMultiGeometry(inputGeometries);

    // Add buffer features to the temporary feature source.
    // Create multiple concentric buffers to show area.
    // If the stylization for the layer draws the features
    // partially transparent, the concentric rings will be
    // progressively darker towards the center.
    // The stylization is set in the layer template file, which
    // is used in function CreateBufferLayer().

    MgFeatureCommandCollection commands = new MgFeatureCommandCollection();
    for (int bufferRing = 0; bufferRing < bufferRingCount; bufferRing++)
    {
        double bufferDist = srs.ConvertMetersToCoordinateSystemUnits(bufferRingSize * (bufferRing + 1));
        MgGeometry bufferGeometry = mergedGeometries.Buffer(bufferDist, srsMeasure);

        MgPropertyCollection properties = new MgPropertyCollection();
        properties.Add(new MgGeometryProperty("BufferGeometry", agfReaderWriter.Write(bufferGeometry)));

        commands.Add(new MgInsertFeatures("BufferClass", properties));
    }

    bufferLayer.UpdateFeatures(commands);

    bufferLayer.SetVisible(true);
    bufferLayer.ForceRefresh();
    bufferLayer.SetDisplayInLegend(true);
    map.Save();

**Java**
    
.. highlight:: java
.. code-block:: java

    //This code fragment assumes you have imported the org.osgeo.mapguide namespace
    
    //No code sample available yet.
    
The functions ``CreateBufferFeatureSource()`` and ``CreateBufferLayer()`` are
in ``bufferfunctions.php``. ``CreateBufferFeatureSource()`` creates a temporary
feature source, with a single feature class, ``BufferClass``. The feature class has
two properties, ``ID`` and ``BufferGeometry``. ``ID`` is autogenerated, so it does not
need to be added with a new feature. ``CreateBufferLayer()`` modifies a layer
definition from an external file and saves it to the repository. For more details,
see :ref:`modifying-maps-and-layers`.

**PHP**

.. highlight:: php
.. code-block:: php

    function CreateBufferFeatureSource($featureService, $wkt, $bufferFeatureResId)
    {
        $bufferClass = new MgClassDefinition();
        $bufferClass->SetName('BufferClass');
        $properties = $bufferClass->GetProperties();
        $idProperty = new MgDataPropertyDefinition('ID');
        $idProperty->SetDataType(MgPropertyType::Int32);
        $idProperty->SetReadOnly(true);
        $idProperty->SetNullable(false);
        $idProperty->SetAutoGeneration(true);
        $properties->Add($idProperty);
        $polygonProperty = new MgGeometricPropertyDefinition('BufferGeometry');
        $polygonProperty->SetGeometryTypes(MgFeatureGeometricType::Surface);
        $polygonProperty->SetHasElevation(false);
        $polygonProperty->SetHasMeasure(false);
        $polygonProperty->SetReadOnly(false);
        $polygonProperty->SetSpatialContextAssociation('defaultSrs');
        $properties->Add($polygonProperty);
        $idProperties = $bufferClass->GetIdentityProperties();
        $idProperties->Add($idProperty);
        $bufferClass->SetDefaultGeometryPropertyName('BufferGeometry');
        $bufferSchema = new MgFeatureSchema('BufferLayerSchema', 'temporary schema to hold a buffer');
        $bufferSchema->GetClasses()->Add($bufferClass);
        $sdfParams = new MgCreateSdfParams('defaultSrs', $wkt, $bufferSchema);
        $featureService->CreateFeatureSource($bufferFeatureResId, $sdfParams);
    }
     
    function CreateBufferLayer($resourceService, $bufferFeatureResId, $sessionId)
    {
        // Load the layer definition template into
        // a PHP DOM object, find the "ResourceId" element, and
        // modify its content to reference the temporary
        // feature source.
        $doc = DOMDocument::load('bufferlayerdefinition.xml');
        $featureSourceNode = $doc->getElementsByTagName('ResourceId')->item(0);
        $featureSourceNode->nodeValue = $bufferFeatureResId->ToString();
        // Get the updated layer definition from the DOM object
        // and save it to the session repository using the
        // ResourceService object.
        $layerDefinition = $doc->saveXML();
        $byteSource = new MgByteSource($layerDefinition, strlen($layerDefinition));
        $byteSource->SetMimeType(MgMimeType::Xml);
        $tempLayerResId = new MgResourceIdentifier("Session:" . $sessionId . "//Buffer.LayerDefinition");
        $resourceService->SetResource($tempLayerResId, $byteSource->GetReader(), null);
        // Create an MgLayer object based on the new layer definition
        // and return it to the caller.
        $bufferLayer = new MgLayer($tempLayerResId, $resourceService);
        $bufferLayer->SetName("Buffer");
        $bufferLayer->SetLegendLabel("Buffer");
        $bufferLayer->SetDisplayInLegend(true);
        $bufferLayer->SetSelectable(false);
        return $bufferLayer;
    }

**.net (C#)**

.. highlight:: csharp
.. code-block:: csharp

    //This code fragment assumes you have imported the OSGeo.MapGuide namespace
    
    //NOTE: This is only subset of the BufferHelper relevant to this particular chapter
    //of the developer's guide. For the full source, see bufferfunctions.aspx from the
    //.net Developer Guide sample.
    
    public class BufferHelper
    {
        private HttpServerUtility _server;

        public BufferHelper(HttpServerUtility server) 
        { 
            _server = server;
        }
        
        HttpServerUtility Server { get { return _server; } }
    
        public void CreateBufferFeatureSource(MgFeatureService featureService, String wkt, MgResourceIdentifier bufferFeatureResId)
        {
            MgClassDefinition bufferClass = new MgClassDefinition();
            bufferClass.SetName("BufferClass");
            MgPropertyDefinitionCollection properties = bufferClass.GetProperties();

            MgDataPropertyDefinition idProperty = new MgDataPropertyDefinition("ID");
            idProperty.SetDataType(MgPropertyType.Int32);
            idProperty.SetReadOnly(true);
            idProperty.SetNullable(false);
            idProperty.SetAutoGeneration(true);
            properties.Add(idProperty);

            MgGeometricPropertyDefinition polygonProperty = new MgGeometricPropertyDefinition("BufferGeometry");
            polygonProperty.SetGeometryTypes(MgFeatureGeometricType.Surface);
            polygonProperty.SetHasElevation(false);
            polygonProperty.SetHasMeasure(false);
            polygonProperty.SetReadOnly(false);
            polygonProperty.SetSpatialContextAssociation("defaultSrs");
            properties.Add(polygonProperty);

            MgPropertyDefinitionCollection idProperties = bufferClass.GetIdentityProperties();
            idProperties.Add(idProperty);

            bufferClass.SetDefaultGeometryPropertyName("BufferGeometry");

            MgFeatureSchema bufferSchema = new MgFeatureSchema("BufferLayerSchema", "temporary schema to hold a buffer");
            bufferSchema.GetClasses().Add(bufferClass);

            MgCreateSdfParams sdfParams = new MgCreateSdfParams("defaultSrs", wkt, bufferSchema);

            featureService.CreateFeatureSource(bufferFeatureResId, sdfParams);
        }

        public MgLayer CreateBufferLayer(MgResourceService resourceService, MgResourceIdentifier bufferFeatureResId, String sessionId)
        {
            // Load the layer definition template into
            // a XmlDocument object, find the "ResourceId" element, and
            // modify its content to reference the temporary
            // feature source.

            XmlDocument doc = new XmlDocument();
            doc.Load(Server.MapPath("bufferlayerdefinition.xml"));
            XmlNode featureSourceNode = doc.GetElementsByTagName("ResourceId")[0];
            featureSourceNode.InnerText = bufferFeatureResId.ToString();

            // Get the updated layer definition from the XmlDocument
            // and save it to the session repository using the
            // ResourceService object.

            MgByteSource byteSource = null;
            using (MemoryStream ms = new MemoryStream())
            {
                doc.Save(ms);
                ms.Position = 0L;
                
                //Note we do this to ensure our XML content is free of any BOM characters
                byte [] layerDefinition = ms.ToArray();
                Encoding utf8 = Encoding.UTF8;
                String layerDefStr = new String(utf8.GetChars(layerDefinition));
                layerDefinition = new byte[layerDefStr.Length-1];
                int byteCount = utf8.GetBytes(layerDefStr, 1, layerDefStr.Length-1, layerDefinition, 0);
                
                byteSource = new MgByteSource(layerDefinition, layerDefinition.Length);
                byteSource.SetMimeType(MgMimeType.Xml);
            }

            MgResourceIdentifier tempLayerResId = new MgResourceIdentifier("Session:" + sessionId + "//Buffer.LayerDefinition");

            resourceService.SetResource(tempLayerResId, byteSource.GetReader(), null);

            // Create an MgLayer object based on the new layer definition
            // and return it to the caller.

            MgLayer bufferLayer = new MgLayer(tempLayerResId, resourceService);
            bufferLayer.SetName("Buffer");
            bufferLayer.SetLegendLabel("Buffer");
            bufferLayer.SetDisplayInLegend(true);
            bufferLayer.SetSelectable(false);
            
            return bufferLayer;
        }
    }

**Java**
    
.. highlight:: java
.. code-block:: java

    //This code fragment assumes you have imported the org.osgeo.mapguide namespace
    
    //No code sample available yet
    
There is an additional example in the Developer's Guide samples. It queries
the parcels in the buffer area and selects parcels that match certain criteria.
The selection is done using a query that combines a basic filter and a spatial
filter.

**PHP**

.. highlight:: php
.. code-block:: php

    $bufferDist = $srs->ConvertMetersToCoordinateSystemUnits($bufferRingSize);
    $bufferGeometry = $mergedGeometries->Buffer($bufferDist, $srsMeasure);
    // Create a filter to select parcels within the buffer. Combine
    // a basic filter and a spatial filter to select all parcels
    // within the buffer that are of type "MFG".
    $queryOptions = new MgFeatureQueryOptions();
    $queryOptions->SetFilter("RTYPE = 'MFG'");
    $queryOptions->SetSpatialFilter('SHPGEOM', $bufferGeometry, MgFeatureSpatialOperations::Inside);

**.net (C#)**

.. highlight:: csharp
.. code-block:: csharp

    //This code fragment assumes you have imported the OSGeo.MapGuide namespace
    
    double bufferDist = srs.ConvertMetersToCoordinateSystemUnits(bufferRingSize);
    MgGeometry bufferGeometry = mergedGeometries.Buffer(bufferDist, srsMeasure);

    // Create a filter to select parcels within the buffer. Combine
    // a basic filter and a spatial filter to select all parcels
    // within the buffer that are of type "MFG".

    queryOptions = new MgFeatureQueryOptions();
    queryOptions.SetFilter("RTYPE = 'MFG'");
    queryOptions.SetSpatialFilter("SHPGEOM", bufferGeometry, MgFeatureSpatialOperations.Inside);

**Java**
    
.. highlight:: java
.. code-block:: java

    //This code fragment assumes you have imported the org.osgeo.mapguide namespace
    
    double bufferDist = srs.ConvertMetersToCoordinateSystemUnits(bufferRingSize);
    MgGeometry bufferGeometry = mergedGeometries.Buffer(bufferDist, srsMeasure);

    // Create a filter to select parcels within the buffer. Combine
    // a basic filter and a spatial filter to select all parcels
    // within the buffer that are of type "MFG".

    queryOptions = new MgFeatureQueryOptions();
    queryOptions.SetFilter("RTYPE = 'MFG'");
    queryOptions.SetSpatialFilter("SHPGEOM", bufferGeometry, MgFeatureSpatialOperations.Inside);

It creates an additional feature source that contains point markers for each of
the selected parcels.

**PHP**

.. highlight:: php
.. code-block:: php

    // Get the features from the feature source,
    // determine the centroid of each selected feature, and
    // add a point to the ParcelMarker layer to mark the
    // centroid.
    // Collect all the points into an MgFeatureCommandCollection,
    // so they can all be added in one operation.
    
    $parcelMarkerCommands = new MgFeatureCommandCollection();
    while ($featureReader->ReadNext())
    {
        $byteReader = $featureReader->GetGeometry('SHPGEOM');
        $geometry = $agfReaderWriter->Read($byteReader);
        $point = $geometry->GetCentroid();

        // Create an insert command for this parcel.
        $properties = new MgPropertyCollection();

        $properties->Add(new MgGeometryProperty('ParcelLocation', $agfReaderWriter->Write($point)));
        $parcelMarkerCommands->Add(new MgInsertFeatures('ParcelMarkerClass', $properties));
    }
    $featureReader->Close();

    if ($parcelMarkerCommands->GetCount() > 0)
    {
        $parcelMarkerLayer->UpdateFeatures($parcelMarkerCommands);
    }
    else
    {
        echo '</p><p>No parcels within the buffer area match.';
    }

**.net (C#)**

.. highlight:: csharp
.. code-block:: csharp

    //This code fragment assumes you have imported the OSGeo.MapGuide namespace
    
    // Get the features from the feature source,
    // determine the centroid of each selected feature, and
    // add a point to the ParcelMarker layer to mark the
    // centroid.
    // Collect all the points into an MgFeatureCommandCollection,
    // so they can all be added in one operation.
    
    MgFeatureCommandCollection parcelMarkerCommands = new MgFeatureCommandCollection();
    while (featureReader.ReadNext())
    {
        MgByteReader byteReader = featureReader.GetGeometry("SHPGEOM");
        MgGeometry geometry = agfReaderWriter.Read(byteReader);
        MgPoint point = geometry.GetCentroid();

        // Create an insert command for this parcel.
        MgPropertyCollection properties = new MgPropertyCollection();
        properties.Add(new MgGeometryProperty("ParcelLocation", agfReaderWriter.Write(point)));
        parcelMarkerCommands.Add(new MgInsertFeatures("ParcelMarkerClass", properties));
    }
    featureReader.Close();

    if (parcelMarkerCommands.GetCount() > 0)
    {
        parcelMarkerLayer.UpdateFeatures(parcelMarkerCommands);
    }
    else
    {
        Response.Write("</p><p>No parcels within the buffer area match.");
    }

**Java**
    
.. highlight:: java
.. code-block:: java

    //This code fragment assumes you have imported the org.osgeo.mapguide namespace
    
    // Get the features from the feature source,
    // determine the centroid of each selected feature, and
    // add a point to the ParcelMarker layer to mark the
    // centroid.
    // Collect all the points into an MgFeatureCommandCollection,
    // so they can all be added in one operation.
    
    MgFeatureCommandCollection parcelMarkerCommands = new MgFeatureCommandCollection();
    while (featureReader.ReadNext())
    {
        MgByteReader byteReader = featureReader.GetGeometry("SHPGEOM");
        MgGeometry geometry = agfReaderWriter.Read(byteReader);
        MgPoint point = geometry.GetCentroid();

        // Create an insert command for this parcel.
        MgPropertyCollection properties = new MgPropertyCollection();
        properties.Add(new MgGeometryProperty("ParcelLocation", agfReaderWriter.Write(point)));
        parcelMarkerCommands.Add(new MgInsertFeatures("ParcelMarkerClass", properties));
    }
    featureReader.Close();

    if (parcelMarkerCommands.GetCount() > 0)
    {
        parcelMarkerLayer.UpdateFeatures(parcelMarkerCommands);
    }
    else
    {
        response.getWriter().Write("</p><p>No parcels within the buffer area match.");
    }
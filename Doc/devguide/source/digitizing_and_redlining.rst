Digitizing and Redlining
========================

.. index::
    single: Digitizing; Introduction
    single: Redlining; Introduction

Introduction
------------

.. tip::

    The Digitizing and Redlining sample, in the Developer's Guide samples,
    demonstrates concepts from this chapter.

This chapter describes *digitizing* (capturing the user's clicks on the map and
converting the locations to map coordinates) and *redlining* (drawing items such
as lines or rectangles on the map in response to the user's clicks).

.. index::
    single: Digitizing; Viewer API

Digitizing
----------

The Viewer API has a number of functions for digitizing user input. For an
example of how these can be used, see task_pane_digitizing.php in the
``digitizing_features`` directory in the Developer Guide samples.

In this example, if the user clicks the button to digitize a point

.. highlight:: html
.. code-block:: html

    <input type="button" value=" Point " onclick="DigitizePoint();">

the script calls the JavaScript function

.. highlight:: javascript
.. code-block:: javascript

    function DigitizePoint() {
        parent.parent.mapFrame.DigitizePoint(OnPointDigitized);
    }

which in turn calls the ``DigitzePoint()`` method of the Viewer API in the map
frame. It also passes the name of a callback function, ``OnPointDigitized``, which
is defined in the current script. ``DigizitzePoint()`` calls this function after it
has digitized the point and passes it the digitized coordinates of the point.

You can use this callback function to process the digitized coordinates as you
wish. In this example, the script simply displays them in the task pane.

.. highlight:: javascript
.. code-block:: javascript

    function OnPointDigitized(point) {
        ShowResults("X: " + point.X + ", Y: " + point.Y);
    }

.. index::
    single: Redlining; Process

Redlining
---------

There are three main steps involved in redlining:

 1. Pass the digitized coordinates from the client to the server.
 2. Create a temporary feature source. This will be used to draw the lines on.
 3. Create a layer to display that temporary feature source.

For example, see ``task_pane_redlining.php`` in the ``digitizing_features``
directory in the Developer Guide samples.

.. index::
    single: Digitizing; Passing Coordinates

Passing Coordinates
^^^^^^^^^^^^^^^^^^^

The digitizing functions in the Viewer API provide us with the digitized
coordinates on the client, but we usually need to pass them to a server side
script. This can be done with the Viewer API, using the Submit() method of
the formFrame.

.. highlight:: javascript
.. code-block:: javascript

    function OnLineDigitized(line) {
        // Send the Javascript variables to 'draw_line.php',
        // via the form frame
        var params = new Array(
            "x0", line.Point(0).X,
            "y0", line.Point(0).Y,
            "x1", line.Point(1).X,
            "y1", line.Point(1).Y,
            "SESSION", "<?= $sessionId ?>",
            "MAPNAME", "<?= $mapName ?>");
        parent.parent.formFrame.Submit("/mapguide/samplesphp/digitizing_features/draw_line.php", params, "scriptFrame");
    }

This submits the coordinates to the server-side function to draw the line. It
uses the hidden ``scriptFrame`` so the page output is not visible.

.. index::
    single: Redlining; Creating a Feature Source
    single: Feature Source; Creating

.. _create-feature-source:

Creating a Feature Source
^^^^^^^^^^^^^^^^^^^^^^^^^

The next step is create a feature source
See ``draw_line.php`` in the ``digitizing_features`` directory in the Developer
Guide samples.

**PHP**

.. highlight:: php
.. code-block:: php

    // Create a temporary feature source to draw the lines on
     
    // Create a feature class definition for the new feature
    // source
    $classDefinition = new MgClassDefinition();
    $classDefinition->SetName("Lines");
    $classDefinition->SetDescription("Lines to display.");
    $geometryPropertyName = "SHPGEOM";
    $classDefinition->SetDefaultGeometryPropertyName( $geometryPropertyName);
       
    // Create an identify property
    $identityProperty = new MgDataPropertyDefinition("KEY");
    $identityProperty->SetDataType(MgPropertyType::Int32);
    $identityProperty->SetAutoGeneration(true);
    $identityProperty->SetReadOnly(true);
    // Add the identity property to the class definition
    $classDefinition->GetIdentityProperties()->Add($identityProperty);
    $classDefinition->GetProperties()->Add($identityProperty);
       
    // Create a name property
    $nameProperty = new MgDataPropertyDefinition("NAME");
    $nameProperty->SetDataType(MgPropertyType::String);
    // Add the name property to the class definition
    $classDefinition->GetProperties()->Add($nameProperty);
      
    // Create a geometry property
    $geometryProperty = new MgGeometricPropertyDefinition($geometryPropertyName);
    $geometryProperty->SetGeometryTypes(MgFeatureGeometricType::Surface);
    // Add the geometry property to the class definition
    $classDefinition->GetProperties()->Add($geometryProperty);
      
    // Create a feature schema
    $featureSchema = new MgFeatureSchema("SHP_Schema", "Line schema");
    // Add the feature schema to the class definition
    $featureSchema->GetClasses()->Add($classDefinition);
      
    // Create the feature source
    $wkt = $map->GetMapSRS();
    $sdfParams = new MgCreateSdfParams("spatial context", $wkt, $featureSchema);
    $featureService->CreateFeatureSource($resourceIdentifier, $sdfParams);

**.net (C#)**

.. highlight:: csharp
.. code-block:: csharp

    //This code fragment assumes you have imported the OSGeo.MapGuide namespace
    
    // Create a temporary feature source to draw the lines on

    // Create a feature class definition for the new feature
    // source
    MgClassDefinition classDefinition = new MgClassDefinition();
    classDefinition.SetName("Lines");
    classDefinition.SetDescription("Lines to display.");
    String geometryPropertyName="SHPGEOM";
    classDefinition.SetDefaultGeometryPropertyName( geometryPropertyName);

    // Create an identify property
    MgDataPropertyDefinition identityProperty = new MgDataPropertyDefinition("KEY");
    identityProperty.SetDataType(MgPropertyType.Int32);
    identityProperty.SetAutoGeneration(true);
    identityProperty.SetReadOnly(true);
    // Add the identity property to the class definition
    classDefinition.GetIdentityProperties().Add(identityProperty);
    classDefinition.GetProperties().Add(identityProperty);

    // Create a name property
    MgDataPropertyDefinition nameProperty = new MgDataPropertyDefinition("NAME");
    nameProperty.SetDataType(MgPropertyType.String);
    // Add the name property to the class definition
    classDefinition.GetProperties().Add(nameProperty);

    // Create a geometry property
    MgGeometricPropertyDefinition geometryProperty = new MgGeometricPropertyDefinition(geometryPropertyName);
    geometryProperty.SetGeometryTypes(MgFeatureGeometricType.Surface);
    // Add the geometry property to the class definition
    classDefinition.GetProperties().Add(geometryProperty);

    // Create a feature schema
    MgFeatureSchema featureSchema = new MgFeatureSchema("SHP_Schema", "Line schema");
    // Add the feature schema to the class definition
    featureSchema.GetClasses().Add(classDefinition);

    // Create the feature source
    String wkt = map.GetMapSRS();
    MgCreateSdfParams sdfParams = new MgCreateSdfParams("spatial context", wkt, featureSchema);
    featureService.CreateFeatureSource(resourceIdentifier, sdfParams);

**Java**
    
.. highlight:: java
.. code-block:: java

    //This code fragment assumes you have imported the org.osgeo.mapguide namespace
    
    // Create a temporary feature source to draw the lines on

    // Create a feature class definition for the new feature
    // source
    MgClassDefinition classDefinition = new MgClassDefinition();
    classDefinition.SetName("Lines");
    classDefinition.SetDescription("Lines to display.");
    String geometryPropertyName="SHPGEOM";
    classDefinition.SetDefaultGeometryPropertyName( geometryPropertyName);

    // Create an identify property
    MgDataPropertyDefinition identityProperty = new MgDataPropertyDefinition("KEY");
    identityProperty.SetDataType(MgPropertyType.Int32);
    identityProperty.SetAutoGeneration(true);
    identityProperty.SetReadOnly(true);
    // Add the identity property to the class definition
    classDefinition.GetIdentityProperties().Add(identityProperty);
    classDefinition.GetProperties().Add(identityProperty);

    // Create a name property
    MgDataPropertyDefinition nameProperty = new MgDataPropertyDefinition("NAME");
    nameProperty.SetDataType(MgPropertyType.String);
    // Add the name property to the class definition
    classDefinition.GetProperties().Add(nameProperty);

    // Create a geometry property
    MgGeometricPropertyDefinition geometryProperty = new MgGeometricPropertyDefinition(geometryPropertyName);
    geometryProperty.SetGeometryTypes(MgFeatureGeometricType.Surface);
    // Add the geometry property to the class definition
    classDefinition.GetProperties().Add(geometryProperty);

    // Create a feature schema
    MgFeatureSchema featureSchema = new MgFeatureSchema("SHP_Schema", "Line schema");
    // Add the feature schema to the class definition
    featureSchema.GetClasses().Add(classDefinition);

    // Create the feature source
    String wkt = map.GetMapSRS();
    MgCreateSdfParams sdfParams = new MgCreateSdfParams("spatial context", wkt, featureSchema);
    featureService.CreateFeatureSource(resourceIdentifier, sdfParams);

.. index::
    single: Redlining; Create a Layer

Create a Layer
^^^^^^^^^^^^^^

.. todo::
    Update page number reference with section link

The final step is to create a new layer to display the feature source, the same
way it was done in :ref:`adding-layers-to-a-map`
.. _modifying-maps-and-layers:

Modifying Maps and Layers
=========================

Introduction
------------

.. tip::

    The Modifying Maps and Layers sample, in the Developer's Guide samples, demonstrates concepts from this chapter.

This chapter describes how to modify maps and layers.

.. index::
    single: Layers; Adding an existing layer to a map

Adding An Existing Layer To A Map
---------------------------------

If the layer already exists in the resource repository, add it to the map by getting
the map's layer collection and then adding the layer to that collection.

**PHP**

.. highlight:: php
.. code-block:: php

    $layerCollection = $map->GetLayers();
    $layerCollection->Add($layer);

**.net (C#)**

.. highlight:: csharp
.. code-block:: csharp

    //This code fragment assumes you have imported the OSGeo.MapGuide namespace
    MgLayerCollection layerCollection = map.GetLayers();
    layerCollection.Add(layer);

**Java**
    
.. highlight:: java
.. code-block:: java

    //This code fragment assumes you have imported the org.osgeo.mapguide namespace
    MgLayerCollection layerCollection = map.GetLayers();
    layerCollection.Add(layer);


By default, newly added layers are added to the bottom of the drawing order,
so they may be obscured by other layers. If you want to specify where the layer
appears in the drawing order, use the ``$layerCollection->Insert()`` method.
For an example, see :ref:`adding-layers-to-a-map`.

.. note::

    In the MapGuide API, getting a collection returns a reference to the collection. So adding the layer to the layer collection immediately updates the map.

.. index::
    single: Layers; Creating layers by modifying XML

Creating Layers by Modifying XML
--------------------------------

The easiest way to programmatically create new layers is to

 1. Build a prototype layer through the UI. To make the scripting simpler, this layer should have as many of the correct settings as can be determined in advance.
 2. Use MapGuide Studio **Save as Xml** command or similar function in Maestro to save the layer as an XML file.
 3. Have the script load the XML file and then use the DOM (Document Object Model) to change the necessary XML elements.
 4. Add the modified layer to the map.

The XML schema for layer definitions is defined by the
``LayerDefinition-version.xsd`` schema, which is documented in the MapGuide Web API Reference. This 
schema closely parallels the UI in the Layer Editor of MapGuide Studio and as described in the 
*MapGuide Studio Help*, and the Layer Editor of MapGuide Maestro.

This example

 * loads a layer that has been created through
 * uses the DOM to change the filter and its associated legend label
 
You can use the DOM to modify any layers, including ones that already exist
in the map, not just new layers that you are adding to the map. You can also
use the DOM to modify other resources; the XML schemas are described in
the *MapGuide Web API Reference*.

**PHP**

.. highlight:: php
.. code-block:: php

    // (initialization etc. not shown here)
    // Open the map
    $map = new MgMap();
    $map->Open($resourceService, $mapName);
    // --------------------------------------------------//
    // Load a layer from XML, and use the DOM to change it
    // Load the prototype layer definition into
    // a PHP DOM object.
    $domDocument =
    DOMDocument::load('RecentlyBuilt.LayerDefinition');
    if ($domDocument == NULL)
    {
        echo "The layer definition RecentlyBuilt.LayerDefinition' could not be found.<BR>\n";
        return;
    }
    // Change the filter
    $xpath = new DOMXPath($domDocument);
    $query = '//AreaRule/Filter';
    // Get a list of all the <AreaRule><Filter> elements in
    // the XML.
    $nodes = $xpath->query($query);
    // Find the correct node and change it
    foreach ($nodes as $node )
    {
        if ($node->nodeValue == 'YRBUILT > 1950')
        {
            $node->nodeValue = 'YRBUILT > 1980';
        }
    }
    // Change the legend label
    $query = '//LegendLabel';
    // Get a list of all the <LegendLabel> elements in the
    // XML.
    $nodes = $xpath->query($query);
    // Find the correct node and change it
    foreach ($nodes as $node )
    {
        if ($node->nodeValue == 'Built after 1950')
        {
            $node->nodeValue = 'Built after 1980';
        }
    }
    // ...

**.net (C#)**

.. highlight:: csharp
.. code-block:: csharp

    //This code fragment assumes you have imported the OSGeo.MapGuide namespace
    
    // --------------------------------------------------//
    // Open the map
    MgMap map = new MgMap(siteConnection);
    map.Open(mapName);

    // ...
    // --------------------------------------------------//
    // Load a layer from XML, and use the DOM to change it

    // Load the prototype layer definition into
    // a PHP DOM object.
    XmlDocument domDocument = new XmlDocument();
    String layerDefPath = Server.MapPath("RecentlyBuilt.LayerDefinition");
    if (!File.Exists(layerDefPath))
    {
        Response.Write("The layer definition 'RecentlyBuilt.LayerDefinition' could not be found.<BR>\n");
        return;
    }
    domDocument.Load(layerDefPath);

    // Get a list of all the <AreaRule><Filter> elements in
    // the XML.
    XmlNodeList nodes = domDocument.SelectNodes("//AreaRule/Filter");
    // Find the correct node and change it
    foreach (XmlNode node in nodes)
    {
        if (node.InnerText == "YRBUILT > 1950")
        {
            node.InnerText = "YRBUILT > 1980";
        }
    }

    // Get a list of all the <LegendLabel> elements in the
    // XML.
    nodes = domDocument.SelectNodes("//LegendLabel");
    // Find the correct node and change it
    foreach (XmlNode node in nodes)
    {
        if (node.InnerText == "Built after 1950")
        {
            node.InnerText = "Built after 1980";
        }
    }

**Java**
    
.. highlight:: java
.. code-block:: java

    //This code fragment assumes you have imported the org.osgeo.mapguide namespace
    
    //Code sample not yet available

.. note::

    Due to the abundance of various XML libraries available for .net and Java, the above examples should
    be considered as one way for modifying XML. By no means is this the **definitive** way.

The page then goes on to save the XML to a resource and loads that resource
into the map, as described in :ref:`adding-layers-to-a-map`.

If you wish to modify an existing layer that is visible in other users' maps,
without affecting those maps:

 1. Copy the layer to the user's session repository.
 2. Modify the layer and save it back to the session repository.
 3. Change the user's map to refer to the modified layer.

See :ref:`adding-layers-to-a-map`

.. index::
    single: LayerDefinitionFactory

Another Way to Create Layers
----------------------------

.. todo::
    Update Maestro API link

The method described in the previous section is easy to use, but requires a
layer definition be created first through the UI. An alternative approach is to
use the methods defined in

``C:\Program Files\OSGeo\MapGuide\Web\www\mapviewerphp\layerdefinitionfactory.php``

This file contains several functions, which can be used to build up a layer
definition. The parameters of these functions enable you to set the most
commonly used settings. (If you need to change other settings, you will have
to either use the UI, or modify the XML of the layer definition.)

The ``layerdefinitionfactory`` is only available for PHP. For development using
ASP.NET, a good alternative is to use the Visual Studio tool ``xsd.exe`` to generate
.NET classes for the LayerDefinition schema, or to use the strongly-typed resource classes
in the :ref:`maestroapi`

``CreateLayerDefinition($resourceId, $featureClass, $geometry, $featureClassRange)``

 * ``$resourceId`` - The repository path of the feature source for the layer. For example: ``Library://Samples/Sheboygan/Data/Parcels.FeatureSource``
 * ``$featureClass`` - The feature class to use. For example, ``SHP_Schema:Parcels``
 * ``$geometry`` - The geometry to use from the feature class. For example, ``SHPGEOM``
 * ``$featureClassRange`` - A scale range created by filling in a scale range template (``ScaleRange.templ``)
 
``CreateScaleRange($minScale, $maxScale, $typeStyle)``

 * ``$minScale`` - The minimum scale range to which this rule applies.
 * ``$maxScale`` - The maximum scale range to which this rule applies.
 * ``$typeStyle`` - A type style created by using ``CreateAreaTypeStyle()``, ``CreateLineTypeStyle()`` or ``CreatePointTypeStyle()``
 
``CreateAreaTypeStyle($areaRules)``

 * ``$areaRules`` - One or more area rules, created by ``CreateAreaRule()``
 
``CreateAreaRule($legendLabel, $filterText, $foregroundColor)``

 * ``$legendLabel`` - The text for the label shown beside this rule in the legend.
 * ``$filterText`` - The filter expression that determines which features match this rule. For example ``SQFT >= 1 AND SQFT < 800``
 * ``$foregroundColor`` - The color to be applied to areas that match this rule.

``CreateTextSymbol($text, $fontHeight, $foregroundColor)``

 * ``$text`` - The string for the text
 * ``$fontHeight`` - The height for the font
 * ``$foregroundColor`` - The foreground color
 
``CreatePointTypeStyle($pointRule)``

 * ``$pointRule`` - One or more point rules, created by ``CreatePointRule()``
 
``CreatePointRule($legendLabel, $filter, $label, $pointSym)``

 * ``$legendLabel`` - The label shown beside this rule in the legend.
 * ``$filter`` - The filter expression that determines which features match this rule.
 * ``$label`` - The text symbol, created by ``CreateTextSymbol()``
 * ``$pointSym`` - A mark symbol created by ``CreateMarkSymbol()``

``CreateMarkSymbol($resourceId, $symbolName, $width, $height, $color)``

 * ``$resourceId`` - The resource ID of the symbol used to mark each point. For example ``Library://Samples/Sheboygan/Symbols/BasicSymbols.SymbolLibrary``
 * ``$symbolName`` - The name of the desired symbol in the symbol library/
 * ``$width`` - The width of the symbol (in points)
 * ``$height`` - The height of the symbol (in points)
 * ``$color`` - The color of the symbol.
 
``CreateLineTypeStyle($lineRules)``
 
 * ``$lineRules`` - One or more rules, created by ``CreateLineRule()`` 
 
``CreateLineRule($color, $legendLabel, $filter)``

 * ``$color`` - The color to be applied to lines that match this rule.
 * ``$legendLabel`` - The label shown beside this rule in the legend.
 * ``$filter`` - The filter expression that determines which features match this rule.

For more information on these settings, see the MapGuide Studio Help.

.. index::
    single: LayerDefinitionFactory; Area Rules

Example: Creating a layer that users Area Rules
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. note::
    
    This example specifically uses the ``LayerDefinitionFactory`` class which is not available for .net and Java

This example shows how to create a new layer using the factory. This layer
uses three area rules to theme parcels by their square footage.

**PHP**

.. highlight:: php
.. code-block:: php

    // ...
    /---------------------------------------------------//
     $factory = new LayerDefinitionFactory();
    /// Create three area rules for three different
    // scale ranges.
    $areaRule1 = $factory->CreateAreaRule( '1 to 800', 'SQFT &gt;= 1 AND SQFT &lt; 800', 'FFFFFF00');
    $areaRule2 = $factory->CreateAreaRule( '800 to 1600','SQFT &gt;= 800 AND SQFT &lt; 1600', 'FFFFBF20');
    $areaRule3 = $factory->CreateAreaRule('1600 to 2400', 'SQFT &gt;= 1600 AND SQFT &lt; 2400', 'FFFF8040');
    // Create an area type style.
    $areaTypeStyle = $factory->CreateAreaTypeStyle($areaRule1 . $areaRule2 . $areaRule3);
    // Create a scale range.
    $minScale = '0';
    $maxScale = '1000000000000';
    $areaScaleRange = $factory->CreateScaleRange($minScale, $maxScale, $areaTypeStyle);
    // Create the layer definiton.
    $featureClass = 'Library://Samples/Sheboygan/Data/' . 'Parcels.FeatureSource';
    $featureName = 'SHP_Schema:Parcels';
    $geometry = 'SHPGEOM';
    $layerDefinition = $factory->CreateLayerDefinition($featureClass, $featureName, $geometry, $areaScaleRange);
    //---------------------------------------------------//
    // ...
    
The script then saves the XML to a resource and loads that resource into the
map. See :ref:`adding-layers-to-a-map`.

.. index::
    single: LayerDefinitionFactory; Line Rules

Example: Using Line Rules
^^^^^^^^^^^^^^^^^^^^^^^^^

.. note::
    
    This example specifically uses the ``LayerDefinitionFactory`` class which is not available for .net and Java

Creating line-based rules is very similar.

**PHP**

.. highlight:: php
.. code-block:: php

    // ...
    //---------------------------------------------------//
    $factory = new LayerDefinitionFactory();
    // Create a line rule.
    $legendLabel = '';
    $filter = '';
    $color = 'FF0000FF';
    $lineRule = $factory->CreateLineRule($legendLabel, $filter, $color);
    // Create a line type style.
    $lineTypeStyle = $factory->CreateLineTypeStyle($lineRule);
    // Create a scale range.
    $minScale = '0';
    $maxScale = '1000000000000';
    $lineScaleRange = $factory->CreateScaleRange($minScale, $maxScale, $lineTypeStyle);
    // Create the layer definiton.
    $featureClass = 'Library://Samples/Sheboygan/Data/' . 'HydrographicLines.FeatureSource';
    $featureName = 'SHP_Schema:HydrographicLines';
    $geometry = 'SHPGEOM';
    $layerDefinition = $factory->CreateLayerDefinition($featureClass, $featureName, $geometry, $lineScaleRange);
    //---------------------------------------------------//
    // ...

.. index::
    single: LayerDefinitionFactory; Point Rules

Example: Using Point Rules
^^^^^^^^^^^^^^^^^^^^^^^^^^

.. note::
    
    This example specifically uses the ``LayerDefinitionFactory`` class which is not available for .net and Java

To create point-based rules, three methods are used.

**PHP**

.. highlight:: php
.. code-block:: php

    // ...
    //---------------------------------------------------//
    $factory = new LayerDefinitionFactory();
    // Create a mark symbol
    $resourceId = 'Library://Samples/Sheboygan/Symbols/BasicSymbols.SymbolLibrary';
    $symbolName = 'PushPin';
    $width = '24'; // points
    $height = '24'; // points
    $color = 'FFFF0000';
    $markSymbol = $factory->CreateMarkSymbol($resourceId, $symbolName, $width, $height, $color);

    // Create a text symbol
    $text = "ID";
    $fontHeight="12";
    $foregroundColor = 'FF000000';
    $textSymbol = $factory->CreateTextSymbol($text, $fontHeight, $foregroundColor);
    // Create a point rule.
    $legendLabel = 'trees';
    $filter = '';
    $pointRule = $factory->CreatePointRule($legendLabel, $filter, $textSymbol, $markSymbol);
     
    // Create a point type style.
    $pointTypeStyle = $factory->CreatepointTypeStyle($pointRule);
     
    // Create a scale range.
    $minScale = '0';
    $maxScale = '1000000000000';
    $pointScaleRange = $factory->CreateScaleRange($minScale, $maxScale, $pointTypeStyle);
    // Create the layer definiton.
    $featureClass = 'Library://Tests/Trees.FeatureSource';
    $featureName = 'Default:Trees';
    $geometry = 'Geometry';
    $layerDefinition = $factory->CreateLayerDefinition($featureClass, $featureName, $geometry, $pointScaleRange);
    //---------------------------------------------------//

.. index::
    single: MgMap; Adding Layers

.. _adding-layers-to-a-map:

Adding Layers to a Map
----------------------

The preceding examples have created or modified the XML for layer definitions
in memory. To add those layers to a map:

1. Save the layer definition to a resource stored in the session repository.
2. Add that resource to the map.

This function adds takes a layer's XML, creates a resource in the session
repository from it, and adds that layer resource to a map.

**PHP**

.. highlight:: php
.. code-block:: php

    require_once('../common/common.php');

    function add_layer_definition_to_map($layerDefinition, $layerName, $layerLegendLabel, $sessionId, $resourceService, &$map)
    // Adds the layer definition (XML) to the map.
    // Returns the layer.
    {
        global $schemaDirectory;

        // Validate the XML.
        $domDocument = new DOMDocument;
        $domDocument->loadXML($layerDefinition);
        if (! $domDocument->schemaValidate($schemaDirectory . "LayerDefinition-1.3.0.xsd") ) // $schemaDirectory is defined in common.php
        {
            echo "ERROR: The new XML document is invalid.<BR>\n.";
            return NULL;
        }    

        // Save the new layer definition to the session repository  
        $byteSource = new MgByteSource($layerDefinition, strlen($layerDefinition));
        $byteSource->SetMimeType(MgMimeType::Xml);
        $resourceID = new MgResourceIdentifier("Session:$sessionId//$layerName.LayerDefinition");
        $resourceService->SetResource($resourceID, $byteSource->GetReader(), null);

        $newLayer = add_layer_resource_to_map($resourceID, $resourceService, $layerName, $layerLegendLabel, $map);

        return $newLayer;
    }

This function adds a layer resource to a map.

**PHP**

.. highlight:: php
.. code-block:: php

    function add_layer_resource_to_map($layerResourceID, $resourceService, $layerName, $layerLegendLabel, &$map)
    // Adds a layer defition (which can be stored either in the Library or a session
    // repository) to the map.
    // Returns the layer.
    {
        $newLayer = new MgLayer($layerResourceID, $resourceService);  

        // Add the new layer to the map's layer collection
        $newLayer->SetName($layerName);
        $newLayer->SetVisible(true);
        $newLayer->SetLegendLabel($layerLegendLabel);
        $newLayer->SetDisplayInLegend(true);
        $layerCollection = $map->GetLayers(); 
        if (! $layerCollection->Contains($layerName) )
        {
            // Insert the new layer at position 0 so it is at the top
            // of the drawing order
            $layerCollection->Insert(0, $newLayer); 
        }

        return $newLayer;
    }

This function adds a layer to a legend's layer group.

**PHP**

.. highlight:: php
.. code-block:: php

    function add_layer_to_group($layer, $layerGroupName, $layerGroupLegendLabel, &$map)
    // Adds a layer to a layer group. If necessary, it creates the layer group.
    {
      
        // Get the layer group
        $layerGroupCollection = $map->GetLayerGroups();
        if ($layerGroupCollection->Contains($layerGroupName))
        {
            $layerGroup = $layerGroupCollection->GetItem($layerGroupName);
        }
        else
        {
            // It does not exist, so create it
            $layerGroup = new MgLayerGroup($layerGroupName); 
            $layerGroup->SetVisible(true);
            $layerGroup->SetDisplayInLegend(true);
            $layerGroup->SetLegendLabel($layerGroupLegendLabel);
            $layerGroupCollection->Add($layerGroup); 
        }

        // Add the layer to the group
        $layer->SetGroup($layerGroup);  
    }

.. index::
    single: Layers; Making permanent changes

Making Changes Permanent
------------------------

So far, all the examples in this chapter have only affected the user's runtime
version of the map. No other users see those changes, and when the current
user logs out those changes will be lost.

To make changes permanent, the script can save the modified layer back into
the Library.

**PHP**

.. highlight:: php
.. code-block:: php

    $byteSource = new MgByteSource($layerDefinition, strlen($layerDefinition));
    $byteSource->SetMimeType(MgMimeType::Xml);
    $resourceId = new MgResourceIdentifier("Library://LayerName.LayerDefinition");
    $resourceService->SetResource($resourceId, $byteSource->GetReader(), null);

.. note::

    Due to security restrictions imposed on the Library repository. Make sure the 
    resource service object was created from a site connection that was initialized by
    a login or session id that has the appropriate permissions to write to the Library
    repository. 
    
    The ``Anonymous`` user does not have write access to the Library repository
Interacting With Layers
=======================

Overview of Layers
------------------

.. tip::
    
    The Interacting With Layers sample, in the Developer's Guide samples, demonstrates concepts from this chapter.

Layers represent vector data, raster data, and drawing data in a map. Each type
of layer has unique characteristics.

.. note::

    The word layer has different meanings in different contexts. A layer can refer
    to the layer definition in the resource repository, and it can also refer to the map
    layer. For the purposes of the Web Tier, a layer refers to a map layer, and a layer
    definition refers to the layer definition in the resource repository.

.. index::
    single: Layers; Basic Properties

Basic Layer Properties
^^^^^^^^^^^^^^^^^^^^^^

.. todo::
    Replace page number references with section links

A map contains one or more layers (``MgLayer`` objects) that are rendered to create
a composite image. Each layer has properties that determine how it displays in
the map and map legend. Some of the properties are:

 * Layer name: A unique identifier
 * Legend label: The label for the layer as it appears in the map legend.
 * Visibility: whether the layer should be displayed in the map. Note that actual visibility is dependent on more than just the visibility setting for a layer. See Layer Visibility on page 31 for further details.
 * Selectable: Whether features in the layer are selectable. This only applies to layers containing feature data.

The ``MgMap::GetLayers()`` method returns an ``MgLayerCollection`` object that
contains all the layers in the map. The ``MgLayerCollection::GetItem()`` method
returns an individual ``MgLayer`` object, by either index number in the collection
or layer name.

Layers in the collection are sorted by drawing order, with the top layers at the
beginning of the collection. For example, using PHP syntax, if ``$layers`` is a
collection containing the layers in a map, then ``$layers->GetItem(0)`` returns
the top-most layer.

.. index::
    single: Layer Groups

Layer Groups
^^^^^^^^^^^^

Layers can be optionally grouped into layer groups. Layers in the same group
are displayed together in the legend.

The visibility for all layers in a group can be set at the group level. If the group
visibility is turned off then none of the layers in the group will be visible,
regardless of their individual visibility settings. If the group visibility is turned
on, then individual layers within the group can be made visible or not visible
separately.

Layer groups can be nested so a group can contain other groups. This provides
a finer level of control for handling layer visibility or legend groups.

The ``MgMap::GetLayerGroups()`` method returns an ``MgLayerGroupCollection``
object that contains all the layer groups in the map.

Each layer group in a map must have a unique name, even if it is nested within
another group.

.. index::
    single: Base Layer Groups

Base Layer Groups
"""""""""""""""""

The AJAX viewer can use *base layer groups* to optimize image rendering times.
Layers in a base layer group are rendered together to generate a single raster
image that can be sent to the viewer. The image is divided into tiles so only
the required tiles need to be rendered and sent, instead of the entire image.
Tiles are cached on the server; if a tile already exists in the cache it does not
need to be rendered before being sent.

Each base layer group has a series of pre-defined scales that are used for
rendering. When a request is made to view a portion of the map at a given
scale, the AJAX viewer renders the tiles at the pre-defined scale that is closest
to the requested map view scale.

Layers within a base layer group are rendered together. Visibility settings for
individual layers are ignored and the visibility setting for the group is used
instead.

Layers above the base layers will generally be vector layers with transparent
backgrounds. This makes the images small and relatively quick to load in the
viewer.

You may have more than one base layer group. Lower layers will be hidden
by higher layers unless the higher layers have transparent areas or have their
visibility turned off.

.. note::
    
    A layer can only belong to one group at a time. It cannot be part of both a base layer group and a regular group.

.. index::
    single: Layers; Styles

Layer Style
^^^^^^^^^^^

.. todo::
    Replace page number references with section links

The data source information and style information for a layer control how
the layer looks when it displayed on a map. This is stored in the layer definition
in the repository. To change any of the data source or style information,
modify the layer definition.

Layer definitions can be modified using . They can also be created and modified
dynamically using the Web Extensions API. See Modifying Maps and Layers
on page 57 for details.

.. index::
    single: Layers; Visiblity

Layer Visibility
^^^^^^^^^^^^^^^^

Whether a layer is visible in a given map depends on three criteria:

 * The visibility setting for the layer
 * The visibility settings for any groups that contain the layer
 * The map view scale and the layer definition for that view scale
 
In order for a layer to be visible, its layer visibility must be on, the visibility
for any group containing the layer must be on, and the layer must have a style
setting defined for the current map view scale.

.. index::
    single: Layers; Actual Visiblity

Example: Actual Visibility
""""""""""""""""""""""""""

For example, assume that there is a layer named Roads that is part of the layer
group Transportation. The layer has view style defined for the scale ranges
``0 to 10000`` and ``10000 to 24000``.

The following table shows some possible settings of the various visibility and
view scale settings, and their effect on the actual layer visibility.

+------------------+------------------+------------+------------------+
| Layer Visibility | Group Visibility | View Scale | Actual Visbility |
+==================+==================+============+==================+
| On               | On               | 10000      | On               |
+------------------+------------------+------------+------------------+
| On               | On               | 25000      | Off              |
+------------------+------------------+------------+------------------+
| On               | Off              | 10000      | Off              |
+------------------+------------------+------------+------------------+
| Off              | On               | 10000      | Off              |
+------------------+------------------+------------+------------------+

.. index::
    single: Layers; Enumerating Map Layers
    single: MgMap; Enumerating Layers

Enumerating Map Layers
----------------------

Map layers are contained within an ``MgMap`` object. To list the layers in a map,
use the ``MgMap::GetLayers()`` method. This returns an ``MgLayerCollection``
object.

To retrieve a single layer, use the ``MgLayerCollection::GetItem`` method with
either an integer index or a layer name. The layer name is the name as defined
in the map, not the name of the layer definition in the repository.

For example:

**PHP**

.. highlight:: php
.. code-block:: php

    $layer = $layers->GetItem('Roads');

**.net (C#)**

.. highlight:: csharp
.. code-block:: csharp

    //This code fragment assumes the OSGeo.MapGuide namespace has been imported
    MgLayer layer = (MgLayer)layers.GetItem("Roads");

**Java**
    
.. highlight:: java
.. code-block:: java

    //This code fragment assumes the org.osgeo.mapguide package has been imported
    MgLayer layer = (MgLayer)layers.GetItem("Roads");

Example
^^^^^^^

The following example lists the layers in a map, along with an indicator of
the layer visibility setting.

**PHP**

.. highlight:: php
.. code-block:: php

    $layers = $map->GetLayers(); // Get layer collection
    echo "<p>Layers:<br />";
    $count = $layers->GetCount();
    for ($i = 0; $i < $count; $i++)
    {
        $layer = $layers->GetItem($i);
        echo $layer->GetName() . ' (' .
        ($layer->GetVisible() ? 'on' : 'off') . ')<br />';
    }
    echo '</p>';
    
**.net (C#)**

.. highlight:: csharp
.. code-block:: csharp

    //This code fragment assumes the OSGeo.MapGuide namespace has been imported
    MgLayerCollection layers = map.GetLayers();
    Response.Write("<p>Layers:<br />");
    int count = layers.GetCount();
    for (int i = 0; i < count; i++)
    {
        MgLayer layer = (MgLayer)layers.GetItem(i);
        Response.Write(layer.GetName() + " (" + (layer.GetVisible() ? "on" : "off") + ")<br/>");
    }
    Repsonse.Write("</p>");

**Java**
    
.. highlight:: java
.. code-block:: java

    //This code fragment assumes the org.osgeo.mapguide package has been imported
    MgLayerCollection layers = map.GetLayers();
    response.getWriter().write("<p>Layers:<br />");
    int count = layers.GetCount();
    for (int i = 0; i < count; i++)
    {
        MgLayer layer = (MgLayer)layers.GetItem(i);
        response.getWriter().write(layer.GetName() + " (" + (layer.GetVisible() ? "on" : "off") + ")<br/>");
    }
    response.getWriter().write("</p>");

.. index::
    single: Layers; Manipulation

Manipulating Layers
-------------------

.. todo::
    Replace page number references with section links

Modifying basic layer properties and changing layer visibility settings can be
done directly using API calls. More complex manipulation requires modifying
layer resources in the repository. For details, see Modifying Maps and Layers
on page 57.

.. index::
    single: Layers; Changing Basic Properties

Changing Basic Properties
^^^^^^^^^^^^^^^^^^^^^^^^^

To query or change any of the basic layer properties like name, label, or group,
use the ``MgLayer::GetProperty()`` and ``MgLayer::SetProperty()`` methods,
where Property is one of the layer properties. You must save and refresh the
map for the changes to take effect.

Example
"""""""

.. todo::
    Update code samples to not use MgMap() ctor. That is deprecated.

The following example toggles the label of the Roads layer between Roads and
Streets.

**PHP**

.. highlight:: php
.. code-block:: php

    MgInitializeWebTier ($webconfigFilePath);
     
    $userInfo = new MgUserInformation($mgSessionId);
    $siteConnection = new MgSiteConnection();
    $siteConnection->Open($userInfo);
    $resourceService =
    $siteConnection->CreateService(MgServiceType::ResourceService);
    $map = new MgMap();
    $map->Open($resourceService, $mgMapName);
    $layers = $map->GetLayers();
    $roadLayer = $layers->GetItem('Roads');
    $roadLabel = $roadLayer->GetLegendLabel();
    if ($roadLabel == 'Roads')
        $newLabel = 'Streets';
    else
        $newLabel = 'Roads';
    $roadLayer->SetLegendLabel($newLabel);
    // You must save the updated map or the
    // changes will not be applied
    // Also be sure to refresh the map on page load.
    $map->Save($resourceService);
    
**.net (C#)**

.. highlight:: csharp
.. code-block:: csharp

    //This code fragment assumes the OSGeo.MapGuide namespace has been imported
    MapGuideApi.MgInitializeWebTier(webConfigFilePath);
    MgUserInformation userInfo = new MgUserInformation(mgSessionId);
    MgSiteConnection siteConnection = new MgSiteConnection();
    siteConnection.Open(userInfo);
    MgResourceService resourceService = (MgResourceService)siteConnection.CreateService(MgServiceType.ResourceService);
    MgMap map = new MgMap();
    map.Open(resourceService, mgMapName);
    MgLayerCollection layers = map.GetLayers();
    MgLayer roadLayer = layers.GetItem("Roads");
    String roadLabel = roadLayer.GetLegendLabel();
    String newLabel = "Roads";
    if (roadLabel == "Roads")
        newLabel = "Streets";
    roadLayer.SetLegendLabel(newLabel);
    // You must save the updated map or the
    // changes will not be applied
    // Also be sure to refresh the map on page load.
    map.Save(resourceService);

**Java**
    
.. highlight:: java
.. code-block:: java

    //This code fragment assumes the org.osgeo.mapguide has been imported
    MapGuideApi.MgInitializeWebTier(webConfigFilePath);
    MgUserInformation userInfo = new MgUserInformation(mgSessionId);
    MgSiteConnection siteConnection = new MgSiteConnection();
    siteConnection.Open(userInfo);
    MgResourceService resourceService = (MgResourceService)siteConnection.CreateService(MgServiceType.ResourceService);
    MgMap map = new MgMap();
    map.Open(resourceService, mgMapName);
    MgLayerCollection layers = map.GetLayers();
    MgLayer roadLayer = layers.GetItem("Roads");
    String roadLabel = roadLayer.GetLegendLabel();
    String newLabel = "Roads";
    if (roadLabel == "Roads")
        newLabel = "Streets";
    roadLayer.SetLegendLabel(newLabel);
    // You must save the updated map or the
    // changes will not be applied
    // Also be sure to refresh the map on page load.
    map.Save(resourceService);

.. index::
    single: Layers; Changing Visibility

Changing Visibility
^^^^^^^^^^^^^^^^^^^

.. todo::
    Replace page number references with section links

To query the actual layer visibility, use the ``MgLayer::IsVisible()`` method.
There is no method to set actual visibility because it depends on other visibility
settings.

To query the visibility setting for a layer, use the ``MgLayer::GetVisible()``
method. To change the visibility setting for a layer, use the
``MgLayer::SetVisible()`` method.

To query the visibility setting for a layer group, use the ``MgLayerGroup::GetVisible()``
method. To change the visibility setting for a layer group, use the
``MgLayerGroup::SetVisible()`` method.

To change the layer visibility for a given view scale, modify the layer resource
and save it back to the repository. See Modifying Maps and Layers on page
57 for details.

The following example turns on the visibility for the Roads layer.

**PHP**

.. highlight:: php
.. code-block:: php

    $layers = $map->GetLayers();
    $roadsLayer = $layers->GetItem('Roads');
    $roadsLayer->SetVisible(True);

**.net (C#)**

.. highlight:: csharp
.. code-block:: csharp

    //This code fragment assumes the OSGeo.MapGuide namespace has been imported
    MgLayerCollection layers = map.GetLayers();
    MgLayer roadsLayer = layers.GetItem("Roads");
    roadsLayer.SetVisible(true);

**Java**
    
.. highlight:: java
.. code-block:: java

    //This code fragment assumes the org.osgeo.mapguide package has been imported
    MgLayerCollection layers = map.GetLayers();
    MgLayer roadsLayer = layers.GetItem("Roads");
    roadsLayer.SetVisible(true);
    
.. note::

    Changing the visibility (or any other layer changes) will have no effect until the map is saved and refreshed.
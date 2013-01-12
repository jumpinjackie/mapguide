.. index:
   single: fusion

Fusion
======

Introduction
------------

Fusion is a web-mapping application development framework for `MapGuide Open Source <http://mapguide.osgeo.org>`_ / `Autodesk Infrastructure Map Server <http://usa.autodesk.com/adsk/servlet/pc/index?id=16505474&siteID=123112>`_ that is built primarily in JavaScript.

It allows non-spatial web developers to build rich mapping applications quickly and easily.

Based on the defacto standard spatial javascript API `OpenLayers <http://www.openalayers.org>`_, you can extend it easily.

Using widgets, developers are able to add, remove, or modify functionality using standard-compliant HTML and CSS.

Fusion does not require any proprietary browser plug-ins and works in `all the major browsers on Windows, Mac, and Linux <http://trac.osgeo.org/fusion/wiki/TechSpecs>`_.

Fusion is also referred to as Flexible Layouts inside Autodesk MapGuide Studio 

Essential Concepts
------------------

It is very important that you understand how MapGuide works. MapGuide comes with its own extensive documentation. If you are not familiar with creating MapDefinition resources, please review the MapGuide documentation.

Fusion is all about creating a web application. You will need some familiarity with HTML, JavaScript?, CSS.

The primary development language for building Fusion applications is JavaScript, but if you need to add server-side functionality using the MapGuide API, then a knowledge of PHP is also required.

Fusion makes extensive use of static and dynamically generated HTML. You need to be familiar with the Document Object Model (DOM) implemented by various browsers, and the subtle differences between the browsers in how scripting with the DOM works. Although it is possible to use Fusion without any DOM scripting, it is likely that most applications will need to use some scripting to achieve their desired functionality.

Fusion makes extensive use of CSS to achieve its look and feel in a minimally intrusive way. The design principle behind much of the structure of the user interface components of Fusion is to let the browser do as much of the work as possible with its native layout engine. This is the most efficient and natural design for web pages, and reduces the amount of javascript code that needs to be executed by the client's browser. You will need to thoroughly understand CSS design and implementation concepts in order to fully customize the Fusion interface, but to use the default look and feel of Fusion with moderate customization does not require advanced CSS knowledge. 

.. _hello-sheboygan:

Hello Sheboygan - An example template
-------------------------------------

Creating a new Fusion application is really quite straight-forward. The simplest way to demonstrate the core concepts of Fusion is to build a small, working application. The Hello Sheboygan application is going to have a map and three navigation tools (zoom in, zoom out, and pan). It will be fully functional, albeit quite plain!

<image of app here>

.. note::

    This section is simply to demonstrate the inner workings of a Fusion template. For practical purposes, you would probably be using the using one of the 5 available templates already provided by MapGuide: 
    
         * `Slate` 
         * `Maroon`
         * `Aqua`
         * `LimeGold` 
         * `TurquoiseYellow`
         
    In such cases, any such custom functionality you want to add into a Fusion application would be done through widgets. See :ref:`widget-basics` for more information.

First, create a directory on your server to hold all the files required by the application. The directory needs to be in a web-accessible location. You can always create a directory in your www folder of your MapGuide Open Source installation if you don't know where else to put it. Call this directory hello.

The next step is to decide what functionality, or widgets, are needed by the application. The list (from the description above) is:

    * a Map widget
    * a Zoom In widget
    * a Zoom Out widget
    * a Pan widget 

These widgets need to be described in an ApplicationDefinition xml file. Fusion allows you to store the ApplicationDefinition resource in the MapGuide repository or as a separate XML file. For the purpose of this document, we will be assuming that you are storing the ApplicationDefinition resource as a separate XML file. There are instructions at the end of this document for using an ApplicationDefinition resource stored in a MapGuide repository.

By convention, the name of the file is ApplicationDefinition.xml. So, create a new file in the hello directory and name it ApplicationDefinition.xml. For our very simple example, we need an ApplicationDefinition tag with a Title tag, a MapGroup tag, and a WidgetSet tag to hold our four widgets (see the ApplicationDefinition page for a more complete description). Individual widgets are represented as Widget tags inside the WidgetSet tag where the actual Widget is specified by the Type tag and the HTML element that the Widget will be created in is specified by the Name tag. Our four widgets each have a specific Widget tag with several options, most of which are not required for this example. The resulting XML for Hello Sheboygan is: 

.. highlight:: xml
.. code-block:: xml

    <ApplicationDefinition 
          xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" 
          xmlns:xsd="http://www.w3.org/2001/XMLSchema" 
          xsi:noNamespaceSchemaLocation="ApplicationDefinition-1.0.0.xsd">
      <Title>Hello Sheboygan</Title>
      <MapGroup>
        <Map xsi:type="MapGuideLayerType">
          <Type>MapGuide</Type>
          <SingleTile>true</SingleTile>
          <Extension>
            <ResourceId>Library://Samples/Sheboygan/Maps/Sheboygan.MapDefinition</ResourceId> 
          </Extension>
        </Map>
      </MapGroup>
      <WidgetSet>
        <MapWidget>
          <Name>Map</Name>
          <Type>Map</Action>
          <StatusText/>
          <MapId>HelloSheboygan</MapId>
        </Widget>
        <Widget>
          <Name>ZoomIn</Name>
          <Type>Zoom</Type>
          <StatusText/>
          <Extension>
            <Direction>in</Direction>
          </Extension>
          <ImageUrl>images/icons/zoom-in.png</ImageUrl>
          <ImageClass/>
          <Label/>
          <Tooltip>Click or click and drag on the map to zoom in</Tooltip>
          <Disabled/>
        </Widget>
        <Widget>
          <Name>ZoomOut</Name>
          <Type>Zoom</Type>
          <StatusText/>
          <Extension>
            <Direction>out</Direction>
          </Extension>
          <ImageUrl>images/icons/zoom-out.png</ImageUrl>
          <ImageClass/>
          <Label/>
          <Tooltip>Click on the map to zoom out</Tooltip>
          <Disabled/>
        </Widget>
        <Widget>
          <Name>Pan</Name>
          <Type>Pan</Type>
          <StatusText/>
          <Extension/>
          <ImageUrl>images/icons/pan.png</ImageUrl>
          <ImageClass/>
          <Label/>
          <Tooltip>Click and drag the map to pan</Tooltip>
          <Disabled/>
        </Widget>    
      </WidgetSet>
      <Extension />
    </ApplicationDefinition>

The next thing that our `Hello World` application needs is a template, which is simply an html file. Create a new file in the `hello` directory and name it index.html. Here's the `index.html` file contents: 

.. highlight:: html
.. code-block:: html

    <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
    <html>
    <head>
    <title>Hello Sheboygan</title>
    <!-- change the source of the following tag to point to your fusion installation -->
    <script type="text/javascript" src="../fusion/lib/fusion.js"></script>
    <script type="text/javascript">
    window.onload = function() {
        Fusion.initialize();
    }
    </script>
    </head>
    <body>
    <div id="Toolbar" style="position: relative; width: 400px; height: 24px">
        <div id="ZoomIn"></div>
        <div id="ZoomOut"></div>
        <div id="Pan"></div>
    </div>
    <div id="Map" style="position: relative; width: 400px; height: 400px"></div>
    </body>
    </html>

It is highly recommended to use a valid doctype for your Fusion application template pages. If you omit the doctype or use an invalid doctype, most browsers will revert to a mode called `Quirks Mode`. Fusion will mostly function correctly, but you may notice some minor issues and your application may appear differently depending on the browser. By using a valid HTML or XHTML doctype, browsers will use a mode called `Standards Compliant Mode` and your application will work consistently between different browsers.

You will need to make sure that the **src** of the script tag that points to `fusion.js` is a valid path, it can be relative (as in the example above) or absolute (starting with `http://`). If it is absolute, then the url must be pointing to the same server as the url you use to load the application.

That is all that is needed to build a fully functional Fusion application. Save your files and then load the application by entering the URL to the `index.html` page in your web browser. If everything is set up correctly, you should see three buttoms appear, followed shortly by a square map image below the buttons. You can click any of the buttons to activate them, and use them on the map. 

Under the hood
^^^^^^^^^^^^^^

As you can see, quite a lot of things are going on without a lot of code. Adding other widgets is as simple as adding an HTML container for them in the HTML page and adding a Widget in the WidgetSet. So how exactly does Fusion turn this small amount of HTML and XML into a functional web mapping application?

The first important step is including fusion.js in your web page (the first script tag in the :ref:`hello-sheboygan` example). This makes Fusion available to your application. The second important step is invoking the `Fusion.initialize()` method during (or after) the **onload** event of the web page (the contents of the second script tag). `Fusion.initialize()` starts the process of getting the application running and the following steps happen:

    * the Fusion configuration file (config.json) is loaded and parsed
    * the ApplicationDefinition configuration file is loaded and parsed
    * Widgets with associated HTML elements are identified
    * the JavaScript file associated with each identified Widget is loaded
    * any dependencies of the Widget are loaded
    * when all Widgets (and their dependencies such as CSS files) are loaded, a new instance of each Widget is created for each HTML element.
    * when all Widget have been initialized, the `FUSION_INITIALIZED` event is emitted. 

An important step in this process is the loading of the MapWidget(s). After each MapWidget has been created, it:

    * attempts to create a session with the server for that Map instance. All Map instances of the same Type will share the same session.
    * if an initial Map is specified:
        * issue a `MAP_LOADING` event
        * attempt to load the Map (which will wait until the session has been created)
        * issue a `MAP_LOADED` event when the map has finished loading 

All Widgets in a WidgetSet are associated with the MapWidget of that WidgetSet. 

How Fusion is integrated with MapGuide
--------------------------------------

MapGuide Open Source architecture is divided into three tiers:

Server Tier:

    This tier runs the MapGuide Open Source server. Fusion is not integrated into this tier.

Web Extensions Tier:

    This tier runs with a Web Server. Fusion is installed in this tier and makes use of the MapGuide Open Source Web Extensions API to implement some functionality.

Client Tier:

    This tier runs in the client's web browser. Most of Fusion exists in this tier, and it completely replaces the existing MapGuide Open Source client tier (known as the AJAX viewer).

The Fusion Server components work directly with the MapGuide Open Source PHP API to implement specific functionality such as buffering features and working with selections.

The Fusion JavaScript API and Widget are installed with the MapGuide Open Source Web Tier, but they are actually loaded up to and executed in the Client Tier. 

Application Definitions, HTML and Widgets
-----------------------------------------

The API and Widget are linked to specific HTML elements within an HTML page through the ApplicationDefinition XML document. A Widget is the executable code (JavaScript class) that is loaded when Fusion identifies an HTML element id and Widget Name that are the same. The Type of the Widget identifies the exact JavaScript file to be loaded and the JavaScript object that needs to be instantiated to create a functional Widget. The runtime instance of the JavaScript object is the Widget.

When the runtime instance is created, it uses the HTML element id to determine where in the HTML page it should display its user interface. Any custom configuration for the Widget is placed in the ApplicationDefinition Widget XML block as sub tags (each Widget has its own list of what customization is possible, see the Widget API Reference for details).

If you are customizing the look and feel of a single Widget by overriding the CSS styling for the Widget, this is normally done by using the #<id> syntax to limit your changes to that specific Widget. 

.. _widget-basics:

Widget Basics
-------------

Widgets are the discrete pieces of functionality that your Fusion application is composed of. The standard fusion template provides a lot of widgets out of the box, and a new Application Definition created by an authoring tool like MapGuide Maestro will include many of these widgets for you. So a default application is already pretty rich in functionality.

Whereas custom commands are the way to add new behaviour to an AJAX viewer application. Custom functionality in a Fusion application can be provided in the following forms:

 * An :ref:`invoke-script-widget` widget
 * An :ref:`invoke-url-widget` widget

.. _invoke-script-widget:

Invoke Script
^^^^^^^^^^^^^

Invoke Script widgets function like their counterpart in the AJAX viewer. The main difference is that your script code has to work against the Fusion API instead of the AJAX viewer API, if you are going to use such features. You will need to use an authoring tool like MapGuide Maestro or hack the Application Defintion XML document itself to modify the script code for a Invoke Script widget

.. _invoke-url-widget:

Invoke URL
^^^^^^^^^^

Invoke URL widgets function like their counterpart in the AJAX viewer. Just like the Invoke Script widget, the main difference is that any javascript code that you emit from the URL you are invoking that uses the AJAX viewer API will need to be converted to use the Fusion API instead. As Invoke URL widgets just simply invoke a URL, you can use any language (.net/Java/PHP) for the server page that is invoked when working with the MapGuide API.

MapGuideViewerApi.js
^^^^^^^^^^^^^^^^^^^^

If you've worked with the AJAX viewer API, then the Fusion API will look completely different to you. The `MapGuideViewerApi.js` utility library is included in the Fusion framework which provides a subset of the AJAX viewer API to help ease the transition of your javascript code from the AJAX viewer API to Fusion.

The `MapGuideViewerApi.js` file is located under the `layers\MapGuide` subdirectory of your Fusion directory

Events
------

The event code in Fusion is designed to provide an asynchronous notification mechanism that can be used to register for, and receive notification of, key events that happen asynchronously in Fusion. The following terms are used with respect to events:

    * `event id`: a unique identifier for an event, represented by a javascript variable that is all in upper case (e.g `FUSION_INITIALIZED`)
    * `trigger`: when an event occurs, it is 'triggered' and all the registered callback functions are notified
    * `register`: provide a callback function that is called when an event is triggered
    * `deregister`: remove a callback function that was previously registered
    * `publish`: anything that can trigger an event must publish all the valid event ids 

The event mechanism of Fusion is implemented by two functions: registerForEvent and deregisterForEvent. Both functions have the same signature, taking an event ID as the first parameter and a callback function pointer as the second parameter.

The Fusion application object provides two specific events that can be used by applications to get notification of when Fusion initialization has completed and when an error occurs anywhere in Fusion. These events are:

`Fusion.Event.FUSION_INITIALIZED:`

    This is triggered when Fusion's initialization is complete and the application is running. This signals that it is safe for the application to communicate with specific Widgets. Note that the MapWidget, specifically, will be ready but may not have actually loaded the map. There is a separate event for that . Applications should register for this event before calling `Fusion.initialize()`.

`Fusion.Event.FUSION_ERROR:`

    This is triggered when an internal error happens. Details on the error are passed to the callback function. Applications should register for this event before calling `Fusion.initialize()` to ensure that they receive errors that happen during initialization.

Widgets in Fusion also use and trigger events. Widgets are designed to be completely independent of one another, allowing them to be added to, or removed from, applications with little or no impact on the other Widgets in the application. However, there are cases (especially with the MapWidget) where it is important that Widgets be able to communicate with other Widgets or with the application as a whole. For these situations, there is an event mechanism built into Fusion that allows Widgets, and applications built on Fusion, to register for and trigger events. The event mechanism allows Widgets to be independent of each other, but still provide a high level of integration when required.

To register a callback function for a Widget event, the application must first obtain a reference to the Widget through one of the methods of the `Fusion` object (`getWidgetById` typically) and then call `registerForEvent` passing one of the event IDs that is valid for that Widget. 

Working With Maps
-----------------

In Fusion, the MapWidget is central to everything that is going on. It is not valid to design a Fusion application without a Map in it. The MapWidget is the primary interface between the application and the spatial data represented by the map. Most widgets in Fusion either display information about the map or allow the user to interact with the map in some way.

The Map widget API is probably the most used one in Fusion. It is completely documented in the Command API reference, but the most used methods are described here.

`loadMap(mapDefinition):`

    This causes the Map widget to load the specified MapDefinition?.

`reloadMap():`

    This causes the Map widget to reload the current MapDefinition?. This is necessary when the map state has changed in certain ways (adding or removing layers in the map, for instance) and is primarily an internal function.

`setExtents(minx, miny, maxx, maxy).` This is used to set the map extents to a particular bounding box programmatically. drawMap():

    This is used to render a map image and load it in the browser. Normally, this is called automatically as required, but occasionally it may be required to be called programmatically when the state of the map has changed on the server without the knowledge of the Map widget.

`query(options):`

    This is described more completely in the Command API Reference, but the query method is used to query the Map in some way and create (or modify) a selection.

`getSessionId():`

    This returns the session id that is used with the server side support scripts for this map.

When a Map is defined in the ApplicationDefinition file, it can have a default MapDefinition that is automatically loaded when the application is loaded. But it is not mandatory to specify a default map. When no default map is specified, the Map widget is still initialized. Loading of a MapDefinition is then done in response to a widget (such as the MapMenu? widget) or some application-specific code. Regardless of how it happens, when a MapDefinition has been loaded, the Map widget will trigger a `Fusion.Event.MAP_LOADED` event. Most widgets are not useful if there is no map loaded, so they use the `Fusion.Event.MAP_LOADED` event to determine when they should be enabled. This means that most widgets will appear initially disabled until the map has been loaded. There are some notable exceptions, including the Map Menu widget which is used to provide a drop-down menu of MapDefinitions that the user can pick from.

Once the Map is loaded, many events may be triggered, including:

`Fusion.Event.MAP_SESSION_CREATED:`

    The Map widget is responsible for creating and maintaining a session with the server. When the session has been created, this event is triggered. Nothing can happen until this event has been triggered.

`Fusion.Event.MAP_LOADING:`

    The Map widget triggers this event when it is starting to load a new Map. This is primarily used by widgets to prepare themselves for the new map by discarding their current state and temporarily disabling themselves.

`Fusion.Event.MAP_LOADED:`

    The Map widget triggers this event when a map has been loaded and is ready.

`Fusion.Event.MAP_RELOADED:`

    The Map widget triggers this event when the current map has been reloaded and is ready.

`Fusion.Event.MAP_EXTENTS_CHANGED:`

    The Map widget triggers this event for any navigation that changes the current extents.

`Fusion.Event.MAP_BUSY_CHANGED:`

    The Map widget maintains a reference count of asynchronous events as they start and finish. An application can use this event to display a processing image so that the user is aware that some asynchronous activitity is happening.

`Fusion.Event.MAP_RESIZED:`

    The Map widget triggers this event when the size of the map is changed.

`Fusion.Event.MAP_SELECTION_ON:`

    The Map widget triggers this event when a new selection has been created.

`Fusion.Event.MAP_SELECTION_OFF:`

    The Map widget triggers this event when the current selection has been cleared.

`Fusion.Event.MAP_ACTIVE_LAYER_CHANGED:`

    The Map widget allows for a single layer to be marked as active by the application. This event is triggered when the active layer is changed.

`Fusion.Event.MAP_GENERIC_EVENT:`

    Most widgets rely directly on their Map widget for everything. In some cases, though, widgets need to be informed of changes in other widgets. In these cases, the Map widget can act as a broker for events through the `MAP_GENERIC_EVENT`. Widgets that employ the `Fusion.Event.MAP_GENERIC_EVENT` normally do so for a specific internal purpose, and the application should not normally register for this event.

Working with Selections

There are several widgets in Fusion that allow the user to interactively select features on the Map. Fusion takes care of updating the Map image with the current selection, if necessary, but does not display attributes of the selected features to the user. That is up to the application.

Regardless of how the features are selected, the Map widget provides the API for an application to retrieve and work with the user's selection. There are two events that can be used by an application to know when the user selection has changed:

`Fusion.Event.MAP_SELECTION_ON:`

    The Map widget triggers this event when a new selection has been created.

`Fusion.Event.MAP_SELECTION_OFF:`

    The Map widget triggers this event when the current selection has been cleared.

When the application receives a `Fusion.Event.MAP_SELECTION_ON` event from the Map widget, it can use the following functions to work with the selection:

`hasSelection():`

    This method returns a boolean value which indicates if there is currently a selection or not

`getSelection(callback):`

    This method retrieves the current selection. Retrieving the selection is potentially an asynchronous operation and so the callee provides a callback function that is called when the selection is ready. The callback function is passed a single argument, a Selection object, described below.

`clearSelection():`

    This method is used to clear the current selection. This removes the selection from the map and invalidates the current selection object.

An application will typically call `getSelection()` in response to the `MAP_SELECTION_ON` event. Typical code for this might look like: 

**JavaScript**

.. highlight:: javascript
.. code-block:: javascript

    window.onload=function() {
        Fusion.registerForEvent(Fusion.Event.FUSION_INITIALIZED, onInitialized);
        Fusion.initialize();
    }

    var theMap;
    function onInitialized() {
        theMap = Fusion.getWidgetById('Map');
        theMap.registerForEvent(Fusion.Event.MAP_SELECTION_ON, selectionOn);
        theMap.registerForEvent(Fusion.Event.MAP_SELECTION_OFF, selectionOff);
    }

    function selectionOn() {
        //a new selection has been made, request it
        theMap.getSelection(displaySelection);
    }

    function displaySelection(selection) {
        //display the selection to the user in some way ...
    }

    function selectionOff() {
        //clear the selection results
    }
    
A Selection object is used to access the selection results. It provides the following API:

`getNumLayers():`

    returns the number of layers that have features selected

`getNumElements():`

    returns the total number of features that are selected

`getLowerLeftCoord():`

    returns the lower, left coordinate of the bounding box of all selected features

`getUpperRightCoord():`

    returns the upper, right coordinate of the bounding box of all selected features

`getLayerByName(name):`

    gets the layer selection object for a layer from the name of the layer. This returns null if there is no layer with the requested name in the selection results

`getLayer(index):`

    gets the layer selection object for the requested layer where index is between 0 and one less that the value returned by `getNumLayers()`.

An application will typically loop over the layers in a selection and retrieve individual results using the Layer Selection object returned by `getLayer()` or `getLayerByName()`. Layer selection objects have the following API

`getName():`

    returns the name of the layer that the selected features are in

`getNumElements():`

    returns the number of features selected in this layer

`getNumProperties():`

    returns the number of data properties, or attributes, of the features in this layer

`getPropertyNames():`

    returns an array of the names of each of the properties

`getPropertyTypes():`

    returns an array of the types of the properties

`getElementValue(elementIndex, propertyIndex):`

    returns the actual value of a given property for a given element

The following code is an example of how to use the Selection and Layer Selection objects to create a tabular display of selected features. 

**JavaScript**

.. highlight:: javascript
.. code-block:: javascript

    function displaySelection(selection) {
        //display the selection to the user in some way ...
        //make sure something got selected ...
        if (selection && selection.getNumElements() > 0) {
           //obtain a reference to the HTML Element that the results
           //will be placed in
           var resultElm = $('selectionResultDiv');
           for (var i=0; i<selection.getNumLayers(); i++) {
              var selectionLayer = selection.getLayer(i);
              var propNames = selectionLayer.getPropertyNames();
              var span = document.createElement('span');
              span.className = 'selectionResultsTitle';
              span.innerHTML = 'Layer ' + selectionLayer.getName();
              resultElm.appendChild(span);
              var table = document.createElement('table');
              table.className = 'selectionResultsTable';
              resultElm.appendChild(table);
              //set up the table header to be the property names         
              var thead = document.createElement('thead');
              table.appendChild(thead);
              var tr = document.createElement('tr');
              thead.appendChild(tr);
              for (var j=0; j<propNames.length; j++) {
                 var td = document.createElement('td');
                 td.innerHTML = propNames[j];
                 tr.appendChild(td);
              }
              //output the selection values
              var tbody = document.createElement('tbody');
              table.appendChild(tbody);
              for (var j=0; j<selectionLayer.getNumElements(); j++) {
                 var tr = document.createElement(tr);
                 tbody.appendChild(tr);
                 for (var k=0; k<propNames.length; k++) {
                    var td = document.createElement('td');
                    td.innerHTML = selectionLayer.getElementValue(j, k);
                    tr.appendChild(td);
                 }
              }
               }
        } else {
           //could display a message of some sort saying nothing was selected?
        }
    }
.. index::
   single: Introduction

Introduction
============

What This Guide Covers
----------------------

This guide describes how to use the MapGuide Open Source Web API and Viewer
API.

It assumes you have read the MapGuide Getting Started guide and are familiar
with using Autodesk MapGuide or MapGuide Maestro. Most examples also assume that you have installed the 
sample data and sample applications supplied with MapGuide.

.. note::
    
    MapGuide Maestro is a free and open source authoring tool for MapGuide. You can download the latest release `here <trac.osgeo.org/mapguide/wiki/maestro>`_

This guide provides a high-level overview of the APIs. More detailed information
is provided in the on-line MapGuide Web API Reference and MapGuide Viewer API
Reference.

.. index::
    single: Essential Concepts

Essential Concepts
------------------

Refer to the MapGuide Getting Started guide for details about the MapGuide
architecture and components. It is important to understand the relationship
between a MapGuide Viewer, a MapGuide Web application, and the MapGuide
site. It is also important to understand resources and repositories.

Web applications reside on the Web Server. They are normally executed by
requests from a MapGuide Viewer. They can in turn communicate with the
MapGuide site and send data back to the Viewer.

When you define a web layout, using Autodesk MapGuide or some other
method, you also define toolbar and menu commands. These can be standard
pre-defined Viewer commands like pan, zoom, and refresh, or they can be
custom commands. Custom commands are a way of extending MapGuide to interact with your mapping data. 

The custom commands are HTML pages, generated on the server using PHP, ASP.NET, or Java (JSP). These languages
can use the Web API to retrieve, manipulate, and update mapping data.

Many custom commands run in the task area, a section of the Viewer that is
designed for user input/output. For more details about the task area and how
it integrates with the rest of the Viewer, see The MapGuide Viewer on page
15.

Preparing to Run the Examples
-----------------------------

MapGuide includes a set of sample applications. Some of them correspond
directly to chapters in this Developer's Guide. These samples are designed to 
show one or two key concepts at a time.

Other sample applications are more full-featured. These are designed to show
some of the capabilities of MapGuide. They are not discussed in detail in this
guide, but they do build upon the basic concepts.

The sample applications are available on the `MapGuide Open Source Download Page <http://mapguide.osgeo.org/downloads.html>`_. See the manual
Installing Sample Data and Viewer Sample Application also available on the download site for details.

Complete examples are available from `MapGuide Open Source Download Page <http://mapguide.osgeo.org/downloads.html>`_.
There are two required components: the source code and a package file for
creating the web layouts. The Sheboygan sample data must also be installed.

.. note::

   The Web API supports .NET, Java, and PHP. Each code sample illustrated in this developer's guide will show the same code in all 3 lanugages
   
   To run the examples on a Linux installation, change any Windows-specific
   file paths to corresponding Linux paths.

This guide includes many code snippets. In most cases, the snippets are
incomplete, lacking initialization and error-checking. For more complete
versions, refer to the sample applications.

.. index::
    single: Application Development

Application Development
-----------------------

.. todo::
    It seems Autodesk themselves were lost for words here. Talk about: Firebug, Visual Studio / Visual Web Developer, error and trace log monitoring
    
Hints for debugging, etc.

Making script frame visible to catch errors there

Use Firefox because of DOM Inspector, Error console, Venckman,

Reading Apache log for PHP errors. Is there an equivalent for IIS? Other sources
of log information?

.. index::
    single: Repositories
    single: Resources

Resources and Repositories
--------------------------

.. todo::
    Re-review for MGOS 2.3 as it introduces watermarks

A MapGuide repository is a database that stores and manages the data for the
site. The repository stores all data except data that is stored in external
databases. Data stored in a repository is a resource.

Types of data stored in the repository:

 * Feature Sources
 * Drawing Sources
 * Layer definitions
 * Map definitions
 * Web layouts
 * Flexible Web Layouts (aka. Application Definitions)
 * Symbol Libraries
 * Symbol Definitions
 * Load Procedures
 * Print Layouts

.. index::
    single: Repositories; Library Repositories
    single: Repositories; Session Repositories

Library and Session
^^^^^^^^^^^^^^^^^^^

Persistent data that is available to all users is stored in the Library repository.

In addition, each session has its own repository, which stores the run-time
map state. It can also be used to store other data, like temporary layers that
apply only to an individual session. For example, a temporary layer might be
used to overlay map symbols indicating places of interest.
Data in a session repository is destroyed when the session ends.

A resource identifier for a resource in the Library will always begin with
``Library://``. For example:

``Library://Samples/Layouts/SamplesPhp.WebLayout``

A resource identifier for a session resource will always begin with ``Session:``,
followed by the session id. For example:

``Session:70ea89fe-0000-1000-8000-005056c00008_en//layer.LayerDefinition``

.. note::
    
    Unless you have logged in with the Administrator login, security restrictions are generally imposed when reading/writing to the Library repository. 

.. tip::

    If you get "resource not found" errors on a session resource id when attempting to read/write resources to the session repository, chances 
    are the MapGuide Server has determined the session has been idle for too long and has already destroyed the repository. To ensure the session
    remains alive, you can tick the "Keep Connection Alive" Web Layout option in Autodesk MapGuide Studio. A similar option exists in MapGuide Maestro's
    Web Layout editor.

.. index::
    single: MgMap; About

Maps
^^^^

.. todo::
    Update examples to not use blank MgMap() ctor. This is deprecated and a common cause of errors and mistakes

A map (``MgMap`` object) is created from a map definition resource. The map definition contains 
basic information about the map, including things like:

 * the coordinate system used in the map
 * the initial map extents
 * references to the layer definitions for layers in the map
 
When the ``MgMap`` object is created, it is initialized with data from the map
definition. As a user interacts with the map, the ``MgMap`` may change, but the
map definition does not.

The map is saved in the session repository so it is available to all pages in the
same session. You cannot save a map in the library repository.
Map creation is handled by the Viewers. When a Viewer first loads, it creates
a map in the session repository. 

The map name is taken from the map definition name. For example, if a web layout references a map definition
named ``Sheboygan.MapDefinition``, then the Viewer will create a map named ``Sheboygan.Map``.

.. note::
    
    ``Sheboygan.Map`` represents the internal state of the runtime instance of ``Sheboygan.MapDefinition`` and is 
    read and written when you call ``Open()`` and ``Save()`` respectively on the ``MgMap`` object. Users are never allowed
    direct access to this resource.

If your application does not use a Viewer, you can create the map and store
it in the repository yourself. To do this, your page must

 * Create an ``MgMap`` object.
 * Initialize the ``MgMap`` object from a map definition.
 * Assign a name to the ``MgMap`` object.
 * Save the map in the session repository.
 
For example, the following section of code creates an ``MgMap`` named ``Sheboygan.Map``, based on ``Sheboygan.MapDefinition``.

**PHP**

.. highlight:: php
.. code-block:: php

    $mapDefId = new MgResourceIdentifier("Library://Samples/Sheboygan/Maps/Sheboygan.MapDefinition");
    $map = new MgMap();
    $mapName = $mapDefId->GetName();
    $map->Create($resourceService, $mapDefId, $mapName);
    $mapId = new MgResourceIdentifier("Session:$sessionId//$mapName." . MgResourceType::Map);
    $map->Save($resourceService, $mapId);

**.net (C#)**

.. highlight:: csharp
.. code-block:: csharp

    //Note: This code fragment assumes you have imported the OSGeo.MapGuide namespace

    MgResourceIdentifier mapDefId = new MgResourceIdentifier("Library://Samples/Sheboygan/Maps/Sheboygan.MapDefinition");
    MgMap map = new MgMap();
    String mapName = mapDefId.GetName();
    map.Create(resourceService, mapDefId, mapName);
    MgResourceIdentifier mapId = new MgResourceIdentifier("Session:$sessionId//$mapName." + MgResourceType.Map);
    map.Save(resourceService, mapId);
    
**Java**
    
.. highlight:: java
.. code-block:: java

    //Note: This code fragment assumes you have imported the org.osgeo.mapguide package

    MgResourceIdentifier mapDefId = new MgResourceIdentifier("Library://Samples/Sheboygan/Maps/Sheboygan.MapDefinition");
    MgMap map = new MgMap();
    String mapName = mapDefId.GetName();
    map.Create(resourceService, mapDefId, mapName);
    MgResourceIdentifier mapId = new MgResourceIdentifier("Session:$sessionId//$mapName." + MgResourceType.Map);
    map.Save(resourceService, mapId);

.. index::
    single: Web Layout; Displaying

Hello, Map - Displaying a Web Layout
------------------------------------

A web layout describes how the map looks when it is displayed in a web
browser. Using Studio, Maestro or some other method to edit the web layout resource,
you can create and customize the layout, changing how it looks in a browser
and what features are enabled.

The AJAX Viewer does not require installing any additional software. It runs
using most browsers, including Internet Explorer, Mozilla Firefox, Google Chrome and Safari.

The simplest way to display a web layout is to pass its resource identifier as a
GET parameter to the Viewer URL. For example, the following will display a
web layout using the AJAX Viewer running on localhost:

``http://localhost/mapguide/mapviewerajax/?WEBLAYOUT=Library%3a%2f%2fSamples%2fLayouts%2fSample.WebLayout``

**Authentication**

All MapGuide sites require authentication with user id and password. If
authentication succeeds, MapGuide creates a session, identified by a unique
session id. This keeps the state consistent between the viewer and the server
across multiple HTTP requests. Subsequent access to the site requires the
session id instead of the user id. By default, the Viewer handles authentication
itself, and it prompts for user id and password when it first loads. There are
situations, though, where it is better to authenticate before loading the Viewer
page.

One common example is a site offering read-only access to visitors. For this
situation, the default MapGuide installation includes a user ``Anonymous``
with an empty password.

To perform authentication before the Viewer loads, embed the Viewer in
another page using a ``<frame>`` or ``<iframe>`` element. The outer page can do
any necessary authentication, create a session, then pass the web layout and
session id to the Viewer frame.

The following example displays a web layout using the AJAX Viewer. It
performs some basic initialization and creates a session, then displays a Viewer
page using the session identifier and the web layout.

**PHP**

.. highlight:: php
.. code-block:: php

    <?php
     
    $installDir =
       'C:\\Program Files\\OSGeo\\MapGuide\\';
    $extensionsDir = $installDir . 'Web\\www\\';
    $viewerDir = $installDir . 'mapviewerphp\\';
     
    include $viewerDir . 'constants.php';
     
    MgInitializeWebTier($extensionsDir . 'webconfig.ini');
     
    $site = new MgSite();
    $site->Open(new MgUserInformation("Anonymous", ""));
     
    $sessionId = $site->CreateSession();
    $webLayout = "Library://Samples/Layouts/SamplesPhp.WebLayout";
     
    ?>
     
    <html>
    <head>
    <title>Simple Sample Application</title>
    </head>
    <body marginheight="0" marginwidth="0">
    <iframe id="viewerFrame" width="100%" height="100%" frameborder=0
    scrolling="no"
    src="/mapguide/mapviewerajax/?SESSION=<?= $sessionId ?>&
    WEBLAYOUT=<?= $webLayout ?>"></iframe>
    </body>
    </html>
    
**.net (C#)**

.. highlight:: csharp
.. code-block:: csharp

    Code sample coming soon!

**Java**

.. highlight:: java
.. code-block:: java

    Code sample coming soon!

.. index::
    single: Web Layout; Adding a Custom Command

Hello, Map 2 - Adding a Custom Command
--------------------------------------

Web layouts can include custom commands added to the toolbar, context
menu, task list, or task pane area of the Viewer. These custom commands
make up the MapGuide application.

This next sample MapGuide page displays some basic information about a
map. It does not do any complicated processing. Its purpose is to illustrate
the steps required to create a MapGuide page and have it connect to a Viewer
on one side and the MapGuide site on the other.

.. index::
    single: Web Layout; Server Pages

Web Layouts and MapGuide Server Pages
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

A *MapGuide Server Page* is any PHP, ASP.NET, or JSP page that makes use of the
MapGuide Web API. These pages are typically invoked by the MapGuide
Viewer or browser and when processed result in HTML pages that are loaded
into a MapGuide Viewer or browser frame. This is the form that will be used
for most examples in this guide. It is possible, however, to create pages that
do not return HTML or interact with the Viewer at all. These can be used for
creating web services as a back-end to another mapping client or for batch
processing of your data.

Creating a MapGuide page requires initial setup, to make the proper
connections between the Viewer, the page, and the MapGuide site. Refer 
to the *MapGuide Studio Help* for details.

One part of the initial setup is creating a web layout, which defines the
appearance and available functions for the Viewer. When you define a web
layout, you assign it a resource name that describes its location in the
repository. The full resource name looks something like this:

``Library://Samples/Layouts/SamplesPhp.WebLayout``

When you open the web layout using a browser with the AJAX Viewer the resource name 
is passed as part of the Viewer URL. Special characters in the resource name are URL-encoded, 
so the full URL would look something like this, (with line breaks removed):

``http://localhost/mapguide/mapviewerajax/?WEBLAYOUT=Library%3a%2f%2fSamples%2fSheboygan%2fLayouts%2fSheboyganPhp.WebLayout``

Part of the web layout defines commands and the toolbars and menus that
contain the commands. These commands can be built-in commands, or they
can be URLs to custom pages. The web layout also includes a URL to a home
task that displays in the task pane. The home task can open other pages.

To create a new page and make it available as a command from the task list,
do the following:

 * Edit the web layout using Studio or Maestro
 * Add a command to the web layout.
 * Set the command type to Invoke URL.
 * Set the URL of the command to the URL of your page
 * Add the command to the Task Bar Menu.

.. note::

    Custom pages are loaded by the Viewer page, so a relative URL for a custom
    page must start at the Viewer directory, then go up one level to reach the ``mapguide``
    directory. For example, a custom page located at ``www/mapguide/samplesphp/index.php`` 
    would use the following relative URL in the web layout
    
    ``../samplesphp/index.php``

It is also possible to add custom commands to the toolbar and the context
menu using the same technique.

For most of the examples in this guide, however, the pages will be links from
a home page loaded in the task pane frame.

.. note::

    Installing the package that comes with the Developer's Guide samples
    creates a web layout automatically. The home task page of this layout contains
    links to examples that correspond to chapters in the Developer's Guide.

MapGuide Page Flow
^^^^^^^^^^^^^^^^^^

Most MapGuide pages follow a similar processing flow. First, they create a
connection with the site server using an existing session id. Then they open
connections to any needed site services. The exact services required depend
on the page function. For example, a page that deals with map feature data
requires a feature service connection.

Once the site connection and any other service connections are open, the
page can use MapGuide Web API calls to retrieve and process data. Output
goes to the task pane or back to the Viewer. See :ref:`mapguide-ajax-viewer` for details about sending data to the Viewer.

.. note::

    MapGuide pages written in PHP require one additional step because PHP
    does not support enumerations compiled into extensions. To deal with this
    limitation, PHP Web Extension pages must include constants.php, which is in
    the mapviewerphp folder. This is not required for ASP.NET or JSP pages.

Example Code
^^^^^^^^^^^^

The following sample illustrates basic page structure. It is designed to be called
as a task from a Viewer. It connects to a MapGuide server and displays the
map name and spatial reference system for the map currently being displayed
in the Viewer.

.. note::
    This sample is very similar to the ``Hello Map`` sample in the Developer's Guide
    samples.

**PHP**

.. highlight:: php
.. code-block:: php

    <html>
        <head><title>Hello, map</title></head>
        <body>
            <p>
            <?php
     
            // Define some common locations
            $installDir ='C:\\Program Files\\OSGeo\\MapGuide\\';
            $extensionsDir = $installDir . 'WebServerExtensions\www\\';
            $viewerDir = $extensionsDir . 'mapviewerphp\\';

            // constants.php is required to set some enumerations
            // for PHP. The same step is not required for .NET
            // or Java applications.
            include $viewerDir . 'constants.php';
     
            try
            {
                // Get the session information passed from the viewer.
                $args = ($_SERVER['REQUEST_METHOD'] == "POST")
                ? $_POST : $_GET;
                $mgSessionId = $args['SESSION']
                $mgMapName = $args['MAPNAME']

                // Basic initialization needs to be done every time.
                MgInitializeWebTier("$extensionsDir\webconfig.ini");

                // Get the user information using the session id,
                // and set up a connection to the site server.
                $userInfo = new MgUserInformation($mgSessionId);
                $siteConnection = new MgSiteConnection();
                $siteConnection->Open($userInfo);

                // Get an instance of the required service(s).
                $resourceService = $siteConnection->
                CreateService(MgServiceType::ResourceService);

                // Display the spatial reference system used for the map.
                $map = new MgMap();
                $map->Open($resourceService, $mgMapName);
                $srs = $map->GetMapSRS();
                echo 'Map <strong>' . $map->GetName() .
                '</strong> uses this reference system: <br />' . $srs;
            }
            catch (MgException $e)
            {
                echo "ERROR: " . $e->GetMessage() . "<br />";
                echo $e->GetStackTrace() . "<br />";
            }
            ?>
            </p>
        </body>
    </html>

**.net (C#)**

.. highlight:: csharp
.. code-block:: csharp

    Code sample coming soon!

**Java**

.. highlight:: java
.. code-block:: java

    Code sample coming soon!

How This Page Works
^^^^^^^^^^^^^^^^^^^

This example page performs the following operations:

**1. Get session information.**

When you first go to the URL containing the web layout, the Viewer
initiates a new session. It prompts for a user id and password, and uses
these to validate with the site server. If the user id and password are valid,
the site server creates a session and sends the session id back to the viewer.
The Viewer passes the session information every time it sends a request
to a MapGuide page. The pages use this information to re-establish a
session.

**2. Perform basic initialization.**

The webconfig.ini file contains parameters required to connect to the site
server, including the IP address and port numbers to use for
communication. ``MgInitializeWebTier()`` reads the file and gets the
necessary values to find the site server and create a connection.

**3. Get user information.**

The site server saves the user credentials along with other session
information. These credentials must be supplied when the user first
connects to the site. At that time, the Viewer authenticates the user and
creates a new session using the credentials. Using the session ID, other
pages can get an encrypted copy of the user credentials that can be used
for authentication.

**4. Create a site connection.**

Any MapGuide pages require a connection to a site server, which manages
the repository and site services.

**5. Create a connection to a resource service.**

Access to resources is handled by a resource service. In this case, the page
needs a resource service in order to retrieve information about the map
resource. You may need to create connections to other services, depending on the
needs of your application.

**6. Retrieve map details.**

The map name is also passed by the viewer to the MapGuide page. Use
this name to open a particular map resource with the resource service.
Once the map is open you can get other information. This example
displays the spatial reference system used by the map, but you can also
get more complex information about the layers that make up the map.

.. note::

    The ``MgInitializeWebTier`` function always needs to be called before you create a site connection and
    create service connections. 
    
    However, this function does not need to be called if you are only using the 
    Geometry and Coordinate System components of the MapGuide API

.. index::
    single: Services

Understanding Services
----------------------

The MapGuide site performs many different functions. These can be all done
by a single server, or you may balance the load across multiple servers. Some
essential functions must execute on the site server, while other functions may
execute on support servers.

A service performs a particular set of related functions. For example, a resource
service manages data in the repository, a feature service provides access to
feature data sources, and a mapping service provides visualization and plotting
functions.

Before a page can use a service, it must open a site connection and create an
instance of the necessary service type. The following example creates a resource
service and a feature service:

**PHP**

.. highlight:: php
.. code-block:: php

    $userInfo = new MgUserInformation($mgSessionId);
    $siteConnection = new MgSiteConnection();
    $siteConnection->Open($userInfo);
    $resourceService = $siteConnection->CreateService(MgServiceType::ResourceService);
    $featureService = $siteConnection->CreateService(MgServiceType::FeatureService);
    
**.net (C#)**

.. highlight:: csharp
.. code-block:: csharp

    //Note: This code fragment assumes you have imported the OSGeo.MapGuide namespace

    MgUserInformation userInfo = new MgUserInformation(mgSessionId);
    MgSiteConnection siteConnection = new MgSiteConnection();
    siteConnection.Open(userInfo);
    MgResourceService resourceService = (MgResourceService)siteConnection.CreateService(MgServiceType.ResourceService);
    MgFeatureService featureService = (MgFeatureService)siteConnection.CreateService(MgServiceType.FeatureService);

**Java**

.. highlight:: java
.. code-block:: java

    //Note: This code fragment assumes you have imported the org.osgeo.mapguide package

    MgUserInformation userInfo = new MgUserInformation(mgSessionId);
    MgSiteConnection siteConnection = new MgSiteConnection();
    siteConnection.Open(userInfo);
    MgResourceService resourceService = (MgResourceService)siteConnection.CreateService(MgServiceType.ResourceService);
    MgFeatureService featureService = (MgFeatureService)siteConnection.CreateService(MgServiceType.FeatureService);
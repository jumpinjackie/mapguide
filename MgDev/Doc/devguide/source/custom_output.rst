.. index::
   single: Custom Output
   
Custom Output
=============

.. todo:
    Talk about KML output as well

Introduction
------------

.. tip::

    The Custom Output sample, in the Developer's Guide samples, demonstrates
    concepts from this chapter.

MapGuide includes services for saving map representations for use in external
programs.

To save a map as a bit-mapped image (PNG or GIF), use the Rendering Service.

To save a map as a Design Web Format (DWF), use the Mapping Service.

An ePlot DWF is designed for offline viewing or printing. It can contain multiple
sheets, where each sheet is a complete map image that can be viewed offline
using Autodesk Design Review. Each sheet in an ePlot DWF is a static
representation of a single map view state.

Characteristics of bit-mapped images:

 * Images can be in PNG or GIF formats.
 * An image displays a portion of the the map view state at a particular scale.
 * The image is static with a fixed resolution. Zooming in creates a pixelated image.
 * Images are cross-platform.
 * Images are suitable for use in HTML pages, word processor documents, or graphics editing programs.

Characteristics of an ePlot DWF:

 * A single ePlot can contain multiple sheets.
 * Each sheet shows a single image, showing a portion of the map view at a particular scale.
 * The ePlot area and scale are static, but geometric features are stored as vector graphics, so zooming is smooth.
 * Some interactive features of the MapGuide DWF Viewer are available, such as the ability to turn layers on and off.
 * The ePlot requires the Autodesk Design Review, either standalone or as a plug-in for Internet Explorer.
 * Images can be copied to the Windows clipboard and used in other applications.
 * Autodesk Design Review is a free program that is only available on Windows. Visit http://www.autodesk.com to download.

.. index::
   single: Custom Output; image
   single: Rendering Service
   single: MgRenderingService

.. _rendering-service:

Rendering Service
-----------------

The Rendering Service creates bit-mapped images of a map suitable for
displaying in a browser or saving to a file. The image is returned as an
``MgByteReader`` object, which can be sent to a browser or saved to a file.

For example, to create a PNG image of a map area, perform the following
operations. Note that the aspect ratio of the envelope should match the image
dimensions or the image will be distorted.

**PHP**

.. highlight:: php
.. code-block:: php

    $byteReader = $renderingService->RenderMap($map, $selection, $envelope, $imageWidth, $imageHeight, $color, 'PNG');
    header("Content-type: " . $byteReader->GetMimeType() );
    $buffer = '';
    while ($byteReader->Read($buffer, 50000) != 0)
    {
        echo $buffer;
    }

**.net (C#)**

.. highlight:: csharp
.. code-block:: csharp

    //This code fragment assumes you have imported the OSGeo.MapGuide namespace
    
    MgByteReader byteReader = renderingService.RenderMap(map, selection, envelope, imageWidth, imageHeight, color, "PNG");
    //See common/common.aspx for implementation of Utility.OutputReaderContent()
    Utility.OutputReaderContent(byteReader, Response);

**Java**
    
.. highlight:: java
.. code-block:: java

    //This code fragment assumes you have imported the org.osgeo.mapguide namespace
    
    //Code sample not yet available

.. index::
   single: Custom Output; dwf
   single: Mapping Service
   single: MgMappingService

Mapping Service
---------------

The Mapping Service creates ePlot DWFs.

An ePlot DWF is designed primarily for offline viewing or printing. It includes
an ``MgPlotSpecification`` that defines the page size and margins. It can also
include an optional ``MgLayout`` that defines additional components to include
in the plot, like a legend or a custom logo. The layout is based on a print
layout in the repository. For a description of the PrintLayout schema, see the
MapGuide Web API Reference.

To create an ePlot DWF with more than one sheet, use an
``MgMapPlotCollection``, where each item in the collection is an ``MgMapPlot`` that
describes a single sheet.

.. note::

    The map name becomes the sheet name in the multi-plot DWF. Because
    each sheet in the DWF must have a unique name, you must create a separate
    MgMap object for each sheet in the DWF.

The following example creates a multi-plot DWF with two sheets. The second
sheet displays the same map area as the first, but it adds the title and legend
information from the print layout.

**PHP**

.. highlight:: php
.. code-block:: php

    $dwfVersion = new MgDwfVersion("6.01", "1.2");
     
    $plotSpec = new MgPlotSpecification(8.5, 11, MgPageUnitsType::Inches);
    $plotSpec->SetMargins(0.5, 0.5, 0.5, 0.5);
     
    $plotCollection = new MgMapPlotCollection();
     
    $plot1 = new MgMapPlot($map, $plotSpec, $layout);
    $plotCollection->Add($plot1);
    // Create a second map for the second sheet in the DWF. This
    // second map uses the print layout
    // to display a page title and legend.
    $map2 = new MgMap($siteConnection);
    $map2->Create($map->GetMapDefinition(), 'Sheet 2');
    $layoutRes = new MgResourceIdentifier("Library://Samples/Sheboygan/Layouts/SheboyganMap.PrintLayout");
    $layout = new MgLayout($layoutRes, "City of Sheboygan", MgPageUnitsType::Inches);
    $plot2 = new MgMapPlot($map2, $map->GetViewCenter()->GetCoordinate(), $map->GetViewScale(), $plotSpec, $layout);
    $plotCollection->Add($plot2);
    $byteReader = $mappingService->GenerateMultiPlot($plotCollection, $dwfVersion);
    // Now output the resulting DWF.
    $outputBuffer = '';
    $buffer = '';
    while ($byteReader->Read($buffer, 50000) != 0)
    {
        $outputBuffer .= $buffer;
    }
    header('Content-Type: ' . $byteReader->GetMimeType());
    header('Content-Length: ' . strlen($outputBuffer));
    echo $outputBuffer;

**.net (C#)**

.. highlight:: csharp
.. code-block:: csharp

    //This code fragment assumes you have imported the OSGeo.MapGuide namespace
    
    MgDwfVersion dwfVersion = new MgDwfVersion("6.01", "1.2");

    MgPlotSpecification plotSpec = new MgPlotSpecification(8.5f, 11.0f, MgPageUnitsType.Inches, 0.5f, 0.5f, 0.5f, 0.5f);

    MgResourceIdentifier layoutRes = new MgResourceIdentifier("Library://Samples/Sheboygan/Layouts/SheboyganMap.PrintLayout");
    MgLayout layout =  new MgLayout(layoutRes, "City of Sheboygan", MgPageUnitsType.Inches);

    MgMapPlotCollection plotCollection = new MgMapPlotCollection();

    MgMapPlot plot1 = new MgMapPlot(map, plotSpec, layout);
    plot1.SetCenterAndScale(map.GetViewCenter().GetCoordinate(), map.GetViewScale() * 2);
    plotCollection.Add(plot1);

    // Create a second map for the second sheet in the DWF. This second sheet uses the print layout
    // to display a page title and legend.

    MgMap map2 = new MgMap(siteConnection);
    map2.Create(map.GetMapDefinition(), "Sheet 2");
    MgMapPlot plot2 = new MgMapPlot(map2, plotSpec, layout);
    plot2.SetCenterAndScale(map.GetViewCenter().GetCoordinate(), map.GetViewScale());
    plotCollection.Add(plot2);

    MgByteReader byteReader = mappingService.GenerateMultiPlot(plotCollection, dwfVersion);

    // Now output the resulting DWF.
    // See common/common.aspx for implementation of Utility.OutputReaderContent()
    Utility.OutputReaderContent(byteReader, Response);

**Java**
    
.. highlight:: java
.. code-block:: java

    //This code fragment assumes you have imported the org.osgeo.mapguide namespace
    
    //Code sample not yet available

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<!--
//  Copyright (C) 2004-2011 by Autodesk, Inc.
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of version 2.1 of the GNU Lesser
//  General Public License as published by the Free Software Foundation.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
-->
<!--
This sample has been updated to reflect simplified usage patterns enabled by convenience APIs
introduced in MapGuide OS 2.0 by allowing you to query and insert features directly from 
the MgLayer objects themselves and the ability to get a MgFeatureReader directly from the MgSelection
object
-->

<%@ Page Language="c#" %>

<%@ Import Namespace="System" %>
<%@ Import Namespace="System.Collections.Specialized" %>
<%@ Import Namespace="System.IO" %>
<%@ Import Namespace="OSGeo.MapGuide" %>

<script runat="server">
    String sessionId;
    String mapName;
</script>

<!-- #Include File="bufferfunctions.aspx" -->
<html>
<head>
    <title>Viewer Sample Application - Create Buffer</title>
    <meta content="text/html; charset=utf-8" http-equiv="Content-Type">
    <meta http-equiv="content-script-type" content="text/javascript" />
    <meta http-equiv="content-style-type" content="text/css" />
    <link href="../styles/globalStyles.css" rel="stylesheet" type="text/css">
    <link href="../styles/otherStyles.css" rel="stylesheet" type="text/css">

    <script language="javascript">
        function OnPageLoad() {
            parent.parent.Refresh();
        }
    </script>

</head>
<body class="AppFrame" onload="OnPageLoad()">
    <h1 class="AppHeading">Create buffer</h1>
    <%
        sessionId = Request.Params["SESSION"];
        mapName = Request.Params["MAPNAME"];
        String selectionXml = HttpUtility.UrlDecode(Request.Params["SELECTION"]);

        try
        {
            // Initialize the Web Extensions and connect to the Server using
            // the Web Extensions session identifier stored in PHP session state.

            MapGuideApi.MgInitializeWebTier(Constants.WebConfigPath);

            MgUserInformation userInfo = new MgUserInformation(sessionId);
            MgSiteConnection siteConnection = new MgSiteConnection();
            siteConnection.Open(userInfo);

            MgResourceService resourceService = (MgResourceService)siteConnection.CreateService(MgServiceType.ResourceService);
            MgFeatureService featureService = (MgFeatureService)siteConnection.CreateService(MgServiceType.FeatureService);
            MgFeatureQueryOptions queryOptions = new MgFeatureQueryOptions();

            MgMap map = new MgMap(siteConnection);
            map.Open(mapName);

            // Check for selection data passed via HTTP POST

            MgSelection selection = null;
            MgReadOnlyLayerCollection selectedLayers = null;
            if (!string.IsNullOrEmpty(selectionXml))
            {
                selection = new MgSelection(map, selectionXml);
                selectedLayers = selection.GetLayers();
            }

            if (selectedLayers != null)
            {
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

                for (int i = 0; i < selectedLayers.GetCount(); i++)
                {
                    // Only check selected features in the Parcels layer.

                    MgLayer layer = (MgLayer)selectedLayers.GetItem(i);

                    if (layer.GetName() == "Parcels")
                    {
                        // Get the selected features from the MgSelection object
                        MgFeatureReader featureReader = selection.GetSelectedFeatures(layer, layer.GetFeatureClassName(), false);

                        // Process each item in the MgFeatureReader. Get the
                        // geometries from all the selected features and
                        // merge them into a single geometry.

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

                    }
                }
            }
            else
                Response.Write("No selected layers");
            Response.Write("</p>");

        }
        catch (MgException e)
        {
            Response.Write("<p>" + e.GetExceptionMessage() + "</p>");
            Response.Write("<p>" + e.GetDetails() + "</p>");
        }
    %>
    <p>The buffer has been created.</p>
</body>
</html>

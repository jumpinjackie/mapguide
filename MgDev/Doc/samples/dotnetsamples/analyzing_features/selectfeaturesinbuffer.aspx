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
object.
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
    <title>Viewer Sample Application - Select Within Buffer</title>
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
    <h1 class="AppHeading">
        Select within buffer area</h1>
    <%

    sessionId = Request.Params["SESSION"];
    mapName = Request.Params["MAPNAME"];
    String selectionXml = HttpUtility.UrlDecode(Request.Params["SELECTION"]);

    try
    {
      // Initialize the Web Extensions and connect to the Server using
      // the Web Extensions session identifier stored in PHP session state.

      MapGuideApi.MgInitializeWebTier (Constants.WebConfigPath);

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
        int bufferRingSize = 500; // measured in metres

        // Set up some objects for coordinate conversion

        String mapWktSrs = map.GetMapSRS();
        MgAgfReaderWriter agfReaderWriter = new MgAgfReaderWriter();
        MgWktReaderWriter wktReaderWriter = new MgWktReaderWriter();
        MgCoordinateSystemFactory coordinateSystemFactory = new MgCoordinateSystemFactory();
        MgCoordinateSystem srs = coordinateSystemFactory.Create(mapWktSrs);
        MgMeasure srsMeasure = srs.GetMeasure();

        // Check for a buffer layer. If it exists, delete
        // the current features.
        // If it does not exist, create a feature source and
        // a layer to hold the buffer.

        BufferHelper helper = new BufferHelper(Server);
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

        // Check for a parcel marker layer. If it exists, delete
        // the current features.
        // If it does not exist, create a feature source and
        // a layer to hold the parcel markers.

        MgLayer parcelMarkerLayer = null;
        layerIndex = map.GetLayers().IndexOf("ParcelMarker");
        if (layerIndex < 0)
        {
            MgResourceIdentifier parcelFeatureResId = new MgResourceIdentifier("Session:" + sessionId + "//ParcelMarker.FeatureSource");
            helper.CreateParcelMarkerFeatureSource(featureService, mapWktSrs, parcelFeatureResId);
            parcelMarkerLayer = helper.CreateParcelMarkerLayer(resourceService, parcelFeatureResId, sessionId);
            map.GetLayers().Insert(0, parcelMarkerLayer);
        }
        else
        {
            parcelMarkerLayer = (MgLayer)map.GetLayers().GetItem(layerIndex);
            MgFeatureCommandCollection commands = new MgFeatureCommandCollection();
            commands.Add(new MgDeleteFeatures("ParcelMarkerClass", "ID like '%'"));
            
            parcelMarkerLayer.UpdateFeatures(commands);
        }

        // Check each layer in the selection.

        for (int i = 0; i < selectedLayers.GetCount(); i++)
        {
          // Only check selected features in the Parcels layer.

          MgLayer layer = (MgLayer)selectedLayers.GetItem(i);

          if (layer.GetName() == "Parcels")
          {

            Response.Write( "Marking all parcels inside the buffer that are of type 'MFG'");

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

            // Create a buffer from the merged geometries

            double bufferDist = srs.ConvertMetersToCoordinateSystemUnits(bufferRingSize);
            MgGeometry bufferGeometry = mergedGeometries.Buffer(bufferDist, srsMeasure);

            // Create a filter to select parcels within the buffer. Combine
            // a basic filter and a spatial filter to select all parcels
            // within the buffer that are of type "MFG".

            queryOptions = new MgFeatureQueryOptions();
            queryOptions.SetFilter("RTYPE = 'MFG'");
            queryOptions.SetSpatialFilter("SHPGEOM", bufferGeometry, MgFeatureSpatialOperations.Inside);

            featureReader = layer.SelectFeatures(queryOptions);
            
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

            // Create a feature in the buffer feature source to show the area covered by the buffer.

            MgPropertyCollection props = new MgPropertyCollection();
            props.Add(new MgGeometryProperty("BufferGeometry", agfReaderWriter.Write(bufferGeometry)));

            MgFeatureCommandCollection commands = new MgFeatureCommandCollection();
            commands.Add(new MgInsertFeatures("BufferClass", props));

            bufferLayer.UpdateFeatures(commands);

            // Ensure that the buffer layer is visible and in the legend.

            bufferLayer.SetVisible(true);
            bufferLayer.ForceRefresh();
            bufferLayer.SetDisplayInLegend(true);
            parcelMarkerLayer.SetVisible(true);
            parcelMarkerLayer.ForceRefresh();

            map.Save();

          }
        }
      }
      else
      {
          Response.Write( "No selected layers");
          Response.Write( "</p>");
      }

    }
    catch (MgException e)
    {
      Response.Write( "<p>" + e.GetExceptionMessage() + "</p>");
      Response.Write( "<p>" + e.GetDetails() + "</p>");
    }
    %>
    <p>Done.</p>
</body>
</html>

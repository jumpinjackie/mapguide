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
<%@ Page Language="c#" %>

<%@ Import Namespace="System" %>
<%@ Import Namespace="System.Collections.Specialized" %>
<%@ Import Namespace="System.IO" %>
<%@ Import Namespace="OSGeo.MapGuide" %>
<!-- #Include File="../modifying_maps_and_layers/layer_functions.aspx" -->
<script runat="server">
    String sessionId;
    String mapName;

    ///////////////////////////////////////////////////////////////////////////////////
    static MgPropertyCollection MakeLine(String name, String x0, String y0, String x1, String y1)
    {
        MgPropertyCollection propertyCollection = new MgPropertyCollection();
        MgStringProperty nameProperty = new MgStringProperty("NAME", name);
        propertyCollection.Add(nameProperty);

        MgWktReaderWriter wktReaderWriter = new MgWktReaderWriter();
        MgAgfReaderWriter agfReaderWriter = new MgAgfReaderWriter();

        MgGeometry geometry = wktReaderWriter.Read(String.Format("LINESTRING XY ({0} {1}, {2} {3})", x0, y0, x1, y1));
        MgByteReader geometryByteReader = agfReaderWriter.Write(geometry);
        MgGeometryProperty geometryProperty = new MgGeometryProperty("SHPGEOM", geometryByteReader);
        propertyCollection.Add(geometryProperty);

        return propertyCollection;
    }

    ///////////////////////////////////////////////////////////////////////////////////
    static bool DoesResourceExist(MgResourceIdentifier resourceIdentifier, MgResourceService resourceService)
    // Returns true if the resource already exists, or false otherwise
    {
        return resourceService.ResourceExists(resourceIdentifier);
    }

    ///////////////////////////////////////////////////////////////////////////////////
    static bool DoesLayerExist(String layerName, MgMap map)
    // Returns true if the layer already exists, or false otherwise
    {
        MgLayerCollection layerCollection = map.GetLayers();
        return (layerCollection.Contains(layerName) ? true : false);
    }

    ///////////////////////////////////////////////////////////////////////////////////
</script>
<html>
<head>
  <meta content="text/html; charset=utf-8" http-equiv="Content-Type">
  <title>Draw a Line</title>

  <meta http-equiv="content-style-type" content="text/css">
  <link href="../styles/globalStyles.css" rel="stylesheet"  type="text/css">
  <link href="../styles/otherStyles.css" rel="stylesheet" type="text/css">
  <meta http-equiv="content-script-type" content="text/javascript">
  <script language="javascript" type="text/javascript">
    function OnPageLoad()
    {
      parent.mapFrame.Refresh();
    }
  </script>
</head>

<body onLoad="OnPageLoad()">
    <%
  
  // Get the parameters passed in from the task pane
  String x0 = Request.Params["x0"];
  String y0 = Request.Params["y0"];
  String x1 = Request.Params["x1"];
  String y1 = Request.Params["y1"];
  sessionId = Request.Params["SESSION"];
  mapName = Request.Params["MAPNAME"];

  try
  {
    // --------------------------------------------------//
    // Basic initialization needs to be done every time.
    MapGuideApi.MgInitializeWebTier(Constants.WebConfigPath);

    // Get the user information using the session id,
    // and set up a connection to the site server.
    MgUserInformation userInfo = new MgUserInformation(sessionId);
    MgSiteConnection siteConnection = new MgSiteConnection();
    siteConnection.Open(userInfo);

    // Get an instance of the required service(s).
    MgResourceService resourceService = (MgResourceService)siteConnection.CreateService(MgServiceType.ResourceService);
    MgFeatureService featureService = (MgFeatureService)siteConnection.CreateService(MgServiceType.FeatureService);

    //---------------------------------------------------//
    // Open the map
    MgMap map = new MgMap(siteConnection);
    map.Open(mapName);

    String layerName = "Lines";
    String layerLegendLabel = "New Lines";
    String groupName = "Analysis";
    String groupLegendLabel = "Analysis";

    //---------------------------------------------------//
    // Does the temporary feature source already exist?
    // If not, create it
    String featureSourceName = "Session:" + sessionId + "//TemporaryLines.FeatureSource";
    MgResourceIdentifier resourceIdentifier = new MgResourceIdentifier(featureSourceName);

    bool featureSourceExists = DoesResourceExist(resourceIdentifier, resourceService);
    if (! featureSourceExists)
    {
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
    }

    // Add the line to the feature source
    MgBatchPropertyCollection batchPropertyCollection = new MgBatchPropertyCollection();
    MgPropertyCollection propertyCollection = MakeLine("Line A", x0, y0, x1, y1);
    batchPropertyCollection.Add(propertyCollection);

    // Add the batch property collection to the feature source
    MgInsertFeatures cmd = new MgInsertFeatures(layerName, batchPropertyCollection);
    MgFeatureCommandCollection featureCommandCollection = new MgFeatureCommandCollection();
    featureCommandCollection.Add(cmd);

    // Execute the "add" commands
    featureService.UpdateFeatures(resourceIdentifier, featureCommandCollection, false);

    //---------------------------------------------------//
    bool layerExists = DoesLayerExist(layerName, map);
    if (! layerExists )
    {
      // Create a new layer which uses that feature source

      // Create a line rule to stylize the lines
      String ruleLegendLabel = "Lines Rule";
      String filter = "";
      String color = "FF0000FF";
      LayerDefinitionFactory factory = new LayerDefinitionFactory(Server);
      String lineRule = factory.CreateLineRule(ruleLegendLabel, filter, color);

      // Create a line type style
      String lineTypeStyle = factory.CreateLineTypeStyle(lineRule);

      // Create a scale range
      String minScale = "0";
      String maxScale = "1000000000000";
      String lineScaleRange = factory.CreateScaleRange(minScale, maxScale, lineTypeStyle);

      // Create the layer definiton
      String featureName = "SHP_Schema:Lines";
      String geometry = "SHPGEOM";
      String layerDefinition = factory.CreateLayerDefinition(featureSourceName, featureName, geometry, lineScaleRange);

      //---------------------------------------------------//
      // Add the layer to the map
      XmlDocument doc = new XmlDocument();
      doc.LoadXml(layerDefinition);
      MgLayer newLayer = LayerFunctions.AddLayerDefinitionToMap(doc, layerName, layerLegendLabel, sessionId, resourceService, map);
      // Add the layer to a layer group
      LayerFunctions.AddLayerToGroup(newLayer,groupName, groupLegendLabel, map);
    }

    // --------------------------------------------------//
    // Turn on the visibility of this layer.
    // (If the layer does not already exist in the map, it will be visible by default when it is added.
    // But if the user has already run this script, he or she may have set the layer to be invisible.)
    MgLayerCollection layerCollection = map.GetLayers();
    if (layerCollection.Contains(layerName))
    {
      MgLayer linesLayer = (MgLayer)layerCollection.GetItem(layerName);
      linesLayer.SetVisible(true);
    }

    MgLayerGroupCollection groupCollection = map.GetLayerGroups();
    if (groupCollection.Contains(groupName))
    {
      MgLayerGroup analysisGroup =groupCollection.GetItem(groupName);
      analysisGroup.SetVisible(true);
    }

    //---------------------------------------------------//
    //  Save the map back to the session repository
    map.Save();

    //---------------------------------------------------//

  }
  catch (MgException e)
  {
    Response.Write( "<script language=\"javascript\" type=\"text/javascript\"> \n");
    Response.Write("    alert(\" " + e.GetExceptionMessage() + " \"); \n");
    Response.Write("</script> \n");
  }
    %>
  </body>
</html>
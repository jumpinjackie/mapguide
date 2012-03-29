<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<%--
  Copyright (C) 2004-2011 by Autodesk, Inc.
  This library is free software; you can redistribute it and/or
  modify it under the terms of version 2.1 of the GNU Lesser
  General Public License as published by the Free Software Foundation.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
--%>
<%@ Page language="c#" %>
<%@ Import Namespace="System" %>
<%@ Import Namespace="System.Xml" %>
<%@ Import Namespace="OSGeo.MapGuide" %>
<script runat="server">
String sessionId;
String mapName;
</script>
<!-- #Include File="layer_functions.aspx" -->
<html>

<head>
    <meta content="text/html; charset=utf-8" http-equiv="Content-Type">
    <meta http-equiv="content-script-type" content="text/javascript">
    <meta http-equiv="content-style-type" content="text/css">
    <link href="../styles/globalStyles.css" rel="stylesheet" type="text/css">
    <link href="../styles/otherStyles.css" rel="stylesheet" type="text/css">

    <script language="javascript" type="text/javascript">
        function OnPageLoad()
        {
            parent.mapFrame.Refresh();
            parent.mapFrame.ZoomToScale(40000);
        }
    </script>
</head>

<body onLoad="OnPageLoad()">

<%
try
{
    //---------------------------------------------------//
    // Initialize
    MapGuideApi.MgInitializeWebTier(Constants.WebConfigPath);
    sessionId = Request.Params["SESSION"];
    mapName = Request.Params["MAPNAME"];
    MgUserInformation userInfo = new MgUserInformation(sessionId);
    MgSiteConnection siteConnection = new MgSiteConnection();
    siteConnection.Open(userInfo);
    MgResourceService resourceService = (MgResourceService)siteConnection.CreateService(MgServiceType.ResourceService);

    // --------------------------------------------------//
    // Open the map
    MgMap map = new MgMap(siteConnection);
    map.Open(mapName);

	// ...
    //---------------------------------------------------//
    // Create a new layer

	LayerDefinitionFactory factory = new LayerDefinitionFactory(this.Server);

    // Create a line rule.
    String legendLabel = "";
    String filter = "";
    String color = "FF0000FF";
	String lineRule = factory.CreateLineRule(legendLabel, filter, color);

    // Create a line type style.
    String lineTypeStyle = factory.CreateLineTypeStyle(lineRule);

    // Create a scale range.
    String minScale = "0";
    String maxScale = "1000000000000";
    String lineScaleRange = factory.CreateScaleRange(minScale, maxScale, lineTypeStyle);

    // Create the layer definiton.
    String featureClass = "Library://Samples/Sheboygan/Data/HydrographicLines.FeatureSource";
    String featureName = "SHP_Schema:HydrographicLines";
    String geometry = "SHPGEOM";
	String layerDefinition = factory.CreateLayerDefinition(featureClass, featureName, geometry, lineScaleRange);

    //---------------------------------------------------//
    // ...

    XmlDocument doc = new XmlDocument();
    doc.LoadXml(layerDefinition);
    // Add the layer to the map
    MgLayer newLayer = LayerFunctions.AddLayerDefinitionToMap(doc, "Hydro", "Hydro", sessionId, resourceService, map);
    LayerFunctions.AddLayerToGroup(newLayer, "Analysis", "Analysis", map);

    // --------------------------------------------------//
    // Turn on the visibility of this layer.
    // (If the layer does not already exist in the map, it will be visible by default when it is added.
    // But if the user has already run this script, he or she may have set the layer to be invisible.)
    MgLayerCollection layerCollection = map.GetLayers();
    if (layerCollection.Contains("Hydro"))
    {
        MgLayerBase squareFootageLayer =layerCollection.GetItem("Hydro");
        squareFootageLayer.SetVisible(true);
    }

    //---------------------------------------------------//
    //  Save the map back to the session repository
    map.Save();

    //---------------------------------------------------//

}
catch (MgException e)
{
	Response.Write("<script language=\"javascript\" type=\"text/javascript\"> \n");
	Response.Write("    alert(\" " +e.GetExceptionMessage() + " \"); \n");
	Response.Write("</script> \n");
}
%>

</body>
</html>



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
<%@ Page language="c#" debug="true" %>
<%@ Import Namespace="System" %>
<%@ Import Namespace="System.IO" %>
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
            parent.mapFrame.ZoomToScale(9999);
        }
    </script>
</head>

<body onLoad="OnPageLoad()">

<%
try
{
    // --------------------------------------------------//
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

    // --------------------------------------------------//
    // ...

    // Add the layer to the map
    MgLayer newLayer = LayerFunctions.AddLayerDefinitionToMap(domDocument, "RecentlyBuilt", "Built after 1980", sessionId, resourceService, map);
    LayerFunctions.AddLayerToGroup(newLayer, "Analysis", "Analysis", map);

    // --------------------------------------------------//
    // Turn off the "Square Footage" themed layer (if it
    // exists) so it does not hide this layer.
    MgLayerCollection layerCollection = map.GetLayers();
    if (layerCollection.Contains("SquareFootage"))
    {
        MgLayerBase squareFootageLayer = layerCollection.GetItem("SquareFootage");
        squareFootageLayer.SetVisible(false);
    }

    // --------------------------------------------------//
    // Turn on the visibility of this layer.
    // (If the layer does not already exist in the map, it will be visible by default when it is added.
    // But if the user has already run this script, he or she may have set the layer to be invisible.)
    layerCollection = map.GetLayers();
    if (layerCollection.Contains("RecentlyBuilt"))
    {
        MgLayerBase recentlyBuiltLayer = layerCollection.GetItem("RecentlyBuilt");
        recentlyBuiltLayer.SetVisible(true);
    }

    // --------------------------------------------------//
    //  Save the map back to the session repository
    map.Save();

    // --------------------------------------------------//


}
catch (MgException e)
{
  Response.Write("<script language=\"javascript\" type=\"text/javascript\"> \n");
  String message = e.GetExceptionMessage().Replace("\n", " ");
  Response.Write("    alert(\" " + message + " \"); \n");
  Response.Write("</script> \n");
}
%>

</body>
</html>

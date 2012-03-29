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
<%@ Import Namespace="System.Collections.Specialized" %>
<%@ Import Namespace="System.IO" %>
<%@ Import Namespace="OSGeo.MapGuide" %>

<script runat="server">
String sessionId;
String mapName;
</script>
<!-- #Include File="../common/common.aspx" -->
<html>
  <head>
    <title>Layer Visibility</title>
    <meta content="text/html; charset=utf-8" http-equiv="Content-Type">
    <meta http-equiv="content-script-type" content="text/javascript">
    <meta http-equiv="content-style-type" content="text/css">
    <link href="../styles/globalStyles.css" rel="stylesheet"  type="text/css">
    <link href="../styles/otherStyles.css" rel="stylesheet" type="text/css">
  </head>
  <body class="AppFrame">
    <h1 class="AppHeading">Layer Visibility</h1>
    <%
    
    try
    {
      MapGuideApi.MgInitializeWebTier(Constants.WebConfigPath);

      sessionId = Request.Params["SESSION"];
      mapName = Request.Params["MAPNAME"];
      
      MgUserInformation userInfo = new MgUserInformation(sessionId);
      MgSiteConnection siteConnection = new MgSiteConnection();
      siteConnection.Open(userInfo);
  
      MgMap map = new MgMap(siteConnection);
      map.Open(mapName);
      
      MgLayerCollection layers = map.GetLayers(); // Get layer collection
      Response.Write("<p>Layers, in draw order:</p>");
      Response.Write("<table class=\"taskPane\" cellspacing=\"0\">");
      Response.Write("<tr><th class=\"rowHead\">Layer</th><th>GetVisible()</th><th>IsVisible()</th></tr>");
      int count = layers.GetCount(); 
      for (int i = 0; i < count; i++)
      {
        MgLayerBase layer = layers.GetItem(i);
        Response.Write("<tr><td class=\"rowHead\">" + layer.GetName() + "</td><td>" + (layer.GetVisible() ? "on" : "off") + "</td><td>" + (layer.IsVisible() ? "on" : "off") + "</td></tr>\n");
      }
      Response.Write("</table>");
    }
    catch (MgException e)
    {
      Response.Write("<p><strong>Error:</strong> ");
      Response.Write(e.GetDetails());
      Response.Write("</p>");
    }
    %>
    <p><code>GetVisible()</code> returns the state of the visibility flag for a given layer (the check box in the
      Layers pane). <code>IsVisible()</code> returns the actual visibility of a layer, based on its visibility flag, 
      the visibility flag of any groups that include the layer, and whether the layer has a defined style for the 
    current view scale.</p>
</body>
</html>
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
String mgMapName;
String newLabel;
</script>
<!-- #Include File="../common/common.aspx" -->
<html>
  <head>
    <title>Rename layer</title>
    <meta content="text/html; charset=utf-8" http-equiv="Content-Type">
    <meta http-equiv="content-script-type" content="text/javascript">
    <meta http-equiv="content-style-type" content="text/css">
    <link href="../styles/globalStyles.css" rel="stylesheet"  type="text/css">
    <link href="../styles/otherStyles.css" rel="stylesheet" type="text/css">

    <script language="javascript" type="text/javascript">
      function OnPageLoad()
      {
        parent.parent.mapFrame.Refresh();
        parent.parent.mapFrame.ZoomToScale(12000);
      }
    </script>
  </head>
  <body class="AppFrame" onLoad="OnPageLoad();">
    <h1 class="AppHeading">Rename Roads Layer</h1>
    <%
    sessionId = Request.Params["SESSION"];
    mgMapName = Request.Params["MAPNAME"];

    try
    {
      // Initialize the Web Extensions and connect to the Server using
      // the Web Extensions session identifier stored in PHP
      // session state.

      MapGuideApi.MgInitializeWebTier (Constants.WebConfigPath);

      MgUserInformation userInfo = new MgUserInformation(sessionId);
      MgSiteConnection siteConnection = new MgSiteConnection();
      siteConnection.Open(userInfo);

      MgMap map = new MgMap(siteConnection);
      map.Open(mgMapName);

      MgLayerCollection layers = map.GetLayers();

      MgLayerBase roadLayer = layers.GetItem("Roads");
      String roadLabel = roadLayer.GetLegendLabel();
      if (roadLabel == "Roads")
        newLabel = "Streets";
      else
        newLabel = "Roads";

      roadLayer.SetLegendLabel(newLabel);

      // You must save the updated map or the
      // changes will not be applied
      // Also be sure to refresh the map on page load.
      map.Save();

      Response.Write("<p>Layer label has been changed to " + newLabel + ".</p>");
      Response.Write("<p><a href=\"toggle_roads_label.aspx?SESSION=" + sessionId + "&MAPNAME=" + mgMapName + "\">Run again</a></p>");
    }
    catch (MgLayerNotFoundException e)
    {
      Response.Write("<p>Layer not found</p>");
    }
    catch (MgObjectNotFoundException e)
    {
      Response.Write("<p>Layer not found</p>");
    }
    catch (MgException e)
    {
      Response.Write(e.GetExceptionMessage());
      Response.Write(e.GetDetails());
    }
  %>

</body>
</html>
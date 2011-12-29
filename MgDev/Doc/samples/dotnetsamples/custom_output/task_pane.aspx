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
<%@ Page language="c#" %>
<%@ Import Namespace="System" %>
<script runat="server">
String sessionId;
String mapName;
String fullPath;
</script>
<html>
  <head>
    <meta content="text/html; charset=utf-8" http-equiv="Content-Type">
    <meta http-equiv="content-script-type" content="text/javascript">
    <meta http-equiv="content-style-type" content="text/css">
    <link href="../styles/globalStyles.css" rel="stylesheet" type="text/css">
    <link href="../styles/otherStyles.css" rel="stylesheet" type="text/css">
    <script language=javascript>
      function submitRequest(pageUrl)
      {
        xmlSel = parent.parent.mapFrame.GetSelectionXML();
        mapScale = parent.parent.mapFrame.GetScale();
        mapHeight = 300;
        mapWidth = 300;
        mapCenter = parent.parent.mapFrame.GetCenter();
        params = new Array("SESSION", parent.parent.mapFrame.GetSessionId(),
          "MAPNAME", parent.parent.mapFrame.GetMapName(),
          "SELECTION", encodeURIComponent(xmlSel),
          "SCALE", mapScale,
          "HEIGHT", mapHeight,
          "WIDTH", mapWidth,
          "CENTERX", mapCenter.X,
          "CENTERY", mapCenter.Y
          );
        parent.parent.formFrame.Submit(pageUrl, params, "_blank");
      }
      
     function OnPageLoad()
     {
         parent.parent.mapFrame.Refresh();
         parent.parent.mapFrame.ZoomToScale(9999);
     }
    </script>
  </head>
  <body onLoad="OnPageLoad()" class="AppFrame">
    <h1 class="AppHeading">Custom Output</h1>
    <%
	    sessionId = Request.Params["SESSION"];
	    mapName = Request.Params["MAPNAME"];
    %>
    <ul>
      <li>
      	(Select one parcel and then click the link below.)<BR>
        <a href="#" onClick="submitRequest('/mapguide/dotnetsamples/custom_output/property_report.aspx'); return false;">
        Property report</a>
        <br/>Create a printable page with an inset image of the selected property.
        <br/>
        <%
        fullPath = Server.MapPath("property_report.aspx");
        %>
        <a target="_blank" href="../common/viewsource.aspx?FILENAME=<%= fullPath %>" >View source</a>
      </li>

      <li>
        <a target="_blank" href="eplot.aspx?SESSION=<%= sessionId %>&MAPNAME=<%= mapName %>" >
        ePlot</a>
        <br/>Create and display an ePlot. Requires Autodesk DWF Viewer or Autodesk Design Review on the client system.
        <br/>
        <%
        fullPath = Server.MapPath("eplot.aspx");
        %>
        <a target="_blank" href="../common/viewsource.aspx?FILENAME=<%= fullPath %>" >View source</a>
      </li>

      <li>
        <a target="_blank" href="multiplot.aspx?SESSION=<%= sessionId %>&MAPNAME=<%= mapName %>" >
        Multi-plot</a>
        <br/>Create and display a multi-plot ePlot. Requires Autodesk DWF Viewer or Autodesk Design Review on the client system.
        <br/>
        <%
        fullPath = Server.MapPath("multiplot.aspx");
        %>
        <a target="_blank" href="../common/viewsource.aspx?FILENAME=<%= fullPath %>" >View source</a>
      </li>

    </ul>

    <p>View source for support files:</p>
    <ul>
      <%
      fullPath = Server.MapPath("createmapimage.aspx");
      %>
      <li><a target="_blank" href="../common/viewsource.aspx?FILENAME=<%= fullPath %>" >createmapimage.aspx</a></li>
      <%
      fullPath = Server.MapPath("task_pane.aspx");
      %>
      <li><a target="_blank" href="../common/viewsource.aspx?FILENAME=<%= fullPath %>" >task_pane.aspx</a></li>
    </ul>

  </body>
</html>

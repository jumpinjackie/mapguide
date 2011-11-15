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
<html>
  <head>
    <meta content="text/html; charset=utf-8" http-equiv="Content-Type">
    <meta http-equiv="content-style-type" content="text/css">
    <link href="../styles/globalStyles.css" rel="stylesheet" type="text/css">
    <link href="../styles/otherStyles.css" rel="stylesheet" type="text/css">
    <script language="javascript">
    function ZoomToDistrict()
    {
        parent.parent.ZoomToView(-87.726134, 43.770338, 8000, true);
    }
    function listSelection()
    {
      xmlSel = encodeURIComponent(parent.parent.mapFrame.GetSelectionXML());
      params = new Array("SESSION", parent.parent.mapFrame.GetSessionId(), 
        "MAPNAME", parent.parent.mapFrame.GetMapName(),
        "SELECTION", xmlSel);
      pageUrl = "../dotnetsamples/working_with_feature_data/listselection.aspx";
      parent.parent.formFrame.Submit(pageUrl, params, "taskPaneFrame");
    }
    </script>
    <meta http-equiv="content-script-type" content="text/javascript">
  </head>
  <body class="AppFrame">
    <h1 class="AppHeading">Working With Feature Data</h1>
    <p>
      <%
      sessionId = Request.Params["SESSION"];
      mapName = Request.Params["MAPNAME"];
      %>
      <ul>
        <li>
          <a href="#"  onclick="ZoomToDistrict();" >Zoom to District 1</a>
          <br />Position the map to display parcels in District 1
        </li>
        <li>
          <a href="queryfeatures.aspx?SESSION=<%= sessionId %>&MAPNAME=<%= mapName %>" >Query features</a>
          <br />List properties in District 1 owned by Schmitt
          <br/>
          <%
          String fullPath = Server.MapPath("queryfeatures.aspx");
          %>
          <a target="_blank" href="../common/viewsource.aspx?FILENAME=<%= fullPath %>" >View source</a>
        </li>
        <li>
          <a href="selectfeatures.aspx?SESSION=<%= sessionId %>&MAPNAME=<%= mapName %>" >Set active selection</a>
          <br />Select properties in District 1 owned by Schmitt
          <br/>
          <%
          fullPath = Server.MapPath("selectfeatures.aspx");
          %>
          <a target="_blank" href="../common/viewsource.aspx?FILENAME=<%= fullPath %>" >View source</a>
        </li>
        <li>
          <a href="#" onclick="listSelection();" >List selected features</a>
          <br />Lists currently selected features
          <br/>
          <%
          fullPath = Server.MapPath("listselection.aspx");
          %>
          <a target="_blank" href="../common/viewsource.aspx?FILENAME=<%= fullPath %>" >View source</a>
        </li>
      </ul>
    </p>
    <p>View the source for this task pane, showing JavaScript for repositioning map and initiating selection list.</p>
      <ul>
        <li>
          <%
          fullPath = Server.MapPath("task_pane.aspx");
          %>
          <a target="_blank" href="../common/viewsource.aspx?FILENAME=<%= fullPath %>" >task_pane.aspx</a>
        </li>
      </ul>
    </p>
  </body>
</html>

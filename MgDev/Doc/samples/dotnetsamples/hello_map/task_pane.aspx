<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<%--
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
--%>
<%@ Page language="c#" %>
<%@ Import Namespace="System" %>
<%@ Import Namespace="System.Collections.Specialized" %>
<%@ Import Namespace="System.IO" %>
<%@ Import Namespace="OSGeo.MapGuide" %>
<script runat="server">
String sessionId;
String mapName;
String fullPath;
</script>
<html>
  <head>
    <meta content="text/html; charset=utf-8" http-equiv="Content-Type">
    <title>Task Pane</title>
    <meta http-equiv="content-style-type" content="text/css">
    <link href="../styles/globalStyles.css" rel="stylesheet" type="text/css">
    <link href="../styles/otherStyles.css" rel="stylesheet" type="text/css">
    <meta http-equiv="content-script-type" content="text/javascript">
  </head>
  <body class="AppFrame">
    <h1 class="AppHeading">Hello Map</h1>
    <%
      sessionId = Request.Params["SESSION"];
      mapName = Request.Params["MAPNAME"];
    %>
    <p>This sample shows the most basic interaction with a map. It connects to a session
    	and queries information about the map.
    </p>
    <ul>
      <li>
        <a href="display_spatial_reference.aspx?SESSION=<%= sessionId %>&MAPNAME=<%= mapName %>">Display Spatial Reference</a>
        <br/>Connect to MapGuide session and display basic information.
        <br/>
        <%
        fullPath = Server.MapPath("display_spatial_reference.aspx");
        %>
        <a target="_blank" href="../common/viewsource.aspx?FILENAME=<%= fullPath %>" >View source</a>
      </li>
    </ul>
    <%
    String path1 = Server.MapPath("../main.aspx");
    String path2 = Server.MapPath("./task_pane.aspx");
    String path3 = Server.MapPath("../common/common.aspx");
    %>
    <p>View source for support files:</p>
    <ul>
      <li><a target="_blank" href="../common/viewsource.aspx?FILENAME=<%= path1 %>">main.aspx</a> (wrapper for Viewer frames)</li>
      <li><a target="_blank" href="../common/viewsource.aspx?FILENAME=<%= path2 %>">task_pane.aspx</a> (this task pane page)</li>
      <li><a target="_blank" href="../common/viewsource.aspx?FILENAME=<%= path3 %>">common.aspx</a> (directory paths)</li>
    </ul>
  </body>
</html>

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
<script runat="server">
String sessionId;
String mapName;
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
    <h1 class="AppHeading">Map Layers</h1>
    <%
      String sessionId = Request.Params["SESSION"];
      String mapName = Request.Params["MAPNAME"];
    %>
    <ul>
      <li>
      	<a href="layer_visibility.aspx?SESSION=<%= sessionId %>&MAPNAME=<%= mapName %>">Layer Visibility</a>
		    <br/>List layers and visibility status
		    <br/>
		    <%
		    String fullPath = Server.MapPath("layer_visibility.aspx");
		    %>
		    <a target="_blank" href="../common/viewsource.aspx?FILENAME=<%= fullPath %>" >View source</a>
      </li>
      <li>
      	<a href="toggle_roads_label.aspx?SESSION=<%= sessionId %>&MAPNAME=<%= mapName %>">Rename Roads Layer</a>
		    <br/>Toggle the name of the Roads layer
		    <br/>
		    <?php
		    fullPath = Server.MapPath("toggle_roads_label.aspx");
		    %>
		    <a target="_blank" href="../common/viewsource.aspx?FILENAME=<%= fullPath %>" >View source</a>
      </li>
    </ul>

  </body>
</html>
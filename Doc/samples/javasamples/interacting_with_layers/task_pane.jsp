<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<!--
//  Copyright (C) 2004-2006  Autodesk, Inc.
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
<%@ page import="java.net.*" %>
<%@ page import="java.io.*" %>
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
      String sessionId = request.getParameter("SESSION");
      String mapName = request.getParameter("MAPNAME");
    %>
    <ul>
      <li>
      	<a href="layer_visibility.jsp?SESSION=<%= sessionId %>&MAPNAME=<%= mapName %>">Layer Visibility</a>
		    <br/>List layers and visibility status
		    <br/>
        <%
        ServletContext context = session.getServletContext();
        File currentPath = new File(context.getRealPath(request.getServletPath()));
        String dir = currentPath.getParent();
        String fullPath = java.net.URLEncoder.encode(dir);
        %>
        
        <!-- Not implemented yet
		    <a target="_blank" href="../common/viewsource.php?FILENAME=<%= fullPath %>/layer_visibility.jsp" >view source</a>
		-->
      </li>
      <li>
      	<a href="toggle_roads_label.jsp?SESSION=<%= sessionId %>&MAPNAME=<%= mapName %>">Rename Roads Layer</a>
		    <br/>Toggle the name of the Roads layer
		    <br/>
		    <!-- Not implemented yet
		    <a target="_blank" href="../common/viewsource.php?FILENAME=<%= fullPath %>/toggle_roads_label.jsp" >View source</a>
		    -->
      </li>
    </ul>

  </body>
</html>

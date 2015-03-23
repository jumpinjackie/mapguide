<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<!--
//  Copyright (C) 2004-2007  Autodesk, Inc.
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
    <h1 class="AppHeading">Hello Map</h1>
    <%
      String sessionId = request.getParameter("SESSION");
      String mapName = request.getParameter("MAPNAME");
    %>
    <p>This sample shows the most basic interaction with a map. It connects to a session
    	and queries information about the map.
    </p>
    <ul>
      <li>
        <a href="display_spatial_reference.jsp?SESSION=<%= sessionId %>&MAPNAME=<%= mapName %>">Display Spatial Reference</a>
        <br/>Connect to MapGuide session and display basic information.
        <br/>
        <%
        String fileName = "display_spatial_reference.jsp";
        ServletContext context = session.getServletContext();
        File currentPath = new File(context.getRealPath(request.getServletPath()));
        String dir = currentPath.getParent();
        String fullPath = java.net.URLEncoder.encode(dir + "/" + fileName);
        %>
        <!-- Not implemented yet
        <a target="_blank" href="../common/viewsource.php?FILENAME=<%= fullPath %>" >View source</a>
        -->
      </li>
    </ul>

	<!-- Not implemented yet
    <p>View source for support files:</p>
    <ul>
      <li><a target="_blank" href="../common/viewsource.php?FILENAME=<%= dir %>/../main.jsp">main.jsp</a> (wrapper for Viewer frames)</li>
      <li><a target="_blank" href="../common/viewsource.php?FILENAME=<%= dir %>/task_pane.jsp">task_pane.jsp</a> (this task pane page)</li>
      <li><a target="_blank" href="../common/viewsource.php?FILENAME=<%= dir %>/../common/common.jsp">common.jsp</a> (directory paths)</li>
    </ul>
    -->
  </body>
</html>

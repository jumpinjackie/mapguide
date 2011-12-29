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
<%@ Page Language="c#" %>

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
		<meta http-equiv="content-script-type" content="text/javascript">
		<meta http-equiv="content-style-type" content="text/css">
		<link href="../styles/globalStyles.css" rel="stylesheet" type="text/css">
		<link href="../styles/otherStyles.css" rel="stylesheet" type="text/css">
    <script src="../common/pageLoadFunctions.js" type="text/javascript" language="JavaScript">
    </script>
	</head>
	<body class="AppFrame">		
		<%
		sessionId = Request.Params["SESSION"];
        mapName = Request.Params["MAPNAME"];
		%>				
		<h1 class="AppHeading">Digitizing &amp; Redlining</h1>	
		<ul>
			<li>
				<a href="task_pane_digitizing.aspx?SESSION=<%= sessionId %>&MAPNAME=<%= mapName %>">Digitizing Sample</a>
        <br/>Show digitizing functions.
        <br/>
        <%
        fullPath = Server.MapPath("task_pane_digitizing.aspx");
        %>
        <a target="_blank" href="../common/viewsource.aspx?FILENAME=<%= fullPath %>" >View source</a>
			</li>
			<li>
				<a href="task_pane_redlining.aspx?SESSION=<%= sessionId %>&MAPNAME=<%= mapName %>">Redlining Sample</a>
        <br/>Draw lines on the map.
        <br/>
        <%
        fullPath = Server.MapPath("task_pane_redlining.aspx");
        %>
        <a target="_blank" href="../common/viewsource.aspx?FILENAME=<%= fullPath %>" >View source</a>
			</li>
		</ul>
    <p>View source for support files:</p>
    <%
        fullPath = Server.MapPath("draw_line.aspx");
        String fullPath2 = Server.MapPath("clear_lines.aspx");    
    %>
    <ul>
      <li><a target="_blank" href="../common/viewsource.aspx?FILENAME=<%= fullPath %>">draw_line.aspx</a></li>
      <li><a target="_blank" href="../common/viewsource.aspx?FILENAME=<%= fullPath2 %>">clear_lines.aspx</a></li>
    </ul>
	</body>
</html>

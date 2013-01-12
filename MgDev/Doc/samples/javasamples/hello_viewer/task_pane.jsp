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
    <h1 class="AppHeading">Hello Viewer</h1>
    <%
      String sessionId = request.getParameter("SESSION");
      String mapName = request.getParameter("MAPNAME");
    %>
    <p>This sample shows different ways of calling MapGuide Viewer API functions.</p>
    <p><strong>1.</strong> The web layout contains a custom Invoke Script command 
    	that calls the <code>ZoomToView()</code> function. Click the <img src="../../stdicons/icon_invokescript.gif" width="16" height="16" /> 
    	button in the tool bar, task list, or context menu
    	to reposition the map view. The Invoke Script command
    	is defined in the web layout for the sample. It contains the following:
    </p>
    <pre><code>ZoomToView(-87.732492, 
43.743934, 5000, true);</code></pre>

    <p><strong>2.</strong> Click <a href="gotopoint.jsp?X=-87.724520&Y=43.768387&Scale=5000" target="scriptFrame">this link</a>
    to display a different location. This loads a page into the hidden script frame.</p>
    <%
    ServletContext context = session.getServletContext();
    File currentPath = new File(context.getRealPath(request.getServletPath()));
    String dir = currentPath.getParent();
    String fullPath = java.net.URLEncoder.encode(dir);
    %>
    <!-- Not implemented yet
    <p>View source for support file:</p>
    <ul>
      <li><a target="_blank" href="../common/viewsource.php?FILENAME=<%= fullPath %>/gotopoint.jsp">gotopoint.jsp</a></li>
    </ul>
    -->

    <p><strong>3.</strong> Click <a href="#" onclick="parent.parent.ZoomToView(-87.707416, 43.747860, 5000, true);">this link</a>
    to display a third location. This uses <code>onclick</code> to call JavaScript directly from the task pane.</p>
    	
    <!-- Not implemented yet
    <p>View source for this file:</p>
    <ul>
      <li><a target="_blank" href="../common/viewsource.php?FILENAME=<%= fullPath %>/task_pane.jsp">task_pane.jsp</a></li>
    </ul>
    -->
    
    <p><strong>4.</strong> The home task pane loads JavaScript from an external file. The script runs when the map first loads. It disables 
    	the task pane and waits until the map has completed loading before enabling the pane. It does this by hooking into 
    	the <code>OnMapLoaded()</code> function of the main frame.</p>
    		
    <!-- Not implemented yet
    <p>View files:</p>
    <ul>
      <li>
        <a target="_blank" href="../common/viewsource.php?FILENAME=<%= fullPath %>/../index.jsp" >index.jsp</a>
      </li>
      <li>
        <a target="_blank" href="../common/viewsource.php?FILENAME=<%= fullPath %>/../common/pageLoadFunctions.js" >pageLoadFunctions.js</a>
      </li>
    </ul>
    -->

  </body>
</html>

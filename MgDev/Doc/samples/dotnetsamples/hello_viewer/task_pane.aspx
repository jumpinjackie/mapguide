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
    <title>Task Pane</title>
    <meta http-equiv="content-style-type" content="text/css">
    <link href="../styles/globalStyles.css" rel="stylesheet" type="text/css">
    <link href="../styles/otherStyles.css" rel="stylesheet" type="text/css">
    <meta http-equiv="content-script-type" content="text/javascript">
  </head>
  <body class="AppFrame">
    <h1 class="AppHeading">Hello Viewer</h1>
    <%
      sessionId = Request.Params["SESSION"];
      mapName = Request.Params["MAPNAME"];
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

    <p><strong>2.</strong> Click <a href="gotopoint.aspx?X=-87.724520&Y=43.768387&Scale=5000" target="scriptFrame">this link</a>
    to display a different location. This loads a page into the hidden script frame.</p>
    <%
    String fullPath = Server.MapPath(".");
    %>
    <p>View source for support file:</p>
    <ul>
      <li><a target="_blank" href="../common/viewsource.aspx?FILENAME=<%= fullPath %>/gotopoint.aspx">gotopoint.aspx</a></li>
    </ul>

    <p><strong>3.</strong> Click <a href="#" onclick="parent.parent.ZoomToView(-87.707416, 43.747860, 5000, true);">this link</a>
    to display a third location. This uses <code>onclick</code> to call JavaScript directly from the task pane.</p>
    <p>View source for this file:</p>
    <ul>
      <li><a target="_blank" href="../common/viewsource.aspx?FILENAME=<%= fullPath %>/task_pane.aspx">task_pane.aspx</a></li>
    </ul>
    
    <p><strong>4.</strong> The home task pane loads JavaScript from an external file. The script runs when the map first loads. It disables 
    	the task pane and waits until the map has completed loading before enabling the pane. It does this by hooking into 
    	the <code>OnMapLoaded()</code> function of the main frame.</p>
    <p>View files:</p>
    <ul>
      <li>
        <a target="_blank" href="../common/viewsource.aspx?FILENAME=<%= fullPath %>/../index.aspx" >index.aspx</a>
      </li>
      <li>
        <a target="_blank" href="../common/viewsource.aspx?FILENAME=<%= fullPath %>/../common/pageLoadFunctions.js" >pageLoadFunctions.js</a>
      </li>
    </ul>

  </body>
</html>

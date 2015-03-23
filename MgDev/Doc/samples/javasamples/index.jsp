<%--
  -Copyright (C) 2004-2007  Autodesk, Inc.
  -This library is free software; you can redistribute it and/or
  -modify it under the terms of version 2.1 of the GNU Lesser
  -General Public License as published by the Free Software Foundation.

  -This library is distributed in the hope that it will be useful,
  -but WITHOUT ANY WARRANTY; without even the implied warranty of
  -MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  -Lesser General Public License for more details.

  -You should have received a copy of the GNU Lesser General Public
  -License along with this library; if not, write to the Free Software
  -Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
--%>

<%@ page import="org.osgeo.mapguide.*" %>
<%@ page import="java.util.*" %>
<%@ page import="java.io.*" %>
<%@ page import="java.text.*" %>
<%@ page import="javax.servlet.jsp.*" %>
<%@ page import="javax.servlet.http.*" %>
<%@ include file="common/common.jsp" %>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html>
  <head>
    <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
    <title>MapGuide Developer Guide Samples</title>
    <meta http-equiv="content-style-type" content="text/css">
    <link href="styles/globalStyles.css" rel="stylesheet"  type="text/css">
    <link href="styles/otherStyles.css" rel="stylesheet" type="text/css">
    <script src="common/pageLoadFunctions.js" type="text/javascript" language="JavaScript">
    </script>
  </head>
  <body class="AppFrame">
    <%
      String sessionId = request.getParameter("SESSION");
      String mapName = request.getParameter("MAPNAME");
      if (mapName == null) mapName = "unknown";
    %>
    
    <p>At any time, click <img src="../stdicons/icon_home.gif" width="16" height="16" /> 
    in the task bar to return to this list of samples.</p>
    <h1>
      Basic Samples
    </h1>
      <ul>
        <li><a href="hello_map/task_pane.jsp?SESSION=<%= sessionId %>&MAPNAME=<%= mapName %>">Hello Map</a></li>
        <li><a href="hello_viewer/task_pane.jsp?SESSION=<%= sessionId %>&MAPNAME=<%= mapName %>">Hello Viewer</a></li>
        <li><a href="interacting_with_layers/task_pane.jsp?SESSION=<%= sessionId %>&MAPNAME=<%= mapName %>">Interacting With Layers</a></li>
        
        <!-- Not implemented yet
        <li><a href="working_with_feature_data/task_pane.jsp?SESSION=<%= sessionId %>&MAPNAME=<%= mapName %>">Working With Feature Data</a></li>
        <li><a href="modifying_maps_and_layers/task_pane.jsp?SESSION=<%= sessionId %>&MAPNAME=<%= mapName %>">Modifying Maps and Layers</a></li>
        <li><a href="analyzing_features/task_pane.jsp?SESSION=<%= sessionId %>&MAPNAME=<%= mapName %>">Analyzing Features</a></li>
        <li><a href="digitizing_features/task_pane.jsp?SESSION=<%= sessionId %>&MAPNAME=<%= mapName %>">Digitizing and Redlining</a></li>
        <li><a href="custom_output/task_pane.jsp?SESSION=<%= sessionId %>&MAPNAME=<%= mapName %>">Custom Output</a></li>
        -->
      </ul>
    <h1>
      Viewer Samples
    </h1>
      <ul>
        <li><a href="../javaviewersample/dwfviewersample.jsp?SESSION=<?= $sessionId ?>&MAPNAME=<?= $mapName ?>" target="_blank">Using DWF Viewer</a></li>
        <li><a href="../javaviewersample/ajaxviewersample.jsp?SESSION=<?= $sessionId ?>&MAPNAME=<?= $mapName ?>" target="_blank">Using Ajax Viewer</a></li>
        <li><a href="../javaviewersample/ajaxtiledviewersample.jsp?SESSION=<?= $sessionId ?>&MAPNAME=<?= $mapName ?>" target="_blank">Using Tiled Ajax Viewer</a></li>
      </ul>
    <%
    String helpDir = webExtensionsDirectory + "www/Help/";
    String helpUrl = "/mapguide/Help/";
    String devGuide = helpDir + "DevGuide.pdf";
    String apiRef = helpDir + "index.htm";
    String viewerApi = helpDir + "viewerapi.html";
    File f_devGuide = new File(devGuide);
    File f_apiRef = new File(apiRef);
    File f_viewerApi = new File(viewerApi);
    if (f_devGuide.exists() || f_apiRef.exists() || f_viewerApi.exists())
    {
    	out.println("<hr><h1>Documentation</h1>");
    	out.println("<p>Opens in a new window</p><ul>");
    	if (f_devGuide.exists())
    	{
    		out.println("<li><a href=\"" + helpUrl + "DevGuide.pdf\" target=\"_blank\">Developer's Guide (PDF)</a></li>"); 
    	}
    	if (f_apiRef.exists())
    	{
    		out.println("<li><a href=\"" + helpUrl + "index.htm\" target=\"_blank\">Web Server Extensions API Reference</a></li>"); 
    	}
    	if (f_viewerApi.exists())
    	{
    		out.println("<li><a href=\"" + helpUrl + "viewerapi.html\" target=\"_blank\">Viewer API Reference</a></li>"); 
    	}
    	out.println("</ul>");
    }
    %>
  </body>
</html>

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
<!-- #Include File="common/common.aspx" -->
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
    sessionId = Request.Params["SESSION"];
    if (Request.Params["MAPNAME"] == null)
        mapName = "unknown";
    else
        mapName = Request.Params["MAPNAME"];
    %>
    
    <p>At any time, click <img src="../stdicons/icon_home.gif" width="16" height="16" /> 
    in the task bar to return to this list of samples.</p>
    <h1>
      Basic Samples
    </h1>
      <ul>
        <li><a href="hello_map/task_pane.aspx?SESSION=<%= sessionId %>&MAPNAME=<%= mapName %>">Hello Map</a></li>
        <li><a href="hello_viewer/task_pane.aspx?SESSION=<%= sessionId %>&MAPNAME=<%= mapName %>">Hello Viewer</a></li>
        <li><a href="interacting_with_layers/task_pane.aspx?SESSION=<%= sessionId %>&MAPNAME=<%= mapName %>">Interacting With Layers</a></li>
        <li><a href="working_with_feature_data/task_pane.aspx?SESSION=<%= sessionId %>&MAPNAME=<%= mapName %>">Working With Feature Data</a></li>
        <li><a href="modifying_maps_and_layers/task_pane.aspx?SESSION=<%= sessionId %>&MAPNAME=<%= mapName %>">Modifying Maps and Layers</a></li>
        <li><a href="analyzing_features/task_pane.aspx?SESSION=<%= sessionId %>&MAPNAME=<%= mapName %>">Analyzing Features</a></li>
        <li><a href="digitizing_features/task_pane.aspx?SESSION=<%= sessionId %>&MAPNAME=<%= mapName %>">Digitizing and Redlining</a></li>
        <li><a href="custom_output/task_pane.aspx?SESSION=<%= sessionId %>&MAPNAME=<%= mapName %>">Custom Output</a></li>
      </ul>
      <hr />
    <!-- Not implemented yet
    <h1>
      Larger Tasks
    </h1>
      <ul>
        <li><a href="../dotnetviewersample/dwfviewersample.aspx?SESSION=<%= sessionId %>&MAPNAME=<%= mapName %>" target="_blank">Using DWF Viewer</a></li>
        <li><a href="../dotnetviewersample/ajaxviewersample.aspx?SESSION=<%= sessionId %>&MAPNAME=<%= mapName %>" target="_blank">Using Ajax Viewer</a></li>
        <li><a href="../dotnetviewersample/ajaxtiledviewersample.aspx?SESSION=<%= sessionId %>&MAPNAME=<%= mapName %>" target="blank">Using Tiled Ajax Viewer</a></li>
      </ul>
    -->
    <%
    //
    // To make the developer documentation available as part of the sample applications, copy or move the contents
    // of WebServerExtensions\Help to WebServerExtensions\www\Help.
    // 
    // If the docs are located in that directory, this scripts links to them from the task pane.
    //
    String helpDir = Path.Combine(Constants.WebExtensionsDirectory, "www\\Help");
    String devGuide = Server.MapPath("../Help/DevGuide.pdf");
    String apiRef = Server.MapPath("../Help/index.htm");
    String viewerApi = Server.MapPath("../Help/viewerapi.html");
    if (File.Exists(devGuide) || File.Exists(apiRef) || File.Exists(viewerApi))
    {
    	Response.Write("<hr><h1>Documentation</h1>");
    	Response.Write("<p>Opens in a new window</p><ul>");
    	if (File.Exists(devGuide))
    	{
    		Response.Write("<li><a href=\"../Help/DevGuide.pdf\" target=\"_blank\">Developer\'s Guide (PDF)</a></li>"); 
    	}
    	if (File.Exists(apiRef))
    	{
    		Response.Write("<li><a href=\"../Help/index.htm\" target=\"_blank\">Web Server Extensions API Reference</a></li>"); 
    	}
    	if (File.Exists(viewerApi))
    	{
    		Response.Write("<li><a href=\"../Help/viewerapi.html\" target=\"_blank\">Viewer API Reference</a></li>");
    	}
    	Response.Write("</ul>");
    }
    %>
  </body>
</html>

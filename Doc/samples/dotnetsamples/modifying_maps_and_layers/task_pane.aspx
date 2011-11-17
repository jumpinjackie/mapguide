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
  </head>
  <body class="AppFrame">
    <h1 class="AppHeading">Modifying Maps &amp; Layers</h1>
      <p>The following links all load in the hidden script frame, leaving this task pane visible.</p>
      <%
        sessionId = Request.Params["SESSION"];
        mapName = Request.Params["MAPNAME"];
      %>
      <ul>
        <li><a href="change_layers_via_dom.aspx?SESSION=<%= sessionId %>&MAPNAME=<%= mapName %>" target="scriptFrame">
          Change Layers via DOM</a>         
          <br>Show buildings built after 1980.
          <br>
          <% 
            fullPath = Server.MapPath("change_layers_via_dom.aspx");
          %>
            <a target="_blank" href="../common/viewsource.aspx?FILENAME=<%= fullPath %>" >View source</a>
        </li>
        
        <li><a href="create_new_squarefootage_layer_definition.aspx?SESSION=<%= sessionId %>&MAPNAME=<%= mapName %>" target="scriptFrame">
          Create Square Footage Layer</a>
          <br>Show a map themed by areas.
          <br>
          <% 
            fullPath = Server.MapPath("create_new_squarefootage_layer_definition.aspx");
          %>
            <a target="_blank" href="../common/viewsource.aspx?FILENAME=<%= fullPath %>" >View source</a>     
        </li>
        
        <li><a href="create_new_line_layer_definition.aspx?SESSION=<%= sessionId %>&MAPNAME=<%= mapName %>" target="scriptFrame">
          Create Hydro Line Layer</a>
          <br>Show a map themed by lines.
          <br>
          <% 
            fullPath = Server.MapPath("create_new_line_layer_definition.aspx");
          %>
            <a target="_blank" href="../common/viewsource.aspx?FILENAME=<%= fullPath %>" >View source</a>   
        </li>
        
        <li><a href="create_new_point_layer_definition.aspx?SESSION=<%= sessionId %>&MAPNAME=<%= mapName %>" target="scriptFrame">
          Create Points of Interest Layer</a>
          <br>Show a map themed by points.
          <br>
          <% 
            fullPath = Server.MapPath("create_new_point_layer_definition.aspx");
            String fullPath2 = Server.MapPath("RecentlyBuilt.LayerDefinition");
            String fullPath3 = Server.MapPath("layer_functions.aspx");
          %>
            <a target="_blank" href="../common/viewsource.aspx?FILENAME=<%= fullPath %>" >View source</a>         
        </li> 
      </ul>     
    <p>View source for support files:</p>
    <ul>
      <li><a target="_blank" href="../common/viewsource.aspx?FILENAME=<%= fullPath2 %>">RecentlyBuilt.LayerDefinition</a></li>
      <li><a target="_blank" href="../common/viewsource.aspx?FILENAME=<%= fullPath3 %>">layer_functions.aspx</a></li>
    </ul>
    <p>Map Debugging:</p>
    <a target="_blank" href="../common/mapinfo.aspx?SESSION=<%= sessionId %>&MAPNAME=<%= mapName %>">Map Information</a>
  </body>
</html>

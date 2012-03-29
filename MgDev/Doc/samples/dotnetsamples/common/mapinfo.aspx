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
<%@ Import Namespace="System.Xml" %>
<%@ Import Namespace="OSGeo.MapGuide" %>
<!-- #Include File="common.aspx" -->
<script runat="server">
String sessionId;
String mapName;
</script>
<html>
    <head>
        <title>Map Info</title>
        <style type="text/css">
            body, html { margin: 0; padding: 0 }
            body { font-family: Verdana; font-size: 0.6em; }
        
        </style>
    </head>
    <body>
        <%
        //---------------------------------------------------//
        // Initialize
        MapGuideApi.MgInitializeWebTier(Constants.WebConfigPath);
        sessionId = Request.Params["SESSION"];
        mapName = Request.Params["MAPNAME"];
        MgUserInformation userInfo = new MgUserInformation(sessionId);
        MgSiteConnection siteConnection = new MgSiteConnection();
        siteConnection.Open(userInfo);
        
        MgMap map = new MgMap(siteConnection);
        map.Open(mapName);
        %>
        <p>Map Name: <%= map.GetName() %></p>
        <p>Object Id: <%= map.GetObjectId() %></p>
        <p>Map Definition: <%= map.GetMapDefinition().ToString() %></p>
        <hr />
        <p>Display DPI: <%= map.GetDisplayDpi() %></p>
        <p>Display Height: <%= map.GetDisplayHeight() %></p>
        <p>Display Width: <%= map.GetDisplayWidth() %></p>
        <hr />
        <p>View Center: (<%= map.GetViewCenter().GetCoordinate().GetX() %>, <%= map.GetViewCenter().GetCoordinate().GetY() %>)</p>
        <p>View Scale: <%= map.GetViewScale() %></p>
        <hr />
        <%
        MgEnvelope dataExtents = map.GetDataExtent();
        MgCoordinate ll = dataExtents.GetLowerLeftCoordinate();
        MgCoordinate ur = dataExtents.GetUpperRightCoordinate();
        %>
        <h3>Data Extents</h3>
        <p>Lower Left: (<%= ll.GetX() %>, <%= ll.GetY() %>)</p>
        <p>Upper Left: (<%= ur.GetX() %>, <%= ur.GetY() %>)</p>
        <hr />
        <%
        MgLayerCollection layers = map.GetLayers();
        %>
        <h3>Layers (<%= layers.GetCount() %>)</h3>
        <table border="1">
            <tr>
                <th>Name</th>
                <th>Group</th>
                <th>Label</th>
                <th>Legend Visible</th>
                <th>Potential Visible</th>
                <th>Actual Visible</th>
                <th>Expand In Legend</th>
                <th>Layer Type</th>
                <th>Layer Definition</th>
                <th>Feature Source Id</th>
                <th>Object Id</th>
                <th>Needs Refresh</th>
            </tr>
            <% 
            for (int i = 0; i < layers.GetCount(); i++) { 
                MgLayerBase layer = layers.GetItem(i);
                MgLayerGroup group = layer.GetGroup();
            %>
            <tr>
                <td><%= layer.GetName() %></td>
                <td><%= group == null ? "" : group.Name %></td>
                <td><%= layer.GetLegendLabel() %></td>
                <td><%= layer.GetDisplayInLegend() %></td>
                <td><%= layer.GetVisible() %></td>
                <td><%= layer.IsVisible() %></td>
                <td><%= layer.GetExpandInLegend() %></td>
                <td><%= layer.GetLayerType() %></td>
                <td><a target="_blank" href="viewresourcecontent.aspx?SESSION=<%= sessionId %>&RESOURCEID=<%= layer.GetLayerDefinition().ToString() %>"><%= layer.GetLayerDefinition().ToString() %></a></td>
                <td><a target="_blank" href="viewresourcecontent.aspx?SESSION=<%= sessionId %>&RESOURCEID=<%= layer.GetFeatureSourceId() %>"><%= layer.GetLayerDefinition().ToString() %></a></td>
                <td><%= layer.GetObjectId() %></td>
                <td><%= layer.NeedsRefresh() %></td>
            </tr>
            <% } %>
        </table>
        <hr />
        <%
        MgLayerGroupCollection layerGroups = map.GetLayerGroups();
        %>
        <h3>Layer Groups (<%= layerGroups.GetCount() %>)</h3>
        <table border="1">
            <tr>
                <th>Name</th>
                <th>Group</th>
                <th>Label</th>
                <th>Legend Visible</th>
                <th>Potential Visible</th>
                <th>Actual Visible</th>
                <th>Expand In Legend</th>
                <th>Group Type</th>
            </tr>
            <% 
            for (int i = 0; i < layerGroups.GetCount(); i++) { 
                MgLayerGroup group = layerGroups.GetItem(i);
                MgLayerGroup parent = group.GetGroup();
            %>
            <tr>
                <td><%= group.GetName() %></td>
                <td><%= parent == null ? "" : parent.Name %></td>
                <td><%= group.GetLegendLabel() %></td>
                <td><%= group.GetDisplayInLegend() %></td>
                <td><%= group.GetVisible() %></td>
                <td><%= group.IsVisible() %></td>
                <td><%= group.GetExpandInLegend() %></td>
                <td><%= group.GetLayerGroupType() %></td>
            </tr>
            <% } %>
        </table>
    </body>
</html>
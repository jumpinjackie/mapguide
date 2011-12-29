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
<html>
<%@ page import="org.osgeo.mapguide.*" %>
<%@ page import="java.util.*" %>
<%@ page import="java.io.*" %>
<%@ page import="java.text.*" %>
<%@ page import="javax.servlet.jsp.*" %>
<%@ page import="javax.servlet.http.*" %>
<%@ include file="../common/common.jsp" %>
  <head>
    <title>Layer Visibility</title>
    <meta content="text/html; charset=utf-8" http-equiv="Content-Type">
    <meta http-equiv="content-script-type" content="text/javascript">
    <meta http-equiv="content-style-type" content="text/css">
    <link href="../styles/globalStyles.css" rel="stylesheet"  type="text/css">
    <link href="../styles/otherStyles.css" rel="stylesheet" type="text/css">
  </head>
  <body class="AppFrame">
    <h1 class="AppHeading">Layer Visibility</h1>
    <%
      String sessionId = request.getParameter("SESSION");
      String mapName = request.getParameter("MAPNAME");

    try
    {
      MapGuideJavaApi.MgInitializeWebTier(webconfigFilePath);

      // Get the user information using the session id,
      // and set up a connection to the site server.
      
      MgUserInformation userInfo = new MgUserInformation(sessionId);
      MgSiteConnection siteConnection = new MgSiteConnection();
      siteConnection.Open(userInfo);
  
      // Get an instance of the required service(s).
      
      MgResourceService resourceService = (MgResourceService) siteConnection.CreateService(MgServiceType.ResourceService);
  
      MgMap map = new MgMap();
      map.Open(resourceService, mapName);

      MgLayerCollection layers = map.GetLayers(); // Get layer collection
      out.println("<p>Layers, in draw order:</p>");
      out.println("<table class=\"taskPane\" cellspacing=\"0\">");
      out.println("<tr><th class=\"rowHead\">Layer</th><th>GetVisible()</th><th>IsVisible()</th></tr>");
      Integer count = layers.GetCount(); 
      for (Integer i = 0; i < count; i++)
      {
        MgLayer layer = (MgLayer) layers.GetItem(i);
        out.println("<tr><td class=\"rowHead\">" + layer.GetName() + 
        "</td><td>" + (layer.GetVisible() ? "on" : "off") + "</td><td>" + (layer.IsVisible() ? "on" : "off") + "</td></tr>\n");
      }
      out.println("</table>");
    }
    catch (MgException e)
    {
      out.println("<p><strong>Error:</strong> ");
      out.println(e.GetDetails());
      out.println("</p>");
    }
    %>
    <p><code>GetVisible()</code> returns the state of the visibility flag for a given layer (the check box in the
      Layers pane). <code>IsVisible()</code> returns the actual visibility of a layer, based on its visibility flag, 
      the visibility flag of any groups that include the layer, and whether the layer has a defined style for the 
    current view scale.</p>
</body>
</html>
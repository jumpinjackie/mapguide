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
<%@ Import Namespace="System.Collections.Specialized" %>
<%@ Import Namespace="System.IO" %>
<%@ Import Namespace="OSGeo.MapGuide" %>

<script runat="server">
String sessionId;
String mapName;
MgSelection selection = null;
</script>
<!-- #Include File="../common/common.aspx" -->
<html>

  <head>
    <title>Viewer Sample Application - List Selected Parcels</title>
    <meta content="text/html; charset=utf-8" http-equiv="Content-Type">
    <meta http-equiv="content-script-type" content="text/javascript" />
    <meta http-equiv="content-style-type" content="text/css" />
    <link href="../styles/globalStyles.css" rel="stylesheet" type="text/css">
    <link href="../styles/otherStyles.css" rel="stylesheet" type="text/css">
  </head>

  <body class="AppFrame" >

    <h1 class="AppHeading">Selected Parcels</h1>

    <%
    sessionId = Request.Params["SESSION"];
    mapName = Request.Params["MAPNAME"];

    try
    {
      // Initialize the Web Extensions and connect to the Server using
      // the Web Extensions session identifier stored in PHP session state.

      MapGuideApi.MgInitializeWebTier (Constants.WebConfigPath);

      MgUserInformation userInfo = new MgUserInformation(sessionId);
      MgSiteConnection siteConnection = new MgSiteConnection();
      siteConnection.Open(userInfo);
      
      MgMap map = new MgMap(siteConnection);
      map.Open(mapName);

      // ----------------------------------------------------------
      // Use the following code for AJAX or DWF Viewers
      // This requires passing selection data via HTTP POST

      MgReadOnlyLayerCollection layers = null;
      if (!String.IsNullOrEmpty(Request.Form["SELECTION"]))
      {
        selection = new MgSelection(map, HttpUtility.UrlDecode(Request.Form["SELECTION"]));
        layers = selection.GetLayers();
      }
      // ---------------------------------------------------------

      // ---------------------------------------------------------
      // Use the following code for AJAX Viewers only.
      // This does not require passing selection data via HTTP POST.
      //
      // selection = new MgSelection(map);
      // selection.Open(resourceService, mapName);
      // layers = selection.GetLayers();
      // ---------------------------------------------------------

      if (layers != null && selection != null)
      {
        for (int i = 0; i < layers.GetCount(); i++)
        {
          // Only check selected features in the Parcels layer.

          MgLayerBase layer = layers.GetItem(i);

          if (layer.GetName() == "Parcels")
          {

            // Create a filter containing the IDs of the selected features on this layer

            String layerClassName = layer.GetFeatureClassName();
            
            MgFeatureReader featureReader = selection.GetSelectedFeatures(layer, layerClassName, false);

            // Process each item in the MgFeatureReader, displaying the owner name

            try {
                while (featureReader.ReadNext())
                {
                  String val = featureReader.GetString("NAME") + "<br />&nbsp;&nbsp;" + featureReader.GetString("RPROPAD");
                  Response.Write(val + "<br />");
                }
            } finally { //Finally guarantees code in this block will execute
                featureReader.Close();
            }
          }
        }
      }
      else 
      {
        Response.Write("No selected layers");
        Response.Write("</p>");
      }
    }
    catch (MgException e)
    {
      Response.Write("<p>" + e.GetExceptionMessage() + "</p>");
      Response.Write("<p>" + e.GetDetails() + "</p>");
    }
    %>

    <% String fileName = Server.MapPath("listselection.aspx"); %>
    <p><a target="_blank" href="../common/viewsource.aspx?FILENAME=<%= fileName %>" >View source</a></p>
  </body>

</html>

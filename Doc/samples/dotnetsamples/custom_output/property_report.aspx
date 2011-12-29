<!-- #Include File="../common/common.aspx" -->
<%@ Page language="c#" %>
<%@ Import Namespace="System" %>
<%@ Import Namespace="System.Collections.Specialized" %>
<%@ Import Namespace="System.IO" %>
<%@ Import Namespace="OSGeo.MapGuide" %>
<script runat="server">
String sessionId;
String mapName;
</script>
<%

//
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
//

sessionId = Request.Params["SESSION"];
String xmlSelection = HttpUtility.UrlDecode(Request.Params["SELECTION"]);
mapName = Request.Params["MAPNAME"];
double mapScale = Convert.ToDouble(Request.Params["SCALE"]);
int imageHeight = Convert.ToInt32(Request.Params["HEIGHT"]);
int imageWidth = Convert.ToInt32(Request.Params["WIDTH"]);
double mapCenterX = Convert.ToDouble(Request.Params["CENTERX"]);
double mapCenterY = Convert.ToDouble(Request.Params["CENTERY"]);

String owner = "";
String address = "";
String billingAddress = "";
String description = "";
 
try
{
  // Initialize the Web Extensions and connect to the Server using
  // the Web Extensions session identifier stored in PHP session state.

  MapGuideApi.MgInitializeWebTier (Constants.WebConfigPath);

  MgUserInformation userInfo = new MgUserInformation(sessionId);

  MgSiteConnection siteConnection = new MgSiteConnection();
  siteConnection.Open(userInfo);

  // If any parcels have been selected, recenter the image on the first selected parcel.
  if (!string.IsNullOrEmpty(xmlSelection))
  {
    MgResourceService resourceService = (MgResourceService)siteConnection.CreateService(MgServiceType.ResourceService);
    MgFeatureService featureService = (MgFeatureService)siteConnection.CreateService(MgServiceType.FeatureService);
    MgFeatureQueryOptions queryOptions = new MgFeatureQueryOptions();

    MgMap map = new MgMap(siteConnection);
    map.Open(mapName);

    MgSelection selection = new MgSelection(map, xmlSelection);
    MgReadOnlyLayerCollection layers = selection.GetLayers();

    if (layers != null)
    {
      for (int i = 0; i < layers.GetCount(); i++)
      {
        // Only check selected features in the Parcels layer.
        MgLayer layer = (MgLayer)layers.GetItem(i);
        if (layer != null && layer.GetName() == "Parcels")
        {
          // Create a filter containing the IDs of the selected features on this layer
          String layerClassName = layer.GetFeatureClassName();
          String selectionString = selection.GenerateFilter(layer, layerClassName);

          // Get the feature resource for the selected layer

          String layerFeatureId = layer.GetFeatureSourceId();
          MgResourceIdentifier layerFeatureResource = new MgResourceIdentifier(layerFeatureId);

          // Apply the filter to the feature resource for the selected layer. This returns
          // an MgFeatureReader of all the selected features.

          queryOptions.SetFilter(selectionString);
          MgFeatureReader featureReader = featureService.SelectFeatures(layerFeatureResource, layerClassName, queryOptions);

          // Create the report details using the first selected parcel. Position the image so the
          // centroid of the selected parcel is in the center of the map.

          featureReader.ReadNext();
          MgByteReader featureAgfGeometry = featureReader.GetGeometry("SHPGEOM");
          MgAgfReaderWriter agfReaderWriter = new MgAgfReaderWriter();
          MgGeometry featureGeometry = agfReaderWriter.Read(featureAgfGeometry);
          MgPoint centroid = featureGeometry.GetCentroid();
          mapCenterX = centroid.GetCoordinate().GetX();
          mapCenterY = centroid.GetCoordinate().GetY();

          owner = featureReader.GetString("RNAME");
          address = featureReader.GetString("RPROPAD");
          billingAddress = featureReader.GetString("RBILAD");
          description = featureReader.GetString("RLDESCR1") + "<br/>" + featureReader.GetString("RLDESCR2") + "<br/>" + featureReader.GetString("RLDESCR3") + "<br/>";

        }
      }
    }
  }

}
catch (MgException e)
{
  Response.Write("<p>" + e.GetExceptionMessage() + "</p>");
  Response.Write("<p>" + e.GetDetails() + "</p>");
}
String selectionEncoded = HttpUtility.UrlEncode(xmlSelection);
String imageUrl = "createmapimage.aspx?SESSION=" + sessionId + "&SELECTION=" + selectionEncoded + "&MAPNAME=" + mapName + "&SCALE=" + mapScale + "&HEIGHT=" + imageHeight + "&WIDTH=" + imageWidth + "&CENTERX=" + mapCenterX + "&CENTERY=" + mapCenterY;

%>
<html>
  <head>
    <title>Property Report</title>
    <meta content="text/html; charset=utf-8" http-equiv="Content-Type">
    <meta http-equiv="content-script-type" content="text/javascript">
    <meta http-equiv="content-style-type" content="text/css">
    <link href="../styles/globalStyles.css" rel="stylesheet" type="text/css">
    <style>
      img {float: left; margin-right: 10px; border: 1px solid #999; padding: 4px}
    </style>
  </head>
  <body>
    <h1>Property Report</h1>
  <p><img id="propertyImage" src="<%= imageUrl %>" width="<%=imageWidth%>" height="<%= imageHeight %>" />
    <%
    if (string.IsNullOrEmpty(xmlSelection))
    {
      Response.Write("No properties selected.");
    }
    else
    {
      Response.Write("Owner: " + owner + "<br/>\n");
      Response.Write("Address: " + address + "<br/>\n");
      Response.Write("Billing Address: " + billingAddress + "<br/>\n");
      Response.Write("Description: " + description + "<br/>\n");
    }
    %>
  </p>
</body>
</html>
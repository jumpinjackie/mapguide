<!-- #Include File="../common/common.aspx" -->
<%@ Page language="c#" ValidateRequest="false" %>
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

try
{
  // Initialize the Web Extensions and connect to the Server using
  // the Web Extensions session identifier stored in PHP session state.

  MapGuideApi.MgInitializeWebTier (Constants.WebConfigPath);

  MgUserInformation userInfo = new MgUserInformation(sessionId);

  MgSiteConnection siteConnection = new MgSiteConnection();
  siteConnection.Open(userInfo);

  // Create the necessary services.

  MgResourceService resourceService = (MgResourceService)siteConnection.CreateService(MgServiceType.ResourceService);
  MgRenderingService renderingService = (MgRenderingService)siteConnection.CreateService(MgServiceType.RenderingService);

  // Open the map and get its SRS

  MgMap map = new MgMap(siteConnection);
  map.Open(mapName);
  String srsWkt = map.GetMapSRS();
  MgCoordinateSystemFactory coordinateSystemFactory = new MgCoordinateSystemFactory();
  MgCoordinateSystem srs = coordinateSystemFactory.Create(srsWkt);

  MgSelection selection = null;
  if (!string.IsNullOrEmpty(xmlSelection))
  {
      selection = new MgSelection(map, xmlSelection);
  }
  else
  {
      selection = new MgSelection(map);
  }

  MgColor color = new MgColor(205, 189, 156);

  MgGeometryFactory geometryFactory = new MgGeometryFactory();
  MgCoordinate mapCenterCoordinate = geometryFactory.CreateCoordinateXY(mapCenterX, mapCenterY);

  // Convert the height in pixels to map units.
  // Create an envelope that contains the image area to display.

  double displayInInches = imageHeight / 96;
  double displayInMeters = displayInInches * .0254;
  double mapHeightInMeters = displayInMeters * mapScale;
  double mapHeightInMapUnits = srs.ConvertMetersToCoordinateSystemUnits(mapHeightInMeters);
  double envelopeOffsetY = mapHeightInMapUnits / 2;
  double envelopeOffsetX = imageWidth / imageHeight * envelopeOffsetY;
  MgEnvelope envelope = new MgEnvelope(mapCenterX - envelopeOffsetX,
    mapCenterY - envelopeOffsetY, mapCenterX + envelopeOffsetX,
    mapCenterY + envelopeOffsetY);

  // Render the image and send it to the browser.

  MgByteReader byteReader = renderingService.RenderMap(map, selection, envelope, imageWidth, imageHeight, color, "PNG");
  Utility.OutputReaderContent(byteReader, Response);
}
catch (MgException e)
{
  Response.Write("<p>" + e.GetExceptionMessage() + "</p>");
  Response.Write("<p>" + e.GetDetails() + "</p>");
}

%>

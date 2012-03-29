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
mapName = Request.Params["MAPNAME"];

try
{
  // Initialize the Web Extensions and connect to the server using
  // the Web Extensions session identifier stored in PHP session state.

  MapGuideApi.MgInitializeWebTier (Constants.WebConfigPath);

  MgUserInformation userInfo = new MgUserInformation(sessionId);
  MgSiteConnection siteConnection = new MgSiteConnection();
  siteConnection.Open(userInfo);

  // Create an instance of ResourceService and use that to open the
  // current map instance stored in session state.

  MgResourceService resourceService = (MgResourceService)siteConnection.CreateService(MgServiceType.ResourceService);
  MgMap map = new MgMap(siteConnection);
  map.Open(mapName);

  MgMappingService mappingService = (MgMappingService)siteConnection.CreateService(MgServiceType.MappingService);

  MgDwfVersion dwfVersion = new MgDwfVersion("6.01", "1.2");

  MgPlotSpecification plotSpec = new MgPlotSpecification(8.5f, 11.0f, MgPageUnitsType.Inches, 0.5f, 0.5f, 0.5f, 0.5f);

  MgResourceIdentifier layoutRes = new MgResourceIdentifier("Library://Samples/Sheboygan/Layouts/SheboyganMap.PrintLayout");
  MgLayout layout =  new MgLayout(layoutRes, "City of Sheboygan", MgPageUnitsType.Inches);

  MgMapPlotCollection plotCollection = new MgMapPlotCollection();

  MgMapPlot plot1 = new MgMapPlot(map, plotSpec, layout);
  plot1.SetCenterAndScale(map.GetViewCenter().GetCoordinate(), map.GetViewScale() * 2);
  plotCollection.Add(plot1);

  // Create a second map for the second sheet in the DWF. This second sheet uses the print layout
  // to display a page title and legend.

  MgMap map2 = new MgMap(siteConnection);
  map2.Create(map.GetMapDefinition(), "Sheet 2");
  MgMapPlot plot2 = new MgMapPlot(map2, plotSpec, layout);
  plot2.SetCenterAndScale(map.GetViewCenter().GetCoordinate(), map.GetViewScale());
  // plot2 = new MgMapPlot(map2, map.GetViewCenter().GetCoordinate(), map.GetViewScale(), plotSpec, layout);
  plotCollection.Add(plot2);

  MgByteReader byteReader = mappingService.GenerateMultiPlot(plotCollection, dwfVersion);

  // Now output the resulting DWF.
  Utility.OutputReaderContent(byteReader, Response);

}
catch (MgException e)
{
  String result = e.GetExceptionMessage() + "<br><br>" + e.GetDetails();

  Response.ContentType = "text/html";
  Response.Headers.Add("Content-Length", result.Length.ToString());
  Response.Write(result);
}

%>

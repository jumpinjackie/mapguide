<%--
  -Copyright (C) 2004-2006  Autodesk, Inc.
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
<%@ include file ="../utilityfunctions.jsp" %>
<%@ include file ="findaddressfunctions.jsp" %>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>

<head>
    <title>Find Address</title>
    <meta content="text/html; charset=utf-8" http-equiv="Content-Type">
    <link rel="stylesheet" href="../styles/gt.css" type="text/css">

    <script language=javascript>
        function InitDocument()
        {
            this.ZoomToResult();
        }
    </script>
</head>

<body onload="InitDocument()">

<table class="RegText" border="0" cellspacing="0" width="100%">
    <tr><td class="Title">&nbsp;Find Address<hr></td></tr>
    <tr><td class="SubTitle">Search Results</td></tr>

<%
    String mgSessionId = request.getParameter("SESSION");
    GeocodeAddress addr = null;
    boolean success = false;

    try
    {
        // Initialize the Web Extensions and connect to the Tux Site using
        // the session identifier stored in JSP session state.

        InitializeWebTier();

        MgUserInformation userInfo = new MgUserInformation(mgSessionId);
        MgSiteConnection siteConnection = new MgSiteConnection();
        siteConnection.Open(userInfo);

        String address = request.getParameter("address") + ", Sheboygan, WI";

        // Make the request to geocoder.us passing the address.
        addr = requestGeocodeAddress(address);

        if (addr != null && addr.lon.length() > 0 && addr.lat.length() > 0)
        {
            // The geocode successfully returned a location.

            out.write("<tr><td><img src=\"../images/pushpinblue.jpg\">");
            out.write("<a href=\"gotopoint.jsp?X=" + addr.lon + "&Y=" + addr.lat + "&Scale=2000\" target=\"scriptFrame\">  " + addr.address1 + "</a></td></tr>");
            out.write("<tr><td>" + addr.address2 + "</td></tr>");
            out.write("<tr><td class=\"Spacer\"></td></tr>");
            out.write("<tr><td>Longitude: " + addr.lon + "</td></tr>");
            out.write("<tr><td>Latitude: " + addr.lat + "<hr></td></tr>");

            // Create ResourceService and FeatureService objects and use
            // the ResourceService object to open the map instance in
            // session state.

            MgResourceService resourceService = (MgResourceService)siteConnection.CreateService(MgServiceType.ResourceService);
            MgFeatureService featureService = (MgFeatureService)siteConnection.CreateService(MgServiceType.FeatureService);

            MgMap map = new MgMap();
            map.Open(resourceService, "Sheboygan");

            // Check the map for the AddressMarker layer. If it does not
            // exist then create a feature source to store address results
            // and a layer to display them.

            MgResourceIdentifier addressMarkerDataResId = new MgResourceIdentifier("Session:" + mgSessionId + "//AddressMarker.FeatureSource");
            MgLayer addressLayer = GetLayerByName(map, "AddressMarker");
            if (addressLayer == null)
            {
                CreateAddressMarkerFeatureSource(featureService, addressMarkerDataResId);
                addressLayer = CreateAddressMarkerLayer(resourceService, addressMarkerDataResId, mgSessionId);

                map.GetLayers().Insert(0, addressLayer);
            }
            else if (addressLayer.GetVisible())
            {
                // If the layer exists and is visible, then display the
                // previous results.

                EmitAddressResults(out, featureService, addressMarkerDataResId, mgSessionId);
            }

            // Insert the results of the Geo-Code into the temporary
            // feature source and ensure the address marker layer
            // is visible.

            MgAgfReaderWriter geometryReaderWriter = new MgAgfReaderWriter();
            MgGeometryFactory geometryFactory = new MgGeometryFactory();
            MgPoint addrPoint = geometryFactory.CreatePoint(geometryFactory.CreateCoordinateXY(Double.parseDouble(addr.lon), Double.parseDouble(addr.lat)));

            MgPropertyCollection properties = new MgPropertyCollection();
            properties.Add(new MgStringProperty("Address", addr.address1));
            properties.Add(new MgGeometryProperty("Location", geometryReaderWriter.Write(addrPoint)));

            MgFeatureCommandCollection commands = new MgFeatureCommandCollection();
            commands.Add(new MgInsertFeatures("AddressMarker", properties));

            featureService.UpdateFeatures(addressMarkerDataResId, commands, false);

            addressLayer.SetVisible(true);
            addressLayer.ForceRefresh();

            map.Save(resourceService);

            success = true;
        }
        else
        {
            out.write("<tr><td>Address not found: " + address + "<hr></td></tr>");
        }
    }
    catch (MgException e)
    {
        out.write("<tr><td>" + e.GetExceptionMessage()+ "</td></tr>");
        out.write("<tr><td class=\"Spacer\"></td></tr>");
        out.write("<tr><td>" + e.GetDetails() + "</td></tr>");
    }
%>

</table>

Geocoding Provided by: <a href="http://geocoder.us/" target="_blank">geocoder.us</a>.<br>
Locative Tecnologies offers the free geocoder.us web service for non commercial purposes. If you wish to use this geocoding web service in an production application you must sign up for a commercial license. Click the link above for more information.

</body>

<script language=javascript>
function ZoomToResult()
{
    success = <%= success? "true": "false" %>;
    if(success)
        parent.parent.ZoomToView(<%= addr!=null? addr.lon: "0" %>, <%= addr!=null? addr.lat: "0" %>, 2000, true);
}
</script>

</html>

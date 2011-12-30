<%--
Copyright (C) 2004-2006  Autodesk, Inc.
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
<%@ Import Namespace="OSGeo.MapGuide" %>
<!-- #Include File="../utilityfunctions.aspx" -->

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN" >
<html>
<head>
    <title>Viewer Sample Application - Clear Address Results</title>
    <meta content="text/html; charset=utf-8" http-equiv="Content-Type">
    <meta http-equiv="content-script-type" content="text/javascript">
</head>
<%
String mgSessionId = GetRequestParameters()["SESSION"];

try
{
    // Initialize the web-extensions and connect to the Site using
    // the session identifier stored

    InitializeWebTier();

    MgUserInformation userInfo = new MgUserInformation(mgSessionId);
    MgSiteConnection siteConnection = new MgSiteConnection();
    siteConnection.Open(userInfo);


    MgFeatureService featureService = siteConnection.CreateService(MgServiceType.FeatureService) as MgFeatureService;

    MgResourceIdentifier addressMarkerDataResId = new MgResourceIdentifier("Session:" + mgSessionId + "//AddressMarker.FeatureSource");

    MgFeatureCommandCollection commands = new MgFeatureCommandCollection();
    commands.Add(new MgDeleteFeatures("AddressMarker", "ID like '%'"));

    featureService.UpdateFeatures(addressMarkerDataResId, commands, false);

    // Create a ReserviceService object and use it to open the Map
    // object from the sessions repository. Use the Map object to hide
    // the "ParcelMarker" layer and then save the updated Map back to
    // the session.

    MgResourceService resourceService = siteConnection.CreateService(MgServiceType.ResourceService) as MgResourceService;
    MgMap map = new MgMap();
    map.Open(resourceService, "Sheboygan");

    MgLayer layer = GetLayerByName(map, "AddressMarker");
    layer.SetVisible(false);
    layer.ForceRefresh();

    map.Save(resourceService);
}
catch (MgException mge)
{
    Response.Write(mge.GetExceptionMessage());
    Response.Write(mge.GetDetails());
}
catch (Exception exc)
{
    Response.Write(exc.Message);
}
%>

<!-- Finally emit this function and assocate it with the onLoad event  -->
<!-- for the page so that it gets executed when this page loads in the -->
<!-- browser. The function calls the Refresh method on the Viewer      -->
<!-- frame forcing the Viewer to update itself to match the state of   -->
<!-- the Server-Side Map object.                                       -->

<script language="javascript">
function OnPageLoad()
{
   parent.Refresh();
   parent.taskFrame.taskPaneFrame.location.href = "findaddressmain.aspx?SESSION=<%= mgSessionId %>";
}
</script>

<body onload="OnPageLoad()">
</body>

</html>

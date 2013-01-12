<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<!--
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
-->
<%@ Page Language="c#" %>

<%@ Import Namespace="System" %>
<%@ Import Namespace="System.Collections.Specialized" %>
<%@ Import Namespace="System.IO" %>
<%@ Import Namespace="OSGeo.MapGuide" %>
<!-- #Include File="../common/common.aspx" -->
<script runat="server">
    String sessionId;
    String mapName;
</script>
<html>
  <head>
    <meta content="text/html; charset=utf-8" http-equiv="Content-Type">
    <title>Clear Lines</title>

    <meta http-equiv="content-style-type" content="text/css">
    <link href="../styles/globalStyles.css" rel="stylesheet"  type="text/css">
    <link href="../styles/otherStyles.css" rel="stylesheet" type="text/css">
    <meta http-equiv="content-script-type" content="text/javascript">
    <script language="javascript" type="text/javascript">
      function OnPageLoad()
      {
        parent.mapFrame.Refresh();
      }
    </script>
  </head>

  <body onLoad="OnPageLoad()">
    <%
      try
      {
        // --------------------------------------------------//
        // Basic initialization
        MapGuideApi.MgInitializeWebTier(Constants.WebConfigPath);

        // Get the information passed from the viewer
        sessionId = Request.Params["SESSION"];
        mapName = Request.Params["MAPNAME"];

        // Get the user information using the session id
        // and set up a connection to the site server
        MgUserInformation userInfo = new MgUserInformation(sessionId);
        MgSiteConnection siteConnection = new MgSiteConnection();
        siteConnection.Open(userInfo);

        // Get an instance of the required service(s)
        MgResourceService resourceService = (MgResourceService)siteConnection.CreateService(MgServiceType.ResourceService);

        // --------------------------------------------------//
        String featureSourceName = "Session:" + sessionId + "//TemporaryLines.FeatureSource";
        MgResourceIdentifier resourceID = new MgResourceIdentifier(featureSourceName);

        resourceService.DeleteResource(resourceID);
      }
      catch (MgException e)
      {
        Response.Write("<script language=\"javascript\" type=\"text/javascript\"> \n");
        Response.Write("    alert(\" " + e.GetExceptionMessage() + " \"); \n");
        Response.Write("</script> \n");
      }
    %>
  </body>
</html>
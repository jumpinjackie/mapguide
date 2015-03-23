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
String webLayout;
String title;
</script>
<!-- #Include File="common/common.aspx" -->
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Frameset//EN" "http://www.w3.org/TR/html4/frameset.dtd">
<%

MgSite site = null;
MgUserInformation userInfo = null;
try
{
    // Initialize the web extensions,
    MapGuideApi.MgInitializeWebTier(Constants.WebConfigPath);

    // Connect to the site server and create a session
    userInfo = new MgUserInformation("Author", "author");
    site = new MgSite();
    site.Open(userInfo);
}
catch (MgException ex)
{
    Response.Write("Could not connect to the MapGuide site server. Please contact the MapGuide site administrator.\n");
    Response.End();
}

try
{
    sessionId = site.CreateSession();

    // Define some constants
    webLayout     = "Library://Samples/Layouts/AspNetSamples.WebLayout";
    title         = "MapGuide Developer's Guide asp.net Samples";
    
    MgSiteConnection siteConn = new MgSiteConnection();
    siteConn.Open(userInfo);
    
    // We check for the existence of the specified WebLayout 
    //
    // If it doesn't exist, we load a copy from the WebLayout.xml on disk. This is a basic example 
    // of programmatically loading resource content into to the repository.
    MgResourceService resSvc = (MgResourceService)siteConn.CreateService(MgServiceType.ResourceService);
    MgResourceIdentifier wlResId = new MgResourceIdentifier(webLayout);
    if (!resSvc.ResourceExists(wlResId)) {
        String xmlPath =  Server.MapPath("WebLayout.xml");
        MgByteSource wlByteSource = new MgByteSource(xmlPath);
        MgByteReader wlByteReader = wlByteSource.GetReader();
        // NOTE: The Author account generally has write access into the site repository
        // which is why we're doing it like this.
        // If this was an Anonymous user, they can't write into the session repository. We would normally 
        // load our content into a session-based repository and modify $webLayout to point to our 
        // session loaded resource
        resSvc.SetResource(wlResId, wlByteReader, null);
    }
}
catch (MgException ex)
{
    Response.Write("ERROR: " + ex.GetExceptionMessage() + "\n");
    Response.Write(ex.GetStackTrace() + "\n");
}
%>
<html>
  <head>
    <title><%= title %>
    </title>
    <meta content="text/html; charset=utf-8" http-equiv="Content-Type" />
    <meta http-equiv="content-script-type" content="text/javascript" />
    <meta http-equiv="content-style-type" content="text/css">
    <link href="styles/globalStyles.css" rel="stylesheet" type="text/css" />
    <link href="styles/globalStyles.css" rel="stylesheet" type="text/css" />
  </head>

  <frameset rows="110,*" frameborder="NO" border="0" framespacing="0">
    <frame src="common/Title.aspx?TitleText=<%= title %>" name="TitleFrame" scrolling="NO" noresize />
    <frame src="../mapviewerajax/?SESSION=<%= sessionId %>&WEBLAYOUT=<%= webLayout %>" name="ViewerFrame" />
  </frameset>
</html>
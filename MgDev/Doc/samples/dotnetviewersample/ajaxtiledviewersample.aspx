<%--
Copyright (C) 2004-2007  Autodesk, Inc.
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

<%@ Page language="c#" Debug="true"%>
<%@ Import Namespace="System.IO" %>
<%@ Import Namespace="System.Collections.Specialized" %>
<%@ Import Namespace="System.Text" %>
<%@ Import Namespace="System.Globalization" %>
<%@ Import Namespace="OSGeo.MapGuide" %>

<!-- #Include File="utilityfunctions.aspx -->

<script runat="server">
String webLayout = "Library://Samples/Sheboygan/Layouts/SheboyganAspTiled.WebLayout";
String defaultUser = "Anonymous";
String defaultPassword = "";
String sessionId = "";
</script>

<%
    try
    {
        // Initialize a session and register a variable to hold the
        // session id, then initialize the Web Extensions, connect
        // to the site, and create a session.

        InitializeWebTier();

        MgUserInformation userInfo = new MgUserInformation(defaultUser, defaultPassword);
        MgSite site = new MgSite();
        site.Open(userInfo);
        sessionId = site.CreateSession();
    }
    catch (Exception e)
    {
        Response.Write(e.Message);
    }
%>

<html>
<head>
	<title>Viewer Sample Application</title>
</head>
<frameset rows="30,*" frameborder="no" framespacing="0">
	<frame id="titleFrame" frameborder="no" marginwidth="0" marginheight="0" scrolling="no" src="title.html">
	<frame id="viewerFrame" frameborder="no" marginwidth="0" marginheight="0" scrolling="no" src="../mapviewernet/ajaxviewer.aspx?SESSION=<%=sessionId%>&WEBLAYOUT=<%=webLayout%>">
</frameset>
</html>

<%--
  -Copyright (C) 2004-2007  Autodesk, Inc.
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
<%@ include file="common/common.jsp" %>
<%
    // Initialize a JSP session and register a variable to hold the
    // session id, then initialize the Web Extensions, and connect
    // to the site, and create a session.

    String realpath = getServletContext().getRealPath("/");
    // Initialize web tier with the site configuration file.  The config
    // file should be in the same folder as this script.
    String configPath =  realpath + "webconfig.ini";
    MapGuideJavaApi.MgInitializeWebTier( configPath );

    MgUserInformation userInfo = new MgUserInformation("Anonymous", "");
    MgSite site = new MgSite();

    site.Open(userInfo);

    String sessionId = site.CreateSession();
    String title = "JSP Samples";
    String webLayout = "Library://Samples/Layouts/JavaSamples.WebLayout";
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
    <frame src="common/title.jsp?TitleText=<%= title %>" name="TitleFrame" scrolling="NO" noresize />
    <frame src="/mapguide2011/mapviewerajax/?SESSION=<%= sessionId %>&WEBLAYOUT=<%= webLayout %>" name="ViewerFrame" />
  </frameset>
</html>

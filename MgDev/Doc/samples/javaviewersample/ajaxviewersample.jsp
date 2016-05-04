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
<%@ include file ="utilityfunctions.jsp" %>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Frameset//EN" "http://www.w3.org/TR/html4/frameset.dtd">
<%
    // Initialize a JSP session and register a variable to hold the
    // session id, then initialize the Web Extensions, and connect
    // to the site, and create a session.

    InitializeWebTier();

    MgUserInformation userInfo = new MgUserInformation("Anonymous", "");
    MgSite site = new MgSite();

    site.Open(userInfo);

    String sessionId = site.CreateSession();
    String webLayout = "Library://Samples/Sheboygan/Layouts/SheboyganJsp.WebLayout";
%>
<html>

<head>
    <title>Viewer Sample Application</title>
    <meta content="text/html; charset=utf-8" http-equiv="Content-Type">
</head>

<frameset rows="50,*" frameborder="NO" border="0" framespacing="0">
    <frame src="title.html?AppName=AJAX" name="TitleFrame" scrolling="NO" noresize>
    <frame src="../mapviewerjava/ajaxviewer.jsp?SESSION=<%= sessionId %>&WEBLAYOUT=<%= webLayout %>" name="ViewerFrame">
</frameset>

</html>

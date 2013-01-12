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

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN" >
<html>

<head>
    <title>Resource - Welcome</title>
    <meta content="text/html; charset=utf-8" http-equiv="Content-Type">
    <link rel="stylesheet" href="../styles/gt.css" type="text/css">
</head>

<body>

<%
String platform = Request.ServerVariables["SERVER_SOFTWARE"];
%>

<table class="RegText" border="0" cellspacing="0" width="100%">
    <tr><td class="Title">&nbsp;Overview<hr></td></tr>
    <tr><td>
        Welcome to the Sample Application.
        
        This sample demonstrates how to construct a
        Viewer application using the Web Extensions APIs. The application is based on a subset
        of the Sheboygan data set and shows how to:
        
        <ul>
            <li>Connect to a Site and create a session.</li>
            <li>Query feature data and work with geometry.</li>
            <li>Change the view of a map from server-side script.</li>
            <li>Read and manipulate XML based resources.</li>
            <li>Store temporary resources within a session.</li>
            <li>Plot a map as an EPlot DWF.</li>
        </ul>
    </td></tr>
    <tr><td>Server Platform: <font color="#AA0000"><%= platform %></font></td></tr>
</table>

</body>

</html>

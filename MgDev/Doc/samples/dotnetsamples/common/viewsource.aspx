<%--
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
--%>
<%@ Page language="c#" %>
<%@ Import Namespace="System" %>
<%@ Import Namespace="System.Collections.Specialized" %>
<%@ Import Namespace="System.IO" %>
<%@ Import Namespace="System.Web" %>
<%@ Import Namespace="OSGeo.MapGuide" %>
<script runat="server">
String sourceFile;
String fileContents;
</script>
<%
    sourceFile = Request.Params["FILENAME"];
	fileContents = HttpUtility.HtmlEncode(File.ReadAllText(sourceFile));
%>
<html>
	<head>
		<title>Source of <%= Path.GetFileName(sourceFile) %></title>
	</head>
	
	<body>
		<p><strong><%= sourceFile %></strong></p>
		<pre style="background-color: #f8f8f8; padding: 10px; border: 2px solid black">
<code><%= fileContents %>
</code>
		</pre>
	</body>
</html>
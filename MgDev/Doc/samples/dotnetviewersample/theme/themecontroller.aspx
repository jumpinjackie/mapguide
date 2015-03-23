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

<%@ Page language="c#" debug="true"%>
<%@ Import Namespace="System.IO" %>
<%@ Import Namespace="System.Collections" %>
<%@ Import Namespace="System.Collections.Specialized" %>
<%@ Import Namespace="System.Text" %>
<%@ Import Namespace="System.Globalization" %>
<%@ Import Namespace="OSGeo.MapGuide" %>
<%@ Import Namespace="Jayrock.Json" %>
<%@ Import Namespace="Jayrock.Json.Conversion" %>

<!-- #Include File="classes/theme.aspx -->
<!-- #Include File="classes/command.aspx -->

<%
    InitializeWebTier();

    NameValueCollection args = GetRequestParameters();
    Theme theme = new Theme(args);
    String responseType = "";
    String responseString = "";

    if(GetParameter(args, "COMMAND") != null)
    {
        int cmd = Int32.Parse(GetParameter(args, "COMMAND"));
        switch (cmd)
        {
            case Command.GETLAYERINFO:
                responseType = "text/plain";
                responseString = JsonConvert.ExportToString(theme.GetLayerInfo());
                break;
            case Command.GETPROPERTYMINMAXCOUNT:
                responseType = "text/plain";
                responseString = JsonConvert.ExportToString(theme.GetPropertyMinMaxCount());
                break;
            case Command.APPLYTHEME:
                responseType = "text/plain";
		        responseString = JsonConvert.ExportToString(theme.ApplyTheme());
                break;
        }
    }
    Response.ContentType = responseType;
    Response.Write(responseString);
%>
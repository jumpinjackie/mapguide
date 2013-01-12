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

<!-- #Include File="classes/query.aspx -->
<!-- #Include File="classes/command.aspx -->

<%
    InitializeWebTier();

    NameValueCollection args = GetRequestParameters();
    Query query = new Query(args);
    String responseType = "";
    String responseString = "";

    if(GetParameter(args, "COMMAND") != null)
    {
        int cmd = Int32.Parse(GetParameter(args, "COMMAND"));
        switch (cmd)
        {
            case Command.GETLAYERPROPERTIES:
                responseType = "text/plain";
                ArrayList properties = query.GetLayerProperties();
                responseString = JsonConvert.ExportToString(properties.ToArray());
                break;
            case Command.TOGGLESPATIALFILTER:
                responseType = "text/plain";
                responseString = JsonConvert.ExportToString(query.ToggleSpatialFilter());
                break;
            case Command.SHOWSPATIALFILTER:
                responseType = "text/plain";
                responseString = JsonConvert.ExportToString(query.ShowSpatialFilter());
                break;
            case Command.EXECUTEQUERY:
                responseType = "text/plain";
                ArrayList resultList = query.Execute();
                responseString = JsonConvert.ExportToString(resultList.ToArray());
                break;
            case Command.GETSELECTIONXML:
                responseType = "text/xml";
                responseString = query.GetSelectionXML();
                break;
        }
    }
    Response.ContentType = responseType;
    Response.Write(responseString);
%>
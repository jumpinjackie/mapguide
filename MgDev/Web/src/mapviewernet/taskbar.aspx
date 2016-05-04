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
<%@ Import Namespace="System.IO" %>
<%@ Import Namespace="OSGeo.MapGuide" %>

<!-- #Include File="common.aspx -->

<script runat="server">
String locale;
</script>

<%
    locale = "";
    Response.Charset = "utf-8";
    GetRequestParameters();

    MgLocalizer.SetLocalizedFilesPath(Request.ServerVariables["APPL_PHYSICAL_PATH"] + "..\\localized\\");

    Response.Write(MgLocalizer.Localize(LoadTemplate(Request, "../viewerfiles/taskbar.templ"), locale, GetClientOS(Request)));
%>

<script runat="server">
void GetRequestParameters()
{
    if (Request.HttpMethod == "POST")
        GetParameters(Request.Form);
    else
        GetParameters(Request.QueryString);
}

void GetParameters(NameValueCollection parameters)
{
    locale = ValidateLocaleString(GetParameter(parameters, "LOCALE"));
}

</script>

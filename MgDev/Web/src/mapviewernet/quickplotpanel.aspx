<%--
Copyright (C) 2004-2010 by Autodesk, Inc.
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

<%@ Import Namespace="System" %>
<%@ Import Namespace="System.IO" %>
<%@ Import Namespace="System.Collections.Specialized" %>
<%@ Import Namespace="System.Text" %>

<!-- #Include File="common.aspx -->

<script runat="server">
String locale = "";
String mapName = "";
String sessionId = "";
int targetType = 0;
int us = 0;
</script>

<%
    Response.Charset = "utf-8";

    GetRequestParameters();

    MgLocalizer.SetLocalizedFilesPath(Request.ServerVariables["APPL_PHYSICAL_PATH"] + "..\\localized\\");
    String templ = MgLocalizer.Localize(LoadTemplate(Request, "../viewerfiles/quickplotpanel.templ"), locale, GetClientOS(Request));

    String vpath = GetSurroundVirtualPath(Request);
    String jsPath = "../viewerfiles/";
    String[] vals = {
                    jsPath,
                    jsPath,
                    targetType.ToString(NumberFormatInfo.InvariantInfo),
                    "quickplotgeneratepicture.aspx",
                    "quickplotpreviewinner.aspx"
                    };

    Response.Write(Substitute(templ, vals));
%>

<script runat="server">

void GetRequestParameters()
{
    if(Request.HttpMethod == "POST")
        GetParameters(Request.Form);
    else
        GetParameters(Request.QueryString);
}

void GetParameters(NameValueCollection parameters)
{
    sessionId = ValidateSessionId(GetParameter(parameters, "SESSION"));
    locale = ValidateLocaleString(GetParameter(parameters, "LOCALE"));
    targetType = GetIntParameter(parameters, "TARGETTYPE");
    us = GetIntParameter(parameters, "US");
    mapName = ValidateMapName(GetParameter(parameters, "MAPNAME"));
}

</script>

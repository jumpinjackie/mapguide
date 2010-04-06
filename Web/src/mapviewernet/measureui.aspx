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

<%@ Page language="c#" %>
<%@ Import Namespace="System" %>
<%@ Import Namespace="System.IO" %>
<%@ Import Namespace="System.Collections.Specialized" %>
<%@ Import Namespace="System.Text" %>
<%@ Import Namespace="System.Web" %>

<!-- #Include File="common.aspx -->

<script runat="server">
int target = 0;
String locale = "";
int popup = 0;
int cmdIndex = -1;
String mapName = "";
String sessionId = "";
double total = 0;
</script>

<%
    Response.Charset = "utf-8";

    GetRequestParameters();

    String templ = LoadTemplate(Request, "../viewerfiles/measureui.templ");

    MgLocalizer.SetLocalizedFilesPath(Request.ServerVariables["APPL_PHYSICAL_PATH"] + "..\\localized\\");
    templ = MgLocalizer.Localize(templ, locale, GetClientOS(Request));

    String vpath = GetSurroundVirtualPath(Request);
    String[] vals = {
                    locale,
                    target.ToString(NumberFormatInfo.InvariantInfo),
                    popup.ToString(NumberFormatInfo.InvariantInfo),
                    mapName,
                    sessionId,
                    total.ToString(NumberFormatInfo.InvariantInfo),
                    "0",
                    "0",
                    vpath + "measure.aspx",
                    vpath + "measure.aspx"
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
    mapName = ValidateMapName(GetParameter(parameters, "MAPNAME"));
    target = GetIntParameter(parameters, "TGT");
    popup = GetIntParameter(parameters, "POPUP");
    cmdIndex = GetIntParameter(parameters, "CMDINDEX");
    total = GetDoubleParameter(parameters, "TOTAL");
}

</script>

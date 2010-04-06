<%--
Copyright (C) 2004-2009 by Autodesk, Inc.
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
<%@ Import Namespace="System.Web" %>
<%@ Import Namespace="System.Globalization" %>
<%@ Import Namespace="OSGeo.MapGuide" %>

<!-- #Include File="common.aspx -->

<script runat="server">
String mapName = "";
String sessionId = "";
String locale = "";
int isTitle = 0;
int isLegend = 0;
int isArrow = 0;
int dpi = 0;
String title = "";
double scale = 0;
double centerX = 0;
double centerY = 0;
</script>

<%
    Response.Charset = "utf-8";

    try
    {
        //fetch the parameters for this request
        //
        GetRequestParameters();

        MgLocalizer.SetLocalizedFilesPath(Request.ServerVariables["APPL_PHYSICAL_PATH"] + "..\\localized\\");
        String templ = MgLocalizer.Localize(LoadTemplate(Request, "../viewerfiles/printablepage.templ"), locale, GetClientOS(Request));
        String agent = GetRootVirtualFolder(Request) + "/mapagent/mapagent.fcgi";
        String[] vals = {
            mapName,
            agent,
            scale.ToString(NumberFormatInfo.InvariantInfo),
            centerX.ToString(NumberFormatInfo.InvariantInfo),
            centerY.ToString(NumberFormatInfo.InvariantInfo),
            dpi.ToString(NumberFormatInfo.InvariantInfo),
            mapName,
            sessionId,
            isTitle.ToString(NumberFormatInfo.InvariantInfo),
            isLegend.ToString(NumberFormatInfo.InvariantInfo),
            isArrow.ToString(NumberFormatInfo.InvariantInfo),
            isTitle == 1 ? EscapeForHtml(title) : "",
            agent,
            mapName,
            sessionId };

        Response.Write(Substitute(templ, vals));
    }
    catch(Exception ne)
    {
        Response.Write(ne.Message);
    }
%>

<script runat="server">

void GetRequestParameters()
{
    if ("POST"== Request.HttpMethod)
    {
        GetParameters(Request.Form);
    }
    else
    {
        GetParameters(Request.QueryString);
    }
}

void GetParameters(NameValueCollection parameters)
{
    sessionId = ValidateSessionId(GetParameter(parameters, "SESSION"));
    locale = ValidateLocaleString(GetParameter(parameters, "LOCALE"));
    isTitle = GetIntParameter(parameters, "ISTITLE");
    isLegend = GetIntParameter(parameters, "ISLEGEND");
    isArrow = GetIntParameter(parameters, "ISARROW");
    dpi = GetIntParameter(parameters, "DPI");
    mapName = ValidateMapName(GetParameter(parameters, "MAPNAME"));
    scale = GetDoubleParameter(parameters, "SCALE");
    centerX = GetDoubleParameter(parameters, "CENTERX");
    centerY = GetDoubleParameter(parameters, "CENTERY");

    title = GetParameter(parameters, "TITLE");
}

</script>

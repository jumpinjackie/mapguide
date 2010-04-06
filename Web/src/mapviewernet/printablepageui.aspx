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
<%@ Import Namespace="System.Web" %>
<%@ Import Namespace="System.Globalization" %>
<%@ Import Namespace="OSGeo.MapGuide" %>

<!-- #Include File="common.aspx -->

<script runat="server">
int popup = 0;
int clientWidth = 0;
int dpi = 96;
String mapName = "";
String sessionId = "";
double scale = 0;
double centerX = 0;
double centerY = 0;
String locale = "";
</script>

<%
    Response.Charset = "utf-8";

    try
    {
        //fetch the parameters for this request
        //
        GetRequestParameters();

        MgLocalizer.SetLocalizedFilesPath(Request.ServerVariables["APPL_PHYSICAL_PATH"] + "..\\localized\\");
        String templ = MgLocalizer.Localize(LoadTemplate(Request, "../viewerfiles/printablepageui.templ"), locale, GetClientOS(Request));

        String[] vals = {
            popup.ToString(NumberFormatInfo.InvariantInfo),
            clientWidth.ToString(NumberFormatInfo.InvariantInfo),
            sessionId,
            mapName,
            scale.ToString(NumberFormatInfo.InvariantInfo),
            centerX.ToString(NumberFormatInfo.InvariantInfo),
            centerY.ToString(NumberFormatInfo.InvariantInfo),
            dpi.ToString(NumberFormatInfo.InvariantInfo),
            GetSurroundVirtualPath(Request) + "printablepage.aspx"};

        Response.Write(Substitute(templ, vals));
    }
    catch(Exception ne)
    {
        String errorMsg = EscapeForHtml(ne.Message);
        Response.Write(errorMsg);
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
    popup = GetIntParameter(parameters, "POPUP");
    clientWidth = GetIntParameter(parameters, "WIDTH");
    dpi = GetIntParameter(parameters, "DPI");
    mapName = ValidateMapName(GetParameter(parameters, "MAPNAME"));
    scale = GetDoubleParameter(parameters, "SCALE");
    centerX = GetDoubleParameter(parameters, "CENTERX");
    centerY = GetDoubleParameter(parameters, "CENTERY");
}

</script>

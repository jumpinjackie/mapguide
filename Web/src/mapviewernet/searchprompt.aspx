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
<%@ Import Namespace="System.Text" %>

<!-- #Include File="common.aspx -->

<script runat="server">
int cmdIndex = 0;
int target = 0;
int popup = 0;
int clientWidth = 0;
String locale = "";
String layerId = "";
String mapName = "";
String sessionId = "";
String filter = "";
int matchLimit = 0;
</script>

<%
    Response.Charset = "utf-8";

    GetRequestParameters();

    //load html template code and format it
    //
    String templ = LoadTemplate(Request, "../viewerfiles/searchprompt.templ");

    // localize template file.
    //
    MgLocalizer.SetLocalizedFilesPath(Request.ServerVariables["APPL_PHYSICAL_PATH"] + "..\\localized\\");
    templ = MgLocalizer.Localize(templ, locale, GetClientOS(Request));

    String[] vals = {
                    target.ToString(NumberFormatInfo.InvariantInfo),
                    popup.ToString(NumberFormatInfo.InvariantInfo),
                    cmdIndex.ToString(NumberFormatInfo.InvariantInfo),
                    clientWidth.ToString(NumberFormatInfo.InvariantInfo),
                    GetSurroundVirtualPath(Request) + "search.aspx",
                    cmdIndex.ToString(NumberFormatInfo.InvariantInfo),
                    target.ToString(NumberFormatInfo.InvariantInfo),
                    popup.ToString(NumberFormatInfo.InvariantInfo),
                    layerId,
                    mapName,
                    sessionId,
                    EscapeForHtml(filter),
                    matchLimit.ToString(NumberFormatInfo.InvariantInfo)
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
    locale =  GetParameter(parameters, "LOCALE");
    cmdIndex = GetIntParameter(parameters, "CMDINDEX");
    target = GetIntParameter(parameters, "TGT");
    popup = GetIntParameter(parameters, "POPUP");
    clientWidth = GetIntParameter(parameters, "WIDTH");
    layerId = GetParameter(parameters, "LAYER");
    mapName = GetParameter(parameters, "MAPNAME");
    sessionId = GetParameter(parameters, "SESSION");
    filter = GetParameter(parameters, "FILTER");
    matchLimit = GetIntParameter(parameters, "MR");
}

</script>

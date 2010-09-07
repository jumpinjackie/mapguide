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

<%@ Page Language="C#" ValidateRequest="false" %>

<%@ Import Namespace="System" %>
<%@ Import Namespace="System.Web" %>
<%@ Import Namespace="System.IO" %>
<%@ Import Namespace="System.Collections.Specialized" %>
<%@ Import Namespace="System.Text" %>
<%@ Import Namespace="System.Text.RegularExpressions"%>

<!-- #Include File="common.aspx -->

<script runat="server">
String locale = "";
String mapName = "";
String sessionId = "";
int targetType = 0;
int us = 0;
int scaleDenominator = 0;

NameValueCollection annotations = new NameValueCollection();
</script>

<%
    Response.Charset = "utf-8";

    GetRequestParameters();

    MgLocalizer.SetLocalizedFilesPath(Request.ServerVariables["APPL_PHYSICAL_PATH"] + "..\\localized\\");
    String templ = MgLocalizer.Localize(LoadTemplate(Request, "../viewerfiles/quickplotpreviewinner.templ"), locale, GetClientOS(Request));
    // Set some annotation labels in the page by replacing the placeholders in the html code
    StringBuilder sb = new StringBuilder(templ);
    foreach (String key in annotations)
    {
        sb.Replace(key, annotations[key]);
    }
    templ = sb.ToString();
    // Set the date annotation according to its format mask
    String pattern = @"\{date:.*\[net=(?<mask>.+?)\].+?\}";
    String mask    = Regex.Match(templ, pattern).Result("${mask}");
    templ = Regex.Replace(templ, pattern, DateTime.Now.ToString(mask));
    // The scale annotation
    
    String jsPath = "../viewerfiles/";
    String[] vals = {jsPath};

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
    scaleDenominator = GetIntParameter(parameters, "SCALE_DENOMINATOR");
    mapName = ValidateMapName(GetParameter(parameters, "MAPNAME"));
    
    // The parameters whose name matches this pattern will be treated as annotation
    String pattern = @"^\{field:.+\}$";
    foreach (String key in parameters.AllKeys)
    {
        if (Regex.Match(key, pattern).Success)
        {
            annotations.Add(key, HttpUtility.HtmlEncode(parameters[key]));
        }
    }
    
    annotations.Add("{scale}", String.Format("1 : {0}", scaleDenominator));
}

</script>

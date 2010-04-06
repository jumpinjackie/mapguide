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
<!-- #Include File="product.aspx -->

<script runat="server">
String sessionId = "";
String locale = "";

String titleBar = product + " Viewer";
String serverTitle = product + " Server";
String viewerTitle = product + " Viewer";
String viewerVersion = "2.1.0.0";
String serverVersion = "##Unknown";
</script>

<%
    Response.Charset = "utf-8";

    try
    {
        InitializeWebTier();

        MgLocalizer.SetLocalizedFilesPath(Request.ServerVariables["APPL_PHYSICAL_PATH"] + "..\\localized\\");

        //fetch the parameters for this request
        //
        GetRequestParameters();

        // Obtain server version info
        //
        MgUserInformation userInfo = new MgUserInformation(sessionId);
        userInfo.SetClientIp(GetClientIp(Request));
        userInfo.SetClientAgent(GetClientAgent());
        
        MgSite site = new MgSite();
        site.Open(userInfo);
        MgServerAdmin serverAdmin = new MgServerAdmin();
        serverAdmin.Open(userInfo);
        MgPropertyCollection infoProps = serverAdmin.GetInformationProperties();
        MgStringProperty versionProp = (MgStringProperty)infoProps.GetItem(MgServerInformationProperties.ServerVersion);
        serverVersion = versionProp.GetValue();

        String[] vals = { titleBar,
            titleBar,
            serverTitle,
            serverVersion,
            viewerTitle,
            viewerVersion };
        String templ = MgLocalizer.Localize(LoadTemplate(Request, "../viewerfiles/ajaxviewerabout.templ"), locale, GetClientOS(Request));
        Response.Write(Substitute(templ, vals));
    }
    catch (Exception ne)
    {
        Response.Write("<p>Exception: " + ne.Message);
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
}

</script>

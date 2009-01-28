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
<%@ Import Namespace="System.Web" %>

<!-- #Include File="common.aspx -->

<script runat="server">
String taskPane = "";
String session = "";
String webLayout = "";
String dwf = "";
String locale = "";
</script>

<%
    Response.Charset = "utf-8";

    GetRequestParameters();

    String templ = LoadTemplate(Request, "../viewerfiles/taskframe.templ");
    String[] vals = {
                    GetSurroundVirtualPath(Request) + "tasklist.aspx",
                    locale,
                    taskPane,
                    session,
                    HttpUtility.UrlEncode(webLayout),
                    dwf,
                    locale
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
    taskPane = GetParameter(parameters, "TASK");
    session = GetParameter(parameters, "SESSION");
    webLayout = GetParameter(parameters, "WEBLAYOUT");
    dwf = GetParameter(parameters, "DWF");
    locale = GetParameter(parameters, "LOCALE");
}

</script>

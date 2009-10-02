<%--
  -Copyright (C) 2004-2009 by Autodesk, Inc.
  -This library is free software; you can redistribute it and/or
  -modify it under the terms of version 2.1 of the GNU Lesser
  -General Public License as published by the Free Software Foundation.

  -This library is distributed in the hope that it will be useful,
  -but WITHOUT ANY WARRANTY; without even the implied warranty of
  -MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  -Lesser General Public License for more details.

  -You should have received a copy of the GNU Lesser General Public
  -License along with this library; if not, write to the Free Software
  -Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
--%>

<%@ page contentType="text/html; charset=UTF-8" pageEncoding="UTF-8" %>
<%@ page import="org.osgeo.mapguide.*" %>
<%@ page import="java.util.*" %>
<%@ page import="java.io.*" %>
<%@ page import="java.text.*" %>
<%@ page import="javax.servlet.jsp.*" %>
<%@ page import="javax.servlet.http.*" %>
<%@ include file="common.jsp" %>
<%@ include file="product.jsp" %>

<%!
String sessionId;
String locale;

String titleBar = product + " Viewer";
String serverTitle = product + " Server";
String viewerTitle = product + " Viewer";
String viewerVersion = "2.2.0.0";
String serverVersion;
%>

<%
sessionId = "";
locale = "";
serverVersion = "##Unknown";

response.setContentType("text/html; charset=UTF-8");
request.setCharacterEncoding("UTF-8");
PrintWriter writer = response.getWriter();
try
{
    MgLocalizer.SetLocalizedFilesPath(getServletContext().getRealPath("/") + "localized/");

    //fetch the parameters for this request
    //
    GetRequestParameters(request);

    InitializeWebTier();

    // Obtain server version info
    //
    MgUserInformation userInfo = new MgUserInformation();
    userInfo.SetMgSessionId(sessionId);
    userInfo.SetClientIp(GetClientIp(request));
    userInfo.SetClientAgent(GetClientAgent());

    MgSite site = new MgSite();
    site.Open(userInfo);
    MgServerAdmin serverAdmin = new MgServerAdmin();
    serverAdmin.Open(userInfo);
    MgPropertyCollection infoProps = serverAdmin.GetInformationProperties();
    MgStringProperty versionProp = (MgStringProperty)infoProps.GetItem(MgServerInformationProperties.ServerVersion);
    serverVersion = versionProp.GetValue();

    String vals[] = { titleBar,
        titleBar,
        serverTitle,
        serverVersion,
        viewerTitle,
        viewerVersion };
    String templ = MgLocalizer.Localize(LoadTemplate("/viewerfiles/ajaxviewerabout.templ"), locale, GetClientOS(request));
    response.getWriter().write(Substitute(templ, vals));
}
catch(Exception ne)
{
    String msg = ne.getLocalizedMessage();
    writer = response.getWriter();
    writer.write(msg);
    response.setContentLength(msg.length());
}

// Flush output buffer
response.flushBuffer();
%>

<%!
void GetRequestParameters(HttpServletRequest request)
{
    sessionId = GetParameter(request, "SESSION");
    locale = GetParameter(request, "LOCALE");
}
%>

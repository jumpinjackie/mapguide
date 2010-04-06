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

<%!
String mapName;
String sessionId;
int isTitle;
int isLegend;
int isArrow;
String title;
double scale;
double centerX;
double centerY;
int dpi;
String templFile;
String locale;
%>

<%
mapName = "";
sessionId = "";
isTitle = 0;
isLegend = 0;
isArrow = 0;
title = "";
scale = 0;
centerX = 0;
centerY = 0;
dpi = 0;
templFile = "";
locale = "";

response.setContentType("text/html; charset=UTF-8");
request.setCharacterEncoding("UTF-8");

PrintWriter writer = response.getWriter();
try
{
    MgLocalizer.SetLocalizedFilesPath(getServletContext().getRealPath("/") + "localized/");

    //fetch the parameters for this request
    //
    GetRequestParameters(request);
    String agent = GetRootVirtualFolder(request) + "/mapagent/mapagent.fcgi";
    String vals[] = { mapName,
        agent,
        String.valueOf(scale),
        String.valueOf(centerX),
        String.valueOf(centerY),
        String.valueOf(dpi),
        mapName,
        sessionId,
        String.valueOf(isTitle),
        String.valueOf(isLegend),
        String.valueOf(isArrow),
        isTitle == 1 ? EscapeForHtml(title) : "",
        agent,
        mapName,
        sessionId };
    String templ = MgLocalizer.Localize(LoadTemplate("/viewerfiles/printablepage.templ"), locale, GetClientOS(request));
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
    sessionId = ValidateSessionId(GetParameter(request, "SESSION"));
    locale = ValidateLocaleString(GetParameter(request, "LOCALE"));
    mapName = ValidateMapName(GetParameter(request, "MAPNAME"));
    isTitle = GetIntParameter(request, "ISTITLE");
    isLegend = GetIntParameter(request, "ISLEGEND");
    isArrow = GetIntParameter(request, "ISARROW");
    dpi = GetIntParameter(request, "DPI");
    scale = GetDoubleParameter(request, "SCALE");
    centerX = GetDoubleParameter(request, "CENTERX");
    centerY = GetDoubleParameter(request, "CENTERY");
    title = GetParameter(request, "TITLE");
}
%>

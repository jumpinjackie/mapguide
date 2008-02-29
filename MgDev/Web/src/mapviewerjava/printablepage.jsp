<%--
  -Copyright (C) 2004-2008 by Autodesk, Inc.
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
String scale;
String centerX;
String centerY;
String dpi;
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
scale = "";
centerX = "";
centerY = "";
dpi = "";
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
        scale,
        centerX,
        centerY,
        dpi,
        mapName,
        sessionId,
        String.valueOf(isTitle),
        String.valueOf(isLegend),
        String.valueOf(isArrow),
        isTitle == 1 ? title : "",
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
    if(IsParameter(request, "MAPNAME"))
        mapName = GetParameter(request, "MAPNAME");

    if(IsParameter(request, "SESSION"))
        sessionId = GetParameter(request, "SESSION");

    if(IsParameter(request, "ISTITLE"))
        isTitle = Integer.parseInt(GetParameter(request, "ISTITLE"));

    if(IsParameter(request, "ISLEGEND"))
        isLegend = Integer.parseInt(GetParameter(request, "ISLEGEND"));

    if(IsParameter(request, "ISARROW"))
        isArrow = Integer.parseInt(GetParameter(request, "ISARROW"));

    if(IsParameter(request, "TITLE"))
        title = GetParameter(request, "TITLE");

    if(IsParameter(request, "SCALE"))
        scale = GetParameter(request, "SCALE");

    if(IsParameter(request, "CENTERX"))
        centerX = GetParameter(request, "CENTERX");

    if(IsParameter(request, "CENTERY"))
        centerY = GetParameter(request, "CENTERY");

    if(IsParameter(request, "DPI"))
        dpi = GetParameter(request, "DPI");

    locale = GetParameter(request, "LOCALE");
}
%>

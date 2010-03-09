<%--
  -Copyright (C) 2004-2010 by Autodesk, Inc.
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
<%@ page isThreadSafe="false" %>

<%!
int popup;
String clientWidth;
String mapName;
String sessionId;
String scale;
String centerX;
String centerY;
String dpi;
String locale;
%>

<%
response.setContentType("text/html; charset=UTF-8");
request.setCharacterEncoding("UTF-8");
PrintWriter writer = response.getWriter();
try
{
    popup = 0;
    clientWidth = mapName = sessionId = scale = "";
    centerX = centerY = dpi = "";

    MgLocalizer.SetLocalizedFilesPath(getServletContext().getRealPath("/") + "localized/");

    //fetch the parameters for this request
    //
    GetRequestParameters(request);

    String templ = MgLocalizer.Localize(LoadTemplate("/viewerfiles/printablepageui.templ"), locale, GetClientOS(request));
    String vals[] = { String.valueOf(popup),
         clientWidth,
         sessionId,
         mapName,
         scale,
         centerX,
         centerY,
         dpi,
         GetSurroundVirtualPath(request) + "printablepage.jsp"};
    response.getWriter().write(Substitute(templ, vals));
}
catch(Exception ne)
{
    String msg = EscapeForHtml(ne.getLocalizedMessage());
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
    if(IsParameter(request, "POPUP"))
        popup = Integer.parseInt(GetParameter(request, "POPUP"));

    if(IsParameter(request, "WIDTH"))
        clientWidth = GetParameter(request, "WIDTH");

    if(IsParameter(request, "MAPNAME"))
        mapName = GetParameter(request, "MAPNAME");

    if(IsParameter(request, "SESSION"))
        sessionId = GetParameter(request, "SESSION");

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

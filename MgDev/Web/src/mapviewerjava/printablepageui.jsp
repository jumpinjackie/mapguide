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
int popup;
int clientWidth;
String mapName;
String sessionId;
double scale;
double centerX;
double centerY;
int dpi;
String locale;
%>

<%
response.setContentType("text/html; charset=UTF-8");
request.setCharacterEncoding("UTF-8");
PrintWriter writer = response.getWriter();
try
{
    popup = 0;
    clientWidth = 0;
    mapName = sessionId = "";
    scale = centerX = centerY = 0;
    dpi = 0;

    MgLocalizer.SetLocalizedFilesPath(getServletContext().getRealPath("/") + "localized/");

    //fetch the parameters for this request
    //
    GetRequestParameters(request);

    String templ = MgLocalizer.Localize(LoadTemplate("/viewerfiles/printablepageui.templ"), locale, GetClientOS(request));
    String vals[] = { String.valueOf(popup),
         String.valueOf(clientWidth),
         sessionId,
         mapName,
         String.valueOf(scale),
         String.valueOf(centerX),
         String.valueOf(centerY),
         String.valueOf(dpi),
         GetSurroundVirtualPath(request) + "printablepage.jsp"};
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
    popup = GetIntParameter(request, "POPUP");
    clientWidth = GetIntParameter(request, "WIDTH");
    dpi = GetIntParameter(request, "DPI");
    scale = GetDoubleParameter(request, "SCALE");
    centerX = GetDoubleParameter(request, "CENTERX");
    centerY = GetDoubleParameter(request, "CENTERY");
}
%>

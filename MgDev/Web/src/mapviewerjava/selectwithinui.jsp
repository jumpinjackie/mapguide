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

<%!
int popup = 0;
String mapName;
String sessionId;
String dwf;
String locale;
%>

<%
    response.setContentType("text/html; charset=UTF-8");
    request.setCharacterEncoding("UTF-8");
    popup = 0;
    mapName = "";
    sessionId = "";
    dwf = "";
    locale = "";

    MgLocalizer.SetLocalizedFilesPath(getServletContext().getRealPath("/") + "localized/");
    GetRequestParameters(request);

    String templ = MgLocalizer.Localize(LoadTemplate("/viewerfiles/selectwithinui.templ"), locale, GetClientOS(request));
    String[] vals = {
                    String.valueOf(popup),
                    GetSurroundVirtualPath(request) + "selectwithin.jsp",
                    mapName,
                    sessionId,
                    dwf
                    };
    response.getWriter().write(Substitute(templ, vals));
%>

<%!
void GetRequestParameters(HttpServletRequest request)
{
    popup = GetIntParameter(request, "POPUP");
    mapName = GetParameter(request, "MAPNAME");
    sessionId = GetParameter(request, "SESSION");
    dwf = GetParameter(request, "DWF");
    locale = GetParameter(request, "LOCALE");
}
%>

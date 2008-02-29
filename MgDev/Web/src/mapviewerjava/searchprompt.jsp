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
int cmdIndex;
int target;
int popup;
int clientWidth;
String layerId;
String mapName;
String sessionId;
String filter;
String matchLabel;
int matchLimit;
String locale;
%>

<%
    response.setContentType("text/html; charset=UTF-8");
    request.setCharacterEncoding("UTF-8");

    cmdIndex = 0;
    target = 0;
    popup = 0;
    clientWidth = 0;
    layerId = "";
    mapName = "";
    sessionId = "";
    filter = "";
    matchLimit = 0;
    locale = "";

    MgLocalizer.SetLocalizedFilesPath(getServletContext().getRealPath("/") + "localized/");
    GetRequestParameters(request);

    //load html template code and format it
    //
    String templ = MgLocalizer.Localize(LoadTemplate("/viewerfiles/searchprompt.templ"), locale, GetClientOS(request));
    String vals[] = {
                    String.valueOf(target),
                    String.valueOf(popup),
                    String.valueOf(cmdIndex),
                    String.valueOf(clientWidth),
                    GetSurroundVirtualPath(request) + "search.jsp",
                    String.valueOf(cmdIndex),
                    String.valueOf(target),
                    String.valueOf(popup),
                    layerId,
                    mapName,
                    sessionId,
                    EscapeForHtml(filter),
                    String.valueOf(matchLimit)
                    };
    response.getWriter().write(Substitute(templ, vals));
%>

<%!

void GetRequestParameters(HttpServletRequest request)
{
    cmdIndex = GetIntParameter(request, "CMDINDEX");
    target = GetIntParameter(request, "TGT");
    popup = GetIntParameter(request, "POPUP");
    clientWidth = GetIntParameter(request, "WIDTH");
    layerId = GetParameter(request, "LAYER");
    mapName = GetParameter(request, "MAPNAME");
    sessionId = GetParameter(request, "SESSION");
    filter = GetParameter(request, "FILTER");
    matchLimit = GetIntParameter(request, "MR");
    locale = GetParameter(request, "LOCALE");
}
%>

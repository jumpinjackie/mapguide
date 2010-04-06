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
    String clr;
    int allowTransparency;
    int transparent;
    String locale;
%>

<%
    response.setContentType("text/html; charset=UTF-8");
    request.setCharacterEncoding("UTF-8");
    clr = "000000";
    allowTransparency = 0;
    transparent = 0;
    locale = "";

    MgLocalizer.SetLocalizedFilesPath(getServletContext().getRealPath("/") + "localized/");
    GetRequestParameters(request);

    String templ = MgLocalizer.Localize(LoadTemplate("/viewerfiles/colorpicker.templ"), locale, GetClientOS(request));
    String[] vals = { clr,
                    allowTransparency == 0 ? "false" : "true",
                    transparent == 0 ? "false" : "true" };

    response.getWriter().write(Substitute(templ, vals));
%>

<%!
void GetRequestParameters(HttpServletRequest request)
{
    locale = ValidateLocaleString(GetParameter(request, "LOCALE"));
    clr = ValidateColorString(GetParameter(request, "CLR"));
    allowTransparency = GetIntParameter(request, "ALLOWTRANS");
    transparent = GetIntParameter(request, "TRANS");
}
%>

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
String taskPane = "";
String sessionId = "";
String webLayout = "";
String dwf = "";
String locale = "";
%>

<%
    response.setContentType("text/html; charset=UTF-8");
    request.setCharacterEncoding("UTF-8");
    GetRequestParameters(request);

    String url = URLDecoder.decode(taskPane, "UTF-8");
    int index = url.indexOf("?");

    if(index > 0)
    {
        String path = url.substring(0, index);
        String query = url.substring(index+1);

        if(query.length() > 0)
            url = String.format("%s?SESSION=%s&WEBLAYOUT=%s&DWF=%s&LOCALE=%s&%s", path, sessionId, URLEncoder.encode(webLayout, "UTF-8"), dwf, locale, query);
        else
            url = String.format("%s?SESSION=%s&WEBLAYOUT=%s&DWF=%s&LOCALE=%s", path, sessionId, URLEncoder.encode(webLayout, "UTF-8"), dwf, locale);
    }
    else
    {
        url = String.format("%s?SESSION=%s&WEBLAYOUT=%s&DWF=%s&LOCALE=%s", taskPane, sessionId, URLEncoder.encode(webLayout), dwf, locale);
    }
    String templ = LoadTemplate("/viewerfiles/taskframe.templ");
    String[] vals = { GetSurroundVirtualPath(request) + "tasklist.jsp",
                    locale,
                    url };
    response.getWriter().write(Substitute(templ, vals));
%>

<%!
void GetRequestParameters(HttpServletRequest request)
{
    taskPane = GetParameter(request, "TASK");
    sessionId = GetParameter(request, "SESSION");
    webLayout = GetParameter(request, "WEBLAYOUT");
    dwf = GetParameter(request, "DWF");
    locale = GetParameter(request, "LOCALE");
}
%>

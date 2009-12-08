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
String mapName = "";
String sessionId = "";
String templFile = "";
String mapFrame = "";
String locale = "";
%>

<%
response.setContentType("text/html; charset=UTF-8");
request.setCharacterEncoding("UTF-8");
PrintWriter writer = response.getWriter();
try
{
    //fetch the parameters for this request
    //
    GetRequestParameters(request);

    templFile = "/viewerfiles/legendctrl.templ";
    String vpath = GetSurroundVirtualPath(request);
    String vals[] = {
        mapFrame,
        vpath + "legend.jsp",
        vpath + "legend.jsp",
        URLEncoder.encode(mapName, "UTF-8"),
        sessionId,
        vpath + "legendui.jsp",
        locale };

    String templ = LoadTemplate(templFile);
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
    mapName = GetParameter(request, "MAPNAME");
    sessionId = GetParameter(request, "SESSION");
    if(IsParameter(request, "MAPFRAME"))
        mapFrame = GetParameter(request, "MAPFRAME");
    else
        mapFrame = "parent";
    locale = GetParameter(request, "LOCALE");
}
%>

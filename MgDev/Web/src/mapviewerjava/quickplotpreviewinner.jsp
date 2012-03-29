<%--
  -Copyright (C) 2004-2011 by Autodesk, Inc.
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
<%@ page import="java.util.*" %>
<%@ page import="java.util.regex.*" %>

<%@ include file="common.jsp" %>
<%@ page isThreadSafe="false" %>

<%!
String mapName;
String sessionId;
String locale;
int targetType;
int us;

Hashtable<String, String> annotations = new Hashtable<String, String>();
%>

<%
    response.setContentType("text/html; charset=UTF-8");
    request.setCharacterEncoding("UTF-8");
    mapName = sessionId = locale = "";
    targetType = us = 0;

    MgLocalizer.SetLocalizedFilesPath(getServletContext().getRealPath("/") + "localized/");
    GetRequestParameters(request);

    String jsPath = "../viewerfiles/";
    String templ = MgLocalizer.Localize(LoadTemplate("/viewerfiles/quickplotpreviewinner.templ"), locale, GetClientOS(request));

    for (Enumeration<String> keys = annotations.keys();
         keys.hasMoreElements();)
    {
        String key = keys.nextElement();
        templ = templ.replaceAll(Pattern.quote(key), annotations.get(key));
    }
    // Set the date annotation according to its format mask
    Pattern pattern = Pattern.compile("\\{date:.*\\[java=(.+?)\\].*?\\}");
    Matcher matcher = pattern.matcher(templ);
    // Set a default format string
    String formatString = "%1$tm/%1$te/%1$tY";
    if (matcher.matches())
    {
        formatString = matcher.group(1);
    }

    templ = templ.replaceAll(pattern.toString(), String.format(formatString, Calendar.getInstance()));
    String[] vals = {jsPath};

    response.getWriter().write(Substitute(templ, vals));
%>

<%!
void GetRequestParameters(HttpServletRequest request)
{
    sessionId = ValidateSessionId(GetParameter(request, "SESSION"));
    locale = ValidateLocaleString(GetParameter(request, "LOCALE"));
    mapName = ValidateMapName(GetParameter(request, "MAPNAME"));
    targetType = GetIntParameter(request, "TARGETTYPE");
    us = GetIntParameter(request, "US");

    // The parameters whose name matches this pattern will be treated as annotation
    String pattern = "^\\{field:.+\\}$";
    for (Enumeration<String> names = request.getParameterNames();
         names.hasMoreElements();)
     {
        String name = names.nextElement();

        if (name.matches(pattern))
        {
            annotations.put(name, EscapeForHtml(request.getParameter(name)));
        }
     }

    annotations.put("{scale}", "1 : " + GetParameter(request, "scale_denominator"));
}
%>

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
<%@ page isThreadSafe="false" %>

<%!
String sessionId = "";
String webLayoutId = "";
int dwf = 0;
String locale = "";
%>

<%
    response.setContentType("text/html; charset=UTF-8");
    request.setCharacterEncoding("UTF-8");
    GetRequestParameters(request);

    try
    {
        InitializeWebTier();

        MgUserInformation cred = new MgUserInformation(sessionId);
        cred.SetClientIp(GetClientIp(request));
        cred.SetClientAgent(GetClientAgent());

        //connect to the site and get a feature service and a resource service instances
        MgSiteConnection site = new MgSiteConnection();
        site.Open(cred);
        
        //Get the MgWebLayout object
        MgResourceService resourceSrvc = (MgResourceService)site.CreateService(MgServiceType.ResourceService);
        MgResourceIdentifier webLayoutResId = new MgResourceIdentifier(webLayoutId);
        MgWebLayout webLayout = new MgWebLayout(resourceSrvc, webLayoutResId);
        MgWebTaskPane taskPane = webLayout.GetTaskPane();
        String taskPaneUrl = taskPane.GetInitialTaskUrl();
        String vpath = GetSurroundVirtualPath(request);
        if (taskPaneUrl == null || taskPaneUrl.length() == 0)
        {
            taskPaneUrl = "gettingstarted.jsp";
        }

        String url = URLDecoder.decode(taskPaneUrl, "UTF-8");
        int index = url.indexOf("?");

        if(index > 0)
        {
            String path = url.substring(0, index);
            String query = url.substring(index+1);

            if(query.length() > 0)
                url = String.format("%s?SESSION=%s&WEBLAYOUT=%s&DWF=%s&LOCALE=%s&%s", path, sessionId, URLEncoder.encode(webLayoutId, "UTF-8"), dwf, locale, query);
            else
                url = String.format("%s?SESSION=%s&WEBLAYOUT=%s&DWF=%s&LOCALE=%s", path, sessionId, URLEncoder.encode(webLayoutId, "UTF-8"), dwf, locale);
        }
        else
        {
            url = String.format("%s?SESSION=%s&WEBLAYOUT=%s&DWF=%s&LOCALE=%s", taskPaneUrl, sessionId, URLEncoder.encode(webLayoutId), dwf, locale);
        }
        String templ = LoadTemplate("/viewerfiles/taskframe.templ");
        String[] vals = { vpath + "tasklist.jsp",
                    locale,
                    url };
        response.getWriter().write(Substitute(templ, vals));
    }
    catch (MgException exc)
    {
        OnError(MgLocalizer.GetString("TASKS", locale), exc.GetExceptionMessage(), response.getWriter(), request);
        return;
    }
    catch (Exception ne)
    {
        OnError(MgLocalizer.GetString("TASKS", locale), ne.getMessage(), response.getWriter(), request);
        return;
    }

%>

<%!
void GetRequestParameters(HttpServletRequest request)
{
    sessionId = ValidateSessionId(GetParameter(request, "SESSION"));
    locale = ValidateLocaleString(GetParameter(request, "LOCALE"));
    webLayoutId = ValidateResourceId(GetParameter(request, "WEBLAYOUT"));
    dwf = GetIntParameter(request, "DWF");
}

void OnError(String title, String msg, PrintWriter outStream, HttpServletRequest request) throws FileNotFoundException, IOException
{
    String templ = MgLocalizer.Localize(LoadTemplate("/viewerfiles/errorpage.templ"), locale, GetClientOS(request));
    String[] vals = {"0", title, msg };
    outStream.write(Substitute(templ, vals));
}
%>

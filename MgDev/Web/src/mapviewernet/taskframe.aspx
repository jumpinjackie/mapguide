<%--
Copyright (C) 2004-2009 by Autodesk, Inc.
This library is free software; you can redistribute it and/or
modify it under the terms of version 2.1 of the GNU Lesser
General Public License as published by the Free Software Foundation.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
--%>

<%@ Page language="c#" %>
<%@ Import Namespace="System" %>
<%@ Import Namespace="System.IO" %>
<%@ Import Namespace="System.Collections.Specialized" %>
<%@ Import Namespace="System.Text" %>
<%@ Import Namespace="System.Web" %>

<!-- #Include File="common.aspx -->

<script runat="server">
String sessionId = "";
String webLayoutId = "";
int dwf = 0;
String locale = "";
</script>

<%
    Response.Charset = "utf-8";
    MgLocalizer.SetLocalizedFilesPath(Request.ServerVariables["APPL_PHYSICAL_PATH"] + "..\\localized\\");

    GetRequestParameters();

    try
    {
        InitializeWebTier();

        MgUserInformation cred = new MgUserInformation(sessionId);
        cred.SetClientIp(GetClientIp(Request));
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
        String vpath = GetSurroundVirtualPath(Request);
        if (taskPaneUrl == null || taskPaneUrl.Length == 0)
        {
            taskPaneUrl = "gettingstarted.aspx";
        }

        String url = HttpUtility.UrlDecode(taskPaneUrl);
        int index = url.IndexOf("?");

        if(index > 0)
        {
            String path = url.Substring(0, index);
            String query = url.Substring(index+1);

            if(query.Length > 0)
                url = String.Format("{0}?SESSION={1}&WEBLAYOUT={2}&DWF={3}&LOCALE={4}&{5}", path, sessionId, HttpUtility.UrlEncode(webLayoutId), dwf, locale, query);
            else
                url = String.Format("{0}?SESSION={1}&WEBLAYOUT={2}&DWF={3}&LOCALE={4}", path, sessionId, HttpUtility.UrlEncode(webLayoutId), dwf, locale);

        }
        else
        {
            url = String.Format("{0}?SESSION={1}&WEBLAYOUT={2}&DWF={3}&LOCALE={4}", taskPaneUrl, sessionId, HttpUtility.UrlEncode(webLayoutId), dwf, locale);
        }
        String templ = LoadTemplate(Request, "../viewerfiles/taskframe.templ");
        String[] vals = {
                    vpath + "tasklist.aspx",
                    locale,
                    url
                    };

        Response.Write(Substitute(templ, vals));
    }
    catch (MgException exc)
    {
        OnError(MgLocalizer.GetString("TASKS", locale), exc.GetExceptionMessage());
        return;
    }
    catch (Exception ne)
    {
        OnError(MgLocalizer.GetString("TASKS", locale), ne.Message);
        return;
    }
%>

<script runat="server">
void GetRequestParameters()
{
    if(Request.HttpMethod == "POST")
        GetParameters(Request.Form);
    else
        GetParameters(Request.QueryString);
}

void GetParameters(NameValueCollection parameters)
{
    sessionId = ValidateSessionId(GetParameter(parameters, "SESSION"));
    locale = ValidateLocaleString(GetParameter(parameters, "LOCALE"));
    webLayoutId = ValidateResourceId(GetParameter(parameters, "WEBLAYOUT"));
    dwf = GetIntParameter(parameters, "DWF");
}

void OnError(String title, String msg)
{
    String templ = MgLocalizer.Localize(LoadTemplate(Request, "../viewerfiles/errorpage.templ"), locale, GetClientOS(Request));
    String[] vals = { "0", title, msg };
    Response.Write(Substitute(templ, vals));
}

</script>

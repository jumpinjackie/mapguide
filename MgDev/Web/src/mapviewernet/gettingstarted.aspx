<%--
Copyright (C) 2004-2010 by Autodesk, Inc.
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
<%@ Import Namespace="OSGeo.MapGuide" %>

<!-- #Include File="common.aspx -->

<script language="c#" runat="server">
String cmdListPage = "WS1a9193826455f5ff9110c71085341391d-2e28.htm";

String sessionId = "";
String webLayout = "";
String pageName = "";
String locale = "";
bool dwf = false;
</script>

<%

    Response.Charset = "utf-8";
    Response.ContentType = "text/html";

    GetRequestParameters();

    try {
        if(pageName == "") {
            //no page name specified, assume main getting started page (the command list)
            pageName = cmdListPage;
        }
        //load the original page
        String orgHtml = "";
        try {
            orgHtml = LoadTemplate(Request, "../localized/help/" + locale + "/" + pageName);
        } catch(Exception e) {
            orgHtml = LoadTemplate(Request, "../localized/help/en/" + pageName);
            locale = GetDefaultLocale();
        }
        String fixedupHtml = FixupPageReferences(orgHtml, webLayout, dwf, GetRootVirtualFolder(Request) + "/");
        if(pageName == cmdListPage) {
            //filter out unused commands
            //
            InitializeWebTier();
            MgUserInformation cred = new MgUserInformation(sessionId);
            cred.SetClientIp(GetClientIp(Request));
            cred.SetClientAgent(GetClientAgent());

            MgSiteConnection site = new MgSiteConnection();
            site.Open(cred);
            MgResourceService resourceSrvc = site.CreateService(MgServiceType.ResourceService) as MgResourceService;
            MgResourceIdentifier wli = new MgResourceIdentifier(webLayout);
            byte[] hb = Encoding.UTF8.GetBytes(fixedupHtml);
            MgByteSource src = new MgByteSource(hb, hb.Length);
            MgWebLayout wl = new MgWebLayout(resourceSrvc, wli);
            MgByteReader pagestream = wl.ProcessGettingStartedPage(src.GetReader(), dwf);
            if(pagestream == null)
                Response.Write(fixedupHtml);
            else
                Response.Write(pagestream.ToString());
        }
        else
            Response.Write(fixedupHtml);
    }
    catch(MgException mge) {
        String errorMsg = EscapeForHtml(mge.GetDetails());
        Response.Write(errorMsg);
    }
    catch(Exception e) {
    }

%>

<script language="c#" runat="server">

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
    dwf = GetIntParameter(parameters, "DWF") == 1;
    webLayout = ValidateResourceId(GetParameter(parameters, "WEBLAYOUT"));
    pageName = GetParameter(parameters, "PAGE");
}

String FixupPageReferences(String html, String webLayout, bool dwf, String vpath) {
    String htmlPrefix = new StringBuilder().Append("gettingstarted.aspx?WEBLAYOUT=").Append(HttpUtility.UrlEncode(webLayout)).Append("&DWF=").Append(dwf?"1":"0").Append("&LOCALE=").Append(locale).Append("&PAGE=").ToString();
    String imgSrcPrefix = new StringBuilder().Append(vpath).Append("localized/help/").Append(locale).Append("/").ToString();
    StringBuilder res = new StringBuilder();
    int index = 0;
    bool found;
    do
    {
        found = false;
        int i = html.IndexOf("href=\"", index);
        int j = html.IndexOf("src=\"", index);
        if(i != -1 || j != -1) {
            found = true;
            bool htmlRef = false;
            if(i != -1) {
                if(j != -1) {
                    if(i < j) {
                        htmlRef = html.Substring(i - 3, 2) == "<a";
                        i += 6;
                    }
                    else
                        i = j + 5;
                }
                else {
                    htmlRef = html.Substring(i - 3, 2) == "<a";
                    i += 6;
                }
            }
            else
                i = j + 5;
            res.Append(html.Substring(index, i - index));
            if(htmlRef) {
                if(FixupRequired(html, i))
                    res.Append(htmlPrefix);
            }
            else {
                if(FixupRequired(html, i))
                    res.Append(imgSrcPrefix);
            }
            index = i;
        }
    } while(found);
    res.Append(html.Substring(index));
    return res.ToString();
}

bool FixupRequired(String html, int refIndex) {
    return html.Substring(refIndex, 7) != "http://" &&
           html.Substring(refIndex, 11) != "javascript:";
}


</script>

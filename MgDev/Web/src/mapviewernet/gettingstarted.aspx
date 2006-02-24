<%@ Page language="c#"  %>
<%@ Import Namespace="System" %>
<%@ Import Namespace="System.IO" %>
<%@ Import Namespace="System.Collections.Specialized" %>
<%@ Import Namespace="System.Text" %>
<%@ Import Namespace="OSGeo.MapGuide" %>

<!-- #Include File="common.aspx -->

<script language="c#" runat="server">
String cmdListPage = "WS1a9193826455f5ff9110c71085341391d-2e28.html";

String webLayout = "";
String pageName = "";
String locale = "";
bool dwf = false;
</script>

<%

    Response.Charset = "utf-8";
    Response.ContentType = "text/html";
    MapGuideApi.InitializeSockets();

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
            locale = "en";
        }
        String fixedupHtml = FixupPageReferences(orgHtml, webLayout, dwf, GetRootVirtualFolder(Request) + "/");
        if(pageName == cmdListPage) {
            //filter out unused commands
            //
            InitializeWebTier();
            MgUserInformation cred = new MgUserInformation("Administrator", "admin");
            MgSiteConnection site = new MgSiteConnection();
            site.Open(cred);
            MgResourceIdentifier wli = new MgResourceIdentifier(webLayout);
            byte[] hb = Encoding.UTF8.GetBytes(fixedupHtml);
            MgByteSource src = new MgByteSource(hb, hb.Length);
            MgWebLayout wl = new MgWebLayout(wli);
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
        Response.Write(mge.GetDetails());
    }
    catch(Exception e) {
    }
    finally {
        MapGuideApi.TerminateSockets();
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
    webLayout = GetParameter(parameters, "WEBLAYOUT");
    pageName = GetParameter(parameters, "PAGE");
    dwf = GetParameter(parameters, "DWF") == "1";
    locale = GetParameter(parameters, "LOCALE");
    if(locale == "")
        locale = "en";
}

String FixupPageReferences(String html, String webLayout, bool dwf, String vpath) {
    String htmlPrefix = new StringBuilder().Append("gettingstarted.aspx?WEBLAYOUT=").Append(webLayout).Append("&DWF=").Append(dwf?"1":"0").Append("&PAGE=").ToString();
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

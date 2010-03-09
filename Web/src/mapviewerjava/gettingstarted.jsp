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
<%@ include file="common.jsp" %>
<%@ page isThreadSafe="false" %>

<%!
String cmdListPage = "WS1a9193826455f5ff9110c71085341391d-2e28.htm";

String sessionId;
String webLayout;
String pageName;
String locale;
boolean dwf;
%>

<%
    sessionId = "";
    webLayout = "";
    pageName = "";
    locale = "";
    dwf = false;

    response.setContentType("text/html; charset=UTF-8");
    request.setCharacterEncoding("UTF-8");
    PrintWriter outStream = response.getWriter();

    GetRequestParameters(request);

    try {
        if(pageName.equals("")) {
            //no page name specified, assume main getting started page (the command list)
            pageName = cmdListPage;
        }
        //load the original page
        String orgHtml = "";
        try {
            orgHtml = LoadTemplate("/localized/help/" + locale + "/" + pageName);
        } catch(Exception e) {
            orgHtml = LoadTemplate("/localized/help/en/" + pageName);
            locale = GetDefaultLocale();
        }
        String fixedupHtml = FixupPageReferences(orgHtml, webLayout, dwf, request.getContextPath() + "/");
        if(pageName.equals(cmdListPage)) {
            //filter out unused commands
            //
            InitializeWebTier();
            MgUserInformation cred = new MgUserInformation(sessionId);
            cred.SetClientIp(GetClientIp(request));
            cred.SetClientAgent(GetClientAgent());

            MgSiteConnection site = new MgSiteConnection();
            site.Open(cred);
            MgResourceService resourceSrvc = (MgResourceService)site.CreateService(MgServiceType.ResourceService);
            MgResourceIdentifier wli = new MgResourceIdentifier(webLayout);
            byte[] hb = fixedupHtml.getBytes("UTF-8");
            MgByteSource src = new MgByteSource(hb, hb.length);
            MgWebLayout wl = new MgWebLayout(resourceSrvc, wli);
            MgByteReader pagestream = wl.ProcessGettingStartedPage(src.GetReader(), dwf);
            if(pagestream == null)
                outStream.write(fixedupHtml);
            else
                outStream.write(pagestream.ToString());
        }
        else
            outStream.write(fixedupHtml);
    }
    catch(MgException mge) {
        String errorMsg = EscapeForHtml(mge.GetDetails());
        outStream.write(errorMsg);
    }
    catch(Exception e) {
        String errorMsg = EscapeForHtml(e.getMessage());
        outStream.write(errorMsg);
    }

%>

<%!
void GetRequestParameters(HttpServletRequest request)
{
    sessionId = GetParameter(request, "SESSION");
    webLayout = GetParameter(request, "WEBLAYOUT");
    pageName = GetParameter(request, "PAGE");
    dwf = GetParameter(request, "DWF").equals("1");
    locale = GetParameter(request, "LOCALE");
    if(locale.length() == 0)
        locale = GetDefaultLocale();
}

String FixupPageReferences(String html, String webLayout, boolean dwf, String vpath) throws UnsupportedEncodingException {
    String htmlPrefix = new StringBuffer().append("gettingstarted.jsp?WEBLAYOUT=").append(URLEncoder.encode(webLayout, "UTF-8")).append("&DWF=").append(dwf?"1":"0").append("&LOCALE=").append(locale).append("&PAGE=").toString();
    String imgSrcPrefix = new StringBuffer().append(vpath).append("localized/help/").append(locale).append("/").toString();
    StringBuffer res = new StringBuffer();
    int index = 0;
    boolean found;
    do
    {
        found = false;
        int i = html.indexOf("href=\"", index);
        int j = html.indexOf("src=\"", index);
        if(i != -1 || j != -1) {
            found = true;
            boolean htmlRef = false;
            if(i != -1) {
                if(j != -1) {
                    if(i < j) {
                        htmlRef = html.substring(i - 3, i - 1).equals("<a");
                        i += 6;
                    }
                    else
                        i = j + 5;
                }
                else {
                    htmlRef = html.substring(i - 3, i - 1).equals("<a");
                    i += 6;
                }
            }
            else
                i = j + 5;
            res.append(html.substring(index, i));
            if(htmlRef) {
                if(FixupRequired(html, i))
                    res.append(htmlPrefix);
            }
            else {
                if(FixupRequired(html, i))
                    res.append(imgSrcPrefix);
            }
            index = i;
        }
    } while(found);
    res.append(html.substring(index));
    return res.toString();
}

boolean FixupRequired(String html, int refIndex) {
    return !html.substring(refIndex, refIndex + 7).equals("http://") &&
           !html.substring(refIndex, refIndex + 11).equals("javascript:");
}

%>

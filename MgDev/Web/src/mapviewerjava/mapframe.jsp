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
<%@ page import="java.util.*" %>
<%@ page import="java.io.*" %>
<%@ page import="java.text.*" %>
<%@ page import="javax.servlet.jsp.*" %>
<%@ page import="javax.servlet.http.*" %>
<%@ include file="common.jsp" %>
<%@ page isThreadSafe="false" %>

<%!
String mapDefinition;
String locale;
int infoWidth;
int showLegend;
int showProperties;
int showSlider;
String sessionId;
String type;
String hlTgt;
String hlTgtName;
%>

<%
    response.setContentType("text/html; charset=UTF-8");
    request.setCharacterEncoding("UTF-8");
    infoWidth = showLegend = showProperties = 0;
    hlTgt = hlTgtName = locale = mapDefinition = sessionId = type = "";

    MgLocalizer.SetLocalizedFilesPath(getServletContext().getRealPath("/") + "localized/");
    GetRequestParameters(request);

    if (type.equals("DWF"))
    {
        String frameName = "";
        if(hlTgt.equals("1"))
        {
            frameName = "taskPaneFrame";
        }
        else if(hlTgt.equals("3"))
        {
            frameName = hlTgtName;
        }
        else
        {
            frameName = "_BLANK";
        }
        String mapRequest = GetRootVirtualFolder(request) + "/mapagent/mapagent.fcgi?OPERATION=GETMAP&VERSION=1.0&MAPDEFINITION=" + URLEncoder.encode(mapDefinition, "UTF-8") + "&DWFVERSION=6.01&EMAPVERSION=1.0&LOCALE=" + locale + (sessionId != ""? "&SESSION=" + sessionId: "") + "&reload=true";
        String vals[] = { mapRequest,
                      String.valueOf(infoWidth),
                      showLegend != 0 || showProperties != 0? "true": "false",
                      showLegend != 0 ? "true": "false",
                      showProperties != 0 ? "true": "false",
                      frameName
                      };
        //load html template code and format it
        //
        String templ = MgLocalizer.Localize(LoadTemplate("/viewerfiles/dwfmappane.templ"), locale, GetClientOS(request));
        response.getWriter().write(Substitute(templ, vals));
    }
    else
    {
        InitializeWebTier();

        // create the map instance and store it with the session
        //
        MgUserInformation userInfo = new MgUserInformation();
        userInfo.SetMgSessionId(sessionId);
        userInfo.SetClientIp(GetClientIp(request));
        userInfo.SetClientAgent(GetClientAgent());

        MgSiteConnection site = new MgSiteConnection();
        site.Open(userInfo);

        MgTileService tileSrvc = (MgTileService)site.CreateService(MgServiceType.TileService);
        int tileSizeX = tileSrvc.GetDefaultTileSizeX();
        int tileSizeY = tileSrvc.GetDefaultTileSizeY();

        MgResourceService resourceSrvc = (MgResourceService)site.CreateService(MgServiceType.ResourceService);

        MgMap map = new MgMap();
        MgResourceIdentifier resId = new MgResourceIdentifier(mapDefinition);
        String mapName = resId.GetName();
        map.Create(resourceSrvc, resId, mapName);

        //create an empty selection object and store it in the session repository
        MgSelection sel = new MgSelection(map);
        sel.Save(resourceSrvc, mapName);

        //get the map extent and calculate the scale factor
        //
        MgEnvelope mapExtent = map.GetMapExtent();
        String srs = map.GetMapSRS();
        double metersPerUnit;
        String unitsType;
        if(srs != null && srs.length() > 0)
        {
            MgCoordinateSystemFactory csFactory = new MgCoordinateSystemFactory();
            MgCoordinateSystem cs = csFactory.Create(srs);
            metersPerUnit = cs.ConvertCoordinateSystemUnitsToMeters(1.0);
            unitsType = cs.GetUnits();
        }
        else
        {
            metersPerUnit = 1.0;
            unitsType = MgLocalizer.GetString("DISTANCEMETERS", locale);
        }

        MgCoordinate llExtent = mapExtent.GetLowerLeftCoordinate();
        MgCoordinate urExtent = mapExtent.GetUpperRightCoordinate();
        String bgColor = map.GetBackgroundColor();
        if(bgColor.length() == 8)
        {
            bgColor = "#" + bgColor.substring(2);
        }
        else
        {
            bgColor = "white";
        }

        String scaleCreationCode = "";

        // Create a sorted set of display scales
        TreeSet scales = new TreeSet();
        for(int i = 0; i < map.GetFiniteDisplayScaleCount(); i++)
        {
            scales.add(new Double(map.GetFiniteDisplayScaleAt(i)));
        }
        Iterator iter = scales.iterator();
        int i = 0;
        while(iter.hasNext())
        {
            scaleCreationCode = scaleCreationCode + "scales[" + i + "]=" + iter.next() + "; ";
            i++;
        }
        MgResourceIdentifier mapStateId = new MgResourceIdentifier("Session:" + sessionId + "//" + mapName + "." + MgResourceType.Map);
        map.Save(resourceSrvc, mapStateId);

        //load html template code and format it
        //
        String templ = MgLocalizer.Localize(LoadTemplate("/viewerfiles/ajaxmappane.templ"), locale, GetClientOS(request));
        String vpath = GetSurroundVirtualPath(request);
        String vals[] = {
                    String.valueOf(tileSizeX),
                    String.valueOf(tileSizeY),
                    GetRootVirtualFolder(request) + "/mapagent/mapagent.fcgi",
                    mapName,
                    mapDefinition,
                    String.valueOf(infoWidth),
                    showLegend != 0 ? "true": "false",
                    showProperties != 0 ? "true": "false",
                    sessionId,
                    String.valueOf(llExtent.GetX()), String.valueOf(llExtent.GetY()),
                    String.valueOf(urExtent.GetX()), String.valueOf(urExtent.GetY()),
                    String.valueOf(metersPerUnit),
                    unitsType,
                    bgColor,
                    hlTgt,
                    hlTgtName,
                    vpath + "setselection.jsp",
                    showSlider != 0? "true": "false",
                    locale,
                    vpath + "getselectedfeatures.jsp",
                    scaleCreationCode,
                    vpath + "ajaxviewerabout.jsp",
                    vpath + "legendctrl.jsp",
                    URLEncoder.encode(mapName, "UTF-8"),
                    sessionId,
                    locale,
                    vpath + "propertyctrl.jsp",
                    locale };
        response.getWriter().write(Substitute(templ, vals));
    }

%>

<%!
void GetRequestParameters(HttpServletRequest request)
{
    type = GetParameter(request, "TYPE");

    String localeParam = GetParameter(request, "LOCALE");
    if(localeParam != null && localeParam.length() > 0)
    {
        locale = localeParam;
    }
    else
    {
        locale = GetDefaultLocale();
    }

    hlTgt = GetParameter(request, "HLTGT");
    hlTgtName = GetParameter(request, "HLTGTNAME");

    if(IsParameter(request, "INFOWIDTH"))
        infoWidth = Integer.parseInt(GetParameter(request, "INFOWIDTH"));

    if(IsParameter(request, "SHOWLEGEND"))
        showLegend = Integer.parseInt(GetParameter(request, "SHOWLEGEND"));

    if(IsParameter(request, "SHOWPROP"))
        showProperties = Integer.parseInt(GetParameter(request, "SHOWPROP"));

    if(IsParameter(request, "MAPDEFINITION"))
        mapDefinition = GetParameter(request, "MAPDEFINITION");

    if(IsParameter(request, "SESSION"))
        sessionId = GetParameter(request, "SESSION");

    if(IsParameter(request, "SHOWSLIDER"))
        showSlider = Integer.parseInt(GetParameter(request, "SHOWSLIDER"));
}
%>

<%--
Copyright (C) 2004-2011 by Autodesk, Inc.
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
<%@ Import Namespace="System.Web" %>
<%@ Import Namespace="System.Globalization" %>
<%@ Import Namespace="OSGeo.MapGuide" %>

<!-- #Include File="common.aspx -->

<script runat="server">
String mapDefinition;
String locale;
int infoWidth;
int showLegend;
int showProperties;
int showSlider;
String sessionId;
String hlTgt;
String hlTgtName;
String selectionColor;
String mapImgFormat;
String selImgFormat;
int pointBufferSize;
</script>

<%
    Response.Charset = "utf-8";

    MgLocalizer.SetLocalizedFilesPath(Request.ServerVariables["APPL_PHYSICAL_PATH"] + "..\\localized\\");

    infoWidth = showLegend = showProperties = 0;
    hlTgt = hlTgtName = locale = mapDefinition = sessionId = "";

    GetRequestParameters();
    String mapRequest = "";

    try
    {
        InitializeWebTier();

        // create the map instance and store it with the session
        //
        MgUserInformation userInfo = new MgUserInformation(sessionId);
        userInfo.SetClientIp(GetClientIp(Request));
        userInfo.SetClientAgent(GetClientAgent());

        MgSiteConnection site = new MgSiteConnection();
        site.Open(userInfo);

        MgResourceService resourceSrvc = (MgResourceService)site.CreateService(MgServiceType.ResourceService);

        MgMap map = new MgMap(site);
        MgResourceIdentifier resId = new MgResourceIdentifier(mapDefinition);
        String mapName = resId.GetName();
        map.Create(resId, mapName);

        MgResourceIdentifier tileSetId = map.GetTileSetDefinition();

        MgTileService tileSrvc = (MgTileService)site.CreateService(MgServiceType.TileService);
        int tileSizeX = tileSrvc.GetDefaultTileSizeX();
        int tileSizeY = tileSrvc.GetDefaultTileSizeY();
        if (null != tileSetId)
        {
            //Overwrite the map definition with tile set id (this is for GETTILE requests) and
            //use size settings from that tile set
            mapDefinition = tileSetId.ToString();
            tileSizeX = tileSrvc.GetDefaultTileSizeX(tileSetId);
            tileSizeY = tileSrvc.GetDefaultTileSizeY(tileSetId);
        }
        
        //create an empty selection object and store it in the session repository
        MgSelection sel = new MgSelection(map);
        sel.Save(resourceSrvc, mapName);

        //get the map extent and calculate the scale factor
        //
        MgEnvelope mapExtent = map.GetMapExtent();
        String srs = map.GetMapSRS();
        double metersPerUnit;
        String unitsType;
        if(srs != null && srs.Length > 0)
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
        if(bgColor.Length == 8)
        {
            bgColor = "#" + bgColor.Substring(2);
        }
        else
        {
            bgColor = "white";
        }

        String scaleCreationCode = "";
        ArrayList scales = new ArrayList();
        for(int i = 0; i < map.GetFiniteDisplayScaleCount(); i++)
        {
            scales.Add(map.GetFiniteDisplayScaleAt(i));
        }
        scales.Sort();
        for(int i = 0; i < scales.Count; i++)
        {
            scaleCreationCode = scaleCreationCode + "scales[" + i + "]=" + scales[i].ToString().Replace(',','.') + "; ";
        }
        MgResourceIdentifier mapStateId = new MgResourceIdentifier("Session:" + sessionId + "//" + mapName + "." + MgResourceType.Map);
        map.Save(resourceSrvc, mapStateId);

        //load html template code and format it
        //
        String templ = MgLocalizer.Localize(LoadTemplate(Request, "../viewerfiles/ajaxmappane.templ"), locale, GetClientOS(Request));
        String vpath = GetSurroundVirtualPath(Request);
        String[] vals = {
                    IntToString(tileSizeX),
                    IntToString(tileSizeY),
                    GetRootVirtualFolder(Request) + "/mapagent/mapagent.fcgi",
                    mapName,
                    mapDefinition,
                    IntToString(infoWidth),
                    showLegend != 0 ? "true": "false",
                    showProperties != 0 ? "true": "false",
                    sessionId,
                    DoubleToString(llExtent.GetX()), DoubleToString(llExtent.GetY()),
                    DoubleToString(urExtent.GetX()), DoubleToString(urExtent.GetY()),
                    DoubleToString(metersPerUnit),
                    unitsType,
                    bgColor,
                    hlTgt,
                    hlTgtName,
                    showSlider != 0? "true": "false",
                    locale,
                    scaleCreationCode,
                    selectionColor,
                    mapImgFormat,
                    selImgFormat,
                    pointBufferSize.ToString(NumberFormatInfo.InvariantInfo),
                    vpath + "ajaxviewerabout.aspx",
                    vpath + "legendctrl.aspx",
                    HttpUtility.UrlEncode(mapName),
                    sessionId,
                    locale,
                    vpath + "propertyctrl.aspx",
                    locale };
        Response.Write(Substitute(templ, vals));
    }
    catch(MgException mge)
    {
        String errorMsg = EscapeForHtml(mge.GetDetails());
        Response.Write(errorMsg);
    }

%>

<script runat="server">

void GetRequestParameters()
{
    if ("POST"== Request.HttpMethod)
    {
        GetParameters(Request.Form);
    }
    else
    {
        GetParameters(Request.QueryString);
    }
}

void GetParameters(NameValueCollection parameters)
{
    sessionId = ValidateSessionId(GetParameter(parameters, "SESSION"));
    locale = ValidateLocaleString(GetParameter(parameters, "LOCALE"));
    hlTgt = ValidateHyperlinkTargetValue(GetParameter(parameters, "HLTGT"));
    hlTgtName = ValidateFrameName(GetParameter(parameters, "HLTGTNAME"));

    infoWidth = GetIntParameter(parameters, "INFOWIDTH");
    showLegend = GetIntParameter(parameters, "SHOWLEGEND");
    showProperties = GetIntParameter(parameters, "SHOWPROP");
    showSlider = GetIntParameter(parameters, "SHOWSLIDER");

    selectionColor = ValidateColorString(GetParameter(parameters, "SELCOLOR"), 8);
    mapImgFormat = GetParameter(parameters, "MAPIMGFORMAT");
    selImgFormat = GetParameter(parameters, "SELIMGFORMAT");
    pointBufferSize = GetIntParameter(parameters, "POINTBUFFERSIZE");

    mapDefinition = ValidateResourceId(GetParameter(parameters, "MAPDEFINITION"));
}

String IntToString(int number)
{
    return number.ToString(NumberFormatInfo.InvariantInfo);
}

String DoubleToString(double number)
{
    return number.ToString(NumberFormatInfo.InvariantInfo);
}

</script>

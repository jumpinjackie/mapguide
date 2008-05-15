<%--
Copyright (C) 2004-2008 by Autodesk, Inc.
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
int  infoWidth;
int showLegend;
int showProperties;
int showSlider;
String sessionId;
String type;
String hlTgt;
String hlTgtName;
</script>

<%
    Response.Charset = "utf-8";

    MgLocalizer.SetLocalizedFilesPath(Request.ServerVariables["APPL_PHYSICAL_PATH"] + "..\\localized\\");

    infoWidth = showLegend = showProperties = 0;
    hlTgt = hlTgtName = locale = mapDefinition = sessionId = type = "";

    GetRequestParameters();
    String mapRequest = "";

    if ("DWF" == type)
    {
        String frameName = "";
        if(hlTgt.Equals("1"))
        {
            frameName = "taskPaneFrame";
        }
        else if(hlTgt.Equals("3"))
        {
            frameName = hlTgtName;
        }
        else
        {
            frameName = "_BLANK";
        }
        mapRequest = GetRootVirtualFolder(Request) + "/mapagent/mapagent.fcgi?OPERATION=GETMAP&VERSION=1.0&MAPDEFINITION=" + HttpUtility.UrlEncode(mapDefinition) + "&DWFVERSION=6.01&EMAPVERSION=1.0&LOCALE=" + locale + (sessionId != "" ? "&SESSION=" + sessionId : "") + "&reload=true";

        //load html template code and format it
        //
        String template = MgLocalizer.Localize(LoadTemplate(Request, "../viewerfiles/dwfmappane.templ"), locale, GetClientOS(Request));
        String[] vals = {
                        mapRequest,
                        infoWidth.ToString(NumberFormatInfo.InvariantInfo),
                        (0!=showLegend) || (0!=showProperties)? "true":"false",
                        0!=showLegend? "true":"false",
                        0!=showProperties? "true":"false",
                        frameName
                        };
        Response.Write(Substitute(template, vals));
    }
    else
    {
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
                scaleCreationCode = scaleCreationCode + "scales[" + i + "]=" + scales[i] + "; ";
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
                        vpath + "setselection.aspx",
                        showSlider != 0? "true": "false",
                        locale,
                        scaleCreationCode,
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
            Response.Write(mge.GetMessage());
            Response.Write("<br>");
            Response.Write(mge.GetDetails());
        }
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
    type = GetParameter(parameters, "TYPE");

    locale = GetParameter(parameters, "LOCALE");
    if(locale == "")
        locale = GetDefaultLocale();

    hlTgt = GetParameter(parameters, "HLTGT");
    hlTgtName = GetParameter(parameters, "HLTGTNAME");

    if (IsParameter(parameters, "INFOWIDTH"))
    {
        infoWidth = Convert.ToInt32(GetParameter(parameters, "INFOWIDTH"));
    }
    if (IsParameter(parameters, "SHOWLEGEND"))
    {
        showLegend = Convert.ToInt32(GetParameter(parameters, "SHOWLEGEND"));
    }
    if (IsParameter(parameters, "SHOWPROP"))
    {
        showProperties = Convert.ToInt32(GetParameter(parameters, "SHOWPROP"));
    }
    if (IsParameter(parameters, "MAPDEFINITION"))
    {
        mapDefinition = GetParameter(parameters, "MAPDEFINITION");
    }
    if (IsParameter(parameters, "SESSION"))
    {
        sessionId = GetParameter(parameters, "SESSION");
    }
    if (IsParameter(parameters, "SHOWSLIDER"))
    {
        showSlider = Convert.ToInt32(GetParameter(parameters, "SHOWSLIDER"));
    }
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

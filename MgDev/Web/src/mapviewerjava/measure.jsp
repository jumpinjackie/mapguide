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
String locale;
String mapName;
String sessionId;
int target;
int popup;
boolean clear;
double x1;
double y1;
double x2;
double y2;
double total;
String srs;
int us;
int segId;
String error;
double distance;
String legendName = "Measure";
String featureName = "Measure";
String dataSource;
%>

<%
    locale = "";
    mapName = "";
    sessionId = "";
    target = 0;
    popup = 0;
    clear = false;
    x1 = 0;
    y1 = 0;
    x2 = 0;
    y2 = 0;
    total = 0;
    srs = "";
    us = 0;
    segId = 1;
    error = "";
    distance = 0;
    dataSource = "";

    response.setContentType("text/html; charset=UTF-8");
    request.setCharacterEncoding("UTF-8");
    distance = 0;
    PrintWriter outStream = response.getWriter();

    MgLocalizer.SetLocalizedFilesPath(getServletContext().getRealPath("/") + "localized/");

    GetRequestParameters(request);

    dataSource = "Session:" + sessionId + "//Measure.FeatureSource";
    String layerDef = "Session:" + sessionId + "//Measure.LayerDefinition";

    try
    {
        InitializeWebTier();

        MgUserInformation cred = new MgUserInformation(sessionId);
        cred.SetClientIp(GetClientIp(request));
        cred.SetClientAgent(GetClientAgent());

        //connect to the site and get a feature service and a resource service instances
        MgSiteConnection site = new MgSiteConnection();
        site.Open(cred);
        MgFeatureService featureSrvc = (MgFeatureService)site.CreateService(MgServiceType.FeatureService);
        MgResourceService resourceSrvc = (MgResourceService)site.CreateService(MgServiceType.ResourceService);

        MgResourceIdentifier dataSourceId = new MgResourceIdentifier(dataSource);
        MgResourceIdentifier layerDefId = new MgResourceIdentifier(layerDef);

        //load the map runtime state and locate the measure layer
        //
        MgMap map = new MgMap();
        map.Open(resourceSrvc, mapName);
        MgLayerCollection layers = map.GetLayers();
        srs = GetMapSrs(map);

        MgLayer layer = FindLayer(layers, layerDef);

        if(clear)
        {
            total = 0;
            if(DataSourceExists(resourceSrvc, dataSourceId))
                ClearDataSource(featureSrvc, dataSourceId, featureName);
        }
        else
        {
            MgCoordinateSystemFactory srsFactory = new MgCoordinateSystemFactory();
            MgCoordinateSystem srsMap = srsFactory.Create(srs);

            int srsType = srsMap.GetType();
            if(srsType == MgCoordinateSystemType.Geographic)
                distance = srsMap.MeasureGreatCircleDistance(x1, y1, x2, y2);
            else
                distance = srsMap.MeasureEuclideanDistance(x1, y1, x2, y2);

            distance = srsMap.ConvertCoordinateSystemUnitsToMeters(distance);

            if(0 == us)
                distance *= 0.001;              //get kilometers
            else
                distance *= 0.000621371192;     //get miles

            total += distance;

            //create the line string geometry representing this segment
            //
            MgGeometryFactory geomFactory = new MgGeometryFactory();
            MgCoordinateCollection coordinates = new MgCoordinateCollection();
            coordinates.Add(geomFactory.CreateCoordinateXY(x1, y1));
            coordinates.Add(geomFactory.CreateCoordinateXY(x2, y2));
            MgLineString geom = geomFactory.CreateLineString(coordinates);

            if(segId == 1)
            {
                //first segment
                //
                if(!DataSourceExists(resourceSrvc, dataSourceId))
                {
                    //create feature source
                    //
                    MgClassDefinition classDef = new MgClassDefinition();

                    classDef.SetName(featureName);
                    classDef.SetDescription(MgLocalizer.GetString("MEASUREFEATURECLASS", locale));
                    classDef.SetDefaultGeometryPropertyName("GEOM");

                    //Set KEY property
                    MgDataPropertyDefinition prop = new MgDataPropertyDefinition("KEY");
                    prop.SetDataType(MgPropertyType.Int32);
                    prop.SetAutoGeneration(true);
                    prop.SetReadOnly(true);
                    classDef.GetIdentityProperties().Add(prop);
                    classDef.GetProperties().Add(prop);

                    //Set PARTIAL property. Hold the distance for this segment
                    prop = new MgDataPropertyDefinition("PARTIAL");
                    prop.SetDataType(MgPropertyType.Double);
                    classDef.GetProperties().Add(prop);

                    //Set TOTAL property. Hold the total distance up to this segment, including it
                    prop = new MgDataPropertyDefinition("TOTAL");
                    prop.SetDataType(MgPropertyType.Double);
                    classDef.GetProperties().Add(prop);

                    //Set geometry property
                    MgGeometricPropertyDefinition geomProp = new MgGeometricPropertyDefinition("GEOM");
                    //geomProp.SetGeometryTypes(MgFeatureGeometricType.mfgtSurface); //TODO use the constant when exposed
                    geomProp.SetGeometryTypes(4);
                    classDef.GetProperties().Add(geomProp);

                    //Create the schema
                    MgFeatureSchema schema = new MgFeatureSchema("MeasureSchema", MgLocalizer.GetString("MEASURESCHEMADESCR", locale));
                    schema.GetClasses().Add(classDef);

                    //finally, creation of the feature source
                    MgCreateSdfParams parameters = new MgCreateSdfParams("MapSrs", srs, schema);
                    featureSrvc.CreateFeatureSource(dataSourceId, parameters);

                    //build map tip
                    String unitText = (us==1)? "Miles": "Kilometers";
                    String tip = "Concat(Concat(Concat('" + MgLocalizer.GetString("MEASUREPARTIAL", locale) + ": ', PARTIAL), Concat(', " + MgLocalizer.GetString("MEASURETOTAL", locale) + ": ', TOTAL)), ' (" + unitText + ")')";

                    //Create the layer definition
                    MgByteReader layerDefContent = BuildLayerDefinitionContent(dataSource, featureName, tip);
                    resourceSrvc.SetResource(layerDefId, layerDefContent, null);

                    //Add the layer to the map, if it's not already in it
                    if(layer == null)
                    {
                        legendName = MgLocalizer.GetString("MEASURELAYER", locale);
                        layer = new MgLayer(layerDefId, resourceSrvc);
                        layer.SetDisplayInLegend(true);
                        layer.SetLegendLabel(legendName);
                        layers.Insert(0, layer);
                    }
                }
                else
                {
                    //data source already exist. clear its content
                    ClearDataSource(featureSrvc, dataSourceId, featureName);
                }
            }
            // create a feature representing this segment and insert it into the data source
            //
            MgPropertyCollection measureProps = new MgPropertyCollection();

            MgDoubleProperty partialProp = new MgDoubleProperty("PARTIAL", distance);
            measureProps.Add(partialProp);

            MgDoubleProperty totalProp = new MgDoubleProperty("TOTAL", total);
            measureProps.Add(totalProp);

            MgAgfReaderWriter agf = new MgAgfReaderWriter();
            MgByteReader geomReader = agf.Write(geom);
            MgGeometryProperty geometryProp = new MgGeometryProperty("GEOM", geomReader);
            measureProps.Add(geometryProp);

            MgInsertFeatures cmd = new MgInsertFeatures(featureName, measureProps);
            MgFeatureCommandCollection commands = new MgFeatureCommandCollection();
            commands.Add(cmd);

            //Insert the distance feature in the temporary data source
            //
            ReleaseReader(featureSrvc.UpdateFeatures(dataSourceId, commands, false));
        }

        if(layer != null)
            layer.ForceRefresh();
        map.Save(resourceSrvc);
    }
    catch(MgException exc)
    {
        OnError(MgLocalizer.GetString("MEASUREERROR", locale), exc.GetDetails(), outStream, request);
        return;
    }
    catch(Exception ne)
    {
        OnError(MgLocalizer.GetString("MEASUREERROR", locale), ne.getMessage(), outStream, request);
        return;
    }

    String templ = LoadTemplate("/viewerfiles/measureui.templ");

    templ = MgLocalizer.Localize(templ, locale, GetClientOS(request));

    String vpath = GetSurroundVirtualPath(request);
    String[] vals = {
            locale,
            String.valueOf(target),
            String.valueOf(popup),
            mapName,
            sessionId,
            String.valueOf(total),
            String.valueOf(distance),
            "1",
            vpath + "measure.jsp",
            vpath + "measure.jsp"
    };
    outStream.write(Substitute(templ, vals));
%>

<%!
void GetRequestParameters(HttpServletRequest request)
{
    mapName = GetParameter(request, "MAPNAME");
    sessionId = GetParameter(request, "SESSION");
    target = GetIntParameter(request, "TGT");
    popup = GetIntParameter(request, "POPUP");
    locale = GetParameter(request, "LOCALE");
    if(IsParameter(request, "CLEAR"))
    {
        clear = true;
    }
    else
    {
        clear = false;
        x1 = GetDoubleParameter(request, "X1");
        y1 = GetDoubleParameter(request, "Y1");
        x2 = GetDoubleParameter(request, "X2");
        y2 = GetDoubleParameter(request, "Y2");
        total = GetDoubleParameter(request, "TOTAL");
        us = GetIntParameter(request, "US");
        segId = GetIntParameter(request, "SEGID");
    }
}

String GetMapSrs(MgMap map) throws MgException
{
    srs = map.GetMapSRS();
    if(!srs.equals(""))
        return srs;

    //SRS is currently optional. Waiting for this to change, set the default SRS to ArbitrayXY meters
    //
    return "LOCALCS[\"Non-Earth (Meter)\",LOCAL_DATUM[\"Local Datum\",0],UNIT[\"Meter\", 1],AXIS[\"X\",EAST],AXIS[\"Y\",NORTH]]";
}

MgLayer FindLayer(MgLayerCollection layers, String layerDef) throws MgException
{
    MgLayer layer = null;
    int i;
    for(i = 0; i < layers.GetCount(); i++)
    {
        MgLayer layer1 = (MgLayer) layers.GetItem(i);
        if(layer1.GetLayerDefinition().ToString().equals(layerDef))
        {
            layer = layer1;
            break;
        }
    }
    return layer;
}

boolean DataSourceExists(MgResourceService resourceSrvc, MgResourceIdentifier resId) throws MgException
{
    return resourceSrvc.ResourceExists(resId);
}

MgByteReader BuildLayerDefinitionContent(String dataSource, String featureName, String tip) throws MgException, Exception
{
    String layerTempl = LoadTemplate("/viewerfiles/linelayerdef.templ");
    String[] vals = {
                dataSource,
                featureName,
                "GEOM",
                tip,
                "1",
                "ff0000" };
    layerTempl = Substitute(layerTempl, vals);
    byte[] bytes = layerTempl.getBytes("UTF-8");

    MgByteSource src = new MgByteSource(bytes, bytes.length);
    return src.GetReader();
}

void ClearDataSource(MgFeatureService featureSrvc, MgResourceIdentifier dataSourceId, String featureName)  throws MgException
{
    MgDeleteFeatures deleteCmd = new MgDeleteFeatures(featureName, "KEY >= 0");
    MgFeatureCommandCollection commands = new MgFeatureCommandCollection();
    commands.Add(deleteCmd);
    featureSrvc.UpdateFeatures(dataSourceId, commands, false);
}

void OnError(String title, String msg, PrintWriter outStream, HttpServletRequest request) throws FileNotFoundException, IOException
{
    String templ = MgLocalizer.Localize(LoadTemplate("/viewerfiles/errorpage.templ"), locale, GetClientOS(request));
    String[] vals = { String.valueOf(popup), title, msg };
    outStream.write(Substitute(templ, vals));
}

void ReleaseReader(MgPropertyCollection res) throws MgException
{
    if(res == null)
        return;
    MgProperty prop = res.GetItem(0);
    if(prop == null)
        return;
    if (prop instanceof MgStringProperty)
        throw new RuntimeException(((MgStringProperty)prop).GetValue());
    MgFeatureProperty resProp = (MgFeatureProperty)prop;
    MgFeatureReader reader = (MgFeatureReader)resProp.GetValue();
    if(reader == null)
        return;
    reader.Close();
}

%>

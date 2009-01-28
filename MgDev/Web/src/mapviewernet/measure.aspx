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
<%@ Import Namespace="OSGeo.MapGuide" %>

<!-- #Include File="common.aspx -->

<script runat="server">
String locale = "";
String mapName = "";
String sessionId = "";
int target = 0;
int popup = 0;
bool clear = false;
double x1 = 0;
double y1 = 0;
double x2 = 0;
double y2 = 0;
double total = 0;
String srs = "";
int us = 0;
int segId = 1;
String error = "";
double distance = 0;
String legendName = "Measure";
String featureName = "Measure";
String dataSource = "";
</script>

<%
    Response.Charset = "utf-8";

    try
    {
        MgLocalizer.SetLocalizedFilesPath(Request.ServerVariables["APPL_PHYSICAL_PATH"] + "..\\localized\\");
        GetRequestParameters();

        dataSource = "Session:" + sessionId + "//Measure.FeatureSource";
        String layerDef = "Session:" + sessionId + "//Measure.LayerDefinition";

        try
        {
            InitializeWebTier();

            MgUserInformation cred = new MgUserInformation(sessionId);
            cred.SetClientIp(GetClientIp(Request));
            cred.SetClientAgent(GetClientAgent());

            //connect to the site and get a feature service and a resource service instances
            MgSiteConnection site = new MgSiteConnection();
            site.Open(cred);
            MgFeatureService featureSrvc = site.CreateService(MgServiceType.FeatureService) as MgFeatureService;
            MgResourceService resourceSrvc = site.CreateService(MgServiceType.ResourceService) as MgResourceService;

            MgResourceIdentifier dataSourceId = new MgResourceIdentifier(dataSource);
            MgResourceIdentifier layerDefId = new MgResourceIdentifier(layerDef);

            //load the map runtime state and locate the measure layer
            //
            MgMap map = new MgMap();
            map.Open(resourceSrvc, mapName);
            MgLayerCollection layers = map.GetLayers();
            srs = GetMapSrs(map);

            MgLayer layer = FindLayer(layers, layerDef);

            if (clear)
            {
                total = 0;
                if (DataSourceExists(resourceSrvc, dataSourceId))
                    ClearDataSource(featureSrvc, dataSourceId, featureName);
            }
            else
            {
                MgCoordinateSystemFactory srsFactory = new MgCoordinateSystemFactory();
                MgCoordinateSystem srsMap = srsFactory.Create(srs);

                int srsType = srsMap.GetType();
                if (srsType == MgCoordinateSystemType.Geographic)
                    distance = srsMap.MeasureGreatCircleDistance(x1, y1, x2, y2);
                else
                    distance = srsMap.MeasureEuclideanDistance(x1, y1, x2, y2);

                distance = srsMap.ConvertCoordinateSystemUnitsToMeters(distance);

                if (0 == us)
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

                if (segId == 1)
                {
                    //first segment
                    //
                    if (!DataSourceExists(resourceSrvc, dataSourceId))
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
                        MgCreateSdfParams parameters = new MgCreateSdfParams("LatLong", srs, schema);
                        featureSrvc.CreateFeatureSource(dataSourceId, parameters);

                        //build map tip
                        String unitText = (us == 1) ? "Miles" : "Kilometers";
                        String tip = String.Format("Concat(Concat(Concat('" + MgLocalizer.GetString("MEASUREPARTIAL", locale) + ": ', PARTIAL), Concat(', " + MgLocalizer.GetString("MEASURETOTAL", locale) + ": ', TOTAL)), ' ({0})')", unitText);

                        //Create the layer definition
                        MgByteReader layerDefContent = BuildLayerDefinitionContent(dataSource, featureName, tip);
                        resourceSrvc.SetResource(layerDefId, layerDefContent, null);

                        //Add the layer to the map, if it's not already in it
                        if (layer == null)
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

            if (layer != null)
                layer.ForceRefresh();
            map.Save(resourceSrvc);
        }
        catch (MgException exc)
        {
            OnError(MgLocalizer.GetString("MEASUREERROR", locale), exc.GetMessage());
            return;
        }
        catch (Exception ne)
        {
            OnError(MgLocalizer.GetString("MEASUREERROR", locale), ne.Message);
            return;
        }

        String measureUiTempl = MgLocalizer.Localize(LoadTemplate(Request, "../viewerfiles/measureui.templ"), locale, GetClientOS(Request));

        String vpath = GetSurroundVirtualPath(Request);
        String[] vals = {
                    locale,
                    target.ToString(NumberFormatInfo.InvariantInfo),
                    popup.ToString(NumberFormatInfo.InvariantInfo),
                    mapName,
                    sessionId,
                    total.ToString(NumberFormatInfo.InvariantInfo),
                    distance.ToString(NumberFormatInfo.InvariantInfo),
                    "1",
                    vpath + "measure.aspx",
                    vpath + "measure.aspx"
                    };
        Response.Write(Substitute(measureUiTempl, vals));

    }
    finally
    {
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
    mapName = GetParameter(parameters, "MAPNAME");
    sessionId = GetParameter(parameters, "SESSION");
    target = GetIntParameter(parameters, "TGT");
    popup = GetIntParameter(parameters, "POPUP");
    locale = GetParameter(parameters, "LOCALE");
    if(IsParameter(parameters, "CLEAR"))
        clear = true;
    else
    {
        x1 = GetDoubleParameter(parameters, "X1");
        y1 = GetDoubleParameter(parameters, "Y1");
        x2 = GetDoubleParameter(parameters, "X2");
        y2 = GetDoubleParameter(parameters, "Y2");
        total = GetDoubleParameter(parameters, "TOTAL");
        us = GetIntParameter(parameters, "US");
        segId = GetIntParameter(parameters, "SEGID");
    }
}

String GetMapSrs(MgMap map)
{
    String srs = map.GetMapSRS();
    if(srs != "")
        return srs;

    //SRS is currently optional. Waiting for this to change, set the default SRS to ArbitrayXY meters
    //
    return "LOCALCS[\"Non-Earth (Meter)\",LOCAL_DATUM[\"Local Datum\",0],UNIT[\"Meter\", 1],AXIS[\"X\",EAST],AXIS[\"Y\",NORTH]]";
}

MgLayer FindLayer(MgLayerCollection layers, String layerDef)
{
    MgLayer layer = null;
    int i=0;
    for(i = 0; i < layers.GetCount(); i++)
    {
        MgLayer layer1 = (MgLayer) layers.GetItem(i);
        if(layer1.GetLayerDefinition().ToString() == layerDef)
        {
            layer = layer1;
            break;
        }
    }
    return layer;
}

bool DataSourceExists(MgResourceService resourceSrvc, MgResourceIdentifier dataSourceId)
{
    try
    {
        MgByteReader cnt = resourceSrvc.GetResourceContent(dataSourceId);
        return true;
    }
    catch(MgResourceNotFoundException)
    {
        return false;
    }
}

MgByteReader BuildLayerDefinitionContent(String dataSource, String featureName, String tip)
{
    String layerTempl = LoadTemplate(Request, "../viewerfiles/linelayerdef.templ");
    String[] vals = {
                    dataSource,
                    featureName,
                    "GEOM",
                    tip,
                    "1",
                    "ff0000"
                    };
    layerTempl = Substitute(layerTempl, vals);
    byte [] bytes = Encoding.UTF8.GetBytes(layerTempl);
    MgByteSource src = new MgByteSource(bytes, bytes.Length);

    return src.GetReader();
}

void ClearDataSource(MgFeatureService featureSrvc, MgResourceIdentifier dataSourceId, String featureName)
{
    MgDeleteFeatures deleteCmd = new MgDeleteFeatures(featureName, "KEY >= 0");
    MgFeatureCommandCollection commands = new MgFeatureCommandCollection();
    commands.Add(deleteCmd);
    featureSrvc.UpdateFeatures(dataSourceId, commands, false);
}

void OnError(String title, String msg)
{
    String templ = MgLocalizer.Localize(LoadTemplate(Request, "../viewerfiles/errorpage.templ"), locale, GetClientOS(Request));
    String[] vals = { popup.ToString(NumberFormatInfo.InvariantInfo), title, msg };
    Response.Write(Substitute(templ, vals));
}

void ReleaseReader(MgPropertyCollection res)
{
    if(res == null)
        return;
    MgProperty prop = res.GetItem(0);
    if (prop == null)
        return;
    if (prop is MgStringProperty)
        throw new Exception(((MgStringProperty)prop).GetValue());
    MgFeatureProperty resProp = (MgFeatureProperty)prop;
    MgFeatureReader reader = (MgFeatureReader)resProp.GetValue();
    if(reader == null)
        return;
    reader.Close();
}

</script>

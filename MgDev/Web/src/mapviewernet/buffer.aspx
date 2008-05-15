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

<%@ Page language="c#" validateRequest="false" %>
<%@ Import Namespace="System" %>
<%@ Import Namespace="System.IO" %>
<%@ Import Namespace="System.Collections.Specialized" %>
<%@ Import Namespace="System.Text" %>
<%@ Import Namespace="OSGeo.MapGuide" %>

<!-- #Include File="common.aspx -->

<script language="c#" runat="server">
String mapName = "";
String sessionId = "";
String bufferName = "";
String layersParam = "";
String locale = "";
int popup = 0;
String lcolor = "";
String ffcolor = "";
String fbcolor = "";
int transparent = 0;
double distance = 0;
String units = "";
String linestyle = "";
String fillstyle = "";
String thickness = "";
int merge = 0;
int foretrans = 50;
String selText = "";
String srs = "";
String featureName = "Buffer";
String dataSource = "";
</script>

<%
    int i=0;

    Response.Charset = "utf-8";
    MgLocalizer.SetLocalizedFilesPath(Request.ServerVariables["APPL_PHYSICAL_PATH"] + "..\\localized\\");

    GetRequestParameters();

    dataSource = "Session:" + sessionId + "//" + bufferName + "_Buffer.FeatureSource";
    String layerDef = "Session:" + sessionId + "//" + bufferName + "_Buffer.LayerDefinition";

    try
    {
        bool newBuffer = false;

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

        //load the map runtime state
        //
        MgMap map = new MgMap();
        map.Open(resourceSrvc, mapName);

        //locate the buffer layer in the map. It might or might not already exist
        //
        MgLayerCollection layers = map.GetLayers();
        MgLayer layer = FindLayer(layers, bufferName);

        String[] layerNames = layersParam.Split(new char[] { ',' });

        // convert distance to meters
        if (units.Equals("mi"))      //miles
            distance *= 1609.35;
        else if (units.Equals("ki")) //kilometers
            distance *= 1000;
        else if (units.Equals("fe")) //feet
            distance *= 0.30480;

        // Get the map SRS
        //
        MgCoordinateSystemFactory srsFactory = new MgCoordinateSystemFactory();
        String srsDefMap = GetMapSrs(map);
        String mapSrsUnits = "";
        MgCoordinateSystem srsMap = srsFactory.Create(srsDefMap);
        bool arbitraryMapSrs = (srsMap.GetType() == MgCoordinateSystemType.Arbitrary);
        if (arbitraryMapSrs)
            mapSrsUnits = srsMap.GetUnits();

        //Create/Modify layer definition
        MgByteReader layerDefContent = BuildLayerDefinitionContent();
        resourceSrvc.SetResource(layerDefId, layerDefContent, null);

        if (layer == null)
        {
            newBuffer = true;
            //Targetting a new layer. create a data source for it
            //
            MgClassDefinition classDef = new MgClassDefinition();

            classDef.SetName(featureName);
            classDef.SetDescription(MgLocalizer.GetString("BUFFERCLASSDESCR", locale));
            classDef.SetDefaultGeometryPropertyName("GEOM");

            //Set KEY property
            MgDataPropertyDefinition prop = new MgDataPropertyDefinition("KEY");
            prop.SetDataType(MgPropertyType.Int32);
            prop.SetAutoGeneration(true);
            prop.SetReadOnly(true);
            classDef.GetIdentityProperties().Add(prop);
            classDef.GetProperties().Add(prop);

            //Set ID property. Hold this segment ID
            prop = new MgDataPropertyDefinition("ID");
            prop.SetDataType(MgPropertyType.Int32);
            classDef.GetProperties().Add(prop);

            //Set geometry property
            MgGeometricPropertyDefinition geomProp = new MgGeometricPropertyDefinition("GEOM");
            //prop.SetGeometryTypes(MgFeatureGeometricType.mfgtSurface); //TODO use the constant when exposed
            geomProp.SetGeometryTypes(4);
            classDef.GetProperties().Add(geomProp);

            //Create the schema
            MgFeatureSchema schema = new MgFeatureSchema("BufferSchema", MgLocalizer.GetString("BUFFERSCHEMADESCR", locale));
            schema.GetClasses().Add(classDef);

            //finally, creation of the feature source
            MgCreateSdfParams sdfParams = new MgCreateSdfParams("LatLong", srsDefMap, schema);
            featureSrvc.CreateFeatureSource(dataSourceId, sdfParams);

            //Add layer to map
            layer = new MgLayer(layerDefId, resourceSrvc);
            layer.SetName(bufferName);
            layer.SetLegendLabel(bufferName);
            layer.SetDisplayInLegend(true);
            layer.SetSelectable(true);
            layers.Insert(0, layer);
        }
        else
        {
            //data source already exist. clear its content
            //
            ClearDataSource(featureSrvc, dataSourceId, featureName);
        }

        MgSelection sel = new MgSelection(map, selText);
        MgReadOnlyLayerCollection selLayers = sel.GetLayers();

        MgAgfReaderWriter agfRW = new MgAgfReaderWriter();
        MgGeometryCollection bufferGeometries = new MgGeometryCollection();
        MgGeometry geomBuffer;

        MgFeatureCommandCollection commands = new MgFeatureCommandCollection();
        int featId = 0;

        MgBatchPropertyCollection propCollection = new MgBatchPropertyCollection();

        int excludedLayers = 0;
        MgCoordinateSystem srsDs = null;
        MgGeometryCollection inputGeometries = new MgGeometryCollection();

        int bufferFeatures = 0;
        for (int li = 0; li < selLayers.GetCount(); li++)
        {
            MgLayer selLayer = (MgLayer) selLayers.GetItem(li);
            bool inputLayer = false;
            String selLayerName = selLayer.GetName();
            for (int il = 0; il < layerNames.Length; il++)
            {
                if (layerNames[il].Equals(selLayerName))
                {
                    inputLayer = true;
                    break;
                }
            }
            if (inputLayer == false)
            {
                continue;
            }

            // get the data source SRS
            //
            MgResourceIdentifier featSourceId = new MgResourceIdentifier(selLayer.GetFeatureSourceId());
            MgSpatialContextReader ctxs = featureSrvc.GetSpatialContexts(featSourceId, false);
            String srsDefDs = "";
            if (ctxs != null && ctxs.ReadNext())
                srsDefDs = ctxs.GetCoordinateSystemWkt();

            if (srsDefDs == null || srsDefDs.Length == 0)
            {
                excludedLayers++;
                continue;
            }

            srsDs = srsFactory.Create(srsDefDs);
            bool arbitraryDsSrs = (srsDs.GetType() == MgCoordinateSystemType.Arbitrary);
            String dsSrsUnits = "";

            if (arbitraryDsSrs)
                dsSrsUnits = srsDs.GetUnits();

            // exclude layer if:
            //  the map is non-arbitrary and the layer is arbitrary or vice-versa
            //     or
            //  layer and map are both arbitrary but have different units
            //
            if ((arbitraryDsSrs != arbitraryMapSrs) || (arbitraryDsSrs && (dsSrsUnits != mapSrsUnits)))
            {
                excludedLayers++;
                continue;
            }

            // calculate distance in the data source SRS units
            //
            double dist = srsDs.ConvertMetersToCoordinateSystemUnits(distance);

            // calculate great circle unless data source srs is arbitrary
            MgCoordinateSystemMeasure measure;
            if (!arbitraryDsSrs)
                measure = srsDs.GetMeasure();
            else
                measure = null;

            // create a SRS transformer if necessary
            MgCoordinateSystemTransform srsXform;
            if (!srsDefDs.Equals(srsDefMap))
                srsXform = srsFactory.GetTransform(srsDs, srsMap);
            else
                srsXform = null;

            String featureClassName = selLayer.GetFeatureClassName();
            
            // TODO:  How to get selectionSize?
            int selectionSize = 20;
            MgStringCollection filters = sel.GenerateFilters(selLayer, featureClassName, selectionSize);
            
            int numFilter = filters.GetCount();
            for (int filterIndex = 0; filterIndex < numFilter; filterIndex++)
            {
                String filter = filters.GetItem(filterIndex);
                if (filter == null || filter.Length == 0)
                    continue;

                MgFeatureQueryOptions query = new MgFeatureQueryOptions();
                query.SetFilter(filter);

                MgResourceIdentifier featureSource = new MgResourceIdentifier(selLayer.GetFeatureSourceId());

                MgFeatureReader features = featureSrvc.SelectFeatures(featureSource, featureClassName, query);

                if (features.ReadNext())
                {
                    MgClassDefinition classDef = features.GetClassDefinition();
                    String geomPropName = classDef.GetDefaultGeometryPropertyName();

                    do
                    {
                        MgByteReader geomReader = features.GetGeometry(geomPropName);
                        MgGeometry geom = agfRW.Read(geomReader);

                        if (merge == 0)
                        {
                            geomBuffer = geom.Buffer(dist, measure);
                            if (geomBuffer != null)
                            {
                                if (srsXform != null)
                                    geomBuffer = (MgGeometry)geomBuffer.Transform(srsXform);
                                AddFeatureToCollection(propCollection, agfRW, featId++, geomBuffer);
                                bufferFeatures++;
                            }
                        }
                        else
                        {
                            if (srsXform != null)
                                geom = (MgGeometry)geom.Transform(srsXform);
                            inputGeometries.Add(geom);
                        }
                    }
                    while (features.ReadNext());

                    features.Close();
                }
            }
        }

        if (merge == 1)
        {
            if (inputGeometries.GetCount() > 0)
            {
                double dist = srsMap.ConvertMetersToCoordinateSystemUnits(distance);
                MgCoordinateSystemMeasure measure;
                if (!arbitraryMapSrs)
                    measure = srsMap.GetMeasure();
                else
                    measure = null;

                MgGeometryFactory geomFactory = new MgGeometryFactory();
                geomBuffer = geomFactory.CreateMultiGeometry(inputGeometries).Buffer(dist, measure);
                if (geomBuffer != null)
                {
                    AddFeatureToCollection(propCollection, agfRW, featId, geomBuffer);
                    bufferFeatures = 1;
                }
            }
        }

        if (propCollection.GetCount() > 0)
        {
            commands.Add(new MgInsertFeatures(featureName, propCollection));

            //Insert the features in the temporary data source
            //
            ReleaseReader(featureSrvc.UpdateFeatures(dataSourceId, commands, false), commands);
        }

        // Save the new map state
        //
        layer.ForceRefresh();
        map.Save(resourceSrvc);

        //build report message
        String title = MgLocalizer.GetString("BUFFERREPORTTITLE", locale);
        String createdUpdatedFmt = newBuffer ? MgLocalizer.GetString("BUFFERREPORTCREATED", locale) : MgLocalizer.GetString("BUFFERREPORTUPDATED", locale);
        String createdUpdatedStr = Substitute(createdUpdatedFmt, new String[] { bufferName });
        String featuresFmt = bufferFeatures > 1 ? MgLocalizer.GetString("BUFFERREPORTFEATURESPLURAL", locale) : MgLocalizer.GetString("BUFFERREPORTFEATURESSINGULAR", locale);
        String featuresStr = Substitute(featuresFmt, new String[] { bufferFeatures.ToString() });
        String msg = createdUpdatedStr + "<p><p>" + featuresStr;

        if (excludedLayers > 0)
        {
            String warningFmt = excludedLayers > 1 ? MgLocalizer.GetString("BUFFERREPORTWARNINGSINGULAR", locale) : MgLocalizer.GetString("BUFFERREPORTWARNINGPLURAL", locale);
            String warningStr = Substitute(warningFmt, new String[] { excludedLayers.ToString() });
            msg = msg + "<p><p>" + warningStr;
        }

        // return the report page
        String templ = LoadTemplate(Request, "../viewerfiles/bufferreport.templ");

        templ = MgLocalizer.Localize(templ, locale, GetClientOS(Request));

        String[] vals = {
                    popup.ToString(NumberFormatInfo.InvariantInfo),
                    MgLocalizer.GetString("BUFFERREPORTTITLE", locale),
                    msg};
        Response.Write(Substitute(templ, vals));
    }
    catch (MgException exc)
    {
        OnError(MgLocalizer.GetString("BUFFERERROR", locale), exc.GetMessage());
        return;
    }
    catch (Exception ne)
    {
        OnError(MgLocalizer.GetString("BUFFERERROR", locale), ne.Message);
        return;
    }

%>

<script language="c#" runat="server">

void GetRequestParameters()
{
    if(Request.HttpMethod == "POST")
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
    mapName = GetParameter(parameters, "MAPNAME");
    sessionId = GetParameter(parameters, "SESSION");
    popup = GetIntParameter(parameters, "POPUP");
    bufferName = GetParameter(parameters, "BUFFER");
    layersParam = GetParameter(parameters, "LAYERS");
    lcolor = GetParameter(parameters, "LCOLOR");
    ffcolor = GetParameter(parameters, "FFCOLOR");
    fbcolor = GetParameter(parameters, "FBCOLOR");
    foretrans = GetIntParameter(parameters, "FORETRANS");
    transparent = GetIntParameter(parameters, "TRANSPARENT");
    locale = GetParameter(parameters, "LOCALE");
    distance = GetLocalizedDoubleParameter(parameters, "DISTANCE", locale);
    units = GetParameter(parameters, "UNITS");
    linestyle = GetParameter(parameters, "LINESTYLE");
    fillstyle = GetParameter(parameters, "FILLSTYLE");
    thickness = GetParameter(parameters, "THICKNESS");
    selText = GetParameter(parameters, "SELECTION");
    if(IsParameter(parameters, "MERGE"))
        merge = 1;

    if(foretrans < 0 || foretrans > 100)
    {
        foretrans = 50;
    }

}

MgLayer FindLayer(MgLayerCollection layers, String layerName)
{
    MgLayer layer = null;
    int i = 0;
    for(i = 0; i < layers.GetCount(); i++)
    {
        MgLayer layer1 = (MgLayer) layers.GetItem(i);

        if(layer1.GetName() == layerName)
        {
            layer = layer1;
            break;
        }
    }
    return layer;
}

MgByteReader BuildLayerDefinitionContent()
{
    String layerTempl = LoadTemplate(Request, "../viewerfiles/arealayerdef.templ");
    String xtrans = String.Format("{0:x2}", (255 * foretrans / 100));
    String[] vals = {
                    dataSource,
                    featureName,
                    "GEOM",
                    fillstyle,
                    xtrans + ffcolor,
                    (0!=transparent)? "ff" + fbcolor: "00" + fbcolor,
                    linestyle,
                    thickness,
                    lcolor
                    };
    layerTempl = Substitute(layerTempl, vals);
    byte [] bytes = Encoding.UTF8.GetBytes(layerTempl);
    MgByteSource src = new MgByteSource(bytes, bytes.Length);
    return src.GetReader();
}



void ClearDataSource(MgFeatureService featureSrvc, MgResourceIdentifier dataSourceId, String featureName)
{
    MgDeleteFeatures deleteCmd = new MgDeleteFeatures(featureName, "ID >= 0");
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

void ReleaseReader(MgPropertyCollection res, MgFeatureCommandCollection commands)
{
    if(res == null)
        return;

    for(int i = 0; i < res.GetCount(); i++)
    {
        MgFeatureCommand cmd = commands.GetItem(i);
        if(cmd is MgInsertFeatures)
        {
            MgFeatureProperty resProp = res.GetItem(i) as MgFeatureProperty;
            if(resProp != null)
            {
                MgFeatureReader reader = resProp.GetValue() as MgFeatureReader;
                if(reader == null)
                    return;
                reader.Close();
            }
        }
    }
}


void AddFeatureToCollection(MgBatchPropertyCollection propCollection, MgAgfReaderWriter agfRW, int featureId, MgGeometry featureGeom)
{
    MgPropertyCollection bufferProps = new MgPropertyCollection();
    MgInt32Property idProp = new MgInt32Property("ID", featureId);
    bufferProps.Add(idProp);
    MgByteReader geomReader = agfRW.Write(featureGeom);
    MgGeometryProperty geomProp = new MgGeometryProperty("GEOM", geomReader);
    bufferProps.Add(geomProp);
    propCollection.Add(bufferProps);
}

String GetMapSrs(MgMap map)
{
    try
    {
        String srs = map.GetMapSRS();
        if (srs!= "")
            return srs;
    }
    catch (MgException e)
    {
    }

    //No SRS, set to ArbitrayXY meters
    //
    return "LOCALCS[\"Non-Earth (Meter)\",LOCAL_DATUM[\"Local Datum\",0],UNIT[\"Meter\", 1],AXIS[\"X\",EAST],AXIS[\"Y\",NORTH]]";
}

</script>

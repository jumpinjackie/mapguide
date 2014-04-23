<%--
Copyright (C) 2004-2007  Autodesk, Inc.
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

<%@ Import Namespace="System.IO" %>
<%@ Import Namespace="System.Collections.Specialized" %>
<%@ Import Namespace="System.Text" %>
<%@ Import Namespace="System.Globalization" %>
<%@ Import Namespace="OSGeo.MapGuide" %>
<%@ Import Namespace="Jayrock.Json" %>
<%@ Import Namespace="Jayrock.Json.Conversion" %>

<!-- #Include File="property.aspx" -->
<!-- #Include File="feature.aspx" -->
<!-- #Include File="../../utilityfunctions.aspx -->

<script language="c#" runat="server">
    public class Query
    {
        NameValueCollection args = null;
        private MgSiteConnection site = null;
        private String[] numOperators = null;
        private String[] numExpressions = null;
        private String[] strOperators = null;
        private String[] strExpressions = null;
        
        public Query(NameValueCollection incomingArgs)
        {
            this.args = incomingArgs;
            this.site = new MgSiteConnection();
            this.site.Open(new MgUserInformation(GetParameter(this.args, "SESSION")));
            this.numOperators = new String[] {"Equal to", "Not equal to", "Greater than", "Greater than or equal to", "Less than", "Less than or equal to"};
            this.numExpressions = new String[] {" = %s", " != %s", " > %s", " >= %s", " < %s", " <= %s"};
            this.strOperators = new String[] {"Begins with", "Contains", "Equal to"};
            this.strExpressions = new String[] {" like '%s%%'", " like '%%%s%%'", " = '%s'"};
        }
        
        public String getNumOp()
        {
            String jsonNumOps = JsonConvert.ExportToString(this.numOperators);
            return jsonNumOps;
        }

        public String getStrOp()
        {
            String jsonStrOps = JsonConvert.ExportToString(this.strOperators);
            return jsonStrOps;
        }
        
        public ArrayList GetMapLayerNames()
        {
            MgResourceService resourceService = (MgResourceService)this.site.CreateService(MgServiceType.ResourceService);
            MgMap map = new MgMap();
            map.Open(resourceService, GetParameter(this.args, "MAPNAME"));
            MgLayerCollection layers = map.GetLayers();
            ArrayList layerNames = new ArrayList();

            for(int i=0; i<layers.GetCount(); i++)
            {
                MgLayer layer = (MgLayer) layers.GetItem(i);

                //TODO: Exclude Raster and Drawing Layers???
                
                if(!layer.GetName().StartsWith("_") && !layer.GetFeatureSourceId().ToUpper().StartsWith("SESSION"))
                {
                    layerNames.Add(layer.GetLegendLabel());
                }
            }
            layerNames.Sort();

            return layerNames;
        }
        
        public ArrayList GetLayerProperties()
        {
            ArrayList properties = new ArrayList();
            MgResourceService resourceService = (MgResourceService)this.site.CreateService(MgServiceType.ResourceService);
            MgMap map = new MgMap();
            map.Open(resourceService, GetParameter(this.args, "MAPNAME"));
            MgLayer layer = (MgLayer) map.GetLayers().GetItem(GetParameter(this.args, "LAYERNAME"));

            MgFeatureService featureService = (MgFeatureService)this.site.CreateService(MgServiceType.FeatureService);
            MgResourceIdentifier resId = new MgResourceIdentifier(layer.GetFeatureSourceId());
            String[] schemaClass = layer.GetFeatureClassName().Split(new Char[]{':'});

            MgClassDefinition classDef = featureService.GetClassDefinition(resId, schemaClass[0], schemaClass[1]);

            for(int i=0; i<classDef.GetProperties().GetCount(); i++)
            {
                MgPropertyDefinition propertyDef = classDef.GetProperties().GetItem(i);

                if(propertyDef.GetPropertyType() == MgFeaturePropertyType.DataProperty)
                {
                    MgDataPropertyDefinition propertyDataDef = (MgDataPropertyDefinition) propertyDef;
                    int dataType = propertyDataDef.GetDataType();
                    if(this.IsValidDataType(dataType))
                    {
                        properties.Add(new Property(propertyDataDef.GetName(), dataType == MgPropertyType.String));
                    }
                }
            }
            return properties;
        }
        
        public bool ToggleSpatialFilter()
        {
            bool result = true;
            MgResourceService resourceService = (MgResourceService)this.site.CreateService(MgServiceType.ResourceService);
            MgMap map = new MgMap();
            map.Open(resourceService, GetParameter(this.args, "MAPNAME"));

            MgLayerCollection layers = map.GetLayers();
            if(layers.Contains("_QuerySpatialFilter"))
            {
                MgLayer layer = (MgLayer) layers.GetItem("_QuerySpatialFilter");
                if(GetParameter(this.args, "VISIBLE").Equals("true"))
                    layer.SetVisible(true);
                else
                    layer.SetVisible(false);

                map.Save(resourceService);
            }

            return result;
        }

        public bool ShowSpatialFilter()
        {
            bool result = true;
            MgResourceIdentifier sdfResId = new MgResourceIdentifier("Session:"+ GetParameter(this.args, "SESSION")+"//Filter.FeatureSource");

            MgResourceService resourceService = (MgResourceService)this.site.CreateService(MgServiceType.ResourceService);
            MgFeatureService featureService = (MgFeatureService)this.site.CreateService(MgServiceType.FeatureService);

            MgFeatureCommandCollection updateCommands = new MgFeatureCommandCollection();

            MgMap map = new MgMap();
            map.Open(resourceService, GetParameter(this.args, "MAPNAME"));

            MgLayer layer = null;
            MgLayerCollection layers = map.GetLayers();
            if(layers.Contains("_QuerySpatialFilter"))
            {
                layer = (MgLayer) layers.GetItem("_QuerySpatialFilter");
                updateCommands.Add(new MgDeleteFeatures("Filter", "ID like '%'"));
            }
            else
            {
                // Create the Feature Source (SDF)

                MgFeatureSchema sdfSchema = this.CreateFilterSchema();
                MgCreateSdfParams sdfParams = new MgCreateSdfParams("MAPCS", map.GetMapSRS(), sdfSchema);
                featureService.CreateFeatureSource(sdfResId, sdfParams);

                // Create the Layer

                MgResourceIdentifier layerResId = new MgResourceIdentifier("Session:" + GetParameter(this.args, "SESSION") + "//Filter.LayerDefinition");
                String layerDefinition = File.ReadAllText(GetQueryXmlTemplatePath());
                layerDefinition = layerDefinition.Replace("%s", sdfResId.ToString());

                MgByteReader reader = new MgByteReader(layerDefinition, "text/xml");
                resourceService.SetResource(layerResId, reader, null);

                layer = new MgLayer(layerResId, resourceService);

                layer.SetName("_QuerySpatialFilter");
                layer.SetLegendLabel("_QuerySpatialFilter");
                layer.SetDisplayInLegend(false);
                layer.SetSelectable(false);

                layers.Insert(0, layer);
            }

            // Make the layer visible

            layer.SetVisible(true);
            map.Save(resourceService);

            // Add the geometry to the filter feature source

            MgPolygon polygon = this.CreatePolygonFromGeomText(GetParameter(this.args, "GEOMTEXT"));
            MgAgfReaderWriter agfWriter = new MgAgfReaderWriter();
            MgByteReader byteReader = agfWriter.Write(polygon);

            MgPropertyCollection propertyValues = new MgPropertyCollection();
            propertyValues.Add(new MgGeometryProperty("Geometry", byteReader));

            updateCommands.Add(new MgInsertFeatures("Filter", propertyValues));

            featureService.UpdateFeatures(sdfResId, updateCommands, false);

            return result;
        }
        
        public ArrayList Execute()
        {
            ArrayList result = new ArrayList();
            MgResourceService resourceService = (MgResourceService)this.site.CreateService(MgServiceType.ResourceService);

            MgMap map = new MgMap();
            map.Open(resourceService, GetParameter(this.args, "MAPNAME"));
            MgLayer layer = (MgLayer) map.GetLayers().GetItem(GetParameter(this.args, "LAYERNAME"));

            MgFeatureService featureService = (MgFeatureService)this.site.CreateService(MgServiceType.FeatureService);
            MgResourceIdentifier resId = new MgResourceIdentifier(layer.GetFeatureSourceId());
            String featureClass = layer.GetFeatureClassName();
            String featureGeometry = layer.GetFeatureGeometryName();

            // Initialize the coordinate system transform

            String[] schemaClass = layer.GetFeatureClassName().Split(new Char[]{':'});
            MgClassDefinition classDef = featureService.GetClassDefinition(resId, schemaClass[0], schemaClass[1]);
            MgGeometricPropertyDefinition geomProp = (MgGeometricPropertyDefinition) classDef.GetProperties().GetItem(featureGeometry);
            String spatialContext = geomProp.GetSpatialContextAssociation();

            MgCoordinateSystemTransform csTransform = null;
            MgCoordinateSystemTransform csInverseTransform = null;
            MgCoordinateSystemFactory coordSysFactory = new MgCoordinateSystemFactory();

            MgSpatialContextReader scReader = featureService.GetSpatialContexts(resId, false);
            while(scReader.ReadNext() && csTransform==null)
            {
                if(spatialContext.Equals(scReader.GetName()))
                {
                    MgCoordinateSystem source = coordSysFactory.Create(scReader.GetCoordinateSystemWkt());
                    MgCoordinateSystem target = coordSysFactory.Create(map.GetMapSRS());
                    csTransform = coordSysFactory.GetTransform(source, target);
                    csInverseTransform = coordSysFactory.GetTransform(target, source);
                }
            }
            scReader.Close();

            // Execute the query

            int queryMax = Int32.Parse(GetParameter(this.args, "QUERYMAX"));
            MgFeatureQueryOptions queryOptions = new MgFeatureQueryOptions();

            if(GetParameter(this.args, "USEPROPERTYFILTER").Equals("true"))
            {
                String propertyFilter = GetParameter(this.args, "PROPERTYNAME");
                int op = Int32.Parse(GetParameter(this.args, "OPERATOR"));
                if(GetParameter(this.args, "ISSTRING").Equals("true"))
                {
                    propertyFilter = propertyFilter + this.strExpressions[op].Replace("%s", GetParameter(this.args, "VALUE"));
                }
                else
                {
                    propertyFilter = propertyFilter + this.numExpressions[op].Replace("%s", GetParameter(this.args, "VALUE"));
                }
                queryOptions.SetFilter(propertyFilter);
            }

            if(GetParameter(this.args, "USESPATIALFILTER").Equals("true"))
            {
                MgPolygon polygon = this.CreatePolygonFromGeomText(GetParameter(this.args, "GEOMTEXT"));
                MgGeometry geometry = (MgGeometry) polygon.Transform(csInverseTransform);
                queryOptions.SetSpatialFilter(featureGeometry, geometry, MgFeatureSpatialOperations.Intersects);
            }

            int count = 0;
            MgAgfReaderWriter geometryReaderWriter = new MgAgfReaderWriter();
            MgFeatureReader featureReader = featureService.SelectFeatures(resId, layer.GetFeatureClassName(), queryOptions);
            while(featureReader.ReadNext() && (queryMax <= 0 || count < queryMax))
            {
                MgByteReader byteReader = featureReader.GetGeometry(featureGeometry);
                MgGeometry geometry = geometryReaderWriter.Read(byteReader);
                MgPoint centerPoint = geometry.GetCentroid();
                centerPoint = (MgPoint) centerPoint.Transform(csTransform);
                Hashtable idList = new Hashtable(this.GetFeatureIdList(featureReader));
                String displayValue = GetFeaturePropertyValue(featureReader, GetParameter(this.args, "OUTPUTPROPERTY")); 
                result.Add(new Feature(displayValue, centerPoint, idList));
                count++;
            }

            return result;
        }
        
        public String GetSelectionXML()
        {
            MgResourceService resourceService = (MgResourceService)this.site.CreateService(MgServiceType.ResourceService);
            MgFeatureService featureService = (MgFeatureService)this.site.CreateService(MgServiceType.FeatureService);

            MgMap map = new MgMap();
            map.Open(resourceService, GetParameter(this.args, "MAPNAME"));
            MgLayer layer = (MgLayer) map.GetLayers().GetItem(GetParameter(this.args, "LAYERNAME"));
            MgResourceIdentifier resId = new MgResourceIdentifier(layer.GetFeatureSourceId());
            String featureClass = layer.GetFeatureClassName();

            String[] schemaClass = layer.GetFeatureClassName().Split(new Char[]{':'});
            MgClassDefinition classDef = featureService.GetClassDefinition(resId, schemaClass[0], schemaClass[1]);

            MgPropertyCollection properties = new MgPropertyCollection();
            MgDataPropertyDefinition dataPropDef = null;
            String jsonIdList = GetParameter(this.args, "IDLIST");
            JsonObject idList = JsonConvert.Import(jsonIdList) as JsonObject;
            if(idList != null)
            {
                foreach(String key in idList.Names)
                {
                    String value = idList[key].ToString();
                    dataPropDef = (MgDataPropertyDefinition) classDef.GetProperties().GetItem(key);
                    switch(dataPropDef.GetDataType())
                    {
                        case MgPropertyType.Boolean :
                            properties.Add(new MgBooleanProperty(key, Boolean.Parse(value)));
                            break;
                        case MgPropertyType.Byte :
                            properties.Add(new MgByteProperty(key, Byte.Parse(value)));
                            break;
                        case MgPropertyType.Single :
                            properties.Add(new MgSingleProperty(key, (float)Double.Parse(value)));
                            break;
                        case MgPropertyType.Double :
                            properties.Add(new MgDoubleProperty(key, Double.Parse(value)));
                            break;
                        case MgPropertyType.Int16 :
                            properties.Add(new MgInt16Property(key, Int16.Parse(value)));
                            break;
                        case MgPropertyType.Int32 :
                            properties.Add(new MgInt32Property(key, Int32.Parse(value)));
                            break;
                        case MgPropertyType.Int64 :
                            properties.Add(new MgInt64Property(key, Int64.Parse(value)));
                            break;
                        case MgPropertyType.String :
                            properties.Add(new MgStringProperty(key, value));
                            break;
                        case MgPropertyType.DateTime :
                        case MgPropertyType.Null :
                        case MgPropertyType.Blob :
                        case MgPropertyType.Clob :
                        case MgPropertyType.Feature :
                        case MgPropertyType.Geometry :
                        case MgPropertyType.Raster :
                            break;
                    }
                }
            }

            MgSelection selection = new MgSelection(map);
            selection.AddFeatureIds(layer, featureClass, properties);

            return selection.ToXml();
        }
                
        private Hashtable GetFeatureIdList(MgFeatureReader featureReader)
        {
            MgClassDefinition classDef = featureReader.GetClassDefinition();
            MgPropertyDefinitionCollection idProps = classDef.GetIdentityProperties();
            Hashtable idList = new Hashtable();

            for(int i=0; i<idProps.GetCount(); i++)
            {
                MgDataPropertyDefinition idProp = (MgDataPropertyDefinition) idProps.GetItem(i);
                String propName = idProp.GetName();
                switch(idProp.GetDataType())
                {
                    case MgPropertyType.Boolean :
                        idList[propName] = featureReader.GetBoolean(propName);
                        break;
                    case MgPropertyType.Byte :
                        idList[propName] = featureReader.GetByte(propName);
                        break;
                    case MgPropertyType.Single :
                        idList[propName] = featureReader.GetSingle(propName);
                        break;
                    case MgPropertyType.Double :
                        idList[propName] = featureReader.GetDouble(propName);
                        break;
                    case MgPropertyType.Int16 :
                        idList[propName] = featureReader.GetInt16(propName);
                        break;
                    case MgPropertyType.Int32 :
                        idList[propName] = featureReader.GetInt32(propName);
                        break;
                    case MgPropertyType.Int64 :
                        idList[propName] = featureReader.GetInt64(propName);
                        break;
                    case MgPropertyType.String :
                        idList[propName] = featureReader.GetString(propName);
                        break;
                    case MgPropertyType.DateTime :
                        idList[propName] = featureReader.GetDateTime(propName);
                        break;
                    case MgPropertyType.Null :
                    case MgPropertyType.Blob :
                    case MgPropertyType.Clob :
                    case MgPropertyType.Feature :
                    case MgPropertyType.Geometry :
                    case MgPropertyType.Raster :
                    break;
                }
            }

            return idList;
        }
        
        private bool IsValidDataType(int type)
        {
            bool valid = true;

            switch(type)
            {
                case MgPropertyType.Blob:
                case MgPropertyType.Clob:
                case MgPropertyType.Byte:
                case MgPropertyType.Feature:
                case MgPropertyType.Geometry:
                case MgPropertyType.Null:
                    valid = false;
                    break;
            }

            return valid;
        }
        
        private MgFeatureSchema CreateFilterSchema()
        {
            MgFeatureSchema filterSchema = new MgFeatureSchema();
            filterSchema.SetName("FilterSchema");

            MgClassDefinition filterClass = new MgClassDefinition();
            filterClass.SetName("Filter");
            MgPropertyDefinitionCollection properties = filterClass.GetProperties();

            MgDataPropertyDefinition idProperty = new MgDataPropertyDefinition("ID");
            idProperty.SetDataType(MgPropertyType.Int32);
            idProperty.SetReadOnly(true);
            idProperty.SetNullable(false);
            idProperty.SetAutoGeneration(true);
            properties.Add(idProperty);

            MgGeometricPropertyDefinition geometryProperty = new MgGeometricPropertyDefinition("Geometry");
            geometryProperty.SetGeometryTypes(MgFeatureGeometricType.Surface);
            geometryProperty.SetHasElevation(false);
            geometryProperty.SetHasMeasure(false);
            geometryProperty.SetReadOnly(false);
            geometryProperty.SetSpatialContextAssociation("MAPCS");
            properties.Add(geometryProperty);

            filterClass.GetIdentityProperties().Add(idProperty);
            filterClass.SetDefaultGeometryPropertyName("Geometry");

            filterSchema.GetClasses().Add(filterClass);

            return filterSchema;
        }
        
        private MgPolygon CreatePolygonFromGeomText(String geomText)
        {
            MgGeometryFactory geometryFactory = new MgGeometryFactory();

            String[] vertices = geomText.Split(new Char[]{','});
            int count = Int32.Parse(vertices[0]);

            MgCoordinateCollection coords = new MgCoordinateCollection();
            for(int i=0; i<count; i++)
            {
                MgCoordinate coord = geometryFactory.CreateCoordinateXY(Double.Parse(vertices[(i*2)+1]), Double.Parse(vertices[(i*2)+2]));
                coords.Add(coord);
            }

            MgLinearRing linearRing = geometryFactory.CreateLinearRing(coords);
            MgPolygon polygon = geometryFactory.CreatePolygon(linearRing, null);

            return polygon;
        }
        
        private String GetQueryXmlTemplatePath()
        {
            String xmlTemplatePath = GetBasePath() + "query\\templates\\filterlayerdefinition.xml";
            return xmlTemplatePath;
        }
    }

</script>

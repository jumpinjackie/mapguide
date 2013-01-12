<%--
  -Copyright (C) 2004-2006  Autodesk, Inc.
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

<%@ page import="org.osgeo.mapguide.*" %>
<%@ page import="java.util.*" %>
<%@ page import="java.io.*" %>
<%@ page import="java.text.*" %>
<%@ page import="javax.servlet.jsp.*" %>
<%@ page import="javax.servlet.http.*" %>
<%@ page import="org.json.*" %>
<%@ include file ="property.jsp" %>
<%@ include file ="feature.jsp" %>

<%!
    public class Query
    {
        Map<String, String> args = null;
        private MgSiteConnection site = null;
        private String[] numOperators = null;
        private String[] numExpressions = null;
        private String[] strOperators = null;
        private String[] strExpressions = null;
        
        public Query(Map<String, String> incomingArgs) throws MgException
        {
            this.args = incomingArgs;
            this.site = new MgSiteConnection();
            this.site.Open(new MgUserInformation(this.args.get("SESSION")));
            this.numOperators = new String[] {"Equal to", "Not equal to", "Greater than", "Greater than or equal to", "Less than", "Less than or equal to"};
            this.numExpressions = new String[] {" = %s", " != %s", " > %s", " >= %s", " < %s", " <= %s"};
            this.strOperators = new String[] {"Begins with", "Contains", "Equal to"};
            this.strExpressions = new String[] {" like '%s%%'", " like '%%%s%%'", " = '%s'"};
        }
        
        public String getNumOp()
        {
            JSONArray jsonArray = new JSONArray();
            for(int i=0;i<6;i++)
            {
                jsonArray.put(numOperators[i]);
            }
            return jsonArray.toString();
        }

        public String getStrOp()
        {
            JSONArray jsonArray = new JSONArray();
            for(int i=0;i<3;i++)
            {
                jsonArray.put(strOperators[i]);
            }
            return jsonArray.toString();
        }
        
        public ArrayList<String> GetMapLayerNames() throws MgException
        {
            MgResourceService resourceService = (MgResourceService)this.site.CreateService(MgServiceType.ResourceService);
            MgMap map = new MgMap();
            map.Open(resourceService, this.args.get("MAPNAME"));
            MgLayerCollection layers = map.GetLayers();
            ArrayList<String> layerNames = new ArrayList<String>();

            for(int i=0; i<layers.GetCount(); i++)
            {
                MgLayer layer = (MgLayer) layers.GetItem(i);

                //TODO: Exclue Raster and Drawing Layers???
                
                if(!layer.GetName().startsWith("_") && !layer.GetFeatureSourceId().toUpperCase().startsWith("SESSION"))
                    layerNames.add(layer.GetLegendLabel());
            }
            Collections.sort(layerNames);

            return layerNames;
        }
        
        public ArrayList<Property> GetLayerProperties() throws MgException
        {
            ArrayList<Property> properties = new ArrayList<Property>();
            MgResourceService resourceService = (MgResourceService)this.site.CreateService(MgServiceType.ResourceService);
            MgMap map = new MgMap();
            map.Open(resourceService, this.args.get("MAPNAME"));
            MgLayer layer = (MgLayer) map.GetLayers().GetItem(this.args.get("LAYERNAME"));

            MgFeatureService featureService = (MgFeatureService)this.site.CreateService(MgServiceType.FeatureService);
            MgResourceIdentifier resId = new MgResourceIdentifier(layer.GetFeatureSourceId());
            String[] schemaClass = layer.GetFeatureClassName().split(":");

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
                        properties.add(new Property(propertyDataDef.GetName(), dataType == MgPropertyType.String));
                    }
                }
            }
            return properties;
        }
        
        public boolean ToggleSpatialFilter() throws MgException
        {
            boolean result = true;
            MgResourceService resourceService = (MgResourceService)this.site.CreateService(MgServiceType.ResourceService);
            MgMap map = new MgMap();
            map.Open(resourceService, this.args.get("MAPNAME"));

            MgLayerCollection layers = map.GetLayers();
            if(layers.Contains("_QuerySpatialFilter"))
            {
                MgLayer layer = (MgLayer) layers.GetItem("_QuerySpatialFilter");
                if(this.args.get("VISIBLE").equals("true"))
                    layer.SetVisible(true);
                else
                    layer.SetVisible(false);

                map.Save(resourceService);
            }

            return result;
        }

        public boolean ShowSpatialFilter() throws MgException, FileNotFoundException, IOException
        {
            boolean result = true;
            MgResourceIdentifier sdfResId = new MgResourceIdentifier("Session:"+this.args.get("SESSION")+"//Filter.FeatureSource");

            MgResourceService resourceService = (MgResourceService)this.site.CreateService(MgServiceType.ResourceService);
            MgFeatureService featureService = (MgFeatureService)this.site.CreateService(MgServiceType.FeatureService);

            MgFeatureCommandCollection updateCommands = new MgFeatureCommandCollection();

            MgMap map = new MgMap();
            map.Open(resourceService, this.args.get("MAPNAME"));

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

                MgResourceIdentifier layerResId = new MgResourceIdentifier("Session:"+this.args.get("SESSION")+"//Filter.LayerDefinition");

                String lineSep = System.getProperty("line.separator");
                BufferedReader buffReader = new BufferedReader(new FileReader(GetQueryXmlTemplatePath()));
                String nextLine = "";
                StringBuffer strBuffer = new StringBuffer();
                while((nextLine = buffReader.readLine()) != null)
                {
                    strBuffer.append(nextLine);
                    strBuffer.append(lineSep);
                }
                String layerDefinition = strBuffer.toString();
                layerDefinition = layerDefinition.replaceAll("%s", sdfResId.ToString());

                MgByteSource byteSource = new MgByteSource(layerDefinition.getBytes(), layerDefinition.length());
                resourceService.SetResource(layerResId, byteSource.GetReader(), null);

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

            MgPolygon polygon = this.CreatePolygonFromGeomText(this.args.get("GEOMTEXT"));
            MgAgfReaderWriter agfWriter = new MgAgfReaderWriter();
            MgByteReader byteReader = agfWriter.Write(polygon);

            MgPropertyCollection propertyValues = new MgPropertyCollection();
            propertyValues.Add(new MgGeometryProperty("Geometry", byteReader));

            updateCommands.Add(new MgInsertFeatures("Filter", propertyValues));

            featureService.UpdateFeatures(sdfResId, updateCommands, false);

            return result;
        }
            
        public ArrayList<Feature> Execute() throws MgException
        {
            ArrayList<Feature> result = new ArrayList<Feature>();
            MgResourceService resourceService = (MgResourceService)this.site.CreateService(MgServiceType.ResourceService);

            MgMap map = new MgMap();
            map.Open(resourceService, this.args.get("MAPNAME"));
            MgLayer layer = (MgLayer) map.GetLayers().GetItem(this.args.get("LAYERNAME"));

            MgFeatureService featureService = (MgFeatureService)this.site.CreateService(MgServiceType.FeatureService);
            MgResourceIdentifier resId = new MgResourceIdentifier(layer.GetFeatureSourceId());
            String featureClass = layer.GetFeatureClassName();
            String featureGeometry = layer.GetFeatureGeometryName();

            // Initialize the coordinate system transform

            String[] schemaClass = layer.GetFeatureClassName().split(":");
            MgClassDefinition classDef = featureService.GetClassDefinition(resId, schemaClass[0], schemaClass[1]);
            MgGeometricPropertyDefinition geomProp = (MgGeometricPropertyDefinition) classDef.GetProperties().GetItem(featureGeometry);
            String spatialContext = geomProp.GetSpatialContextAssociation();

            MgCoordinateSystemTransform csTransform = null;
            MgCoordinateSystemTransform csInverseTransform = null;
            MgCoordinateSystemFactory coordSysFactory = new MgCoordinateSystemFactory();

            MgSpatialContextReader scReader = featureService.GetSpatialContexts(resId, false);
            while(scReader.ReadNext() && csTransform==null)
            {
                if(spatialContext.equals(scReader.GetName()))
                {
                    MgCoordinateSystem source = coordSysFactory.Create(scReader.GetCoordinateSystemWkt());
                    MgCoordinateSystem target = coordSysFactory.Create(map.GetMapSRS());
                    csTransform = coordSysFactory.GetTransform(source, target);
                    csInverseTransform = coordSysFactory.GetTransform(target, source);
                }
            }
            scReader.Close();

            // Execute the query

            int queryMax = Integer.parseInt(this.args.get("QUERYMAX").trim());
            MgFeatureQueryOptions queryOptions = new MgFeatureQueryOptions();

            if(this.args.get("USEPROPERTYFILTER").equals("true"))
            {
                String propertyFilter = this.args.get("PROPERTYNAME");
                int operator = Integer.valueOf(this.args.get("OPERATOR"));
                int count = 0;
                if(this.args.get("ISSTRING").equals("true"))
                {
                    propertyFilter = propertyFilter + this.strExpressions[operator].replaceAll("%s", this.args.get("VALUE"));
                }
                else
                {
                    propertyFilter = propertyFilter + this.numExpressions[operator].replaceAll("%s", this.args.get("VALUE"));
                }
                queryOptions.SetFilter(propertyFilter);
            }

            if(this.args.get("USESPATIALFILTER").equals("true"))
            {
                MgPolygon polygon = this.CreatePolygonFromGeomText(this.args.get("GEOMTEXT"));
                MgGeometry geometry = (MgGeometry) polygon.Transform(csInverseTransform);
                queryOptions.SetSpatialFilter(featureGeometry, geometry, MgFeatureSpatialOperations.Intersects);
            }

            int count = 0;
            MgAgfReaderWriter geometryReaderWriter = new MgAgfReaderWriter();
            MgFeatureReader featureReader = featureService.SelectFeatures(resId, layer.GetFeatureClassName(), queryOptions);
            String displayValue = null;
            while(featureReader.ReadNext() && (queryMax <= 0 || count < queryMax))
            {
                MgByteReader byteReader = featureReader.GetGeometry(featureGeometry);
                MgPoint centerPoint = null;
                try 
                {
                    MgGeometry geometry = geometryReaderWriter.Read(byteReader);
                    centerPoint = geometry.GetCentroid();
                    centerPoint = (MgPoint) centerPoint.Transform(csTransform);
                }
                catch (MgException ex) //Maybe because of bad geometry
                {
                    centerPoint = null;
                }

                Map idList = new HashMap(this.GetFeatureIdList(featureReader));
                
                int propertyType = featureReader.GetPropertyType(this.args.get("OUTPUTPROPERTY"));
                switch(propertyType)
                {
                    case MgPropertyType.Boolean :
                        displayValue = String.valueOf(featureReader.GetBoolean(this.args.get("OUTPUTPROPERTY")));
                        break;
                    case MgPropertyType.Byte :
                        displayValue = String.valueOf(featureReader.GetByte(this.args.get("OUTPUTPROPERTY")));
                        break;
                    case MgPropertyType.Single :
                        displayValue = String.valueOf(featureReader.GetSingle(this.args.get("OUTPUTPROPERTY")));
                        break;
                    case MgPropertyType.Double :
                        displayValue = String.valueOf(featureReader.GetDouble(this.args.get("OUTPUTPROPERTY")));
                        break;
                    case MgPropertyType.Int16 :
                        displayValue = String.valueOf(featureReader.GetInt16(this.args.get("OUTPUTPROPERTY")));
                        break;
                    case MgPropertyType.Int32 :
                        displayValue = String.valueOf(featureReader.GetInt32(this.args.get("OUTPUTPROPERTY")));
                        break;
                    case MgPropertyType.Int64 :
                        displayValue = String.valueOf(featureReader.GetInt64(this.args.get("OUTPUTPROPERTY")));
                        break;
                    case MgPropertyType.String :
                        displayValue = featureReader.GetString(this.args.get("OUTPUTPROPERTY"));
                        break;
                    case MgPropertyType.DateTime :
                    case MgPropertyType.Null :
                    case MgPropertyType.Blob :
                    case MgPropertyType.Clob :
                    case MgPropertyType.Feature :
                    case MgPropertyType.Geometry :
                    case MgPropertyType.Raster :
                        displayValue = "[unsupported data type]";
                        break;
                }
                result.add(new Feature(displayValue, centerPoint, idList));
                count++;
            }

            return result;
        }

        public String GetSelectionXML() throws MgException, JSONException
        {
            MgResourceService resourceService = (MgResourceService)this.site.CreateService(MgServiceType.ResourceService);
            MgFeatureService featureService = (MgFeatureService)this.site.CreateService(MgServiceType.FeatureService);

            MgMap map = new MgMap();
            map.Open(resourceService, this.args.get("MAPNAME"));
            MgLayer layer = (MgLayer) map.GetLayers().GetItem(this.args.get("LAYERNAME"));
            MgResourceIdentifier resId = new MgResourceIdentifier(layer.GetFeatureSourceId());
            String featureClass = layer.GetFeatureClassName();

            String[] schemaClass = layer.GetFeatureClassName().split(":");
            MgClassDefinition classDef = featureService.GetClassDefinition(resId, schemaClass[0], schemaClass[1]);

            MgPropertyCollection properties = new MgPropertyCollection();
            MgDataPropertyDefinition dataPropDef = null;
            JSONObject json = new JSONObject(this.args.get("IDLIST"));

            Iterator it = json.keys();
            while(it.hasNext())
            {
                String key = (String) it.next();
                Object value = json.get(key);
                dataPropDef = (MgDataPropertyDefinition) classDef.GetProperties().GetItem(key);
                switch(dataPropDef.GetDataType())
                {
                    case MgPropertyType.Boolean :
                        properties.Add(new MgBooleanProperty(key, Boolean.valueOf(value.toString())));
                        break;
                    case MgPropertyType.Byte :
                        properties.Add(new MgByteProperty(key, Byte.valueOf(value.toString())));
                        break;
                    case MgPropertyType.Single :
                        properties.Add(new MgSingleProperty(key, Float.valueOf(value.toString())));
                        break;
                    case MgPropertyType.Double :
                        properties.Add(new MgDoubleProperty(key, Double.valueOf(value.toString())));
                        break;
                    case MgPropertyType.Int16 :
                        properties.Add(new MgInt16Property(key, Short.valueOf(value.toString())));
                        break;
                    case MgPropertyType.Int32 :
                        properties.Add(new MgInt32Property(key, Integer.valueOf(value.toString())));
                        break;
                    case MgPropertyType.Int64 :
                        properties.Add(new MgInt64Property(key, Long.valueOf(value.toString())));
                        break;
                    case MgPropertyType.String :
                        properties.Add(new MgStringProperty(key, value.toString()));
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

            MgSelection selection = new MgSelection(map);
            selection.AddFeatureIds(layer, featureClass, properties);

            return selection.ToXml();
        }
            
        private Map GetFeatureIdList(MgFeatureReader featureReader) throws MgException
        {
            MgClassDefinition classDef = featureReader.GetClassDefinition();
            MgPropertyDefinitionCollection idProps = classDef.GetIdentityProperties();
            Map idList = new HashMap();

            for(int i=0; i<idProps.GetCount(); i++)
            {
                MgDataPropertyDefinition idProp = (MgDataPropertyDefinition) idProps.GetItem(i);

                switch(idProp.GetDataType())
                {
                    case MgPropertyType.Boolean :
                        idList.put(idProp.GetName(), featureReader.GetBoolean(idProp.GetName()));
                        break;
                    case MgPropertyType.Byte :
                        idList.put(idProp.GetName(), featureReader.GetByte(idProp.GetName()));
                        break;
                    case MgPropertyType.Single :
                        idList.put(idProp.GetName(), featureReader.GetSingle(idProp.GetName()));
                        break;
                    case MgPropertyType.Double :
                        idList.put(idProp.GetName(), featureReader.GetDouble(idProp.GetName()));
                        break;
                    case MgPropertyType.Int16 :
                        idList.put(idProp.GetName(), featureReader.GetInt16(idProp.GetName()));
                        break;
                    case MgPropertyType.Int32 :
                        idList.put(idProp.GetName(), featureReader.GetInt32(idProp.GetName()));
                        break;
                    case MgPropertyType.Int64 :
                        idList.put(idProp.GetName(), featureReader.GetInt64(idProp.GetName()));
                        break;
                    case MgPropertyType.String :
                        idList.put(idProp.GetName(), featureReader.GetString(idProp.GetName()));
                        break;
                    case MgPropertyType.DateTime :
                        idList.put(idProp.GetName(), featureReader.GetDateTime(idProp.GetName()));
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
            
        private boolean IsValidDataType(int type)
        {
            boolean valid = true;

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
            
        private MgFeatureSchema CreateFilterSchema() throws MgException
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
        
        private MgPolygon CreatePolygonFromGeomText(String geomText) throws MgException
        {
            MgGeometryFactory geometryFactory = new MgGeometryFactory();

            String[] vertices = geomText.split(",");
            int count = Integer.valueOf(vertices[0]);

            MgCoordinateCollection coords = new MgCoordinateCollection();
            for(int i=0; i<count; i++)
            {
                MgCoordinate coord = geometryFactory.CreateCoordinateXY(Double.valueOf(vertices[(i*2)+1]), Double.valueOf(vertices[(i*2)+2]));
                coords.Add(coord);
            }

            MgLinearRing linearRing = geometryFactory.CreateLinearRing(coords);
            MgPolygon polygon = geometryFactory.CreatePolygon(linearRing, null);

            return polygon;
        }

    }

%>

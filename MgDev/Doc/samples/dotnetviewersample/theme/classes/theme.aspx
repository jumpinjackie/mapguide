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
<%@ Import Namespace="System.Xml" %>
<%@ Import Namespace="OSGeo.MapGuide" %>
<%@ Import Namespace="Jayrock.Json" %>
<%@ Import Namespace="Jayrock.Json.Conversion" %>

<!-- #Include File="property.aspx" -->
<!-- #Include File="layerinfo.aspx" -->
<!-- #Include File="../../utilityfunctions.aspx -->

<script language="c#" runat="server">
    public class Theme
    {
        NameValueCollection args = null;
        private MgSiteConnection site = null;
        private String[] distNameArray = null;
        private String[] distValueArray = null;
        
        public Theme(NameValueCollection incomingArgs)
        {
            this.args = incomingArgs;
            this.site = new MgSiteConnection();
            this.site.Open(new MgUserInformation(GetParameter(this.args, "SESSION")));
            this.distNameArray = new String[] {"Individual", "Equal", "Standard Deviation", "Quantile", "Jenks (Natural Breaks)"};
            this.distValueArray = new String[] {"INDIV_DIST", "EQUAL_DIST", "STDEV_DIST", "QUANT_DIST", "JENK_DIST"};
        }
        
        public String getNameArray()
        {
            String jsonDistNames = JsonConvert.ExportToString(this.distNameArray);
            return jsonDistNames;
        }

        public String getValueArray()
        {
            String jsonDistValues = JsonConvert.ExportToString(this.distValueArray);
            return jsonDistValues;
        }
        
        public ArrayList GetMapLayerNames()
        {
            MgResourceService resourceService = (MgResourceService)this.site.CreateService(MgServiceType.ResourceService);
            MgFeatureService featureService = (MgFeatureService)this.site.CreateService(MgServiceType.FeatureService);
            
            MgMap map = new MgMap();
            map.Open(resourceService, GetParameter(this.args, "MAPNAME"));
            MgLayerCollection layers = map.GetLayers();
            ArrayList layerNames = new ArrayList();

            for(int i=0; i<layers.GetCount(); i++)
            {
            MgLayer layer = (MgLayer) layers.GetItem(i);
                if(!layer.GetName().StartsWith("_") && !layer.GetFeatureSourceId().ToUpper().StartsWith("SESSION"))
                {
                    MgResourceIdentifier resId = new MgResourceIdentifier(layer.GetFeatureSourceId());
                    String[] schemaClass = layer.GetFeatureClassName().Split(new Char[]{':'});

                    MgClassDefinition classDef = featureService.GetClassDefinition(resId, schemaClass[0], schemaClass[1]);
                    MgPropertyDefinition propDef = classDef.GetProperties().GetItem(layer.GetFeatureGeometryName());
                
                    if(propDef.GetPropertyType() == MgFeaturePropertyType.GeometricProperty)
                    {
                        MgGeometricPropertyDefinition geomPropDef= (MgGeometricPropertyDefinition) propDef;

                        if(geomPropDef.GetGeometryTypes() == MgFeatureGeometricType.Surface)
                        {
                            layerNames.Add(layer.GetLegendLabel());
                        }
                    }
                }
            }
            layerNames.Sort();

            return layerNames;
        }
        
        public LayerInfo GetLayerInfo()
        {
            ArrayList properties = new ArrayList();
            ArrayList scaleRanges = new ArrayList();
            
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
                    ArrayList distroTypes = new ArrayList(this.GetDistributionsForDataType(dataType));
                    if(distroTypes.Count > 0)
                    {
                        properties.Add(new Property(propertyDataDef.GetName(), dataType, distroTypes));
                    }
                }
            }
            
            // Next get a list of all of the layers scale ranges.
            
            MgResourceIdentifier layerDefResId = layer.GetLayerDefinition();
            MgByteReader byteReader = resourceService.GetResourceContent(layerDefResId);
            
            XmlDocument doc = new XmlDocument();
            doc.LoadXml(byteReader.ToString());
            XmlNodeList nodeList = doc.GetElementsByTagName("VectorScaleRange");
            
            foreach(XmlElement node in nodeList)
            {
                String range = null;
                
                XmlNodeList minNodeList = node.GetElementsByTagName("MinScale");
                if(minNodeList.Count > 0)
                {
                    range = minNodeList.Item(0).FirstChild.Value;
                }
                else
                {
                    range = "0";
                }
                
                XmlNodeList maxNodeList = node.GetElementsByTagName("MaxScale");
                if(maxNodeList.Count > 0)
                {
                    range = range + " - " + maxNodeList.Item(0).FirstChild.Value;
                }
                else
                {
                    range = range + " - Infinity";
                }
                
                scaleRanges.Add(range);
            }
            
            return new LayerInfo(properties, scaleRanges);
        }
        
        public ArrayList GetPropertyMinMaxCount()
        {
            ArrayList propertyMinMaxCount = new ArrayList();
            
            MgResourceService resourceService = (MgResourceService)this.site.CreateService(MgServiceType.ResourceService);
            
            MgMap map = new MgMap();
            map.Open(resourceService, GetParameter(this.args, "MAPNAME"));
            MgLayer layer = (MgLayer) map.GetLayers().GetItem(GetParameter(this.args, "LAYERNAME"));

            MgFeatureService featureService = (MgFeatureService)this.site.CreateService(MgServiceType.FeatureService);
            MgResourceIdentifier resId = new MgResourceIdentifier(layer.GetFeatureSourceId());
            
            String minValue = null;
            String maxValue = null;
            int count = 0;
            
            MgFeatureQueryOptions queryOptions = new MgFeatureQueryOptions();
            queryOptions.AddFeatureProperty(GetParameter(this.args, "PROPERTYNAME"));
            
            MgFeatureReader featureReader = featureService.SelectFeatures(resId, layer.GetFeatureClassName(), queryOptions);
            while(featureReader.ReadNext())
            {
                String value = GetFeaturePropertyValue(featureReader, GetParameter(this.args, "PROPERTYNAME"));
                int propertyType = featureReader.GetPropertyType(GetParameter(this.args, "PROPERTYNAME"));
                if(count == 0)
                {
                    maxValue = value;
                    minValue = value;
                }
                switch(propertyType)
                {
                    case MgPropertyType.String :
                        if(value.Length > 0)
                        {
                            if(value.CompareTo(maxValue) > 0)
                                maxValue = value;
                            if(value.CompareTo(minValue) < 0)
                                minValue = value;
                        }
                        break;
                    case MgPropertyType.Byte :
                    case MgPropertyType.Int16 :
                    case MgPropertyType.Int32 :
                    case MgPropertyType.Int64 :
                        if(value.Length > 0)
                        {
                            if(Int64.Parse(value) > Int64.Parse(maxValue))
                                maxValue = value;
                            if(Int64.Parse(value) < Int64.Parse(minValue))
                                minValue = value;
                        }
                        break;
                    case MgPropertyType.Single :
                    case MgPropertyType.Double :
                        if(value != null)
                        {
                            if(Double.Parse(value) > Double.Parse(maxValue))
                                maxValue = value;
                            if(Double.Parse(value) < Double.Parse(minValue))
                                minValue = value;
                        }
                        count++;
                        break;
                    case MgPropertyType.Boolean :
                    case MgPropertyType.DateTime :
                    case MgPropertyType.Null :
                    case MgPropertyType.Blob :
                    case MgPropertyType.Clob :
                    case MgPropertyType.Feature :
                    case MgPropertyType.Geometry :
                        break;
                }
                count++;
            }
            featureReader.Close();
            
            propertyMinMaxCount.Add(minValue.ToString());
            propertyMinMaxCount.Add(maxValue.ToString());
            propertyMinMaxCount.Add(count.ToString());
            
            return propertyMinMaxCount;
        }
        
        public String ApplyTheme()
        {
            MgResourceService resourceService = (MgResourceService)this.site.CreateService(MgServiceType.ResourceService);
            MgFeatureService featureService = (MgFeatureService)this.site.CreateService(MgServiceType.FeatureService);

            MgMap map = new MgMap();
            map.Open(resourceService, GetParameter(this.args, "MAPNAME"));
            MgLayerCollection layers = map.GetLayers();
            MgLayer layer = (MgLayer) layers.GetItem(GetParameter(this.args, "LAYERNAME"));
            
            MgResourceIdentifier resId = new MgResourceIdentifier(layer.GetFeatureSourceId());
            MgResourceIdentifier layerDefResId = layer.GetLayerDefinition();
            MgByteReader byteReader = resourceService.GetResourceContent(layerDefResId);
            
            // Load the Layer Definition and Navigate to the specified <VectorScaleRange>
            
            XmlDocument doc = new XmlDocument();
            String xmlLayerDef = byteReader.ToString();
            doc.LoadXml(xmlLayerDef);
            XmlNodeList nodeList = doc.GetElementsByTagName("VectorScaleRange");
                
            XmlElement vectorScaleRangecElement = (XmlElement) nodeList.Item(Int32.Parse(GetParameter(this.args, "SCALERANGEINDEX")));
            XmlElement areaTypeStyle = (XmlElement) vectorScaleRangecElement.GetElementsByTagName("AreaTypeStyle").Item(0);
            
            // Remove any existing <AreaRule> elements.
            
            XmlNodeList areaRuleList = areaTypeStyle.GetElementsByTagName("AreaRule");
            int count = areaRuleList.Count;
            for(int i = 0; i < count; i++)
            {
                //The areaRuleList shrinks as we remove items, so always
                //remove the first item (don't use the index i)
                areaTypeStyle.RemoveChild(areaRuleList.Item(0));
            }
            
            // Now create the new <AreaRule> elements.
            
            String areaRuleTemplate = File.ReadAllText(GetThemeXmlTemplatePath());
            MgFeatureAggregateOptions aggregateOptions = new MgFeatureAggregateOptions();
            
            String value = null;
            String filterText = null;
            String areaRuleXML = null;
            XmlDocument areaDoc = null;
            XmlNode areaNode = null;
            double portion = 0.0;
            int numRules = Int32.Parse(GetParameter(this.args, "NUMRULES"));
            double increment = (numRules > 1)? 1.0 / (numRules - 1) : 1.0;
            
            if("INDIV_DIST".Equals(GetParameter(this.args, "DISTRO")))
            {
                aggregateOptions.AddFeatureProperty(GetParameter(this.args, "PROPERTYNAME"));
                aggregateOptions.SelectDistinct(true);
                
                MgDataReader dataReader = featureService.SelectAggregate(resId, layer.GetFeatureClassName(), aggregateOptions);
                while(dataReader.ReadNext())
                {
                    value = GetFeaturePropertyValue(dataReader, GetParameter(this.args, "PROPERTYNAME"));
                    
                    filterText = "&quot;" + GetParameter(this.args, "PROPERTYNAME") + "&quot; = ";
                    if(Int32.Parse(GetParameter(this.args, "DATATYPE")) == MgPropertyType.String)
                        filterText = filterText + "'" + value + "'";
                    else
                        filterText = filterText + value;

                    areaRuleXML = String.Format(areaRuleTemplate,
                                                GetParameter(this.args, "PROPERTYNAME") + ":" + value,
                                                filterText,
                                                this.InterpolateColor(portion, GetParameter(this.args, "FILLFROM"), GetParameter(this.args, "FILLTO"), Int32.Parse(GetParameter(this.args, "FILLTRANS"))),
                                                this.InterpolateColor(portion, GetParameter(this.args, "LINEFROM"), GetParameter(this.args, "LINETO"), 0));
                    areaDoc = new XmlDocument();
                    areaDoc.LoadXml(areaRuleXML);
                    areaNode = doc.ImportNode(areaDoc.DocumentElement, true);
                    areaTypeStyle.AppendChild(areaNode);
                    
                    portion = portion + increment;
                }
                dataReader.Close();
            }
            else
            {
                ArrayList values = new ArrayList();
                
                aggregateOptions.AddComputedProperty("THEME_VALUE",
                    GetParameter(this.args, "DISTRO") + "(\"" + GetParameter(this.args, "PROPERTYNAME") + "\"," + GetParameter(this.args, "NUMRULES") + "," + GetParameter(this.args, "MINVALUE") + "," + GetParameter(this.args, "MAXVALUE") + ")");
                MgDataReader dataReader = featureService.SelectAggregate(resId, layer.GetFeatureClassName(), aggregateOptions);
                while(dataReader.ReadNext())
                {
                    value = GetFeaturePropertyValue(dataReader, "THEME_VALUE");
                    values.Add(value);
                }
                dataReader.Close();
                
                for(int i=0; i<values.Count - 1; i++)
                {
                    filterText = "&quot;" + GetParameter(this.args, "PROPERTYNAME") + "&quot; &gt;= " + values[i] + " AND &quot;" + GetParameter(this.args, "PROPERTYNAME");
                    if(i == values.Count - 1)
                        filterText = filterText + "&quot; &lt;= " + values[i+1];
                    else
                        filterText = filterText + "&quot; &lt; " + values[i+1];
                    
                    areaRuleXML = String.Format(areaRuleTemplate,
                                                GetParameter(this.args, "PROPERTYNAME") + ":" + values[i] + " - " + values[i+1],
                                                filterText,
                                                this.InterpolateColor(portion, GetParameter(this.args, "FILLFROM"), GetParameter(this.args, "FILLTO"), Int32.Parse(GetParameter(this.args, "FILLTRANS"))),
                                                this.InterpolateColor(portion, GetParameter(this.args, "LINEFROM"), GetParameter(this.args, "LINETO"), 0));
                    
                    areaDoc = new XmlDocument();
                    areaDoc.LoadXml(areaRuleXML); 
                    areaNode = doc.ImportNode(areaDoc.DocumentElement, true);
                    areaTypeStyle.AppendChild(areaNode);
                                
                    portion = portion + increment;
                }
            }
            
            // Now save our new layer definition to the session and add it to the map.
            
            String xmlString = doc.DocumentElement.OuterXml;
            String uniqueName = this.MakeUniqueLayerName(map, GetParameter(this.args, "LAYERNAME"), GetParameter(this.args, "THEMENAME"));
            String legendLabel = layer.GetLegendLabel();
            if(GetParameter(this.args, "THEMENAME").Length > 0)
                legendLabel = legendLabel + " (" + GetParameter(this.args, "THEMENAME") + ")";
            
            MgResourceIdentifier layerResId = new MgResourceIdentifier("Session:" + GetParameter(this.args, "SESSION") + "//" + uniqueName + ".LayerDefinition");
            resourceService.SetResource(layerResId, new MgByteReader(xmlString, "text/xml"), null);
            
            MgLayer newLayer = new MgLayer(layerResId, resourceService);
            newLayer.SetName(uniqueName);
            newLayer.SetLegendLabel(legendLabel);
            newLayer.SetDisplayInLegend(layer.GetDisplayInLegend());
            newLayer.SetVisible(true);
            newLayer.SetSelectable(layer.GetSelectable());
            layers.Insert(layers.IndexOf(layer), newLayer);
            
            map.Save(resourceService);

            return uniqueName;
        }
        
        private String InterpolateColor(double portion, String startColor, String endColor, int percentTransparent)
        {
            int alpha = (int)(255*(100.0 - percentTransparent)/100.0);
            String result = "";
            if(startColor.Equals(endColor))
            {
                result = String.Format("{0:X2}{1}", alpha, startColor);
            }
            else
            {
                int red = this.CalculateRGB(portion, startColor.Substring(0,2), endColor.Substring(0,2));
                int green = this.CalculateRGB(portion, startColor.Substring(2,2), endColor.Substring(2,2));
                int blue = this.CalculateRGB(portion, startColor.Substring(4,2), endColor.Substring(4,2));
                result = String.Format("{0:X2}{1:X2}{2:X2}{3:X2}", alpha, red, green, blue);           
            }
            return result;
        }
        
        private int CalculateRGB(double portion, String startRGB, String endRGB)
        {
            double result = Int32.Parse(startRGB, NumberStyles.HexNumber) + portion * (Int32.Parse(endRGB, NumberStyles.HexNumber) - Int32.Parse(startRGB, NumberStyles.HexNumber));
            return (int) result;
        }
        
        private ArrayList GetDistributionsForDataType(int type)
        {
            ArrayList distroTypes = new ArrayList();
            
            switch(type)
            {
                case MgPropertyType.String :
                    distroTypes.Add(0);
                    break;
                case MgPropertyType.Byte :
                case MgPropertyType.Int16 :
                case MgPropertyType.Int32 :
                case MgPropertyType.Int64 :
                    distroTypes.Add(0);
                    distroTypes.Add(1);
                    distroTypes.Add(2);
                    distroTypes.Add(3);
                    distroTypes.Add(4);
                    break;
                case MgPropertyType.Single :
                case MgPropertyType.Double :
                    distroTypes.Add(1);
                    distroTypes.Add(2);
                    distroTypes.Add(3);
                    distroTypes.Add(4);
                    break;
                case MgPropertyType.Boolean :
                case MgPropertyType.DateTime :
                case MgPropertyType.Blob :
                case MgPropertyType.Clob :
                case MgPropertyType.Feature :
                case MgPropertyType.Geometry :
                case MgPropertyType.Null :
                    break;
            }
            return distroTypes;
        }
        
        private String MakeUniqueLayerName(MgMap map, String layerName, String themeName) 
        {
            String desiredName = "_" + layerName + themeName;
            String uniqueName = desiredName;
            int index = 1;
            
            while(map.GetLayers().Contains(uniqueName))
            {
                uniqueName = desiredName + index.ToString();
                index++;
            }
            return uniqueName;
        }
               
        String GetThemeXmlTemplatePath()
        {
            String xmlTemplatePath = GetBasePath() + "theme\\templates\\arearuletemplate.xml";
            return xmlTemplatePath;
        }
    }
</script>

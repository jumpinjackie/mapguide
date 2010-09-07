<%@ Page Language="C#" %>
<%@ Import Namespace="System" %>
<%@ Import Namespace="System.IO" %>
<%@ Import Namespace="System.Collections.Generic" %>
<%@ Import Namespace="System.Web" %>
<%@ Import Namespace="System.Xml" %>
<%@ Import Namespace="System.Globalization" %>
<%@ Import Namespace="OSGeo.MapGuide" %>
<!-- #include File="common.aspx" -->

<script runat="server">
    
    class SelectionSet
    {
        private Dictionary<string, List<Feature>> _layers;

        public SelectionSet()
        {
            _layers = new Dictionary<String, List<Feature>>();
        }

        public void AddFeature(Feature feat)
        {
            if (!_layers.ContainsKey(feat.LayerName))
                _layers[feat.LayerName] = new List<Feature>();

            _layers[feat.LayerName].Add(feat);
        }

        public String[] GetLayers()
        {
            return new List<String>(_layers.Keys).ToArray();
        }

        public Feature[] GetFeatures(String layerName)
        {
            if (_layers.ContainsKey(layerName))
                return _layers[layerName].ToArray();

            return null;
        }
    }

    class ZoomPoint
    {
        public double X;
        public double Y;
    }

    class FeatureProperty
    {
        public String Name;
        public String Value;
    }

    class Feature
    {
        public String LayerName;
        public ZoomPoint Zoom;

        private Dictionary<string, FeatureProperty> _properties;

        public Feature(string layerName)
        {
            this.LayerName = layerName;
            _properties = new Dictionary<string, FeatureProperty>();
        }

        public void AddProperty(FeatureProperty prop)
        {
            _properties[prop.Name] = prop;
        }

        public FeatureProperty[] GetProperties()
        {
            return new List<FeatureProperty>(_properties.Values).ToArray();
        }
    }
    
    String mapName;
    String sessionId;
    String locale;
    CultureInfo culture;

    static NameValueCollection GetLayerPropertyMappings(MgResourceService resSvc, MgLayerBase layer)
    {
        NameValueCollection mappings = new NameValueCollection();
        MgByteReader content = resSvc.GetResourceContent(layer.GetLayerDefinition());
        XmlDocument doc = new XmlDocument();
        doc.LoadXml(content.ToString());
        XmlNodeList propNodes = doc.SelectNodes("//LayerDefinition/VectorLayerDefinition/PropertyMapping");
        foreach (XmlNode propNode in propNodes)
        {
            String name = propNode["Name"].InnerText;
            String value = propNode["Value"].InnerText;

            mappings[name] = value;
        }
        return mappings;
    }
    
    String GetPropertyValueFromFeatureReader(MgFeatureReader reader, MgAgfReaderWriter agfRw, int propType, String propName)
    {
        String value = "";
        switch(propType)
        {
            case MgPropertyType.Boolean:
                value = String.Format(culture, "{0}", reader.GetBoolean(propName));
                break;
            case MgPropertyType.Byte:
                value = String.Format(culture, "{0:d}", reader.GetByte(propName));
                break; 
            case MgPropertyType.DateTime:
                value = GetDateTimeString(reader.GetDateTime(propName)); // yyyy-mm-dd is enforced regardless of locale
                break;
            case MgPropertyType.Single:
                value = String.Format(culture, "{0:f}", reader.GetSingle(propName));
                break;
            case MgPropertyType.Double:
                value = String.Format(culture, "{0:f}", reader.GetDouble(propName));
                break;
            case MgPropertyType.Int16:
                value = String.Format(culture, "{0:d}", reader.GetInt16(propName));
                break;
            case MgPropertyType.Int32:
                value = String.Format(culture, "{0:d}", reader.GetInt32(propName));
                break;
            case MgPropertyType.Int64:
                value = String.Format(culture, "{0:d}", reader.GetInt64(propName));
                break;
            case MgPropertyType.String:
                value = JsonEscape(reader.GetString(propName)); //String content is arbitrary
                break;
            default: //NOT PRESENTABLE IN PROPERTY GRID
                value = "";
                break;
        }
        return value;   
    }

    static String GetDateTimeString(MgDateTime value)
    {
        return String.Format("{0}-{1}-{2}", value.Year, value.Month, value.Day);
    }
    
    void GetParameters(NameValueCollection param)
    {
        mapName = param["MAPNAME"];
        sessionId = param["SESSION"];
        locale = param["LOCALE"];
    }

    void GetRequestParameters()
    {
        if (Request.RequestType == "POST")
            GetParameters(Request.Form);
        else
            GetParameters(Request.QueryString);
    }
    
    String JsonEscape(String str)
    {
        return EscapeForHtml(str).Replace("\\", "\\\\");
    }
    
    String JsonifyError(Exception ex)
    {
        return "{\"Error\":{\"Message\":\"" + JsonEscape(ex.Message) + "\",\"StackTrace\":\"" + JsonEscape(ex.StackTrace) + "\"}}";
    }

    static String GetJson(SelectionSet set)
    {
        /*
        A sample of the JSON output this method will produce:
        
        
        {
            "Layer1" : [ 
                { 
                    'values' { "name" : "name1" , "value" : "value1" }, 
                    'zoom' : { x: num1, y: num2 } 
                } , 
                ..,
                ..,
                ..,
            ],
            "Layer2" : [ 
                { 
                    'values' { "name" : "name2" , "value" : "value2" }, 
                    'zoom' : { x: num1, y: num2 } 
                } , 
                ..,
                ..,
                ..,
            ]
        }
        */
        
        if (set == null)
            return "";
        
        StringBuilder sb = new StringBuilder();
        //Begin selection set
        sb.Append("{");
        String[] layers = set.GetLayers();
        for (int i = 0; i < layers.Length; i++)
        {
            //Begin layer
            sb.Append("\"" + layers[i] + "\" : [");
            Feature[] features = set.GetFeatures(layers[i]);
            for (int j = 0; j < features.Length; j++)
            {
                Feature feat = features[j];
                //begin feature
                //begin feature properties
                sb.Append("{\"values\" : [");
                FeatureProperty[] properties = feat.GetProperties();
                for(int k = 0; k < properties.Length; k++)
                {
                    FeatureProperty fp = properties[k];
                    sb.Append("{\"name\" : \"" + fp.Name + "\", \"value\" : \"" + fp.Value + "\" }");
                    if (k != properties.Length - 1)
                        sb.Append(",");
                }
                //end feature properties
                //begin zoom
                sb.Append("], \"zoom\" : ");
                if (feat.Zoom == null)
                    sb.Append("null");
                else
                    sb.Append("{\"x\" : " + feat.Zoom.X + ", \"y\" : " + feat.Zoom.Y + "}");
                //end zoom
                //end feature
                sb.Append("}");
                if (j != features.Length - 1)
                    sb.Append(",");
            }
            //End Layer
            sb.Append("]");
            if (i != layers.Length - 1)
                sb.Append(",");
        }
        //End selection set
        sb.Append("}");
        return sb.ToString();
    }
    
</script>
<% 

    mapName = "";
    sessionId = "";
    locale = "";

    GetRequestParameters();
    
    if (String.IsNullOrEmpty(locale))
        locale = GetDefaultLocale();
        
    culture = CultureInfo.GetCultureInfo(locale);
    
    //HACK: The default locale (en) resolves to a neutral culture, .net forbids the use of 
    //neutral cultures for formatting purposes, so default to InvariantCulture if the resolved
    //culture is not neutral.
    if (culture.IsNeutralCulture)
        culture = CultureInfo.InvariantCulture; //We need a non-neutral culture

    try
    {
        MgUserInformation cred = new MgUserInformation(sessionId);
        cred.SetClientIp(GetClientIp(Request));
        cred.SetClientAgent(GetClientAgent());

        MgSiteConnection site = new MgSiteConnection();
        site.Open(cred);

        MgResourceService resSvc = (MgResourceService)site.CreateService(MgServiceType.ResourceService);
        
        MgMap map = new MgMap(site);
        map.Open(mapName);

        MgSelection selection = new MgSelection(map);
        selection.Open(resSvc, mapName);

        MgReadOnlyLayerCollection layers = selection.GetLayers();
        if (layers != null && layers.Count > 0)
        {
            int layerCount = layers.Count;
            MgAgfReaderWriter agfRW = new MgAgfReaderWriter();
            SelectionSet selectionSet = new SelectionSet();

            for (int i = 0; i < layerCount; i++)
            {
                MgLayerBase layer = layers[i];
                String layerName = layer.Name;

                MgResourceIdentifier fsId = new MgResourceIdentifier(layer.GetFeatureSourceId());
                String className = layer.GetFeatureClassName();
                String geomName = layer.GetFeatureGeometryName();

                MgFeatureQueryOptions query = new MgFeatureQueryOptions();
                NameValueCollection mappings = GetLayerPropertyMappings(resSvc, layer);

                foreach (String key in mappings.Keys)
                {
                    query.AddFeatureProperty(key);
                }

                query.AddFeatureProperty(geomName);
                String filter = selection.GenerateFilter(layer, className);
                query.SetFilter(filter);

                MgFeatureReader reader = layer.SelectFeatures(query);

                MgClassDefinition clsDef = reader.GetClassDefinition();
                MgPropertyDefinitionCollection props = clsDef.GetProperties();

                while (reader.ReadNext())
                {
                    Feature feat = new Feature(layerName);
                    ZoomPoint zoom = null;

                    for (int k = 0; k < props.Count; k++)
                    {
                        MgPropertyDefinition propDef = props[k];
                        String propName = propDef.Name;
                        int propType = reader.GetPropertyType(propName);

                        if (mappings[propName] != null || propType == MgPropertyType.Geometry)
                        {
                            String value = "";
                            if (!reader.IsNull(propName))
                            {
                                if (propName == geomName)
                                {
                                    MgByteReader agf = reader.GetGeometry(propName);
                                    MgGeometry geom = agfRW.Read(agf);
                                    MgCoordinate pt = geom.Centroid.Coordinate;

                                    zoom = new ZoomPoint();
                                    zoom.X = pt.X;
                                    zoom.Y = pt.Y;

                                    feat.Zoom = zoom;
                                }
                                else
                                {
                                    value = GetPropertyValueFromFeatureReader(reader, agfRW, propType, propName);
                                }

                                if (mappings[propName] != null)
                                {
                                    FeatureProperty fp = new FeatureProperty();
                                    fp.Name = mappings[propName];
                                    fp.Value = value;

                                    feat.AddProperty(fp);
                                }
                            }
                        }
                    }
                    selectionSet.AddFeature(feat);
                }
                reader.Close();
            }

            //Now output the selection set
            Response.AddHeader("Content-Type", "application/json");
            Response.AddHeader("X-JSON", "true");

            Response.Write(GetJson(selectionSet));
        }
    }
    catch (MgException ex)
    {
        Response.AddHeader("Content-Type", "application/json");
        Response.AddHeader("X-JSON", "true");
        Response.Write(JsonifyError(ex));
    }
    catch (Exception ex)
    { 
        Response.AddHeader("Content-Type", "application/json");
        Response.AddHeader("X-JSON", "true");
        Response.Write(JsonifyError(ex));
    }
    
%>
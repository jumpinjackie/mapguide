<%@ page contentType="text/html; charset=UTF-8" pageEncoding="UTF-8" %>
<%@ page import="org.osgeo.mapguide.*" %>
<%@ page import="java.util.*" %>
<%@ page import="java.lang.*" %>
<%@ page import="java.io.*" %>
<%@ page import="java.text.*" %>
<%@ page import="javax.xml.parsers.DocumentBuilder" %>
<%@ page import="javax.xml.parsers.DocumentBuilderFactory" %>
<%@ page import="org.w3c.dom.Document" %>
<%@ page import="org.w3c.dom.Element" %>
<%@ page import="org.w3c.dom.Node" %>
<%@ page import="org.w3c.dom.NodeList" %>
<%@ page import="javax.servlet.jsp.*" %>
<%@ page import="javax.servlet.http.*" %>
<%@ include file="common.jsp" %>
<%@ page isThreadSafe="false" %>
<%!

    String mapName;
    String sessionId;
    String localeCode;
    Locale locale;

    class SelectionSet
    {
        private HashMap<String, Vector<Feature>> _layers;

        public SelectionSet()
        {
            _layers = new HashMap<String, Vector<Feature>>();
        }

        public void addFeature(Feature feat)
        {
            if (!_layers.containsKey(feat.LayerName))
                _layers.put(feat.LayerName, new Vector<Feature>());

            _layers.get(feat.LayerName).add(feat);
        }

        public String[] getLayers()
        {
            String[] layers = new String[_layers.keySet().size()];
            _layers.keySet().toArray(layers);
            
            return layers;
        }

        public Feature[] getFeatures(String layerName)
        {
            if (_layers.containsKey(layerName))
            {
                Vector<Feature> layerFeatures = _layers.get(layerName);
                Feature[] feats = new Feature[layerFeatures.size()];
                layerFeatures.toArray(feats);
                return feats;
            }

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

        private HashMap<String, FeatureProperty> _properties;

        public Feature(String layerName)
        {
            this.LayerName = layerName;
            _properties = new HashMap<String, FeatureProperty>();
        }

        public void addProperty(FeatureProperty prop)
        {
            _properties.put(prop.Name, prop);
        }

        public FeatureProperty[] getProperties()
        {
            Collection<FeatureProperty> values = _properties.values();
            FeatureProperty[] props = new FeatureProperty[values.size()];
            values.toArray(props);
            return props;
        }
    }
    
    static String getTextValue(Element el, String tagName)
    {
        String textVal = null;
        NodeList nl = el.getElementsByTagName(tagName);
        if (nl != null && nl.getLength() > 0)
        {
            Element e = (Element)nl.item(0);
            textVal = e.getFirstChild().getNodeValue();
        }
        return textVal;
    }
    
    static HashMap<String, String> GetLayerPropertyMappings(MgResourceService resSvc, MgLayerBase layer) throws Exception
    {
        HashMap<String, String> mappings = new HashMap<String, String>();
        
        MgByteReader content = resSvc.GetResourceContent(layer.GetLayerDefinition());
        ByteArrayInputStream contentReader = new ByteArrayInputStream(content.ToString().getBytes("UTF-8"));
        
        DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();
        DocumentBuilder db = dbf.newDocumentBuilder();
        Document doc = db.parse(contentReader);
        
        doc.getDocumentElement().normalize();
        NodeList propNodes = doc.getElementsByTagName("PropertyMapping");
        
        for (int i = 0; i < propNodes.getLength(); i++)
        {
            Element propEl = (Element)propNodes.item(i);
            String name = getTextValue(propEl, "Name");
            String value = getTextValue(propEl, "Value");
            
            if (name != null && value != null)
                mappings.put(name, value);
        }
        
        return mappings;
    }
    
    String GetPropertyValueFromFeatureReader(MgFeatureReader reader, MgAgfReaderWriter agfRw, int propType, String propName) throws Exception
    {
        String value = "";
        switch(propType)
        {
            case MgPropertyType.Boolean:
                value = String.format(locale, "%s", reader.GetBoolean(propName));
                break;
            case MgPropertyType.Byte:
                value = String.format(locale, "%d", reader.GetByte(propName));
                break; 
            case MgPropertyType.DateTime:
                value = GetDateTimeString(reader.GetDateTime(propName)); // yyyy-mm-dd is enforced regardless of locale
                break;
            case MgPropertyType.Single:
                value = String.format(locale, "%f", reader.GetSingle(propName));
                break;
            case MgPropertyType.Double:
                value = String.format(locale, "%f", reader.GetDouble(propName));
                break;
            case MgPropertyType.Int16:
                value = String.format(locale, "%d", reader.GetInt16(propName));
                break;
            case MgPropertyType.Int32:
                value = String.format(locale, "%d", reader.GetInt32(propName));
                break;
            case MgPropertyType.Int64:
                value = String.format(locale, "%d", reader.GetInt64(propName));
                break;
            case MgPropertyType.String:
                value = JsonEscape(reader.GetString(propName)); // string content is arbitrary
                break;
            default: //NOT PRESENTABLE IN PROPERTY GRID
                value = "";
                break;
        }
        return value;   
    }

    static String GetDateTimeString(MgDateTime value) throws MgException
    {
        return value.GetYear() + "-" + value.GetMonth() + "-" + value.GetDay();
    }
    
    void GetParameters(HttpServletRequest request)
    {
        mapName = GetParameter(request, "MAPNAME");
        sessionId = GetParameter(request, "SESSION");
        localeCode = GetParameter(request, "LOCALE");
    }
    
    String JsonEscape(String str)
    {
        return EscapeForHtml(str).replace("\\", "\\\\");
    }
    
    String JsonifyError(Exception ex)
    {
        if (ex == null)
            return "";
        /*
        {
            "Error" : {
                "Message" : <exception-message>,
                "StackTrace" : <exception-stack-trace>
            }
        }
        */
        
        StringBuffer sb = new StringBuffer();
        //Use exception message or type name if no message found
        String msg = ex.getMessage();
        if (msg == null || msg.length() == 0)
        {
            msg = MgLocalizer.GetString("SERVERERROR", localeCode);
        }
        //Begin response
        sb.append("{\"Error\":{");
        //Exception message
        sb.append("\"Message\":\"" + JsonEscape(msg) + "\",");
        StringBuffer strace = new StringBuffer();
        StackTraceElement[] st = ex.getStackTrace();
        for (int i = 0; i < st.length; i++)
        {
            strace.append(st[i].getClassName() + "." + st[i].getMethodName() + "(" + st[i].getLineNumber() + ")\\n");
        }
        sb.append("\"StackTrace\":\"" + JsonEscape(strace.toString()) + "\"");
        //End response
        sb.append("}}");
        return sb.toString();
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
        
        StringBuffer sb = new StringBuffer();
        //Begin selection set
        sb.append("{");
        String[] layers = set.getLayers();
        for (int i = 0; i < layers.length; i++)
        {
            //Begin layer
            sb.append("\"" + layers[i] + "\" : [");
            Feature[] features = set.getFeatures(layers[i]);
            for (int j = 0; j < features.length; j++)
            {
                Feature feat = features[j];
                //begin feature
                //begin feature properties
                sb.append("{\"values\" : [");
                FeatureProperty[] properties = feat.getProperties();
                for(int k = 0; k < properties.length; k++)
                {
                    FeatureProperty fp = properties[k];
                    sb.append("{\"name\" : \"" + fp.Name + "\", \"value\" : \"" + fp.Value + "\" }");
                    if (k != properties.length - 1)
                        sb.append(",");
                }
                //end feature properties
                //begin zoom
                sb.append("], \"zoom\" : ");
                if (feat.Zoom == null)
                    sb.append("null");
                else
                    sb.append("{\"x\" : " + feat.Zoom.X + ", \"y\" : " + feat.Zoom.Y + "}");
                //end zoom
                //end feature
                sb.append("}");
                if (j != features.length - 1)
                    sb.append(",");
            }
            //End Layer
            sb.append("]");
            if (i != layers.length - 1)
                sb.append(",");
        }
        //End selection set
        sb.append("}");
        return sb.toString();
    }
%>
<%
    mapName = "";
    sessionId = "";
    localeCode = "";

    GetParameters(request);
    
    if (null == localeCode || localeCode.length() == 0)
        localeCode = GetDefaultLocale();
        
    locale = new Locale(localeCode);

    try
    {
        MgUserInformation cred = new MgUserInformation(sessionId);
        cred.SetClientIp(GetClientIp(request));
        cred.SetClientAgent(GetClientAgent());

        MgSiteConnection site = new MgSiteConnection();
        site.Open(cred);

        MgResourceService resSvc = (MgResourceService)site.CreateService(MgServiceType.ResourceService);
        
        MgMap map = new MgMap(site);
        map.Open(mapName);

        MgSelection selection = new MgSelection(map);
        selection.Open(resSvc, mapName);

        MgReadOnlyLayerCollection layers = selection.GetLayers();
        if (layers != null && layers.GetCount() > 0)
        {
            int layerCount = layers.GetCount();
            MgAgfReaderWriter agfRW = new MgAgfReaderWriter();
            SelectionSet selectionSet = new SelectionSet();

            for (int i = 0; i < layerCount; i++)
            {
                MgLayerBase layer = layers.GetItem(i);
                String layerName = layer.GetName();

                MgResourceIdentifier fsId = new MgResourceIdentifier(layer.GetFeatureSourceId());
                String className = layer.GetFeatureClassName();
                String geomName = layer.GetFeatureGeometryName();

                MgFeatureQueryOptions query = new MgFeatureQueryOptions();
                HashMap<String, String> mappings = GetLayerPropertyMappings(resSvc, layer);
                Set<String> propNames = mappings.keySet();

                for (String name : propNames)
                {
                    query.AddFeatureProperty(name);
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

                    for (int k = 0; k < props.GetCount(); k++)
                    {
                        MgPropertyDefinition propDef = props.GetItem(k);
                        String propName = propDef.GetName();
                        int propType = reader.GetPropertyType(propName);

                        if (mappings.get(propName) != null || propType == MgPropertyType.Geometry)
                        {
                            String value = "";
                            if (!reader.IsNull(propName))
                            {
                                if (propName.equals(geomName))
                                {
                                    MgByteReader agf = reader.GetGeometry(propName);
                                    MgGeometry geom = agfRW.Read(agf);
                                    MgCoordinate pt = geom.GetCentroid().GetCoordinate();

                                    zoom = new ZoomPoint();
                                    zoom.X = pt.GetX();
                                    zoom.Y = pt.GetY();

                                    feat.Zoom = zoom;
                                }
                                else
                                {
                                    value = GetPropertyValueFromFeatureReader(reader, agfRW, propType, propName);
                                }

                                if (mappings.get(propName) != null)
                                {
                                    FeatureProperty fp = new FeatureProperty();
                                    fp.Name = mappings.get(propName);
                                    fp.Value = value;

                                    feat.addProperty(fp);
                                }
                            }
                        }
                    }
                    selectionSet.addFeature(feat);
                }
                reader.Close();
            }

            //Now output the selection set
            response.addHeader("Content-Type", "application/json");
            response.addHeader("X-JSON", "true");

            response.getWriter().write(GetJson(selectionSet));
        }
    }
    catch (MgException ex)
    {
        response.addHeader("Content-Type", "application/json");
        response.addHeader("X-JSON", "true");
        response.getWriter().write(JsonifyError(ex));
    }
    catch (Exception ex)
    {
        response.addHeader("Content-Type", "application/json");
        response.addHeader("X-JSON", "true");
        response.getWriter().write(JsonifyError(ex));
    }
%>
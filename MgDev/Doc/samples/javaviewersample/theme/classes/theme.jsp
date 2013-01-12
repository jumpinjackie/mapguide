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
<%@ page import="javax.xml.parsers.*" %>
<%@ page import="javax.xml.transform.*" %>
<%@ page import="javax.xml.transform.dom.*" %>
<%@ page import="javax.xml.transform.stream.*" %>
<%@ page import="org.w3c.dom.*" %>
<%@ page import="org.xml.sax.*" %>
<%@ page import="org.json.*" %>
<%@ include file ="property.jsp" %>
<%@ include file ="layerinfo.jsp" %>

<%!
    public class Theme
    {
    	Map<String, String> args = null;
    	private MgSiteConnection site = null;
    	private String[] distNameArray = null;
    	private String[] distValueArray = null;
    	
    	public Theme(Map<String, String> incomingArgs) throws MgException
    	{
    	    this.args = incomingArgs;
    	    this.site = new MgSiteConnection();
    	    this.site.Open(new MgUserInformation(this.args.get("SESSION")));
    	    this.distNameArray = new String[] {"Individual", "Equal", "Standard Deviation", "Quantile", "Jenks (Natural Breaks)"};
    	    this.distValueArray = new String[] {"INDIV_DIST", "EQUAL_DIST", "STDEV_DIST", "QUANT_DIST", "JENK_DIST"};
    	}
    	
    	public String getNameArray()
	{
	    JSONArray jsonArray = new JSONArray();
	    for(int i=0;i<5;i++)
	    {
		jsonArray.put(distNameArray[i]);
	    }
	    return jsonArray.toString();
	}

	public String getValueArray()
	{
	    JSONArray jsonArray = new JSONArray();
	    for(int i=0;i<5;i++)
	    {
		jsonArray.put(distValueArray[i]);
	    }
	    return jsonArray.toString();
	}
        
        public ArrayList<String> GetMapLayerNames() throws MgException
	{
	    MgResourceService resourceService = (MgResourceService)this.site.CreateService(MgServiceType.ResourceService);
	    MgFeatureService featureService = (MgFeatureService)this.site.CreateService(MgServiceType.FeatureService);
	    
	    MgMap map = new MgMap();
	    map.Open(resourceService, this.args.get("MAPNAME"));
	    MgLayerCollection layers = map.GetLayers();
	    ArrayList<String> layerNames = new ArrayList<String>();

	    for(int i=0; i<layers.GetCount(); i++)
	    {
		MgLayer layer = (MgLayer) layers.GetItem(i);
		if(!layer.GetName().startsWith("_") && !layer.GetFeatureSourceId().toUpperCase().startsWith("SESSION"))
		{
		    MgResourceIdentifier resId = new MgResourceIdentifier(layer.GetFeatureSourceId());
		    String[] schemaClass = layer.GetFeatureClassName().split(":");
		
		    MgClassDefinition classDef = featureService.GetClassDefinition(resId, schemaClass[0], schemaClass[1]);
		    MgPropertyDefinition propDef = classDef.GetProperties().GetItem(layer.GetFeatureGeometryName());
		    
		    if(propDef.GetPropertyType() == MgFeaturePropertyType.GeometricProperty)
		    {
			MgGeometricPropertyDefinition geomPropDef = (MgGeometricPropertyDefinition) propDef;
			
			if(geomPropDef.GetGeometryTypes() == MgFeatureGeometricType.Surface)
			    layerNames.add(layer.GetLegendLabel());
		    }
		}
	    }
	    Collections.sort(layerNames);

	    return layerNames;
        }
        
	public LayerInfo GetLayerInfo() throws MgException, ParserConfigurationException, IOException, SAXException
        {
            ArrayList<Property> properties = new ArrayList<Property>();
            ArrayList<String> scaleRanges = new ArrayList<String>();
            
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
                    ArrayList<Integer> distroTypes = new ArrayList(this.GetDistributionsForDataType(dataType));
                    if(distroTypes.size() > 0)
                    {
                        properties.add(new Property(propertyDataDef.GetName(), dataType, distroTypes));
                    }
                }
            }
            
            // Next get a list of all of the layers scale ranges.
            
            MgResourceIdentifier layerDefResId = layer.GetLayerDefinition();
            MgByteReader byteReader = resourceService.GetResourceContent(layerDefResId);
            
            DocumentBuilderFactory docFactory = DocumentBuilderFactory.newInstance();
            DocumentBuilder docBuilder = docFactory.newDocumentBuilder();
            Document doc = docBuilder.parse(new InputSource(new StringReader(byteReader.ToString())));
            NodeList nodeList = doc.getElementsByTagName("VectorScaleRange");
            
            for(int i=0; i<nodeList.getLength(); i++)
            {
                Element currentNode = (Element) nodeList.item(i);
                String range = null;
                
                NodeList minNodeList = currentNode.getElementsByTagName("MinScale");
                if(minNodeList.getLength() > 0)
                    range = minNodeList.item(0).getFirstChild().getNodeValue();
                else
                    range = "0";
                
                NodeList maxNodeList = currentNode.getElementsByTagName("MaxScale");
                if(maxNodeList.getLength() > 0)
                    range = range + " - " + maxNodeList.item(0).getFirstChild().getNodeValue();
                else
                    range = range + " - Infinity";
                
                scaleRanges.add(range);
            }
            
            return new LayerInfo(properties, scaleRanges);
        }
        
        public ArrayList<String> GetPropertyMinMaxCount() throws MgException
        {
            ArrayList<String> propertyMinMaxCount = new ArrayList<String>();
            
            MgResourceService resourceService = (MgResourceService)this.site.CreateService(MgServiceType.ResourceService);
            
            MgMap map = new MgMap();
            map.Open(resourceService, this.args.get("MAPNAME"));
            MgLayer layer = (MgLayer) map.GetLayers().GetItem(this.args.get("LAYERNAME"));

            MgFeatureService featureService = (MgFeatureService)this.site.CreateService(MgServiceType.FeatureService);
            MgResourceIdentifier resId = new MgResourceIdentifier(layer.GetFeatureSourceId());
            
            String minValue = null;
            String maxValue = null;
            boolean firstTime = true;
            int count = 0;
            
            MgFeatureQueryOptions queryOptions = new MgFeatureQueryOptions();
            queryOptions.AddFeatureProperty(this.args.get("PROPERTYNAME"));
            
            MgFeatureReader featureReader = featureService.SelectFeatures(resId, layer.GetFeatureClassName(), queryOptions);
            while(featureReader.ReadNext())
            {
                String value = this.GetFeaturePropertyValue(featureReader, this.args.get("PROPERTYNAME"));
                int propertyType = featureReader.GetPropertyType(this.args.get("PROPERTYNAME"));
                if(firstTime)
                {
                    maxValue = value;
                    minValue = value;
                    firstTime = false;
                }
                switch(propertyType)
                {
                    case MgPropertyType.String :
                        if(!value.equals(""))
                        {
                            if(value.compareTo(maxValue) > 0)
                                maxValue = value;
                            if(value.compareTo(minValue) < 0)
                                minValue = value;
                        }
                        break;
                    case MgPropertyType.Byte :
                    case MgPropertyType.Int16 :
                    case MgPropertyType.Int32 :
                    case MgPropertyType.Int64 :
                        if(!value.equals(""))
                        {
                            if(Integer.valueOf(value) > Integer.valueOf(maxValue))
                                maxValue = value;
                            if(Integer.valueOf(value) < Integer.valueOf(minValue))
                                minValue = value;
                        }
                        break;
                    case MgPropertyType.Single :
                    case MgPropertyType.Double :
                        if(value!=null)
                        {
                            if(Double.valueOf(value) > Double.valueOf(maxValue))
                                maxValue = value;
                            if(Double.valueOf(value) < Double.valueOf(minValue))
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
            
            propertyMinMaxCount.add(minValue.toString());
            propertyMinMaxCount.add(maxValue.toString());
            propertyMinMaxCount.add(String.valueOf(count));
            
            return propertyMinMaxCount;
        }
        
        public String ApplyTheme() throws MgException, ParserConfigurationException, IOException, SAXException, TransformerConfigurationException, TransformerException
        {
    	    MgResourceService resourceService = (MgResourceService)this.site.CreateService(MgServiceType.ResourceService);
    	    MgFeatureService featureService = (MgFeatureService)this.site.CreateService(MgServiceType.FeatureService);

    	    MgMap map = new MgMap();
    	    map.Open(resourceService, this.args.get("MAPNAME"));
    	    MgLayerCollection layers = map.GetLayers();
            MgLayer layer = (MgLayer) layers.GetItem(this.args.get("LAYERNAME"));
            
            MgResourceIdentifier resId = new MgResourceIdentifier(layer.GetFeatureSourceId());
            MgResourceIdentifier layerDefResId = layer.GetLayerDefinition();
            MgByteReader byteReader = resourceService.GetResourceContent(layerDefResId);
            
            // Load the Layer Definition and Navigate to the specified <VectorScaleRange>
            
            DocumentBuilderFactory docFactory = DocumentBuilderFactory.newInstance();
            DocumentBuilder docBuilder = docFactory.newDocumentBuilder();
            Document doc = docBuilder.parse(new InputSource(new StringReader(byteReader.ToString())));

            NodeList nodeList = doc.getElementsByTagName("VectorScaleRange");
                
            Element vectorScaleRangecElement = (Element) nodeList.item(Integer.valueOf(this.args.get("SCALERANGEINDEX")));
            Element areaTypeStyle = (Element) vectorScaleRangecElement.getElementsByTagName("AreaTypeStyle").item(0);
            
            // Remove any existing <AreaRule> elements.
            
            NodeList areaRuleList = areaTypeStyle.getElementsByTagName("AreaRule");
            int count = areaRuleList.getLength();
            for(int i=0; i<count; i++)
            {
                areaTypeStyle.removeChild(areaRuleList.item(0));
            }
            
            // Now create the new <AreaRule> elements.
            
            String lineSep = System.getProperty("line.separator");
            BufferedReader buffReader = new BufferedReader(new FileReader(GetThemeXmlTemplatePath()));
            String nextLine = "";
            StringBuffer strBuffer = new StringBuffer();
            while((nextLine = buffReader.readLine()) != null)
            {
                strBuffer.append(nextLine);
                strBuffer.append(lineSep);
            }
            String areaRuleTemplate = strBuffer.toString();
            MgFeatureAggregateOptions aggregateOptions = new MgFeatureAggregateOptions();
            
            String value = null;
            String filterText = null;
            String areaRuleXML = null;
            Document areaDoc = null;
            Node areaNode = null;
            double portion = 0.0;
            double increment = (Integer.valueOf(this.args.get("NUMRULES")) > 1)? 1.0 / (Integer.valueOf(this.args.get("NUMRULES"))-1) : 1.0;
            
            if("INDIV_DIST".equals(this.args.get("DISTRO")))
            {
                aggregateOptions.AddFeatureProperty(this.args.get("PROPERTYNAME"));
                aggregateOptions.SelectDistinct(true);
                
                MgDataReader dataReader = featureService.SelectAggregate(resId, layer.GetFeatureClassName(), aggregateOptions);
                while(dataReader.ReadNext())
                {
                    value = this.GetFeaturePropertyValue(dataReader, this.args.get("PROPERTYNAME"));
                    
                    filterText = "&quot;" + this.args.get("PROPERTYNAME") + "&quot; = ";
                    if(Integer.valueOf(this.args.get("DATATYPE")) == MgPropertyType.String)
                        filterText = filterText + "'" + value + "'";
                    else
                        filterText = filterText + value;

                    areaRuleXML = String.format(areaRuleTemplate,
                                                this.args.get("PROPERTYNAME") + ":" + value,
                                                filterText,
                                                this.InterpolateColor(portion, this.args.get("FILLFROM"), this.args.get("FILLTO"), Integer.valueOf(this.args.get("FILLTRANS"))),
                                                this.InterpolateColor(portion, this.args.get("LINEFROM"), this.args.get("LINETO"), 0));
                    docFactory = DocumentBuilderFactory.newInstance();
                    docBuilder = docFactory.newDocumentBuilder();
                    areaDoc = docBuilder.parse(new InputSource(new StringReader(areaRuleXML))); 

                    areaNode = doc.importNode(areaDoc.getDocumentElement(), true);
                    areaTypeStyle.appendChild(areaNode);
                    
                    portion = portion + increment;
                }
                dataReader.Close();
            }
            else
            {
                ArrayList<String> values = new ArrayList<String>();
                
                aggregateOptions.AddComputedProperty("THEME_VALUE",
                    this.args.get("DISTRO") + "(\"" + this.args.get("PROPERTYNAME") + "\"," + this.args.get("NUMRULES") + "," + this.args.get("MINVALUE") + "," + this.args.get("MAXVALUE") + ")");
                
                MgDataReader dataReader = featureService.SelectAggregate(resId, layer.GetFeatureClassName(), aggregateOptions);
                while(dataReader.ReadNext())
                {
                    value = this.GetFeaturePropertyValue(dataReader, "THEME_VALUE");
                    values.add(value);
                }
                dataReader.Close();
                
                for(int i=0; i<values.size() - 1; i++)
                {
                    filterText = "&quot;" + this.args.get("PROPERTYNAME") + "&quot; &gt;= " + values.get(i) + " AND &quot;" + this.args.get("PROPERTYNAME");
                    if(i == values.size() - 1)
                        filterText = filterText + "&quot; &lt;= " + values.get(i+1);
                    else
                        filterText = filterText + "&quot; &lt; " + values.get(i+1);
                    
                    areaRuleXML = String.format(areaRuleTemplate,
                                                this.args.get("PROPERTYNAME") + ":" + values.get(i) + " - " + values.get(i+1),
                                                filterText,
                                                this.InterpolateColor(portion, this.args.get("FILLFROM"), this.args.get("FILLTO"), Integer.valueOf(this.args.get("FILLTRANS"))),
                                                this.InterpolateColor(portion, this.args.get("LINEFROM"), this.args.get("LINETO"), 0));
                    
                    docFactory = DocumentBuilderFactory.newInstance();
                    docBuilder = docFactory.newDocumentBuilder();
                    areaDoc = docBuilder.parse(new InputSource(new StringReader(areaRuleXML))); 

                    areaNode = doc.importNode(areaDoc.getDocumentElement(), true);
                    areaTypeStyle.appendChild(areaNode);
		                        
                    portion = portion + increment;
                }
            }
            
            // Now save our new layer definition to the session and add it to the map.
            
            DOMSource domSource = new DOMSource(doc);
            StringWriter writer = new StringWriter();
            StreamResult result = new StreamResult(writer);
            TransformerFactory tf = TransformerFactory.newInstance();
            Transformer transformer = tf.newTransformer();
            transformer.transform(domSource, result);
            String layerDefinition = writer.toString();
            String uniqueName = this.MakeUniqueLayerName(map, this.args.get("LAYERNAME"), this.args.get("THEMENAME"));
            String legendLabel = layer.GetLegendLabel();
            if(this.args.get("THEMENAME").trim().length() > 0)
                legendLabel = legendLabel + " (" + this.args.get("THEMENAME") + ")";
            
            MgResourceIdentifier layerResId = new MgResourceIdentifier("Session:" + this.args.get("SESSION") + "//" + uniqueName + ".LayerDefinition");
            
            MgByteSource byteSource = new MgByteSource(layerDefinition.getBytes(), layerDefinition.length());
            resourceService.SetResource(layerResId, byteSource.GetReader(), null);
            
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
            String result = String.format("%02x", 255*(100-percentTransparent)/100);
            
            if(startColor.equals(endColor))
                return result+startColor;
            
            int red = this.CalculateRGB(portion, startColor.substring(0,2), endColor.substring(0,2));
            result = (Integer.toHexString(red).length() == 1)? result+"0"+Integer.toHexString(red) : result+Integer.toHexString(red);
            int green = this.CalculateRGB(portion, startColor.substring(2,4), endColor.substring(2,4));
            result = (Integer.toHexString(green).length() == 1)? result+"0"+Integer.toHexString(green) : result+Integer.toHexString(green);
            int blue = this.CalculateRGB(portion, startColor.substring(4,6), endColor.substring(4,6));
            result = (Integer.toHexString(blue).length() == 1)? result+"0"+Integer.toHexString(blue) : result+Integer.toHexString(blue);
            
            return result;
        }
        
        private int CalculateRGB(double portion, String startRGB, String endRGB)
        {
            double result = Integer.parseInt(startRGB, 16) + portion*(Integer.parseInt(endRGB, 16) - Integer.parseInt(startRGB, 16));
            return (int) result;
        }
        
        private ArrayList<Integer> GetDistributionsForDataType(int type) throws MgException
        {
            ArrayList<Integer> distroTypes = new ArrayList<Integer>();
            
            switch(type)
            {
                case MgPropertyType.String :
                    distroTypes.add(0);
                    break;
                case MgPropertyType.Byte :
                case MgPropertyType.Int16 :
                case MgPropertyType.Int32 :
                case MgPropertyType.Int64 :
                    distroTypes.add(0);
                    distroTypes.add(1);
                    distroTypes.add(2);
                    distroTypes.add(3);
                    distroTypes.add(4);
                    break;
                case MgPropertyType.Single :
                case MgPropertyType.Double :
                    distroTypes.add(1);
                    distroTypes.add(2);
                    distroTypes.add(3);
                    distroTypes.add(4);
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
        
        private String MakeUniqueLayerName(MgMap map, String layerName, String themeName) throws MgException
        {
            String desiredName = "_" + layerName + themeName;
            String uniqueName = desiredName;
            int index = 1;
            
            while(map.GetLayers().Contains(uniqueName))
            {
                uniqueName = desiredName + Integer.toString(index);
                index++;
            }
            return uniqueName;
        }
        
        private String GetFeaturePropertyValue(MgReader dataReader, String name) throws MgException
        {
            String value = null;
            int propertyType = dataReader.GetPropertyType(name);
            switch(propertyType)
            {
                case MgPropertyType.Boolean :
                    value = String.valueOf(dataReader.GetBoolean(name));
                    break;
                case MgPropertyType.Byte :
                    value = String.valueOf(dataReader.GetByte(name));
                    break;
                case MgPropertyType.Single :
                    value = String.valueOf(dataReader.GetSingle(name));
                    break;
                case MgPropertyType.Double :
                    value = String.valueOf(dataReader.GetDouble(name));
                    break;
                case MgPropertyType.Int16 :
                    value = String.valueOf(dataReader.GetInt16(name));
                    break;
                case MgPropertyType.Int32 :
                    value = String.valueOf(dataReader.GetInt32(name));
                    break;
                case MgPropertyType.Int64 :
                    value = String.valueOf(dataReader.GetInt64(name));
                    break;
                case MgPropertyType.String :
                    value = dataReader.GetString(name);
                    break;
                case MgPropertyType.DateTime :
                case MgPropertyType.Null :
                case MgPropertyType.Blob :
                case MgPropertyType.Clob :
                case MgPropertyType.Feature :
                case MgPropertyType.Geometry :
                case MgPropertyType.Raster :
                    value = "[unsupported data type]";
                    break;
            }
            return value;
        }
    }

%>

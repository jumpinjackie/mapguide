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
<%@ page import="javax.xml.parsers.*" %>
<%@ page import="org.w3c.dom.*" %>
<%@ include file="common.jsp" %>

<%!

class TreeItem
{
    public TreeItem(String name, boolean isGroup, Object rtObject)
    {
        this.name = name;
        this.isGroup = isGroup;
        this.rtObject = rtObject;
        if(isGroup)
            this.children = new ArrayList();
        else
            this.children = null;
        this.parent = null;
    }

    public void Attach(TreeItem child)
    {
        if(this.children == null)
            this.children = new ArrayList();

        this.children.add(child);
    }

    public String name;
    public boolean isGroup;
    public Object rtObject;
    public ArrayList children;
    public String parentName;
    public TreeItem parent;
}

String mapName = "";
String sessionId = "";
boolean summary = false;
int layerCount = 0;
String[] layerIds = null;
int intermediateVar = 0;
String output = "\nvar layerData = new Array();\n";

%>

<%
response.setContentType("text/html; charset=UTF-8");
request.setCharacterEncoding("UTF-8");
PrintWriter writer = response.getWriter();

try
{
    output = "\nvar layerData = new Array();\n";
    intermediateVar = 0;

    // Initialize web tier with the site configuration file.
    InitializeWebTier();

    //fetch the parameters for this request
    //
    GetRequestParameters(request);

    // Connect to the site and get a resource service instance
    //
    MgUserInformation userInfo = new MgUserInformation();
    userInfo.SetMgSessionId(sessionId);
    userInfo.SetClientIp(GetClientIp(request));
    userInfo.SetClientAgent(GetClientAgent());


    MgSiteConnection site = new MgSiteConnection();
    site.Open(userInfo);
    MgResourceService resourceSrvc = (MgResourceService)site.CreateService(MgServiceType.ResourceService);

    //Load the map runtime state.
    //
    MgMap map = new MgMap();
    map.Open(resourceSrvc, mapName);

    int updateType = -1;

    ArrayList tree = BuildLayerTree(map);
    if(summary)
    {
        updateType = 0;
        // return only the layer structure, that is mainly groups/layers/layer-ids. Do not parse layer definitions.
        //
        BuildClientSideTree(tree, null, "null", false, "layerData", resourceSrvc, null);
    }
    else
    {
        HashMap layerMap = null;
        if(layerCount == 0)
            updateType = 1;
        else
        {
            updateType = 2;
            layerMap = BuildLayerMap(map);
        }
        BuildClientSideTree(tree, null, "null", true, "layerData", resourceSrvc, layerMap);
    }

    //load html template code and format it
    //
    String templ = LoadTemplate("/viewerfiles/legendupdate.templ");
    String vals[] = { String.valueOf(updateType), output, GetSurroundVirtualPath(request) + "legend.jsp"};
    String outputString = Substitute(templ, vals);

    writer.write(outputString);
}
catch(MgException e)
{
    String details = e.GetDetails();
    writer = response.getWriter();
    writer.write(details);
    response.setContentLength(details.length());
}
catch(Exception ne)
{
    String msg = ne.getLocalizedMessage();
    writer = response.getWriter();
    writer.write(msg);
    response.setContentLength(msg.length());
}

// Flush output buffer
response.flushBuffer();
%>

<%!
void GetRequestParameters(HttpServletRequest request)
{
    if(IsParameter(request, "MAPNAME"))
        mapName = GetParameter(request, "MAPNAME");

    if(IsParameter(request, "SESSION"))
        sessionId = GetParameter(request, "SESSION");

    if(IsParameter(request, "SUMMARY"))
    {
        summary = true;
    }
    else
    {
        summary = false;
        if(IsParameter(request, "LC"))
        {
            layerCount = Integer.parseInt(GetParameter(request, "LC"));
        }

        if(layerCount > 0 && IsParameter(request, "LAYERS"))
        {
            String layers = GetParameter(request, "LAYERS");
            layerIds = layers.split(",");
        }
    }
}
%>

<%!
ArrayList BuildLayerTree(MgMap map) throws MgException
{
    ArrayList tree = new ArrayList();
    HashMap knownGroups = new HashMap();
    ArrayList unresolved = new ArrayList();
    MgLayerGroupCollection groups = map.GetLayerGroups();

    for(int i = 0; i < groups.GetCount(); i++)
    {
        MgLayerGroup rtGroup = (MgLayerGroup)groups.GetItem(i);
        TreeItem node = new TreeItem(rtGroup.GetName(), true, rtGroup);
        knownGroups.put(node.name, node);
        MgLayerGroup parentGroup = rtGroup.GetGroup();
        if(parentGroup == null)
        {
            tree.add(node);
        }
        else
        {
            String parentName = parentGroup.GetName();
            TreeItem parentNode = (TreeItem)knownGroups.get(parentName);
            if(parentNode != null)
                parentNode.Attach(node);
            else
            {
                node.parentName = parentName;
                unresolved.add(node);
            }
        }
    }
    if(unresolved.size() > 0)
    {
        for(int i = 0; i < unresolved.size(); i++)
        {
            TreeItem node = (TreeItem)unresolved.get(i);
            TreeItem parentNode = (TreeItem)knownGroups.get(node.parentName);
            if(parentNode != null)
                parentNode.Attach(node);
            else
                tree.add(node); //should not happen. place group in the root if parent is not known
        }
    }
    MgLayerCollection layers = map.GetLayers();
    for(int i = 0; i < layers.GetCount(); i++)
    {
        MgLayer rtLayer = (MgLayer) layers.GetItem(i);
        TreeItem node = new TreeItem(rtLayer.GetName(), false, rtLayer);
        MgLayerGroup parentGroup = rtLayer.GetGroup();
        if(parentGroup == null)
            tree.add(node);
        else
        {
            TreeItem parentNode = (TreeItem)knownGroups.get(parentGroup.GetName());
            if(parentNode != null)
                parentNode.Attach(node);
            else
                tree.add(node); //should not happen. place layer in the root if parent is not known
        }
    }

    return tree;
}
%>

<%!
HashMap BuildLayerMap(MgMap map) throws MgException
{
    HashMap layerMap = new HashMap();
    MgLayerCollection layers = map.GetLayers();
    for(int i = 0; i < layers.GetCount(); i++)
    {
        MgLayer rtLayer = (MgLayer) layers.GetItem(i);
        layerMap.put(rtLayer.GetObjectId(), rtLayer);
    }
    return layerMap;
}
%>

<%!
void BuildClientSideTree(ArrayList tree, TreeItem parent, String parentName, boolean fulldata, String container, MgResourceService resSrvc, HashMap layerMap) throws MgException
{
    // 2 passes: pass 1 adds layers to the tree, pass 2 adds groups
    //
    int treeIndex = 0;
    for(int pass = 0; pass < 2; pass++)
    {
        for(int i = 0; i < tree.size(); i++)
        {
            TreeItem node = (TreeItem)tree.get(i);
            if(node.isGroup)
            {
                if(pass == 1)
                {
                    String groupName = "grp" + (intermediateVar++);
                    String arrChildName;
                    if(node.children != null)
                    {
                        arrChildName = "c" + (intermediateVar++);
                        output = output + "var " + arrChildName + " = new Array();\n";
                    }
                    else
                        arrChildName = "null";

                    MgLayerGroup rtLayerGroup = (MgLayerGroup)node.rtObject;
                    if(fulldata)
                    {
                        output = output + String.format("var %s = new GroupItem(\"%s\", %s, %s, %s, %s, \"%s\", \"%s\", %s);\n",
                                                           new Object[] {groupName,
                                                           StrEscape(rtLayerGroup.GetLegendLabel()),
                                                           rtLayerGroup.GetExpandInLegend()? "true": "false",
                                                           parentName,
                                                           rtLayerGroup.GetVisible()? "true": "false",
                                                           rtLayerGroup.GetDisplayInLegend()? "true": "false",
                                                           rtLayerGroup.GetObjectId(),
                                                           StrEscape(rtLayerGroup.GetName()),
                                                           rtLayerGroup.GetLayerGroupType() == MgLayerGroupType.BaseMap? "true": "false"});
                    }
                    else
                    {
                        output = output + String.format("var %s = new GroupSummary(\"%s\", \"%s\", %s, %s);\n",
                                                           new Object[] {groupName,
                                                           StrEscape(rtLayerGroup.GetName()),
                                                           rtLayerGroup.GetObjectId(),
                                                           arrChildName,
                                                           parentName});
                    }
                    output = output + String.format("%s[%d] = %s;\n", new Object[] {container, Integer.valueOf(treeIndex), groupName});
                    ++treeIndex;

                    if(node.children != null)
                    {
                        BuildClientSideTree(node.children, node, groupName, fulldata, arrChildName, resSrvc, null);
                        output = output + String.format("%s.children = %s;\n", new Object[] {groupName, arrChildName});
                    }
                }
            }
            else
            {
                if(pass == 0)
                {
                    MgLayer rtLayer = (MgLayer)node.rtObject;
                    if(fulldata)
                    {
                        MgResourceIdentifier resId = rtLayer.GetLayerDefinition();
                        String layerName = "lyr" + (intermediateVar++);
                        String objectId = rtLayer.GetObjectId();
                        output = output + String.format("var %s = new LayerItem(\"%s\", \"%s\", %s, %s, %s, %s, %s, \"%s\", \"%s\", %s);\n",
                                                           new Object[] {layerName,
                                                           StrEscape(rtLayer.GetLegendLabel()),
                                                           rtLayer.GetName(),
                                                           rtLayer.GetExpandInLegend()? "true": "false",
                                                           parentName,
                                                           rtLayer.GetVisible()? "true": "false",
                                                           rtLayer.GetDisplayInLegend()? "true": "false",
                                                           rtLayer.GetSelectable()? "true": "false",
                                                           resId.ToString(),
                                                           objectId,
                                                           rtLayer.GetLayerType() == MgLayerType.BaseMap? "true": "false"});

                        output = output + String.format("%s[%d] = %s;\n",
                                                           new Object[] {container,
                                                           Integer.valueOf(treeIndex),
                                                           layerName});
                        ++treeIndex;

                        if(layerMap == null || !layerMap.containsKey(objectId))
                        {
                            BuildLayerDefinitionData(resSrvc, resId, layerName);
                        }
                    }
                    else
                    {
                        output = output + String.format("%s[%d] = new LayerSummary(\"%s\", \"%s\", \"%s\");\n",
                                                            new Object[] {container,
                                                            Integer.valueOf(i),
                                                            StrEscape(rtLayer.GetName()),
                                                            rtLayer.GetObjectId(),
                                                            rtLayer.GetLayerDefinition().ToString()});
                    }
                }
            }
        }
    }
}
%>

<%!
void BuildLayerDefinitionData(MgResourceService resSrvc, MgResourceIdentifier resId, String layerVarName)
{
    try
    {
        MgByteReader layerByteReader = resSrvc.GetResourceContent(resId);
        InputStream layerStream = ByteReaderToStream(layerByteReader);
        DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
        DocumentBuilder builder = factory.newDocumentBuilder();
        Document doc = builder.parse(layerStream);
        int type = 0;
        NodeList scaleRanges = doc.getElementsByTagName("VectorScaleRange");
        if(scaleRanges.getLength() == 0) {
            scaleRanges = doc.getElementsByTagName("GridScaleRange");
            if(scaleRanges.getLength() == 0) {
                scaleRanges = doc.getElementsByTagName("DrawingLayerDefinition");
                if(scaleRanges.getLength() == 0)
                    return;
                type = 2;
            }
            else
                type = 1;
        }

        String typeStyles[] = new String[]{"PointTypeStyle", "LineTypeStyle", "AreaTypeStyle", "CompositeTypeStyle"};
        String ruleNames[] = new String[]{"PointRule", "LineRule", "AreaRule", "CompositeRule"};

        for(int sc = 0; sc < scaleRanges.getLength(); sc++)
        {
            Element scaleRange = (Element)scaleRanges.item(sc);
            String scaleRangeVarName = "sc" + (intermediateVar++);
            String minScale, maxScale;
            NodeList minElt = scaleRange.getElementsByTagName("MinScale");
            NodeList maxElt = scaleRange.getElementsByTagName("MaxScale");
            minScale = "0";
            maxScale = "1000000000000.0";  // as MDF's VectorScaleRange::MAX_MAP_SCALE
            if(minElt.getLength() > 0)
                minScale = minElt.item(0).getChildNodes().item(0).getNodeValue().toString();
            if(maxElt.getLength() > 0)
                maxScale = maxElt.item(0).getChildNodes().item(0).getNodeValue().toString();
            output = output + String.format("var %s = new ScaleRangeItem(%s, %s, %s);\n",
                                        new Object[]{scaleRangeVarName,
                                        minScale,
                                        maxScale,
                                        layerVarName});
            output = output + String.format("%s.children[%d] = %s;\n", new Object[] {layerVarName, Integer.valueOf(sc), scaleRangeVarName});

            if(type != 0)
                break;

            int styleIndex = 0;
            for(int ts=0; ts < typeStyles.length; ts++)
            {
                NodeList typeStyle = scaleRange.getElementsByTagName(typeStyles[ts]);
                int catIndex = 0;
                for(int st = 0; st < typeStyle.getLength(); st++)
                {
                    // We will check if this typestyle is going to be shown in the legend
                    NodeList showInLegend = ((Element)typeStyle.item(st)).getElementsByTagName("ShowInLegend");
                    if(showInLegend != null && showInLegend.getLength() > 0)
                        if(showInLegend.item(0).getChildNodes().item(0).getNodeValue().equals("false"))
                            continue;   // This typestyle does not need to be shown in the legend

                    NodeList rules = ((Element)typeStyle.item(st)).getElementsByTagName(ruleNames[ts]);
                    for(int r = 0; r < rules.getLength(); r++)
                    {
                        Element rule = (Element)rules.item(r);
                        NodeList label = rule.getElementsByTagName("LegendLabel");
                        NodeList filter = rule.getElementsByTagName("Filter");

                        String labelText = "";
                        if(label != null && label.getLength() > 0)
                        {
                            NodeList subItems = label.item(0).getChildNodes();
                            if(subItems != null && subItems.getLength() > 0)
                                labelText = subItems.item(0).getNodeValue();
                        }
                        String filterText = "";
                        if(filter != null && filter.getLength() > 0)
                        {
                            NodeList subItems2 = filter.item(0).getChildNodes();
                            if(subItems2 != null && subItems2.getLength() > 0)
                                filterText = subItems2.item(0).getNodeValue();
                        }

                        output = output + String.format("%s.children[%d] = new StyleItem(\"%s\", \"%s\", %d, %d);\n",
                                                    new Object[]{scaleRangeVarName,
                                                    Integer.valueOf(styleIndex++),
                                                    StrEscape(labelText.trim()),
                                                    StrEscape(filterText.trim()),
                                                    ts+1,
                                                    catIndex++});
                    }
                }
            }
        }
        output = output + String.format("%s.lyrtype = %d;\n", new Object[]{layerVarName, new Integer(type) });
    }
    catch(MgException e)
    {
        //broken layer definition. just don't create any info for that layer
        return;
    }
    catch(Exception e)
    {
        //broken layer definition. just don't create any info for that layer
        return;
    }
}
%>

<%!
InputStream ByteReaderToStream(MgByteReader byteReader) throws MgException
{
    InputStream stream = null;
    if(byteReader != null)
    {
        ByteArrayOutputStream bos = new ByteArrayOutputStream();

        byte[] byteBuffer = new byte[1024];
        int numBytes = byteReader.Read(byteBuffer, 1024);
        while(numBytes > 0)
        {
            bos.write(byteBuffer, 0, numBytes);
            numBytes = byteReader.Read(byteBuffer, 1024);
        }
        stream = new ByteArrayInputStream(bos.toByteArray());
    }
    return stream;
}

String StrEscape(String str)
{
    return StrEscape(str, false);
}

String StrEscape(String str, boolean single)
{
    String org = single ? "'" : "\"";
    String rep = single ? "\\\\'": "\\\\\"";

    return str.replaceAll(org, rep);
}

%>

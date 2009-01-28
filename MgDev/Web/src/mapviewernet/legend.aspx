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
<%@ Import Namespace="System.Collections" %>
<%@ Import Namespace="System.Collections.Specialized" %>
<%@ Import Namespace="System.Web" %>
<%@ Import Namespace="System.Globalization" %>
<%@ Import Namespace="System.Xml" %>
<%@ Import Namespace="OSGeo.MapGuide" %>

<!-- #Include File="common.aspx -->

<script runat="server">
class TreeItem
{
    public TreeItem(String name, bool isGroup, Object rtObject)
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

        this.children.Add(child);
    }

    public String name;
    public bool isGroup;
    public Object rtObject;
    public ArrayList children;
    public String parentName;
    public TreeItem parent;
}

String mapName = "";
String sessionId = "";
bool summary = false;
int layerCount = 0;
String[] layerIds = null;
int intermediateVar = 0;
String output = "\nvar layerData = new Array();\n";
</script>

<%
    Response.Charset = "utf-8";

    try
    {
        // Initialize web tier with the site configuration file.
        InitializeWebTier();

        //fetch the parameters for this request
        //
        GetRequestParameters();

        // Connect to the site and get a resource service instance
        //
        MgUserInformation userInfo = new MgUserInformation();
        userInfo.SetMgSessionId(sessionId);
        userInfo.SetClientIp(GetClientIp(Request));
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
            Hashtable layerMap = null;
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
        String templ = LoadTemplate(Request, "../viewerfiles/legendupdate.templ");
        String[] vals = { updateType.ToString(NumberFormatInfo.InvariantInfo),
            output,
            GetSurroundVirtualPath(Request) + "legend.aspx"};
        String outputString = Substitute(templ, vals);

        Response.Write(Substitute(templ, vals));
    }
    catch(MgException mge)
    {
        Response.Write(mge.GetMessage());
        Response.Write("<br>");
        Response.Write(mge.GetDetails());
    }

%>

<script runat="server">

void GetRequestParameters()
{
    if ("POST"== Request.HttpMethod)
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
    if (IsParameter(parameters, "MAPNAME"))
    {
        mapName = GetParameter(parameters, "MAPNAME");
    }
    if (IsParameter(parameters, "SESSION"))
    {
        sessionId = GetParameter(parameters, "SESSION");
    }
    if (IsParameter(parameters, "SUMMARY"))
    {
        summary = true;
    }
    else
    {
        if (IsParameter(parameters, "LC"))
        {
            layerCount = Convert.ToInt32(GetParameter(parameters, "LC"));
        }
        if (layerCount > 0 && IsParameter(parameters, "LAYERS"))
        {
            String layers = GetParameter(parameters, "LAYERS");

            char[] delimiter = {','};
            layerIds = layers.Split(delimiter);
        }
    }
}

String IntToString(int number)
{
    return number.ToString(NumberFormatInfo.InvariantInfo);
}

String DoubleToString(double number)
{
    return number.ToString(NumberFormatInfo.InvariantInfo);
}

ArrayList BuildLayerTree(MgMap map)
{
    ArrayList tree = new ArrayList();
    Hashtable knownGroups = new Hashtable();
    ArrayList unresolved = new ArrayList();
    MgLayerGroupCollection groups = map.GetLayerGroups();

    for(int i = 0; i < groups.GetCount(); i++)
    {
        MgLayerGroup rtGroup = (MgLayerGroup)groups.GetItem(i);
        TreeItem node = new TreeItem(rtGroup.GetName(), true, rtGroup);
        knownGroups.Add(node.name, node);
        MgLayerGroup parentGroup = rtGroup.GetGroup();
        if(parentGroup == null)
        {
            tree.Add(node);
        }
        else
        {
            String parentName = parentGroup.GetName();
            TreeItem parentNode = (TreeItem)knownGroups[parentName];
            if(parentNode != null)
                parentNode.Attach(node);
            else
            {
                node.parentName = parentName;
                unresolved.Add(node);
            }
        }
    }
    if(unresolved.Count > 0)
    {
        for(int i = 0; i < unresolved.Count; i++)
        {
            TreeItem node = (TreeItem)unresolved[i];
            TreeItem parentNode = (TreeItem)knownGroups[node.parentName];
            if(parentNode != null)
                parentNode.Attach(node);
            else
                tree.Add(node); //should not happen. place group in the root if parent is not known
        }
    }
    MgLayerCollection layers = map.GetLayers();
    for(int i = 0; i < layers.GetCount(); i++)
    {
        MgLayer rtLayer = (MgLayer) layers.GetItem(i);
        TreeItem node = new TreeItem(rtLayer.GetName(), false, rtLayer);
        MgLayerGroup parentGroup = rtLayer.GetGroup();
        if(parentGroup == null)
            tree.Add(node);
        else
        {
            TreeItem parentNode = (TreeItem)knownGroups[parentGroup.GetName()];
            if(parentNode != null)
                parentNode.Attach(node);
            else
                tree.Add(node); //should not happen. place layer in the root if parent is not known
        }
    }

    return tree;
}


Hashtable BuildLayerMap(MgMap map)
{
    Hashtable layerMap = new Hashtable();
    MgLayerCollection layers = map.GetLayers();
    for(int i = 0; i < layers.GetCount(); i++)
    {
        MgLayer rtLayer = (MgLayer) layers.GetItem(i);
        layerMap.Add(rtLayer.GetObjectId(), rtLayer);
    }
    return layerMap;
}


void BuildClientSideTree(ArrayList tree, TreeItem parent, String parentName, bool fulldata, String container, MgResourceService resSrvc, Hashtable layerMap)
{
    // 2 passes: pass 1 adds layers to the tree, pass 2 adds groups
    //
    int treeIndex = 0;
    for(int pass = 0; pass < 2; pass++)
    {
        for(int i = 0; i < tree.Count; i++)
        {
            TreeItem node = (TreeItem)tree[i];
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
                    {
                        arrChildName = "null";
                    }

                    MgLayerGroup rtLayerGroup = (MgLayerGroup)node.rtObject;
                    if(fulldata)
                    {
                        output = output + String.Format("var {0} = new GroupItem(\"{1}\", {2}, {3}, {4},{5}, \"{6}\", \"{7}\", {8});\n",
                                                        groupName,
                                                        StrEscape(rtLayerGroup.GetLegendLabel()),
                                                        rtLayerGroup.GetExpandInLegend()? "true": "false",
                                                        parentName,
                                                        rtLayerGroup.GetVisible() ? "true" : "false",
                                                        rtLayerGroup.GetDisplayInLegend() ? "true" : "false",
                                                        rtLayerGroup.GetObjectId(),
                                                        StrEscape(rtLayerGroup.GetName()),
                                                        rtLayerGroup.GetLayerGroupType() == MgLayerGroupType.BaseMap? "true": "false");
                    }
                    else
                    {
                        output = output + String.Format("var {0} = new GroupSummary(\"{1}\", \"{2}\", {3}, {4});\n",
                                                        groupName,
                                                        StrEscape(rtLayerGroup.GetName()),
                                                        rtLayerGroup.GetObjectId(),
                                                        arrChildName,
                                                        parentName);
                    }
                    output = output + String.Format("{0}[{1}] = {2};\n", container, treeIndex, groupName);
                    ++treeIndex;

                    if(node.children != null)
                    {
                        BuildClientSideTree(node.children, node, groupName, fulldata, arrChildName, resSrvc, null);
                        output = output + String.Format("{0}.children = {1};\n", groupName, arrChildName);
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
                        output = output + String.Format("var {0} = new LayerItem(\"{1}\", \"{2}\", {3}, {4}, {5}, {6}, {7}, \"{8}\", \"{9}\", {10});\n",
                                                        layerName,
                                                        rtLayer.GetLegendLabel(),
                                                        StrEscape(rtLayer.GetName()),
                                                        rtLayer.GetExpandInLegend()? "true": "false",
                                                        parentName,
                                                        rtLayer.GetVisible()? "true": "false",
                                                        rtLayer.GetDisplayInLegend() ? "true" : "false",
                                                        rtLayer.GetSelectable() ? "true" : "false",
                                                        resId.ToString(),
                                                        objectId,
                                                        rtLayer.GetLayerType() == MgLayerType.BaseMap? "true": "false");

                        output = output + String.Format("{0}[{1}] = {2};\n",
                                                        container,
                                                        treeIndex,
                                                        layerName);
                        ++treeIndex;

                        if(layerMap == null || !layerMap.ContainsKey(objectId))
                        {
                            BuildLayerDefinitionData(resSrvc, resId, layerName);
                        }
                    }
                    else
                    {
                        output = output + String.Format("{0}[{1}] = new LayerSummary(\"{2}\", \"{3}\", \"{4}\");\n",
                                                            container,
                                                            i,
                                                            rtLayer.GetName(),
                                                            rtLayer.GetObjectId(),
                                                            rtLayer.GetLayerDefinition().ToString());
                    }
                }
            }
        }
    }
}


void BuildLayerDefinitionData(MgResourceService resSrvc, MgResourceIdentifier resId, String layerVarName)
{
    try
    {
        MgByteReader layerByteReader = resSrvc.GetResourceContent(resId);
        String layerDefString = layerByteReader.ToString();
        XmlDocument doc = new XmlDocument();
        doc.LoadXml(layerDefString);
        int type = 0;
        XmlNodeList scaleRanges = doc.GetElementsByTagName("VectorScaleRange");
        if(scaleRanges.Count == 0)
        {
            scaleRanges = doc.GetElementsByTagName("GridScaleRange");
            if(scaleRanges.Count == 0) {
                scaleRanges = doc.GetElementsByTagName("DrawingLayerDefinition");
                if(scaleRanges.Count == 0)
                    return;
                type = 2;
            }
            else
                type = 1;
        }

        String[] typeStyles = new String[]{"PointTypeStyle", "LineTypeStyle", "AreaTypeStyle", "CompositeTypeStyle"};
        String[] ruleNames = new String[]{"PointRule", "LineRule", "AreaRule", "CompositeRule"};

        for(int sc = 0; sc < scaleRanges.Count; sc++)
        {
            XmlElement scaleRange = (XmlElement)scaleRanges[sc];
            XmlNodeList minElt = scaleRange.GetElementsByTagName("MinScale");
            XmlNodeList maxElt = scaleRange.GetElementsByTagName("MaxScale");
            String minScale, maxScale;
            minScale = "0";
            maxScale = "1000000000000.0";   // as MDF's VectorScaleRange::MAX_MAP_SCALE
            if(minElt.Count > 0)
                minScale = minElt[0].ChildNodes[0].Value;
            if(maxElt.Count > 0)
                maxScale = maxElt[0].ChildNodes[0].Value;
            String scaleRangeVarName = "sc" + (intermediateVar++);
            output = output + String.Format("var {0} = new ScaleRangeItem({1}, {2}, {3});\n",
                                        scaleRangeVarName,
                                        minScale,
                                        maxScale,
                                        layerVarName);
            output = output + String.Format("{0}.children[{1}] = {2};\n", layerVarName, sc, scaleRangeVarName);

            if(type != 0)
                break;

            int styleIndex = 0;
            for(int ts=0; ts < typeStyles.Length; ts++)
            {
                XmlNodeList typeStyle = scaleRange.GetElementsByTagName(typeStyles[ts]);
                int catIndex = 0;
                for(int st = 0; st < typeStyle.Count; st++)
                {
                    // We will check if this typestyle is going to be shown in the legend
                    XmlNodeList showInLegend = ((XmlElement)typeStyle[st]).GetElementsByTagName("ShowInLegend");
                    if(showInLegend.Count > 0)
                        if(!bool.Parse(showInLegend[0].ChildNodes[0].Value))
                            continue;   // This typestyle does not need to be shown in the legend

                    XmlNodeList rules = ((XmlElement)typeStyle[st]).GetElementsByTagName(ruleNames[ts]);
                    for(int r = 0; r < rules.Count; r++)
                    {
                        XmlElement rule = (XmlElement)rules[r];
                        XmlNodeList label = rule.GetElementsByTagName("LegendLabel");
                        XmlNodeList filter = rule.GetElementsByTagName("Filter");

                        String labelText = "";
                        if(label != null && label.Count > 0 && label[0].ChildNodes.Count > 0)
                            labelText = label[0].ChildNodes[0].Value;
                        String filterText = "";
                        if(filter != null && filter.Count > 0 && filter[0].ChildNodes.Count > 0)
                            filterText = filter[0].ChildNodes[0].Value;

                        output = output + String.Format("{0}.children[{1}] = new StyleItem(\"{2}\", \"{3}\", {4}, {5});\n",
                                                    scaleRangeVarName,
                                                    styleIndex++,
                                                    StrEscape(labelText.Trim()),
                                                    StrEscape(filterText.Trim()),
                                                    ts+1,
                                                    catIndex++);
                    }
                }
            }
        }
        output = output + String.Format("{0}.lyrtype = {1};\n", layerVarName, type.ToString(NumberFormatInfo.InvariantInfo) );
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

String StrEscape(String str)
{
    return StrEscape(str, false);
}

String StrEscape(String str, bool single)
{
    String c = single? "'" : "\"";
    return str.Replace(c, "\\"+c);
}

</script>

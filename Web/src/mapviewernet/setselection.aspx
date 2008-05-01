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

<script runat="server">
String mapName = "";
String sessionId = "";
String selText = "";
bool queryInfo = false;
</script>

<%
    Response.Charset = "utf-8";
    Response.ContentType = "text/xml";

    GetRequestParameters();

    try
    {
        InitializeWebTier();

        MgUserInformation cred = new MgUserInformation(sessionId);
        cred.SetClientIp(GetClientIp(Request));
        cred.SetClientAgent(GetClientAgent());

        //connect to the site and get an instance of the resoucre service
        //
        MgSiteConnection site = new MgSiteConnection();
        site.Open(cred);
        MgResourceService resourceSrvc = (MgResourceService)site.CreateService(MgServiceType.ResourceService);

        //load the map runtime state
        //
        MgMap map = new MgMap();
        map.Open(resourceSrvc, mapName);

        // Create the selection set and save it
        MgSelection sel = new MgSelection(map);
        if (selText != null && selText.Length > 0)
            sel.FromXml(selText);
        sel.Save(resourceSrvc, mapName);

        if (queryInfo)
        {
            //Query feature info for the feature in the selection set. This will return the current set
            //along with property info
            //There must be only one feature in the feature set
            MgReadOnlyLayerCollection layers = sel.GetLayers();
            if (layers == null || layers.GetCount() != 1)
            {
                Response.Write("Error: There must be exactly one feature in the set."); ///NOXLATE dbg report only
                return;
            }
            MgLayer layer = (MgLayer) layers.GetItem(0);
            String featureClassName = layer.GetFeatureClassName();
            String filter = sel.GenerateFilter(layer, featureClassName);
            MgFeatureService featureSrvc = (MgFeatureService)site.CreateService(MgServiceType.FeatureService);
            MgFeatureQueryOptions query = new MgFeatureQueryOptions();
            query.SetFilter(filter);
            MgResourceIdentifier featureSource = new MgResourceIdentifier(layer.GetFeatureSourceId());
            MgFeatureReader features = featureSrvc.SelectFeatures(featureSource, featureClassName, query);
            int featCount = 0;
            while (features.ReadNext())
            {
                if (featCount++ == 1)
                    break;
            }
            if (featCount != 1)
            {
                Response.Write("Error: There must be exactly one feature in the set."); ///NOXLATE dbg report only
                return;
            }
            MgRenderingService renderingSrvc = (MgRenderingService)site.CreateService(MgServiceType.RenderingService);
            MgStringCollection layerNames = new MgStringCollection();
            layerNames.Add(layer.GetName());
            MgFeatureInformation featInfo = renderingSrvc.QueryFeatures(map, layerNames, null, MgFeatureSpatialOperations.Intersects, selText, 1, 2);
            Response.Write(featInfo.ToXml().ToString());
        }
    }
    catch (MgException e)
    {
        Response.Write("\nSetSelection Exception: " + e.GetDetails());
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
    selText = GetParameter(parameters, "SELECTION");
    if(IsParameter(parameters, "QUERYINFO"))
        queryInfo = GetParameter(parameters, "QUERYINFO") == "1";
}

</script>

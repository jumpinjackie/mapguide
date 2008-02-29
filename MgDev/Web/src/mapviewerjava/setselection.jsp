<%--
  -Copyright (C) 2004-2008 by Autodesk, Inc.
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
<%@ include file="common.jsp" %>

<%!
String mapName = "";
String sessionId = "";
String selText = "";
boolean queryInfo;
%>

<%

response.setContentType("text/xml; charset=UTF-8");

request.setCharacterEncoding("UTF-8");
mapName = sessionId = selText = "";
queryInfo = false;

//fetch the parameters for this request
//
GetRequestParameters(request);

try
{
    InitializeWebTier();

    MgUserInformation cred = new MgUserInformation(sessionId);

    //connect to the site and get an instance of the resource service
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
    if(selText != null && selText.length() > 0)
        sel.FromXml(selText);
    sel.Save(resourceSrvc, mapName);

    if(queryInfo) {
        //Query feature info for the feature in the selection set. This will return the current set
        //along with property info
        //There must be only one feature in the feature set
        MgReadOnlyLayerCollection layers = sel.GetLayers();
        if(layers == null && layers.GetCount() != 1) {
            response.getWriter().write("Error: There must be exactly one feature in the set."); ///NOXLATE dbg report only
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
        while(features.ReadNext()) {
            if(featCount++ == 1)
                break;
        }
        if(featCount != 1) {
            response.getWriter().write("Error: There must be exactly one feature in the set."); ///NOXLATE dbg report only
            return;
        }
        MgRenderingService renderingSrvc = (MgRenderingService)site.CreateService(MgServiceType.RenderingService);
        MgStringCollection layerNames = new MgStringCollection();
        layerNames.Add(layer.GetName());
        MgFeatureInformation featInfo = renderingSrvc.QueryFeatures(map, layerNames, null, MgFeatureSpatialOperations.Intersects, selText, 1, 2);
        response.getWriter().write(featInfo.ToXml().ToString());
    }
}
catch(MgException e)
{
    response.getWriter().write("\nClearSelection Exception: " + e.GetDetails());
}
catch(Exception e)
{
}
%>

<%!
void GetRequestParameters(HttpServletRequest request)
{
    if(IsParameter(request, "MAPNAME"))
        mapName = GetParameter(request, "MAPNAME");

    if(IsParameter(request, "SESSION"))
        sessionId = GetParameter(request, "SESSION");

    if(IsParameter(request, "SELECTION"))
        selText = GetParameter(request, "SELECTION");

    if(IsParameter(request, "QUERYINFO"))
        queryInfo = GetParameter(request, "QUERYINFO").equals("1");
}
%>

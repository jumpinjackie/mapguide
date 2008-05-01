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
String layers = "";
String inputSel = "";
String dwf = "";
%>

<%
    response.setContentType("text/html; charset=UTF-8");
    request.setCharacterEncoding("UTF-8");
    PrintWriter outStream = response.getWriter();

    GetRequestParameters(request);

    try
    {
        InitializeWebTier();

        MgUserInformation cred = new MgUserInformation(sessionId);
        cred.SetClientIp(GetClientIp(request));
        cred.SetClientAgent(GetClientAgent());

        //connect to the site and get an instance of each service used in this script
        //
        MgSiteConnection site = new MgSiteConnection();
        site.Open(cred);
        MgFeatureService featureSrvc = (MgFeatureService)site.CreateService(MgServiceType.FeatureService);
        MgRenderingService renderingSrvc = (MgRenderingService)site.CreateService(MgServiceType.RenderingService);
        MgResourceService resourceSrvc = (MgResourceService)site.CreateService(MgServiceType.ResourceService);

        //load the map runtime state
        //
        MgMap map = new MgMap();
        map.Open(resourceSrvc, mapName);

        String[] layersList = layers.split(",");
        if(layersList.length > 0)
        {
            MgStringCollection layerNames = new MgStringCollection();
            for(int i = 0; i < layersList.length; i++)
            {
                layerNames.Add(layersList[i]);
            }

            // create a multi-polygon or a multi-geometry containing the input selected features
            //
            MgGeometry inputGeom = MultiGeometryFromSelection(featureSrvc, map, inputSel);
            if(inputGeom != null)
            {
                // Query all the features belonging the the layer list that intersects with the input geometries
                //
                MgFeatureInformation fi = renderingSrvc.QueryFeatures(map, layerNames, inputGeom, MgFeatureSpatialOperations.Intersects, -1);
                if(fi !=  null)
                {
                    MgSelection resultSel = fi.GetSelection();
                    if(resultSel != null)
                    {
                        // Return XML
                        response.addHeader("Content-type", "text/xml");
                        outStream.write(resultSel.ToXml());
                    }
                }
            }
        }
    }
    catch(MgException exc)
    {
        outStream.write("\nException: " + exc.GetDetails());
        return;
    }
    catch(Exception ne)
    {
        return;
    }

%>

<%!
MgGeometry MultiGeometryFromSelection(MgFeatureService featureSrvc, MgMap map, String selText) throws MgException
{
    MgSelection sel = new MgSelection(map, selText);
    MgReadOnlyLayerCollection selLayers = sel.GetLayers();
    if(selLayers == null)
        return null;
    MgGeometryCollection geomColl = new MgGeometryCollection();
    MgAgfReaderWriter agfRW = new MgAgfReaderWriter();
    boolean polyOnly = true;

    for(int i = 0; i < selLayers.GetCount(); i++)
    {
        MgLayer layer = (MgLayer) selLayers.GetItem(i);
        String filter = sel.GenerateFilter(layer, layer.GetFeatureClassName());

        MgFeatureQueryOptions query = new MgFeatureQueryOptions();
        query.SetFilter(filter);
        MgResourceIdentifier featureSource = new MgResourceIdentifier(layer.GetFeatureSourceId());
        MgFeatureReader features = featureSrvc.SelectFeatures(featureSource, layer.GetFeatureClassName(), query);

        if(features != null)
        {
            MgClassDefinition classDef = features.GetClassDefinition();
            String geomPropName = classDef.GetDefaultGeometryPropertyName();
            int j = 0;
            boolean isPoly = true;
            while(features.ReadNext())
            {
                MgByteReader geomReader = features.GetGeometry(geomPropName);
                MgGeometry geom = agfRW.Read(geomReader);
                if(j++ == 0)
                {
                    int type = geom.GetGeometryType();
                    if(type == MgGeometryType.MultiPolygon || type == MgGeometryType.CurvePolygon || type == MgGeometryType.MultiCurvePolygon)
                    {
                        isPoly = false;
                        polyOnly = false;
                    }
                    else if(type != MgGeometryType.Polygon)
                    {
                        break;
                    }
                }
                geomColl.Add(geom);
            }
            features.Close();
        }
    }

    if(geomColl.GetCount() == 0)
    {
        return null;
    }

    MgGeometryFactory gf = new MgGeometryFactory();
    if(polyOnly)
    {
        MgPolygonCollection polyColl = new MgPolygonCollection();
        for(int j = 0; j < geomColl.GetCount(); j++)
        {
            polyColl.Add((MgPolygon)geomColl.GetItem(j));
        }
        return gf.CreateMultiPolygon(polyColl);
    }
    else
    {
        return gf.CreateMultiGeometry(geomColl);
    }
}

void GetRequestParameters(HttpServletRequest request)
{
    mapName = GetParameter(request, "MAPNAME");
    sessionId = GetParameter(request, "SESSION");
    inputSel = GetParameter(request, "SELECTION");
    layers = GetParameter(request, "LAYERS");
    dwf = GetParameter(request, "DWF");
}
%>

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
<%@ Import Namespace="System.Collections" %>
<%@ Import Namespace="System.Collections.Specialized" %>
<%@ Import Namespace="System.Text" %>
<%@ Import Namespace="OSGeo.MapGuide" %>

<!-- #Include File="common.aspx -->

<script runat="server">
String mapName = "";
String sessionId = "";
String layers = "";
String inputSel = "";
String dwf = "";
</script>

<%
    Response.Charset = "utf-8";

    GetRequestParameters();

    try
    {
        InitializeWebTier();

        MgUserInformation cred = new MgUserInformation(sessionId);
        cred.SetClientIp(GetClientIp(Request));
        cred.SetClientAgent(GetClientAgent());

        //connect to the site and get an instance of each service used in this script
        //
        MgSiteConnection site = new MgSiteConnection();
        site.Open(cred);
        MgFeatureService featureSrvc = site.CreateService(MgServiceType.FeatureService) as MgFeatureService;
        MgRenderingService renderingSrvc = site.CreateService(MgServiceType.RenderingService) as MgRenderingService;
        MgResourceService resourceSrvc = site.CreateService(MgServiceType.ResourceService) as MgResourceService;

        //load the map runtime state
        //
        MgMap map = new MgMap();
        map.Open(resourceSrvc, mapName);

        String [] layersList = layers.Split(',');
        if(layersList.Length > 0)
        {
            MgStringCollection layerNames = new MgStringCollection();
            for(int i = 0; i < layersList.Length; i++)
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
                        Response.AddHeader("Content-type", "text/xml");
                        Response.Write(resultSel.ToXml());
                    }
                }
            }
        }
    }
    catch(MgException exc)
    {
        Response.Write("\nException: " + exc.GetDetails());
        return;
    }
    catch(Exception ne)
    {
        return;
    }
%>

<script runat="server">
MgGeometry MultiGeometryFromSelection(MgFeatureService featureSrvc, MgMap map, String selText)
{
    MgSelection sel = new MgSelection(map, selText);
    MgReadOnlyLayerCollection selLayers = sel.GetLayers();
    if(selLayers == null)
        return null;
    MgGeometryCollection geomColl = new MgGeometryCollection();
    MgAgfReaderWriter agfRW = new MgAgfReaderWriter();
    bool polyOnly = true;

    for(int i = 0; i < selLayers.GetCount(); i++)
    {
        MgLayer layer = (MgLayer) selLayers.GetItem(i);

        // TODO:  How to get selectionSize?
        int selectionSize = 20;
        MgStringCollection filters = sel.GenerateFilters(layer, layer.GetFeatureClassName(), selectionSize);
        
        int numFilters = filters.GetCount();
        
        for (int filterIndex = 0; filterIndex < numFilters; filterIndex++)
        {
            String filter = filters.GetItem(filterIndex);
            if (filter == "")
                continue;

            MgFeatureQueryOptions query = new MgFeatureQueryOptions();
            query.SetFilter(filter);
            MgResourceIdentifier featureSource = new MgResourceIdentifier(layer.GetFeatureSourceId());
            MgFeatureReader features = featureSrvc.SelectFeatures(featureSource, layer.GetFeatureClassName(), query);

            if(features != null)
            {
                MgClassDefinition classDef = features.GetClassDefinition();
                String geomPropName = classDef.GetDefaultGeometryPropertyName();
                int j = 0;
                bool isPoly = true;
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
    }

    if (geomColl.GetCount() == 0)
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
    inputSel = GetParameter(parameters, "SELECTION");
    layers = GetParameter(parameters, "LAYERS");
    dwf = GetParameter(parameters, "DWF");
}

</script>

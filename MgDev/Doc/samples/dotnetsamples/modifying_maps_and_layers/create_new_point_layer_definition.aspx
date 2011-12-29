<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<%--
  Copyright (C) 2004-2011 by Autodesk, Inc.
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
<%@ Import Namespace="System.Xml" %>
<%@ Import Namespace="OSGeo.MapGuide" %>
<!-- #Include File="layer_functions.aspx" -->z
<script runat="server">
String sessionId;
String mapName;

///////////////////////////////////////////////////////////////////////////////////
MgPropertyCollection MakePoint(String name, double x, double y)
{
	MgPropertyCollection propertyCollection = new MgPropertyCollection();
	MgStringProperty nameProperty = new MgStringProperty("NAME", name);
	propertyCollection.Add(nameProperty);

	MgWktReaderWriter wktReaderWriter = new MgWktReaderWriter();
	MgAgfReaderWriter agfReaderWriter = new MgAgfReaderWriter();

	MgGeometry geometry = wktReaderWriter.Read("POINT XY (" + x + " " + y + ")");
	MgByteReader geometryByteReader = agfReaderWriter.Write(geometry);
	MgGeometryProperty geometryProperty = new MgGeometryProperty("GEOM", geometryByteReader);
	propertyCollection.Add(geometryProperty);

	return propertyCollection;
}

</script>
<html>

<head>
    <meta content="text/html; charset=utf-8" http-equiv="Content-Type">
    <meta http-equiv="content-script-type" content="text/javascript">
    <meta http-equiv="content-style-type" content="text/css">
    <link href="../styles/globalStyles.css" rel="stylesheet" type="text/css">
    <link href="../styles/otherStyles.css" rel="stylesheet"  type="text/css">
    <script language="javascript" type="text/javascript">
        function OnPageLoad()
        {
            parent.mapFrame.Refresh();
            parent.mapFrame.ZoomToScale(40000);
        }
    </script>
</head>

<body onLoad="OnPageLoad()">

<%
try
{
    //---------------------------------------------------//
    // Initialize
    MapGuideApi.MgInitializeWebTier(Constants.WebConfigPath);
    sessionId = Request.Params["SESSION"];
    mapName = Request.Params["MAPNAME"];
    MgUserInformation userInfo = new MgUserInformation(sessionId);
    MgSiteConnection siteConnection = new MgSiteConnection();
    siteConnection.Open(userInfo);
    MgResourceService resourceService = (MgResourceService)siteConnection.CreateService(MgServiceType.ResourceService);
    MgFeatureService featureService = (MgFeatureService)siteConnection.CreateService(MgServiceType.FeatureService);

    //---------------------------------------------------//
    // Open the map
    MgMap map = new MgMap(siteConnection);
    map.Open(mapName);

    //---------------------------------------------------//
    // Create a feature source with point data.
    // (The Sheboygan sample data does not contain such data,
    // so we"ll create it.)

	// Create a feature class definition for the new feature source
	MgClassDefinition classDefinition = new MgClassDefinition();
	classDefinition.SetName("Points");
	classDefinition.SetDescription("Feature class with point data.");
	classDefinition.SetDefaultGeometryPropertyName("GEOM");

    MgPropertyDefinitionCollection idProps = classDefinition.GetIdentityProperties();
    MgPropertyDefinitionCollection clsProps = classDefinition.GetProperties();
    
	// Create an identify property
	MgDataPropertyDefinition identityProperty = new MgDataPropertyDefinition("KEY");
	identityProperty.SetDataType(MgPropertyType.Int32);
	identityProperty.SetAutoGeneration(true);
	identityProperty.SetReadOnly(true);
	// Add the identity property to the class definition
    clsProps.Add(identityProperty);
    idProps.Add(identityProperty);

	// Create a name property
	MgDataPropertyDefinition nameProperty = new MgDataPropertyDefinition("NAME");
	nameProperty.SetDataType(MgPropertyType.String);
	// Add the name property to the class definition
    clsProps.Add(nameProperty);

	// Create a geometry property
	MgGeometricPropertyDefinition geometryProperty = new MgGeometricPropertyDefinition("GEOM");
	geometryProperty.SetGeometryTypes(MgFeatureGeometricType.Point);
	// Add the geometry property to the class definition
    clsProps.Add(geometryProperty);

	// Create a feature schema
	MgFeatureSchema featureSchema = new MgFeatureSchema("PointSchema", "Point schema");
    MgClassDefinitionCollection classes = featureSchema.GetClasses();
	// Add the feature schema to the class definition
    classes.Add(classDefinition);

	// Create the feature source
	String featureSourceName = "Library://Samples/DevGuide/Data/points.FeatureSource";
	MgResourceIdentifier resourceIdentifier = new MgResourceIdentifier(featureSourceName);
	//wkt = "LOCALCS[\"*XY-MT*\",LOCAL_DATUM[\"*X-Y*\",10000],UNIT[\"Meter\", 1],AXIS[\"X\",EAST],AXIS[\"Y\",NORTH]]";
    String wkt = "GEOGCS[\"LL84\",DATUM[\"WGS84\",SPHEROID[\"WGS84\",6378137.000,298.25722293]],PRIMEM[\"Greenwich\",0],UNIT[\"Degree\",0.01745329251994]]";
    MgCreateSdfParams sdfParams = new MgCreateSdfParams("LatLong", wkt, featureSchema);
    featureService.CreateFeatureSource(resourceIdentifier, sdfParams);

	// We need to add some data to the sdf before using it.  The spatial context
	// reader must have an extent.
	MgBatchPropertyCollection batchPropertyCollection = new MgBatchPropertyCollection();
	MgWktReaderWriter wktReaderWriter = new MgWktReaderWriter();
	MgAgfReaderWriter agfReaderWriter = new MgAgfReaderWriter();
	MgGeometryFactory geometryFactory = new MgGeometryFactory();

    // Make four points
	batchPropertyCollection.Add(MakePoint("Point A", -87.727, 43.748));

	batchPropertyCollection.Add(MakePoint("Point B", -87.728, 43.730));

	batchPropertyCollection.Add(MakePoint("Point C", -87.726, 43.750));

	batchPropertyCollection.Add(MakePoint("Point D", -87.728, 43.750));

	// Add the batch property collection to the feature source
	MgInsertFeatures cmd = new MgInsertFeatures("Points", batchPropertyCollection);
	MgFeatureCommandCollection featureCommandCollection = new MgFeatureCommandCollection();
	featureCommandCollection.Add(cmd);

	// Execute the "add" commands
	featureService.UpdateFeatures(resourceIdentifier, featureCommandCollection, false);

    // ...
    //---------------------------------------------------//
    // Create a new layer

    LayerDefinitionFactory factory = new LayerDefinitionFactory(this.Server);

    // Create a mark symbol
    String resourceId = "Library://Samples/Sheboygan/Symbols/BasicSymbols.SymbolLibrary";
    String symbolName = "PushPin";
    String width = "24";  // unit = points
    String height = "24"; // unit = points
    String color = "FFFF0000";
    String markSymbol = factory.CreateMarkSymbol(resourceId, symbolName, width, height, color);

    // Create a text symbol
    String text = "ID";
    String fontHeight="12";
    String foregroundColor = "FF000000";
    String textSymbol = factory.CreateTextSymbol(text, fontHeight, foregroundColor);

    // Create a point rule.
    String legendLabel = "trees";
    String filter = "";
    String pointRule = factory.CreatePointRule(legendLabel, filter, textSymbol, markSymbol);

    // Create a point type style.
    String pointTypeStyle = factory.CreatePointTypeStyle(pointRule);

    // Create a scale range.
    String minScale = "0";
    String maxScale = "1000000000000";
    String pointScaleRange = factory.CreateScaleRange(minScale, maxScale, pointTypeStyle);

    // Create the layer definiton.
    String featureName = "PointSchema:Points";
    String geometry = "GEOM";
    String layerDefinition = factory.CreateLayerDefinition(featureSourceName, featureName, geometry, pointScaleRange);
    //---------------------------------------------------//
    // ...

    XmlDocument doc = new XmlDocument();
    doc.LoadXml(layerDefinition);
    // Add the layer to the map
    MgLayer newLayer = LayerFunctions.AddLayerDefinitionToMap(doc, "Points", "Points of Interest", sessionId, resourceService, map);
    LayerFunctions.AddLayerToGroup(newLayer, "Analysis", "Analysis", map);

    // --------------------------------------------------//
    // Turn on the visibility of this layer.
    // (If the layer does not already exist in the map, it will be visible by default when it is added.
    // But if the user has already run this script, he or she may have set the layer to be invisible.)
    MgLayerCollection layerCollection = map.GetLayers();
    if (layerCollection.Contains("Points"))
    {
        MgLayerBase pointsLayer =layerCollection.GetItem("Points");
        pointsLayer.SetVisible(true);
    }

    //---------------------------------------------------//
    //  Save the map back to the session repository
    map.Save();

    //---------------------------------------------------//
}
catch (MgException e)
{
	Response.Write("<script language=\"javascript\" type=\"text/javascript\"> \n");
	Response.Write("    alert(\" " + e.GetExceptionMessage() + " \"); \n");
	Response.Write("</script> \n");
}

%>

</body>
</html>



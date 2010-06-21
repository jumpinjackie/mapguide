<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<!--
//  Copyright (C) 2004-2010 by Autodesk, Inc.
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of version 2.1 of the GNU Lesser
//  General Public License as published by the Free Software Foundation.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
-->
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

<?php
require_once('../common/common.php');
require_once($webExtensionsDirectory . 'www/mapviewerphp/layerdefinitionfactory.php');
require_once('layer_functions.php');
try
{
    //---------------------------------------------------//
    // Initialize
    MgInitializeWebTier($webconfigFilePath);
    $args = ($_SERVER['REQUEST_METHOD'] == "POST") ? $_POST : $_GET;
    $sessionId = $args['SESSION'];
    $mapName = $args['MAPNAME'];
    $userInfo = new MgUserInformation($sessionId);
    $siteConnection = new MgSiteConnection();
    $siteConnection->Open($userInfo);
    $resourceService = $siteConnection->CreateService(MgServiceType::ResourceService);
    $featureService = $siteConnection->CreateService(MgServiceType::FeatureService);

    //---------------------------------------------------//
    // Open the map
    $map = new MgMap();
    $map->Open($resourceService, $mapName);

    //---------------------------------------------------//
    // Create a feature source with point data.
    // (The Sheboygan sample data does not contain such data,
    // so we'll create it.)

	// Create a feature class definition for the new feature source
	$classDefinition = new MgClassDefinition();
	$classDefinition->SetName("Points");
	$classDefinition->SetDescription("Feature class with point data.");
	$classDefinition->SetDefaultGeometryPropertyName("GEOM");

	// Create an identify property
	$identityProperty = new MgDataPropertyDefinition("KEY");
	$identityProperty->SetDataType(MgPropertyType::Int32);
	$identityProperty->SetAutoGeneration(true);
	$identityProperty->SetReadOnly(true);
	// Add the identity property to the class definition
	$classDefinition->GetIdentityProperties()->Add($identityProperty);
	$classDefinition->GetProperties()->Add($identityProperty);

	// Create a name property
	$nameProperty = new MgDataPropertyDefinition("NAME");
	$nameProperty->SetDataType(MgPropertyType::String);
	// Add the name property to the class definition
	$classDefinition->GetProperties()->Add($nameProperty);

	// Create a geometry property
	$geometryProperty = new MgGeometricPropertyDefinition("GEOM");
	$geometryProperty->SetGeometryTypes(MgFeatureGeometricType::Surface);
	// Add the geometry property to the class definition
	$classDefinition->GetProperties()->Add($geometryProperty);

	// Create a feature schema
	$featureSchema = new MgFeatureSchema("PointSchema", "Point schema");
	// Add the feature schema to the class definition
	$featureSchema->GetClasses()->Add($classDefinition);

	// Create the feature source
	$featureSourceName = 'Library://Samples/DevGuide/Data/points.FeatureSource';
	$resourceIdentifier = new MgResourceIdentifier($featureSourceName);
	$wkt = "LOCALCS[\"*XY-MT*\",LOCAL_DATUM[\"*X-Y*\",10000],UNIT[\"Meter\", 1],AXIS[\"X\",EAST],AXIS[\"Y\",NORTH]]";
	$sdfParams = new MgCreateSdfParams("ArbitraryXY", $wkt, $featureSchema);
    $featureService->CreateFeatureSource($resourceIdentifier, $sdfParams);

	// We need to add some data to the sdf before using it.  The spatial context
	// reader must have an extent.
	$batchPropertyCollection = new MgBatchPropertyCollection();
	$wktReaderWriter = new MgWktReaderWriter();
	$agfReaderWriter = new MgAgfReaderWriter();
	$geometryFactory = new MgGeometryFactory();

    // Make four points
	$propertyCollection = MakePoint("Point A", -87.727, 43.748);
	$batchPropertyCollection->Add($propertyCollection);
	unset($propertyCollection);

	$propertyCollection = MakePoint("Point B", -87.728, 43.730);
	$batchPropertyCollection->Add($propertyCollection);
	unset($propertyCollection);

	$propertyCollection = MakePoint("Point C", -87.726, 43.750);
	$batchPropertyCollection->Add($propertyCollection);
	unset($propertyCollection);

	$propertyCollection = MakePoint("Point D", -87.728, 43.750);
	$batchPropertyCollection->Add($propertyCollection);
	unset($propertyCollection);

	// Add the batch property collection to the feature source
	$cmd = new MgInsertFeatures("Points", $batchPropertyCollection);
	$featureCommandCollection = new MgFeatureCommandCollection();
	$featureCommandCollection->Add($cmd);

	// Execute the "add" commands
	$featureService->UpdateFeatures($resourceIdentifier, $featureCommandCollection, false);

    // ...
    //---------------------------------------------------//
    // Create a new layer

    $factory = new LayerDefinitionFactory();

    // Create a mark symbol
    $resourceId = 'Library://Samples/Sheboygan/'
      . 'Symbols/BasicSymbols.SymbolLibrary';
    $symbolName = 'PushPin';
    $width = '24';  // unit = points
    $height = '24'; // unit = points
    $color = 'FFFF0000';
    $markSymbol = $factory->CreateMarkSymbol($resourceId,
      $symbolName, $width, $height, $color);

    // Create a text symbol
    $text = "ID";
    $fontHeight="12";
    $foregroundColor = 'FF000000';
    $textSymbol = $factory->CreateTextSymbol($text,
      $fontHeight, $foregroundColor);

    // Create a point rule.
    $legendLabel = 'trees';
    $filter = '';
    $pointRule = $factory->CreatePointRule($legendLabel,
      $filter, $textSymbol, $markSymbol);

    // Create a point type style.
    $pointTypeStyle = $factory->
      CreatepointTypeStyle($pointRule);

    // Create a scale range.
    $minScale = '0';
    $maxScale = '1000000000000';
    $pointScaleRange = $factory->CreateScaleRange(
      $minScale, $maxScale, $pointTypeStyle);

    // Create the layer definiton.
    $featureName = 'PointSchema:Points';
    $geometry = 'GEOM';
    $layerDefinition = $factory->
      CreateLayerDefinition($featureSourceName,
      $featureName, $geometry, $pointScaleRange);
    //---------------------------------------------------//
    // ...

    // Add the layer to the map
    $newLayer = add_layer_definition_to_map($layerDefinition, "Points", "Points of Interest", $sessionId, $resourceService, $map);
    add_layer_to_group($newLayer, "Analysis", "Analysis", $map);

    // --------------------------------------------------//
    // Turn on the visibility of this layer.
    // (If the layer does not already exist in the map, it will be visible by default when it is added.
    // But if the user has already run this script, he or she may have set the layer to be invisible.)
    $layerCollection = $map->GetLayers();
    if ($layerCollection->Contains("Points"))
    {
        $pointsLayer =$layerCollection->GetItem("Points");
        $pointsLayer->SetVisible(true);
    }

    //---------------------------------------------------//
    //  Save the map back to the session repository
    $sessionIdName = "Session:$sessionId//$mapName.Map";
    $sessionResourceID = new MgResourceIdentifier($sessionIdName);
    $sessionResourceID->Validate();
    $map->Save($resourceService, $sessionResourceID);

    //---------------------------------------------------//
}
catch (MgException $e)
{
	echo "<script language=\"javascript\" type=\"text/javascript\"> \n";
	echo "    alert(\" " . $e->GetExceptionMessage() . " \"); \n";
	echo "</script> \n";
}

///////////////////////////////////////////////////////////////////////////////////
function MakePoint($name, $x, $y)
{
	$propertyCollection = new MgPropertyCollection();
	$nameProperty = new MgStringProperty("NAME", $name);
	$propertyCollection->Add($nameProperty);

	$wktReaderWriter = new MgWktReaderWriter();
	$agfReaderWriter = new MgAgfReaderWriter();

	$geometry = $wktReaderWriter->Read("POINT XY ($x $y)");
	$geometryByteReader = $agfReaderWriter->Write($geometry);
	$geometryProperty = new MgGeometryProperty("GEOM", $geometryByteReader);
	$propertyCollection->Add($geometryProperty);

	return $propertyCollection;
}
?>

</body>
</html>



<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<!--
//  Copyright (C) 2004-2011 by Autodesk, Inc.
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
    <link href="../styles/otherStyles.css" rel="stylesheet" type="text/css">

    <script language="javascript" type="text/javascript">
        function OnPageLoad()
        {
            parent.mapFrame.Refresh();
            parent.mapFrame.ZoomToScale(9999);
        }
    </script>
</head>

<body onLoad="OnPageLoad()">

<?php
require_once('../common/common.php');
require_once('../common/layerdefinitionfactory.php');
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

    // --------------------------------------------------//
    // Open the map
    $map = new MgMap($siteConnection);
    $map->Open($mapName);


	// ...
    //---------------------------------------------------//
    // Create a new layer

    $factory = new LayerDefinitionFactory();

    /// Create three area rules for three different
    // scale ranges.
    $areaRule1 = $factory->CreateAreaRule(    '1 to 800',
      'SQFT &gt;= 1 AND SQFT &lt; 800',     'FFFFFF00');
    $areaRule2 = $factory->CreateAreaRule( '800 to 1600',
      'SQFT &gt;= 800 AND SQFT &lt; 1600',  'FFFFBF20');
    $areaRule3 = $factory->CreateAreaRule('1600 to 2400',
      'SQFT &gt;= 1600 AND SQFT &lt; 2400', 'FFFF8040');

    // Create an area type style.
    $areaTypeStyle = $factory->CreateAreaTypeStyle(
      $areaRule1 . $areaRule2 . $areaRule3);

    // Create a scale range.
    $minScale = '0';
    $maxScale = '10000';
    $areaScaleRange = $factory->CreateScaleRange(
      $minScale, $maxScale, $areaTypeStyle);

    // Create the layer definiton.
    $featureClass = 'Library://Samples/Sheboygan/Data/'
    .  'Parcels.FeatureSource';
    $featureName = 'SHP_Schema:Parcels';
    $geometry = 'SHPGEOM';
    $layerDefinition = $factory->CreateLayerDefinition(
      $featureClass, $featureName, $geometry,
      $areaScaleRange);

    //---------------------------------------------------//
    // ...

    // Add the layer to the map
    $newLayer = add_layer_definition_to_map($layerDefinition, "SquareFootage", "Square Footage", $sessionId, $resourceService, $map);
    add_layer_to_group($newLayer, "Analysis", "Analysis", $map);

    //---------------------------------------------------//
    // Turn off the "Recently Built" themed layer (if it exists) so it does not hide this layer.
    $layerCollection = $map->GetLayers();
    if ($layerCollection->Contains("RecentlyBuilt"))
    {
        $recentlyBuiltLayer =$layerCollection->GetItem("RecentlyBuilt");
        $recentlyBuiltLayer->SetVisible(false);
    }

    // --------------------------------------------------//
    // Turn on the visibility of this layer.
    // (If the layer does not already exist in the map, it will be visible by default when it is added.
    // But if the user has already run this script, he or she may have set the layer to be invisible.)
    $layerCollection = $map->GetLayers();
    if ($layerCollection->Contains("SquareFootage"))
    {
        $squareFootageLayer =$layerCollection->GetItem("SquareFootage");
        $squareFootageLayer->SetVisible(true);
    }

    //---------------------------------------------------//
    //  Save the map back to the session repository
    $map->Save();

    //---------------------------------------------------//
}
catch (MgException $e)
{
	echo "<script language=\"javascript\" type=\"text/javascript\"> \n";
	echo "    alert(\" " . $e->GetExceptionMessage() . " \"); \n";
	echo "</script> \n";
}
?>

</body>
</html>


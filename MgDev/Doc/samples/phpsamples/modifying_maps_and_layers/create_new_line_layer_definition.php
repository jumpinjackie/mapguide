<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<!--
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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

    // --------------------------------------------------//
    // Open the map
    $map = new MgMap();
    $map->Open($resourceService, $mapName);

	// ...
    //---------------------------------------------------//
    // Create a new layer
    
	$factory = new LayerDefinitionFactory();
	    
    // Create a line rule.
    $legendLabel = '';
    $filter = '';
    $color = 'FF0000FF';
	$lineRule = $factory->CreateLineRule(
	  $legendLabel, $filter, $color);
	
    // Create a line type style.
    $lineTypeStyle = $factory->
      CreateLineTypeStyle($lineRule);
      
    // Create a scale range.
    $minScale = '0';
    $maxScale = '1000000000000';
    $lineScaleRange = $factory->
      CreateScaleRange($minScale, $maxScale, 
      $lineTypeStyle);   

    // Create the layer definiton.
    $featureClass = 'Library://Samples/Sheboygan/Data/'
      . 'HydrographicLines.FeatureSource';
    $featureName = 'SHP_Schema:HydrographicLines';
    $geometry = 'SHPGEOM';
	$layerDefinition = $factory->
	  CreateLayerDefinition($featureClass, $featureName, 
	  $geometry, $lineScaleRange);
    
    //---------------------------------------------------//  
    // ... 
          
    // Add the layer to the map
    $newLayer = add_layer_definition_to_map($layerDefinition, "Hydro", "Hydro", $sessionId, $resourceService, $map);
    add_layer_to_group($newLayer, "Analysis", "Analysis", $map);
	
    // --------------------------------------------------//
    // Turn on the visibility of this layer.
    // (If the layer does not already exist in the map, it will be visible by default when it is added.
    // But if the user has already run this script, he or she may have set the layer to be invisible.)
    $layerCollection = $map->GetLayers();
    if ($layerCollection->Contains("Hydro"))
    {
        $squareFootageLayer =$layerCollection->GetItem("Hydro");
        $squareFootageLayer->SetVisible(true);
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
	echo "    alert(\" " . $e->GetMessage() . " \"); \n";
	echo "</script> \n";
}
?>

</body>
</html>



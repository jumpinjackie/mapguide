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
require_once('layer_functions.php');
try
{
    // --------------------------------------------------//
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
    // --------------------------------------------------//
    // Load a layer from XML, and use the DOM to change it

    // Load the prototype layer definition into
    // a PHP DOM object.
    $domDocument =
      DOMDocument::load('RecentlyBuilt.LayerDefinition');
    if ($domDocument == NULL)
    {
        echo "The layer definition
          'RecentlyBuilt.LayerDefinition' could not be
          found.<BR>\n";
        return;
    }

    // Change the filter
    $xpath = new DOMXPath($domDocument);
    $query = '//AreaRule/Filter';
    // Get a list of all the <AreaRule><Filter> elements in
    // the XML.   
    $nodes = $xpath->query($query);
    // Find the correct node and change it
    foreach ($nodes as $node )
    {
        if ($node->nodeValue == 'YRBUILT > 1950')
        {
            $node->nodeValue = 'YRBUILT > 1980';
        }
    }

    // Change the legend label
    $query = '//LegendLabel';
    // Get a list of all the <LegendLabel> elements in the
    // XML.
    $nodes = $xpath->query($query);
    // Find the correct node and change it
    foreach ($nodes as $node )
    {
        if ($node->nodeValue == 'Built after 1950')
        {
            $node->nodeValue = 'Built after 1980';
        }
    }

    // --------------------------------------------------//
  // ...
  
    // Add the layer to the map
    $layerDefinition = $domDocument->saveXML();
    $newLayer = add_layer_definition_to_map($layerDefinition, "RecentlyBuilt", "Built after 1980", $sessionId, $resourceService, $map);
    add_layer_to_group($newLayer, "Analysis", "Analysis", $map);

    // --------------------------------------------------//
    // Turn off the "Square Footage" themed layer (if it
    // exists) so it does not hide this layer.
    $layerCollection = $map->GetLayers();
    if ($layerCollection->Contains("SquareFootage"))
    {
        $squareFootageLayer =
          $layerCollection->GetItem("SquareFootage");
        $squareFootageLayer->SetVisible(false);
    }

    // --------------------------------------------------//
    // Turn on the visibility of this layer.
    // (If the layer does not already exist in the map, it will be visible by default when it is added.
    // But if the user has already run this script, he or she may have set the layer to be invisible.)
    $layerCollection = $map->GetLayers();
    if ($layerCollection->Contains("RecentlyBuilt"))
    {
        $recentlyBuiltLayer = $layerCollection->GetItem("RecentlyBuilt");
        $recentlyBuiltLayer->SetVisible(true);
    }
    
    // --------------------------------------------------//
    //  Save the map back to the session repository
    $sessionIdName = "Session:$sessionId//$mapName.Map";
    $sessionResourceID = new MgResourceIdentifier($sessionIdName);
    $sessionResourceID->Validate();
    $map->Save($resourceService, $sessionResourceID);

    // --------------------------------------------------//


}
catch (MgException $e)
{
  echo "<script language=\"javascript\" type=\"text/javascript\"> \n";
  $message = $e->GetMessage();
  $message = str_replace("\n", " ", $message);
  echo "    alert(\" " . $message . " \"); \n";
  echo "</script> \n";
}
?>

</body>
</html>

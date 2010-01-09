<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
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
<head>
  <meta content="text/html; charset=utf-8" http-equiv="Content-Type">
  <title>Draw a Line</title>
  
  <meta http-equiv="content-style-type" content="text/css">
  <link href="../styles/globalStyles.css" rel="stylesheet"  type="text/css">
  <link href="../styles/otherStyles.css" rel="stylesheet" type="text/css">
  <meta http-equiv="content-script-type" content="text/javascript">
  <script language="javascript" type="text/javascript">
    function OnPageLoad()
    {
      parent.mapFrame.Refresh();
    }
  </script>
</head>

<body onLoad="OnPageLoad()">
  <?php
  require_once('../common/common.php');
  require_once($webExtensionsDirectory . 'www/mapviewerphp/layerdefinitionfactory.php');  
  require_once('../modifying_maps_and_layers/layer_functions.php');
  
  // Get the parameters passed in from the task pane
  $args = ($_SERVER['REQUEST_METHOD'] == "POST")?$_POST: $_GET;
  $x0 = $args['x0'];
  $y0 = $args['y0'];
  $x1 = $args['x1'];
  $y1 = $args['y1'];
  $sessionId = $args['SESSION'];
  $mapName = $args['MAPNAME'];
  
  try
  {
    // --------------------------------------------------//
    // Basic initialization needs to be done every time.
    MgInitializeWebTier($webconfigFilePath);
    
    // Get the session information passed from the viewer.
    $sessionId = ($_SERVER['REQUEST_METHOD'] == "POST")?$_POST['SESSION']: $_GET['SESSION'];
       
    // Get the user information using the session id,
    // and set up a connection to the site server.     
    $userInfo = new MgUserInformation($sessionId);
    $siteConnection = new MgSiteConnection();
    $siteConnection->Open($userInfo);
    
    // Get an instance of the required service(s).    
    $resourceService = $siteConnection->
    CreateService(MgServiceType::ResourceService);
    $featureService = $siteConnection->CreateService(MgServiceType::FeatureService);      
  
    //---------------------------------------------------//
    // Open the map
    $map = new MgMap();
    $map->Open($resourceService, $mapName);
        
    $layerName = "Lines";
    $layerLegendLabel = "New Lines";
    $groupName = "Analysis";
    $groupLegendLabel = "Analysis";   
    
    //---------------------------------------------------//
    // Does the temporary feature source already exist?
    // If not, create it
    $featureSourceName = "Session:$sessionId//TemporaryLines.FeatureSource"; 
    $resourceIdentifier = new MgResourceIdentifier($featureSourceName);
    
    $featureSourceExists = DoesResourceExist($resourceIdentifier, $resourceService);
    if (! $featureSourceExists) 
    {             
      // Create a temporary feature source to draw the lines on
                              
      // Create a feature class definition for the new feature 
      // source
      $classDefinition = new MgClassDefinition();
      $classDefinition->SetName("Lines");
      $classDefinition->SetDescription("Lines to display.");
      $geometryPropertyName="SHPGEOM";
      $classDefinition->
        SetDefaultGeometryPropertyName( $geometryPropertyName); 
      
      // Create an identify property
      $identityProperty = new MgDataPropertyDefinition("KEY");
      $identityProperty->SetDataType(MgPropertyType::Int32);
      $identityProperty->SetAutoGeneration(true);
      $identityProperty->SetReadOnly(true); 
      // Add the identity property to the class definition
      $classDefinition->GetIdentityProperties()->
        Add($identityProperty);
      $classDefinition->GetProperties()->Add($identityProperty);
      
      // Create a name property
      $nameProperty = new MgDataPropertyDefinition("NAME");
      $nameProperty->SetDataType(MgPropertyType::String); 
      // Add the name property to the class definition
      $classDefinition->GetProperties()->Add($nameProperty);
      
      // Create a geometry property
      $geometryProperty = new 
        MgGeometricPropertyDefinition($geometryPropertyName);   
      $geometryProperty->
        SetGeometryTypes(MgFeatureGeometricType::Surface);  
      // Add the geometry property to the class definition
      $classDefinition->GetProperties()->Add($geometryProperty); 
      
      // Create a feature schema
      $featureSchema = new MgFeatureSchema("SHP_Schema", 
        "Line schema");
      // Add the feature schema to the class definition
      $featureSchema->GetClasses()->Add($classDefinition);             
      
      // Create the feature source
      $wkt = $map->GetMapSRS();
      $sdfParams = new MgCreateSdfParams("spatial context", 
        $wkt, $featureSchema);  
      $featureService->CreateFeatureSource($resourceIdentifier, 
        $sdfParams);          
    }  
    
    // Add the line to the feature source
    $batchPropertyCollection = new MgBatchPropertyCollection();
    $propertyCollection = MakeLine("Line A", $x0, $y0, $x1, $y1);
    $batchPropertyCollection->Add($propertyCollection);
          
    // Add the batch property collection to the feature source
    $cmd = new MgInsertFeatures($layerName, $batchPropertyCollection); 
    $featureCommandCollection = new MgFeatureCommandCollection();
    $featureCommandCollection->Add($cmd);
    
    // Execute the "add" commands
    $featureService->UpdateFeatures($resourceIdentifier, $featureCommandCollection, false);     
    
    //---------------------------------------------------//
    $layerExists = DoesLayerExist($layerName, $map);
    if (! $layerExists )
    {
      // Create a new layer which uses that feature source         
        
      // Create a line rule to stylize the lines
      $ruleLegendLabel = 'Lines Rule';
      $filter = '';
      $color = 'FF0000FF';
      $factory = new LayerDefinitionFactory();
      $lineRule = $factory->CreateLineRule($ruleLegendLabel, $filter, $color);
      
      // Create a line type style
      $lineTypeStyle = $factory->CreateLineTypeStyle($lineRule);
      
      // Create a scale range
      $minScale = '0';
      $maxScale = '1000000000000';
      $lineScaleRange = $factory->CreateScaleRange($minScale, $maxScale, $lineTypeStyle);   
    
      // Create the layer definiton
      $featureName = 'SHP_Schema:Lines';
      $geometry = 'SHPGEOM';
      $layerDefinition = $factory->CreateLayerDefinition($featureSourceName, $featureName, $geometry, $lineScaleRange);
      
      //---------------------------------------------------//  
      // Add the layer to the map
      $newLayer = add_layer_definition_to_map($layerDefinition, $layerName, $layerLegendLabel, $sessionId, $resourceService, $map);
      // Add the layer to a layer group
      add_layer_to_group($newLayer,$groupName, $groupLegendLabel, $map);
    }
  
    // --------------------------------------------------//
    // Turn on the visibility of this layer.
    // (If the layer does not already exist in the map, it will be visible by default when it is added.
    // But if the user has already run this script, he or she may have set the layer to be invisible.)
    $layerCollection = $map->GetLayers();
    if ($layerCollection->Contains($layerName)) 
    {
      $linesLayer =$layerCollection->GetItem($layerName);
      $linesLayer->SetVisible(true);
    }
    
    $groupCollection = $map->GetLayerGroups();
    if ($groupCollection->Contains($groupName))
    {
      $analysisGroup =$groupCollection->GetItem($groupName);
      $analysisGroup->SetVisible(true);
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
  
  ///////////////////////////////////////////////////////////////////////////////////
  function MakeLine($name, $x0, $y0 , $x1, $y1)
  {
    $propertyCollection = new MgPropertyCollection();
    $nameProperty = new MgStringProperty("NAME", $name);
    $propertyCollection->Add($nameProperty);
    
    $wktReaderWriter = new MgWktReaderWriter();
    $agfReaderWriter = new MgAgfReaderWriter(); 
    
    $geometry = $wktReaderWriter->Read("LINESTRING XY ($x0 $y0, $x1 $y1)"); 
    $geometryByteReader = $agfReaderWriter->Write($geometry); 
    $geometryProperty = new MgGeometryProperty("SHPGEOM", $geometryByteReader);
    $propertyCollection->Add($geometryProperty);
    
    return $propertyCollection;
  }
  
  ///////////////////////////////////////////////////////////////////////////////////
  function DoesResourceExist($resourceIdentifier, $resourceService)
  // Returns true if the resource already exists, or false otherwise
  {
    return $resourceService->ResourceExists($resourceIdentifier);
  }
  
  ///////////////////////////////////////////////////////////////////////////////////
  function DoesLayerExist($layerName, $map)
  // Returns true if the layer already exists, or false otherwise
  {
    $layerCollection = $map->GetLayers();   
    return( $layerCollection->Contains($layerName) ? true : false ) ;
  }
  
  ///////////////////////////////////////////////////////////////////////////////////
  ?>
  </body>
</html>
<?php
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

require_once('../common/common.php');

function add_layer_definition_to_map($layerDefinition, $layerName, $layerLegendLabel, $sessionId, $resourceService, &$map)
// Adds the layer definition (XML) to the map.
// Returns the layer.
{
  global $schemaDirectory;

    // Validate the XML.
    $domDocument = new DOMDocument;
    $domDocument->loadXML($layerDefinition);
  if (! $domDocument->schemaValidate($schemaDirectory . "LayerDefinition-1.3.0.xsd") ) // $schemaDirectory is defined in common.php
  {
    echo "ERROR: The new XML document is invalid.<BR>\n.";
    return NULL;
  }    

  // Save the new layer definition to the session repository  
  $byteSource = new MgByteSource($layerDefinition, strlen($layerDefinition));
  $byteSource->SetMimeType(MgMimeType::Xml);
  $resourceID = new MgResourceIdentifier("Session:$sessionId//$layerName.LayerDefinition");
  $resourceService->SetResource($resourceID, $byteSource->GetReader(), null);
  
  $newLayer = add_layer_resource_to_map($resourceID, $resourceService, $layerName, $layerLegendLabel, $map);
  
  return $newLayer;
}

//////////////////////////////////////////////////////////////
function add_layer_to_group($layer, $layerGroupName, $layerGroupLegendLabel, &$map)
// Adds a layer to a layer group. If necessary, it creates the layer group.
{
  
  // Get the layer group
  $layerGroupCollection = $map->GetLayerGroups();
  if ($layerGroupCollection->Contains($layerGroupName))
  {
    $layerGroup = $layerGroupCollection->GetItem($layerGroupName);
  }
  else
  {
    // It does not exist, so create it
    $layerGroup = new MgLayerGroup($layerGroupName); 
    $layerGroup->SetVisible(true);
    $layerGroup->SetDisplayInLegend(true);
    $layerGroup->SetLegendLabel($layerGroupLegendLabel);
    $layerGroupCollection->Add($layerGroup); 
  }
  
  // Add the layer to the group
  $layer->SetGroup($layerGroup);  
}

//////////////////////////////////////////////////////////////
function add_layer_resource_to_map($layerResourceID, $resourceService, $layerName, $layerLegendLabel, &$map)
// Adds a layer defition (which can be stored either in the Library or a session
// repository) to the map.
// Returns the layer.
{
  $newLayer = new MgLayer($layerResourceID, $resourceService);  
  
  // Add the new layer to the map's layer collection
  $newLayer->SetName($layerName);
  $newLayer->SetVisible(true);
  $newLayer->SetLegendLabel($layerLegendLabel);
  $newLayer->SetDisplayInLegend(true);
  $layerCollection = $map->GetLayers(); 
  if (! $layerCollection->Contains($layerName) )
  {
    // Insert the new layer at position 0 so it is at the top
    // of the drawing order
    $layerCollection->Insert(0, $newLayer); 
  }
  
  return $newLayer;
}

//////////////////////////////////////////////////////////////
?>

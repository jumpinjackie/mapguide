<?php

//
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
//

include "../common/common.php";

$args = ($_SERVER['REQUEST_METHOD'] == 'POST') ? $_POST : $_GET;
$sessionId = $args['SESSION'];
$xmlSelection = $args['SELECTION'];
$mapName = $args['MAPNAME'];
$mapScale = $args['SCALE'];
$imageHeight = $args['HEIGHT'];
$imageWidth = $args['WIDTH'];
$mapCenterX = $args['CENTERX'];
$mapCenterY = $args['CENTERY'];

try
{
  // Initialize the Web Extensions and connect to the Server using
  // the Web Extensions session identifier stored in PHP session state.

  MgInitializeWebTier ($webconfigFilePath);

  $userInfo = new MgUserInformation($sessionId);

  $siteConnection = new MgSiteConnection();
  $siteConnection->Open($userInfo);

  // Create the necessary services.
  
  $resourceService = $siteConnection->
    CreateService(MgServiceType::ResourceService);
  $renderingService = $siteConnection->
    CreateService(MgServiceType::RenderingService);

  // Open the map and get its SRS
  
  $map = new MgMap();
  $map->Open($resourceService, $mapName);
  $srsWkt = $map->GetMapSRS();
  $coordinateSystemFactory = new MgCoordinateSystemFactory();
  $srs = $coordinateSystemFactory->Create($srsWkt);

  if ($xmlSelection != '')
  {
    $selection = new MgSelection($map, $xmlSelection);
  }
  else
  {
    $selection = new MgSelection($map);
  }

  $color = new MgColor(205, 189, 156);

  $geometryFactory = new MgGeometryFactory();
  $mapCenterCoordinate = $geometryFactory->
    CreateCoordinateXY($mapCenterX, $mapCenterY);

  // Convert the height in pixels to map units.
  // Create an envelope that contains the image area to display.
  
  $displayInInches = $imageHeight / 96;
  $displayInMeters = $displayInInches * .0254;
  $mapHeightInMeters = $displayInMeters * $mapScale;
  $mapHeightInMapUnits = $srs->ConvertMetersToCoordinateSystemUnits($mapHeightInMeters);
  $envelopeOffsetY = $mapHeightInMapUnits / 2;
  $envelopeOffsetX = $imageWidth / $imageHeight * $envelopeOffsetY;
  $envelope = new MgEnvelope($mapCenterX - $envelopeOffsetX, 
    $mapCenterY - $envelopeOffsetY, $mapCenterX + $envelopeOffsetX, 
    $mapCenterY + $envelopeOffsetY);

  // Render the image and send it to the browser.
  
  $byteReader = $renderingService->RenderMap($map, $selection, 
    $envelope, $imageWidth, $imageHeight, $color, 'PNG');
  
  header("Content-type: " . $byteReader->GetMimeType() );

  $buffer = '';
  while ($byteReader->Read($buffer, 50000) != 0)
  {
    echo $buffer;
  }
}
catch (MgException $e)
{
  echo '<p>' . $e->GetMessage() . '</p>';
  echo '<p>' . $e->GetDetails() . '</p>';
}

?>

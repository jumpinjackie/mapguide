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

  // If any parcels have been selected, recenter the image on the first selected parcel.
  if ($xmlSelection != '')
  {
    $resourceService = $siteConnection->CreateService(MgServiceType::ResourceService);
    $featureService = $siteConnection->CreateService(MgServiceType::FeatureService);
    $queryOptions = new MgFeatureQueryOptions();

    $map = new MgMap();
    $map->Open($resourceService, $mapName);

    $selection = new MgSelection($map, $xmlSelection);
    $layers = $selection->GetLayers();

    if ($layers)
    {
      for ($i = 0; $i < $layers->GetCount(); $i++)
      {
        // Only check selected features in the Parcels layer.

        $layer = $layers->GetItem($i);

        if ($layer && $layer->GetName() == 'Parcels')
        {

          // Create a filter containing the IDs of the selected features on this layer

          $layerClassName = $layer->GetFeatureClassName();
          $selectionString = $selection->GenerateFilter($layer, $layerClassName);

          // Get the feature resource for the selected layer

          $layerFeatureId = $layer->GetFeatureSourceId();
          $layerFeatureResource = new MgResourceIdentifier($layerFeatureId);

          // Apply the filter to the feature resource for the selected layer. This returns
          // an MgFeatureReader of all the selected features.

          $queryOptions->SetFilter($selectionString);
          $featureReader = $featureService->SelectFeatures($layerFeatureResource, $layerClassName, $queryOptions);

          // Create the report details using the first selected parcel. Position the image so the
          // centroid of the selected parcel is in the center of the map.

          $featureReader->ReadNext();
          $featureAgfGeometry = $featureReader->GetGeometry('SHPGEOM');
          $agfReaderWriter = new MgAgfReaderWriter();
          $featureGeometry = $agfReaderWriter->Read($featureAgfGeometry);
          $centroid = $featureGeometry->GetCentroid();
          $mapCenterX = $centroid->GetCoordinate()->GetX();
          $mapCenterY = $centroid->GetCoordinate()->GetY();

          $owner = $featureReader->GetString('RNAME');
          $address = $featureReader->GetString('RPROPAD');
          $billingAddress = $featureReader->GetString('RBILAD');
          $description = $featureReader->GetString('RLDESCR1') . '<br/>' . $featureReader->GetString('RLDESCR2') . '<br/>' . $featureReader->GetString('RLDESCR3') . '<br/>';

        }
      }
    }
  }

}
catch (MgException $e)
{
  echo '<p>' . $e->GetExceptionMessage() . '</p>';
  echo '<p>' . $e->GetDetails() . '</p>';
}
$selectionEncoded = urlencode($xmlSelection);
$imageUrl = "createmapimage.php?SESSION=$sessionId&SELECTION=$selectionEncoded&MAPNAME=$mapName&SCALE=$mapScale" .
  "&HEIGHT=$imageHeight&WIDTH=$imageWidth&CENTERX=$mapCenterX&CENTERY=$mapCenterY";

?>
<html>
  <head>
    <title>Property Report</title>
    <meta content="text/html; charset=utf-8" http-equiv="Content-Type">
    <meta http-equiv="content-script-type" content="text/javascript">
    <meta http-equiv="content-style-type" content="text/css">
    <link href="../styles/globalStyles.css" rel="stylesheet" type="text/css">
    <style>
      img {float: left; margin-right: 10px; border: 1px solid #999; padding: 4px}
    </style>
  </head>
  <body>
    <h1>Property Report</h1>
  <p><img src="<?php echo $imageUrl ?>" width="<?=$imageWidth?>" height="<?= $imageHeight ?>" />
    <?php
    if ($xmlSelection == '')
    {
      echo 'No properties selected.';
    }
    else
    {
      echo "Owner: $owner<br/>\n";
      echo "Address: $address<br/>\n";
      echo "Billing Address: $billingAddress<br/>\n";
      echo "Description: $description<br/>\n";
    }
    ?>
  </p>
</body>
</html>
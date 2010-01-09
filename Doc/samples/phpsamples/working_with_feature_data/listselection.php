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
    <title>Viewer Sample Application - List Selected Parcels</title>
    <meta content="text/html; charset=utf-8" http-equiv="Content-Type">
    <meta http-equiv="content-script-type" content="text/javascript" />
    <meta http-equiv="content-style-type" content="text/css" />
    <link href="../styles/globalStyles.css" rel="stylesheet" type="text/css">
    <link href="../styles/otherStyles.css" rel="stylesheet" type="text/css">
  </head>

  <body class="AppFrame" >

    <h1 class="AppHeading">Selected Parcels</h1>

    <?php
    include '../common/common.php';

    $args = ($_SERVER['REQUEST_METHOD'] == 'POST') ? $_POST : $_GET;
    $sessionId = $args['SESSION'];
    $mapName = $args['MAPNAME'];

    try
    {
      // Initialize the Web Extensions and connect to the Server using
      // the Web Extensions session identifier stored in PHP session state.

      MgInitializeWebTier ($webconfigFilePath);

      $userInfo = new MgUserInformation($sessionId);
      $siteConnection = new MgSiteConnection();
      $siteConnection->Open($userInfo);

      $resourceService = $siteConnection->CreateService(MgServiceType::ResourceService);
      $featureService = $siteConnection->CreateService(MgServiceType::FeatureService);

      $map = new MgMap();
      $map->Open($resourceService, $mapName);

      // ----------------------------------------------------------
      // Use the following code for AJAX or DWF Viewers
      // This requires passing selection data via HTTP POST

      if (isset($_POST['SELECTION']) && $_POST['SELECTION'] != '')
      {
        $selection = new MgSelection($map, $_POST['SELECTION']);
        $layers = $selection->GetLayers();
      }
      else
      $layers = 0;
      // ---------------------------------------------------------
      
      // ---------------------------------------------------------
      // Use the following code for AJAX Viewers only.
      // This does not require passing selection data via HTTP POST.
      //
      // $selection = new MgSelection($map);
      // $selection->Open($resourceService, $mapName);
      // $layers = $selection->GetLayers();
      // ---------------------------------------------------------

      if ($layers)
      { 
        $queryOptions = new MgFeatureQueryOptions();
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

            // Process each item in the MgFeatureReader, displaying the owner name

            while ($featureReader->ReadNext())
            {
              $val = $featureReader->GetString('NAME') . '<br />&nbsp;&nbsp;' . $featureReader->GetString('RPROPAD');
              echo $val . '<br />';
            }
          }
        }
      }
      else
      echo 'No selected layers';
      echo '</p>';

    }
    catch (MgException $e)
    {
      echo '<p>' . $e->GetMessage() . '</p>';
      echo '<p>' . $e->GetDetails() . '</p>';
    }
    ?>

    <?php $fileName = urlencode(__FILE__); ?>
    <p><a target="_blank" href="../common/viewsource.php?FILENAME=<?= $fileName ?>" >View source</a></p>
  </body>

</html>

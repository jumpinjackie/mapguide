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
    <title>Viewer Sample Application - Create Buffer</title>
    <meta content="text/html; charset=utf-8" http-equiv="Content-Type">
    <meta http-equiv="content-script-type" content="text/javascript" />
    <meta http-equiv="content-style-type" content="text/css" />
    <link href="../styles/globalStyles.css" rel="stylesheet" type="text/css">
    <link href="../styles/otherStyles.css" rel="stylesheet" type="text/css">
    <script language="javascript">
      function OnPageLoad()
      {
        parent.parent.Refresh();
      }
    </script>
  </head>

  <body class="AppFrame" onLoad="OnPageLoad()">

    <h1 class="AppHeading">Create buffer</h1>

    <?php
    include '../common/common.php';

    $args = ($_SERVER['REQUEST_METHOD'] == "POST") ? $_POST : $_GET;
    $sessionId = $args['SESSION'];
    $mapName = $args['MAPNAME'];
    $mapName = ($_SERVER['REQUEST_METHOD'] == "POST")? $_POST['MAPNAME']: $_GET['MAPNAME'];

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
      $queryOptions = new MgFeatureQueryOptions();

      $map = new MgMap();
      $map->Open($resourceService, $mapName);

      // Check for selection data passed via HTTP POST

      if (isset($_POST['SELECTION']) && $_POST['SELECTION'] != '')
      {
        $selection = new MgSelection($map, $_POST['SELECTION']);
        $selectedLayers = $selection->GetLayers();
      }
      else
      $selectedLayers = 0;

      if ($selectedLayers)
      {

        include 'bufferfunctions.php';
        $bufferRingSize = 100; // measured in metres
        $bufferRingCount = 5;

        // Set up some objects for coordinate conversion

        $mapWktSrs = $map->GetMapSRS();
        $agfReaderWriter = new MgAgfReaderWriter();
        $wktReaderWriter = new MgWktReaderWriter();
        $coordinateSystemFactory = new MgCoordinateSystemFactory();
        $srs = $coordinateSystemFactory->Create($mapWktSrs);
        $srsMeasure = new MgCoordinateSystemMeasure($srs);

        // Check for a buffer layer. If it exists, delete
        // the current features.
        // If it does not exist, create a feature source and
        // a layer to hold the buffer.

        try
        {
          $bufferLayer = $map->GetLayers()->GetItem('Buffer');
          $bufferFeatureResId = new MgResourceIdentifier($bufferLayer->GetFeatureSourceId());

          $commands = new MgFeatureCommandCollection();
          $commands->Add(new MgDeleteFeatures('BufferClass', "ID like '%'"));

          $featureService->UpdateFeatures($bufferFeatureResId, $commands, false);
        }
        catch (MgObjectNotFoundException $e)
        {
          // When an MgObjectNotFoundException is thrown, the layer
          // does not exist and must be created.

          $bufferFeatureResId = new MgResourceIdentifier("Session:" . $sessionId . "//Buffer.FeatureSource");
          CreateBufferFeatureSource($featureService, $mapWktSrs, $bufferFeatureResId);
          $bufferLayer = CreateBufferLayer($resourceService, $bufferFeatureResId, $sessionId);
          $map->GetLayers()->Insert(0, $bufferLayer);
        }

        for ($i = 0; $i < $selectedLayers->GetCount(); $i++)
        {
          // Only check selected features in the Parcels layer.

          $layer = $selectedLayers->GetItem($i);

          if ($layer->GetName() == 'Parcels')
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

            // Process each item in the MgFeatureReader. Get the
            // geometries from all the selected features and
            // merge them into a single geometry.

            $inputGeometries = new MgGeometryCollection();
            while ($featureReader->ReadNext())
            {
              $featureGeometryData = $featureReader->GetGeometry('SHPGEOM');
              $featureGeometry = $agfReaderWriter->Read($featureGeometryData);

              $inputGeometries->Add($featureGeometry);
            }

            $geometryFactory = new MgGeometryFactory();
            $mergedGeometries = $geometryFactory->CreateMultiGeometry($inputGeometries);

            // Add buffer features to the temporary feature source.
            // Create multiple concentric buffers to show area.
            // If the stylization for the layer draws the features 
            // partially transparent, the concentric rings will be 
            // progressively darker towards the center.
            // The stylization is set in the layer template file, which
            // is used in function CreateBufferLayer().

            $commands = new MgFeatureCommandCollection();
            for ($bufferRing = 0; $bufferRing < $bufferRingCount; $bufferRing++)
            {
              $bufferDist = $srs->ConvertMetersToCoordinateSystemUnits($bufferRingSize * ($bufferRing + 1));
              $bufferGeometry = $mergedGeometries->Buffer($bufferDist, $srsMeasure);

              $properties = new MgPropertyCollection();
              $properties->Add(new MgGeometryProperty('BufferGeometry', $agfReaderWriter->Write($bufferGeometry)));

              $commands->Add(new MgInsertFeatures('BufferClass', $properties));
            }

            $results = $featureService->UpdateFeatures($bufferFeatureResId, $commands, false);

            $bufferLayer->SetVisible(true);
            $bufferLayer->ForceRefresh();
            $bufferLayer->SetDisplayInLegend(true);
            $map->Save($resourceService);

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
    
    <p>The buffer has been created.</p>

  </body>

</html>

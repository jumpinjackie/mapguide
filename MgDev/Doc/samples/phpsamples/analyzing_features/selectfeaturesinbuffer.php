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

<!--
This sample has been updated to reflect simplified usage patterns enabled by convenience APIs
introduced in MapGuide OS 2.0 by allowing you to query and insert features directly from 
the MgLayer objects themselves and the ability to get a MgFeatureReader directly from the MgSelection
object.
-->
<html>

  <head>
    <title>Viewer Sample Application - Select Within Buffer</title>
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

    <h1 class="AppHeading">Select within buffer area</h1>

    <?php
    include '../common/common.php';

    $args = ($_SERVER['REQUEST_METHOD'] == "POST") ? $_POST : $_GET;
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
      $queryOptions = new MgFeatureQueryOptions();

      $map = new MgMap($siteConnection);
      $map->Open($mapName);

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
        $bufferRingSize = 500; // measured in metres

        // Set up some objects for coordinate conversion

        $mapWktSrs = $map->GetMapSRS();
        $agfReaderWriter = new MgAgfReaderWriter();
        $wktReaderWriter = new MgWktReaderWriter();
        $coordinateSystemFactory = new MgCoordinateSystemFactory();
        $srs = $coordinateSystemFactory->Create($mapWktSrs);
        $srsMeasure = $srs->GetMeasure();

        // Check for a buffer layer. If it exists, delete
        // the current features.
        // If it does not exist, create a feature source and
        // a layer to hold the buffer.

        /*
        // Old way, pre MapGuide OS 2.0. Kept here for reference
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
        */
        
        // This is how things can be done now
        $layerIndex = $map->GetLayers()->IndexOf('Buffer');
        if ($layerIndex < 0)
        {
            // The layer does not exist and must be created.

            $bufferFeatureResId = new MgResourceIdentifier("Session:" . $sessionId . "//Buffer.FeatureSource");
            CreateBufferFeatureSource($featureService, $mapWktSrs, $bufferFeatureResId);
            $bufferLayer = CreateBufferLayer($resourceService, $bufferFeatureResId, $sessionId);
            $map->GetLayers()->Insert(0, $bufferLayer);
        }
        else
        {
            $bufferLayer = $map->GetLayers()->GetItem($layerIndex);
            $commands = new MgFeatureCommandCollection();
            $commands->Add(new MgDeleteFeatures('BufferClass', "ID like '%'"));
            
            $bufferLayer->UpdateFeatures($commands);
        }

        // Check for a parcel marker layer. If it exists, delete
        // the current features.
        // If it does not exist, create a feature source and
        // a layer to hold the parcel markers.

        /*
        // Old way, pre MapGuide OS 2.0. Kept here for reference
        try
        {
          $parcelMarkerLayer = $map->GetLayers()->GetItem('ParcelMarker');
          $parcelFeatureResId = new MgResourceIdentifier($parcelMarkerLayer->GetFeatureSourceId());

          $commands = new MgFeatureCommandCollection();
          $commands->Add(new MgDeleteFeatures('ParcelMarkerClass', "ID like '%'"));

          $featureService->UpdateFeatures($parcelFeatureResId, $commands, false);
        }
        catch (MgObjectNotFoundException $e)
        {
          // When an MgObjectNotFoundException is thrown, the layer
          // does not exist and must be created.

          $parcelFeatureResId = new MgResourceIdentifier("Session:" . $sessionId . "//ParcelMarker.FeatureSource");
          CreateParcelMarkerFeatureSource($featureService, $mapWktSrs, $parcelFeatureResId);
          $parcelMarkerLayer = CreateParcelMarkerLayer($resourceService, $parcelFeatureResId, $sessionId);
          $map->GetLayers()->Insert(0, $parcelMarkerLayer);
        }
        */
        
        // New way, post MapGuide 2.0
        $layerIndex = $map->GetLayers()->IndexOf('ParcelMarker');
        if ($layerIndex < 0)
        {
            $parcelFeatureResId = new MgResourceIdentifier("Session:" . $sessionId . "//ParcelMarker.FeatureSource");
            CreateParcelMarkerFeatureSource($featureService, $mapWktSrs, $parcelFeatureResId);
            $parcelMarkerLayer = CreateParcelMarkerLayer($resourceService, $parcelFeatureResId, $sessionId);
            $map->GetLayers()->Insert(0, $parcelMarkerLayer);
        }
        else
        {
            $parcelMarkerLayer = $map->GetLayers()->GetItem($layerIndex);
            $commands = new MgFeatureCommandCollection();
            $commands->Add(new MgDeleteFeatures('ParcelMarkerClass', "ID like '%'"));
            
            $parcelMarkerLayer->UpdateFeatures($commands);
        }

        // Check each layer in the selection.

        for ($i = 0; $i < $selectedLayers->GetCount(); $i++)
        {
          // Only check selected features in the Parcels layer.

          $layer = $selectedLayers->GetItem($i);

          if ($layer->GetName() == 'Parcels')
          {

            echo 'Marking all parcels inside the buffer that are of type "MFG"';

            /*
            // Old way, pre MapGuide OS 2.0. Kept here for reference
            
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
            */
            
            // New way, post MapGuide 2.0
            
            $featureReader = $selection->GetSelectedFeatures($layer, $layer->GetFeatureClassName(), false);
            
            
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

            // Create a buffer from the merged geometries

            $bufferDist = $srs->ConvertMetersToCoordinateSystemUnits($bufferRingSize);
            $bufferGeometry = $mergedGeometries->Buffer($bufferDist, $srsMeasure);

            // Create a filter to select parcels within the buffer. Combine
            // a basic filter and a spatial filter to select all parcels
            // within the buffer that are of type "MFG".

            $queryOptions = new MgFeatureQueryOptions();
            $queryOptions->SetFilter("RTYPE = 'MFG'");
            $queryOptions->SetSpatialFilter('SHPGEOM', $bufferGeometry, MgFeatureSpatialOperations::Inside);

            /*
            // Old way, pre MapGuide OS 2.0. Kept here for reference
            $featureResId = new MgResourceIdentifier("Library://Samples/Sheboygan/Data/Parcels.FeatureSource");
            $featureReader = $featureService->SelectFeatures($featureResId, "Parcels", $queryOptions);
            */
            
            // New way, post MapGuide OS 2.0
            $featureReader = $layer->SelectFeatures($queryOptions);
            
            // Get the features from the feature source,
            // determine the centroid of each selected feature, and
            // add a point to the ParcelMarker layer to mark the
            // centroid.
            // Collect all the points into an MgFeatureCommandCollection,
            // so they can all be added in one operation.
            
            $parcelMarkerCommands = new MgFeatureCommandCollection();
            while ($featureReader->ReadNext())
            {
                $byteReader = $featureReader->GetGeometry('SHPGEOM');
                $geometry = $agfReaderWriter->Read($byteReader);
                $point = $geometry->GetCentroid();

                // Create an insert command for this parcel.
                $properties = new MgPropertyCollection();
                $properties->Add(new MgGeometryProperty('ParcelLocation', $agfReaderWriter->Write($point)));
                $parcelMarkerCommands->Add(new MgInsertFeatures('ParcelMarkerClass', $properties));
            }
            $featureReader->Close();

            if ($parcelMarkerCommands->GetCount() > 0)
            {
                // Old way, pre MapGuide OS 2.0. Kept here for reference
                //$featureService->UpdateFeatures($parcelFeatureResId, $parcelMarkerCommands, false);
                
                // New way, post MapGuide OS 2.0
                $parcelMarkerLayer->UpdateFeatures($parcelMarkerCommands);
            }
            else
            {
                echo '</p><p>No parcels within the buffer area match.';
            }

            // Create a feature in the buffer feature source to show the area covered by the buffer.

            $properties = new MgPropertyCollection();
            $properties->Add(new MgGeometryProperty('BufferGeometry', $agfReaderWriter->Write($bufferGeometry)));

            $commands = new MgFeatureCommandCollection();
            $commands->Add(new MgInsertFeatures('BufferClass', $properties));

            // Old way, pre MapGuide OS 2.0
            //$featureService->UpdateFeatures($bufferFeatureResId, $commands, false);
            
            // New way, post MapGuide OS 2.0
            $bufferLayer->UpdateFeatures($commands);

            // Ensure that the buffer layer is visible and in the legend.

            $bufferLayer->SetVisible(true);
            $bufferLayer->ForceRefresh();
            $bufferLayer->SetDisplayInLegend(true);
            $parcelMarkerLayer->SetVisible(true);
            $parcelMarkerLayer->ForceRefresh();

            $map->Save();

          }
        }
      }
      else
      echo 'No selected layers';
      echo '</p>';

    }
    catch (MgException $e)
    {
      echo '<p>' . $e->GetExceptionMessage() . '</p>';
      echo '<p>' . $e->GetDetails() . '</p>';
    }
    ?>

    <p>Done.</p>

  </body>

</html>

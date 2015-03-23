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
    <title>Viewer Sample Application - Server-side Selection</title>
    <meta content="text/html; charset=utf-8" http-equiv="Content-Type">
    <meta http-equiv="content-script-type" content="text/javascript" />
    <meta http-equiv="content-style-type" content="text/css" />
    <link href="../styles/globalStyles.css" rel="stylesheet" type="text/css">
    <link href="../styles/otherStyles.css" rel="stylesheet" type="text/css">
  </head>

  <body class="AppFrame" onLoad="OnPageLoad()">

    <h1 class="AppHeading">Select features</h1>

    <?php
    include '../common/common.php';

    $args = ($_SERVER['REQUEST_METHOD'] == "POST")? $_POST : $_GET;
    $sessionId = $args['SESSION'];
    $mapName = $args['MAPNAME'];
    $selectionXml = '';
    $idName = "";
    $geomName = "";

    try
    {

      // Initialize the Web Extensions and connect to the Server using
      // the Web Extensions session identifier stored in PHP session state.

      MgInitializeWebTier ($webconfigFilePath);

      $userInfo = new MgUserInformation($sessionId);
      $siteConnection = new MgSiteConnection();
      $siteConnection->Open($userInfo);

      $map = new MgMap($siteConnection);
      $map->Open($mapName);

      // Get the geometry for the boundaries of District 6
      $layer = $map->GetLayers()->GetItem('Districts');
      
      $districtQuery = new MgFeatureQueryOptions();
      $districtQuery->SetFilter("ID = '6'"); //ID property is string

      $featureReader = $layer->SelectFeatures($districtQuery);
      $featureReader->ReadNext();
      $districtGeometryData = $featureReader->GetGeometry($layer->GetFeatureGeometryName());

      // Convert the AGF binary data to MgGeometry.

      $agfReaderWriter = new MgAgfReaderWriter();
      $districtGeometry = $agfReaderWriter->Read($districtGeometryData);

      // Create a filter to select the desired features. Combine
      // a basic filter and a spatial filter.

      $queryOptions = new MgFeatureQueryOptions();
      $queryOptions->SetFilter("RNAME LIKE 'SCHMITT%'");
      $queryOptions->SetSpatialFilter('SHPGEOM', $districtGeometry, MgFeatureSpatialOperations::Inside);

      // Get the features from the feature source,
      // turn it into a selection, then save the selection as XML.

      $layer = $map->GetLayers()->GetItem('Parcels');
      $featureReader = $layer->SelectFeatures($queryOptions);

      $layer = $map->GetLayers()->GetItem('Parcels');
      $selection = new MgSelection($map);
      $selection->AddFeatures($layer, $featureReader, 0);
      $selectionXml = $selection->ToXml();

      echo 'Selecting parcels owned by Schmitt in District&nbsp;6';
    }
    catch (MgException $e)
    {
      echo $e->GetExceptionMessage();
      echo $e->GetDetails();
    }
    ?>

  </body>

  <script language="javascript">

    <!-- Emit this function and assocate it with the onLoad event for the -->
    <!-- page so that it gets executed when this page loads in the        -->
    <!-- browser. The function calls the SetSelectionXML method on the    -->
    <!-- Viewer Frame, which updates the current selection on the viewer  -->
    <!-- and the server.                                                  -->

    function OnPageLoad()
    {
      selectionXml = '<?php echo $selectionXml; ?>';
      parent.parent.SetSelectionXML(selectionXml);
    }

  </script>

</html>
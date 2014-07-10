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
    <title>Viewer Sample Application - Query Parcels</title>
    <meta content="text/html; charset=utf-8" http-equiv="Content-Type">
    <meta http-equiv="content-script-type" content="text/javascript" />
    <meta http-equiv="content-style-type" content="text/css" />
    <link href="../styles/globalStyles.css" rel="stylesheet" type="text/css">
    <link href="../styles/otherStyles.css" rel="stylesheet" type="text/css">
  </head>

  <body class="AppFrame">

    <h1 class="AppHeading">Feature Query</h1>

    <?php
    require_once '../common/common.php';

    $args = ($_SERVER['REQUEST_METHOD'] == "POST")? $_POST : $_GET;
    $sessionId = $args['SESSION'];
    $mapName = $args['MAPNAME'];

    try
    {

      // Initialize the Web Extensions and connect to
      // the Server using the Web Extensions session
      // identifier stored in PHP session state.

      MgInitializeWebTier ($webconfigFilePath);

      $userInfo = new MgUserInformation($sessionId);
      $siteConnection = new MgSiteConnection();
      $siteConnection->Open($userInfo);

      $map = new MgMap($siteConnection);
      $map->Open($mapName);

      // Get the geometry for the boundaries of District 6

      $districtQuery = new MgFeatureQueryOptions();
      $districtQuery->SetFilter("ID = '6'"); //ID property is string

      $layer = $map->GetLayers()->GetItem('Districts');
      $classDef = $layer->GetClassDefinition();
      $clsIdProps = $classDef->GetIdentityProperties();
      $idProp = $clsIdProps->GetItem(0);      
      $districtQuery->SetFilter($idProp->GetName()." = 1");
      
      $featureReader = $layer->SelectFeatures($districtQuery);
      $featureReader->ReadNext();
      $districtGeometryData = $featureReader->GetGeometry($layer->GetFeatureGeometryName());

      // Convert the AGF binary data to MgGeometry.

      $agfReaderWriter = new MgAgfReaderWriter();
      $districtGeometry = $agfReaderWriter->
      Read($districtGeometryData);

      // Create a filter to select the desired features.
      // Combine a basic filter and a spatial filter.

      $queryOptions = new MgFeatureQueryOptions();
      $queryOptions->SetFilter("RNAME LIKE 'SCHMITT%'");

      $queryOptions->SetSpatialFilter('SHPGEOM',
      $districtGeometry,
      MgFeatureSpatialOperations::Inside);

      // Select the features.

      $layer = $map->GetLayers()->GetItem('Parcels');
      $featureReader = $layer->SelectFeatures($queryOptions);

      // For each selected feature, display the address.

      echo '<p>Properties owned by Schmitt ';
      echo 'in District 6</p><p>';

      while ($featureReader->ReadNext())
      {
        $val = $featureReader->GetString('RPROPAD');
        echo $val . '<br />';
      }
      $featureReader->Close();
      echo '</p>';
    }
    catch (MgException $e)
    {
      echo $e->getTraceAsString();
      echo $e->GetExceptionMessage();
      echo $e->GetDetails();
    }
    catch (Exception $e)
    {
      echo $e->getMessage();
    }
    ?>
  </body>
</html>
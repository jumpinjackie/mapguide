<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<!--
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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
    <title>Hello, map</title>
    <meta content="text/html; charset=utf-8" http-equiv="Content-Type">
    <meta http-equiv="content-script-type" content="text/javascript">
    <meta http-equiv="content-style-type" content="text/css">
    <link href="../styles/globalStyles.css" rel="stylesheet"  type="text/css">
    <link href="../styles/otherStyles.css" rel="stylesheet" type="text/css">
  </head>
  <body class="AppFrame">
    <h1 class="AppHeading">Spatial Reference</h1>
    <?php
    
    // Get the directory paths for this MapGuide installation
    
    require_once('../common/common.php');

    try
    {
      // --------------------------------------------------//
      // Basic initialization needs to be done every time.
      
      MgInitializeWebTier($webconfigFilePath);

      // Get the session information passed from the viewer.
      
      $args = ($_SERVER['REQUEST_METHOD'] == "POST")? $_POST : $_GET;
      $sessionId = $args['SESSION'];
      $mapName = $args['MAPNAME'];

      // Get the user information using the session id,
      // and set up a connection to the site server.
      
      $userInfo = new MgUserInformation($sessionId);
      $siteConnection = new MgSiteConnection();
      $siteConnection->Open($userInfo);

      // Get an instance of the required service(s).
      
      $resourceService = $siteConnection->
      CreateService(MgServiceType::ResourceService);

      // Finished basic initialization.
      // --------------------------------------------------//
      
      // Query the spatial reference system used for the map.
      
      $map = new MgMap();
      $map->Open($resourceService, $mapName);
      $srs = $map->GetMapSRS();
      
      // Format it and display it in the task pane.
      
      echo '<p>Map <strong>' . $map->GetName() .
      '</strong> uses this reference system: </p>';
      $srs = chunk_split($srs, 30);
      echo '<pre style="padding: 2px; border: 1px solid black; background-color: #eee">' . $srs . '</pre>';
    }
    catch (MgException $e)
    {
    echo "<p><strong>Error:</strong> ";
    echo $e->GetDetails();
    echo "</p>";
    }
  ?>

</body>
</html>
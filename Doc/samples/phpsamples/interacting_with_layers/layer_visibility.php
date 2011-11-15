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
    <title>Layer Visibility</title>
    <meta content="text/html; charset=utf-8" http-equiv="Content-Type">
    <meta http-equiv="content-script-type" content="text/javascript">
    <meta http-equiv="content-style-type" content="text/css">
    <link href="../styles/globalStyles.css" rel="stylesheet"  type="text/css">
    <link href="../styles/otherStyles.css" rel="stylesheet" type="text/css">
  </head>
  <body class="AppFrame">
    <h1 class="AppHeading">Layer Visibility</h1>
    <?php
    require_once('../common/common.php');

    try
    {
      MgInitializeWebTier($webconfigFilePath);

      $args = ($_SERVER['REQUEST_METHOD'] == "POST")? $_POST : $_GET;
      $sessionId = $args['SESSION'];
      $mapName = $args['MAPNAME'];
      
      $userInfo = new MgUserInformation($sessionId);
      $siteConnection = new MgSiteConnection();
      $siteConnection->Open($userInfo);
  
      $map = new MgMap($siteConnection);
      $map->Open($mapName);
      
      $layers = $map->GetLayers(); // Get layer collection
      echo "<p>Layers, in draw order:</p>";
      echo '<table class="taskPane" cellspacing="0">';
      echo '<tr><th class="rowHead">Layer</th><th>GetVisible()</th><th>IsVisible()</th></tr>';
      $count = $layers->GetCount(); 
      for ($i = 0; $i < $count; $i++)
      {
        $layer = $layers->GetItem($i);
        echo "<tr><td class=\"rowHead\">" . $layer->GetName() . "</td><td>" . ($layer->GetVisible() ? 'on' : 'off') . "</td><td>" . ($layer->IsVisible() ? 'on' : 'off') . "</td></tr>\n";
      }
      echo '</table>';
    }
    catch (MgException $e)
    {
      echo "<p><strong>Error:</strong> ";
      echo $e->GetDetails();
      echo "</p>";
    }
    ?>
    <p><code>GetVisible()</code> returns the state of the visibility flag for a given layer (the check box in the
      Layers pane). <code>IsVisible()</code> returns the actual visibility of a layer, based on its visibility flag, 
      the visibility flag of any groups that include the layer, and whether the layer has a defined style for the 
    current view scale.</p>
</body>
</html>
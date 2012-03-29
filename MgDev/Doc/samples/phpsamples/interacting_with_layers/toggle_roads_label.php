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
    <title>Rename layer</title>
    <meta content="text/html; charset=utf-8" http-equiv="Content-Type">
    <meta http-equiv="content-script-type" content="text/javascript">
    <meta http-equiv="content-style-type" content="text/css">
    <link href="../styles/globalStyles.css" rel="stylesheet"  type="text/css">
    <link href="../styles/otherStyles.css" rel="stylesheet" type="text/css">

    <script language="javascript" type="text/javascript">
      function OnPageLoad()
      {
        parent.parent.mapFrame.Refresh();
        parent.parent.mapFrame.ZoomToScale(12000);
      }
    </script>
  </head>
  <body class="AppFrame" onLoad="OnPageLoad();">
    <h1 class="AppHeading">Rename Roads Layer</h1>
    <?php
    require_once('../common/common.php');

    $sessionId = ($_SERVER['REQUEST_METHOD'] == "POST")?
    $_POST['SESSION']: $_GET['SESSION'];
    $mgMapName = ($_SERVER['REQUEST_METHOD'] == "POST")?
    $_POST['MAPNAME']: $_GET['MAPNAME'];

    try
    {
      // Initialize the Web Extensions and connect to the Server using
      // the Web Extensions session identifier stored in PHP
      // session state.

      MgInitializeWebTier ($webconfigFilePath);

      $userInfo = new MgUserInformation($sessionId);
      $siteConnection = new MgSiteConnection();
      $siteConnection->Open($userInfo);

      $map = new MgMap($siteConnection);
      $map->Open($mgMapName);

      $layers = $map->GetLayers();

      $roadLayer = $layers->GetItem('Roads');
      $roadLabel = $roadLayer->GetLegendLabel();
      if ($roadLabel == 'Roads')
      $newLabel = 'Streets';
      else
      $newLabel = 'Roads';

      $roadLayer->SetLegendLabel($newLabel);

      // You must save the updated map or the
      // changes will not be applied
      // Also be sure to refresh the map on page load.
      $map->Save();

      echo "<p>Layer label has been changed to $newLabel.</p>";

      echo '<p><a href="toggle_roads_label.php?SESSION=' . $sessionId .
      '&MAPNAME=' . $mgMapName . '">Run again</a></p>';

    }
    catch (MgLayerNotFoundException $e)
    {
      echo '<p>Layer not found</p>';
    }
    catch (MgObjectNotFoundException $e)
    {
      echo '<p>Layer not found</p>';
    }
    catch (MgException $e)
    {
      echo $e->GetExceptionMessage();
      echo $e->GetDetails();
    }
  ?>

</body>
</html>
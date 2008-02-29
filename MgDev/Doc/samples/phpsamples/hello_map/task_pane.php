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
    <meta content="text/html; charset=utf-8" http-equiv="Content-Type">
    <title>Task Pane</title>
    <meta http-equiv="content-style-type" content="text/css">
    <link href="../styles/globalStyles.css" rel="stylesheet" type="text/css">
    <link href="../styles/otherStyles.css" rel="stylesheet" type="text/css">
    <meta http-equiv="content-script-type" content="text/javascript">
  </head>
  <body class="AppFrame">
    <h1 class="AppHeading">Hello Map</h1>
    <?php
      $args =  ($_SERVER['REQUEST_METHOD'] == "POST") ? $_POST : $_GET;
      $sessionId = $args['SESSION'];
      $mapName = $args['MAPNAME'];
    ?>
    <p>This sample shows the most basic interaction with a map. It connects to a session
    	and queries information about the map.
    </p>
    <ul>
      <li>
        <a href="display_spatial_reference.php?SESSION=<?= $sessionId ?>&MAPNAME=<?= $mapName ?>">Display Spatial Reference</a>
        <br/>Connect to MapGuide session and display basic information.
        <br/>
        <?php
        $fileName = 'display_spatial_reference.php';
        $dir = getcwd();
        $fullPath = urlencode($dir . '/' . $fileName);
        ?>
        <a target="_blank" href="../common/viewsource.php?FILENAME=<?= $fullPath ?>" >View source</a>
      </li>
    </ul>

    <p>View source for support files:</p>
    <ul>
      <li><a target="_blank" href="../common/viewsource.php?FILENAME=<?= getcwd() ?>/../main.php">main.php</a> (wrapper for Viewer frames)</li>
      <li><a target="_blank" href="../common/viewsource.php?FILENAME=<?= getcwd() ?>/task_pane.php">task_pane.php</a> (this task pane page)</li>
      <li><a target="_blank" href="../common/viewsource.php?FILENAME=<?= getcwd() ?>/../common/common.php">common.php</a> (directory paths)</li>
    </ul>
  </body>
</html>

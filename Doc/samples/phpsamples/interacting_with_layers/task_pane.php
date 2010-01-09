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
		<meta content="text/html; charset=utf-8" http-equiv="Content-Type">
		<title>Task Pane</title>
		<meta http-equiv="content-style-type" content="text/css">
		<link href="../styles/globalStyles.css" rel="stylesheet" type="text/css">
		<link href="../styles/otherStyles.css" rel="stylesheet" type="text/css">
		<meta http-equiv="content-script-type" content="text/javascript">
	</head>
  <body class="AppFrame">
    <h1 class="AppHeading">Map Layers</h1>
    <?php
      $args = ($_SERVER['REQUEST_METHOD'] == "POST")? $_POST : $_GET;
      $sessionId = $args['SESSION'];
      $mapName = $args['MAPNAME'];
    ?>
    <ul>
      <li>
      	<a href="layer_visibility.php?SESSION=<?= $sessionId ?>&MAPNAME=<?= $mapName ?>">Layer Visibility</a>
		    <br/>List layers and visibility status
		    <br/>
		    <?php
		    $fileName = 'layer_visibility.php';
		    $dir = getcwd();
		    $fullPath = urlencode($dir . '/' . $fileName);
		    ?>
		    <a target="_blank" href="../common/viewsource.php?FILENAME=<?= $fullPath ?>" >View source</a>
      </li>
      <li>
      	<a href="toggle_roads_label.php?SESSION=<?= $sessionId ?>&MAPNAME=<?= $mapName ?>">Rename Roads Layer</a>
		    <br/>Toggle the name of the Roads layer
		    <br/>
		    <?php
		    $fileName = 'toggle_roads_label.php';
		    $dir = getcwd();
		    $fullPath = urlencode($dir . '/' . $fileName);
		    ?>
		    <a target="_blank" href="../common/viewsource.php?FILENAME=<?= $fullPath ?>" >View source</a>
      </li>
    </ul>

  </body>
</html>

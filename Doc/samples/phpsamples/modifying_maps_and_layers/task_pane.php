<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<!--
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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
    <meta http-equiv="content-script-type" content="text/javascript">
    <meta http-equiv="content-style-type" content="text/css">
    <link href="../styles/globalStyles.css" rel="stylesheet" type="text/css">
    <link href="../styles/otherStyles.css" rel="stylesheet" type="text/css">
  </head>
  <body class="AppFrame">
    <h1 class="AppHeading">Modifying Maps &amp; Layers</h1>
      <p>The following links all load in the hidden script frame, leaving this task pane visible.</p>
      <?php
        $args = ($_SERVER['REQUEST_METHOD'] == "POST") ? $_POST : $_GET;
        $sessionId = $args['SESSION'];
        $mapName = $args['MAPNAME'];
      ?>
      <ul>
        <li><a href="change_layers_via_dom.php?SESSION=<?= $sessionId ?>&MAPNAME=<?= $mapName ?>" target="scriptFrame">
          Change Layers via DOM</a>         
          <br>Show buildings built after 1980.
          <br>
          <?php 
            $fileName = 'change_layers_via_dom.php'; 
            $dir = getcwd();
            $fullPath = urlencode($dir . '\\' . $fileName);
          ?>
            <a target="_blank" href="../common/viewsource.php?FILENAME=<?= $fullPath ?>" >View source</a>
        </li>
        
        <li><a href="create_new_squarefootage_layer_definition.php?SESSION=<?= $sessionId ?>&MAPNAME=<?= $mapName ?>" target="scriptFrame">
          Create Square Footage Layer</a>
          <br>Show a map themed by areas.
          <br>
          <?php 
            $fileName = 'create_new_squarefootage_layer_definition.php'; 
            $dir = getcwd();
            $fullPath = urlencode($dir . '\\' . $fileName);
          ?>
            <a target="_blank" href="../common/viewsource.php?FILENAME=<?= $fullPath ?>" >View source</a>     
        </li>
        
        <li><a href="create_new_line_layer_definition.php?SESSION=<?= $sessionId ?>&MAPNAME=<?= $mapName ?>" target="scriptFrame">
          Create Hydro Line Layer</a>
          <br>Show a map themed by lines.
          <br>
          <?php 
            $fileName = 'create_new_line_layer_definition.php'; 
            $dir = getcwd();
            $fullPath = urlencode($dir . '\\' . $fileName);
          ?>
            <a target="_blank" href="../common/viewsource.php?FILENAME=<?= $fullPath ?>" >View source</a>   
        </li>
        
        <li><a href="create_new_point_layer_definition.php?SESSION=<?= $sessionId ?>&MAPNAME=<?= $mapName ?>" target="scriptFrame">
          Create Points of Interest Layer</a>
          <br>Show a map themed by points.
          <br>
          <?php 
            $fileName = 'create_new_point_layer_definition.php'; 
            $dir = getcwd();
            $fullPath = urlencode($dir . '\\' . $fileName);
          ?>
            <a target="_blank" href="../common/viewsource.php?FILENAME=<?= $fullPath ?>" >View source</a>         
        </li> 
      </ul>     
    <p>View source for support files:</p>
    <ul>
      <li><a target="_blank" href="../common/viewsource.php?FILENAME=<?= getcwd() ?>/RecentlyBuilt.LayerDefinition">RecentlyBuilt.LayerDefinition</a></li>
      <li><a target="_blank" href="../common/viewsource.php?FILENAME=<?= getcwd() ?>/layer_functions.php">layer_functions.php</a></li>
    </ul>
  </body>
</html>

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
    <meta http-equiv="content-style-type" content="text/css">
    <link href="../styles/globalStyles.css" rel="stylesheet" type="text/css">
    <link href="../styles/otherStyles.css" rel="stylesheet" type="text/css">
    <script language="javascript">
    function ZoomToDistrict()
    {
        parent.parent.ZoomToView(-87.726134, 43.770338, 8000, true);
    }
    function listSelection()
    {
      xmlSel = parent.parent.mapFrame.GetSelectionXML();
      params = new Array("SESSION", parent.parent.mapFrame.GetSessionId(), 
        "MAPNAME", parent.parent.mapFrame.GetMapName(),
        "SELECTION", xmlSel);
      pageUrl = "/mapguide/phpsamples/working_with_feature_data/listselection.php";
      parent.parent.formFrame.Submit(pageUrl, params, "taskPaneFrame");
    }
    </script>
    <meta http-equiv="content-script-type" content="text/javascript">
  </head>
  <body class="AppFrame">
    <h1 class="AppHeading">Working With Feature Data</h1>
    <p>
      <?php
      $args = ($_SERVER['REQUEST_METHOD'] == "POST")? $_POST : $_GET;
      $sessionId = $args['SESSION'];
      $mapName = $args['MAPNAME'];
      ?>
      <ul>
        <li>
          <a href="#"  onclick="ZoomToDistrict();" >Zoom to District 1</a>
          <br />Position the map to display parcels in District 1
        </li>
        <li>
          <a href="queryfeatures.php?SESSION=<?= $sessionId ?>&MAPNAME=<?= $mapName ?>" >Query features</a>
          <br />List properties in District 1 owned by Schmitt
          <br/>
          <?php
          $fileName = 'queryfeatures.php';
          $dir = getcwd();
          $fullPath = urlencode($dir . '/' . $fileName);
          ?>
          <a target="_blank" href="../common/viewsource.php?FILENAME=<?= $fullPath ?>" >View source</a>
        </li>
        <li>
          <a href="selectfeatures.php?SESSION=<?= $sessionId ?>&MAPNAME=<?= $mapName ?>" >Set active selection</a>
          <br />Select properties in District 1 owned by Schmitt
          <br/>
          <?php
          $fileName = 'selectfeatures.php';
          $dir = getcwd();
          $fullPath = urlencode($dir . '/' . $fileName);
          ?>
          <a target="_blank" href="../common/viewsource.php?FILENAME=<?= $fullPath ?>" >View source</a>
        </li>
        <li>
          <a href="#" onclick="listSelection();" >List selected features</a>
          <br />Lists currently selected features
          <br/>
          <?php
          $fileName = 'listselection.php';
          $dir = getcwd();
          $fullPath = urlencode($dir . '/' . $fileName);
          ?>
          <a target="_blank" href="../common/viewsource.php?FILENAME=<?= $fullPath ?>" >View source</a>
        </li>
      </ul>
    </p>
    <p>View the source for this task pane, showing JavaScript for repositioning map and initiating selection list.</p>
      <ul>
        <li>
          <?php
          $fileName = 'task_pane.php';
          $dir = getcwd();
          $fullPath = urlencode($dir . '/' . $fileName);
          ?>
          <a target="_blank" href="../common/viewsource.php?FILENAME=<?= $fullPath ?>" >task_pane.php</a>
        </li>
      </ul>
    </p>
  </body>
</html>

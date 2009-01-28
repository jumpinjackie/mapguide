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
    <script language=javascript>
      function submitBufferRequest(pageUrl)
      {
        xmlSel = parent.parent.mapFrame.GetSelectionXML();
        if (xmlSel.length > 0)
        {
          params = new Array("SESSION", parent.parent.mapFrame.GetSessionId(),
          "MAPNAME", parent.parent.mapFrame.GetMapName(),
          "SELECTION", xmlSel);
          parent.parent.formFrame.Submit(pageUrl, params, "taskPaneFrame");
        }
        else {
          alert ("Please select a parcel");
        }
      }
      
     function OnPageLoad()
     {
         parent.parent.mapFrame.Refresh();
         parent.parent.mapFrame.ZoomToScale(9999);
     }
    </script>
  </head>
  <body onLoad="OnPageLoad()" class="AppFrame">
    <h1 class="AppHeading">Analyzing Features</h1>
    <?php
      $args =  ($_SERVER['REQUEST_METHOD'] == "POST") ? $_POST: $_GET;
      $sessionId = $args['SESSION'];
      $mapName = $args['MAPNAME'];
    ?>
    <ul>
      <li>
        <a href="#" onClick="submitBufferRequest('/mapguide/phpsamples/analyzing_features/createbuffer.php'); return false;">
        Create buffer</a>
        <br/>Create a buffer around a selected parcel.
        <br/>
        <?php
        $fileName = 'createbuffer.php';
        $dir = getcwd();
        $fullPath = urlencode($dir . '/' . $fileName);
        ?>
        <a target="_blank" href="../common/viewsource.php?FILENAME=<?= $fullPath ?>" >View source</a>
      </li>

      <li>
        <a href="#" onClick="submitBufferRequest('/mapguide/phpsamples/analyzing_features/selectfeaturesinbuffer.php'); return false;">
        Find features in buffer</a>
        <br/>Create a buffer around a selected parcel, then mark parcels inside the buffer that are
        of type "MFG".
        <br/>
        <?php
        $fileName = 'selectfeaturesinbuffer.php';
        $dir = getcwd();
        $fullPath = urlencode($dir . '\\' . $fileName);
        ?>
        <a target="_blank" href="../common/viewsource.php?FILENAME=<?= $fullPath ?>" >View source</a>
      </li>

    </ul>

    <p>View source for support files:</p>
    <ul>
      <li><a target="_blank" href="../common/viewsource.php?FILENAME=<?= getcwd() ?>/task_pane.php">task_pane.php</a></li>
      <li><a target="_blank" href="../common/viewsource.php?FILENAME=<?= getcwd() ?>/bufferfunctions.php">bufferfunctions.php</a></li>
      <li><a target="_blank" href="../common/viewsource.php?FILENAME=<?= getcwd() ?>/bufferlayerdefinition.xml">bufferlayerdefinition.xml</a></li>
      <li><a target="_blank" href="../common/viewsource.php?FILENAME=<?= getcwd() ?>/parcelmarker.xml">parcelmarker.xml</a></li>
    </ul>
  </body>
</html>

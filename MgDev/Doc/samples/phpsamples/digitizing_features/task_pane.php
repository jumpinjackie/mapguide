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
		<meta content="text/html; charset=utf-8" http-equiv="Content-Type">
		<meta http-equiv="content-script-type" content="text/javascript">
		<meta http-equiv="content-style-type" content="text/css">
		<link href="../styles/globalStyles.css" rel="stylesheet" type="text/css">
		<link href="../styles/otherStyles.css" rel="stylesheet" type="text/css">
    <script src="../common/pageLoadFunctions.js" type="text/javascript" language="JavaScript">
    </script>
	</head>
	<body class="AppFrame">		
		<?php
			$args = ($_SERVER['REQUEST_METHOD'] == "POST")? $_POST: $_GET;
			$sessionId = $args['SESSION'];
      if (isset($args['MAPNAME']))
        $mapName = $args['MAPNAME'];
      else
        $mapName = 'unknown';
		?>				
		<h1 class="AppHeading">Digitizing &amp; Redlining</h1>	
		<ul>
			<li>
				<a href="task_pane_digitizing.php?SESSION=<?= $sessionId ?>&MAPNAME=<?= $mapName ?>">Digitizing Sample</a>
        <br/>Show digitizing functions.
        <br/>
        <?php
        $fileName = 'task_pane_digitizing.php';
        $dir = getcwd();
        $fullPath = urlencode($dir . '/' . $fileName);
        ?>
        <a target="_blank" href="../common/viewsource.php?FILENAME=<?= $fullPath ?>" >View source</a>
			</li>
			<li>
				<a href="task_pane_redlining.php?SESSION=<?= $sessionId ?>&MAPNAME=<?= $mapName ?>">Redlining Sample</a>
        <br/>Draw lines on the map.
        <br/>
        <?php
        $fileName = 'task_pane_redlining.php';
        $dir = getcwd();
        $fullPath = urlencode($dir . '/' . $fileName);
        ?>
        <a target="_blank" href="../common/viewsource.php?FILENAME=<?= $fullPath ?>" >View source</a>
			</li>
		</ul>
    <p>View source for support files:</p>
    <ul>
      <li><a target="_blank" href="../common/viewsource.php?FILENAME=<?= getcwd() ?>/draw_line.php">draw_line.php</a></li>
      <li><a target="_blank" href="../common/viewsource.php?FILENAME=<?= getcwd() ?>/clear_lines.php">clear_lines.php</a></li>
    </ul>
	</body>
</html>

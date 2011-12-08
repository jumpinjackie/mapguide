<?php

//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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
//

?>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Frameset//EN" "http://www.w3.org/TR/html4/frameset.dtd">

<?php
    include 'utilityfunctions.php';

    // Initialize a PHP session and register a variable to hold the
    // Server session id, then initialize the Web Extensions,
    // and connect to the Server, and create a session.

    MgInitializeWebTier($configFilePath);

	$site = new MgSite();
	$site->Open(new MgUserInformation('Anonymous', ''));

	$sessionId = $site->CreateSession();
	$webLayout = "Library://Samples/Sheboygan/Layouts/SheboyganPhpTiled.WebLayout";
?>

<html>

<head>
    <title>Viewer Sample Application</title>
    <meta content="text/html; charset=utf-8" http-equiv="Content-Type">
</head>

<frameset rows="50,*" frameborder="NO" border="0" framespacing="0">
    <frame src="title.html?AppName=AJAX" name="TitleFrame" scrolling="NO" noresize>
    <frame src="../mapviewerphp/ajaxviewer.php?SESSION=<?php echo $sessionId; ?>&WEBLAYOUT=<?php echo $webLayout; ?>" name="ViewerFrame">
</frameset>

</html>
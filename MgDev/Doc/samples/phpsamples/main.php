<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Frameset//EN" "http://www.w3.org/TR/html4/frameset.dtd">
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
<?php
require_once('common/common.php');

try
{
    // Initialize the web extensions,
    MgInitializeWebTier ($webconfigFilePath);

    // Connect to the site server and create a session
    $userInfo = new MgUserInformation("Author", "author");
    $site = new MgSite();
    $site->Open($userInfo);
}
catch (MgException $e)
{
    echo "Could not connect to the MapGuide site server. Please contact the MapGuide site administrator.";
    die();
}

try
{
    $sessionId = $site->CreateSession();

    // Define some constants
    $webLayout     = "Library://Samples/Layouts/PHPSamples.WebLayout";
    $title         = "MapGuide Developer's Guide PHP Samples";
}
catch (MgException $e)
{
    echo "ERROR: " . $e->GetExceptionMessage("eng") . "\n";
    echo $e->GetStackTrace("eng") . "\n";
}
?>

<html>
  <head>
    <title><?= $title ?>
    </title>
    <meta content="text/html; charset=utf-8" http-equiv="Content-Type" />
    <meta http-equiv="content-script-type" content="text/javascript" />
    <meta http-equiv="content-style-type" content="text/css">
    <link href="styles/globalStyles.css" rel="stylesheet" type="text/css" />
    <link href="styles/globalStyles.css" rel="stylesheet" type="text/css" />
  </head>

  <frameset rows="110,*" frameborder="NO" border="0" framespacing="0">
    <frame src="common/Title.php?TitleText=<?= $title ?>" name="TitleFrame" scrolling="NO" noresize />
    <frame src="../mapviewerajax/?SESSION=<?= $sessionId ?>&WEBLAYOUT=<?= $webLayout ?>" name="ViewerFrame" />
  </frameset>
</html>

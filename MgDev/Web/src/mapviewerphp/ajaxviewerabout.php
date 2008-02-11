<?php

//
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
//

include 'common.php';
include 'product.php';
include 'constants.php';

define('VIEWER_VERSION', "7.0.139.3");

$titleBar = $product . " Viewer";
$serverTitle = $product . " Server";
$viewerTitle = $product . " Viewer";
$viewerVersion = VIEWER_VERSION;
$serverVersion = "##Unknown";
$sessionId = "";
$locale = "";

SetLocalizedFilesPath(GetLocalizationPath());

GetRequestParameters();

try
{
    // Initialize web tier with the site configuration file.
    InitializeWebTier();

    // Set up user info
    $userInfo = new MgUserInformation();
    $userInfo->SetMgSessionId($sessionId);

    // Create a Site object and open the Site Server.
    $site = new MgSite();
    $site->Open($userInfo);

    $serverAdmin = new MgServerAdmin();
    $serverAdmin->Open($userInfo);
    $infoProps = $serverAdmin->GetInformationProperties();
    $versionProp = $infoProps->GetItem(MgServerInformationProperties::ServerVersion);
    $serverVersion = $versionProp->GetValue();
}
catch(MgException $e)
{
}
catch (Exception $e)
{
}

$templ = Localize(file_get_contents("../viewerfiles/ajaxviewerabout.templ"), $locale, GetClientOS());
printf($templ, $titleBar, $titleBar, $serverTitle, $serverVersion, $viewerTitle, $viewerVersion);

function GetParameters($params)
{
    global $sessionId, $locale;

    $sessionId = $params['SESSION'];
    $locale = $params['LOCALE'];
}

function GetRequestParameters()
{
    if($_SERVER['REQUEST_METHOD'] == "POST")
        GetParameters($_POST);
    else
        GetParameters($_GET);
}

?>

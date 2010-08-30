<?php

//
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
//

    include 'common.php';
    include 'constants.php';

    $locale = "";
    $targetType = 0;
    $mapName = "";
    $sessionId = "";
    $us = "";

    GetRequestParameters();

    $templ  = file_get_contents("../viewerfiles/quickplotpanel.templ");
    SetLocalizedFilesPath(GetLocalizationPath());
    $templ  = Localize($templ, $locale, GetClientOS());
    $vpath  = GetSurroundVirtualPath();
    $jsPath = "../viewerfiles/";
    print sprintf($templ, $jsPath, $jsPath, $targetType, "quickplotgeneratepicture.php", "quickplotpreviewinner.php");

function GetParameters($params)
{
    global $target, $cmdIndex, $clientWidth, $mapName, $sessionId, $targetType, $us, $locale;

    $locale = $params['LOCALE'];
    $mapName = $params['MAPNAME'];
    $sessionId = $params['SESSION'];
    $targetType = $params['TARGETTYPE'];
    $us = $params['US'];
}

function GetRequestParameters()
{
    if($_SERVER['REQUEST_METHOD'] == "POST")
        GetParameters($_POST);
    else
        GetParameters($_GET);
}

?>

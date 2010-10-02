<?php

//
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
//

    include 'common.php';

    $target = 0;
    $locale = "";
    $popup = 0;
    $cmdIndex = 0;
    $clientWidth = 0;
    $mapName = "";
    $sessionId = "";
    $total = 0;

    GetRequestParameters();

    $templ = file_get_contents("../viewerfiles/measureui.templ");

    SetLocalizedFilesPath(GetLocalizationPath());
    $templ = Localize($templ, $locale, GetClientOS());

    $vpath = GetSurroundVirtualPath();
    print sprintf($templ, $locale, $target, $popup, $mapName, $sessionId, $total, 0, 0, $vpath . "measure.php", $vpath . "measure.php");

function GetParameters($params)
{
    global $target, $cmdIndex, $clientWidth, $mapName, $sessionId, $total, $popup, $locale;

    $sessionId = ValidateSessionId(GetParameter($params, 'SESSION'));
    $locale = ValidateLocaleString(GetParameter($params, 'LOCALE'));
    $mapName = ValidateMapName(GetParameter($params, 'MAPNAME'));
    $target = GetIntParameter($params, 'TGT');
    $popup = GetIntParameter($params, 'POPUP');
    $cmdIndex = GetIntParameter($params, 'CMDINDEX');
    $clientWidth = GetIntParameter($params, 'WIDTH');
    $total = GetDoubleParameter($params, 'TOTAL');
}

function GetRequestParameters()
{
    if($_SERVER['REQUEST_METHOD'] == "POST")
        GetParameters($_POST);
    else
        GetParameters($_GET);
}

?>

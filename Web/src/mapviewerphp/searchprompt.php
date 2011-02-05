<?php

//
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
//

    include 'common.php';
    include 'constants.php';

    $locale = "";
    $cmdIndex = 0;
    $target = 0;
    $popup = 0;
    $clientWidth = 0;
    $layerId = "";
    $mapName = "";
    $sessionId = "";
    $filter = "";
    $matchLimit = 0;

    GetRequestParameters();

    $templ = file_get_contents("../viewerfiles/searchprompt.templ");
    SetLocalizedFilesPath(GetLocalizationPath());
    $templ = Localize($templ, $locale, GetClientOS());

    print sprintf($templ,
                  $target,
                  $popup,
                  $cmdIndex,
                  $clientWidth,
                  GetSurroundVirtualPath() . "search.php",
                  $cmdIndex,
                  $target,
                  $popup,
                  EscapeForHtml($layerId),
                  $mapName,
                  $sessionId,
                  EscapeForHtml($filter),
                  $matchLimit);

function GetParameters($params)
{
    global $cmdIndex, $target, $clientWidth, $layerId, $popup, $locale;
    global $mapName, $sessionId, $filter, $matchLimit;

    $sessionId = ValidateSessionId(GetParameter($params, 'SESSION'));
    $locale = ValidateLocaleString(GetParameter($params, 'LOCALE'));
    $mapName = ValidateMapName(GetParameter($params, 'MAPNAME'));
    $cmdIndex = GetIntParameter($params, 'CMDINDEX');
    $target = GetIntParameter($params, 'TGT');
    $popup = GetIntParameter($params, 'POPUP');
    $clientWidth = GetIntParameter($params, 'WIDTH');
    $matchLimit = GetIntParameter($params, 'MR');
    $layerId = GetParameter($params, 'LAYER');
    $filter = GetParameter($params, 'FILTER');
}

function GetRequestParameters()
{
    if($_SERVER['REQUEST_METHOD'] == "POST")
        GetParameters($_POST);
    else
        GetParameters($_GET);
}

?>

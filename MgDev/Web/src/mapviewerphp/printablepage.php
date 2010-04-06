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
    $mapName = "";
    $sessionId = "";
    $isTitle = 0;
    $isLegend = 0;
    $isArrow = 0;
    $title = "";
    $scale = 0;
    $centerX = 0;
    $centerY = 0;
    $dpi = 0;

    GetRequestParameters();

    $templ = file_get_contents("../viewerfiles/printablepage.templ");
    SetLocalizedFilesPath(GetLocalizationPath());
    $templ = Localize($templ, $locale, GetClientOS());
    $agent = GetRootVirtualFolder() . "/mapagent/mapagent.fcgi";
    print sprintf($templ,
                  $mapName,
                  $agent,
                  $scale,
                  $centerX,
                  $centerY,
                  $dpi,
                  $mapName,
                  $sessionId,
                  $isTitle,
                  $isLegend,
                  $isArrow,
                  $isTitle == 1 ? EscapeForHtml($title) : "",
                  $agent,
                  $mapName,
                  $sessionId
                  );

function GetParameters($params)
{
    global $mapName, $sessionId, $title, $locale;
    global $scale, $centerX, $centerY, $dpi;
    global $isTitle, $isLegend, $isArrow;

    $sessionId = ValidateSessionId(GetParameter($params, 'SESSION'));
    $locale = ValidateLocaleString(GetParameter($params, 'LOCALE'));
    $mapName = ValidateMapName(GetParameter($params, 'MAPNAME'));
    $isTitle = GetIntParameter($params, 'ISTITLE');
    $isLegend = GetIntParameter($params, 'ISLEGEND');
    $isArrow = GetIntParameter($params, 'ISARROW');
    $dpi = GetIntParameter($params, 'DPI');
    $scale = GetDoubleParameter($params, 'SCALE');
    $centerX = GetDoubleParameter($params, 'CENTERX');
    $centerY = GetDoubleParameter($params, 'CENTERY');
    $title = GetParameter($params, 'TITLE');
}

function GetRequestParameters()
{
    if($_SERVER['REQUEST_METHOD'] == "POST")
        GetParameters($_POST);
    else
        GetParameters($_GET);
}

?>

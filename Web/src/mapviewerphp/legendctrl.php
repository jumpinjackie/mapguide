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

    $mapName = "";
    $sessionId = "";
    $mapFrame = "";
    $locale = "";

    GetRequestParameters();

    $templ = file_get_contents("../viewerfiles/legendctrl.templ");
    $vpath = GetSurroundVirtualPath();
    printf($templ,
            $mapFrame,
            $vpath . "legend.php",
            $vpath . "legend.php",
            urlencode($mapName),
            $sessionId,
            $vpath . "legendui.php",
            $locale );

function GetParameters($params)
{
    global $mapName, $sessionId, $mapFrame, $locale;

    $sessionId = ValidateSessionId(GetParameter($params, 'SESSION'));
    $locale = ValidateLocaleString(GetParameter($params, 'LOCALE'));
    $mapName = ValidateMapName(GetParameter($params, 'MAPNAME'));
    if(isset($params['MAPFRAME']))
        $mapFrame = ValidateFrameName(GetParameter($params, 'MAPFRAME'));
    else
        $mapFrame = "parent";
}

function GetRequestParameters()
{
    if($_SERVER['REQUEST_METHOD'] == "POST")
        GetParameters($_POST);
    else
        GetParameters($_GET);
}

?>

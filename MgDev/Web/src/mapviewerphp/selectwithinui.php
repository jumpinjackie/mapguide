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
    include 'constants.php';

    $locale = "";
    $popup = 0;
    $mapName = "";
    $sessionId = "";
    $dwf = "";

    GetRequestParameters();

    $templ = file_get_contents("../viewerfiles/selectwithinui.templ");
    SetLocalizedFilesPath(GetLocalizationPath());
    $templ = Localize($templ, $locale, GetClientOS());
    print sprintf($templ, $popup, GetSurroundVirtualPath() . "selectwithin.php", $mapName, $sessionId, $dwf);


function GetParameters($params)
{
    global $mapName, $sessionId, $dwf, $locale;

    if(isset($params['LOCALE']))
        $locale = $params['LOCALE'];
    $popup = $params['POPUP'];
    $mapName = $params['MAPNAME'];
    $sessionId = $params['SESSION'];
    $dwf = $params['DWF'];
}

function GetRequestParameters()
{
    if($_SERVER['REQUEST_METHOD'] == "POST")
        GetParameters($_POST);
    else
        GetParameters($_GET);
}
?>

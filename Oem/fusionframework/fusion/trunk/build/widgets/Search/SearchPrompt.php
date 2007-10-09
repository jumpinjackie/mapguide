<?php

//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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

    $fusionMGpath = '../../MapGuide/php/';
    include $fusionMGpath . 'Common.php';

    $locale = "";
    $popup = 0;
    $mapName = "";
    $sessionId = "";
    $widgetName = "";

    GetRequestParameters();

    $templ = file_get_contents("./SearchPrompt.templ");
    SetLocalizedFilesPath(GetLocalizationPath());
    $templ = Localize($templ, $locale, GetClientOS());
    $vpath = GetSurroundVirtualPath();
    print sprintf($templ, $popup, $widgetName, $vpath."Search.php", $mapName, $sessionId);



function GetParameters($params)
{
    global $popup, $locale;
    global $mapName, $sessionId, $widgetName;

    if(isset($params['locale']))
        $locale = $params['locale'];
    $popup = $params['popup'];
    $widgetName = $params['widgetname'];
    $mapName = $params['mapname'];
    $sessionId = $params['session'];
}

function GetRequestParameters()
{
    if($_SERVER['REQUEST_METHOD'] == "POST")
        GetParameters($_POST);
    else
        GetParameters($_GET);
}

?>

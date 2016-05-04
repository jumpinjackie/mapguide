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

    $locale = "";
    GetRequestParameters();

    SetLocalizedFilesPath(GetLocalizationPath());

    //This detection assumed 2 browsers, IE and Mozilla.
    //Because Mozilla cannot render properly the border within the cell, we show this border only on IE
    $user_agent = strtolower($_SERVER['HTTP_USER_AGENT']);
    if(strpos($user_agent, "msie"))
        $borderColor = "#c0c0c0";
    else
        $borderColor = "#dfdfdf";

    // Load the HTML template, format it and returns it to the client
    //
    print Localize(file_get_contents("../viewerfiles/statusbar.templ"), $locale, GetClientOS());

function GetParameters($params)
{
    global $locale;

    $locale = ValidateLocaleString(GetParameter($params, 'LOCALE'));
}

function GetRequestParameters()
{
    if($_SERVER['REQUEST_METHOD'] == "POST")
        GetParameters($_POST);
    else
        GetParameters($_GET);
}

?>

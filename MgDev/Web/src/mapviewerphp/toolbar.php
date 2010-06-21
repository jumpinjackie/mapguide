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

    $locale = "";
    GetRequestParameters();

    SetLocalizedFilesPath(GetLocalizationPath());

    // pass diferent button height depending on the browser
    // to achieve a similar rendering, as there is no cross-browser conditional CSS
    //
    $user_agent = strtolower($_SERVER['HTTP_USER_AGENT']);
    if(strpos($user_agent, "msie"))
       $btnHeight = 25;
    else
       $btnHeight = 23;

    // return the HTML template
    //
    $templ = Localize(file_get_contents("../viewerfiles/toolbar.templ"), $locale, GetClientOS());
    printf($templ, $btnHeight);

function GetParameters($params)
{
    global $locale;

    if(isset($params['LOCALE']))
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

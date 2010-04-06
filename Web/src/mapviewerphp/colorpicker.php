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
    include 'constants.php';

    $locale = "";
    $clr = "000000";
    $allowTransparency = false;
    $transparent = false;

    GetRequestParameters();

    $templ = file_get_contents("../viewerfiles/colorpicker.templ");
    SetLocalizedFilesPath(GetLocalizationPath());
    $templ = Localize($templ, $locale, GetClientOS());
    print sprintf($templ, $clr, $allowTransparency ? "true": "false", $transparent ? "true": "false");


function GetParameters($params)
{
    global $clr, $allowTransparency, $transparent, $locale;

    $locale = ValidateLocaleString(GetParameter($params, 'LOCALE'));
    $clr = ValidateColorString(GetParameter($params, 'CLR'));
    $allowTransparency = (GetIntParameter($params, 'ALLOWTRANS') == 1);
    $transparent = (GetIntParameter($params, 'TRANS') == 1);
}

function GetRequestParameters()
{
    if($_SERVER['REQUEST_METHOD'] == "POST")
        GetParameters($_POST);
    else
        GetParameters($_GET);
}
?>

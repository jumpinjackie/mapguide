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

  $tgt = 0;
  $popup = 0;
  $dwf = 0;
  $locale = "";

  GetRequestParameters();

  SetLocalizedFilesPath(GetLocalizationPath());

  $templ = Localize(file_get_contents("../viewerfiles/viewoptions.templ"), $locale, GetClientOS());
  print sprintf($templ, $tgt, $popup, $dwf);

function GetParameters($params)
{
    global $tgt, $popup, $dwf, $locale;

    $tgt = $params['TGT'];
    $popup = $params['POPUP'];
    $dwf = $params['DWF'];
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

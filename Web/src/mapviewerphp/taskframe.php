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

$taskPane = "";
$session = "";
$webLayout = "";
$dwf = "";
$locale = "";

GetRequestParameters();

//If there is an initial url, it will be encoded, so parse the decoded url.
$comp = parse_url(urldecode($taskPane));

//If there is a query component to the initial url, append it to the end of the full url string
if(!isset($comp["query"]) || strlen($comp["query"]) == 0)
	$url = sprintf("%s?SESSION=%s&WEBLAYOUT=%s&DWF=%s&LOCALE=%s", $comp["path"], $session, urlencode($webLayout), $dwf, $locale);
else
	$url = sprintf("%s?SESSION=%s&WEBLAYOUT=%s&DWF=%s&LOCALE=%s&%s", $comp["path"], $session, urlencode($webLayout), $dwf, $locale, $comp["query"]);

$templ = file_get_contents("../viewerfiles/taskframe.templ");
print sprintf($templ, GetSurroundVirtualPath()."tasklist.php", $locale, $url);

function GetParameters($params)
{
    global $taskPane, $session, $webLayout, $dwf, $locale;

    $taskPane = $params['TASK'];
    $session = $params['SESSION'];
    $webLayout = $params['WEBLAYOUT'];
    $locale = $params['LOCALE'];
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

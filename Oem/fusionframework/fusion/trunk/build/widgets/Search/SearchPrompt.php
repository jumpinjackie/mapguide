<?php
/**
 * SearchPrompt
 *
 * $Id: $
 *
 * Copyright (c) 2007, DM Solutions Group Inc.
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

    $fusionMGpath = '../../MapGuide/php/';
    include $fusionMGpath . 'Common.php';

    $locale = GetDefaultLocale();
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

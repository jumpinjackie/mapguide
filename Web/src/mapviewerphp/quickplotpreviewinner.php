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

    include 'common.php';
    include 'constants.php';
    
    $locale = GetDefaultLocale();
    $scaleDenominator;
    $annotations;
    
    $args = GetRequestMethod();
    GetParameters($args);

    $templ = file_get_contents("../viewerfiles/QuickPlotPreviewInner.templ");
    SetLocalizedFilesPath(GetLocalizationPath());
    
    // Localize the page
    $templ = Localize($templ, $locale, GetClientOS());
    
    // Set some annotation labels in the page by replacing the placeholders in the html code
    $templ = str_replace(array_keys($annotations), array_values($annotations), $templ);
    // Set the date annotation according to its format mask
    $pattern = "/\{date:.*\[php=(.+?)\].+?\}/";
    $matches = array();
    if (preg_match($pattern, $templ, $matches))
    {
        $mask  = $matches[1];
        $date  = date($mask);
        $templ = preg_replace($pattern, $date, $templ);
    }
    
    $jsPath    = "../viewerfiles/";
    print sprintf($templ, $jsPath);
?>

<?php
function GetParameters($params)
{
    global $scaleDenominator, $annotations;
    
    $scaleDenominator = intval($params["scale_denominator"]);
    $annotations = array();
    
    // The parameters whose name matches this pattern will be treated as annotation
    $pattern = "/^\{field:.+\}$/i";
    foreach ($params as $key => $value)
    {
        if (preg_match($pattern, $key) == 1)
        {
            $annotations[$key] = htmlspecialchars(urlDecode($value), ENT_QUOTES);
        }
    }
    
    // The scale annotation
    $annotations["{scale}"] = "1 : " . $scaleDenominator;
}

function GetRequestMethod()
{
    if($_SERVER['REQUEST_METHOD'] == "POST")
    {
        return $_POST;
    }
    else
    {
        return $_GET;
    }
}

?>

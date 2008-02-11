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

function InitializeWebTier()
{
    // Initialize web tier with the site configuration file.  The config
    // file should be two directory levels above this script.
    $thisFile = __FILE__;
    $pos = strrpos($thisFile, '\\');
    if ($pos == false)
    {
        $pos = strrpos($thisFile, '/');
    }
    $cfgFile = substr($thisFile, 0, $pos+1) . "../webconfig.ini";
    MgInitializeWebTier($cfgFile);
}

function GetDefaultLocale()
{
    return "en"; // localizable string
}

function GetLocalizationPath()
{
    $thisFile = __FILE__;
    $pos = strrpos($thisFile, '\\');
    if ($pos == false)
        $pos = strrpos($thisFile, '/');
    return substr($thisFile, 0, $pos+1) . "../localized/";
}

function FormatMessage($id, $locale, $params)
{
    $text = GetString($id, $locale);
    for($i = 0; $i < count($params); $i++)
    {
        $tag = "{" . $i . "}";
        $pos1 = strpos($text, $tag);
        if($pos1 == false)
            break;
        if($pos1 > 0)
            $begin = substr($text, 0, $pos1);
        else
            $begin = "";
        $end = substr($text, $pos1 + strlen($tag));
        $text = $begin . $params[$i] . $end;
    }
    return $text;
}

function GetSurroundVirtualPath()
{
    if (isset($_SERVER["REQUEST_URI"]))
    {
        $path = $_SERVER["REQUEST_URI"];
        $baseuri = substr($path, 0, strrpos($path, '?') + 1);
    }
    else
    {
        $baseuri = $_SERVER["PHP_SELF"];
    }

    return substr($baseuri, 0, strrpos($baseuri, '/') + 1);
}

function GetRootVirtualFolder()
{
    if (isset($_SERVER["REQUEST_URI"]))
    {
        $path = $_SERVER["REQUEST_URI"];
        $baseuri = substr($path, 0, strrpos($path, '?') + 1);
    }
    else
    {
        $baseuri = $_SERVER["PHP_SELF"];
    }

    return substr($baseuri, 0, strpos($baseuri, '/', 1));
}

function GetClientOS() {
    if(!isset($_SERVER['HTTP_USER_AGENT']))
        return 0;
    $agent = $_SERVER['HTTP_USER_AGENT'];
    if(strpos($agent, "Windows") != FALSE)
        return 0;
    else if(strpos($agent, "Macintosh") != FALSE)
        return 1;
    return 2;
}

function EscapeForHtml($str)
{
    $org = array("'", "\"", "\n", "<", ">");
    $repl = array("&#39;", "&quot;", " ", "&lt;", "&gt;" );
    return str_replace($org, $repl, $str);
}

function GetDecimalFromLocalizedString($numberString, $locale)
{
    if($locale != null && $numberString != null)
    {
        // Remove thousand separators
        $thousandSeparator = GetLocalizedString("THOUSANDSEPARATOR", $locale);
        if($thousandSeparator != null && strlen($thousandSeparator) > 0)
        {
            $numberString = str_replace($thousandSeparator, "", $numberString);
        }
        
        // Replace localized decimal separators with "."
        $decimalSeparator = GetLocalizedString("DECIMALSEPARATOR", $locale);
        if($decimalSeparator != null && strlen($decimalSeparator) > 0 && $decimalSeparator != ".")
        {
            $numberString = str_replace($decimalSeparator, ".", $numberString);
        }
    }
    return $numberString;
}

?>

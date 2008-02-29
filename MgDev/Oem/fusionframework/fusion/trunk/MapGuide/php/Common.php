<?php
/**
 * Common
 *
 * $Id: Common.php 1237 2008-02-20 23:46:47Z cclaydon $
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

/*****************************************************************************
 * Purpose: common include file for all server side scripts, responsible for
 *          setting up the user and site connection.
 *****************************************************************************/

/*
 * If MapGuide Web Server Extensions are not installed in the default
 * location, you may need to edit these values to reflect the actual
 * installation path.  You do not need to modify any other values in
 * this file to configure Fusion.
 */

$defaultInstallDir = dirname(__FILE__)."/../../../../../";
$defaultExtensionDir = realpath($defaultInstallDir . "webserverextensions/www/");

/**
 * Developer's notes:
 *
 * This file should be included by all server-side scripts.  It includes some
 * default paths to install locations and manages all session-related and
 * user-related stuff except for actually creating a session.
 *
 * It also creates a MgResourceService instance for use by other scripts
 * and recreates an MgMap instance from the session if a mapname is passed.
 *
 * For widgets that are installed outside of the Fusion directory structure,
 * the value of $extensionDir can be set before including this script to avoid
 * some problems.
 */

//widgets outside Fusion can set the $extensionDir before including Common.php
//TODO: I don't think this is necessary anymore (PS)
if (!isset($extensionDir)){
    $installDir = $defaultInstallDir;
    $extensionDir = $defaultExtensionDir;
}

include dirname(__FILE__)."/Constants.php";
// Initialize

MgInitializeWebTier($extensionDir. "/webconfig.ini");
try {
    /* If no session has been established yet, then we use the credentials passed
     * to this script to connect to the site server.  By default, we use the
     * Anonymous user.
     */
    if (!isset($_REQUEST['session'])) {
        $username = isset($_REQUEST['username']) ? $_REQUEST['username'] : 'Anonymous';
        $password = isset($_REQUEST['password']) ? $_REQUEST['password'] : '';
        $user = new MgUserInformation($username, $password);
        $siteConnection = new MgSiteConnection();
        $siteConnection->Open($user);
    } else {
        /* If a session has previously been established, we can connect using the
         * credentials that the user logged in with previously ... these are stored
         * in the MapGuide session.
         * It is possible for the user to re-authenticate using a different set of
         * credentials.  Handle this here, but keep the session the same.
         */
        $sessionID = $_REQUEST['session'];
        session_id(str_replace('_', '-', $sessionID));
        session_start();

        /* current user is re-authenticating or not? */
        if (isset($_REQUEST['username']) && isset($_REQUEST['password'])) {
            $user = new MgUserInformation($_REQUEST['username'], $_REQUEST['password']);
            $user->SetMgSessionId($sessionID);
        } else {
            $user = new MgUserInformation($sessionID);
        }

        /* open a connection to the site.  This will generate exceptions if the user
         * is set up properly - not found, invalid password etc
         */
        $siteConnection = new MgSiteConnection();
        $siteConnection->Open($user);

        /* MgUserInformation doesn't tell us who is logged in so we store it
         * in the php session for convenience
         */
        if (isset($_REQUEST['username']) && isset($_REQUEST['password'])) {
            $_SESSION['username'] = $_REQUEST['username'];
        }
        //echo "<current user: >".$_SESSION['username']. '</current>';
    }
} catch (MgAuthenticationFailedException $afe) {
    header('Content-type: text/xml');
    echo "<Exception>";
    echo "<Type>Authentication Failed</Type>";
    echo "<Message>" . $afe->GetMessage() . "</Message>";
    echo "<Details>" . $afe->GetDetails() . "</Details>";
    echo "</Exception>";
    exit;
} catch (MgUserNotFoundException $unfe) {
    header("HTTP/1.0 500 Internal Server Error");
    header('Content-type: text/xml');
    echo "<Exception>";
    echo "<Type>User Not Found</Type>";
    echo "<Message>" . $unfe->GetMessage() . "</Message>";
    echo "<Details>" . $unfe->GetDetails() . "</Details>";
    echo "</Exception>";
    exit;
} catch (MgSessionExpiredException $see) {
    header('Content-type: text/xml');
    echo "<Exception>";
    echo "<Type>Session Expired</Type>";
    echo "<Message>" . $see->GetMessage() . "</Message>";
    echo "<Details>" . $see->GetDetails() . "</Details>";
    echo "</Exception>";
    exit;
} catch (MgException $e) {
    header('Content-type: text/xml');
    echo "<Exception>";
    echo "<Type>Exception</Type>";
    echo "<Message>" . $e->GetMessage() . "</Message>";
    echo "<Details>" . $e->GetDetails() . "</Details>";
    echo "</Exception>";
    exit;
}


//common resource service to be used by all scripts
$resourceService = $siteConnection->CreateService(MgServiceType::ResourceService);

if (isset($_REQUEST['mapname'])) {
    $mapName = $_REQUEST['mapname'];
    $mapResourceID = new MgResourceIdentifier( 'Session:'.$sessionID.'//'.$mapName.'.MapDefinition');
    $mapStateID = new MgResourceIdentifier('Session:'.$sessionID.'//'.$mapName.'.'.MgResourceType::Map);
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
    return substr($thisFile, 0, $pos+1) . "../../../localized/";
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

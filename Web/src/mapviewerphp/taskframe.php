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
include 'constants.php';

$sessionId = "";
$webLayoutId = "";
$dwf = 0;
$locale = "";

GetRequestParameters();
SetLocalizedFilesPath(GetLocalizationPath());

try
{
    InitializeWebTier();

    $cred = new MgUserInformation($sessionId);
    $cred->SetClientIp(GetClientIp());
    $cred->SetClientAgent(GetClientAgent());

    //Connect to the site
    $site = new MgSiteConnection();
    $site->Open($cred);

    //Get the MgWebLayout object
    $resourceSrvc = $site->CreateService(MgServiceType::ResourceService);
    $webLayoutResId = new MgResourceIdentifier($webLayoutId);
    $webLayout = new MgWebLayout($resourceSrvc, $webLayoutResId);
    $taskPane = $webLayout->GetTaskPane();
    $taskPaneUrl = $taskPane->GetInitialTaskUrl();
    $vpath = GetSurroundVirtualPath();
    if ($taskPaneUrl == null || strlen($taskPaneUrl) == 0)
    {
        $taskPaneUrl = "gettingstarted.php";
    }

    //If there is an initial url, it will be encoded, so parse the decoded url.
    $tpDecodedUrl = urldecode($taskPaneUrl);
    $baseUrl = $tpDecodedUrl;
    $query = "";
    
    //Is relative?
    if (strncasecmp($tpDecodedUrl, "http://", 7) != 0)
        $baseUrl = $vpath . $tpDecodedUrl;
        
    //Strip query string component off of base url (if there is any) and assign to query component
    $tokens = explode("?", $baseUrl);
    if (count($tokens) == 2)
    {
        $query = $tokens[1];
        $baseUrl = $tokens[0];
    }
    
    //If there is a query component to the initial url, append it to the end of the full url string
    if (strlen($query) == 0)
        $url = sprintf("%s?SESSION=%s&WEBLAYOUT=%s&DWF=%s&LOCALE=%s", $baseUrl, $sessionId, urlencode($webLayoutId), $dwf, $locale);
    else
        $url = sprintf("%s?SESSION=%s&WEBLAYOUT=%s&DWF=%s&LOCALE=%s&%s", $baseUrl, $sessionId, urlencode($webLayoutId), $dwf, $locale, $query);

    $templ = file_get_contents("../viewerfiles/taskframe.templ");
    print sprintf($templ, $vpath ."tasklist.php", $locale, $url);
}
catch(MgException $e)
{
    OnError(GetLocalizedString( "TASKS", $locale ), $e->GetDetails());
    return;
}
catch(Exception $ne)
{
    OnError(GetLocalizedString( "TASKS", $locale ), $ne->getMessage());
    return;
}

function GetParameters($params)
{
    global $taskPane, $sessionId, $webLayoutId, $dwf, $locale;

    $sessionId = ValidateSessionId(GetParameter($params, 'SESSION'));
    $locale = ValidateLocaleString(GetParameter($params, 'LOCALE'));
    $webLayoutId = ValidateResourceId(GetParameter($params, 'WEBLAYOUT'));
    $dwf = GetIntParameter($params, 'DWF');
}

function GetRequestParameters()
{
    if($_SERVER['REQUEST_METHOD'] == "POST")
        GetParameters($_POST);
    else
        GetParameters($_GET);
}

function OnError($title, $msg)
{
    global $target;
    $templ = Localize(file_get_contents("../viewerfiles/errorpage.templ"), $locale, GetClientOS());
    print sprintf($templ, "0", $title, $msg);
}

?>

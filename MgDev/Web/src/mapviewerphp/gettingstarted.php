<?php

//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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

    $cmdListPage = "WS1a9193826455f5ff9110c71085341391d-2e28.html";

    $webLayout = "";
    $pageName = "";
    $locale = "";
    $dwf = false;

    GetRequestParameters();

    try
    {
        if($pageName == "") {
            //no page name specified, assume main getting started page (the command list)
            $pageName = $cmdListPage;
        }
        //load the original page
        $locpath = GetLocalizationPath();
        if(file_exists($locpath . "/help/" . $locale . "/" . $pageName))
            $orgHtml = file_get_contents($locpath . "/help/" . $locale . "/" . $pageName);
        else {
            if(file_exists($locpath . "/help/en/" . $pageName)) {
                $orgHtml = file_get_contents($locpath . "/help/en/" . $pageName);
                $locale = "en";
            }
            else {
                echo "";
                return;
            }
        }
        $fixedupHtml = FixupPageReferences($orgHtml, $webLayout, $dwf, GetRootVirtualFolder() . "/", $locale);
        if($pageName == $cmdListPage) {
            //filter out unused commands
            //
            InitializeWebTier();
            $cred = new MgUserInformation("Administrator", "admin");
            $site = new MgSiteConnection();
            $site->Open($cred);
            $wli = new MgResourceIdentifier($webLayout);
            $src = new MgByteSource($fixedupHtml, strlen($fixedupHtml));
            $wl = new MgWebLayout($wli);
            $pagestream = $wl->ProcessGettingStartedPage($src->GetReader(), $dwf);
            if($pagestream == null)
                echo $fixedupHtml;
            else
                echo $pagestream->ToString();
        }
        else
            echo $fixedupHtml;
    }
    catch(MgException $e)
    {
        echo $e->GetDetails();
        return;
    }


function GetParameters($params)
{
    global $webLayout, $pageName, $dwf, $locale;

    $webLayout = $params['WEBLAYOUT'];
    if(isset($params['PAGE']))
        $pageName = $params['PAGE'];
    $dwf = $params['DWF'] == "1";
    if(isset($params['LOCALE']))
        $locale = $params['LOCALE'];
    else
        $locale = "en";
}

function GetRequestParameters()
{
    if($_SERVER['REQUEST_METHOD'] == "POST")
        GetParameters($_POST);
    else
        GetParameters($_GET);
}

function FixupPageReferences($html, $webLayout, $dwf, $vroot, $locale) {
    $htmlPrefix = "gettingstarted.php?WEBLAYOUT=" . $webLayout . "&DWF=". ($dwf?"1":"0") . "&PAGE=";
    $imgScrPrefix = $vroot . "localized/help/" . $locale . "/";
    $index = 0;
    $res = "";
    do
    {
        $found = false;
        $i = strpos($html, "href=\"", $index);
        $j = strpos($html, "src=\"", $index);
        if($i != FALSE || $j != FALSE) {
            $htmlRef = false;
            $found = true;
            if($i != FALSE) {
                if($j != FALSE) {
                    if($i < $j) {
                        $htmlRef = substr($html, $i - 3, 2) == "<a";
                        $i += 6;
                    }
                    else
                        $i = $j + 5;
                }
                else {
                    $htmlRef = substr($html, $i - 3, 2) == "<a";
                    $i += 6;
                }
            }
            else
                $i = $j + 5;

            $res = $res . substr($html, $index, $i - $index);
            if($htmlRef) {
                if(FixupRequired($html, $i))
                    $res = $res . $htmlPrefix;
            }
            else {
                if(FixupRequired($html, $i))
                    $res = $res . $imgScrPrefix;
            }
            $index = $i;
        }
    } while($found);
    $res = $res . substr($html, $index);
    return $res;
}

function FixupRequired($html, $refIndex) {
    return substr($html, $refIndex, 7) != "http://" && 
           substr($html, $refIndex, 11) != "javascript:";
}


?>

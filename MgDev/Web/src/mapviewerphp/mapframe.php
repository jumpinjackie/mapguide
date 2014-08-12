<?php

//
//  Copyright (C) 2004-2011 by Autodesk, Inc.
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

$mapDefinition = "";
$locale = "";
$infoWidth = 0;
$showLegend = false;
$showProperties = false;
$sessionId = "";
$username = '';
$password = '';
$hlTgt = '';
$hlTgtName = '';
$showSlider = true;
$selectionColor = '0000FFFF';
$mapImgFormat = 'PNG';
$selImgFormat = 'PNG';
$pointBufferSize = 2;

GetRequestParameters();

SetLocalizedFilesPath(GetLocalizationPath());

try
{
    InitializeWebTier();

    // create the map instance and store it with the session
    //
    $userInfo = new MgUserInformation();
    $userInfo->SetMgSessionId($sessionId);
    $userInfo->SetClientIp(GetClientIp());
    $userInfo->SetClientAgent(GetClientAgent());

    $site = new MgSiteConnection();
    $site->Open($userInfo);

    $resourceSrvc = $site->CreateService(MgServiceType::ResourceService);

    $map = new MgMap($site);
    $resId = new MgResourceIdentifier($mapDefinition);
    $mapName = $resId->GetName();
    $map->Create($resId, $mapName);
    
    $tileSetId = $map->GetTileSetDefinition();
    
    $tileSrvc = $site->CreateService(MgServiceType::TileService);
    $tileSizeX = $tileSrvc->GetDefaultTileSizeX();
    $tileSizeY = $tileSrvc->GetDefaultTileSizeY();
    if (NULL != $tileSetId)
    {
        //Overwrite the map definition with tile set id (this is for GETTILE requests) and
        //use size settings from that tile set
        $mapDefinition = $tileSetId->ToString();
        $tileSizeX = $tileSrvc->GetDefaultTileSizeX($tileSetId);
        $tileSizeY = $tileSrvc->GetDefaultTileSizeY($tileSetId);
    }

    //create an empty selection object and store it in the session repository
    $sel = new MgSelection($map);
    $sel->Save($resourceSrvc, $mapName);

    //get the map extent and calculate the scale factor
    //
    $mapExtent = $map->GetMapExtent();
    $srs = $map->GetMapSRS();
    if($srs != "")
    {
        $csFactory = new MgCoordinateSystemFactory();
        $cs = $csFactory->Create($srs);
        $metersPerUnit = $cs->ConvertCoordinateSystemUnitsToMeters(1.0);
        $unitsType = $cs->GetUnits();
    }
    else
    {
        $metersPerUnit = 1.0;
        $unitsType = GetLocalizedString("DISTANCEMETERS", $locale);
    }

    $llExtent = $mapExtent->GetLowerLeftCoordinate();
    $urExtent = $mapExtent->GetUpperRightCoordinate();
    $bgColor = $map->GetBackgroundColor();
    if(strlen($bgColor) == 8)
        $bgColor = '#' . substr($bgColor, 2);
    else
        $bgColor = "white";

    $scaleCreationCode = "";
    $scales = array();
    for($i = 0; $i < $map->GetFiniteDisplayScaleCount(); $i ++)
        $scales[$i] = $map->GetFiniteDisplayScaleAt($i);
    sort($scales);
    for($i = 0; $i < count($scales); $i ++)
        $scaleCreationCode = $scaleCreationCode . "scales[" . $i . "]=" . str_replace(",", ".", $scales[$i]) . "; ";

    $mapStateId = new MgResourceIdentifier("Session:" . $sessionId . "//" . $mapName . "." . MgResourceType::Map);
    $map->Save($resourceSrvc, $mapStateId);
    $templ = Localize(file_get_contents("../viewerfiles/ajaxmappane.templ"), $locale, GetClientOS());
    $vpath = GetSurroundVirtualPath();
    printf ($templ,
                $tileSizeX,
                $tileSizeY,
                GetRootVirtualFolder() . "/mapagent/mapagent.fcgi",
                $mapName,
                $mapDefinition,
                $infoWidth,
                $showLegend?"true":"false",
                $showProperties?"true":"false",
                $sessionId,
                $llExtent->GetX(), $llExtent->GetY(), $urExtent->GetX(), $urExtent->GetY(),
                $metersPerUnit,
                $unitsType,
                $bgColor,
                $hlTgt, $hlTgtName,
                $showSlider? "true": "false",
                $locale,
                $scaleCreationCode,
                $selectionColor,
                $mapImgFormat,
                $selImgFormat,
                $pointBufferSize,
                $vpath . "ajaxviewerabout.php",
                $vpath . "legendctrl.php",
                urlencode($mapName),
                $sessionId,
                $locale,
                $vpath . "propertyctrl.php",
                $locale
                );
}
catch(MgException $e)
{
    $errorMsg = EscapeForHtml($e->GetDetails());
    echo $errorMsg;
}

//load ajax template code and format it
function GetParameters($params)
{
    global $mapDefinition;
    global $infoWidth, $showLegend, $showProperties, $sessionId;
    global $locale, $hlTgt, $hlTgtName, $showSlider;
    global $selectionColor, $mapImgFormat, $selImgFormat, $pointBufferSize;

    $sessionId = ValidateSessionId(GetParameter($params, 'SESSION'));
    $locale = ValidateLocaleString(GetParameter($params, 'LOCALE'));
    $hlTgt = ValidateHyperlinkTargetValue(GetParameter($params, 'HLTGT'));
    $hlTgtName = ValidateFrameName(GetParameter($params, 'HLTGTNAME'));
    $mapDefinition = ValidateResourceId(GetParameter($params, 'MAPDEFINITION'));
    $showLegend = (GetIntParameter($params, 'SHOWLEGEND') == 1);
    $showProperties = (GetIntParameter($params, 'SHOWPROP') == 1);
    $showSlider = (GetIntParameter($params, 'SHOWSLIDER') == 1);
    $infoWidth = GetIntParameter($params, 'INFOWIDTH');
    $selectionColor = ValidateColorString(GetParameter($params, 'SELCOLOR'), 8);
    $mapImgFormat = GetParameter($params, 'MAPIMGFORMAT');
    $selImgFormat = GetParameter($params, 'SELIMGFORMAT');
    $pointBufferSize = GetIntParameter($params, 'POINTBUFFERSIZE');
}

function GetRequestParameters()
{
    if($_SERVER['REQUEST_METHOD'] == "POST")
        GetParameters($_POST);
    else
        GetParameters($_GET);
}

?>

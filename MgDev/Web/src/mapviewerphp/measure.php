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

    $locale = "";
    $mapName = "";
    $sessionId = "";
    $target = 0;
    $popup = 0;
    $clear = false;
    $x1 = 0;
    $y1 = 0;
    $x2 = 0;
    $y2 = 0;
    $total = 0;
    $srs = "";
    $us = 1;
    $segId = 1;
    $error = "";
    $distance = 0;
    $legendName = "Measure";
    $featureName = "Measure";

    SetLocalizedFilesPath(GetLocalizationPath());

    GetRequestParameters();

    $dataSource = "Session:" . $sessionId . "//Measure.FeatureSource";
    $layerDef = "Session:" . $sessionId . "//Measure.LayerDefinition";

    try
    {
        InitializeWebTier();

        $cred = new MgUserInformation($sessionId);
        $cred->SetClientIp(GetClientIp());
        $cred->SetClientAgent(GetClientAgent());

        //connect to the site and get a feature service and a resource service instances
        $site = new MgSiteConnection();
        $site->Open($cred);
        $featureSrvc = $site->CreateService(MgServiceType::FeatureService);
        $resourceSrvc = $site->CreateService(MgServiceType::ResourceService);

        $dataSourceId = new MgResourceIdentifier($dataSource);
        $layerDefId = new MgResourceIdentifier($layerDef);

        //load the map runtime state and locate the measure layer
        //
        $map = new MgMap();
        $map->Open($resourceSrvc, $mapName);
        $layers = $map->GetLayers();
        $srs = GetMapSrs($map);

        $layer = FindLayer($layers, $layerDef);

        if($clear)
        {
            $total = 0;
            if(DataSourceExists($resourceSrvc, $dataSourceId))
                ClearDataSource($featureSrvc, $dataSourceId, $featureName);
        }
        else
        {
            $srsFactory = new MgCoordinateSystemFactory();
            $srsMap = $srsFactory->Create($srs);

            $srsType = $srsMap->GetType();
            if($srsType == MgCoordinateSystemType::Geographic)
                $distance = $srsMap->MeasureGreatCircleDistance($x1, $y1, $x2, $y2);
            else
                $distance = $srsMap->MeasureEuclideanDistance($x1, $y1, $x2, $y2);

            $distance = $srsMap->ConvertCoordinateSystemUnitsToMeters($distance);
            if(!$us)
                $distance *= 0.001;             //get kilometers
            else
                $distance *= 0.000621371192;    //get miles

            $total += $distance;

            //create the line string geometry representing this segment
            //
            $geomFactory = new MgGeometryFactory();
            $coordinates = new MgCoordinateCollection();
            $coordinates->Add($geomFactory->CreateCoordinateXY($x1, $y1));
            $coordinates->Add($geomFactory->CreateCoordinateXY($x2, $y2));
            $geom = $geomFactory->CreateLineString($coordinates);

            if($segId == 1)
            {
                //first segment
                //
                if(!DataSourceExists($resourceSrvc, $dataSourceId))
                {
                    //create feature source
                    //
                    $classDef = new MgClassDefinition();

                    $classDef->SetName($featureName);
                    $classDef->SetDescription(GetLocalizedString("MEASUREFEATURECLASS", $locale));
                    $classDef->SetDefaultGeometryPropertyName("GEOM");

                    //Set KEY property
                    $prop = new MgDataPropertyDefinition("KEY");
                    $prop->SetDataType(MgPropertyType::Int32);
                    $prop->SetAutoGeneration(true);
                    $prop->SetReadOnly(true);
                    $classDef->GetIdentityProperties()->Add($prop);
                    $classDef->GetProperties()->Add($prop);

                    //Set PARTIAL property. Hold the distance for this segment
                    $prop = new MgDataPropertyDefinition("PARTIAL");
                    $prop->SetDataType(MgPropertyType::Double);
                    $classDef->GetProperties()->Add($prop);

                    //Set TOTAL property. Hold the total distance up to this segment, including it
                    $prop = new MgDataPropertyDefinition("TOTAL");
                    $prop->SetDataType(MgPropertyType::Double);
                    $classDef->GetProperties()->Add($prop);

                    //Set geometry property
                    $prop = new MgGeometricPropertyDefinition("GEOM");
                    //$prop->SetGeometryTypes(MgFeatureGeometricType::mfgtSurface); //TODO use the constant when exposed
                    $prop->SetGeometryTypes(4);
                    $classDef->GetProperties()->Add($prop);

                    //Create the schema
                    $schema = new MgFeatureSchema("MeasureSchema", GetLocalizedString("MEASURESCHEMADESCR", $locale));
                    $schema->GetClasses()->Add($classDef);

                    //finally, creation of the feature source
                    $params = new MgCreateSdfParams("MapSrs", $srs, $schema);
                    $featureSrvc->CreateFeatureSource($dataSourceId, $params);

                    //build map tip
                    $unitText = $us? GetLocalizedString("DISTANCEMILES", $locale): GetLocalizedString("DISTANCEKILOMETERS", $locale);
                    $tip = sprintf("Concat(Concat(Concat('" . GetLocalizedString("MEASUREPARTIAL", $locale) . ": ', PARTIAL), Concat(', " . GetLocalizedString("MEASURETOTAL", $locale) . ": ', TOTAL)), ' (%s)')", $unitText);

                    //Create the layer definition
                    $layerDefContent = BuildLayerDefinitionContent($dataSource, $featureName, $tip);
                    $resourceSrvc->SetResource($layerDefId, $layerDefContent, null);

                    //Add the layer to the map, if it's not already in it
                    if($layer == null)
                    {
                        $legendName = GetLocalizedString("MEASURELAYER", $locale);
                        $layer = new MgLayer($layerDefId, $resourceSrvc);
                        $layer->SetDisplayInLegend(true);
                        $layer->SetLegendLabel($legendName);
                        $layers->Insert(0, $layer);
                    }
                }
                else
                {
                    //data source already exist. clear its content
                    ClearDataSource($featureSrvc, $dataSourceId, $featureName);
                }
            }
            // create a feature representing this segment and insert it into the data source
            //
            $measureProps = new MgPropertyCollection();

            $partialProp = new MgDoubleProperty("PARTIAL", $distance);
            $measureProps->Add($partialProp);

            $totalProp = new MgDoubleProperty("TOTAL", $total);
            $measureProps->Add($totalProp);

            $agf = new MgAgfReaderWriter();
            $geomReader = $agf->Write($geom);
            $geomProp = new MgGeometryProperty("GEOM", $geomReader);
            $measureProps->Add($geomProp);

            $cmd = new MgInsertFeatures($featureName, $measureProps);
            $commands = new MgFeatureCommandCollection();
            $commands->Add($cmd);

            ReleaseReader($featureSrvc->UpdateFeatures($dataSourceId, $commands, false));
        }

        if($layer != null)
            $layer->ForceRefresh();
        $map->Save($resourceSrvc);
    }
    catch(MgException $e)
    {
        OnError(GetLocalizedString("MEASUREERROR", $locale), $e->GetDetails());
        return;
    }
    catch(Exception $ne)
    {
        OnError(GetLocalizedString("MEASUREERROR", $locale), $ne->getMessage());
        return;
    }

    $templ = file_get_contents("../viewerfiles/measureui.templ");

    $templ = Localize($templ, $locale, GetClientOS());
    $vpath = GetSurroundVirtualPath();
    print sprintf($templ,
                $locale,
                $target,
                $popup,
                $mapName,
                $sessionId,
                $total,
                $distance,
                1,
                $vpath . "measure.php",
                $vpath . "measure.php");


function OnError($title, $msg)
{
    global $target, $popup;
    $templ = Localize(file_get_contents("../viewerfiles/errorpage.templ"), $locale, GetClientOS());
    print sprintf($templ, $popup, $title, $msg);
}

function DataSourceExists($resourceSrvc, $dataSourceId)
{
    try
    {
        $cnt = $resourceSrvc->GetResourceContent($dataSourceId);
        return true;
    }
    catch(MgResourceNotFoundException $rnfe)
    {
        return false;
    }
}

function BuildLayerDefinitionContent($dataSource, $featureName, $tip)
{
    $layerTempl = file_get_contents("../viewerfiles/linelayerdef.templ");
    $xmlStr = sprintf($layerTempl,
                      $dataSource,
                      $featureName,
                      "GEOM",
                      $tip,
                      1,
                      "ff0000");
    $src = new MgByteSource($xmlStr, strlen($xmlStr));
    return $src->GetReader();
}

function ClearDataSource($featureSrvc, $dataSourceId, $featureName)
{
    $deleteCmd = new MgDeleteFeatures($featureName, "KEY >= 0");
    $commands = new MgFeatureCommandCollection();
    $commands->Add($deleteCmd);
    $featureSrvc->UpdateFeatures($dataSourceId, $commands, false);
}

function FindLayer($layers, $layerDef)
{
    $layer = null;
    for($i = 0; $i < $layers->GetCount(); $i++)
    {
        $layer1 = $layers->GetItem($i);
        if($layer1->GetLayerDefinition()->ToString() == $layerDef)
        {
            $layer = $layer1;
            break;
        }
    }
    return $layer;
}

function GetParameters($params)
{
    global $mapName, $sessionId, $x1, $y1, $x2, $y2, $popup;
    global $total, $clear, $us, $segId, $target, $locale;

    if(isset($params['LOCALE']))
        $locale = $params['LOCALE'];
    $mapName = $params['MAPNAME'];
    $sessionId = $params['SESSION'];
    $target = $params['TGT'];
    $popup = $params['POPUP'];
    if(isset($params['CLEAR']))
        $clear = true;
    else
    {
        $x1 = $params['X1'];
        $y1 = $params['Y1'];
        $x2 = $params['X2'];
        $y2 = $params['Y2'];
        $total = $params['TOTAL'];
        $us = $params['US'];
        $segId = $params['SEGID'];
    }
}

function GetRequestParameters()
{
    if($_SERVER['REQUEST_METHOD'] == "POST")
        GetParameters($_POST);
    else
        GetParameters($_GET);
}

function GetMapSrs($map)
{
    $srs = $map->GetMapSRS();
    if($srs != "")
       return $srs;

    //SRS is currently optional. Waiting for this to change, set the default SRS to ArbitrayXY meters
    //
    return "LOCALCS[\"Non-Earth (Meter)\",LOCAL_DATUM[\"Local Datum\",0],UNIT[\"Meter\", 1],AXIS[\"X\",EAST],AXIS[\"Y\",NORTH]]";
}

function ReleaseReader($res)
{
    if($res == null)
        return;
    $prop = $res->GetItem(0);
    if($prop == null)
        return;
    if ($prop instanceof MgStringProperty)
        throw new Exception($prop->GetValue());
    $reader = $prop->GetValue();
    if($reader == null)
        return;
    $reader->Close();
}

?>

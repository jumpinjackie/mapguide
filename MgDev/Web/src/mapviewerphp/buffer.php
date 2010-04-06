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
    $bufferName = "";
    $layersParam = "";
    $popup = 0;
    $lcolor = "";
    $ffcolor = "";
    $fbcolor = "";
    $transparent = 0;
    $distance = 0;
    $units = "";
    $linestyle = "";
    $fillstyle = "";
    $thickness = 0;
    $merge = 0;
    $foretrans = 50;
    $selText = "";
    $srs = "";
    $featureName = "Buffer";
    $params = null;

    GetRequestParameters();
    SetLocalizedFilesPath(GetLocalizationPath());

    $dataSource = "Session:" . $sessionId . "//" . $bufferName . "_Buffer.FeatureSource";
    $layerDef = "Session:" . $sessionId . "//" . $bufferName . "_Buffer.LayerDefinition";

    try
    {
        $newBuffer = false;

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

        //load the map runtime state
        //
        $map = new MgMap();
        $map->Open($resourceSrvc, $mapName);

        //locate the buffer layer in the map. It might or might not already exist
        //
        $layers = $map->GetLayers();
        $layer = FindLayer($layers, $bufferName);

        $layerNames = split(",", $layersParam);

        // convert distance to meters
        if($units == "mi")              //miles
            $distance *= 1609.35;
        else if($units == "ki")         //kilometers
            $distance *= 1000;
        else if($units == "fe")         //feet
            $distance *= .30480;

        // Get the map SRS
        //
        $srsFactory = new MgCoordinateSystemFactory();
        $srsDefMap = GetMapSRS($map);
        $mapSrsUnits = "";
        $srsMap = $srsFactory->Create($srsDefMap);
        $arbitraryMapSrs = $srsMap->GetType() == MgCoordinateSystemType::Arbitrary;
        if($arbitraryMapSrs)
            $mapSrsUnits = $srsMap->GetUnits();

        //Create/Modify layer definition
        $layerDefContent = BuildLayerDefinitionContent();
        $resourceSrvc->SetResource($layerDefId, $layerDefContent, null);

        if($layer == null)
        {
            $newBuffer = true;
            //Targetting a new layer. create a data source for it
            //
            $classDef = new MgClassDefinition();

            $classDef->SetName($featureName);
            $classDef->SetDescription(GetLocalizedString("BUFFERCLASSDESCR", $locale));
            $classDef->SetDefaultGeometryPropertyName("GEOM");

            //Set KEY property
            $prop = new MgDataPropertyDefinition("KEY");
            $prop->SetDataType(MgPropertyType::Int32);
            $prop->SetAutoGeneration(true);
            $prop->SetReadOnly(true);
            $classDef->GetIdentityProperties()->Add($prop);
            $classDef->GetProperties()->Add($prop);

            //Set ID property.
            $prop = new MgDataPropertyDefinition("ID");
            $prop->SetDataType(MgPropertyType::Int32);
            $classDef->GetProperties()->Add($prop);

            //Set geometry property
            $prop = new MgGeometricPropertyDefinition("GEOM");
            //$prop->SetGeometryTypes(MgFeatureGeometricType::mfgtSurface); //TODO use the constant when exposed
            $prop->SetGeometryTypes(4);
            $classDef->GetProperties()->Add($prop);

            //Create the schema
            $schema = new MgFeatureSchema("BufferSchema", GetLocalizedString("BUFFERSCHEMADESCR", $locale));
            $schema->GetClasses()->Add($classDef);

            //finally, creation of the feature source
            $sdfParams = new MgCreateSdfParams("LatLong", $srsDefMap, $schema);
            $featureSrvc->CreateFeatureSource($dataSourceId, $sdfParams);

            //Add layer to map
            $layer = new MgLayer($layerDefId, $resourceSrvc);
            $layer->SetName($bufferName);
            $layer->SetLegendLabel($bufferName);
            $layer->SetDisplayInLegend(true);
            $layer->SetSelectable(true);
            $layers->Insert(0, $layer);
        }
        else
        {
            //data source already exist. clear its content
            //
            ClearDataSource($featureSrvc, $dataSourceId, $featureName);
        }

        $sel = new MgSelection($map, $selText);

        $selLayers = $sel->GetLayers();

        $agfRW = new MgAgfReaderWriter();
        $bufferGeometries = new MgGeometryCollection();

        $commands = new MgFeatureCommandCollection();
        $featId = 0;

        $propCollection = new MgBatchPropertyCollection();
        $excludedLayers = 0;
        $srsDs = null;
            $inputGeometries = new MgGeometryCollection();

        $bufferFeatures = 0;
        $allCompatible = false;

        for($li = 0; $li < $selLayers->GetCount(); $li++)
        {
            $selLayer = $selLayers->GetItem($li);
            $inputLayer = false;
            $selLayerName = $selLayer->GetName();
            for($il = 0; $il < count($layerNames); $il ++)
            {
                if($layerNames[$il] == $selLayerName)
                {
                    $inputLayer = true;
                    break;
                }
            }
            if($inputLayer == false)
                continue;

            // get the data source SRS
            //
            $featSourceId = new MgResourceIdentifier($selLayer->GetFeatureSourceId());
            $ctxs = $featureSrvc->GetSpatialContexts($featSourceId, false);
            $srsDefDs = "";
            if($ctxs != null && $ctxs->ReadNext())
                $srsDefDs = $ctxs->GetCoordinateSystemWkt();

            if($srsDefDs == "")
            {
                $excludedLayers ++;
                continue;
            }

            $srsDs = $srsFactory->Create($srsDefDs);
            $arbitraryDsSrs = $srsDs->GetType() == MgCoordinateSystemType::Arbitrary;
            $dsSrsUnits = "";

            if($arbitraryDsSrs)
                $dsSrsUnits = $srsDs->GetUnits();

            // exclude layer if:
            //  the map is non-arbitrary and the layer is arbitrary or vice-versa
            //     or
            //  layer and map are both arbitrary but have different units
            //
            if(($arbitraryDsSrs != $arbitraryMapSrs) || ($arbitraryDsSrs && ($dsSrsUnits != $mapSrsUnits)))
            {
                $excludedLayers ++;
                continue;
            }

            // calculate distance in the data source SRS units
            //
            $dist = $srsDs->ConvertMetersToCoordinateSystemUnits($distance);

            // calculate great circle unless data source srs is arbitrary
            if(!$arbitraryDsSrs)
                $measure = $srsDs->GetMeasure();
            else
                $measure = null;

            // create a SRS transformer if necessary.
            if($srsDefDs != $srsDefMap)
                $srsXform = $srsFactory->GetTransform($srsDs, $srsMap);
            else
                $srsXform = null;

            $featureClassName = $selLayer->GetFeatureClassName();
            $filter = $sel->GenerateFilter($selLayer, $featureClassName);
            if($filter == "")
                continue;

            $query = new MgFeatureQueryOptions();
            $query->SetFilter($filter);

            $featureSource = new MgResourceIdentifier($selLayer->GetFeatureSourceId());

            $features = $featureSrvc->SelectFeatures($featureSource, $featureClassName, $query);

            if($features->ReadNext())
            {
                $classDef = $features->GetClassDefinition();
                $geomPropName = $classDef->GetDefaultGeometryPropertyName();

                do
                {
                    $geomReader = $features->GetGeometry($geomPropName);
                    $geom = $agfRW->Read($geomReader);

                    if(!$merge)
                    {
                        $geomBuffer = $geom->Buffer($dist, $measure);
                        if($geomBuffer != null)
                        {
                            if($srsXform != null)
                                $geomBuffer = $geomBuffer->Transform($srsXform);
                            AddFeatureToCollection($propCollection, $agfRW, $featId++, $geomBuffer);
                            $bufferFeatures++;
                        }
                    }
                    else
                    {
                        if($srsXform != null)
                            $geom = $geom->Transform($srsXform);
                        $inputGeometries->Add($geom);
                    }
                }
                while($features->ReadNext());

                $features->Close();
            }
        }

        if($merge)
        {
            if($inputGeometries->GetCount() > 0)
            {
                $dist = $srsMap->ConvertMetersToCoordinateSystemUnits($distance);
                if(!$arbitraryMapSrs)
                    $measure = $srsMap->GetMeasure();
                else
                    $measure = null;

                $geomFactory = new MgGeometryFactory();
                $geomBuffer = $geomFactory->CreateMultiGeometry($inputGeometries)->Buffer($dist, $measure);
                if($geomBuffer != null)
                {
                    AddFeatureToCollection($propCollection, $agfRW, $featId, $geomBuffer);
                    $bufferFeatures = 1;
                }
            }
        }

        if($propCollection->GetCount() > 0)
        {
            $commands->Add(new MgInsertFeatures($featureName, $propCollection));

            //Insert the features in the temporary data source
            //
            $res = $featureSrvc->UpdateFeatures($dataSourceId, $commands, false);
        }

        // Save the new map state
        //
        $layer->ForceRefresh();
        $map->Save($resourceSrvc);

        //build report message
        $title = GetLocalizedString( "BUFFERREPORTTITLE", $locale );
        $createdUpdatedFmt = $newBuffer ? GetLocalizedString( "BUFFERREPORTCREATED", $locale ) : GetLocalizedString( "BUFFERREPORTUPDATED", $locale );
        $createdUpdatedStr = sprintf( $createdUpdatedFmt, $bufferName );
        $featuresFmt = $bufferFeatures > 1 ? GetLocalizedString( "BUFFERREPORTFEATURESPLURAL", $locale ) : GetLocalizedString( "BUFFERREPORTFEATURESSINGULAR", $locale );
        $featuresStr = sprintf( $featuresFmt, $bufferFeatures );
        $msg = $createdUpdatedStr."<p><p>".$featuresStr;

        //add warning message, if necessary
        if($excludedLayers > 0)
        {
            $warningFmt = $excludedLayers > 1 ? GetLocalizedString( "BUFFERREPORTWARNINGPLURAL", $locale ) : GetLocalizedString( "BUFFERREPORTWARNINGSINGULAR", $locale );
            $warningStr = sprintf( $warningFmt, $excludedLayers );
            $msg = $msg."<p><p>".$warningStr;
        }

        // return the report page
        $templ = file_get_contents("../viewerfiles/bufferreport.templ");
        $templ = Localize($templ, $locale, GetClientOS());
        print sprintf($templ, $popup, $title, $msg);
    }
    catch(MgException $e)
    {
        OnError(GetLocalizedString( "BUFFERREPORTERRORTITLE", $locale ), $e->GetDetails());
        return;
    }
    catch(Exception $ne)
    {
        OnError(GetLocalizedString( "BUFFERREPORTERRORTITLE", $locale ), $ne->getMessage());
        return;
    }


function OnError($title, $msg)
{
    global $target, $popup;
    $templ = Localize(file_get_contents("../viewerfiles/errorpage.templ"), $locale, GetClientOS());
    print sprintf($templ, $popup, $title, $msg);
}

function GetParameters()
{
    global $params, $selText, $locale;
    global $mapName, $sessionId, $bufferName, $lcolor, $ffcolor, $fbcolor, $layersParam, $popup;
    global $transparent, $distance, $units, $linestyle, $fillstyle, $thickness, $merge, $foretrans;

    $sessionId = ValidateSessionId(GetParameter($params, 'SESSION'));
    $locale = ValidateLocaleString(GetParameter($params, 'LOCALE'));
    $mapName = ValidateMapName(GetParameter($params, 'MAPNAME'));

    $lcolor = ValidateColorString(GetParameter($params, 'LCOLOR'));
    $ffcolor = ValidateColorString(GetParameter($params, 'FFCOLOR'));
    $fbcolor = ValidateColorString(GetParameter($params, 'FBCOLOR'));
    $popup = GetIntParameter($params, 'POPUP');
    $transparent = GetIntParameter($params, 'TRANSPARENT');
    $distance = GetDecimalFromLocalizedString(GetParameter($params, 'DISTANCE'), $locale);
    if(isset($params['MERGE']))
        $merge = 1;
    $foretrans = GetDoubleParameter($params, 'FORETRANS');
    $thickness = GetDoubleParameter($params, 'THICKNESS');
    $bufferName = GetParameter($params, 'BUFFER');
    
    $layersParam = GetParameter($params, 'LAYERS');
    $units = GetParameter($params, 'UNITS');
    $linestyle = GetParameter($params, 'LINESTYLE');
    $fillstyle = GetParameter($params, 'FILLSTYLE');
    $selText = GetParameter($params, 'SELECTION');

    //unescape strings
    //
    if(ini_get("magic_quotes_sybase") == "1")
        $selText = str_replace("''", "'", $selText);
    else if(get_magic_quotes_gpc() == "1")
    {
        //Unescape double quotes
        $selText = str_replace('\\"', '"', $selText);

        //remove additional backslash
        $selText = str_replace("\\", "", $selText);
    }

    if((int)$foretrans < 0 || (int)$foretrans > 100)
        $foretrans = 50;
}

function GetRequestParameters()
{
    global $params;
    if($_SERVER['REQUEST_METHOD'] == "POST")
        $params = $_POST;
    else
        $params = $_GET;

    GetParameters();
}

function BuildLayerDefinitionContent()
{
    global $dataSource, $featureName, $ffcolor, $fbcolor, $transparent, $linestyle, $thickness, $lcolor, $fillstyle, $foretrans;

    $xtrans = sprintf("%02x", 255 * $foretrans / 100);
    $layerTempl = file_get_contents("../viewerfiles/arealayerdef.templ");
    $xmlStr = sprintf($layerTempl,
                      $dataSource,
                      $featureName,
                      "GEOM",
                      $fillstyle,
                      $xtrans . $ffcolor,
                      $transparent? "ff" . $fbcolor: "00" . $fbcolor,
                      $linestyle,
                      $thickness,
                      $lcolor);
    $src = new MgByteSource($xmlStr, strlen($xmlStr));
    return $src->GetReader();
}

function FindLayer($layers, $layerName)
{
    $layer = null;
    for($i = 0; $i < $layers->GetCount(); $i++)
    {
        $layer1 = $layers->GetItem($i);
        if($layer1->GetName() == $layerName)
        {
            $layer = $layer1;
            break;
        }
    }
    return $layer;
}

function ClearDataSource($featureSrvc, $dataSourceId, $featureName)
{
    $deleteCmd = new MgDeleteFeatures($featureName, "ID >= 0");
    $commands = new MgFeatureCommandCollection();
    $commands->Add($deleteCmd);
    $featureSrvc->UpdateFeatures($dataSourceId, $commands, false);
}

function AddFeatureToCollection($propCollection, $agfRW, $featureId, $featureGeom)
{
    $bufferProps = new MgPropertyCollection();
    $idProp = new MgInt32Property("ID", $featureId);
    $bufferProps->Add($idProp);
    $geomReader = $agfRW->Write($featureGeom);
    $geomProp = new MgGeometryProperty("GEOM", $geomReader);
    $bufferProps->Add($geomProp);
    $propCollection->Add($bufferProps);
}

function GetMapSrs($map)
{
    $srs = $map->GetMapSRS();
    if($srs != "")
        return $srs;

    //No SRS, set to ArbitrayXY meters
    //
    return "LOCALCS[\"Non-Earth (Meter)\",LOCAL_DATUM[\"Local Datum\",0],UNIT[\"Meter\", 1],AXIS[\"X\",EAST],AXIS[\"Y\",NORTH]]";
}

?>

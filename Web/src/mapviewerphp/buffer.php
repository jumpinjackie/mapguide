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

    $locale = "";
    $mapName = "";
    $sessionId = "";
    $bufferName = "";
    $layersParam = "";
    $popup = 0;
    $lcolor = "";
    $ffcolor = "";
    $fbcolor = "";
    $transparent = "";
    $distance = "";
    $units = "";
    $linestyle = "";
    $fillstyle = "";
    $thickness = "";
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

        // calculate the distance in map units
        $srsFactory = new MgCoordinateSystemFactory();
        $srsDefMap = $map->GetMapSRS();
        $srsMap = null;
        if($srsDefMap != "")
        {
            $srsMap = $srsFactory->Create($srsDefMap);
            $arbitraryMapSrs = $srsMap->GetType() == MgCoordinateSystemType::Arbitrary;
            if($arbitraryMapSrs == false)
                $distance = $srsMap->ConvertMetersToCoordinateSystemUnits($distance);
        }
        else
            $arbitraryMapSrs = true;

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
            $sdfParams = new MgCreateSdfParams("LatLong", $srs, $schema);
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
        $prevArbSrs = "";
        $arbSrs = null;

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
            $ctxs = $featureSrvc->GetSpatialContexts($featSourceId, true);
            $srsDefDs = "";
            if($ctxs != null)
            {
                $ctxs->ReadNext();
                $srsDefDs = $ctxs->GetCoordinateSystemWkt();
                if($srsDefDs == "")
                {
                    $excludedLayers ++;
                    continue;
                }
            }
            else
            {
                $excludedLayers ++;
                continue;
            }

            $srsDs = $srsFactory->Create($srsDefDs);
            $arbitraryDsSrs = $srsDs->GetType() == MgCoordinateSystemType::Arbitrary;
            if($arbitraryDsSrs)
            {
                if($prevArbSrs != "" && $prevArbSrs != $srsDefDs && $arbitraryMapSrs && $merge)
                    throw Exception(GetLocalizedString("BUFFERDIFFARBXY", $locale));
                $prevArbSrs = $srsDefDs;
                if($arbSrs == null)
                    $arbSrs = $srsDs;
            }

            if($arbitraryDsSrs != $arbitraryMapSrs)
            {
                //exclude this layer because its srs is incompatible with the map srs
                $excludedLayers ++;
                continue;
            }

            // calculate great circle unless data source srs is arbitrary
            if($arbitraryDsSrs == true)
                $measure = new MgCoordinateSystemMeasure($srsDs);
            else
                $measure = null;

            // create a SRS transformer if necessary
            if(($arbitraryMapSrs == false) && ($arbitraryDsSrs == false) && $srsDefDs != $srsDefMap)
                $srsXform = new MgCoordinateSystemTransform($srsDs, $srsMap);
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

                    if($srsXform != null)
                    {
                        $wkt = new MgWktReaderWriter();
                        $geomDest = $geom->Transform($srsXform);
                    }
                    else
                        $geomDest = $geom;

                    if(!$merge)
                    {
                        $geomBuffer = $geomDest->Buffer($distance, $measure);
                        AddFeatureToCollection($propCollection, $agfRW, $featId++, $geomBuffer);
                        $bufferFeatures++;
                    }
                    else
                    {
                        $inputGeometries->Add($geomDest);
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
                if($arbitraryMapSrs)
                    $measure = new MgCoordinateSystemMeasure($arbSrs);
                else
                    $measure = null;

                $geomFactory = new MgGeometryFactory();
                $geomBuffer = $geomFactory->CreateMultiGeometry($inputGeometries)->Buffer($distance, $measure);
                AddFeatureToCollection($propCollection, $agfRW, $featId, $geomBuffer);
                $bufferFeatures = 1;
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
            $warningFmt = $excludedLayers > 1 ? GetLocalizedString( "BUFFERREPORTWARNINGSINGULAR", $locale ) : GetLocalizedString( "BUFFERREPORTWARNINGPLURAL", $locale );
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

    if(isset($params['LOCALE']))
        $locale = $params['LOCALE'];

    $mapName = $params['MAPNAME'];
    $sessionId = $params['SESSION'];
    $popup = $params['POPUP'];
    $bufferName = $params['BUFFER'];
    $layersParam = $params['LAYERS'];
    $lcolor = $params['LCOLOR'];
    $ffcolor = $params['FFCOLOR'];
    $fbcolor = $params['FBCOLOR'];
    $foretrans = $params['FORETRANS'];
    $transparent = $params['TRANSPARENT'];
    $distance = $params['DISTANCE'];
    $units = $params['UNITS'];
    $linestyle = $params['LINESTYLE'];
    $fillstyle = $params['FILLSTYLE'];
    $thickness = $params['THICKNESS'];
    if(isset($params['MERGE']))
        $merge = 1;
    $selText = $params['SELECTION'];

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

?>

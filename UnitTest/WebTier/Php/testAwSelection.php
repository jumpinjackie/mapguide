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


require_once("../../../Web/src/PhpApi/Constants.php");
require_once("Utils.php");

interface FeatureClass
{
    function ClassDefinition();

    function InsertCommand($key);

    function ValidateFeature($key, $featureReader);

}

class IntKeyFeature implements FeatureClass
{
    function ClassDefinition()
    {
        $classDef = new MgClassDefinition();

        $classDef->SetName("IntKey");
        $classDef->SetDescription("Integer key feature class");
        $classDef->SetDefaultGeometryPropertyName("GEOM");

        // Set key property
        $prop = new MgDataPropertyDefinition("KEY");
        $prop->SetDataType(MgPropertyType::Int32);
        $prop->SetAutoGeneration(false);

        $classDef->GetIdentityProperties()->Add($prop);
        $classDef->GetProperties()->Add($prop);

        $prop = new MgDataPropertyDefinition("NAME");
        $prop->SetDataType(MgPropertyType::String);

        $classDef->GetProperties()->Add($prop);

        // Set geometry property
        $prop = new MgGeometricPropertyDefinition("GEOM");
        $prop->SetGeometryTypes(4);

        $classDef->GetProperties()->Add($prop);

        return $classDef;
    }

    function InsertCommand($key)
    {

        $props = new MgPropertyCollection();

        // key is autogen
        $keyProp = new MgInt32Property("KEY", $key);
        $props->Add($keyProp);

        $nameProp = new MgStringProperty("NAME", "IntKey".$key);
        $props->Add($nameProp);

        $wkt = new MgWktReaderWriter();
        $agf = new MgAgfReaderWriter();
        $geom = $wkt->Read("POLYGON ((20 20, 20 100, 120 100, 140 20, 20 20))");
        $geomReader = $agf->Write($geom);
        $geomProp = new MgGeometryProperty("GEOM", $geomReader);
        $props->Add($geomProp);

        $cmd = new MgInsertFeatures("IntKey", $props);
        return $cmd;
    }

    function ValidateFeature($key, $featureReader)
    {
    }
}

class StringKeyFeature implements FeatureClass
{
    function ClassDefinition()
    {
        $classDef = new MgClassDefinition();

        $classDef->SetName("StringKey");
        $classDef->SetDescription("String key feature class");
        $classDef->SetDefaultGeometryPropertyName("GEOM");

        // Set key property
        $prop = new MgDataPropertyDefinition("SKEY");
        $prop->SetDataType(MgPropertyType::String);
        $prop->SetAutoGeneration(false);

        $classDef->GetIdentityProperties()->Add($prop);
        $classDef->GetProperties()->Add($prop);

        $prop = new MgDataPropertyDefinition("NAME");
        $prop->SetDataType(MgPropertyType::String);

        $classDef->GetProperties()->Add($prop);

        // Set geometry property
        $prop = new MgGeometricPropertyDefinition("GEOM");
        $prop->SetGeometryTypes(4);

        $classDef->GetProperties()->Add($prop);

        return $classDef;
    }

    function InsertCommand($key)
    {

        $props = new MgPropertyCollection();

        $keyProp = new MgStringProperty("SKEY", $key);
        $props->Add($keyProp);

        $nameProp = new MgStringProperty("NAME", "StringKey".$key);
        $props->Add($nameProp);

        $wkt = new MgWktReaderWriter();
        $agf = new MgAgfReaderWriter();
        $geom = $wkt->Read("POLYGON ((20 20, 20 100, 120 100, 140 20, 20 20))");
        $geomReader = $agf->Write($geom);
        $geomProp = new MgGeometryProperty("GEOM", $geomReader);
        $props->Add($geomProp);

        $cmd = new MgInsertFeatures("StringKey", $props);
        return $cmd;
    }

    function ValidateFeature($key, $featureReader)
    {
    }
}




try
{

    Utils::MapAgentInit("../../../Web/src/webconfig.ini");
    $site = new MgSiteConnection();
    $cred = new MgUserInformation();
    $cred->SetMgUsernamePassword("Administrator","admin");
    $cred->SetLocale("en");
    $site->Open($cred);
    $fsvc = $site->CreateService(MgServiceType::FeatureService);
    $rsvc = $site->CreateService(MgServiceType::ResourceService);

    $wkt = "LOCALCS[\"Non-Earth (Meter)\",LOCAL_DATUM[\"Local Datum\",0],UNIT[\"Meter\", 1],AXIS[\"X\",EAST],AXIS[\"Y\",NORTH]]";
    $featureName = 'Library://TrevorWekel/NewSdf.FeatureSource';
    $id = new MgResourceIdentifier($featureName);

    $intFeature = new IntKeyFeature();
    $stringFeature = new StringKeyFeature();
    $schema = new MgFeatureSchema("TestSchema", "Temporary test schema");
    $schema->GetClasses()->Add($intFeature->ClassDefinition());

    $params = new MgCreateSdfParams("ArbitraryXY", $wkt, $schema);

    $src = new MgByteSource("NewSdf.MapDefinition");
    $rid = new MgResourceIdentifier("Library://TrevorWekel/NewSdf.MapDefinition");
    $rsvc->SetResource($rid, $src->GetReader(), null);

    $src = new MgByteSource("NewSdfInt.LayerDefinition");
    $rid = new MgResourceIdentifier("Library://TrevorWekel/NewSdfInt.LayerDefinition");
    $rsvc->SetResource($rid, $src->GetReader(), null);

    $src = new MgByteSource("NewSdfString.LayerDefinition");
    $rid = new MgResourceIdentifier("Library://TrevorWekel/NewSdfString.LayerDefinition");
    $rsvc->SetResource($rid, $src->GetReader(), null);

    echo "Deleting existing feature source\n";
    $rsvc->DeleteResource($id);

    echo "Creating new feature source\n";
    $fsvc->CreateFeatureSource($id, $params);


    // We need to add some data to the sdf before using it.  The spatial context
    // reader must have an extent.

    $cmdColl = new MgFeatureCommandCollection();

    for ($i = 1; $i<=20; $i++)
    {
        $insert = $intFeature->InsertCommand($i);
        $cmdColl->Add($insert);
    }


    echo "Updating features\n";
    $fsvc->UpdateFeatures($id, $cmdColl, false);

    $mapId = new MgResourceIdentifier("Library://TrevorWekel/NewSdf.MapDefinition");
    $map = new MgMap();
    $map->Create($rsvc, $mapId, "NewMap");

    echo "Building Selection from Add()\n";
    $sel = new MgSelection($map);

    $slayer = $map->GetLayers()->GetItem(0);
    $sel->AddFeatureIdInt32($slayer, "IntKey", 1);
    $sel->AddFeatureIdInt32($slayer, "IntKey", 10);
    $sel->AddFeatureIdInt32($slayer, "IntKey", 20);

    echo "XML FeatureSet is\n".$sel->ToXml()."\n";

    echo "\nString Filter: ".$sel->GenerateFilter($slayer, "StringKey")."\n\n";

    echo "Building Selection from XML\n";
    $sel2 = new MgSelection($map, $sel->ToXml());

    // Test basic methods
    $layerColl = $sel2->GetLayers();
    $newLayer = $layerColl->GetItem(0);
    echo "First layer selected is ".$newLayer->GetName()."\n";

    echo "BadKey Filter: ".$sel2->GenerateFilter($slayer, "BadKey")."\n";
    $filter = $sel2->GenerateFilter($slayer, "IntKey");
    echo "\nString Filter: ".$filter."\n\n";

    $query = new MgFeatureQueryOptions();
    $query->AddFeatureProperty("KEY");
    $query->AddFeatureProperty("NAME");
    $query->SetFilter($filter);

    echo "Selected features\n";
    $reader = $fsvc->SelectFeatures($id, "IntKey", $query);
    while ($reader->ReadNext() == true)
    {
        echo $reader->GetString("NAME")."\n";
    }

    echo "MgSelection from Reader\n";
    $reader = $fsvc->SelectFeatures($id, "IntKey", $query);
    $selection = new MgSelection($map);
    $layer1 = $map->GetLayers()->GetItem(0);
    $selection->AddFeatures($layer1, $reader, 0);
    echo $selection->ToXml();

    $envelope = $selection->GetExtents($fsvc);
    $ll = $envelope->GetLowerLeftCoordinate();
    $ur = $envelope->GetUpperRightCoordinate();
    echo "(".$ll->GetX().",".$ll->GetY().") - (".$ur->GetX().",".$ur->GetY().")\n";

}
catch (MgException $exc)
{
    echo $exc->GetExceptionMessage()."\n";
    echo $exc->GetDetails()."\n";
}
echo "Done.\n";
?>

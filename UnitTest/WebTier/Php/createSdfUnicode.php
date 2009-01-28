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


require_once("../../../Web/src/PhpApi/Constants.php");
require_once("Utils.php");

try
{

    Utils::MapAgentInit("../../../Web/src/webconfig.ini");
    $site = new MgSiteConnection();
    $cred = new MgUserInformation();
    $cred->SetMgUsernamePassword("Administrator","admin");
    $cred->SetLocale("en");
    $site->Open($cred);
    $svc = $site->CreateService(MgServiceType::FeatureService);
}
catch (MgException $exc)
{
    echo $exc->GetMessage()."\n";
    echo $exc->GetDetails()."\n";
    return;
}

echo "Created Services\n";

// Create class definition for new feature  class
$classDef = new MgClassDefinition();

$classDef->SetName("Buffer");
$classDef->SetDescription("Feature class for buffer layer");
$classDef->SetDefaultGeometryPropertyName("GEOM");

// Set key property
$prop = new MgDataPropertyDefinition("KEY");
$prop->SetDataType(MgPropertyType::Int32);
$prop->SetAutoGeneration(true);
$prop->SetReadOnly(true);

$classDef->GetIdentityProperties()->Add($prop);
$classDef->GetProperties()->Add($prop);

$prop = new MgDataPropertyDefinition("NAME");
$prop->SetDataType(MgPropertyType::String);

$classDef->GetProperties()->Add($prop);

// Set geometry property
$prop = new MgGeometricPropertyDefinition("GEOM");
//$prop->SetGeometryTypes(MgFeatureGeometricType::Surface);
$prop->SetGeometryTypes(MgFeatureGeometricType::Point);

$classDef->GetProperties()->Add($prop);
$schema = new MgFeatureSchema("BufferSchema", "Temporary buffer schema");
$schema->GetClasses()->Add($classDef);

$wkt = "LOCALCS[\"Non-Earth (Meter)\",LOCAL_DATUM[\"Local Datum\",0],UNIT[\"Meter\", 1],AXIS[\"X\",EAST],AXIS[\"Y\",NORTH]]";
$params = new MgCreateSdfParams("ArbitraryXY", $wkt, $schema);

echo "Created sdfparams\n";

$layerName = 'Library://TrevorWekel/NewSdf.FeatureSource';
$id = new MgResourceIdentifier($layerName);

try
{

    $svc->CreateFeatureSource($id, $params);
}
catch (MgException $exc)
{
    echo $exc->GetMessage()."\n";
    echo $exc->GetDetails()."\n";
    return;
}

echo "Created featuresource\n";

// We need to add some data to the sdf before using it.  The spatial context
// reader must have an extent.

$batchProp = new MgBatchPropertyCollection();
$wkt = new MgWktReaderWriter();
$agf = new MgAgfReaderWriter();
$fact = new MgGeometryFactory();

echo "Created wkt/agf\n";

$count = 10;
$i = 0;
for ($i=1; $i<=$count; $i++)
{

    $bufferProps = new MgPropertyCollection();
    $nameProp = new MgStringProperty("NAME", "Name_Eš??????Ab2??˜Ab2????_".$i);
    $bufferProps->Add($nameProp);

    $x = 120+$i/$count;
    $y = 100+$i/$count;
    //$wktText = "POLYGON ((20 20, 20 100, {$x} {$y}, 140 20, 20 20))";
    //$geom = $wkt->Read($wktText);
    $coord = $fact->CreateCoordinateXY($x, $y);
    $geom = $fact->CreatePoint($coord);
    $geomReader = $agf->Write($geom);
    $geomProp = new MgGeometryProperty("GEOM", $geomReader);
    $bufferProps->Add($geomProp);

    $batchProp->Add($bufferProps);
}

echo "Created geometries via wkt\n";

$cmd = new MgInsertFeatures("Buffer", $batchProp);
$cmdColl = new MgFeatureCommandCollection();

$cmdColl->Add($cmd);

try
{
    $svc->CreateFeatureSource($id, $params);
    $startTime = microtime(true);
    $props = $svc->UpdateFeatures($id, $cmdColl, false);
    $endTime = microtime(true);
    $diffTime = $endTime - $startTime;
    $reader = $props->GetItem(0)->GetValue();
    while ($reader->ReadNext() == true)
    {
        $key = $reader->GetInt32("KEY");
    }
    $reader->Close();

    // Now select a few of the feature and make sure we get everything back correctly.

    $query = new MgFeatureQueryOptions();
    $query->AddFeatureProperty("NAME");
    $query->AddFeatureProperty("KEY");
    $query->AddFeatureProperty("GEOM");
    $query->SetFilter("");

    $reader = $svc->SelectFeatures($id, "Buffer", $query);
    while ($reader->ReadNext() == true)
    {
        $geomText = '';
        $agfStream = $reader->GetGeometry("GEOM");
        $geom = $agf->Read($agfStream);
        $geomText = $wkt->Write($geom);
        echo $reader->GetInt32("KEY")." ".$reader->GetString("NAME")." ".$geomText."\n";
    }

    echo "Time for ".$i." ops was ".$diffTime. " seconds\n";
    echo "Average throughput of ".$i/$diffTime. " features per second\n";
}
catch (MgException $exc)
{
    echo "First failure on insert... caught...\n";
    echo $exc->GetMessage()."\n";
    echo $exc->GetDetails()."\n";
    echo $exc->GetStackTrace()."\n";
}


echo "Done.\n";
?>

<?php

//
//  Copyright (C) 2004-2014 by Autodesk, Inc.
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

//Command line script that executes all tests in the given database files

define('Run_MAIN_METHOD', true);
require_once("sqlite_constants.php");
require_once("Utils.php");
main();

function main()
{
    if (($_SERVER['argc'] >1) && ($_SERVER['argv'][1] == "-help"))
    {
        ShowUsage();
        return 1;
    }

    $updatesFailed = 0;
    $updatesFailed += UpdateDumpFile("../../TestData/ResourceService/ResourceServiceTest.db");
    $updatesFailed += UpdateDumpFile("../../TestData/DrawingService/DrawingServiceTest.db");
    $updatesFailed += UpdateDumpFile("../../TestData/FeatureService/FeatureServiceTest.db");
    $updatesFailed += UpdateDumpFile("../../TestData/SiteService/SiteServiceTest.db");
    $updatesFailed += UpdateDumpFile("../../TestData/MappingService/MappingServiceTest.db");
    $updatesFailed += UpdateDumpFile("../../TestData/ServerAdmin/ServerAdminTest.db");
    $updatesFailed += UpdateDumpFile("../../TestData/MapLayer/MapLayerTest.db");
    $updatesFailed += UpdateDumpFile("../../TestData/WebLayout/WebLayoutTest.db");
    $updatesFailed += UpdateDumpFile("../../TestData/Wfs/WfsTest.db");
    $updatesFailed += UpdateDumpFile("../../TestData/Wms/WmsTest.db");
    $updatesFailed += UpdateDumpFile("../../TestData/Unicode/UnicodeTest.db");
    
    exit($updatesFailed);
}

function UpdateDumpFile($dbName)
{
    echo "Checking $dbName\n";
    $ret = 0;
    $db = NULL;
    try {
        $dbPath = Utils::GetPath($dbName);
        $db = new SqliteDB();
        $db->Open($dbPath);
        Utils::CreateDumpFile($db, true);
    } catch (SqliteException $ex) {
        echo $ex->GetExceptionMessage()."\n";
        $ret = 1;
    }
    $db = NULL;
    return $ret;
}

function ShowUsage()
{
    printf("Usage: php UpdateDumpFiles.php\n");
}

?>

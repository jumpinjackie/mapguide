<?php

//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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
require_once("Run.php");
main();

function main()
{
    SetGlobals();
    $testsFailed = 0;
    $testsRun = 0;
    $isEnterprise = false;
    $file = fopen("UnitTests.log", "w");

    $str = sprintf("Run started: %s\n\n",date("D M j G:i:s T Y"));

    echo $str;
    fwrite($file, $str);

    if (($_SERVER['argc'] >1) && ($_SERVER['argv'][1] == "-help"))
    {
        ShowUsage();
    }

    if (($_SERVER['argc']>1) && ($_SERVER['argv'][1]=="/enterprise"))
    {
        $isEnterprise = true;
    }

    if (!$file)
    {
        echo "Unable to create log file UnitTests.log";
    }

    $testsFailed += ExecuteTest("Api", "../../TestData/ResourceService/ResourceServiceTest.dump", $testsRun, $file, $isEnterprise);
    $testsFailed += ExecuteTest("Http", "../../TestData/ResourceService/ResourceServiceTest.dump", $testsRun, $file, $isEnterprise);

    $testsFailed += ExecuteTest("Api", "../../TestData/DrawingService/DrawingServiceTest.dump", $testsRun, $file, $isEnterprise);
    $testsFailed += ExecuteTest("Http", "../../TestData/DrawingService/DrawingServiceTest.dump", $testsRun, $file, $isEnterprise);

    $testsFailed += ExecuteTest("Api", "../../TestData/FeatureService/FeatureServiceTest.dump", $testsRun, $file, $isEnterprise);
    $testsFailed += ExecuteTest("Http", "../../TestData/FeatureService/FeatureServiceTest.dump", $testsRun, $file, $isEnterprise);

    $testsFailed += ExecuteTest("Api", "../../TestData/SiteService/SiteServiceTest.dump", $testsRun, $file, $isEnterprise);
    $testsFailed += ExecuteTest("Http", "../../TestData/SiteService/SiteServiceTest.dump", $testsRun, $file, $isEnterprise);

    $testsFailed += ExecuteTest("Api", "../../TestData/MappingService/MappingServiceTest.dump", $testsRun, $file, $isEnterprise);
    $testsFailed += ExecuteTest("Http", "../../TestData/MappingService/MappingServiceTest.dump", $testsRun, $file, $isEnterprise);

    $testsFailed += ExecuteTest("Api", "../../TestData/ServerAdmin/ServerAdminTest.dump", $testsRun, $file, $isEnterprise);

    $testsFailed += ExecuteTest("Api", "../../TestData/MapLayer/MapLayerTest.dump", $testsRun, $file, $isEnterprise);

    $testsFailed += ExecuteTest("Api", "../../TestData/WebLayout/WebLayoutTest.dump", $testsRun, $file, $isEnterprise);

   // $testsFailed += ExecuteTest("Http", "../../TestData/Wfs/WfsTest.dump", $testsRun, $file, $isEnterprise);

   // $testsFailed += ExecuteTest("Http", "../../TestData/Wms/WmsTest.dump", $testsRun, $file, $isEnterprise);

    $testsFailed += ExecuteTest("Api", "../../TestData/Unicode/UnicodeTest.dump", $testsRun, $file, $isEnterprise);

    $str = sprintf("\n\nTests failed/run: %d/%d\n", $testsFailed, $testsRun);
    $str .=sprintf("Run ended: %s\n\n",date("D M j G:i:s T Y"));
    echo $str;
    fwrite($file, $str);

    if ($file)
    {
        fclose($file);
    }

    exit($testsFailed);
}

function ExecuteTest($requestType, $dumpFileName, &$testsRun, $file, $ent)
{
    try
    {
        $exitStatus=0;
        //Create database objects
        $dbPath = Utils::GetDbPath($dumpFileName);
        $dbName = Utils::GetPath($dbPath);

        if (file_exists($dbName))
        {
            $db = new SqliteDB();
            $db->Open($dbName);
            $vm = new SqliteVM($db);
            //Select all tests from the specified request type. Order tests using ExecuteSequence field
            if ($ent)
            {
                $rType = $requestType;
                $status = $vm->Execute("Select TestName, TestType from TestCase where TestType=\"$rType\" or TestType=\"$requestType\"order by ExecuteSequence");
            }
            else
            {
                $status = $vm->Execute("Select TestName, TestType from TestCase where TestType=\"$requestType\" order by ExecuteSequence");
            }

            while ($status==SQLITE_ROW)
            {
                $testName = $vm->GetString("TestName");
                $requestType = $vm->GetString("TestType");

                printf ("Executing %s test: %s\n", $requestType, $testName);
                //File in the $_POST array to simulate a GET request that is  normally send by the HTML forms
                $_POST['testName']=$testName;
                $_POST['requestType']=$requestType;
                $_POST[$testName.':dbPath']=$dbPath;

                $result = new Run($testsRun);
                $exitStatus += $result->main($testsRun, $file);
                $status = $vm->NextRow();
            }
            unset($vm);
            unset($db);
            return $exitStatus;
        }
    }
    catch (SqliteException $s)
    {
        print $s->GetExceptionMessage();
        return 1;
    }
}

function SetGlobals()
{
    //Sets global variables
    $_POST['output']="text";
    $_POST['testExecutionMode']="validate";
}

function ShowUsage()
{
    printf("Usage: php RunTests.php [-option]\n");
    printf ("\t-help Displays this help\n");
    printf ("\t/enterprise Runs unit tests for enterprise server(All feature providers)\n");
}

?>

<?php

//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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


//This class executes a test case based on input received from the HTML forms

require_once("ExecuteOperation.php");
require_once("Validate.php");
require_once("Result.php");
require_once("HtmlPrinter.php");
require_once("Utils.php");

if (!defined('Run_MAIN_METHOD'))
{
    define('Run_MAIN_METHOD', 'Run::main');
    $test = new Run();
    $testsRun = 0;
    $test->main($testsRun);
}

class Run
{
    //Define any private data members here
    private $unitTestVm;
    private $unitTestResultVm;
    private $apiCall;
    private $unitTestDb;

    private $validate;
    private $operation;

    public function main(&$testsRun, $file=null)
    {
        $status=-1;
        //Print HTML tags for the result page
        if ($_POST['output']=="html")
        {
            header('Content-type: text/html; charset=UTF-8');
            HtmlPrinter::PrintHtmlHeader("Test Results");
            HtmlPrinter::PrintFormHeader("Run.php");
        }

        //Open the file here if the file resource is null.
        //This is in the case a test is executed from HTML form.
        if (null == $file)
        {
            $file=fopen("UnitTests.log", "w");
        }
        //If initialization is successful then run the test and clean up
        if (self::Initialize())
        {
            $status = self::RunTest($testsRun, $file);
            self::CleanUp();
        }

        //Close HTML tags for the output
        if ($_POST['output']=="html")
        {
            HtmlPrinter::PrintHtmlFooter();
        }
        return $status;
    }


    function Initialize()
    {
        try
        {
            //Retrieves data from the $_POST array.
            //Initializes objects that will required during the test execution
            if (!isset($_SERVER['SERVER_ADDR']))
            {
                $url="http://localhost/mapguide/mapagent/mapagent.fcgi";
            }
            else
            {
                $url="http://".$_SERVER['SERVER_ADDR'].":".$_SERVER['SERVER_PORT']."/mapguide/mapagent/mapagent.fcgi";

            }

            $testName=$_POST['testName'];
            $dbName = Utils::GetPath($_POST[$testName.":dbPath"]);
            if (file_exists($dbName))
            {
                $status = true;

                $this->unitTestDb = new SqliteDB();
                $this->unitTestDb->Open("$dbName");

                $this->unitTestVm = new SqliteVM($this->unitTestDb);
                $this->unitTestResultVm = new SqliteVM($this->unitTestDb);

                $this->operation = new ExecuteOperation($_POST['requestType'], $this->unitTestDb, $url);
                $this->validate = new Validate($this->unitTestDb);
            }
            else
            {
                //TODO: What to do when the database is missing
                print("Cannot find database. Where did you put my database: ".$dbName."?");
                $status=false;
            }
            return $status;
        }
        catch (MgException $e)
        {
            print $e->GetMessage('en');
            return false;
        }
        catch (SqliteException $s)
        {
            print $s->GetMessage();
            return false;
        }
    }

    //Deletes all objects after the test execution
    function CleanUp()
    {
        try
        {
            unset($this->operation);
            unset($this->validate);
            $this->unitTestResultVm = null;
            $this->unitTestVm = null;
            $this->unitTestDb = null;
        }
        catch (MgException $e)
        {
            print $e->GetMessage('en');
        }
        catch (SqliteException $s)
        {
            print $s->GetMessage();
        }
    }

    //Runs a test
    function RunTest(&$testsRun, $file)
    {
        try
        {
            $actualResult=new Result();
            $exitStatus=0;
            $testName=$_POST['testName'];

            //Add the test in the log file
            $entry="\n\n************************************************************\n";
            $entry .= sprintf("Executing %s\n", $testName);
            $entry.="************************************************************\n\n";

            fwrite($file, $entry);

            //Get the list of operations to be executed for this test. This list should comma separated, no spaces.
            $this->unitTestVm->Execute("Select ParamSets from TestCase where TestName=\"$testName\"");
            $sets = $this->unitTestVm->GetString("ParamSets");

            $paramSet = strtok($sets, ",");


            if ($_POST['output']=="html")
            {
                //Start the HTML table for the results
                HtmlPrinter::PrintResultTableHeader($_POST['requestType']." Test ".$testName);
            }

            //For each operation in the list
            //Execute the operation and validate the result
            while ($paramSet != "")
            {
                $this->unitTestVm->Execute("Select ParamValue from Params where ParamSet=$paramSet AND ParamName=\"OPERATION\";");
                $paramValue = $this->unitTestVm->GetString("ParamValue");

                $testsRun++;

                //Add the operation to the log file
                $this->AddLogFileEntry($file, $paramValue, $paramSet, $this->unitTestResultVm);

                $actualResult = $this->operation->Execute($paramSet, $paramValue);

                if (substr_count($_POST['requestType'],"Http"))
                {
                    $status = $this->validate->ValidateHttpRequest($_POST['testName'], $paramSet, $paramValue, $actualResult, $file);
                }
                else
                {
                    $status = $this->validate->ValidateApiRequest($_POST['testName'], $paramSet, $paramValue, $actualResult, $file);
                }

                $exitStatus += $status;

                $paramSet = strtok(",");
            }

            //Closes the Html table. Also outputs hidden fields that are needed for Generation mode
            if ($_POST['output']=="html")
            {
                HtmlPrinter::printTableEnd();
                HtmlPrinter::PrintGenerateFormHiddenFields();
                HtmlPrinter::PrintFormFooter($_POST['requestType'], "Generate");

            }

            if ($_POST['testExecutionMode'] == 'generate')
            {
                Utils::CreateDumpFile($this->unitTestDb);
            }

            return $exitStatus;
        }
        catch (MgException $e)
        {
            print $e->GetMessage("en")."\n";
        }
        catch (SqliteException $s)
        {
            print $s->GetMessage()."\n";
        }
    }

    function AddLogFileEntry($file, $operation, $paramSet, $vm)
    {
        fwrite($file, "\nParamSet: ".$paramSet."\n");

        $status = $vm->Execute("Select * from Params where ParamSet=$paramSet");

        while ($status==SQLITE_ROW)
        {
            $paramName = $vm->GetString("ParamName");
            $paramValue = $vm->GetString("ParamValue");
            $entry = sprintf("%s: %s\n", $paramName, $paramValue);
            fwrite($file, $entry);
            $status = $vm->NextRow();
        }
    }
}
?>

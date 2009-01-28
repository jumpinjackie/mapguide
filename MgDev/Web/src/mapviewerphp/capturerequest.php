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

class CaptureRequest
{
    private $db;
    private $vm;
    private $paramSet;
    private $testCase;
    private $operationsSet;
    private $dbDataPath;
    private $fileDataPath;

    function __construct($testCase, $dbFileName, $fileDataPath, $dbDataPath)
    {
        $this->fileDataPath=$fileDataPath;
        $this->dbDataPath=$dbDataPath;
        $this->testCase = $testCase;
        $dbName = $this->fileDataPath.$dbFileName;
        $this->db = new SqliteDB;

        //If database does not exist, create the file and the tables
        if (!file_exists($dbName))
        {
            $this->db->Open($dbName);
            $this->vm = new SqliteVM($this->db, true);
            $this->vm->Execute("CREATE TABLE Params(ParamSet INTEGER, ParamName TEXT, ParamValue TEXT)");
            $this->vm->Execute("CREATE TABLE ApiTestResults(Description TEXT, ParamSet INTEGER, ContentType TEXT, Result TEXT)");
            $this->vm->Execute("CREATE TABLE HttpTestResults(Description TEXT, ParamSet INTEGER, ContentType TEXT, Result TEXT)");
            $this->vm->Execute("CREATE TABLE TestCase(TestName TEXT, ParamSets TEXT, Description TEXT, TestType TEXT, Prerequisite TEXT)");
        }
        else
        {
            $this->db->Open($dbName);
            $this->vm = new SqliteVM($this->db, true);
        }
        //Find the biggest paramSet
        if ($this->vm->Execute("SELECT DISTINCT ParamSet FROM Params ORDER BY ParamSet DESC") == SQLITE_ROW)
        {
            $this->paramSet = $this->vm->GetInt("ParamSet")+1;
        }
        else
        {
            $this->paramSet=0;
        }
        //Check if we have already operations for that test case
        if ($this->vm->Execute("SELECT ParamSets from TestCase WHERE TestName=\"$this->testCase\"") == SQLITE_ROW)
        {
            $this->operationsSet = $this->vm->GetString("ParamSets");
        }
        else
        {

            $currentDate=date("D M j G:i:s T Y");
            $this->vm->Execute("INSERT INTO TestCase(TestName, Description, TestType) VALUES(\"$this->testCase\", \"Test case: $this->testCase created $currentDate\", \"Http\")");
            $this->operationsSet="";
        }
    }

    function __destruct()
    {
        unset($this->vm);
        unset($this->db);
    }

    function SaveRequest()
    {
        $reqParam="";
        $this->operationsSet.=$this->paramSet.",";
        if($_SERVER['REQUEST_METHOD'] == "GET")
        {
            self::CaptureParameters($_GET);
        }
        else //POST
        {
            self::CaptureParameters($_POST);
            self::CaptureFileParameters();
        }
        $this->paramSet++;
    }

    // Capture request parameters from GET or POST operation
    function CaptureParameters($args)
    {
        try
        {
            $testDescription=$this->testCase."_".$args['OPERATION'];
            $this->vm->Execute("INSERT INTO ApiTestResults(Description, ParamSet) VALUES(\"$testDescription\", $this->paramSet)");
            $this->vm->Execute("INSERT INTO HttpTestResults(Description, ParamSet) VALUES(\"$testDescription\", $this->paramSet)");

            $this->vm->Execute("UPDATE TestCase SET ParamSets=\"$this->operationsSet\" WHERE TestName=\"$this->testCase\"");

            foreach ($args as $k => $v)
            {
                //TODO: Update row entry for test case
                $status = $this->vm->Execute("INSERT INTO Params VALUES($this->paramSet, \"$k\", \"$v\");");
            }
        }
        catch (SqliteException $s)
        {
            print $s->GetMessage('eng');
        }
    }

    // Get any file parameters that exist from POST operation
    function CaptureFileParameters()
    {
        if($_SERVER['REQUEST_METHOD'] == "POST")
        {
            foreach ($_FILES as $k => $v)
            {
                //Create a directory if it does not exist
                if (!file_exists($this->fileDataPath))
                {
                    //For this to work you have to set appropriate persmissions in the folder. Note, IIS is not using you Windows login name for authentication
                    mkdir($this->fileDataPath);
                }

                //Determine the file name
                if ( $_FILES[$k]['name']=="CONTENT" || $_FILES[$k]['name']=="HEADER" || $_FILES[$k]['name']=="DATA")
                {
                    $file = self::GetFileName($k);
                }
                else
                {
                    $file=$_FILES[$k]['name'];
                }

                //Check for duplicate files
                if (file_exists($this->fileDataPath.$file))
                {
                    //Generates unique temporary file
                    $temp=tempnam($this->fileDataPath, "");

                    $start = strrpos($temp, "\\")+1;
                    $length = strpos($temp, ".")-$start;
                    $file = substr($temp, $start, $length)."_".$file;
                    unlink($temp);
                }

                copy($_FILES[$k]['tmp_name'], $this->fileDataPath.$file);

                $this->vm->Execute("INSERT INTO Params VALUES($this->paramSet, \"$k\", \"$this->dbDataPath$file\");");

                //printf("K is: %s \n\nV is : %s", $k, $v);
                foreach ($_FILES[$k] as $k1 => $v1)
                {
                    printf ("<br><br>K is: %s<br><br>K1 is: %s<br><br>V1 is: %s<br><br>", $k, $k1, $v1);
                }
            }
        }
    }

    function GetFileName($fileName)
    {
        if ($this->vm->Execute("Select ParamValue from Params where ParamSet=$this->paramSet and ParamName=\"DATANAME\"")==SQLITE_ROW)
        {
            $file=$this->vm->GetString("ParamValue");
        }
        else
        {
            $this->vm->Execute("Select ParamValue from Params where ParamSet=$this->paramSet and ParamName=\"RESOURCEID\"");
            $resourceId = $this->vm->GetString("ParamValue");
            $file = $fileName."_".substr($resourceId, strrpos($resourceId, "/")+1);
        }
        return $file;
    }
}
?>

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


//Class that performs the validation for each operation

require_once("Result.php");
require_once("HtmlPrinter.php");
require_once("ValidateUtils.php");

class Validate
{
    private $vm;

    public function __construct($db)
    {
        $this->vm = new SqliteVM($db, true);
    }

    public function __destruct()
    {
        unset($this->vm);
    }

    public function ValidateApiRequest($serviceType, $paramSet, $operation, $actualResult, $file)
    {
        //Default values for the outcome
        $exitStatus=0;
        $outcome="pass";

        //If we have an exception we need to remove the stack trace because different line numbers will fail the test
        $resultData = ValidateUtils::RemoveStackTrace($actualResult->GetResultData());

        //Get the mime type based on the content type in the result
        $mimeType = $actualResult->GetContentType();

        //If we have exception message we need to remove any parts that may contain system dependent information
        //Ex. file paths
        $resultData = ValidateUtils::ProcessExceptionMessage($resultData);

        //Get the file extension that will be used for a dump
        $actualExtension = ValidateUtils::GetExtension($mimeType);

        //If we have an ALWAYSPASS parameter defined for the operation then skip the whole validation process
        //This parameter should only be used for clean up operations that are no related with the tests
        if ($this->vm->Execute("Select ParamValue from Params where ParamName=\"ALWAYSPASS\" and ParamSet=$paramSet")!=SQLITE_ROW)
        {
            //Form a file name that is going to be used to write results to a file
            $type=substr($serviceType, 0, strpos($serviceType, "_"));
            $filePath = Utils::GetPath("../../TestData/".$type."/DumpFiles/".$type."ApiTest");
            $fileName = $filePath."_".$paramSet.".".$actualExtension;

            if ($_POST['testExecutionMode'] == "dump")
            {
                file_put_contents($fileName, $resultData);
            }
            else
            {
                //This section is special case handling for the operations that return different data after each call
                $resultData=ValidateUtils::SpecialDataHandling($operation, $resultData, $mimeType);

                if ($_POST['testExecutionMode'] == "generate" && array_key_exists($paramSet, $_POST))
                {
                    //Get the sample result that is stored in the database. If we are using file on disk for validation
                    //then do not overwrite the filename in the database
                    //To distinguish between sample data and filename all filenames should be prefixed with "@@"
                    $status = $this->vm->Execute("Select Result from ApiTestResults where ParamSet=$paramSet;");
                    $sampleResult = $this->vm->GetString("Result");

                    if ("@@"!=substr($sampleResult, 0, 2))
                    {
                        $responseBody="";
                        //Insert the sample data as a BLOB
                        //Update the row for that param set or create a new row if we do not have it yet
                        if ($status == SQLITE_ROW)
                        {
                            $this->vm->Prepare("update ApiTestResults set Result = :blob where ParamSet=$paramSet;");
                        }
                        else
                        {
                            printf ("<b>A new row has been created in ApiTestResults table to store the result for operation %s</b><br />", $paramSet);
                            print ("<b>Please, update the Description field for that row later</b><br /><br />");
                            $this->vm->Prepare("INSERT INTO ApiTestResults(ParamSet, Result) VALUES($paramSet, :blob);");
                        }
                        $this->vm->BindBlob(":blob", $resultData, mb_strlen($resultData, "latin1"));
                        $this->vm->Execute();

                        //Update the mime type in the database if available
                        if ($mimeType != null)
                        {
                            $this->vm->Execute("UPDATE ApiTestResults SET ContentType=\"$mimeType\" WHERE ParamSet=$paramSet;");
                        }
                        //To ensure that the data generated in the database is correct, output the result in a file
                        //Check the file contents to ensure that this is the data that you to be stored in the database
                        file_put_contents($fileName, $resultData);
                    }
                }
                elseif ($_POST['testExecutionMode'] == "validate" || $_POST['testExecutionMode'] == "show")
                {
                    $expectedResult="";
                    $resultContent="";

                    //Get the sample result and the expected content type from the database
                    $this->vm->Execute("Select Result, ContentType from ApiTestResults where ParamSet=$paramSet");
                    $byteReader = $this->vm->GetBlob("Result");
                    $expectedExtension = ValidateUtils::GetExtension($this->vm->GetString("ContentType"));

                    while ($byteReader->Read($resultContent, 1024)>0)
                    {
                        $expectedResult.=$resultContent;
                    }

                    //If we are validating from a file then get the contents of that file
                    //File names should be prefixed with "@@" to distinguish them from BLOB data
                    if ("@@"==substr($expectedResult, 0, 2))
                    {
                        //Remove "@@" from the filename. Get the path to the file and pump the contents in a variable
                        $sampleDataFile=substr($expectedResult, 2);
                        $sampleDataFile=Utils::GetPath($sampleDataFile);
                        $expectedResult=file_get_contents($sampleDataFile);
                    }

                    if ($_POST['testExecutionMode'] == "validate")
                    {
                        //If the results are different and special validation fails then the operation failed ->mark it red
                        if (strncasecmp($resultData, $expectedResult, strlen($resultData . $expectedResult)) && !(ValidateUtils::SpecialValidation($operation, $resultData, $expectedResult)))
                        {
                            $outcome="fail";
                            $exitStatus=1;

                            if (($expectedExtension!="xml") && ($expectedExtension!="html") && ($expectedExtension!="txt"))
                            {
                                $expectedResult= "Unable to display binary data";
                            }

                            if (($actualExtension!="xml") && ($actualExtension!="html") && ($actualExtension!="txt"))
                            {
                                $resultData="Unable to display binary data";
                            }
                        }

                    }
                    else
                    {
                        $type=substr($serviceType, 0, strpos($serviceType, "_"));
                        $showPath = Utils::GetPath("../../TestData/".$type."/ShowFiles/".$type."ApiTest");
                        $showName = $showPath."_".$paramSet.".".$actualExtension;
                        file_put_contents($showName, $expectedResult);
                    }
                }
            }
        }

        if ($_POST['output']=="html")
        {
            if ($outcome=="fail")
            {
                HtmlPrinter::AddResultRow($operation, $outcome, $paramSet, $resultData, $expectedResult);
                $str = sprintf("\n****ACTUAL RESULT****\n%s\n****EXPECTED RESULT****\n%s\n********\n\n\n", $resultData, $expectedResult);
                fwrite($file, $str);
            }
            else
            {
                HtmlPrinter::AddResultRow($operation, $outcome, $paramSet);
            }
        }
        else
        {
            if ($outcome=="fail")
            {
                printf("****".$serviceType." ".$paramSet." ".$operation." failed.\n");
                $str = sprintf("\n****ACTUAL RESULT****\n%s\n****EXPECTED RESULT****\n%s\n********\n\n\n", $resultData, $expectedResult);
                echo $str;
                fwrite($file, $str);
            }
        }
        return $exitStatus;
    }

    public function ValidateHttpRequest($serviceType, $paramSet, $operation, $actualResult, $file)
    {
        //Default values for outcome
        $exitStatus=0;
        $outcome="pass";

        //Get the content type of the result
        $contentType = $actualResult->GetContentType();
        //We need to remove the stack trace if an exception is thrown because the different line numbers may fail the test
        $resultData = ValidateUtils::RemoveStackTrace($actualResult->GetResultData());

        //If we have exception message we need to remove any parts that may contain system dependent information
        //Ex. file paths
        $resultData = ValidateUtils::ProcessExceptionMessage($resultData);

        //Get the extension for the dump file based on the content type of the result
        $actualExtension = ValidateUtils::GetExtension($contentType);

        //If ALWAYSPASS parameter is defined the the whole validation is skipped.
        //This parameter should only be used for clean up operations that are no related to the test
        if ($this->vm->Execute("Select ParamValue from Params where ParamName=\"ALWAYSPASS\" and ParamSet=$paramSet")!=SQLITE_ROW)
        {
            //Form a file name that is going to be used for the outputting results to a file
            $type=substr($serviceType, 0, strpos($serviceType, "_"));
            $filePath = Utils::GetPath("../../TestData/".$type."/DumpFiles/".$type."HttpTest");
            $fileName = $filePath."_".$paramSet.".".$actualExtension;

            if ($_POST['testExecutionMode'] == "dump")
            {
                file_put_contents($fileName, $resultData);
            }
            else
            {
                //This section is special case handling for the operations that return different data after each call
                $resultData=ValidateUtils::SpecialDataHandling($operation, $resultData, $contentType);

                if ($_POST['testExecutionMode'] == "generate" && array_key_exists($paramSet,$_POST))
                {
                    //Get the sample result that is stored in the database. If we are using file on disk for validation
                    //then do not overwrite the filename in the database
                    //To distinguish between sample data and filename all filenames should be prefixed with "@@"
                    $status = $this->vm->Execute("Select Result from HttpTestResults where ParamSet=$paramSet;");
                    $sampleResult = $this->vm->GetString("Result");

                    if ("@@"!=substr($sampleResult, 0, 2))
                    {
                        $responseBody="";

                        //Store the result as a BLOB in the database
                        //Update the row for that param set or create a new row if we do not have it yet
                        if ($status == SQLITE_ROW)
                        {
                            $this->vm->Prepare("update HttpTestResults set Result = :blob where ParamSet=$paramSet;");
                        }
                        else
                        {
                            printf ("<b>A new row has been created in HttpTestResults table to store the result for operation %s</b><br />", $paramSet);
                            print ("<b>Please, update the Description field for that row later</b><br /><br />");
                            $this->vm->Prepare("INSERT INTO HttpTestResults(ParamSet, Result) VALUES($paramSet, :blob);");
                        }
                        $this->vm->BindBlob(":blob", $resultData, strlen($resultData));
                        $this->vm->Execute();

                        if ($contentType != null)
                        {
                            $this->vm->Execute("UPDATE HttpTestResults SET ContentType=\"$contentType\" WHERE ParamSet=$paramSet;");
                        }
                        //Write the result to a file to ensure the correct data is populated in the database
                        file_put_contents($fileName, $resultData);
                    }
                }
                elseif ($_POST['testExecutionMode'] == "validate" || $_POST['testExecutionMode'] == "show")
                {
                    $expectedResult="";
                    $resultContent="";

                    //Get the sample data from the database
                    $this->vm->Execute("Select Result, ContentType from HttpTestResults where ParamSet=$paramSet");
                    $byteReader = $this->vm->GetBlob("Result");
                    $expectedExtension = ValidateUtils::GetExtension($this->vm->GetString("ContentType"));

                    while ($byteReader->Read($resultContent, 1024)>0)
                    {
                        $expectedResult.=$resultContent;
                    }

                    //If we are validating from a file then get the contents of that file
                    //File names should be prefixed with "@@" to distinguish them from BLOB data
                    if ("@@"==substr($expectedResult, 0, 2))
                    {
                        //Remove "@@" from the filename. Get the path to the file and pump the contents in a variable
                        $sampleDataFile=substr($expectedResult, 2);
                        $sampleDataFile=Utils::GetPath($expectedResult);
                        $expectedResult=file_get_contents($sampleDataFile);
                    }


                    if ($_POST['testExecutionMode'] == "validate")
                    {

                        //If the results are different and special validation fails then then the operation failed ->mark it red
                        if ( strncasecmp($resultData, $expectedResult, strlen($resultData.$expectedResult)) && !(ValidateUtils::SpecialValidation($operation, $resultData, $expectedResult)))
                        {
                            $outcome="fail";
                            $exitStatus=1;

                            if (($expectedExtension!="xml") && ($expectedExtension!="html") && ($expectedExtension!="txt"))
                            {
                                $expectedResult= "Unable to display binary data";
                            }

                            if (($actualExtension!="xml") && ($actualExtension!="html") && ($actualExtension!="txt"))
                            {
                                $resultData="Unable to display binary data";
                            }
                        }
                    }
                    else
                    {
                        $type=substr($serviceType, 0, strpos($serviceType, "_"));
                        $showPath = Utils::GetPath("../../TestData/".$type."/ShowFiles/".$type."HttpTest");
                        $showName = $showPath."_".$paramSet.".".$actualExtension;
                        file_put_contents($showName, $expectedResult);
                    }
                }
            }
        }

        if ($_POST['output']=="html")
        {
            if ($outcome=="fail")
            {
                HtmlPrinter::AddResultRow($operation, $outcome, $paramSet, $resultData, $expectedResult);
                $str = sprintf("\n****ACTUAL RESULT****\n%s\n****EXPECTED RESULT****\n%s\n********\n\n\n", $resultData, $expectedResult);
                fwrite($file, $str);
            }
            else
            {
                HtmlPrinter::AddResultRow($operation, $outcome, $paramSet);
            }
        }
        else
        {
            if ($outcome=="fail")
            {
                printf("****".$serviceType." ".$paramSet." ".$operation." failed.\n");
                $str = sprintf("\n****ACTUAL RESULT****\n%s\n****EXPECTED RESULT****\n%s\n********\n\n\n", $resultData, $expectedResult);
                echo $str;
                fwrite($file, $str);
            }
        }
        return $exitStatus;
    }
}
?>

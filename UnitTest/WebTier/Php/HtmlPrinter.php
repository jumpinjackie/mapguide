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

require_once("Utils.php");

//This is a helper class that contains methods that output data in an Html format.

class HtmlPrinter
{
    public function PrintHtmlHeader($title)
    {
        print ("<html>\n");
        print ("<head>\n");
        print ("<title>$title</title>\n");
        print ("</head>\n");
        print ("<body>\n");
        print("<h2>$title</h2>\n");
    }

    public function PrintTestCases($testSuite, $dumpFilePath, $testType)
    {
        try
        {
            self::PrintTestTableHeader($testSuite, "Test Case" , "Description", "Prerequisite");

            $dbPath = Utils::GetDbPath($dumpFilePath);
            $dbName = Utils::GetPath($dbPath);
            if (file_exists($dbName))
            {
                $db = new SqliteDB();
                $db->Open($dbName);
                $vm = new SqliteVM($db, true);


                $status = $vm->Execute("Select * from TestCase WHERE TestType=\"$testType\" Order by ExecuteSequence;");
                while ($status == SQLITE_ROW)
                {
                    $testName = $vm->GetString("TestName");
                    $description = $vm->GetString("Description");
                    $prerequisite = $vm->GetString("Prerequisite");
                    print("<tr>\n");
                    print("<td><input type=\"radio\" name=\"testName\" value=\"$testName\"></td>\n");
                    printf("<td>%s</td>\n<td>%s</td><td>%s</td>\n", $testName, $description, $prerequisite);
                    print("<td><input type=\"hidden\" name=\"$testName:dbPath\" value=\"$dbPath\"></td>\n");
                    print("</tr>\n");
                    $status = $vm->NextRow();
                }
                unset($vm);
                unset($db);
            }
        }
        catch (SqliteException $s)
        {
            print $s->GetMessage();
        }
        self::printTableEnd();
    }

    public function PrintTestTableHeader($caption="", $c1="", $c2="", $c3="")
    {
        print("<table border=\"1\" cellpadding=\"5\">\n");
        print("<h3><b>$caption</b></h3>");
        print("<tr>\n");
        printf("<td>&nbsp</td>\n<th>%s</th>\n<th>%s</th><th>%s</th>\n", $c1, $c2, $c3);
        print("</tr>\n");
    }

    public function printTableEnd()
    {
        print("</table><br />\n");
    }

    public function PrintSelectTestMode()
    {
        print("<h3>Select Test Mode</h3>\n");
        print("<p>\n");
        print("<select name=\"testExecutionMode\">\n");
        print("<option value=\"validate\">Validate\n");
        print("<option value=\"dump\">Dump\n");
        print("</select>\n");
        print("</p>\n");
    }

    public function PrintSelectOutput()
    {
        print("<h3>Select Output</h3>\n");
        print("<p>\n");
        print("<select name=\"output\">\n");
        print("<option value=\"html\">Html\n");
        print("<option value=\"text\">Text\n");
        print("</select>\n");
        print("</p>\n");
    }

    public function PrintHtmlFooter()
    {
        print ("</body>\n");
        print ("</html>\n");
    }

    public function PrintFormHeader($target)
    {
        print ("<form name=\"input\" action=\"$target\" method=\"POST\" id=\"myform\">\n");
    }

    public function PrintFormFooter($requestType, $action)
    {
        print("<td><input type=\"hidden\" name=\"requestType\" value=\"$requestType\"></td>\n");
        print ("<input type=\"submit\" value=\"$action\" ID=\"$action\" NAME=\"$action\"> <input type=\"reset\" ID=\"Reset1\" NAME=\"Reset1\">\n");
        print ("</form>\n");
    }

    public function AddResultRow($operation, $outcome, $paramSet, $actualResult="", $expectedResult="")
    {
        $checkBox="<input type=\"checkbox\" name=$paramSet>";
        if ( $outcome=="fail")
        {
            print("<tr bgcolor=\"tomato\">\n");
        }
        else
        {
            print ("<tr bgcolor=\"lightgreen\">\n");
        }
        printf("<td>%s</td><td>%s</td><td>%s</td>\n<td>%s</td>\n<td>%s</td>\n<td>%s</td>\n", $checkBox, $paramSet, $operation, $outcome, $actualResult, $expectedResult);
        print("</tr>");
    }

    public function PrintResultTableHeader($caption="")
    {
        print("<table scroll=\"no\" border=\"1\" cellpadding=\"5\">\n");
        print("<h3><b>$caption</b></h3>");
        print("<tr>\n");
        print("<td>&nbsp</td><th>Param Set</th><th>Operation</th>\n<th>Outcome</th>\n<th>Actual Result</th>\n<th>Expected Result</th>\n");
        print("</tr>\n");
    }

    public function PrintGenerateFormHiddenFields()
    {
        $output = $_POST['output'];
        $testName = $_POST['testName'];
        $dbPath =$_POST[$testName.":dbPath"];

        print("<td><input type=\"hidden\" name=\"output\" value=\"$output\"></td>\n");
        print("<td><input type=\"hidden\" name=\"testExecutionMode\" value=\"generate\"></td>\n");
        print("<td><input type=\"hidden\" name=\"testName\" value=\"$testName\"></td>\n");
        print("<td><input type=\"hidden\" name=\"$testName:dbPath\" value=\"$dbPath\"></td>\n");
    }
}

?>

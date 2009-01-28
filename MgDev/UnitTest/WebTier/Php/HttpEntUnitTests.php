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

//This script generates the HTML form for Api tests

require_once("HtmlPrinter.php");

if (!defined('HttpUnitTests_MAIN_METHOD'))
{

    define('HttpUnitTests_MAIN_METHOD', 'HttpUnitTests::main');
    $test = new HttpUnitTests();
    $test->main();
}

class HttpUnitTests
{
    public function main()
    {
        HtmlPrinter::PrintHtmlHeader("HttpEnt Tests");
        HtmlPrinter::PrintFormHeader("Run.php");

        //Add your tests here
        //HtmlPrinter::PrintTestCases("YourTestName", "Path to database file where your data is stored", "Http")
        HtmlPrinter::PrintTestCases("FeatureService", "../../TestData/FeatureService/FeatureServiceTest.dump", "HttpEnt");

        HtmlPrinter::PrintSelectTestMode();
        HtmlPrinter::PrintSelectOutput();
        HtmlPrinter::PrintFormFooter("HttpEnt", "RunTests");
        HtmlPrinter::PrintHtmlFooter();
    }
}
?>

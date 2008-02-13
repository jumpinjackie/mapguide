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

require_once("HtmlPrinter.php");
//This script generates the HTML form for Api tests

if (!defined('ApiUnitTests_MAIN_METHOD'))
{
    define('ApiUnitTests_MAIN_METHOD', 'ApiUnitTests::main');
    $test = new ApiUnitTests();
    $test->main();
}

class ApiUnitTests
{
    public function main()
    {
        HtmlPrinter::PrintHtmlHeader("Api Tests");
        HtmlPrinter::PrintFormHeader("Run.php");

        //Add your tests here
        //HtmlPrinter::PrintTestCases("YourTestName", "Path to database file where your data is stored", "Api")
        HtmlPrinter::PrintTestCases("ResourceService", "../../TestData/ResourceService/ResourceServiceTest.dump", "Api");
        HtmlPrinter::PrintTestCases("DrawingService", "../../TestData/DrawingService/DrawingServiceTest.dump", "Api");
        HtmlPrinter::PrintTestCases("FeatureService", "../../TestData/FeatureService/FeatureServiceTest.dump", "Api");
        HtmlPrinter::PrintTestCases("MappingService", "../../TestData/MappingService/MappingServiceTest.dump", "Api");
        HtmlPrinter::PrintTestCases("SiteServiceTests", "../../TestData/SiteService/SiteServiceTest.dump", "Api");
        HtmlPrinter::PrintTestCases("ServerAdminTests", "../../TestData/ServerAdmin/ServerAdminTest.dump", "Api");
        HtmlPrinter::PrintTestCases("MapLayerTests", "../../TestData/MapLayer/MapLayerTest.dump", "Api");
        HtmlPrinter::PrintTestCases("WebLayoutTests", "../../TestData/WebLayout/WebLayoutTest.dump", "Api");

        HtmlPrinter::PrintTestCases("UnicodeTests", "../../TestData/Unicode/UnicodeTest.dump", "Api");

        HtmlPrinter::PrintSelectTestMode();
        HtmlPrinter::PrintSelectOutput();
        HtmlPrinter::PrintFormFooter("Api", "RunTests");
        HtmlPrinter::PrintHtmlFooter();
    }
}
?>

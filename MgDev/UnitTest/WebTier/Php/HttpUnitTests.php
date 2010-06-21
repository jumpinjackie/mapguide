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
        HtmlPrinter::PrintHtmlHeader("Http Tests");
        HtmlPrinter::PrintFormHeader("Run.php");

        //Add your tests here
        //HtmlPrinter::PrintTestCases("YourTestName", "Path to database file where your data is stored", "Api")
        HtmlPrinter::PrintTestCases("ResourceService", "../../TestData/ResourceService/ResourceServiceTest.dump", "Http");
        HtmlPrinter::PrintTestCases("DrawingService", "../../TestData/DrawingService/DrawingServiceTest.dump", "Http");
        HtmlPrinter::PrintTestCases("FeatureService", "../../TestData/FeatureService/FeatureServiceTest.dump", "Http");
        HtmlPrinter::PrintTestCases("MappingService", "../../TestData/MappingService/MappingServiceTest.dump", "Http");
        HtmlPrinter::PrintTestCases("SiteServiceTests", "../../TestData/SiteService/SiteServiceTest.dump", "Http");
        HtmlPrinter::PrintTestCases("WfsTests", "../../TestData/Wfs/WfsTest.dump", "Http");
        HtmlPrinter::PrintTestCases("WmsTests", "../../TestData/Wms/WmsTest.dump", "Http");

        HtmlPrinter::PrintSelectTestMode();
        HtmlPrinter::PrintSelectOutput();
        HtmlPrinter::PrintFormFooter("Http", "RunTests");
        HtmlPrinter::PrintHtmlFooter();
    }
}
?>

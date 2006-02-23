<?php
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

        HtmlPrinter::PrintSelectTestMode();
        HtmlPrinter::PrintSelectOutput();
        HtmlPrinter::PrintFormFooter("Http", "RunTests");
        HtmlPrinter::PrintHtmlFooter();
    }
}
?>

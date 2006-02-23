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

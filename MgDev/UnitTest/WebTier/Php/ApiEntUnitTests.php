<?php
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
        HtmlPrinter::PrintHtmlHeader("ApiEnt Tests");
        HtmlPrinter::PrintFormHeader("Run.php");

        //Add your tests here
        //HtmlPrinter::PrintTestCases("YourTestName", "Path to database file where your data is stored", "Api")
        HtmlPrinter::PrintTestCases("FeatureService", "../../TestData/FeatureService/FeatureServiceTest.dump", "ApiEnt");

        HtmlPrinter::PrintSelectTestMode();
        HtmlPrinter::PrintSelectOutput();
        HtmlPrinter::PrintFormFooter("ApiEnt", "RunTests");
        HtmlPrinter::PrintHtmlFooter();
    }
}
?>

<?php
//
// Usage:
// php-cgi updateResource.php user=SomeUserName num=NumberOfResources loop=LoopCount
//
//
require_once("../../../Web/src/PhpApi/constants.php");
require_once("Utils.php");

try
{

    Utils::MapAgentInit("../../../Web/src/webconfig.ini");
    $site = new MgSiteConnection();
    $cred = new MgUserInformation();
    $cred->SetMgUsernamePassword("Administrator","admin");
    $cred->SetLocale("en");
    $site->Open($cred);
    $svc = $site->CreateService(MgServiceType::ResourceService);
}
catch (MgException $exc)
{
    header("HTTP/1.1 559".$exc->GetMessage());
    $hdr = "Status: 559 ".$exc->GetMessage();
    header($hdr);
    echo "<html>\n";
    echo "<body>\n";
    echo $hdr."\n";
    echo $exc->GetMessage()."\n";
    echo $exc->GetDetails()."\n";
    echo "</body>\n";
    echo "</html>\n";
    return;
}
echo "<html>\n";
echo "<body>\n";

$layerName = 'Library://TrevorWekel/PerfTest/Sdf1.FeatureSource';

$res = new MgByteSource("../../TestData/FeatureService/Sdf1.FeatureSource");
echo "Updating resource " . $layerName . "\n";
ob_flush();

$id = new MgResourceIdentifier($layerName);

try
{

    $svc->SetResource($id, $res->GetReader(), null);

}
catch (MgException $e)
{
    // Resource already exists, probably ok.
    echo $e->GetDetails(). "\n";
    ob_flush();
}

$shortFileName = $_POST["file"];

if (strlen($shortFileName) <= 0)
{
    echo "Need file argument.  Exiting\n";
    return;
}

$fileName = "../../TestData/FeatureService/".$shortFileName;
$res = new MgByteSource($fileName);
$rdr = $res->GetReader();
echo "Starting data transmission of " .filesize($fileName) . " bytes from " . $fileName . " \n.";
ob_flush();
$opStart = microtime(true);
$svc->SetResourceData($id, $shortFileName, "File", $rdr);
$opEnd = microtime(true);
$diffTime = $opEnd - $opStart;
echo "Transmission completed in " . $diffTime . " seconds\n";
echo "Effective bandwidth was " . filesize($fileName)/$diffTime/1000000 . " million bytes/sec\n";
echo "</body>\n";
echo "</html>\n";
?>

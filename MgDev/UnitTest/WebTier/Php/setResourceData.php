<?php

//
//  Copyright (C) 2004-2011 by Autodesk, Inc.
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
    header("HTTP/1.1 559".$exc->GetExceptionMessage());
    $hdr = "Status: 559 ".$exc->GetExceptionMessage();
    header($hdr);
    echo "<html>\n";
    echo "<body>\n";
    echo $hdr."\n";
    echo $exc->GetExceptionMessage()."\n";
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

<?php
// get current path
$curPath = __FILE__;
$pos = strrpos($curPath, '\\');
if ($pos == false)
{
    $pos = strrpos($curPath, '/');
}
$curPath = substr_replace($curPath, "", $pos+1);

//Note: current path might already be the script!!
MgInitializeWebTier("../../../Web/src/webconfig.ini");

$locale = $_GET['LOCALE'];

try
{
$site = new MgSiteConnection();
$cred = new MgUserInformation();
$cred->SetMgUsernamePassword("Administrator","admin");
$cred->SetLocale($locale);
$site->Open($cred);
$svc = $site->CreateService(5);
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
echo "Done.\n";
echo "</body>\n";
echo "</html>\n";
?>

<?php

require_once dirname(__FILE__)."/../Web/src/PhpApi/constants.php";

$file = dirname(__FILE__)."/webconfig.ini";
if (array_key_exists("MG_WEBCONFIG_INI", $_SERVER))
    $file = $_SERVER["MG_WEBCONFIG_INI"];

$mgp = dirname(__FILE__)."/TestData/Samples/Sheboygan/Sheboygan.mgp";
if (array_key_exists("MG_SHEBOYGAN_MGP", $_SERVER))
    $mgp = $_SERVER["MG_SHEBOYGAN_MGP"];

if (!file_exists($mgp)) {
    echo "Sheboygan.mgp not found. Please put this file in this directory before running this script\n";
    exit(255);
}
if (!file_exists($file)) {
    $src = dirname(__FILE__)."/../Web/src/webconfig.ini";
    $csMapDir = dirname(__FILE__)."/../Oem/CsMap/Dictionaries";
    $resDir = dirname(__FILE__)."/../Common/MapGuideCommon/Resources";
    
    $ini = file($src);
    $newlines = array();
    foreach ($ini as $lineNum => $line) {
        $write = "";
        $mdp = strpos($line, 'MentorDictionaryPath');
        $rp = strpos($line, 'ResourcesPath');
        if ($mdp !== FALSE && $mdp == 0)
            $write = "MentorDictionaryPath = $csMapDir";
        else if ($rp !== FALSE && $rp == 0)
            $write = "ResourcesPath = $resDir";
        else
            $write = trim($line);

        if ($write != "")
            array_push($newlines, $write);
    }
    
    file_put_contents($file, implode("\n", $newlines));
    echo "Saved: $file\n";
}
try {
    MgInitializeWebTier($file);
    $siteConn = new MgSiteConnection();
    $userInfo = new MgUserInformation("Administrator", "admin");
    $siteConn->Open($userInfo);
    $src = new MgByteSource($mgp);
    $rdr = $src->GetReader();
    $resSvc = $siteConn->CreateService(MgServiceType::ResourceService);
    echo "Loading Sheboygan.mgp\n";
    $resSvc->ApplyResourcePackage($rdr);
    echo "Sheboygan.mgp loaded\n";
    exit(0);
} catch (MgException $ex) {
    echo $ex->GetDetails();
    exit(255);
}

?>
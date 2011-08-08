<?php
    include 'resizableadmin.php';

    LoadSessionVars();

    // Did the user logout?
    CheckForLogout();

    $mapDefiniton=$_GET["mapDefinition"];

    $siteConn = new MgSiteConnection();
    $siteConn->Open($userInfo);
    $resourceSrvc = $siteConn->CreateService(MgServiceType::ResourceService);

    $webLayoutName= "Session:" . $adminSession . "//" . "Map" .rand(). "." . MgResourceType::WebLayout;
    $webLayOutId = new MgResourceIdentifier($webLayoutName);

    // get contents of a file into a string
    try
    {
        $filename = "profilingmapxml\\MapViewerTemplate.xml";
        $handle = fopen($filename, "r");
        $contents = fread($handle, filesize($filename));
        fclose($handle);
    }
    catch (Exception $e) 
    {
        echo $e->getMessage();
    }

    $contents= str_replace("{0}", $mapDefiniton, $contents);

    $content_byteSource = new MgByteSource($contents,strlen($contents));
    $content_byteSource->setMimeType("text/xml");
    $content_byteReader = $content_byteSource->GetReader();

    $resourceSrvc->SetResource($webLayOutId, $content_byteReader, null);
    
    $webLayoutUrl="../mapviewerajax/?WEBLAYOUT=".urlencode($webLayoutName)."&LOCALE=en";

    header('Location:'. $webLayoutUrl);
?>

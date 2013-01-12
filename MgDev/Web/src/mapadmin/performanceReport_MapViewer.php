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
        //under Liunx, we should use '/' instead of '\\'
        $filename = "profilingmapxml/MapViewerTemplate.xml";
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

    //pass the seesion ID with the url, so when the map viewer is opened, there is no need to re-enter the password
    $ajaxViewerFolder = "mapviewerajax/?";

    //when user installed the ajax java viewer, the apache will add a redict
    //from mapviewerajax to mapviewerjava, and change the relative path to absolute path
    //which casues the cross-domain problem
    //now we check if we install the ajax java, then use the "mapviewerjava/ajaxviewer.jsp?" directly to avoid this problem
    $os = (DIRECTORY_SEPARATOR=='\\') ? "winNT" : "linux";
    //under the linux system, the mapviewerjava is always installed, so if someone is running apache + java on linux,
    //the admin should manually change the line 51 as:
    //if(file_exists("../mapviewerjava/ajaxviewer.jsp"))
    if( ("winNT" == $os) && file_exists("../mapviewerjava/ajaxviewer.jsp"))
    {
        $ajaxViewerFolder = "mapviewerjava/ajaxviewer.jsp?";
    }

    $webLayoutUrl="../".$ajaxViewerFolder."WEBLAYOUT=".urlencode($webLayoutName)."&LOCALE=en&SESSION=".$site->GetCurrentSession();

    header('Location:'. $webLayoutUrl);
?>

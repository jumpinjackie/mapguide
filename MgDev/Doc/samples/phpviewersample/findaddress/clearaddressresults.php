<?php

//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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

?>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>

<head>
    <title>Viewer Sample Application - Clear Address Results</title>
    <meta content="text/html; charset=utf-8" http-equiv="Content-Type">
    <meta http-equiv="content-script-type" content="text/javascript">
</head>

<?php
    include '../utilityfunctions.php';

    $mgSessionId = ($_SERVER['REQUEST_METHOD'] == "POST")? $_POST['SESSION']: $_GET['SESSION'];

    try
    {
        // Initialize the Web Extensions and connect to the Server using
        // the Web Extensions session identifier stored in PHP session state.

        MgInitializeWebTier($configFilePath);

        $userInfo = new MgUserInformation($mgSessionId);
        $siteConnection = new MgSiteConnection();
        $siteConnection->Open($userInfo);

        $featureService = $siteConnection->CreateService(MgServiceType::FeatureService);

        $addressMarkerDataResId = new MgResourceIdentifier("Session:" . $mgSessionId . "//AddressMarker.FeatureSource");

        $commands = new MgFeatureCommandCollection();
        $commands->Add(new MgDeleteFeatures('AddressMarker', "ID like '%'"));

        $featureService->UpdateFeatures($addressMarkerDataResId, $commands, false);

        // Create a ReserviceService object and use it to open the Map
        // object from the sessions repository. Use the Map object to hide
        // the "ParcelMarker" layer and then save the updated Map back to
        // the session.

        $resourceService = $siteConnection->CreateService(MgServiceType::ResourceService);
        $map = new MgMap();
        $map->Open($resourceService, 'Sheboygan');

        $layer = GetLayerByName($map, 'AddressMarker');
        $layer->SetVisible(false);
        $layer->ForceRefresh();

        $map->Save($resourceService);
    }
    catch (MgException $e)
    {
        echo $e->GetExceptionMessage();
        echo $e->GetDetails();
    }
?>

<!-- Finally emit this function and assocate it with the onLoad event  -->
<!-- for the page so that it gets executed when this page loads in the -->
<!-- browser. The function calls the Refresh method on the Viewer      -->
<!-- frame forcing the Viewer to update itself to match the state of   -->
<!-- the Server-Side Map object.                                       -->

<script language=javascript>
function OnPageLoad()
{
   parent.Refresh();
   parent.taskFrame.taskPaneFrame.location.href = "findaddressmain.php?SESSION=<?php echo $mgSessionId; ?>";
}
</script>

<body onLoad="OnPageLoad()">
</body>

</html>

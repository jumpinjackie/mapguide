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
    <title>Find Address Task</title>
    <meta content="text/html; charset=utf-8" http-equiv="Content-Type">
    <link rel="stylesheet" href="../styles/gt.css" type="text/css">

    <script language="javascript">
    function Submit()
    {
    	var addressForm = document.getElementById("addressForm");
    	var addressValue = document.getElementById("addressValue");
    	var addressInput = document.getElementById("addressInput");

    	addressValue.value = addressInput.value;
    	addressForm.submit();
    }
    </script>
</head>

<body>

<?php
    include '../utilityfunctions.php';
    include 'findaddressfunctions.php';

    $mgSessionId = ($_SERVER['REQUEST_METHOD'] == "POST")? $_POST['SESSION']: $_GET['SESSION'];
    $showPreviousResults = false;

    try
    {
        // Initialize the Web Extensions and connect to the Server using
        // the Web Extensions session identifier stored in PHP session state.

        MgInitializeWebTier($configFilePath);

        $userInfo = new MgUserInformation($mgSessionId);
        $siteConnection = new MgSiteConnection();
        $siteConnection->Open($userInfo);

        // Create a ReserviceService object and use it to open the Map
        // object from the sessions repository. Use the Map object to
        // determine if the "AddressMarker" layer is visible.

        $resourceService = $siteConnection->CreateService(MgServiceType::ResourceService);
        $map = new MgMap();
        $map->Open($resourceService, 'Sheboygan');
        $addressLayer = GetLayerByName($map, 'AddressMarker');

        if ($addressLayer != null)
        {
            $showPreviousResults = $addressLayer->GetVisible();
        }
    }
    catch (MgException $e)
    {
        echo $e->GetExceptionMessage();
        echo $e->GetDetails();
    }
?>

<form id="addressForm" action="findaddress.php" method="get" target="_self">
    <input name="SESSION" type="hidden" value="<?php echo $mgSessionId; ?>">
    <input id="addressValue" name="address" type="hidden" value="">
</form>

<table class="RegText" border="0" cellspacing="0" width="100%">
    <tr><td class="Title">&nbsp;Find Address<hr></td></tr>
    <tr><td class="SubTitle">Search for an Address</td></tr>
    <tr><td>Address:</td></tr>
    <tr>
        <td class="RegText">
        <input maxlength="100" class="Ctrl" id="addressInput" style="width: 95%">
        </td>
    </tr>
    <tr><td><strong>Sheboygan, WI</strong></td></tr>
    <tr>
        <td>
        <input class="Ctrl" type="button" onClick="Submit()" value="Find" style="width:60px">
        </td>
    </tr>

    <tr><td><hr>Hint try:</td></tr>
    <tr><td>&nbsp;&nbsp;601 North 5th Street</td></tr>
    <tr><td>&nbsp;&nbsp;720 Riverfront Drive<hr></td></tr>
</table>

<?php
    // If there are previous results, display them as hyperlinks and emit
    // a second form with a Clear Results button.

    if ($showPreviousResults)
    {
        $featureService = $siteConnection->CreateService(MgServiceType::FeatureService);
        $addressMarkerDataResId = new MgResourceIdentifier("Session:" . $mgSessionId . "//AddressMarker.FeatureSource");

        EmitAddressResults($featureService, $addressMarkerDataResId, $mgSessionId);
    }
?>

Geocoding Provided by: <a href="http://geocoder.us/" target="_blank">geocoder.us</a>.<br>
Locative Tecnologies offers the free geocoder.us web service for non commercial purposes. If you wish to use this geocoding web service in an production application you must sign up for a commercial license. Click the link above for more information.

</body>

</html>

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
    <title>Viewer Sample Application - Find Address</title>
    <meta content="text/html; charset=utf-8" http-equiv="Content-Type">
    <link rel="stylesheet" href="../styles/gt.css" type="text/css">

    <script language=javascript>
        function InitDocument()
        {
            this.ZoomToResult();
        }
    </script>
</head>

<body onload="InitDocument()">

<table class="RegText" border="0" cellspacing="0" width="100%">
    <tr><td class="Title">&nbsp;Find Address<hr></td></tr>
    <tr><td class="SubTitle">Search Results</td></tr>

<?php
    include '../utilityfunctions.php';
    include 'findaddressfunctions.php';

    // Include the xmlrpc library.
    require_once('kd_xmlrpc.php');

    $mgSessionId = ($_SERVER['REQUEST_METHOD'] == "POST")? $_POST['SESSION']: $_GET['SESSION'];
    $success = false;
    $long = 0; $lat = 0;

    try
    {
        // Initialize the Web Extensions and connect to the Server using
        // the Web Extensions session identifier stored in PHP session state.

        MgInitializeWebTier ($configFilePath);

        $userInfo = new MgUserInformation($mgSessionId);
        $siteConnection = new MgSiteConnection();
        $siteConnection->Open($userInfo);

        $address = $_GET['address'] . ', Sheboygan, WI';

        // Make the request to geocoder.us passing the address.
        list($success, $response) = XMLRPC_request('rpc.geocoder.us', '/service/xmlrpc', 'geocode', array(XMLRPC_prepare($address)));

        // Stuff the response into a new variable:
        $nested_array = $response;

        // For some reason (presumably a good one), geocoder.us responds with a nested
        // array. We need to extract the inner array from the outer and stuff it into a
        // new variable:
        $geoarray = array_pop($nested_array);

        // The $success variable is intended for error handling, but since geocoder.us
        // will send an error response to invalid queries (thus flagging $success to 1),
        // we can't use this to check for errors. Instead, we'll check the values in the
        // array and deem success or failure based on that.

        if (array_key_exists('lat', $geoarray) && array_key_exists('long', $geoarray))
        {
            // The geocode successfully returned a location.

            $lat = $geoarray['lat'];
            $long = $geoarray['long'];

            // Build an address1 and address2 variables from the result.
            $address1 = $geoarray['number'] . ' '
                . (strlen($geoarray['prefix']) > 0 ? $geoarray['prefix'] . ' ' : '')
                . (strlen($geoarray['street']) > 0 ? $geoarray['street'] . ' ' : '')
                . (strlen($geoarray['type']) > 0 ? $geoarray['type'] . ' ' : '')
                . (strlen($geoarray['suffix']) > 0 ? $geoarray['suffix'] . ' ' : '');

            $address2 = $geoarray['city']
                . ', ' . $geoarray['state']
                . '  ' . $geoarray['zip'];

            echo '<tr><td><img src="../images/pushpinblue.jpg">';
            echo '<a href="gotopoint.php?X=' . $long . '&Y=' . $lat . '&Scale=2000" target="scriptFrame"> ' . $address1 . '</a></td></tr>';
            echo '<tr><td>' . $address2 . '</td></tr>';
            echo '<tr><td class="Spacer"></td></tr>';
            echo '<tr><td>Longitude: ' . $long . '</td></tr>';
            echo '<tr><td>Latitude: ' . $lat . '<hr></td></tr>';

            // Create ResourceService and FeatureService objects and use
            // the ResourceService object to open the map instance in
            // session state.

            $resourceService = $siteConnection->CreateService(MgServiceType::ResourceService);
            $featureService = $siteConnection->CreateService(MgServiceType::FeatureService);

            $map = new MgMap();
            $map->Open($resourceService, 'Sheboygan');

            // Check the map for the AddressMarker layer. If it does not
            // exist then create a feature source to store address results
            // and a layer to display them.

            $addressMarkerDataResId = new MgResourceIdentifier("Session:" . $mgSessionId . "//AddressMarker.FeatureSource");
            $addressLayer = GetLayerByName($map, 'AddressMarker');
            if ($addressLayer == null)
            {
                CreateAddressMarkerFeatureSource($featureService, $addressMarkerDataResId);
                $addressLayer = CreateAddressMarkerLayer($resourceService, $addressMarkerDataResId, $mgSessionId);

                $map->GetLayers()->Insert(0, $addressLayer);
            }
            else if ($addressLayer->GetVisible())
            {
                // If the layer exists and is visible, then display the
                // previous results.

                EmitAddressResults($featureService, $addressMarkerDataResId, $mgSessionId);
            }

            // Insert the results of the Geo-Code into the temporary
            // feature source and ensure the address marker layer
            // is visible.

            $geometryReaderWriter = new MgAgfReaderWriter();
            $geometryFactory = new MgGeometryFactory();
            $addrPoint = $geometryFactory->CreatePoint($geometryFactory->CreateCoordinateXY((double)$long, (double)$lat));

            $properties = new MgPropertyCollection();
            $properties->Add(new MgStringProperty('Address', $address1));
            $properties->Add(new MgGeometryProperty('Location', $geometryReaderWriter->Write($addrPoint)));

            $commands = new MgFeatureCommandCollection();
            $commands->Add(new MgInsertFeatures('AddressMarker', $properties));

            $featureService->UpdateFeatures($addressMarkerDataResId, $commands, false);

            $addressLayer->SetVisible(true);
            $addressLayer->ForceRefresh();

            $map->Save($resourceService);

            $success = true;
        }
        else
        {
            echo '<tr><td>Address not found: ' . $address . '<hr></td></tr>';
            $success = false;
        }
    }
    catch (MgException $e)
    {
        echo $e->GetExceptionMessage();
        echo '<br>';
        echo $e->GetDetails();
    }
?>

</table>

Geocoding Provided by: <a href="http://geocoder.us/" target="_blank">geocoder.us</a>.<br>
Locative Tecnologies offers the free geocoder.us web service for non commercial purposes. If you wish to use this geocoding web service in an production application you must sign up for a commercial license. Click the link above for more information.

</body>

<script language=javascript>
function ZoomToResult()
{
    success = <?php echo $success == true? "true": "false"; ?>;
    if(success)
        parent.parent.ZoomToView(<?php echo $long; ?>, <?php echo $lat; ?>, 2000, true);
}
</script>

</html>

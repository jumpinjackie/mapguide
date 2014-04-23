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

function EmitAddressResults($featureService, $addressMarkerDataResId, $mgSessionId)
{
    echo '<table class="RegText" border="0" cellspacing="0" width="100%">';
    echo '<tr><td class="SubTitle">Previous Results</td></tr>';

    $geometryReaderWriter = new MgAgfReaderWriter();
    $featureReader = $featureService->SelectFeatures($addressMarkerDataResId, 'AddressMarker', null);

    while ($featureReader->ReadNext())
    {
        $address = $featureReader->GetString('Address');
        $byteReader = $featureReader->GetGeometry('Location');

        $geometry = $geometryReaderWriter->Read($byteReader);
        $x = $geometry->GetCoordinate()->GetX();
        $y = $geometry->GetCoordinate()->GetY();

        echo '<tr><td>';
        echo '<img src="../images/pushpinblue.jpg">';
        echo '<a href="gotopoint.php?X=' . $x . '&Y=' . $y . '&Scale=2000" target="scriptFrame"> ' . $address . '</a></td></tr>';
    }
    $featureReader->Close();

    echo '</table>';
    echo '<form action="clearaddressresults.php" method="get" target="scriptFrame">';
    echo '<input type="hidden" name="SESSION" value="' . $mgSessionId . '">';
    echo '<input type="button" class="Ctrl" value="Clear Results" onClick="javascript:submit()">';
    echo '</form>';
    echo '<hr>';
}

// Create a temporary Feature Source to store geocode results.

function CreateAddressMarkerFeatureSource($featureService, $addressMarkerDataResId)
{
    $ll84Wkt = 'GEOGCS["WGS84 Lat/Long\'s, Degrees, -180 ==> +180",DATUM["D_WGS_1984",SPHEROID["World_Geodetic_System_of_1984",6378137,298.257222932867]],PRIMEM["Greenwich",0],UNIT["Degree",0.017453292519943295]]';

    $addressClass = new MgClassDefinition();
    $addressClass->SetName('AddressMarker');
    $properties = $addressClass->GetProperties();

    $idProperty = new MgDataPropertyDefinition('ID');
    $idProperty->SetDataType(MgPropertyType::Int32);
    $idProperty->SetReadOnly(true);
    $idProperty->SetNullable(false);
    $idProperty->SetAutoGeneration(true);
    $properties->Add($idProperty);

    $addressProperty = new MgDataPropertyDefinition('Address');
    $addressProperty->SetDataType(MgPropertyType::String);
    $addressProperty->SetLength(512);
    $properties->Add($addressProperty);

    $locationProperty = new MgGeometricPropertyDefinition('Location');
    $locationProperty->SetGeometryTypes(MgGeometryType::Point);
    $locationProperty->SetHasElevation(false);
    $locationProperty->SetHasMeasure(false);
    $locationProperty->SetReadOnly(false);
    $locationProperty->SetSpatialContextAssociation('LL84');
    $properties->Add($locationProperty);

    $idProperties = $addressClass->GetIdentityProperties();
    $idProperties->Add($idProperty);

    $addressClass->SetDefaultGeometryPropertyName('Location');

    $addressSchema = new MgFeatureSchema();
    $addressSchema->SetName('AddressMarkerSchema');
    $addressSchema->GetClasses()->Add($addressClass);

    $sdfParams = new MgCreateSdfParams('LL84', $ll84Wkt, $addressSchema);

    $featureService->CreateFeatureSource($addressMarkerDataResId, $sdfParams);
}

// Create a temporary Layer to display geocode results.

function CreateAddressMarkerLayer($resourceService, $addressMarkerDataResId, $sessionId)
{
    // Load the AddressMarker layer definition template into
    // a PHP DOM object, find the "ResourceId" element, and
    // modify it's content to reference the temporary
    // feature source.

    $doc = DOMDocument::load(GetXmlPath());
    $featureSourceNode = $doc->getElementsByTagName('ResourceId')->item(0);
    $featureSourceNode->nodeValue = $addressMarkerDataResId->ToString();

    // Get the updated layer definition from the DOM object
    // and save it to the session repository using the
    // ResourceService object.

    $layerDefinition = $doc->saveXML();
    $byteSource = new MgByteSource($layerDefinition, strlen($layerDefinition));
    $byteSource->SetMimeType(MgMimeType::Xml);

    $tempLayerResId = new MgResourceIdentifier("Session:" . $sessionId . "//AddressMarker.LayerDefinition");

    $resourceService->SetResource($tempLayerResId, $byteSource->GetReader(), null);

    // Create an MgLayer object based on the new layer definition
    // and return it to the caller.

    $addressLayer = new MgLayer($tempLayerResId, $resourceService);
    $addressLayer->SetName("AddressMarker");
    $addressLayer->SetLegendLabel("AddressMarker");
    $addressLayer->SetDisplayInLegend(false);
    $addressLayer->SetSelectable(false);

    return $addressLayer;
}

?>

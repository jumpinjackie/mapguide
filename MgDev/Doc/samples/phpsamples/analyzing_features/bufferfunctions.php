<?php

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

function CreateBufferFeatureSource($featureService, $wkt, $bufferFeatureResId)
{
    $bufferClass = new MgClassDefinition();
    $bufferClass->SetName('BufferClass');
    $properties = $bufferClass->GetProperties();

    $idProperty = new MgDataPropertyDefinition('ID');
    $idProperty->SetDataType(MgPropertyType::Int32);
    $idProperty->SetReadOnly(true);
    $idProperty->SetNullable(false);
    $idProperty->SetAutoGeneration(true);
    $properties->Add($idProperty);

    $polygonProperty = new MgGeometricPropertyDefinition('BufferGeometry');
    $polygonProperty->SetGeometryTypes(MgFeatureGeometricType::Surface);
    $polygonProperty->SetHasElevation(false);
    $polygonProperty->SetHasMeasure(false);
    $polygonProperty->SetReadOnly(false);
    $polygonProperty->SetSpatialContextAssociation('defaultSrs');
    $properties->Add($polygonProperty);

    $idProperties = $bufferClass->GetIdentityProperties();
    $idProperties->Add($idProperty);

    $bufferClass->SetDefaultGeometryPropertyName('BufferGeometry');

    $bufferSchema = new MgFeatureSchema('BufferLayerSchema', 'temporary schema to hold a buffer');
    $bufferSchema->GetClasses()->Add($bufferClass);

    $sdfParams = new MgCreateSdfParams('defaultSrs', $wkt, $bufferSchema);

    $featureService->CreateFeatureSource($bufferFeatureResId, $sdfParams);
}

function CreateBufferLayer($resourceService, $bufferFeatureResId, $sessionId)
{
    // Load the layer definition template into
    // a PHP DOM object, find the "ResourceId" element, and
    // modify its content to reference the temporary
    // feature source.

    $doc = DOMDocument::load('bufferlayerdefinition.xml');
    $featureSourceNode = $doc->getElementsByTagName('ResourceId')->item(0);
    $featureSourceNode->nodeValue = $bufferFeatureResId->ToString();

    // Get the updated layer definition from the DOM object
    // and save it to the session repository using the
    // ResourceService object.

    $layerDefinition = $doc->saveXML();
    $byteSource = new MgByteSource($layerDefinition, strlen($layerDefinition));
    $byteSource->SetMimeType(MgMimeType::Xml);

    $tempLayerResId = new MgResourceIdentifier("Session:" . $sessionId . "//Buffer.LayerDefinition");

    $resourceService->SetResource($tempLayerResId, $byteSource->GetReader(), null);

    // Create an MgLayer object based on the new layer definition
    // and return it to the caller.

    $bufferLayer = new MgLayer($tempLayerResId, $resourceService);
    $bufferLayer->SetName("Buffer");
    $bufferLayer->SetLegendLabel("Buffer");
    $bufferLayer->SetDisplayInLegend(true);
    $bufferLayer->SetSelectable(false);
    
    return $bufferLayer;
}

function CreateParcelMarkerFeatureSource($featureService, $wkt, $parcelMarkerDataResId)
{
    $parcelClass = new MgClassDefinition();
    $parcelClass->SetName('ParcelMarkerClass');
    $properties = $parcelClass->GetProperties();

    $idProperty = new MgDataPropertyDefinition('ID');
    $idProperty->SetDataType(MgPropertyType::Int32);
    $idProperty->SetReadOnly(true);
    $idProperty->SetNullable(false);
    $idProperty->SetAutoGeneration(true);
    $properties->Add($idProperty);

    $pointProperty = new MgGeometricPropertyDefinition('ParcelLocation');
    $pointProperty->SetGeometryTypes(MgGeometryType::Point);
    $pointProperty->SetHasElevation(false);
    $pointProperty->SetHasMeasure(false);
    $pointProperty->SetReadOnly(false);
    $pointProperty->SetSpatialContextAssociation('defaultSrs');
    $properties->Add($pointProperty);

    $idProperties = $parcelClass->GetIdentityProperties();
    $idProperties->Add($idProperty);

    $parcelClass->SetDefaultGeometryPropertyName('ParcelLocation');

    $parcelSchema = new MgFeatureSchema('ParcelLayerSchema', 'temporary schema to hold parcel markers');
    $parcelSchema->GetClasses()->Add($parcelClass);

    $sdfParams = new MgCreateSdfParams('defaultSrs', $wkt, $parcelSchema);

    $featureService->CreateFeatureSource($parcelMarkerDataResId, $sdfParams);
}

function CreateParcelMarkerLayer($resourceService, $parcelMarkerDataResId, $sessionId)
{
    // Load the ParcelMarker layer definition template into
    // a PHP DOM object, find the "ResourceId" element, and
    // modify its content to reference the temporary
    // feature source.

    $doc = DOMDocument::load('parcelmarker.xml');
    $featureSourceNode = $doc->getElementsByTagName('ResourceId')->item(0);
    $featureSourceNode->nodeValue = $parcelMarkerDataResId->ToString();

    // Get the updated layer definition from the DOM object
    // and save it to the session repository using the
    // ResourceService object.

    $layerDefinition = $doc->saveXML();
    $byteSource = new MgByteSource($layerDefinition, strlen($layerDefinition));
    $byteSource->SetMimeType(MgMimeType::Xml);

    $tempLayerResId = new MgResourceIdentifier("Session:" . $sessionId . "//ParcelMarker.LayerDefinition");

    $resourceService->SetResource($tempLayerResId, $byteSource->GetReader(), null);

    // Create an MgLayer object based on the new layer definition
    // and return it to the caller.

    $parcelMarkerLayer = new MgLayer($tempLayerResId, $resourceService);
    $parcelMarkerLayer->SetName("ParcelMarker");
    $parcelMarkerLayer->SetLegendLabel("ParcelMarker");
    $parcelMarkerLayer->SetDisplayInLegend(true);
    $parcelMarkerLayer->SetSelectable(false);

    return $parcelMarkerLayer;
}
?>
<?php
/**
 * Query
 *
 * $Id: Query.php 963 2007-10-16 15:37:30Z madair $
 *
 * Copyright (c) 2007, DM Solutions Group Inc.
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/*****************************************************************************
 * Purpose: create a new selection based on one or more attribute filters and
 *          a spatial filter
 *****************************************************************************/


function GetPropertyValueFromFeatReader($featureReader, $propertyType, $propertyName)
{
    $val = "";

    switch ($propertyType)
    {
       case MgPropertyType::Null :
         //fwrite($logFileHandle, "$propertyName is a null propertyn");
         $val= "";
         break;
       case MgPropertyType::Boolean :
         $val = $featureReader->GetBoolean($propertyName);
         //$valStr = printBoolean($val);
         break;
       case MgPropertyType::Byte :
         $val = $featureReader->GetByte($propertyName);
         break;
       case MgPropertyType::DateTime :
         $val = $featureReader->GetDateTime($propertyName);
         //$valStr = printDateTime($val);
         break;
       case MgPropertyType::Single :
         $val = $featureReader->GetSingle($propertyName);
         break;
       case MgPropertyType::Double :
         $val = $featureReader->GetDouble($propertyName);
         break;
       case MgPropertyType::Int16 :
         $val = $featureReader->GetInt16($propertyName);
         break;
       case MgPropertyType::Int32 :
         $val = $featureReader->GetInt32($propertyName);
         break;
       case MgPropertyType::Int64 :
         $val = $featureReader->GetInt64($propertyName);
         break;
       case MgPropertyType::String :
         $val = $featureReader->GetString($propertyName);
         break;
       case MgPropertyType::Blob :
         //fwrite($logFileHandle, "$propertyName is blobn");
         break;
       case MgPropertyType::Clob :
         //fwrite($logFileHandle, "$propertyName is clobn");
              break;
       case MgPropertyType::Feature :
         /*
              $val = $featureReader->GetFeatureObject($propertyName);
             if ($val != NULL) {
                  fwrite($logFileHandle, "$propertyName is a featuren");
                  printFeatureReader($val);
             }
         */
         break;
       case MgPropertyType::Geometry :
         /*
              fwrite($logFileHandle, "$propertyName is a geometryn");
              $val = $featureReader->GetGeometry($propertyName);
              if ($val != NULL) {
                 $aGeometry = $agfReaderWriter->Read($val);
                 //$aGeometry->Envelope();
                 $wktRepresentation = $wktReaderWriter->Write($aGeometry);
                 fwrite($logFileHandle, "WKT Representation: "$wktRepresentation"n");
              } else {
                 fwrite($logFileHandle, "This geometry property is nulln");
              }
         */
         break;
       case MgPropertyType::Raster :
         /*
              $val = $featureReader->GetRaster($propertyName);
             fwrite($logFileHandle, "$propertyName is a rastern");
         */
         break;
       default :
         $val = "";
    }

    return $val;
}

try {
    /* set up the session */
    include ("Common.php");
    include('../../common/php/Utilities.php');
    include('Utilities.php');

    /* the name of the layer in the map to query */
    if ($_REQUEST['layers'] != '') {
        $layers = explode(',',$_REQUEST['layers']);
    } else {
        $layers = array();
    }

    // echo "<!--";
    //     print_r($_REQUEST);
    //     echo "-->";

    /* selection variant if set */
    $variant = MgFeatureSpatialOperations::Intersects;
    if (isset($_REQUEST['variant'])) {
        if (strcasecmp($_REQUEST['variant'],'contains') == 0) {
            $variant = MgFeatureSpatialOperations::Contains;
        } else if (strcasecmp($_REQUEST['variant'],'inside') == 0) {
            $variant = MgFeatureSpatialOperations::Inside;
        }
    }
    /* a filter expression to apply, in the form of an FDO SQL statement */
    $filter = isset($_REQUEST['filter']) ? str_replace(array('*', '"'), array('%', "'"),html_entity_decode(urldecode( $_REQUEST['filter']))) : false;
    //echo "<!-- filter: $filter -->\n";
    /* a spatial filter in the form on a WKT geometry */
    $spatialFilter = (isset($_REQUEST['spatialfilter']) && $_REQUEST['spatialfilter'] != '') ? urldecode($_REQUEST['spatialfilter']) : false;
    //echo "spatial filter is $spatialFilter<BR>";
    /* we need a feature service to query the features */
    $featureService = $siteConnection->CreateService(MgServiceType::FeatureService);

    /* open the map from the session using the provided map name.  The map was
       previously created by calling LoadMap. */
    $map = new MgMap();
    $map->Open($resourceService, $mapName);




    /* add the spatial filter if provided.  It is expected to come as a
       WKT string, so we need to convert it to an MgGeometry */
    if ($spatialFilter !== false ) {
        //echo 'setting spatial filter<br>';
        $wktRW = new MgWktReaderWriter();
        $geom = $wktRW->Read($spatialFilter);
    }
    /* add the features to the map selection and save it*/
    $selection = new MgSelection($map);
    /*
    */
    /* if extending the current selection */
    $bExtendSelection = isset($_REQUEST['extendselection']) && strcasecmp($_REQUEST['extendselection'], 'true') == 0;
    if ($bExtendSelection) {
        $aLayerSelections = array();
        $selection->Open($resourceService, $mapName);
        $aLayers = selectionToArray($selection, array());
    }

    /*holds selection array*/
    $properties = NULL;
    $properties->layers = array();

    /* Get the map SRS - we use this to convert distances */
    $srsFactory = new MgCoordinateSystemFactory();
    //safely get an SRS ... (in Utilities)
    $srsDefMap = GetMapSRS($map);
    $srsMap = $srsFactory->Create($srsDefMap);

    $mapLayers = $map->GetLayers();
    $bAllLayers = false;
    $nLayers = count($layers);
    if ($nLayers == 0) {
        $nLayers = $mapLayers->GetCount();
        $bAllLayers = true;
    }

    for ($i=0; $i<$nLayers; $i++) {
        try {
            if (!$bAllLayers) {
                $layerObj = $mapLayers->GetItem($layers[$i]);
            } else {
                $layerObj = $mapLayers->GetItem($i);
            }

            if (!$layerObj->GetSelectable() || !$layerObj->IsVisible()) {
                continue;
            }


            /* get the feature source from the layer */
            $featureResId = new MgResourceIdentifier($layerObj->GetFeatureSourceId());
            $featureGeometryName = $layerObj->GetFeatureGeometryName();

            $queryOptions = new MgFeatureQueryOptions();

            /* the class that is used for this layer will be used to select
               features */
            $class = $layerObj->GetFeatureClassName();

            //only retrieve properties that we actually need
            $mappings = $_SESSION['property_mappings'][$layerObj->GetObjectId()];
            //TODO : seems that property mapping breaks the selection ????
            //could it be that $selection->AddFeatures($layerObj, $featureReader, 0) is
            //the one causing a problem when the properies are limited ?

            if (0 && count($mappings) > 0)
            {
              foreach($mappings as $name => $value) {
                $queryOptions->AddFeatureProperty($name);
                //echo "$name $value <br>\n";
              }
              $geomName = $layerObj->GetFeatureGeometryName();
              $queryOptions->AddFeatureProperty($geomName);
            }

            /* add the attribute query if provided */
            if ($filter !== false) {
              //echo "<!-- setting filter $filter -->\n";
              $queryOptions->SetFilter($filter);
            }

            //$queryOptions->AddFeatureProperty('RSQFT');
             if ($spatialFilter !== false ) {
                $queryOptions->SetSpatialFilter($featureGeometryName, $geom, $variant);
            }

            /* select the features */
             try {
            $featureReader = $featureService->SelectFeatures($featureResId, $class, $queryOptions);
             }
              catch (MgException $e)
                {
                    echo "ERROR2: " . $e->GetMessage() . "\n";
                    echo $e->GetDetails() . "\n";
                    echo $e->GetStackTrace() . "\n";
                }

              $layerName = $layerObj->GetName();
            array_push($properties->layers, $layerName);

            if ($bExtendSelection) {
                /* possibly toggle features in the map */
                $newSelection = new MgSelection($map);
                $newSelection->AddFeatures($layerObj, $featureReader, 0);
                $aLayers = selectionToArray($newSelection, $aLayers);
            }
            else
            {
                try
                {
                $spatialContext = $featureService->GetSpatialContexts($featureResId, true);
                $srsLayerWkt = false;
                if($spatialContext != null && $spatialContext->ReadNext() != null) {
                    $srsLayerWkt = $spatialContext->GetCoordinateSystemWkt();
                    /* skip this layer if the srs is empty */
                }
                if ($srsLayerWkt == null) {
                    $srsLayerWkt = $srsDefMap;
                }
                /* create a coordinate system from the layer's SRS wkt */
                $srsLayer = $srsFactory->Create($srsLayerWkt);

                // exclude layer if:
                //  the map is non-arbitrary and the layer is arbitrary or vice-versa
                //     or
                //  layer and map are both arbitrary but have different units
                //
                $bLayerSrsIsArbitrary = ($srsLayer->GetType() == MgCoordinateSystemType::Arbitrary);
                $bMapSrsIsArbitrary = ($srsMap->GetType() == MgCoordinateSystemType::Arbitrary);
                $bComputedProperties = true;
                $bNeedsTransform = false;
                if (($bLayerSrsIsArbitrary != $bMapSrsIsArbitrary) ||
                    ($bLayerSrsIsArbitrary && ($srsLayer->GetUnits() != $srsMap->GetUnits()))) {
                    $bComputedProperties = false;
                } else {
                    $srsTarget = null;
                    $srsXform = null;
                    $bNeedsTransform = ($srsLayer->GetUnitScale() != 1.0);
                }
                }
                catch (MgException $e)
                {
                    echo "ERROR: " . $e->GetMessage() . "\n";
                    echo $e->GetDetails() . "\n";
                    echo $e->GetStackTrace() . "\n";
                }

                /* add the features to the map */
                $selection->AddFeatures($layerObj, $featureReader, 0);
                $featureReader->Close();


                $featureReader = $featureService->SelectFeatures($featureResId, $class, $queryOptions);
                $properties = BuildSelectionArray($featureReader, $layerName, $properties,
                                                  $bComputedProperties,
                                                  $srsLayer, $bNeedsTransform, $layerObj);
            }

             /* close the feature reader - not doing this could cause problems */
            $featureReader->Close();

        } catch (MgObjectNotFoundException $onfe) {
            //skip layers not in the map?
            echo "Object not found";
             echo "ERROR: " . $onfe->GetMessage() . "\n";
             echo $onfe->GetDetails() . "\n";
             echo $onfe->GetStackTrace() . "\n";
        } catch (MgException $e) {
            //what should we do with general exceptions?
            echo "general exception";
        }
    }


    if ($bExtendSelection) {
        $selection = new MgSelection($map);
        $queryOptions = new MgFeatureQueryOptions();
        $layers = $map->GetLayers();
        foreach($aLayers as $szLayer => $aLayer) {
            $oLayer = $layers->GetItem($szLayer);
            foreach($aLayer as $szClass => $aFilter) {
                /* get the feature source from the layer */
                $featureResId = new MgResourceIdentifier($oLayer->GetFeatureSourceId());
                $featureGeometryName = $oLayer->GetFeatureGeometryName();
                $szFilter = implode(' OR ', $aFilter);
                $queryOptions->setFilter($szFilter);
                /* the class that is used for this layer will be used to select
                   features */
                $class = $oLayer->GetFeatureClassName();


                /* select the features */
                $featureReader = $featureService->SelectFeatures($featureResId, $class, $queryOptions);
                $selection->AddFeatures($oLayer, $featureReader,0);

                $layerName = $oLayer->GetName();
                array_push($properties->layers, $layerName);
                $featureReader->Close();
                $featureReader = $featureService->SelectFeatures($featureResId, $class, $queryOptions);

                $spatialContext = $featureService->GetSpatialContexts($featureResId, true);
                $srsLayerWkt = false;
                if($spatialContext != null && $spatialContext->ReadNext() != null) {
                    $srsLayerWkt = $spatialContext->GetCoordinateSystemWkt();
                    /* skip this layer if the srs is empty */
                }
                if ($srsLayerWkt == null) {
                    $srsLayerWkt = $srsDefMap;
                }
                /* create a coordinate system from the layer's SRS wkt */
                $srsLayer = $srsFactory->Create($srsLayerWkt);

                // exclude layer if:
                //  the map is non-arbitrary and the layer is arbitrary or vice-versa
                //     or
                //  layer and map are both arbitrary but have different units
                //
                $bLayerSrsIsArbitrary = ($srsLayer->GetType() == MgCoordinateSystemType::Arbitrary);
                $bMapSrsIsArbitrary = ($srsMap->GetType() == MgCoordinateSystemType::Arbitrary);
                $bComputedProperties = true;
                $bNeedsTransform = false;
                if (($bLayerSrsIsArbitrary != $bMapSrsIsArbitrary) ||
                    ($bLayerSrsIsArbitrary && ($srsLayer->GetUnits() != $srsMap->GetUnits()))) {
                    $bComputedProperties = false;
                } else {
                    $srsTarget = null;
                    $srsXform = null;
                    $bNeedsTransform = ($srsLayer->GetUnitScale() != 1.0);
                }

                $properties = BuildSelectionArray($featureReader, $layerName, $properties,
                                                  $bComputedProperties,
                                                  $srsLayer, $bNeedsTransform, $oLayer);
                $featureReader->Close();
            }
        }
    }
    $selection->Save($resourceService, $mapName);

    //print_r($properties);
    //echo var2json($properties);

    header('Content-type: text/x-json');
    header('X-JSON: true');
    $layers = $selection->GetLayers();
    $result = NULL;
    $result->hasSelection = false;
    if ($layers && $layers->GetCount() >= 0)
    {
        $result->hasSelection = true;
        $oExtents = $selection->GetExtents($featureService);
        if ($oExtents)
        {
            $oMin = $oExtents->GetLowerLeftCoordinate();
            $oMax = $oExtents->GetUpperRightCoordinate();
            $result->extents = NULL;
            $result->extents->minx = $oMin->GetX();
            $result->extents->miny = $oMin->GetY();
            $result->extents->maxx = $oMax->GetX();
            $result->extents->maxy = $oMax->GetY();

            /*keep the full extents of the selection when saving the selection in the session*/
            $properties->extents = NULL;
            $properties->extents->minx = $oMin->GetX();
            $properties->extents->miny = $oMin->GetY();
            $properties->extents->maxx = $oMax->GetX();
            $properties->extents->maxy = $oMax->GetY();
        }
        $result->layers = array();
        for ($i=0; $i<$layers->GetCount(); $i++) {
          $layer = $layers->GetItem($i);
          $layerName = $layer->GetName();
          array_push($result->layers, $layerName);
          $layerClassName = $layer->GetFeatureClassName();
          $filter = $selection->GenerateFilter($layer, $layerClassName);
          $a = explode('OR', $filter);
          $result->$layerName->featureCount = count($a);
        }

        /*save selection in the session*/
        $_SESSION['selection_array'] = $properties;
    }
    echo var2json($result);



}
catch (MgException $e)
{
  echo "ERROR: " . $e->GetMessage() . "\n";
  echo $e->GetDetails() . "\n";
  echo $e->GetStackTrace() . "\n";
}

function selectionToArray($selection, $aLayers, $bToggle = true) {
    $layers = $selection->GetLayers();
    if ($layers)
    {
        for ($i = 0; $i < $layers->GetCount(); $i++)
        {
            $layer = $layers->GetItem($i);
            if ($layer)
            {
                $objId = $layer->GetName();
                if (!array_key_exists($objId,$aLayers)) {
                    $aLayers[$objId] = array();
                }
                $layerClassName = $layer->GetFeatureClassName();
                if (!array_key_exists($layerClassName, $aLayers[$objId])) {
                    $aLayers[$objId][$layerClassName] = array();
                }
                $selectionString = $selection->GenerateFilter($layer, $layerClassName);
                $aFilters = explode('OR', $selectionString);
                foreach($aFilters as $filter) {
                    $filter = trim($filter);
                    $key = array_search($filter, $aLayers[$objId][$layerClassName]);
                    if ($key !== false) {
                        unset($aLayers[$objId][$layerClassName][$key]);
                    } else {
                        array_push($aLayers[$objId][$layerClassName], $filter);
                    }
                }
            }
        }
    }
    return $aLayers;
}


/**
   keep all the attributes of selected features in an array
 */
function BuildSelectionArray($featureReader, $layerName, $properties, $bComputedProperties,
                             $srsLayer, $bNeedsTransform, $layerObj)
{
    $agf = new MgAgfReaderWriter();
     $srsFactory = new MgCoordinateSystemFactory();

    $properties->$layerName->propertynames = array();
    $properties->$layerName->propertyvalues = array();
    $properties->$layerName->propertytypes = array();
    $properties->$layerName->numelements = 0;
    $properties->$layerName->values = array();

    $properties->$layerName->metadatanames= array();
    array_push($properties->$layerName->metadatanames, 'dimension');
    array_push($properties->$layerName->metadatanames, 'bbox');
    array_push($properties->$layerName->metadatanames, 'center');
    array_push($properties->$layerName->metadatanames, 'area');
    array_push($properties->$layerName->metadatanames, 'length');

    $propCount = $featureReader->GetPropertyCount();
    $numproperties =0;
    for($j=0; $j<$propCount; $j++)
    {
        $propName = $featureReader->GetPropertyName($j);
        $propType = $featureReader->GetPropertyType($propName);
        if ($propType == MgPropertyType::Geometry) {
            continue;
        }
        $mappings = $_SESSION['property_mappings'][$layerObj->GetObjectId()];
        if (count($mappings) > 0)
        {
          if (!in_array($propName, array_keys($mappings)))
            continue;
        }
        $mapping = isset($mappings[$propName]) ? $mappings[$propName] : $propName;
        array_push($properties->$layerName->propertynames, $propName);
        array_push($properties->$layerName->propertyvalues, $mapping);
        array_push($properties->$layerName->propertytypes, $propType);
        $numproperties++;

    }

    while ($featureReader->ReadNext())
    {
        $properties->$layerName->values[$properties->$layerName->numelements] = array();
        $properties->$layerName->metadata[$properties->$layerName->numelements] = array();

        $dimension = '';
        $bbox = '';
        $center = '';
        $area = '';
        $length = '';
        if ($bComputedProperties)
        {
            $classDef = $featureReader->GetClassDefinition();
            $geomName = $classDef->GetDefaultGeometryPropertyName();
            if ($geomName != '') {
                $geomByteReader = $featureReader->GetGeometry($geomName);
                $agf = new MgAgfReaderWriter();
                $geom = $agf->Read($geomByteReader);

                $envelope = $geom->Envelope();
                $ll = $envelope->GetLowerLeftCoordinate();
                $ur = $envelope->GetUpperRightCoordinate();
                $bbox = $ll->GetX().','.$ll->GetY().','.$ur->GetX().','.$ur->GetY();
                $centroid = $geom->GetCentroid()->GetCoordinate();
                $center = $centroid->GetX().','.$centroid->GetY();

                /* 0 = point, 1 = curve, 2 = surface */
                $dimension = $geom->GetDimension();

                if ($geom->GetDimension() > 0) {
                    if ($bNeedsTransform) {
                        $srsTarget = $srsFactory->Create(getUtmWkt($centroid->GetX(),
                                                                   $centroid->GetY()));
                        $srsXform = $srsFactory->GetTransform($srsLayer, $srsTarget);
                    }
                    if ($srsXform != null) {
                        try {
                            $ageom = $geom->Transform($srsXform);
                            $geom = $ageom;
                        } catch (MgException $ee) {}
                    }

                    if ($geom->GetDimension() > 1) {
                        $area = $geom->GetArea();
                    }
                    if ($geom->GetDimension() > 0) {
                        $length = $geom->GetLength();
                    }
                }
            }
        }
        array_push($properties->$layerName->metadata[$properties->$layerName->numelements], $dimension);
        array_push($properties->$layerName->metadata[$properties->$layerName->numelements], $bbox);
        array_push($properties->$layerName->metadata[$properties->$layerName->numelements], $center);
        array_push($properties->$layerName->metadata[$properties->$layerName->numelements], $area);
        array_push($properties->$layerName->metadata[$properties->$layerName->numelements], $length);


        for($j=0; $j<$numproperties; $j++)
        {
            $propname = $properties->$layerName->propertynames[$j];
            $value =  GetPropertyValueFromFeatReader($featureReader,
                                                     $properties->$layerName->propertytypes[$j],
                                                     $propname);
            $value = htmlentities($value);
            $value = addslashes($value);
            $value = preg_replace( "/\r?\n/", "<br>", $value );
            $aTmp = $properties->$layerName->values[$properties->$layerName->numelements];
            array_push($properties->$layerName->values[$properties->$layerName->numelements], $value);
            //$properties->$layerName->values[$properties->$layerName->numelements][$propname] = $value;
        }
        $properties->$layerName->numelements= $properties->$layerName->numelements+1;
    }
    return $properties;
}
function getUtmWkt($lon, $lat)
{
    /** WGS 84 / Auto UTM **/
    $zone = floor( ($lon + 180.0) / 6.0 ) + 1;

    //WGS84 AUTO UTM
    $epsg42001 = "PROJCS[\"WGS 84 / Auto UTM\",GEOGCS[\"WGS 84\",DATUM[\"WGS_1984\",SPHEROID[\"WGS_1984\",6378137,298.257223563]],PRIMEM[\"Greenwich\",0],UNIT[\"Decimal_Degree\",0.0174532925199433]],PROJECTION[\"Transverse_Mercator\"],PARAMETER[\"central_meridian\",%.16f],PARAMETER[\"latitude_of_origin\",0],PARAMETER[\"scale_factor\",0.9996],PARAMETER[\"false_easting\",500000],PARAMETER[\"false_northing\",%.16f],UNIT[\"Meter\",1]]";

    //WGS 84 AUTO TRANSVERSE MERCATOR
    $epsg42002 = "PROJCS[\"WGS 84 / Auto Tr. Mercator\",GEOGCS[\"WGS 84\",DATUM[\"WGS_1984\",SPHEROID[\"WGS_1984\",6378137,298.257223563]],PRIMEM[\"Greenwich\",0],UNIT[\"Decimal_Degree\",0.0174532925199433]],PROJECTION[\"Transverse_Mercator\"],PARAMETER[\"central_meridian\",%.16f],PARAMETER[\"latitude_of_origin\",0],PARAMETER[\"scale_factor\",0.9996],PARAMETER[\"false_easting\",500000],PARAMETER[\"false_northing\",%.16f],UNIT[\"Meter\",1]]";

    //WGS 84 AUTO ORTHOGRAHPIC
    $epsg42003 = "PROJCS[\"WGS 84 / Auto Orthographic\",GEOGCS[\"WGS 84\",DATUM[\"WGS_1984\",SPHEROID[\"WGS_1984\",6378137,298.257223563]],PRIMEM[\"Greenwich\",0],UNIT[\"Decimal_Degree\",0.0174532925199433]],PROJECTION[\"Orthographic\"],PARAMETER[\"central_meridian\",%.16f],PARAMETER[\"latitude_of_origin\",%.16f],UNIT[\"Meter\",1]]";

    //WGS 84 AUTO EQUIRECTANGULAR
    $epsg42004 = "PROJCS[\"WGS 84 / Auto Equirectangular\",GEOGCS[\"WGS 84\",DATUM[\"WGS_1984\",SPHEROID[\"WGS_1984\",6378137,298.257223563]],PRIMEM[\"Greenwich\",0],UNIT[\"Decimal_Degree\",0.0174532925199433]],PROJECTION[\"Equirectangular\"],PARAMETER[\"central_meridian\",0],PARAMETER[\"latitude_of_origin\",0],PARAMETER[\"standard_parallel_1\",%.16f],UNIT[\"Meter\",1]]";


    //$wkt = sprintf( $epsg42001, -183.0 + $zone * 6.0, ($lat >= 0.0) ? 0.0 : 10000000.0 );
    //$wkt = sprintf( $epsg42002, $lon, ($lat >= 0.0) ? 0.0 : 10000000.0 );
    $wkt = sprintf( $epsg42003, $lon, $lat);
    //$wkt = sprintf( $epsg42004, $lat);
    return $wkt;
}
?>

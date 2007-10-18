<?php
/**
 * AttributeQuery
 *
 * $Id: AttributeQuery.php 963 2007-10-16 15:37:30Z madair $
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
 *          optionally a spatial filter
 *****************************************************************************/

try {
    /* set up the session */
    include ("Common.php");
    include ("Utilities.php");
    
    /* join properties */
    $joinLayer = isset($_REQUEST['joinlayer']) ? $_REQUEST['joinlayer'] : false;
    $joinForeignKey = isset($_REQUEST['joinfk']) ? $_REQUEST['joinfk'] : false;
    $joinPrimaryKey = isset($_REQUEST['joinpk']) ? $_REQUEST['joinpk'] : false;

    /* limit number of results */
    $resultLimit = isset($_REQUEST['limit']) ? $_REQUEST['limit'] : 0;
    
    /* the name of the layer in the map to query */
    $layer = $_REQUEST['layer'];

    /* a filter expression to apply, in the form of an FDO SQL statement */
    $filter = isset($_REQUEST['filter']) ? html_entity_decode(urldecode($_REQUEST['filter'])) : false;
    /* passing a % in the URL is a 'BAD THING' (tm) - we pass a * instead */
    $filter = str_replace('*', '%', $filter);

    /* we need a feature service to query the features */
    $featureService = $siteConnection->CreateService(MgServiceType::FeatureService);

    /* open the map from the session using the provided map name.  The map was
       previously created by calling LoadMap. */
    $map = new MgMap();
    $map->Open($resourceService, $mapName);

    /* set up the objects required to search the requested layer */
    $layerObj = new MgLayer(new MgResourceIdentifier($layer), $resourceService);
    
    if ($layerObj == null) {
        echo "layer $layer not found.";
        exit;
    }
    $mappings = GetLayerPropertyMappings($resourceService,$layerObj);
    $reverseMappings = array_flip($mappings);
    
    /* set up the objects required by the join, if required */
    $joinLayerObj = null;
    $joinMappings = null;
    if ($joinLayer != false) {
        $joinLayerObj = new MgLayer(new MgResourceIdentifier($joinLayer), $resourceService);
        $joinMappings = GetLayerPropertyMappings($resourceService,$joinLayerObj);
        $joinReverseMappings = array_flip($joinMappings);
        /* grab the real property name of the joined field */
        $primaryLinkProperty = $reverseMappings[$joinPrimaryKey];
        $foreignLinkProperty = $joinReverseMappings[$joinForeignKey];

        /* remove join keys from both child and parent */
        if (array_key_exists($joinForeignKey, $joinReverseMappings)) {
            unset($joinMappings[$joinReverseMappings[$joinForeignKey]]);
            unset($joinReverseMappings[$joinForeignKey]);
        }
        
        if (array_key_exists($joinPrimaryKey, $reverseMappings)) {
            unset($mappings[$reverseMappings[$joinPrimaryKey]]);
            unset($reverseMappings[$joinPrimaryKey]);
        }
        
    }
    
    /* get the feature source from the layer */
    $featureResId = new MgResourceIdentifier($layerObj->GetFeatureSourceId());
    $featureGeometryName = $layerObj->GetFeatureGeometryName();
    /* the class that is used for this layer will be used to select features */
    $class = $layerObj->GetFeatureClassName();
    
    /* add the attribute query if provided */
    $queryOptions = new MgFeatureQueryOptions();
    if ($filter !== false) {
        $queryOptions->SetFilter($filter);
    }

    /* select the features */
    $featureReader = $featureService->SelectFeatures($featureResId, $class, $queryOptions);
    /* figure out if the feature source has geometry in the non-joined case */
    $classDefn = $featureReader->GetClassDefinition();
    $hasGeom = ($classDefn->GetDefaultGeometryPropertyName() != '') ? 1 : 0;
        
    $primaryValues = array();
    $foreignValues = array();
    $geometries = array();
    $joinValues = array();
    
    /* output the attributes from the main feature source.  We take some
     * pains in the join case to build up a filter for the joined feature
     * source so we can go get additional attributes and check for geometry
     * there if necessary
     */
    $filterExpr = array();
    $linkValues = array();
    $index = 0;
    while ($featureReader->ReadNext()) {
        /* put the has_geometry property in first - it may be calculated later
         * if the layer is joined, but we still need a placeholder
         */
        $geometries[$index] = ($joinLayerObj != null) ? 0 : $hasGeom;
        
        /* stuff all the mapped properties in */
        foreach($mappings as $key => $mapped) {
            $value = GetPropertyValueFromFeatReader($featureReader, $key);
            //clean up the values to make them safe for transmitting to the client
            $value = htmlentities($value);
            $value = addslashes($value);
            $value = preg_replace( "/\r?\n/", "<br>", $value );
            $primaryValues[$mapped][$index] = $value;
        }
        /* fill in the joined property values in case we don't get a value later */
        foreach($joinMappings as $key => $mapped) {
            $foreignValues[$key][$index] = '';
        }
        
        /* if we are joining to another feature source, we aren't finished yet */
        if ($joinLayerObj != null) {
            $linkValue = GetPropertyValueFromFeatReader($featureReader, $primaryLinkProperty);
            array_push($joinValues, $linkValue);
            array_push($filterExpr, $foreignLinkProperty."=".$linkValue);
            if (!array_key_exists($linkValue,$linkValues)) {
                $linkValues[$linkValue] = array();
            }
            array_push($linkValues[$linkValue], $index);
        }
        $index ++;
    }
    $featureReader->Close();
    if (count($filterExpr) > 0) {
        $filter = '(' . implode(') OR (', $filterExpr).')';
        $parentOptions = new MgFeatureQueryOptions();
        $parentOptions->SetFilter($filter);
        $joinFeatureResId = new MgResourceIdentifier($joinLayerObj->GetFeatureSourceId());
        $joinClass = $joinLayerObj->GetFeatureClassName();
        $geomReader = $featureService->SelectFeatures($joinFeatureResId, $joinClass, $parentOptions);
        while($geomReader->ReadNext()) {
            $linkValue = GetPropertyValueFromFeatReader($geomReader, $foreignLinkProperty);
            if (isset($linkValues[$linkValue])) {
                foreach($linkValues[$linkValue] as $index) {
                    $geometries[$index] = 1;
                    foreach($joinMappings as $key => $mapped) {
                        $value = GetPropertyValueFromFeatReader($geomReader, $key);
                        $foreignValues[$mapped][$index] = $value;

                    }
                }
            }
        }
    }
    
    header('Content-type: text/x-json');
    header('X-JSON: true');
    /* insert has_geometry property by default */
    echo "{properties:['".implode("','", array_values($mappings))."','".implode("','", array_values($joinMappings))."'],\n";
    echo "property_keys:['".implode("','", array_keys($mappings))."','".implode("','", array_keys($joinMappings))."'],\n";
    echo "geometries:[".implode(",", $geometries)."],\n";
    echo "join_values:[".implode(",", $joinValues)."],\n";
    $nTotalElements = count($geometries);
    echo "total_count:$nTotalElements,\n";
    echo "values:[";
    $sep = '';
    $nElements = ($resultLimit > 0)?min($resultLimit, $nTotalElements):$nTotalElements;
    echo "/*$resultLimit   $nElements*/";
    for ($i=0; $i<$nElements; $i++) {
        echo $sep."[";
        $vSep = '';
        foreach($primaryValues as $key => $values) {
            if (array_key_exists($i, $values)) {
               echo $vSep."'".$values[$i]."'";
            } else {
               echo $vSep."''";
            }
            $vSep = ',';
        }
        foreach($foreignValues as $key => $values) {
            if (array_key_exists($i, $values)) {
               echo $vSep."'".$values[$i]."'";
            } else {
               echo $vSep."''";
            }
            $vSep = ',';
        }
        echo "]";
        $sep = ',';
    }
    echo "]};";
}
catch (MgException $e)
{
  echo "ERROR: " . $e->GetMessage() . "\n";
  echo $e->GetDetails() . "\n";
  echo $e->GetStackTrace() . "\n";
}

function GetPropertyValueFromFeatReader($featureReader, $propertyName)
{
    $val = "";
    $propertyType = $featureReader->GetPropertyType($propertyName);
    switch ($propertyType)
    {
       case MgPropertyType::Null :
         //fwrite($logFileHandle, "$propertyName is a null propertyn");
         $val= "";
         break;
       case MgPropertyType::Boolean :
         $val = booleanToString($featureReader->GetBoolean($propertyName));
         //$valStr = printBoolean($val);
         break;
       case MgPropertyType::Byte :
         $val = $featureReader->GetByte($propertyName);
         break;
       case MgPropertyType::DateTime :
         //$val = dateTimeToString($featureReader->GetDateTime($propertyName));
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

 function booleanToString($b) {
     $result = '';
     if (is_object($b)) {
         $result = ($b->GetValue()) ? 'true' : 'false';
     }
     return result;
 }

 function dateTimeToString($dt) {
     $result = '';
     if (is_object($dt)) {
         if ($dt->isDateTime()) {
             $result = dateTimeToDateString($dt) . ' ' . dateTimeToTimeString($dt);
         } else if ($dt->isTime()) {
             $result = dateTimeToTimeString($dt);
         } else if ($dt->isDate()) {
             $result = dateTimeToDateString($dt);
         }
     }
     return $result;
 }

function dateTimeToTimeString($dt) {
    $result = '';
    if ($dt->isTime() || $dt->isDateTime()) {
        //$result = $dt->GetHour() . ':' . $dt->GetMinute() . ':' . $dt->GetSecond() . '.' . $dt->GetMicrosecond();
    }
    return $result;
}

function dateTimeToDateString($dt) {
    $result = '';
    if ($dt->isDate() || $dt->isDateTime()) {
        $result = $dt->GetDay() . ' ' . $dt->GetMonth() . ' ' . $dt->GetYear();
    }
    return $result;
}
?>

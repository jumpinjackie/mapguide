<?php
/*****************************************************************************
 *
 * $Id: Query.php 834 2007-09-19 19:55:50Z madair $
 *
 * Purpose: create a new selection based on one or more attribute filters and
 *          a spatial filter
 *
 * Project: MapGuide Open Source GMap demo application
 *
 * Author: DM Solutions Group Inc
 *
 * Copyright (c) 2007 DM Solutions Group Inc.
 *****************************************************************************
 * This code shall not be copied or used without the expressed written consent
 * of DM Solutions Group Inc.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 * 
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *****************************************************************************/

try {
    /* set up the session */
    include ("Common.php");
    include('../../common/php/Utilities.php');

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

    /* add the attribute query if provided */
    $queryOptions = new MgFeatureQueryOptions();
    if ($filter !== false) {
        //echo "<!-- setting filter $filter -->\n";
        $queryOptions->SetFilter($filter);
    }

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

            if ($spatialFilter !== false ) {
                $queryOptions->SetSpatialFilter($featureGeometryName, $geom, $variant);
            }
            
            /* the class that is used for this layer will be used to select
               features */
            $class = $layerObj->GetFeatureClassName();

            /* select the features */
            $featureReader = $featureService->SelectFeatures($featureResId, $class, $queryOptions);
            if ($bExtendSelection) {
                /* possibly toggle features in the map */
                $newSelection = new MgSelection($map);
                $newSelection->AddFeatures($layerObj, $featureReader, 0);
                $aLayers = selectionToArray($newSelection, $aLayers);
            } else {
                /* add the features to the map */
                $selection->AddFeatures($layerObj, $featureReader, 0);
            }
            
            /* close the feature reader - not doing this could cause problems */
            $featureReader->Close();
            
        } catch (MgObjectNotFoundException $onfe) {
            //skip layers not in the map?
            echo "Object not found";
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
            }
        }
    }
    $selection->Save($resourceService, $mapName);
    
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
?>

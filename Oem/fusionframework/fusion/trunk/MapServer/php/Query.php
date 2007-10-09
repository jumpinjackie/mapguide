<?php
/*****************************************************************************
 *
 * $Id: Query.php 836 2007-09-19 20:09:18Z pspencer $
 *
 * Purpose: create a new selection based on one or more attribute filters and
 *          a spatial filter
 *
 * Project: Fusion MapServer
 *
 * Author: DM Solutions Group Inc
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
 *
 *****************************************************************************/

/* set up the session */
include ("Common.php");
include ("Utilities.php");
include('../../common/php/Utilities.php');

/* the name of the layer in the map to query */
if ($_REQUEST['layers'] != '') {
    $layers = explode(',',$_REQUEST['layers']);
} else {
    $layers = array();
}

//echo "<!--";
//print_r($_REQUEST);
//echo "-->";

/* selection variant if set */
$variant = 'intersects';
if (isset($_REQUEST['variant'])) {
    if (strcasecmp($_REQUEST['variant'],'contains') == 0) {
        $variant = 'contains';
    } else if (strcasecmp($_REQUEST['variant'],'inside') == 0) {
        $variant = 'inside';
    }
}
/* a filter expression to apply, in the form of an FDO SQL statement */
$filter = isset($_REQUEST['filter']) ? str_replace(array('*', '"'), array('%', "'"),html_entity_decode(urldecode( $_REQUEST['filter']))) : false;
//echo "<!-- filter: $filter -->\n";
/* a spatial filter in the form on a WKT geometry */
$spatialFilter = (isset($_REQUEST['spatialfilter']) && $_REQUEST['spatialfilter'] != '') ? urldecode($_REQUEST['spatialfilter']) : false;
//echo "spatial filter is $spatialFilter<BR>";

if (!isset($mapName)) {
    die('mapname not set');
}
if (isset($_SESSION['maps']) && isset($_SESSION['maps'][$mapName])) {
    $oMap = ms_newMapObj($_SESSION['maps'][$mapName]);
}

/* add the spatial filter if provided.  It is expected to come as a
   WKT string, so we need to convert it to a shape */
if ($spatialFilter !== false ) {
    $oSpatialFilter = ms_shapeObjFromWkt($spatialFilter);
}

/* if extending the current selection */
$bExtendSelection = isset($_REQUEST['extendselection']) && strcasecmp($_REQUEST['extendselection'], 'true') == 0;
if ($bExtendSelection) {
    //TODO figure out how to load an existing selection here
    $oMap->loadquery(getSessionSavePath()."query.qry");
}

$bAllLayers = false;
$nLayers = count($layers);
$nSelections = 0;
if ($nLayers == 0) {
    $nLayers = $oMap->numlayers;
    $bAllLayers = true;
}

$result = NULL;
$result->hasSelection = false;
$result->layers = array();
for ($i=0; $i<$nLayers; $i++) {
    if (!$bAllLayers) {
        $oLayer = $oMap->GetLayerByName($layers[$i]);
    } else {
        $oLayer = $oMap->GetLayer($i);
    }
    $oLayer->set('tolerance', 0);
    if (@$oLayer->queryByShape($oSpatialFilter) == MS_SUCCESS) {
        $result->hasSelection = true;
        $layerName = $oLayer->name;
        array_push($result->layers, $layerName);
        $result->$layerName->featureCount = $oLayer->getNumResults();
        //TODO: dump out the extents of the selection
    }

    if ($bExtendSelection) {
    } else {
    }
}
if ($bExtendSelection) {
}

header('Content-type: text/x-json');
header('X-JSON: true');
if ($result->hasSelection) {
    $oMap->savequery(getSessionSavePath()."query.qry");
    $result->queryFile = getSessionSavePath()."query.qry";
}

echo var2json($result);

?>

<?php
/**
 * SetLayers
 *
 * $Id: Selection.php 963 2007-10-16 15:37:30Z madair $
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
 * Purpose: Set the layers that are in the current Map in the session
 *****************************************************************************/

/* set up the session */
include ("Common.php");
include ("Utilities.php");


if (isset($_SESSION['maps']) && isset($_SESSION['maps'][$mapName])) {
    $oMap = ms_newMapObj($_SESSION['maps'][$mapName]);
}

/* the name of the layer in the map to query */
if ($_REQUEST['layerindex'] != '') {
	$layers = explode(',',$_REQUEST['layerindex']);
} else {
	$layers = array();
}
$currentLayers = $oMap->getlayersdrawingorder();

if (count($currentLayers) != count($layers) ) {
	for ($i=0; $i<count($layers); $i++) {
		if ($i != $layers[$i]) {
			$layer = $oMap->getLayer($i);
			$tmp = $layer->set('status', 'MS_DELETE');
			if ($tmp >= 0) {
				$res = true;
			} else {
				$res = false;
			}
			break;
		}
		$res = false;
	}
} else {
	$res = $oMap->setlayersdrawingorder($layers);
}

header('Content-type: text/x-json');
header('X-JSON: true');
if ($res) {
	$oMap->save($_SESSION['maps'][$mapName]);
	$currentLayers = $oMap->getlayersdrawingorder();
    echo "{success: true, layerindex: [".implode(",",$currentLayers)."]}";
} else {
    echo "{success: false, layerindex: [".$_REQUEST['layerindex']."]}";
}

?>

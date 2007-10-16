<?php
/*****************************************************************************
 *
 * $Id: $
 *
 * Purpose: Measure a feature
 *
 * Project: Fusion
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


include ('Common.php');
include ('Utilities.php');

try {
    if (!isset($_REQUEST['session']) || 
        !isset($_REQUEST['mapname']) ||
        !isset($_REQUEST['x1']) || 
        !isset($_REQUEST['y1']) || 
        !isset($_REQUEST['x2']) || 
        !isset($_REQUEST['y2'])) {
        echo "<Error>Arguments missing </Error>";
        exit;
    }
    $x1 = $_REQUEST['x1'];
    $y1 = $_REQUEST['y1'];
    $x2 = $_REQUEST['x2'];
    $y2 = $_REQUEST['y2'];
    $map = new MgMap();
    $map->Open($resourceService, $mapName);
    $srsFactory = new MgCoordinateSystemFactory();
    $srs = GetMapSRS($map);
    $srsMap = $srsFactory->Create($srs);
    $srsType = $srsMap->GetType();
    if ($srsType == MgCoordinateSystemType::Geographic) {
        $distance = $srsMap->MeasureGreatCircleDistance($x1, $y1, $x2, $y2);
    } else {
        $distance = $srsMap->MeasureEuclideanDistance($x1, $y1, $x2, $y2);
    }
    $distance = $srsMap->ConvertCoordinateSystemUnitsToMeters($distance);   
    header('Content-type: text/x-json');
    header('X-JSON: true');
    echo "{distance:$distance}";
    exit;
} catch (MgException $e) {
    echo "last error";
    echo "ERROR: " . $e->GetMessage() . "\n";
    echo $e->GetDetails() . "\n";
    echo $e->GetStackTrace() . "\n";
}
?>
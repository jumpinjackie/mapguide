<?php
/*****************************************************************************
 *
 * $Id: $
 *
 * Purpose: Measure a line segment and return the distance
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
    if (isset($_SESSION['maps']) && isset($_SESSION['maps'][$mapName])) {
        $oMap = ms_newMapObj($_SESSION['maps'][$mapName]);
    }
    $shapeObj = ms_newShapeObj(MS_SHAPE_LINE);
    $lineObj = ms_newLineObj();
    $lineObj->addXY($x1,$y1);
    $lineObj->addXY($x2,$y2);
    $shapeObj->add($lineObj);
    $distance = $shapeObj->getLength($shapeObj);
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
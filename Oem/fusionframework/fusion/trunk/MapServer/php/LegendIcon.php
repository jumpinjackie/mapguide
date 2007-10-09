<?php
/*****************************************************************************
 * $Id: LegendIcon.php 593 2007-05-09 20:49:46Z pspencer $
 * Purpose: Draw a legend icon 
 * Project: Fusion
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
 *****************************************************************************/
include(dirname(__FILE__).'/Common.php');

if (!isset($mapName)) {
    die('mapname not set');
}
if (isset($_SESSION['maps']) && isset($_SESSION['maps'][$mapName])) {
    $oMap = ms_newMapObj($_SESSION['maps'][$mapName]);
    $oLayer = $oMap->getLayerByName($REQUEST_VARS['layername']);
    $oClass = $oLayer->getClass($REQUEST_VARS['classindex']);
    /* TODO: should size be configurable? */
    $oImg = $oClass->createLegendIcon(16,16);
    /* TODO: can we figure out what the content type is? */
    header('Content-type: image/png');
    $oImg->saveImage("");
    $oImg->free();
}
?>
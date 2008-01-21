<?php
/**
 * LegendIcon
 *
 * $Id: LegendIcon.php 1063 2007-11-29 22:55:04Z assefa $
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
 * Purpose: Draw a legend icon 
 *****************************************************************************/

include(dirname(__FILE__).'/Common.php');

if (!isset($mapName)) {
    die('mapname not set');
}
if (isset($_SESSION['maps']) && isset($_SESSION['maps'][$mapName])) {
    $oMap = ms_newMapObj($_SESSION['maps'][$mapName]);
    $oLayer = $oMap->getLayerByName($REQUEST_VARS['layername']);
    $oClass = $oLayer->getClass($REQUEST_VARS['classindex']);
    $width = $oMap->legend->keysizex;
    $height = $oMap->legend->keysizey;
    if ($width <=0)
      $width = 16;
    if ($height <=0)
      $height = 16;
    
    $oImg = $oClass->createLegendIcon($width, $height);
    /* TODO: can we figure out what the content type is? */
    header('Content-type: image/png');
    $oImg->saveImage("");
    $oImg->free();
}
?>

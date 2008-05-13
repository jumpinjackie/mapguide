<?php
/**
 * LegendIcon
 *
 * $Id: LegendIcon.php 1396 2008-05-08 15:34:30Z madair $
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

$legendIconCacheFile = "";

if (isset($_SESSION['maps']) && isset($_SESSION['maps'][$mapName])) {
    /* json decode only in PHP 5.2 and later */
    if (function_exists('json_decode')) {
        $configFile = realpath(dirname(__FILE__)."/../../config.json");
        if (file_exists($configFile)) {
            $configStr = file_get_contents($configFile);
            /* replace single quotes with double quotes */
            $configStr = str_replace("'", '"', $configStr);
            /* get rid of new lines, it just complicates things */
            $configStr = str_replace("\n", '', $configStr);
            /* get rid of embedded comments */
            $configStr = preg_replace("/\/\*.*\*\//U", "", $configStr);
            /* the regex after this one can't handle http: as a value, so mangle it. */
            $configStr = preg_replace("/http:/U", "http_", $configStr);
            /* quote unquoted attribute names */
            $configStr = preg_replace("/[^\"]{1}(\w*):/U", "\"$1\":", $configStr);
            /* decode the whole thing */
            $configObj = json_decode($configStr, false);
            /* if the legendIconCache dir is set */
            if (isset($configObj->mapserver->legendIconCacheDir)) {
              $legendIconCacheDir = $configObj->mapserver->legendIconCacheDir;

              // check for closing '/'
              $legendIconCacheDir = str_replace( '\\', '/', trim( $legendIconCacheDir ) );
              if ( substr( $legendIconCacheDir, -1 ) != '/' )
              {
                  $legendIconCacheDir .= '/';
              }

              $cacheLegendIcons = true;
              $str = file_get_contents($_SESSION['maps'][$mapName]);
              /* create a unique location for the map icons based on
               * the content of the of map file.  If the content changes
               * then the icons should be rebuilt anyway
               */
              $legendIconCacheDir = $legendIconCacheDir.md5($str)."/";
              if (!is_dir($legendIconCacheDir)) {
                mkdir($legendIconCacheDir);
              }
              /* TODO: can we figure out what the content type is? */
              $legendIconCacheFile = $legendIconCacheDir."_".$REQUEST_VARS['layername']."_".$REQUEST_VARS['classindex'].".png";
              /* if the icon exists, return it */
              if (file_exists($legendIconCacheFile)) {
                  /* TODO: can we figure out what the content type is? */
                  header('Content-type: image/png');
                  readfile($legendIconCacheFile);
                  exit;
              }
            }
        }
    }

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
    if ($cacheLegendIcons) {
        $oImg->saveImage($legendIconCacheFile);
        readfile($legendIconCacheFile);
    } else {
      $oImg->saveImage("");
    }
    $oImg->free();
}
?>

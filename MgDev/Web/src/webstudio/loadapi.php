<?php
/**
 * @project         MapGuide Open Source Web Studio
 * @revision        $Id: loadapi.php,v 1.2 2006/06/12 06:03:40 pspencer Exp $
 * @fileoverview    this file compresses multiple javascript files into a single file
 * @author          Paul Spencer (pspencer@dmsolutions.ca)
 * @copyright       &copy; 2006 DM Solutions Group Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of version 2.1 of the GNU Lesser
 * General Public License as published by the Free Software Foundation.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
$files = array(//"js/scriptaculous/lib/prototype.js",
               // "js/scriptaculous/src/scriptaculous.js",
//                "js/scriptaculous/src/builder.js",
//                "js/scriptaculous/src/controls.js",
//                "js/scriptaculous/src/dragdrop.js",
//                "js/scriptaculous/src/effect.js",
//                "js/scriptaculous/src/slider.js",
               "js/jx.js",
               "js/WSApplication.js",
               "js/WSBroker.js",
               "js/WSFactory.js",
               "js/WSView.js",
               "js/WSDialog.js",
               "js/WSUtility.js");

ob_start ("ob_gzhandler");
header("Content-type: text/javascript; charset: UTF-8");
header("Cache-Control: must-revalidate");
$days = 30;
$offset = 60 * 60 * 24 * $days; 
$ExpStr = "Expires: " . gmdate("D, d M Y H:i:s", time() + $offset) . " GMT";
header($ExpStr);

foreach($files as $file) {
    output_js_file($file);
}
exit;

function output_js_file( $file ) {
    echo file_get_contents($file);
    return;
    
    //todo: figure out why this doesn't work!
    $szContents = file_get_contents($file);
    $aSearch = array('/\/\/.*/', // c++ style comments - //something
                     '/\/\*.*\*\//sU', // c style comments - /* something */
                     '/\s{2,}/s', //2 or more spaces down to one space
                     '/\n\n/', //newlines removed
                     '/\s=/', //space =
                     '/=\s/', // = space
                     );
    
    $aReplace = array( '',
                       '',
                       ' ',
                       '',
                       '=',
                       '=',
                      );
    $szResult = preg_replace( $aSearch, $aReplace, $szContents );
    echo $szResult;
}
?>

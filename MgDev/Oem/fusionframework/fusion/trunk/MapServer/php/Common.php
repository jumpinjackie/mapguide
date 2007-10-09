<?php
/*****************************************************************************
 * $Id: Common.php 688 2007-07-17 15:43:55Z zak $
 * Purpose: Common include file for PHP scripts implementing a server-side 
 *          component for MapServer
 * Project: Fusion
 * Author:  DM Solutions Group Inc
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
 *****************************************************************************
 *
 * Developer's notes:
 *
 * This file should be included by all server-side scripts.  It includes some
 * default paths to install locations and manages all session-related and 
 * user-related stuff except for actually creating a session.
 * 
 * Configuration: you may edit the following section if necessary
 */

$szPHPMapScriptModule = 'php_mapscript.'.PHP_SHLIB_SUFFIX;
$szGDModule = 'php_gd2.'.PHP_SHLIB_SUFFIX;

/*
 * End of configurable items.
 *
 * The remainder of this file just sets up a common environment for server-side
 * scripts.
 */
include(dirname(__FILE__).'/Session.php');
installSessionDirectoryHandler();

/* merge request vars.  $_REQUEST includes cookies in preference to get/post,
 * which we don't like */
$REQUEST_VARS = array_merge($_GET, $_POST);
 
/* Load mapscript */
if (!extension_loaded('MapScript')) {
    dl( $szPHPMapScriptModule );
    if (!extension_loaded('MapScript')) {
        die("Failed to load $szPHPMapScriptModule.  Please check configuration settings in ".__FILE__);
    }
}

/* handle restoring a session */
if (isset($REQUEST_VARS['session'])) {
    $sessionID = $REQUEST_VARS['session'];
    initializeSession( "sid", "", $sessionID );
}

if (isset($_REQUEST['mapname'])) { 
    $mapName = $_REQUEST['mapname'];
}

?>
<?php
//  Copyright (C) 2004-2010 by Autodesk, Inc.
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of version 2.1 of the GNU Lesser
//  General Public License as published by the Free Software Foundation.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

// Define some commonly used directories and files

// -----------------------------------------------------------------------------------
// Use the following for Windows installations
// -----------------------------------------------------------------------------------
$webExtensionsDirectory = 'C:\Program Files\MapGuideOpenSource\WebServerExtensions\\';
  
$MapGuideServerDirectory = 'C:\Program Files\MapGuideOpenSource\Server\\';
  
$viewerFilesDirectory = $webExtensionsDirectory . 'www\viewerfiles\\';
  
$schemaDirectory = $MapGuideServerDirectory . 'Schema\\';
  
$webconfigDirectory = $webExtensionsDirectory . 'www\\';

$webconfigFilePath = $webconfigDirectory . 'webconfig.ini';
// -----------------------------------------------------------------------------------
// End of Windows configuration
// -----------------------------------------------------------------------------------

// -----------------------------------------------------------------------------------
// Use the following for Linux installations
// -----------------------------------------------------------------------------------
/************************************************************************************* 
$webExtensionsDirectory = '/usr/local/mapguideopensource/webserverextensions/';
  
$MapGuideServerDirectory = '/usr/local/mapguideopensource/server/';
  
$viewerFilesDirectory = $webExtensionsDirectory . 'www/viewerfiles/';
  
$schemaDirectory = $MapGuideServerDirectory . 'Schema/';
  
$webconfigDirectory = $webExtensionsDirectory . 'www/';

$webconfigFilePath = $webconfigDirectory . 'webconfig.ini';
*************************************************************************************/
// -----------------------------------------------------------------------------------
// End of Linux configuration
// -----------------------------------------------------------------------------------

// Include constants like MgServiceType::ResourceService
require_once($webExtensionsDirectory . 'www/mapviewerphp/constants.php');	

?>
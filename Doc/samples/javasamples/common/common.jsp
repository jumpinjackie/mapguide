<%
//  Copyright (C) 2004-2006  Autodesk, Inc.
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
String webExtensionsDirectory = "C:\\Program Files\\Autodesk\\MapGuideEnterprise2011\\WebServerExtensions\\";
  
String MapGuideServerDirectory = "C:\\Program Files\\Autodesk\\MapGuideEnterprise2011\\Server\\";
  
String viewerFilesDirectory = webExtensionsDirectory + "www\\viewerfiles\\";
  
String schemaDirectory = MapGuideServerDirectory + "Schema\\";
  
String webconfigDirectory = webExtensionsDirectory + "www\\";

String webconfigFilePath = webconfigDirectory + "webconfig.ini";
// -----------------------------------------------------------------------------------
// End of Windows configuration
// -----------------------------------------------------------------------------------

// -----------------------------------------------------------------------------------
// Use the following for Linux installations
// -----------------------------------------------------------------------------------
/************************************************************************************* 
String webExtensionsDirectory = "/opt/Autodesk/mapguideenterprise2011/webserverextensions/";
  
String MapGuideServerDirectory = "/opt/Autodesk/mapguideenterprise2011/server/";
  
String viewerFilesDirectory = webExtensionsDirectory + "www/viewerfiles/";
  
String schemaDirectory = MapGuideServerDirectory + "Schema/";
  
String webconfigDirectory = webExtensionsDirectory + "www/";

String webconfigFilePath = webconfigDirectory + "webconfig.ini";
*************************************************************************************/
// -----------------------------------------------------------------------------------
// End of Linux configuration
// -----------------------------------------------------------------------------------

%>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<!--
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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
-->
<html>
  <head>
    <meta content="text/html; charset=utf-8" http-equiv="Content-Type">
    <title>Clear Lines</title>
        
    <meta http-equiv="content-style-type" content="text/css">
    <link href="../styles/globalStyles.css" rel="stylesheet"  type="text/css">
    <link href="../styles/otherStyles.css" rel="stylesheet" type="text/css">
    <meta http-equiv="content-script-type" content="text/javascript"> 
    <script language="javascript" type="text/javascript">
      function OnPageLoad()
      {
        parent.mapFrame.Refresh();
      }
    </script>
  </head>

  <body onLoad="OnPageLoad()">
    <?php 
      require_once('../common/common.php');  
      try
      {
        // --------------------------------------------------//
        // Basic initialization
        MgInitializeWebTier($webconfigFilePath);
        
        // Get the information passed from the viewer
        $sessionId = ($_SERVER['REQUEST_METHOD'] == "POST")?$_POST['SESSION']: $_GET['SESSION'];
        $mapName = ($_SERVER['REQUEST_METHOD'] == "POST")?$_POST['MAPNAME']: $_GET['MAPNAME'];
           
        // Get the user information using the session id
        // and set up a connection to the site server  
        $userInfo = new MgUserInformation($sessionId);
        $siteConnection = new MgSiteConnection();
        $siteConnection->Open($userInfo);
        
        // Get an instance of the required service(s)
        $resourceService = $siteConnection->CreateService(MgServiceType::ResourceService);    
      
        // --------------------------------------------------//   
        $featureSourceName = "Session:$sessionId//TemporaryLines.FeatureSource"; 
        $resourceID = new MgResourceIdentifier($featureSourceName);
        
        $resourceService->DeleteResource($resourceID);
      }
      catch (MgException $e)
      {
        echo "<script language=\"javascript\" type=\"text/javascript\"> \n";
        echo "    alert(\" " . $e->GetMessage() . " \"); \n";
        echo "</script> \n";
      }   
    ?>
  </body>
</html>
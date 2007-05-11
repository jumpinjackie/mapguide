<?php

//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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
//

?>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>

	<head>
	    <title>Displays the schema</title>
	    <link href="displayschema.css" rel="stylesheet" type="text/css">
	</head>

	<body onLoad="Confirmation()">

		<?php
		    include '../mapadmin/Constants.php';
		    include 'displayschemafunctions.php';
		    include 'layerdefinitionfactory.php';
		    include 'mapdefinitionfactory.php';
		    include 'weblayoutfactory.php';

		    $sessionId = $_GET['sessionId'];
		    $resName = $_GET['resId'];
		    $schemaName = $_GET['schemaName'];
		    $className = $_GET['className'];
		    $geomName = $_GET['geomname'];
		    $geomType = $_GET['geomtype'];
		    $totalEntries = 0;
		    $firstTime = true;
		    $validSession = 1;

			try
		    {
				$thisFile = __FILE__;
				$pos = strrpos($thisFile, '\\');
				if ($pos == false)
					$pos = strrpos($thisFile, '/');
				$configFilePath = substr($thisFile, 0, $pos+1) . "../webconfig.ini";
		        MgInitializeWebTier ($configFilePath);

				$userInfo = new MgUserInformation($sessionId);
		        $site = new MgSiteConnection();
		        $site->Open($userInfo);

				$featureSrvc = $site->CreateService(MgServiceType::FeatureService);
				$resourceSrvc = $site->CreateService(MgServiceType::ResourceService);
				$featuresId = new MgResourceIdentifier($resName);

				$schemaName = substr(strrchr($schemaName, "/"), 1);
				$featureName = $schemaName . ':' . $className;

				$featureReader = $featureSrvc->SelectFeatures($featuresId, $className, null);
				while($featureReader->ReadNext())
					$totalEntries++;

				// Create a layer definition
				$layerfactory = new LayerDefinitionFactory();
				$layerDefinition = CreateLayerDef($layerfactory, $resName, $featureName, $geomName, $geomType);

				// Save the layer definition to a resource stored in the session repository
				$byteSource = new MgByteSource($layerDefinition, strlen($layerDefinition));
				$byteSource->SetMimeType(MgMimeType::Xml);
				$resName = 'Session:' . $sessionId . '//' . $className . '.LayerDefinition';
				$resId = new MgResourceIdentifier($resName);
				$resourceSrvc->SetResource($resId, $byteSource->GetReader(), null);

				// Finds the coordinate system
				$agfReaderWriter = new MgAgfReaderWriter();
				$spatialcontext = $featureSrvc->GetSpatialContexts($featuresId, false);
				$spatialcontext->ReadNext();
				$coordinate = $spatialcontext->GetCoordinateSystemWkt();

				// Finds the extent
				$extentByteReader = $spatialcontext->GetExtent();
				if($extentByteReader->ToString()==null)
				{
					throw new Exception("Extent is null. Cannot display feature.");
				}
		        $extentGeometry = $agfReaderWriter->Read($extentByteReader);
		        $iterator = $extentGeometry->GetCoordinates();
		        while($iterator->MoveNext())
				{
					$x = $iterator->GetCurrent()->GetX();
					$y = $iterator->GetCurrent()->GetY();
					if($firstTime)
					{
						$maxX = $x;
						$minX = $x;
						$maxY = $y;
						$minY = $y;
						$firstTime = false;
					}
					if($maxX<$x)
						$maxX = $x;
					if($minX>$x||$minX==0)
						$minX = $x;
					if($maxY<$y)
						$maxY = $y;
					if($minY>$y||$minY==0)
						$minY = $y;
				}

				// Create a map definition
				$mapfactory = new MapDefinitionFactory();
				$mapDefinition = CreateMapDef($mapfactory, $className, $resName, $coordinate, $minX, $maxX, $minY, $maxY);

				// Save the map definition to a resource stored in the session repository
				$byteSource = new MgByteSource($mapDefinition, strlen($mapDefinition));
				$byteSource->SetMimeType(MgMimeType::Xml);
				$resName = 'Session:' . $sessionId . '//' . $className . '.MapDefinition';
				$resId = new MgResourceIdentifier($resName);
				$resourceSrvc->SetResource($resId, $byteSource->GetReader(), null);

				// Create a web layout
				$webfactory = new WebLayoutFactory();
				$webLayout = CreateWebLay($webfactory, $resName);

				// Save the web layout to a resource stored in the session repository
				$byteSource = new MgByteSource($webLayout, strlen($webLayout));
				$byteSource->SetMimeType(MgMimeType::Xml);
				$resName = 'Session:' . $sessionId . '//' . $className . '.WebLayout';
				$resId = new MgResourceIdentifier($resName);
				$resourceSrvc->SetResource($resId, $byteSource->GetReader(), null);
			}
			catch (MgSessionExpiredException $s)
			{
				$validSession = 0;
				echo "Session has expired. Please Refresh Page.";
			}
		    catch (MgException $mge)
		    {
				$validSession = 0;
		        echo $mge->GetMessage();
		    }
		    catch (Exception $e)
		    {
				$validSession = 0;
		        echo $e->GetMessage();
		    }

		?>

	</body>

	<script language="JavaScript">
		function Confirmation()
		{
			// checks for valid session
			if(<?php echo $validSession ?> > 0)
			{
				if(<?php echo $totalEntries ?> > 1000)
				{
					var answer = confirm("This feature source contains <?php echo $totalEntries ?> features. Would you like to continue?");
					if (answer)
						location = '/mapguide2008/mapviewerajax/?SESSION=<?php echo $sessionId ?>&WEBLAYOUT=<?php echo $resName ?>';
					else
						location = '/mapguide2008/schemareport/blank.php';
				}
				else
					location = '/mapguide2008/mapviewerajax/?SESSION=<?php echo $sessionId ?>&WEBLAYOUT=<?php echo $resName ?>';
			}


		}
	</script>

</html>

<?php

//
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
//

?>
<?php

	$thisFile = __FILE__;
	$pos = strrpos($thisFile, '\\');
	if ($pos == false)
	{
	   $pos = strrpos($thisFile, '/');
	}

	$web_Dir = substr($thisFile, 0, $pos+1);
	$configFilePath = $web_Dir . "../webconfig.ini";

	include $web_Dir . "../mapviewerphp/constants.php";


	// Searches the map for a layer with the specified name.

	function GetXmlPath()
	{

		$thisFile = __FILE__;
		$pos = strrpos($thisFile, '\\');
		if ($pos == false)
		{
		   $pos = strrpos($thisFile, '/');
		}

		$web_Dir = substr($thisFile, 0, $pos+1);

		return $web_Dir. "findaddress/templates/addressmarker.xml";
	}

	$web_Dir = substr($thisFile, 0, $pos+1);
	function GetLayerByName($map, $layerName)
	{
	    $layer = null;
	    for ($i = 0; $i < $map->GetLayers()->GetCount(); $i++)
	    {
	        $nextLayer = $map->GetLayers()->GetItem($i);
	        if ($nextLayer->GetName() == $layerName)
	        {
	            $layer = $nextLayer;
	            break;
	        }
	    }
	    return $layer;
	}

	// Read the content from an MgByteReader and send it back as the response.

	function OutputReaderContent($byteReader)
	{
		$buffer = '';
		$contentLen = 0;
		do
		{
			$data = str_pad("\0", 50000, "\0");
			$len = $byteReader->Read($data, 50000);
			if ($len > 0)
			{
				$contentLen = $contentLen + $len;
				$buffer = $buffer . substr($data, 0, $len);
			}
		} while ($len > 0);

		header('Content-Type: ' . $byteReader->GetMimeType());
		header('Content-Length: ' . $contentLen);
		echo $buffer;

		return;
	}
?>
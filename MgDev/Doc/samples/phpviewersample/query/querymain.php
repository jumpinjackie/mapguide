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

	require_once '../utilityfunctions.php';
    require_once '../JSON.php';
	require_once 'classes/query.php';

	$args = ($_SERVER['REQUEST_METHOD'] == "POST") ? $_POST : $_GET;

	$errorMsg = null;
	$errorDetail = null;

	try
	{
		MgInitializeWebTier($configFilePath);

		$query = new Query($args);

		$layerNames = $query->GetMapLayerNames();
	}
	catch (MgException $mge)
	{
		$errorMsg = $mge->GetMessage();
		$errorDetail = $mge->GetDetails();
		echo $errorMsg;
		echo $errorDetail;
	}
	catch (Exception $e)
	{
		$errorMsg = $e->GetMessage();
		echo $errorMsg;
	}
?>
<html>
<head>
	<title>Query Features</title>
    <link rel="stylesheet" href="../styles/gt.css" type="text/css">
	<script language="javascript" src="../../viewerfiles/browserdetect.js"></script>
	<script language="javascript" src="../json.js"></script>

	<script language="javascript">
		var READY_STATE_UNINITIALIZED 	= 0;
		var READY_STATE_LOADING 		= 1;
		var READY_STATE_LOADED 			= 2;
		var READY_STATE_INTERACTIVE 	= 3;
		var READY_STATE_COMPLETE 		= 4;

		var NOT_BUSY_IMAGE = "../images/loader_inactive.gif";
		var BUSY_IMAGE = "../images/loader_pulse.gif";

        var session = '<?= $args['SESSION'] ?>';
        var mapName = '<?= $args['MAPNAME'] ?>';

		var strOps = '<?php $json = new Services_JSON(); echo $json->encode($query->strOperators) ?>'.parseJSON();
		var numOps = '<?php $json = new Services_JSON(); echo $json->encode($query->numOperators) ?>'.parseJSON();
		var queryReqHandler = null;
		var spatialFilterGeomText = null;
		var properties = null;
		var results;

		function OnLayerChange()
		{
            var layerSelect = document.getElementById("layerSelect");
            var propertySelect = document.getElementById("propertySelect");
            var outputSelect = document.getElementById("outputSelect");

            reqParams = "COMMAND=GETLAYERPROPERTIES";
			reqParams += "&SESSION=" + encodeURIComponent(session);
            reqParams += "&MAPNAME=" + encodeURIComponent(mapName);
			reqParams += "&LAYERNAME=" + encodeURIComponent(layerSelect.value);

            if (msie)
                reqHandler = new ActiveXObject("Microsoft.XMLHTTP");
            else
                reqHandler = new XMLHttpRequest();

            reqHandler.open("POST", "querycontroller.php", false);
            reqHandler.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");

			reqHandler.send(reqParams);
			properties = reqHandler.responseText.parseJSON();

			propertySelect.options.length = 0;
			outputSelect.options.length = 0;
			for (var i = 0; i < properties.length; i++)
			{
				propertySelect.options[i] = new Option(properties[i].name, i, false, false);
				outputSelect.options[i] = new Option(properties[i].name, i, false, false);
			}
			OnPropertyChange();
		}

		function OnPropertyChange()
		{
            var propertySelect = document.getElementById("propertySelect");
            var operatorSelect = document.getElementById("operatorSelect");

			operatorSelect.options.length = 0;
			if (propertySelect.selectedIndex >= 0)
			{
				var ops = null;
				if (properties[propertySelect.value].isString)
					ops = strOps;
				else
					ops = numOps;

				for (var i = 0; i < ops.length; i++)
				{
					operatorSelect.options[i] = new Option(ops[i], i, false, false);
				}
			}

		}

		function OnToggleSpatialFilter()
		{
			if (spatialFilterGeomText != null)
			{
				ToggleSpatialFilter(document.getElementById("spatialFilter").checked);
			}
		}

		function OnDigitizeRectangle()
		{
			parent.parent.mapFrame.DigitizeRectangle(OnRectangleDigitized);
		}

		function OnRectangleDigitized(rectangle)
		{
			var geomText = "5,"
				+ rectangle.Point1.X + "," + rectangle.Point1.Y + ","
				+ rectangle.Point2.X + "," + rectangle.Point1.Y + ","
				+ rectangle.Point2.X + "," + rectangle.Point2.Y + ","
				+ rectangle.Point1.X + "," + rectangle.Point2.Y + ","
				+ rectangle.Point1.X + "," + rectangle.Point1.Y;

			ShowSpatialFilter(geomText);
		}

		function OnDigitizePolygon()
		{
			parent.parent.mapFrame.DigitizePolygon(OnPolyonDigitized);
		}

		function OnPolyonDigitized(polygon)
		{
			var geomText = polygon.Count;
			for (var i = 0; i < polygon.Count; i++)
			{
				geomText += "," + polygon.Point(i).X + "," + polygon.Point(i).Y;
			}

			ShowSpatialFilter(geomText);
		}

		function OnClearSpatialFilter()
		{
			document.getElementById("spatialFilter").checked = false;
			OnToggleSpatialFilter();

			spatialFilterGeomText = null;
		}

		function ShowSpatialFilter(geomText)
		{
            reqParams = "COMMAND=SHOWSPATIALFILTER";
			reqParams += "&SESSION=" + encodeURIComponent(session);
            reqParams += "&MAPNAME=" + encodeURIComponent(mapName);
			reqParams += "&GEOMTEXT=" + encodeURIComponent(geomText);

            if (msie)
                reqHandler = new ActiveXObject("Microsoft.XMLHTTP");
            else
                reqHandler = new XMLHttpRequest();

            if (msie)
                reqHandler.open("POST", "querycontroller.php", false);
            else
                reqHandler.open("POST", "../phpviewersample/query/querycontroller.php", false);
            reqHandler.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
            reqHandler.send(reqParams);

			spatialFilterGeomText = geomText;
			document.getElementById("spatialFilter").checked = true;
			parent.parent.Refresh();
		}

		function ToggleSpatialFilter(visible)
		{
			reqParams = "COMMAND=TOGGLESPATIALFILTER";
			reqParams += "&SESSION=" + encodeURIComponent(session);
			reqParams += "&MAPNAME=" + encodeURIComponent(mapName);
			reqParams += "&VISIBLE=" + encodeURIComponent(visible);

			if (msie)
				reqHandler = new ActiveXObject("Microsoft.XMLHTTP");
			else
				reqHandler = new XMLHttpRequest();

			reqHandler.open("POST", "querycontroller.php", false);
			reqHandler.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
			reqHandler.send(reqParams);

			parent.parent.Refresh();
		}

		function ExecuteQuery()
		{
		    var layerSelect = document.getElementById("layerSelect");

            reqParams = "COMMAND=EXECUTEQUERY";
			reqParams += "&SESSION=" + encodeURIComponent(session);
            reqParams += "&MAPNAME=" + encodeURIComponent(mapName);
			reqParams += "&LAYERNAME=" + encodeURIComponent(layerSelect.value);

			if (document.getElementById("propertyFilter").checked)
			{
				reqParams += "&USEPROPERTYFILTER=true";
				property = properties[document.getElementById("propertySelect").value];
				reqParams += "&PROPERTYNAME=" + encodeURIComponent(property.name);
				reqParams += "&ISSTRING=" + encodeURIComponent(property.isString);
				reqParams += "&OPERATOR=" + encodeURIComponent(document.getElementById("operatorSelect").value);
				reqParams += "&VALUE=" + encodeURIComponent(document.getElementById("valueInput").value);
			}
			else
			{
				reqParams += "&USEPROPERTYFILTER=false";
			}

			if (document.getElementById("spatialFilter").checked && spatialFilterGeomText != null)
			{
				reqParams += "&USESPATIALFILTER=true";
				reqParams += "&GEOMTEXT=" + encodeURIComponent(spatialFilterGeomText);
			}
			else
			{
				reqParams += "&USESPATIALFILTER=false";
			}

			reqParams += "&OUTPUTPROPERTY=" + encodeURIComponent(properties[document.getElementById("outputSelect").value].name);
			reqParams += "&QUERYMAX=" + encodeURIComponent(document.getElementById("queryMax").value);

            if (msie)
                queryReqHandler = new ActiveXObject("Microsoft.XMLHTTP");
            else
                queryReqHandler = new XMLHttpRequest();

			queryReqHandler.onreadystatechange = OnReadyStateChange;
            queryReqHandler.open("POST", "querycontroller.php", true);
            queryReqHandler.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");

			document.getElementById("executeBtn").disabled = true;
			document.getElementById("busyImg").src = BUSY_IMAGE;

			queryReqHandler.send(reqParams);
		}

		function SelectFeature()
		{
		    var layerSelect = document.getElementById("layerSelect");
			var resultSelect = document.getElementById("resultSelect");

            reqParams = "COMMAND=GETSELECTIONXML";
			reqParams += "&SESSION=" + encodeURIComponent(session);
            reqParams += "&MAPNAME=" + encodeURIComponent(mapName);
			reqParams += "&LAYERNAME=" + encodeURIComponent(layerSelect.value);
			reqParams += "&IDLIST=" + results[resultSelect.value].idList.toJSONString();

            if (msie)
                reqHandler = new ActiveXObject("Microsoft.XMLHTTP");
            else
                reqHandler = new XMLHttpRequest();

            reqHandler.open("POST", "querycontroller.php", false);
            reqHandler.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
			reqHandler.send(reqParams);
			selectionXml = reqHandler.responseText;

            parent.parent.mapFrame.SetSelectionXML(TrimString(selectionXml));
        }

        function TrimString(responseString)
        {
            responseString = responseString.replace( /^\s+/g, "" );
            return responseString.replace( /\s+$/g, "" );
        }

		function ZoomToFeature()
		{
			var resultSelect = document.getElementById("resultSelect");
			var scaleInput = document.getElementById("scaleInput");
			scale = scaleInput.value;
			scale = (scale == 0) ? 10000 : scale;
			scale = scale*1.0;

			feature = results[resultSelect.value];
			parent.parent.mapFrame.ZoomToView(feature.centerX, feature.centerY, scale, true);
		}

		function OnReadyStateChange()
		{
			var ready = queryReqHandler.readyState;

			if (ready == READY_STATE_COMPLETE)
			{
				results = queryReqHandler.responseText.parseJSON();

				var resultSelect = document.getElementById("resultSelect");
				resultSelect.options.length = 0;
				for (var i = 0; i < results.length; i++)
				{
					resultSelect.options[i] = new Option(results[i].displayValue, i, false, false);
				}
				OnResultChange();

				document.getElementById("executeBtn").disabled = false;
				document.getElementById("busyImg").src = NOT_BUSY_IMAGE;
				queryReqHandler = null;
			}
		}

		function OnResultChange()
		{
            var resultSelect = document.getElementById("resultSelect");
			var scaleInput = document.getElementById("scaleInput");
			var zoomBtn = document.getElementById("zoomBtn");
			var selectBtn = document.getElementById("selectBtn");

			if (resultSelect.selectedIndex >= 0)
			{
				scaleInput.disabled = false;
				zoomBtn.disabled = false;
				selectBtn.disabled = false;
			}
			else
			{
				scaleInput.disabled = true;
				zoomBtn.disabled = true;
				selectBtn.disabled = true;
			}
		}

		function OnLoad()
		{
			OnLayerChange();
			OnResultChange();
		}

		function OnUnload()
		{
			ToggleSpatialFilter(false);
		}
	</script>

</head>

<body onLoad="OnLoad();" onUnload="OnUnload();" marginwidth=5 marginheight=5 leftmargin=5 topmargin=5 bottommargin=5 rightmargin=5>

<?php if ($errorMsg == null) { ?>

<table class="RegText" border="0" cellspacing="0" width="100%">
	<tr><td class="Title"><img id="busyImg" src="../images/loader_inactive.gif" style="vertical-align:bottom">&nbsp;Query Features<hr></td></tr>
	<tr><td class="SubTitle">Select a Layer</td></tr>
    <tr><td>Layer:</td></tr>
	<tr>
		<td class="RegText">
			<select size="1" class="Ctrl" id="layerSelect" onChange="OnLayerChange()" style="width: 100%">
				<?php
					$selected = 'selected';
					foreach($layerNames as $layerName => $layerLabel) {
				?>
				<option value="<?= $layerName ?>" <?=$selected ?> ><?= $layerLabel ?></option>
				<?php
						$selected = '';
					}
				?>
		  	</select>
		</td>
	</tr>
    <tr><td class="Spacer"></td></tr>
	<tr><td class="SubTitle"><input class="Ctrl" type="checkbox" id="propertyFilter" checked>&nbsp;Property Filter</td></tr>
    <tr><td>Property:</td></tr>
	<tr>
		<td class="RegText">
			<select size="1" class="Ctrl" id="propertySelect" onChange="OnPropertyChange()" style="width: 100%">
		  	</select>
		</td>
	</tr>
    <tr><td>Operator:</td></tr>
	<tr>
		<td class="RegText">
			<select size="1" class="Ctrl" id="operatorSelect" style="width: 100%">
		  	</select>
		</td>
	</tr>
    <tr><td>Value:</td></tr>
	<tr>
		<td class="RegText">
            <input maxlength="100" class="Ctrl" id="valueInput" style="width: 100%">
		</td>
	</tr>
    <tr><td class="Spacer"></td></tr>
	<tr><td class="SubTitle"><input class="Ctrl" type="checkbox" onChange="OnToggleSpatialFilter()" id="spatialFilter">&nbsp;Spatial Filter</td></tr>
    <tr><td>Digitize:</td></tr>
    <tr>
        <td align="center">
            <input type="button" name="" value="Rectangle" class="Ctrl" id="rectButton" onClick="OnDigitizeRectangle()" style="width: 30%">
            <input type="button" name="" value="Polygon" class="Ctrl" id="polyButtton" onClick="OnDigitizePolygon()" style="width: 30%">
            <input type="button" name="" value="Clear" class="Ctrl" id="clearButton" onClick="OnClearSpatialFilter()" style="width: 30%">
        </td>
    </tr>
    <tr><td class="Spacer"></td></tr>
	<tr><td class="SubTitle">Output</td></tr>
    <tr><td>Output property:</td></tr>
	<tr>
		<td class="RegText">
			<select size="1" class="Ctrl" id="outputSelect" style="width: 100%">
		  	</select>
		</td>
	</tr>
    <tr><td class="Spacer"></td></tr>
    <tr><td><hr></td></tr>
	<tr>
		<td>
			<input id="executeBtn" class="Ctrl" name="" type="button" onClick="ExecuteQuery()" value="Execute" style="width:60px">
			&nbsp;&nbsp;Max results:&nbsp;
			<input name="" id="queryMax" class="Ctrl" type="text" value="100" size="5">
		</td>
	</tr>
    <tr><td class="Spacer"></td></tr>
	<tr><td class="SubTitle">Results</td></tr>
	<tr>
		<td>
			<select class="Ctrl" id="resultSelect" size="15" onChange="OnResultChange()" style="width: 100%"></select>
		</td>
	</tr>
	<tr>
		<td>
			Scale:&nbsp;<input class="Ctrl" id="scaleInput" type="text" size="6" value="10000">
			<input class="Ctrl" id="zoomBtn" type="button" onClick="ZoomToFeature()" value="Zoom" style="width:60px">
			<input class="Ctrl" id="selectBtn" type="button" onClick="SelectFeature()" value="Select" style="width:60px">
		</td>
	</tr>

</table>

<?php } else { ?>

<table class="RegText" border="0" cellspacing="0" width="100%%">
	<tr><td class="Title">Error<hr></td></tr>
	<tr><td><?= $errorMsg ?></td></tr>
	<tr><td><?= $errorDetail ?></td></tr>
</table>

<?php } ?>

</body>

</html>

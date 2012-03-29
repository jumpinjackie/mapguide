<?php
//
//  Copyright (C) 2004-2011 by Autodesk, Inc.
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

try
{
    include 'resizableadmin.php';

    LoadSessionVars();

    // Did the user logout?
    CheckForLogout();

    // Are we cancelling?  If so, there is nothing to do.
    CheckForCancel( 'performanceReport.php' );

    // Define Local values
    $menuCategory = PERFORMANCE_REPORT_MENU_ITEM;
    $pageTitle = PERFORMANCE_REPORT_MENU_ITEM;
    $helpPage = 'HelpDocs/performance_report.htm';
    $pageName = 'PerformanceReportPage';
    $formName = 'PerformanceReportForm';
    $homePage = NULL;
    $confirmationMsg = "";
    $errorMsg = "";
    $mapProfileResult=new MapProfileResult();
    $mapResources;
    $displayManager = new DisplayProfileResultManager();
    $recentSettings = new RecentSettings();

    //this user-defined sort function is according to the design
    function CompareMapName($a, $b)
    {
        $strA = $a["ShortNames"];
        $strB = $b["ShortNames"];

        $strALength = strlen($strA);
        $strBLength = strlen($strB);
        $length = 0;

        if($strALength >= $strBLength)
        {
            $length = $strBLength;
        }
        else
        {
            $length = $strALength;
        }

        if(0 == $strALength)
        {
            return 0;
        }

        for($i = 0; $i < $length; $i++)
        {
            $strALower = strtolower($strA[$i]);
            $strBLower = strtolower($strB[$i]);

            if($strALower == $strBLower)
            {
                if($strA[$i] == $strB[$i])
                {
                    continue;
                }

                if(strtoupper($strA[$i]) == $strB[$i])
                {
                    return -1;
                }

                if(strtolower($strA[$i]) == $strB[$i])
                {
                    return 1;
                }
            }
            else if($strALower < $strBLower)
            {
                return -1;
            }
            else
            {
                return 1;
            }
        }

        if($strALength == $strBLength)
        {
            return 0;
        }

        if($strALength > $strBLength)
        {
            return 1;
        }

        if($strALength < $strBLength)
        {
            return -1;
        }
    }

    function GetAllMapResources()
    {
        try
        {
            global $site;
            global $userInfo;
            global $mapResources;
            $mapResourcesXml = "";

            // Enumerates all maps in the library
            $resourceID = new MgResourceIdentifier("Library://");
            //connect to the site and get a resource service instance
            $siteConn = new MgSiteConnection();
            $siteConn->Open($userInfo);
            $resourceService = $siteConn->CreateService(MgServiceType::ResourceService);

            $byteReader = $resourceService->EnumerateResources($resourceID, -1, "MapDefinition");

            $chunk = "";
            do
            {
                $chunkSize = $byteReader->Read($chunk, 4096);
                $mapResourcesXml = $mapResourcesXml . $chunk;
            } while ($chunkSize != 0);

            $resourceList = new DOMDocument();
            $resourceList->loadXML($mapResourcesXml);

            $resourceIdNodeList = $resourceList->documentElement->getElementsByTagName("ResourceId");

            for ($i = 0; $i < $resourceIdNodeList->length; $i++)
            {
                $mapResourceID = $resourceIdNodeList->item($i)->nodeValue;
                $shortMapName = strrchr($mapResourceID, '/');
                $shortMapName = substr($shortMapName, 1, strlen($shortMapName) - 15);
                $mapResources[$i]["FullNames"] = $mapResourceID;
                $mapResources[$i]["ShortNames"] = $shortMapName;
            }

            usort($mapResources, "CompareMapName");
        }
        catch (Exception $exc)
        {
            $errorMsg = $exc->getMessage();
        }
    }

    GetAllMapResources();

    $recentSettings->GetRecentSettings();    
}
catch ( MgException $e )
{
    CheckForFatalMgException( $e );
    $errorMsg = $e->GetExceptionMessage();
}
catch ( Exception $e )
{
    $errorMsg = $e->getMessage();
}
?>

<!-- PAGE DEFINITION -->
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
    <?php OutputHeader( $pageTitle ); ?>
<body>
    <div>
        <style type="text/css">
        .layerResultsHeaderStyle table,.layerResultsHeaderStyle td
        {
                font:100% Arial, Helvetica, sans-serif;
        }

        .layerResultsHeaderStyle table
        {
            width:100%;
            border-collapse:collapse;
            margin: 0px;
            padding: 0px;
        }

        .layerResultsHeaderStyle th
        {
            text-align:center;
            padding:.5em;
            border-top:1px solid #CCCCCC;
            background-color:#EEEEEE;
            border-bottom: none;
            cursor:  default;
        }

        .layerResultsStyle table,.layerResultsStyle td
        {
                font:100% Arial, Helvetica, sans-serif;
        }

        .layerResultsStyle table
        {
            width:100%;
            border-collapse:collapse;
            margin:1em 0;
        }

        .layerResultsStyle th,.layerResultsStyle td
        {
            text-align:left;
            padding:6px 3px;
            border:1px solid #CCCCCC;
            word-wrap:break-word;
            overflow:hidden;
        }

        .layerResultsStyle th
        {
            background-color:#EEEEEE;
        }

        .layerResultsStyle tr.even td
        {
            background:#EEEEEE;
        }      

        .layerResultsStyle tr.odd td
        {
            background:#FFFFFF;
        }

        /* use this if you want to apply different styleing to empty table cells*/
        .layerResultsStyle td.empty,.layerResultsStyle tr.odd td.empty,.layerResultsStyle tr.even td.empty
        {
            background:#FFFFFF;
        }

        #layerBody
        {
             Height:    300px;
             margin:    0px;
             padding:   0px;
             overFlow:  auto;
             cursor: pointer;
        }

        .mapDefinitionResultTableStyle tr
        {
            height: 35px;
        }
        
        .mapDefinitionResultTableStyle td
        {
            text-align: left;
            vertical-align: top;
        }

        .mapDefinitionResultTableStyle
        {
            font-size: medium;
        }

        .hideTooltip
        {
            display: none;
        }

        .showTooltip
        {
            position: absolute;
            padding: 2px 4px 4px 4px;
            background-color: #ffffee;
            color: black;
            border-top: 1px solid #999999;
            border-right: 2px solid #666666;
            border-bottom: 2px solid #666666;
            border-left: 1px solid #666666;
            z-index: 100;
            line-height: 1.0;
            word-wrap:break-word;
            white-space: normal;
            font-weight: normal;
        }

        .mapNameStyle
        {
            background: transparent url('images/mapDefinitionNameInfo.png') no-repeat right;
            padding-right: 20px;
            cursor:default;
        }
        
        .warnMsgStyle
        {
            border-color: red;
        }

        .bgDivStyle
        {
            position:absolute;
            left:0px;
            top:0px;
            background-color:#777777;
            filter:alpha(Opacity=30);
            opacity:0.6;
            z-index:10000;
            display: none;
        }

        .mapViewerDialogStyle
        {
            background-color:white;
            border: 1px solid #000000;
            padding: 0px;
            margin: 0px;
            position:absolute;
            z-index:10000;
            width:800px;
            height:600px;
            text-align:center;
            line-height:25px;
            display: none;
        }

        .mapSelectorBtnStyle
        {
            background-color:red;
            border: 1px solid #000000;
            padding: 0px;
            margin: 0px;
            position:absolute;
            z-index:100001;
            width:200px;
            height:60px;
            text-align:center;
            line-height:25px;
            top: 400px;
            left: 400px;
        }
        
        #ResultNotMatchWrn
        {
            border: 2px solid #CCCCCC;
            background: #FFFEBB url('images/warning.png') scroll no-repeat 2px 2px;
            margin-top: 25px;
            padding-left: 30px;
            display: none;
            width: auto;
        }

        .errorMessage
        {
            border: 2px solid #CCCCCC;
            background: #FFFEBB url('images/warning.png') scroll no-repeat 2px 2px;
            margin-top: 20px;
            padding: 4px 5px 5px 32px;
            width: auto;
        }

        .wrnMessage
        {
            padding:5px 15px 5px 15px;
            display:none;
            filter: progid:DXImageTransform.Microsoft.gradient(startColorstr=\'#EEEEEE\', endColorstr=\'#BBBBBB\');
            background: #CCCCCC;
            background: -moz-linear-gradient(top, #EEEEEE, #BBBBBB);
            background: -webkit-gradient(linear, left top, left bottom, from(#EEEEEE), to(#BBBBBB));
        }

    </style>
    </div>
    <script type="text/javascript">
        var Common = {
            getItself: function(id)
            {
                return "string" == typeof id ? document.getElementById(id) : id;
            },

            getEvent: function()
            {
                if (document.all)
                {
                    return window.event;
                }
                func = getEvent.caller;
                while (func != null)
                {
                    var arg0 = func.arguments[0];
                    if (arg0)
                    {
                        if ((arg0.constructor == Event || arg0.constructor == MouseEvent) || (typeof (arg0) == "object" && arg0.preventDefault && arg0.stopPropagation))
                        {
                            return arg0;
                        }
                    }
                    func = func.caller;
                }
                return null;
            },

            getMousePos: function(ev)
            {
                if (!ev)
                {
                    ev = this.getEvent();
                }
                if (ev.pageX || ev.pageY)
                {
                    return {x: ev.pageX , y: ev.pageY};
                }

                if (document.documentElement.scrollLeft && document.documentElement.scrollTop)
                {
                    return {
                        x: ev.clientX + document.documentElement.scrollLeft - document.documentElement.clientLeft,
                        y: ev.clientY + document.documentElement.scrollTop - document.documentElement.clientTop
                    };
                }
                else if (document.body)
                {
                    return {
                        x: ev.clientX + document.body.scrollLeft - document.body.clientLeft,
                        y: ev.clientY + document.body.scrollTop - document.body.clientTop
                    };
                }
            },

            getElementPos: function(el)
            {
                el = this.getItself(el);
                var xValue = 0, yValue = 0;
                do
                {
                    xValue += el.offsetLeft;
                    yValue += el.offsetTop;
                } while ((el = el.offsetParent));

                return { x: xValue, y: yValue };
            }
        }

        function CollapsibleTabClick(tabId,contentId)
        {
            var collapseImage = document.getElementById(tabId);
            var content = document.getElementById(contentId);

            if (collapseImage.alt == "down")
            {
                collapseImage.alt = "left";
                collapseImage.src = "images/arrow_left.png";
                content.style.display = "none";
            }
            else
            {
                collapseImage.alt = "down";
                collapseImage.src = "images/arrow_down.png";
                content.style.display = "block";
            }
        }

        function CollapseSettingTab()
        {
            var collapseImage = document.getElementById("settings_CollapseImage_ID");
            var content = document.getElementById("settingsContent");

            collapseImage.alt = "left";
            collapseImage.src = "images/arrow_left.png";

            content.style.display = "none";
        }

        //As soon as the user changed the settings,
        //an inline alert message is displayed between the Action buttons and the “Results” title
        function SetResultNotMatchWarningMsg(visible)
        {
            var wrnMsg = document.getElementById("ResultNotMatchWrn");
            if(visible)
            {
                wrnMsg.style.display = "block";
            }
            else
            {
                wrnMsg.style.display = "none";
            }
        }

        //Determine whether to show the "settings changed" inline warning message or not
        function ShowReportWarningMsg()
        {
            var scale = document.getElementById("txtScale");
            var centerPoint = document.getElementById("txtCenterPoint");
            var mapSelector = document.getElementById("mapSelector_DO_NOT_PERSIST");

            var mapIdLastRun = document.getElementById("lastRunMapId");
            var centerPointLastRun = document.getElementById("lastRunCenterPoint");
            var scaleLastRun = document.getElementById("lastRunScale");

            var currentMapIdValue = RemoveSpace(mapSelector.value);
            var currentCenterPointValue = RemoveSpace(centerPoint.value);
            var currentScaleValue = RemoveSpace(scale.value);

            //if the value of the mapIdLastRun textbox is not empty,
            //then the performance report has been ran at least once
            if(mapIdLastRun.value != "")
            {
                //if the map resource ID is not match then show the warning message
                if(currentMapIdValue != mapIdLastRun.value)
                {
                    SetResultNotMatchWarningMsg(true);
                    return;
                }

                //if the center point is not match then show the warning message
                if(ValidateCenterPoint(false))
                {
                    var point = currentCenterPointValue.split("*");
                    var x = point[0];
                    var y = point[1];

                    currentCenterPointValue = RemoveSpace( FormatCenterPoint(x,y,2) );

                    if(currentCenterPointValue != centerPointLastRun.value)
                    {
                        SetResultNotMatchWarningMsg(true);
                        return;
                    }
                }
                else
                {
                    SetResultNotMatchWarningMsg(true);
                    return;
                }

                //if the scale is not match then show the warning message
                if(ValidateScale(false))
                {
                    currentScaleValue = RemoveSpace( FormatNumber(currentScaleValue,4) );

                    if(currentScaleValue != scaleLastRun.value)
                    {
                       SetResultNotMatchWarningMsg(true);
                       return;
                    }
                }
                else 
                {
                    SetResultNotMatchWarningMsg(true);
                    return;
                }

                SetResultNotMatchWarningMsg(false);
            }
        }

        function SaveLastRunSettings()
        {
            var scale = document.getElementById("txtScale");
            var centerPoint = document.getElementById("txtCenterPoint");
            var mapSelector = document.getElementById("mapSelector_DO_NOT_PERSIST");

            var mapIdLastRun = document.getElementById("lastRunMapId");
            var centerPointLastRun = document.getElementById("lastRunCenterPoint");
            var scaleLastRun = document.getElementById("lastRunScale");

            mapIdLastRun.value = RemoveSpace(mapSelector.value);
            centerPointLastRun.value = RemoveSpace(centerPoint.value);
            scaleLastRun.value = RemoveSpace(scale.value);
        }

        function ExpandResultsTab()
        {
            var content = document.getElementById("resultsTab");
            content.style.display = "block";
        }

        function MapResoucesNameSelectChange()
        {
            var tipDiv = document.getElementById("mapResourceNameTip");
            var mapDefinitonSelector = document.getElementById("mapSelector_DO_NOT_PERSIST");

            var settingsBtn = document.getElementById("mapViewerBtn");
            var centerPoint = document.getElementById("txtCenterPoint");
            var scale = document.getElementById("txtScale");
            
            //for IE
            if(centerPoint.detachEvent)
            {
                centerPoint.detachEvent("onpropertychange",CenterPointPropertyChange);
                scale.detachEvent("onpropertychange",ScalePropertyChange);
            }

            //If another map is selected, then the center point and scale should be cleared of values,
            //that is return to default values “enter center point”  and “enter scale”.
            if(centerPoint.value != "Enter center point")
            {
                centerPoint.value = "Enter center point";
            }

            if(scale.value != "Enter scale")
            {
                scale.value = "Enter scale";
            }

            //for IE
            if(centerPoint.attachEvent)
            {
               centerPoint.attachEvent("onpropertychange",CenterPointPropertyChange);
               scale.attachEvent("onpropertychange",ScalePropertyChange);
            }

            //show the map definition ID by the side of the map selector
            if(mapDefinitonSelector.selectedIndex >= 0)
            {
                tipDiv.innerHTML = mapDefinitonSelector.options[mapDefinitonSelector.selectedIndex].value;
                //clear warning message
                var mapSelectorWarnMsg = document.getElementById("selectMapResourceWarningMessage");
                mapSelectorWarnMsg.innerHTML = "";

                //text input Controls in Step 2 are enabled once a map resource is selected in Step 1.
                centerPoint.removeAttribute("disabled");
                scale.removeAttribute("disabled");
                settingsBtn.removeAttribute("disabled");

                var visible = ValidateScale(false);
                if(visible)
                {
                   visible = ValidateCenterPoint(false);
                }

                SetRunButtonState(visible);
            }
            else
            {
                tipDiv.innerHTML = "";
                //when user clear the settings, the mapselector is not seleced and the text area is disabled
                settingsBtn.setAttribute("disabled","disabled");
                centerPoint.setAttribute("disabled", "disabled");
                scale.setAttribute("disabled", "disabled");

                SetRunButtonState(false);
            }

            ShowReportWarningMsg();

            ClearWrnMsg();
        }

        function ClearWrnMsg()
        {
            var scale = document.getElementById("txtScale");
            scale.style.borderColor = "";

            var centerPoint = document.getElementById("txtCenterPoint");
            centerPoint.style.borderColor =  "";

            var scaleWarnMsg = document.getElementById("scaleWarnMessage");
            scaleWarnMsg.innerHTML = "";

            var centerPointWarnMsg = document.getElementById("centerPointWarnMessage");
            centerPointWarnMsg.innerHTML = "";
        }

        //check the value for the scale is resonable or not
        function ValidateScale(needFormat)
        {
            var result = false;

            var scale = document.getElementById("txtScale");
            var scaleWarnMsg = document.getElementById("scaleWarnMessage");
            var scaleValue = RemoveSpace(scale.value);

            //for IE
            if(scale.detachEvent)
            {
                scale.detachEvent("onpropertychange",ScalePropertyChange);
            }

            if("" == scaleValue)
            {
                result = false;
                scaleWarnMsg.innerHTML = "A map scale was not entered.";
                scale.style.borderColor = "red";
            }
            else
            {
                //positive float
                var regFloat =/^(\+)?\d+(\.(\d)+)?$/;
                result = regFloat.test(scaleValue);
                
                if("0" == scaleValue)
                {
                    result = false;
                }
                
                if(result)
                {
                    if( 0.0 == parseFloat(scaleValue))
                        result = false;
                }

                if(!result)
                {
                    scaleWarnMsg.innerHTML = "Not a valid map scale.";
                    scale.style.borderColor = "red";
                }
                else
                {
                    if(needFormat)
                    {
                        scale.value = FormatNumber(scaleValue,4);
                    }

                    scaleWarnMsg.innerHTML = "";
                    scale.style.borderColor = "";
                }
            }
            
            //for IE
            if(scale.attachEvent)
            {
                scale.attachEvent("onpropertychange",ScalePropertyChange);
            }

            return result;
        }

        //check the value for the center point is resonable or not
        function ValidateCenterPoint(needFormat)
        {
            var result = false;

            var centerPoint = document.getElementById("txtCenterPoint");
            var centerPointWarnMsg = document.getElementById("centerPointWarnMessage");
            var centerPointValue = RemoveSpace(centerPoint.value);

            //for IE
            if(centerPoint.detachEvent)
            {
                centerPoint.detachEvent("onpropertychange",CenterPointPropertyChange);
            }

            if("" == centerPointValue)
            {
                result = false;
                centerPointWarnMsg.innerHTML = "A center point was not entered.";
                centerPoint.style.borderColor = "red";
            }
            else
            {
                try
                {
                    var point = centerPointValue.split("*");
                    if(point.length != 2)
                    {
                        result = false;
                        centerPointWarnMsg.innerHTML = "Not a valid center point.";
                        centerPoint.style.borderColor = "red";
                    }
                    else
                    {
                        var x = point[0];
                        var y = point[1];

                        result = !(IsNotFloat(x)||IsNotFloat(y));

                        if(!result)
                        {
                            centerPointWarnMsg.innerHTML = "Not a valid center point.";
                            centerPoint.style.borderColor = "red";
                        }
                        else
                        {
                            if(needFormat)
                            {
                                centerPoint.value = FormatCenterPoint(x,y,2);
                            }

                            centerPointWarnMsg.innerHTML = "";
                            centerPoint.style.borderColor = "";
                        }
                    }
                }
                catch(e)
                {
                    result = false;
                    centerPointWarnMsg.innerHTML = "Not a valid center point.";
                    centerPoint.style.borderColor = "red";
                }
            }

            //for IE
            if(centerPoint.attachEvent)
            {
                centerPoint.attachEvent("onpropertychange",CenterPointPropertyChange);
            }

            return result;
        }

        function InitialEvents()
        {
            var centerPointEl = document.getElementById("txtCenterPoint");
            var scaleEl = document.getElementById("txtScale");

            //for browsers which support addEventListener, add event handler
            //for input, include the FF, Chrome, Safari
            if (centerPointEl.addEventListener)
            {
                centerPointEl.addEventListener("input", CenterPointTxtChange, false);
                scaleEl.addEventListener("input", ScaleTxtChange, false);
            }
            else if(centerPointEl.attachEvent)
            {
                //For IE, use attachEvent to add event hanlder for onpropertychange
                centerPointEl.attachEvent("onpropertychange",CenterPointPropertyChange);
                scaleEl.attachEvent("onpropertychange",ScalePropertyChange);
            }
            else
            {
                //For browers, which don't support both, use onkeup event
                centerPointEl.onkeyup = CenterPointTxtChange;
                scaleEl.onkeyup = ScaleTxtChange;
            }
        }

        //For IE
        function ScalePropertyChange(e)
        {
            if("value" == e.propertyName)
            {
                ScaleTxtChange();
            }
        }

        //For IE
        function CenterPointPropertyChange(e)
        {
            if("value" == e.propertyName)
            {
                CenterPointTxtChange();
            }
        }

        function ScaleTxtChange()
        {
            //when the scale textbox has new input
            //get the new value and check if the input is resonable
            var visible = ValidateScale(false);

            //check other settings to see if it is reasonable
            if(visible)
            {
                var mapDefinitonSelector = document.getElementById("mapSelector_DO_NOT_PERSIST");
                if(mapDefinitonSelector.selectedIndex >= 0)
                {
                    visible = ValidateCenterPoint(false);
                }
                else
                {
                    visible=false;
                }
            }

            SetRunButtonState(visible);

            //show the "settings changed" inline warning message depends on new setting
            ShowReportWarningMsg();
        }

        function CenterPointTxtChange()
        {
            //when the center point textbox has new input
            //get the new value and check if the input is resonable
            var visible = ValidateCenterPoint(false);

            //check other settings to see if it is reasonable
            if(visible)
            {
                var mapDefinitonSelector = document.getElementById("mapSelector_DO_NOT_PERSIST");
                if(mapDefinitonSelector.selectedIndex >= 0)
                {
                    visible = ValidateScale(false);
                }
                else
                {
                    visible = false;
                }
            }

            SetRunButtonState(visible);

            //show the "settings changed" inline warning message depends on new setting
            ShowReportWarningMsg();
        }

        function ScaleFocus()
        {
            var scale = document.getElementById("txtScale");

            //for IE
            if(scale.detachEvent)
            {
                scale.detachEvent("onpropertychange",ScalePropertyChange);
            }

            if( scale.value=="Enter scale")
            {
                scale.value="";
            }

            //for IE
            if(scale.attachEvent)
            {
               scale.attachEvent("onpropertychange",ScalePropertyChange);
            }
        }

        function CenterPointFocus()
        {
            var centerPoint = document.getElementById("txtCenterPoint");

            //for IE
            if(centerPoint.detachEvent)
            {
                centerPoint.detachEvent("onpropertychange",CenterPointPropertyChange);
            }

            if( centerPoint.value=="Enter center point")
            {
                centerPoint.value="";
            }

            //for IE
            if(centerPoint.attachEvent)
            {
               centerPoint.attachEvent("onpropertychange",CenterPointPropertyChange);
            }
        }

        var checkMapFrameLoadedInterval = null;
        function SelectMapSettings()
        {
            var mapFrame = document.getElementById("mapViewerFrame");
            var mapDefinitonSelector = document.getElementById("mapSelector_DO_NOT_PERSIST");
            if(mapDefinitonSelector.selectedIndex >= 0)
            {
                mapFrame.src = "performanceReport_MapViewer.php?mapDefinition=" + mapDefinitonSelector.options[mapDefinitonSelector.selectedIndex].value;
            }

            //when open the dialogue, scroll to the top
            window.scrollTo(0,0);
            //disable the scroll bar
            document.body.style.overflow = "hidden";

            var bgDiv = document.getElementById("bgDiv");
            var mapViewerDiv = document.getElementById("mapViewerDialog");

            bgDiv.style.display = "block";
            SetBgDivSize(bgDiv);

            mapViewerDiv.style.display = "block";
            SetMapViewerLoaction(mapViewerDiv);

            checkMapFrameLoadedInterval = setInterval(CheckMapFrameLoaded, 100);
        }

        //when the map viewer dialogue is displayed, user press "Enter",
        //it will cause the page reload, so we add a textbox here,
        //and make it focus when the map viewer is shown.
        //add the event handler for the "keyUp", when user press the key "Enter",
        //make it the same with press button "OK".
        //Here is an issue, to make the input invisible but can focus,
        //we set the width and height as 0, and border as none,
        //it works well in IE9 and firefox, but in chrome and safari,
        //if the border set as none, this event will not be fired.
        function MapViewerKeyUp(event)
        {
            if(event.keyCode == 13)
            {
                if(!IsMapFrameLoaded())
                    return;

                MapViewerBtnOKClicked();
             }
        }

        function SetBgDivSize(bgDiv)
        {
           //different browsers explain the offsetHeight, clientWidth differently,
           //so we add a spacing to the height and width
           //because the overflow of the body will be set as "hidden"
           //so the exceed part will not be displayed and ensure the bgDiv will conver the whole webpage
           var spacing = 32;
           bgDiv.style.height = document.body.offsetHeight + spacing + "px";
           bgDiv.style.width = document.body.offsetWidth + spacing + "px";
        }

        //default map viewer width and height
        var mapViewerWidth = 800;
        var mapViewerHeight = 600;
        function SetMapViewerLoaction(mapViewerDiv)
        {
            if(document.body.clientHeight > mapViewerHeight)
            {
                mapViewerDiv.style.top=(document.body.clientHeight/2-mapViewerHeight/2)+"px";
            }
            else
            {
                mapViewerDiv.style.top="10px";
            }

            if(document.body.clientWidth > mapViewerWidth)
            {
                mapViewerDiv.style.left=(document.body.clientWidth/2-mapViewerWidth/2)+"px";
            }
            else
            {
                mapViewerDiv.style.left="10px";
            }
        }

        window.onresize = WindowResized;

        function WindowResized()
        {
            var mapViewerDiv = document.getElementById("mapViewerDialog");

            if("block" == mapViewerDiv.style.display.toLowerCase())
            {
                SetMapViewerLoaction(mapViewerDiv);
            }

            var bgDiv = document.getElementById("bgDiv");
            if("block" == bgDiv.style.display.toLowerCase())
            {
                SetBgDivSize(bgDiv);
            }
        }

        function CheckMapFrameLoaded()
        {
            //if the session is time out, the whole page should redirect to the login page
            var mapViewerDocumentElement = null;

            try
            {
                mapViewerDocumentElement = document.getElementById("mapViewerFrame").contentDocument.documentElement;
            }
            catch(e)
            {
                //For IE
                mapViewerDocumentElement = document.getElementById("mapViewerFrame").contentWindow.document.documentElement;
            }

            if(mapViewerDocumentElement.innerHTML.indexOf("Site Administrator - Login") > 1)
            {
                if(mapViewerDocumentElement.innerHTML.indexOf("login.php") > 1)
                {
                    window.location.href="login.php?ErrorMsg=Fatal error encountered! Please login again.";
                }
            }

            if(!IsMapFrameLoaded())
                return;

            clearInterval(checkMapFrameLoadedInterval);

            //make the textbox focused
            var autoFoucsTxt = document.getElementById("mapViewerAutoFocusTxt");
            autoFoucsTxt.focus();

            CreateButtons();
        }

        function IsMapFrameLoaded()
        {
            var mainFrame = window.frames["mapViewerFrame"];

            if (!mainFrame)
                return false;

            if (!mainFrame || !mainFrame.GetMapFrame)
                return false;

            var mapFrame = mainFrame.GetMapFrame();

            return (mapFrame.ZoomToView != null);
        }

        function CloseMapViewer()
        {
            var bgDiv = document.getElementById("bgDiv");
            var msgDiv = document.getElementById("mapViewerDialog");

            bgDiv.style.display = "none";
            msgDiv.style.display="none";

            //must destory the old page, if not, the CheckMapFrameLoaded
            //will get the old page's frame and then the buttons will not be displayed in new page
            var mapFrame = document.getElementById("mapViewerFrame");
            mapFrame.src="";

            //set the overflow to "auto", so after close the dialogue
            //the page will back to normal
            document.body.style.overflow = "auto";
        }

        var messageShorterInterval = null;

        var zoomToViewInterval = null;

        function CreateButtons()
        {
            var mapViewerFrame = window.frames["mapViewerFrame"];

            var mapFrame = mapViewerFrame.GetMapFrame();
            var mapFrameDocument = mapFrame.document;

            mapFrame.ShowMapMessage("Use the map controls in the toolbar above to specify the center point and scale.", "info");
            //use setInterval to make sure that the message div has been created.
            messageShorterInterval = window.setInterval(function(){ makeMessageShorter(mapFrame); }, 100);

            zoomToViewInterval = window.setInterval(function(){ ZoomToPreviousView(mapFrame); }, 100);

            var mapSpace = mapFrameDocument.getElementById("mapSpace");
            var buttonPanel = mapFrameDocument.createElement('div');
            buttonPanel.style.backgroundColor="transparent";
            buttonPanel.style.padding="0px";
            buttonPanel.style.margin="0px";
            buttonPanel.style.position="absolute";
            buttonPanel.style.zIndex="10000";
            buttonPanel.style.top="490px";
            buttonPanel.style.left="660px";
            buttonPanel.style.width="160px";

            var buttonPanelInnerHtml="<table>";
            buttonPanelInnerHtml+="<tr>";
            buttonPanelInnerHtml+="<td>";
            buttonPanelInnerHtml+='<button type="button" id="mapViewerAutoFocusBtn" style="width: 57px; height: 22px; border-style:none;border: 1px solid #000000;" onclick="window.parent.parent.MapViewerBtnOKClicked()" tabindex="1">OK</button>';
            buttonPanelInnerHtml+="</td>";
            buttonPanelInnerHtml+="<td>";
            buttonPanelInnerHtml+='<button type="button" style="width: 57px; height: 22px; border-style:none;border: 1px solid #000000;margin-left:10px;" onclick="window.parent.parent.MapViewerBtnCloseClicked()"  tabindex="2">Cancel</button>';
            buttonPanelInnerHtml+="</td>";
            buttonPanelInnerHtml+="</tr>";
            buttonPanelInnerHtml+="</table>";

            buttonPanel.innerHTML = buttonPanelInnerHtml;
            mapSpace.appendChild(buttonPanel);
        }

        function MapViewerBtnCloseClicked()
        {
            CloseMapViewer();
        }

        function MapViewerBtnOKClicked()
        {
           var mapViewerFrame = window.frames["mapViewerFrame"];
           var mapFrame = mapViewerFrame.GetMapFrame();
           var center = mapFrame.GetCenter();
           var scale = mapFrame.GetScale();

           var centerPoint = document.getElementById("txtCenterPoint");
           if(centerPoint.detachEvent)
           {
                centerPoint.detachEvent("onpropertychange",CenterPointPropertyChange);
           }
           centerPoint.value = center.X + "*" + center.Y;
           if(centerPoint.attachEvent)
           {
                centerPoint.attachEvent("onpropertychange",CenterPointPropertyChange);
           }
           var centerPointValidate = ValidateCenterPoint(true);

           var scaleInput = document.getElementById("txtScale");
           if(scaleInput.detachEvent)
           {
               scaleInput.detachEvent("onpropertychange",ScalePropertyChange);
           }
           scaleInput.value = scale;
           if(scaleInput.attachEvent)
           {
               scaleInput.attachEvent("onpropertychange",ScalePropertyChange);
           }
           var scaleValidate = ValidateScale(true);

           var visible = centerPointValidate && scaleValidate;
           if(visible)
           {
               var mapDefinitonSelector = document.getElementById("mapSelector_DO_NOT_PERSIST");
               if(mapDefinitonSelector.selectedIndex >= 0)
               {
                   visible = true;
                   SaveCurrentSelect(mapDefinitonSelector.options[mapDefinitonSelector.selectedIndex].value,center,scale);
               }
               else
               {
                   visible = false;
               }
           }

           SetRunButtonState(visible);

           CloseMapViewer();
        }

        function SaveCurrentSelect(mapId,center,scale)
        {
            var mapIdSaved = document.getElementById("mapViewerLastOpenMapId");
            mapIdSaved.value = mapId;

            var centerPointSaved = document.getElementById("mapViewerLastOpenCenterPoint");
            centerPointSaved.value = center.X + "*" + center.Y;

            var scaleSaved = document.getElementById("mapViewerLastOpenScale");
            scaleSaved.value = scale;

            //when the user set the new value from the mapviewer, the warning info should be shown
            ShowReportWarningMsg();
        }

        function makeMessageShorter(mapFrame)
        {
            if(!mapFrame.mapMessage || !mapFrame.mapMessage.container)
                return;

            window.clearInterval(messageShorterInterval);
            // make it shorter
            mapFrame.mapMessage.container.style.padding = "1px";
        }

        function ZoomToPreviousView(mapFrame)
        {
            if(!mapFrame.mapLoading)
                return;

            window.clearInterval(zoomToViewInterval);

            var mapViewerFrame = window.frames["mapViewerFrame"];
            var mapFrame = mapViewerFrame.GetMapFrame();

            var mapIdSaved = document.getElementById("mapViewerLastOpenMapId");
            var mapDefinitonSelector = document.getElementById("mapSelector_DO_NOT_PERSIST");
            var selectMapId = mapDefinitonSelector.options[mapDefinitonSelector.selectedIndex].value;

            if(("" != mapIdSaved.value) && (mapIdSaved.value === selectMapId))
            {
                var centerPointSaved = document.getElementById("mapViewerLastOpenCenterPoint");
                var scaleSaved = document.getElementById("mapViewerLastOpenScale");

                var point = centerPointSaved.value.split("*");
                if(2 == point.length)
                {
                    mapFrame.ZoomToView( parseFloat(point[0]), parseFloat(point[1]), parseFloat(scaleSaved.value), true);
                }
            }
        }

        //remove the space at the begin and end of a string
        function Trim(str)
        {
            var returnedStr="";

            var i = 0;
            for(; i < str.length && str.charAt(i)==" "; i++);

            var j = str.length;
            for(; j>0 && str.charAt(j-1)==" " ; j--);

            if(i<=j)
            {
                returnedStr = str.substring(i,j);
            }

            return returnedStr;
        }

        //remove all the space inside a string
        function RemoveSpace(str)
        {
            var i=0,strWithoutSpace="";

            if(null == str)
                return strWithoutSpace;

            if(0 == str.length)
                return strWithoutSpace;

            for(;i < str.length;i++)
            {
                c = str.charAt(i);
                if(c!=' ')
                {
                    strWithoutSpace+=c;
                }
            }
            return strWithoutSpace;
        }

        function FormatNumber(s, n)
        {
           var n = n >= 0 && n <= 20 ? n : 2;

           var s = parseFloat((s + "").replace(/[^\d\.-]/g, "")).toFixed(n) + "";

           var l = s.split(".")[0].split("").reverse()

           if(n > 0)
           {
               var r = s.split(".")[1];
               var t = "";
               var i=0;
               for(; i < l.length; i ++ )
               {
                  t += l[i] + ((i + 1) % 3 == 0 && (i + 1) != l.length ? " " : "");
               }

               return t.split("").reverse().join("") + "." + r;
           }
           else
           {
               var t = "";
               var i=0;
               for(; i < l.length; i ++ )
               {
                  t += l[i] + ((i + 1) % 3 == 0 && (i + 1) != l.length ? " " : "");
               }

               return t.split("").reverse().join("");
           }
        }

        function FormatCenterPoint(x,y,fractionDigits)
        {
             return parseFloat(x).toExponential(fractionDigits) + "*" + parseFloat(y).toExponential(fractionDigits);
        }

        function IsNotFloat(str)
        {
          str = RemoveSpace(str);

          if("" == str)
              return true;

          var num=0;
          if((num=str.indexOf('E'))!=-1||(num=str.indexOf('e'))!=-1)
          {
                var doubleStr=str.substring(0,num);
                var eStr=str.substring(num+1);
                var result=IsNotSimpleFloat(doubleStr)||IsNotInt(eStr);

                return result;
          }
          else
          {
             return IsNotSimpleFloat(str);
          }
        }

        function IsNotInt(str)
        {
            //remove the + or -
            var firstChar=str.substring(0,1);
            if("+" == firstChar || "-" == firstChar)
            {
                str=str.substring(1);
            }

            if ((str.length>1 && str.substring(0,1)=="0") || IsNotNum(str))
            {
                return true;
            }

            return false;
        }

        function IsNotNum(str)
        {
          if ("" == str)
            return true;

          var i = 0;
          for(; i < str.length;i++)
          {
             var oneNum = str.substring(i,i+1);
             if (oneNum < "0" || oneNum > "9")
                return true;
            }
           return false;
        }

        function IsNotSimpleFloat(str)
        {
            var len = str.length;
            var dotNum = 0;

            if (0 == len)
            {
                return true;
            }

            //remove the + or -
            var firstChar=str.substring(0,1);
            if("+" == firstChar || "-" == firstChar)
            {
                str=str.substring(1);
                len=str.length;
            }

            var i = 0;
            for(;i < len;i++)
            {
                var oneNum = str.substring(i,i+1);

                if ("." == oneNum)
                    dotNum++;

                if ( ((oneNum < "0" || oneNum > "9") && oneNum != ".") || dotNum > 1)
                    return true;
            }

            if (len > 1 && "0" == str.substring(0,1))
            {
                if (str.substring(1,2) != ".")
                    return true;
            }

            return false;
          }

        function ShowToopTip(linkObj,toolTipID,e)
        {
            var toolTip = document.getElementById(toolTipID);
            toolTip.style.display = "none";

            var ev = e || window.event;
            var mosPos = Common.getMousePos(ev);
            var elPos = Common.getElementPos(linkObj);
            var downMouseLeft = 13,downMouseTop = 2;

            //must before set opr to get offsetHeight...
            //toolTip.style.display = "";
            //set tooltip position
            toolTip.style.top = elPos.y + linkObj.offsetHeight+ downMouseTop+ "px";
            toolTip.style.left = mosPos.x + downMouseLeft + "px";

            var bodyWidth = document.body.clientWidth;
            //clientWidth is different among different browsers,
            //The IE is shorter than others, so to avoid the tooltip length exceeds the border
            //we add a fix here
            if(window.addEventListener)
            {
                bodyWidth -= 12;
            }
            var toolTipWidth = Math.abs(bodyWidth - (mosPos.x + downMouseLeft));
            toolTipWidth = toolTipWidth > 280?280:toolTipWidth;

            //in IE9, if we don't set the width, and the tooltip is near the browser border,
            //it will break the string to many lines if the string contains white spaces
            //for example, Library://Samples/Sheboygan/Maps/Test a map with many white space.MapDefinition
            //this will be display like this:
            //Library://Samples/Sheboygan/Maps/Test
            //a
            //map
            //with
            //many
            //white
            //space.MapDefinition
            toolTip.style.width = toolTipWidth + "px";
            
            toolTip.style.display = "block";
            toolTip.className = "showTooltip";
        }

        function HideToolTop(toolTipID)
        {
            var toolTip = document.getElementById(toolTipID);
            toolTip.style.display = "none";
        }

        //AJAX part
        var xmlHttp;

        function GetXmlHttpObject()
        {
            var xmlHttp=null;

            try
             {
                 // Firefox, Opera 8.0+, Safari
                 xmlHttp=new XMLHttpRequest();
             }
            catch (e)
             {
                 // Internet Explorer
                 try
                  {
                      xmlHttp=new ActiveXObject("Msxml2.XMLHTTP");
                  }
                 catch (e)
                  {
                      xmlHttp=new ActiveXObject("Microsoft.XMLHTTP");
                  }
             }
            return xmlHttp;
        }

        function GetResult()
        {
            var mapDefinitonSelector = document.getElementById("mapSelector_DO_NOT_PERSIST");

            if(mapDefinitonSelector.selectedIndex < 0)
            {
                var mapResourceWarningMessage = document.getElementById("selectMapResourceWarningMessage");
                mapResourceWarningMessage.innerHTML = "A map resource was not selected.";
                return false;
            }

            if(!ValidateCenterPoint(true))
                return false;

            if(!ValidateScale(true))
                return false;

            //when a new report is run, the inline message disappears
            SetResultNotMatchWarningMsg(false);

            var loadingImg = document.getElementById("ajax_loading_img");
            loadingImg.style.display = "inline";

            var btnClear = document.getElementById("btnClearSpan");
            btnClear.style.display = "none";

            var runButton = document.getElementById("runBtn");
            runButton.setAttribute("disabled", "disabled");

            xmlHttp = GetXmlHttpObject();
            if (xmlHttp == null)
            {
               alert ("Browser does not support HTTP Request!");
               return;
            }

            //the profiling map API requires the image width and height
            var imageWidth = document.body.clientWidth?document.body.clientWidth:1280;
            var imageHeight = document.body.clientHeight?document.body.clientHeight:1024;

            //get the parameters for map resource ID, center point, scale
            var scale = document.getElementById("txtScale");
            var centerPoint = document.getElementById("txtCenterPoint");
            var mapSelector = document.getElementById("mapSelector_DO_NOT_PERSIST");

            //pass the parameters through the URL query string
            var url="performanceReport_GetResult.php";

            url+= "?scale=" + encodeURIComponent(RemoveSpace(scale.value));
            url+= "&centerPoint=" + encodeURIComponent(RemoveSpace(centerPoint.value));
            url+= "&mapDefinition=" + encodeURIComponent(mapSelector.value);
            url+= "&imageWidth=" + imageWidth;
            url+= "&imageHeight=" + imageHeight;
            url+= "&sid="+Math.random();

            //send the ajax request
            xmlHttp.onreadystatechange = StateChanged;
            xmlHttp.open("POST",url,true);
            xmlHttp.send(null);
        }

        function StateChanged()
        {
            if ((4 == xmlHttp.readyState || "complete" == xmlHttp.readyState)&& 200 == xmlHttp.status)
            {
                //if the session is time out, the whole page should redirect to the login page
                if(xmlHttp.responseText.indexOf("Site Administrator - Login") > 1)
                {
                    if(xmlHttp.responseText.indexOf("login.php") > 1)
                        {
                            window.location.href="login.php?ErrorMsg=Fatal error encountered! Please login again.";
                        }
                }
                else
                {
                    var profileResult = document.getElementById("resultsTab");
                    var result = xmlHttp.responseText;
                    var btnClear = document.getElementById("btnClearSpan");
                    var loadingImg = document.getElementById("ajax_loading_img");

                    if(Trim(result) == "mapNotExist")
                    {
                        alert("The selected map resource is no longer available. Select a different map resource to continue.");
                        btnClear.style.display = "inline";
                        loadingImg.style.display = "none";
                        SetRunButtonState(true);
                        return;
                    }

                    profileResult.innerHTML = xmlHttp.responseText;

                    btnClear.style.display = "inline";
                    loadingImg.style.display = "none";

                    CollapseSettingTab();
                    ExpandResultsTab();
                    SetRunButtonState(true);

                    SetLayerJsArray();
                    SetRecentSettingJsArray();

                    SetRecentSettingsContent();

                    SaveLastRunSettings();

                    //make the layer table default sorting
                    var layerTableHeader = document.getElementById("layerHeaderTable");
                    SortLayers.sortByColumn(layerTableHeader.tHead.rows[0].children[1]);
                    SortLayers.sortByColumn(layerTableHeader.tHead.rows[0].children[1]);
                }
            }
        }

        // the js in the ajax returned content will not be executed and recognized,
        // so we get the js content and append it to the <head>
        function SetLayerJsArray()
        {
            var tempLayersJsArray = document.getElementById("layerDetailsJsArray");
            if(tempLayersJsArray)
            {
                //get js content in the hidden span
                var innerScript = tempLayersJsArray.innerHTML;
                innerScript = innerScript.replace(/\\n/,'');

                //check to remove the script if it already exists
                var insertScript = document.getElementById("layerDetailsJs");
                if(insertScript)
                {
                    document.getElementsByTagName("head").item(0).removeChild(insertScript);
                }

                //every new execute should generate new js array data
                insertScript = document.createElement("script");
                insertScript.type = "text/javascript";
                insertScript.id = "layerDetailsJs";
                insertScript.text=innerScript;

                document.getElementsByTagName("head").item(0).appendChild(insertScript);
                tempLayersJsArray.parentNode.removeChild(tempLayersJsArray);
            }
        }

        function SetRecentSettingJsArray()
        {
            var tempSettingJsArray = document.getElementById("tempRecentSettingsJsArray");

            if(tempSettingJsArray)
            {
                //get js content in the hidden span
                var innerScript = tempSettingJsArray.innerHTML;
                innerScript = innerScript.replace(/\\n/,'');

                //check to remove the script if it already exists
                var insertScript = document.getElementById("recentSettingsJs");
                if(insertScript)
                {
                    document.getElementsByTagName("head").item(0).removeChild(insertScript);
                }

                //every new execute should generate new js array data
                insertScript = document.createElement("script");
                insertScript.type = "text/javascript";
                insertScript.id = "recentSettingsJs";
                insertScript.text = innerScript;

                document.getElementsByTagName("head").item(0).appendChild(insertScript);

                tempSettingJsArray.parentNode.removeChild(tempSettingJsArray);
            }
        }

        function SetRecentSettingsContent()
        {
            //remove the layer detail info if it already exist
            var tempRecentSettings = document.getElementById("tempRecentSettingDiv");
            var settingsTab= document.getElementById("settingsDiv");
            var settingsContentDiv = document.getElementById("recentSettingsDiv");

            if (settingsContentDiv)
            {
                settingsContentDiv.parentNode.removeChild(settingsContentDiv);
            }

            settingsContentDiv = document.createElement("div");
            settingsContentDiv.id = "recentSettingsDiv";
            settingsContentDiv.innerHTML = tempRecentSettings.innerHTML;

            if (tempRecentSettings)
            {
                tempRecentSettings.parentNode.removeChild(tempRecentSettings);
            }

            settingsTab.appendChild(settingsContentDiv);
        }
        //End AJAX part

        function ExportCSV()
        {
            var url="performanceReport_Export.php";
            var centerPoint = document.getElementById("txtCenterPoint");
            url+="?centerPoint="+encodeURIComponent(centerPoint.value);
            var form = document.getElementById("getCSVFileForm");
            form.action=url;
            form.submit();
        }

        function ClearSettings()
        {
            //When Clear button clicked, removes the settings from
            //map selector, scale textbox and center point textbox.
            //Also removes any validation messages that are visible.
            var mapDefinitonSelector = document.getElementById("mapSelector_DO_NOT_PERSIST");

            if(mapDefinitonSelector.selectedIndex >= 0)
            {
                mapDefinitonSelector.selectedIndex = -1;
                //change the selectedIndex valule by the js will not fire the event "onchange",
                //so we manually do it.
                MapResoucesNameSelectChange();
            }

            //Also removes any validation messages that are visible.
            ClearWrnMsg();

            var mapSelectorWarnMsg = document.getElementById("selectMapResourceWarningMessage");
            mapSelectorWarnMsg.innerHTML = "";

            SetRunButtonState(false);
        }

        function SetRunButtonState(visible)
        {                 
            var runButton = document.getElementById("runBtn");

            if(visible)
            {
                runButton.removeAttribute("disabled");
            }
            else
            {
                runButton.setAttribute("disabled", "disabled");
            }
        }

        //when the layer info row click find the corresponding layer detail info and display it on the UI
        function LayerDataTableRowClicked(layerName,tableRow)
        {
            var layerCount = layerDetailValues.length;

            var i = 0;
            for(; i < layerCount; i++)
            {
                if( Trim(layerName) == Trim(layerDetailValues[i][0]) )
                {
                    DisplayLayerDetail(layerDetailValues[i]);
                    break;
                }
            }
            
            var j = 0;
            var errorStatus = tableRow.getAttribute("errorstatus");
            if(errorStatus == "0")
            {
                for (;j < tableRow.cells.length; j++)
                {
                    var obj = tableRow.cells[j];
                    obj.style.backgroundColor = "#b4c6de";
                }
            }
            else
            {
                for (;j < tableRow.cells.length; j++)
                {
                    var obj = tableRow.cells[j];
                    obj.style.backgroundColor = "red";
                }
            }

            var layersTable = document.getElementById("layerResultsTable");
            var allRows = layersTable.tBodies[0].rows;
            var k = 0;
            for (;k < allRows.length; k++)
            {
                var lastSelectedRow = allRows[k];

                var isSelected = lastSelectedRow.getAttribute("rowselected");

                if("true" == isSelected)
                {
                    if(tableRow === lastSelectedRow)
                    {
                        break;
                    }
                    
                    var m = 0;
                    var errorStatus = lastSelectedRow.getAttribute("errorstatus");
                    if(errorStatus == "0")
                    {
                        for (;m < lastSelectedRow.cells.length; m++)
                        {
                            var obj = lastSelectedRow.cells[m];
                            obj.style.backgroundColor = "";
                        }
                    }
                    else
                    {
                        for (;m < lastSelectedRow.cells.length; m++)
                        {
                            var obj = lastSelectedRow.cells[m];
                            obj.style.backgroundColor = "#FFFEBB";
                        }
                    }
                    
                    lastSelectedRow.setAttribute("rowselected","false");
                }
            }
            
            tableRow.setAttribute("rowselected","true");
        }

        //the detail info of a layer is on the left of the layer info table
        //this method will display the selected layer detail info on the UI with designed style
        //Note: when use document.getElementById, make sure the variable name is not the same with the "ID"
        //like this: layerInfoDetail =document.getElementById("layerInfoDetail");
        //it will cause problem under IE9 quirks mode, layerInfoDetail is already a built-in object of the document.
        //two ways to solve this problem
        //1) make the variable name different with the "ID", layerDetail =document.getElementById("layerInfoDetail");
        //2) add "var" before the varibale name, var layerInfoDetail =document.getElementById("layerInfoDetail");
        function DisplayLayerDetail(layerDetail)
        {                    
            var layerDetailContent = '<div style="padding-bottom: 8px; padding-top: 7px; width: 80%;">' + layerDetail[0] + '</div>'+ "\n";
            layerDetailContent += '<div style=" border: 1px solid #cccccc; width: 80%">';

            if( "" != Trim(layerDetail[3]) )
            {
                layerDetailContent += '<div style="padding: 10px; background-color:#FFFEBB;word-wrap:break-word;">';
                layerDetailContent += "<span style='font-weight:bold;'>Details</span>";
                layerDetailContent += "<br/><br/>";
                layerDetailContent += layerDetail[3];
                layerDetailContent += "</div>";
            }

            layerDetailContent += '<div style="padding: 10px;">';
            layerDetailContent += "<span style='font-weight:bold;'>Filter</span>";
            layerDetailContent += "<br/><br/>";

            //If no filter was applied, then "No filter was applied to this layer." is displayed.
            if( "" == Trim(layerDetail[1]) )
            {
                layerDetailContent += "No filter was applied to this layer.";
            }
            else
            {
                layerDetailContent += layerDetail[1];
            }

            layerDetailContent += "</div>";
            layerDetailContent += '<div style=" background-color: #EEEEEE;padding: 10px;word-wrap:break-word;">';
            layerDetailContent += "<span style='font-weight:bold;'>Scale Range</span>";
            layerDetailContent += "<br/><br/>";
            layerDetailContent += layerDetail[2];
            layerDetailContent += "</div>" + "\n";
            layerDetailContent += "</div>" + "\n";

            //remove the layer detail info if it already exist
            var layerInfo = document.getElementById("layerDetailContentDiv");

            if (layerInfo)
            {
                layerInfo.parentNode.removeChild(layerInfo);
            }

            var layerDetailDiv = document.createElement("div");
            layerDetailDiv.id="layerDetailContentDiv";
            layerDetailDiv.innerHTML = layerDetailContent;

            //the placeholder for the layer detail info
            var layerInfoDetail =document.getElementById("layerInfoDetail");
            layerInfoDetail.appendChild(layerDetailDiv);
        }

        //sort layers information table part
        var SortLayers = {
            //reverse the table rows
            reverse: function(tBody)
            {
                // reverse the rows in a tbody
                newRows = [];
                var i = 0;
                for (; i < tBody.rows.length; i++)
                {
                    newRows[i] = tBody.rows[i];
                }

                var j=1;

                i = newRows.length - 1;
                for (; i >= 0; i--)
                {
                    tBody.appendChild(newRows[i]);

                    if(0 == j%2)
                    {
                        newRows[i].className="even";
                    }
                    else
                    {
                        newRows[i].className="odd";
                    }

                    j++;
                }
                delete newRows;
            },

            sortByColumn: function(headerColumn)
            {
                //get the table body of the layers information table
                var layersTBody = document.getElementById("layerResultsTable").tBodies[0];

                if (headerColumn.className.search(/\blayersSorted\b/) != -1)
                {
                    // if we're already sorted by this column, just reverse the table
                    SortLayers.reverse(layersTBody);
                    headerColumn.className = headerColumn.className.replace("layersSorted","layersSortedReverse");

                    headerColumn.removeChild(document.getElementById('layersSortDown'));

                    layersSortUp = document.createElement("span");
                    layersSortUp.id = "layersSortUp";
                    layersSortUp.innerHTML =  "&nbsp;<image src='images/arrow_up.png' alt='&#x25B4;'/>";
                    headerColumn.appendChild(layersSortUp);

                    return;
                }

                if (headerColumn.className.search(/\blayersSortedReverse\b/) != -1)
                {
                    // if we're already sorted by this column, just reverse the table
                    SortLayers.reverse(layersTBody);
                    headerColumn.className = headerColumn.className.replace("layersSortedReverse","layersSorted");

                    headerColumn.removeChild(document.getElementById('layersSortUp'));

                    layersSortDown = document.createElement('span');
                    layersSortDown.id = "layersSortDown";
                    layersSortDown.innerHTML = "&nbsp;<image src='images/arrow_down.png' alt='&#x25BE;'/>";
                    headerColumn.appendChild(layersSortDown);

                    return;
                }

                // remove layersSorted classes
                layerHeadRow = headerColumn.parentNode;
                var i = 0;
                for( ; i < layerHeadRow.childNodes.length; i++)
                {
                    cell = layerHeadRow.childNodes[i];
                    if (cell.nodeType == 1)// an element
                    {   
                        cell.className = cell.className.replace('layersSortedReverse', '');
                        cell.className = cell.className.replace('layersSorted', '');
                    }
                }

                //remove the icon sort down or up if it already exist
                var layersSortDown = document.getElementById('layersSortDown');

                if (layersSortDown)
                {
                    layersSortDown.parentNode.removeChild(layersSortDown);
                }

                var layersSortUp = document.getElementById('layersSortUp');

                if (layersSortUp)
                {
                    layersSortUp.parentNode.removeChild(layersSortUp);
                }

                //by default, we sort the talbe by ASC
                headerColumn.className = "layersSorted";
                layersSortDown = document.createElement('span');
                layersSortDown.id = "layersSortDown";
                layersSortDown.innerHTML = "&nbsp;<image src='images/arrow_down.png' alt='&#x25BE;'/>";
                headerColumn.appendChild(layersSortDown);

                //get the content need to be sorted into a 2-D array
                rowArray = [];

                colIndex = parseInt(headerColumn.getAttribute("columnindex")) - 1;
                rows = layersTBody.rows;
                var j = 0;
                for (; j < rows.length; j++)
                {
                    rowArray[rowArray.length] = [SortLayers.getInnerText(rows[j].cells[colIndex]), rows[j]];
                }

                //only the second column needs to be sorted by numeric, others are all by alpha
                if(1 == colIndex)
                {
                    rowArray.sort(SortLayers.sortByNumeric);
                }
                else
                {
                    rowArray.sort(SortLayers.sortByAlpha);
                }                     

                //reset the row alternate color after sorting
                tableBody = layersTBody;
                for (j = 0; j < rowArray.length; j++)
                {
                    tableBody.appendChild(rowArray[j][1]);

                    if(0 == j%2)
                    {
                        rowArray[j][1].className="odd";
                    }
                    else
                    {
                        rowArray[j][1].className="even";
                    }
                }

                delete rowArray;
            },

            getInnerText: function(node)
            {
                // gets the text we want to use for sorting for a cell.
                // strips leading and trailing whitespace.
                try
                {
                    if (node.getAttribute("sortKey") != null)
                    {
                        return node.getAttribute("sortKey");
                    }
                    else if (typeof node.textContent != 'undefined')
                    {
                        return node.textContent.replace(/^\s+|\s+$/g, '');
                    }
                    else if (typeof node.innerText != 'undefined')
                    {
                        return node.innerText.replace(/^\s+|\s+$/g, '');
                    }
                    else if (typeof node.text != 'undefined')
                    {
                        return node.text.replace(/^\s+|\s+$/g, '');
                    }
                    else
                    {
                        switch (node.nodeType)
                        {
                            case 4:
                                return node.nodeValue.replace(/^\s+|\s+$/g, '');
                                break;
                            case 1:
                            case 11:
                                var innerText = '';
                                var i = 0;
                                for (; i < node.childNodes.length; i++) {
                                    innerText += SortLayers.getInnerText(node.childNodes[i]);
                                }
                                return innerText.replace(/^\s+|\s+$/g, '');
                                break;
                            default:
                                return '';
                        }
                    }
                }
                catch(e)
                {
                    return '';
                }
            },

            // sort functions
            // a is an array with two elements, so as b
            // so you are comparing a[0] and b[0]
            sortByNumeric: function(a, b)
            {
                a0 = parseFloat(a[0].replace(/[^0-9.-]/g, ''));
                if (isNaN(a0))
                {
                    a0 = 0;
                }

                b0 = parseFloat(b[0].replace(/[^0-9.-]/g, ''));
                if (isNaN(b0))
                {
                    b0 = 0;
                }

                return a0 - b0;
            },

            //Making sorttable sort your columns case-insensitive
            sortByAlpha: function(a,b)
            {
                if (a[0].toLowerCase() == b[0].toLowerCase())
                {
                    return 0;                               
                }

                if (a[0].toLowerCase() < b[0].toLowerCase())
                {
                    return -1;
                }

                return 1;
            }
        }

        //recent Settings
        window.onload = LoadRecentSettings;

        function LoadRecentSettings()
        {
            var tempSettingJs = document.getElementById("recentSettingsJsArray");

            if(tempSettingJs)
            {
                //get js content in the hidden span
                var innerScript = tempSettingJs.innerHTML;
                innerScript = innerScript.replace(/\\n/,'');

                //check to remove the script if it already exists
                var insertScript = document.getElementById("recentSettingsJs");
                if(insertScript)
                {
                    document.getElementsByTagName("head").item(0).removeChild(insertScript);
                }

                //every new execute should generate new js array data
                insertScript = document.createElement("script");
                insertScript.type = "text/javascript";
                insertScript.id = "recentSettingsJs";
                insertScript.text = innerScript;

                document.getElementsByTagName("head").item(0).appendChild(insertScript);

                tempSettingJs.parentNode.removeChild(tempSettingJs);
            }

            SetSettingsWithCookie();
            //register the events handler for the center point textbox and scale textbox
            //input change
            InitialEvents();
        }

        function SetSettingsWithCookie()
        {
            var mapIdCookie = document.getElementById("cookieMapId");
            var centerPointCookie = document.getElementById("cookieCenterPoint");
            var scaleCookie = document.getElementById("cookieScale");

            if(mapIdCookie)
            {
                var centerPoint = document.getElementById("txtCenterPoint");
                centerPoint.value = centerPointCookie.value;

                var scaleInput = document.getElementById("txtScale");
                scaleInput.value = FormatNumber(scaleCookie.value,4);

                var mapDefinitonSelector = document.getElementById("mapSelector_DO_NOT_PERSIST");
                mapDefinitonSelector.value = Trim(mapIdCookie.value);

                var tipDiv = document.getElementById("mapResourceNameTip");
                tipDiv.innerHTML = mapDefinitonSelector.value;

                var settingsBtn = document.getElementById("mapViewerBtn");
                centerPoint.removeAttribute("disabled");
                scaleInput.removeAttribute("disabled");
                settingsBtn.removeAttribute("disabled");
                SetRunButtonState(true);
            }
        }

        function RecentSettingClicked(settingId)
        {
            var settingsCount = recentSettings.length;
            var selectSetting = null;

            var i = 0;
            for(; i < settingsCount; i++)
            {
                if( Trim(settingId) == Trim(recentSettings[i][0]) )
                {
                    selectSetting = recentSettings[i];
                    break;
                }
            }

            if(null != selectSetting)
            {
                var mapDefinitonSelector = document.getElementById("mapSelector_DO_NOT_PERSIST");
                var i = 0;
                var found = false;
                for(;i < mapDefinitonSelector.options.length;i++)
                {
                    var mapId = Trim(mapDefinitonSelector.options[i].value);
                    if(Trim(selectSetting[1]) == mapId)
                    {
                        found =  true;
                    }
                }

                if(found)
                {
                    mapDefinitonSelector.value = Trim(selectSetting[1]);
                }
                else
                {
                    alert("The selected map resource is no longer available. Select a different map resource to continue.");
                    return;
                }

                var centerPoint = document.getElementById("txtCenterPoint");
                if(centerPoint.detachEvent)
                {
                    centerPoint.detachEvent("onpropertychange",CenterPointPropertyChange);
                }
                centerPoint.value = selectSetting[2];
                if(centerPoint.attachEvent)
                {
                    centerPoint.attachEvent("onpropertychange",CenterPointPropertyChange);
                }

                ValidateCenterPoint(false);

                var scaleInput = document.getElementById("txtScale");
                if(scaleInput.detachEvent)
                {
                    scaleInput.detachEvent("onpropertychange",ScalePropertyChange);
                }
                scaleInput.value = FormatNumber(selectSetting[3],4);
                if(scaleInput.attachEvent)
                {
                    scaleInput.attachEvent("onpropertychange",ScalePropertyChange);
                }

                ValidateScale(false);

                var tipDiv = document.getElementById("mapResourceNameTip");
                tipDiv.innerHTML = mapDefinitonSelector.value;

                var settingsBtn = document.getElementById("mapViewerBtn");

                centerPoint.removeAttribute("disabled");
                scaleInput.removeAttribute("disabled");
                settingsBtn.removeAttribute("disabled");

                SetRunButtonState(true);

                ShowReportWarningMsg();
            }
        }
    </script>
    <div id="bgDiv" class="bgDivStyle"></div>
    <table cellspacing="0" cellpadding="0" border="0" class="main">
        <tr>
            <?php DisplaySiteHeader( false, true, true, true, $formName, $homePage ); ?>
        </tr>
        <tr>
            <?php DisplayLefthandSash( $menuCategory ); ?>

            <!-- Contents Area -->
            <?php BeginContentArea( $formName ); ?>

            <?php
                  DeclareHiddenVars( $pageName );
                  DisplayTitleBar( $pageTitle, $helpPage );           
                  DisplayConfirmationMsg( $confirmationMsg );
                  DisplayErrorMsg( $errorMsg );                
            ?>
            <div id="settingsTitle">
             <table>
                 <tr>
                     <td style=" width: 8px;">
                         <img src="images/arrow_down.png" alt="down" style="cursor:pointer;"
                              id="settings_CollapseImage_ID" onclick="CollapsibleTabClick('settings_CollapseImage_ID','settingsContent')"/>
                     </td>
                     <td style="font-size:18px;  font-weight: bold; text-align: left;"  >
                         <span style="cursor:pointer;" onclick="CollapsibleTabClick('settings_CollapseImage_ID','settingsContent')">Settings</span>
                     </td>
                 </tr>
             </table>
            </div>
            <?php
                //get settings in cookie
                if(isset($_COOKIE['c_mapResourceId'],$_COOKIE['c_centerPoint'],$_COOKIE['c_scale']))
                {
                    echo "<span style='display:none;'>";
                    echo '<input type="hidden" id="cookieMapId" value="'.$_COOKIE['c_mapResourceId'].'"/>';
                    echo '<input type="hidden" id="cookieCenterPoint" value="'.$_COOKIE['c_centerPoint'].'"/>';
                    echo '<input type="hidden" id="cookieScale" value="'.$_COOKIE['c_scale'].'"/>';
                    echo "</span>";
                }
            ?>
        <div>
        </div>
            <div id="settingsContent">
                <table style="width:100%;">
                    <tr>
                        <td style="width: 70%">
                            <div >
                                <table>
                                    <tr>
                                        <td colspan="2" style=" font-size:14px;">
                                            Follow the Steps to create a new report or choose from the recent settings.
                                        </td>
                                    </tr>
                                </table>
                            </div>
                            <div id="settingContentLeft" style="border:solid 1px #FFFFFF; margin-top: 10px;">
                                <table style="text-align: left; vertical-align:text-top; width: 100%">
                                    <tr>
                                        <td>
                                            <div style="font-size: 16px; margin-bottom: 5px; font-weight: bold;">Step 1</div>
                                            <div style=" background-color: #EEEEEE; padding-top: 2px; padding-left: 15px; padding-bottom: 15px;">
                                                <p>&nbsp;Select a map resource name.</p>
                                                <span style=" font-weight: bold;">&nbsp;Map Resource Name</span>
                                                    <table>
                                                        <tr>
                                                            <td>
                                                                <select name="mapSelector_DO_NOT_PERSIST" id="mapSelector_DO_NOT_PERSIST" style="width:180px; color: #2c4e56; padding: 5px;" size="5"
                                                                        onchange="MapResoucesNameSelectChange()">
                                                                   <?php
                                                                       //<option value="Library://Samples/Sheboygan/Maps/Sheboygan.MapDefinition">Sheboygan</option>
                                                                       for ($i = 0; $i < count($mapResources); $i++)
                                                                       {
                                                                            echo "<option value='";
                                                                            echo $mapResources[$i]["FullNames"];
                                                                            echo "'>";
                                                                            echo $mapResources[$i]["ShortNames"];
                                                                            echo "</option>";
                                                                       }
                                                                   ?>
                                                                </select>
                                                            </td>
                                                            <td style=" padding: 2px 10px; text-align: left; vertical-align: top;">
                                                                <div id="mapResourceNameTip">
                                                                </div>
                                                                <div id="selectMapResourceWarningMessage" style="color:#F03136;">
                                                                </div>
                                                                <input type="hidden" id="lastRunMapId" value=""/>
                                                                <input type="hidden" id="lastRunCenterPoint" value=""/>
                                                                <input type="hidden" id="lastRunScale" value=""/>
                                                            </td>
                                                        </tr>
                                                    </table>
                                            </div>
                                        </td>
                                    </tr>
                                    <tr>
                                        <td style="padding-top: 15px;">
                                            <div style="font-size: 16px; margin-bottom: 5px; font-weight: bold;">Step 2</div>
                                            <div style=" background-color: #EEEEEE; padding-top: 10px; padding-left: 15px; padding-bottom: 15px;">
                                                <table>
                                                    <tr>
                                                        <td colspan="2" style=" padding-bottom: 15px;">
                                                            <div style=" margin-bottom: 5px;">
                                                                Choose a center point and scale from the map viewer or enter the values manually.
                                                            </div>
                                                            <input type="button" id="mapViewerBtn" value="Select Settings..." onClick="SelectMapSettings();"  disabled="disabled" style="width:160px; height: 28px;font-weight: bold;">
                                                            <div id="mapViewerDialog" align="center" class="mapViewerDialogStyle" onkeydown="MapViewerKeyUp(event);">
                                                                <table width="100%" style="margin:0px; padding: 0px;" cellspacing="0" cellpadding="0">
                                                                    <tr>
                                                                        <td style="border-bottom:1px solid #000000; " >
                                                                            <span id="msgTitle" style=" cursor: default;margin:0px; padding:3px; height:24px; text-align: left; font-weight: bold; vertical-align: middle;">
                                                                            Select a Center Point and Map Scale
                                                                            </span>
                                                                        </td>
                                                                        <td style=" text-align: right;border-bottom:1px solid #000000; cursor: default;">
                                                                            <img alt="Close" src="images/close.png" onclick="CloseMapViewer();"/>
                                                                            <input type="text" id="mapViewerAutoFocusTxt" style="width: 0px; height:0px; border: none; cursor:default;"/>
                                                                        </td>
                                                                    </tr>
                                                                    <tr>
                                                                        <td colspan="2">
                                                                            <iframe name="mapViewerFrame" src="" width="800px" height="575px" scrolling="no"
                                                                                    id="mapViewerFrame" style="border:none;">
                                                                            </iframe>
                                                                        </td>
                                                                    </tr>
                                                                </table>
                                                            </div>
                                                            <!--System should load map by the previous center point and scale-->
                                                            <!--if this map was opened by 'Select Settings' before-->
                                                            <!--So these inputs are used to save the settings of last open-->
                                                            <input type="hidden" id="mapViewerLastOpenMapId" value=""/>
                                                            <input type="hidden" id="mapViewerLastOpenCenterPoint" value=""/>
                                                            <input type="hidden" id="mapViewerLastOpenScale" value=""/>
                                                        </td>
                                                    </tr>
                                                    <tr>
                                                        <td style="padding:5px;">
                                                            <div style=" margin-bottom: 5px; font-weight: bold;">
                                                                Center Point
                                                            </div>
                                                            <input type="textbox" name="txtCenterPoint" id="txtCenterPoint" style="width:220px;"
                                                                   onFocus="CenterPointFocus();" value="Enter center point" disabled="disabled"/>
                                                        </td>
                                                        <td style="padding:5px; width: 60%">
                                                            <div style=" margin-bottom: 5px; font-weight: bold;">
                                                                Scale
                                                            </div>
                                                            <label for="txtScale">1: &nbsp;&nbsp;</label>
                                                            <input type="textbox" name="txtScale" id="txtScale" onFocus="ScaleFocus();"
                                                                   value="Enter scale" disabled="disabled"/>
                                                        </td>
                                                    </tr>
                                                    <tr>
                                                        <td style="height:18px;">
                                                            <span id="centerPointWarnMessage" style="color: #F03136; padding-left: 5px;">
                                                            </span>
                                                        </td>
                                                        <td>
                                                            <span id="scaleWarnMessage" style="color: #F03136; padding-left: 5px;">
                                                            </span>
                                                        </td>
                                                    </tr>
                                                </table>
                                            </div>
                                        </td>
                                    </tr>
                                    <tr>
                                        <td style="padding-top: 80px;">
                                            <table style="margin: 0px; padding: 0px;" cellspacing="0" cellpadding="0">
                                                <tr>
                                                    <td style="padding-right:5px;">
                                                        <input id="runBtn" type="button" value="Run" style="width:80px; font-weight: bold;height: 28px;"
                                                               onclick="GetResult();" disabled="disabled"/>
                                                    </td>
                                                    <td>
                                                        <span id="ajax_loading_img" style="display:none;">
                                                            <img src="images/ajax_loading.gif" alt="loading"/>
                                                        </span>
                                                    </td>
                                                    <td>
                                                        <span style="margin-left: 25px;" id="btnClearSpan">
                                                            <input type="button" value="Clear" onclick="ClearSettings();"
                                                                   name="Clear" style="width:80px; font-weight: bold;height: 28px;">
                                                        </span>
                                                    </td>
                                                </tr>
                                            </table>
                                        </td>
                                    </tr>
                                    <tr>
                                        <td>
                                            <div id="ResultNotMatchWrn" >
                                                <span style="font-size:12pt; font-weight: bold;">
                                                    The performance report results no longer match the settings found in Steps 1 and 2.
                                                </span>
                                                <p style="padding-left: 3px; margin-bottom: 8px; margin-top: 6px">
                                                    Return the settings found in Steps 1 and 2 to the previous settings or run a new performance report.
                                                </p>
                                            </div>
                                        </td>
                                    </tr>
                                </table>
                            </div>
                        </td>
                        <td style=" vertical-align: top; width: 30%; padding-left: 15px;">
                            <span id="recentSettingsJsArray" style="display:none;">
                                <?php
                                    $displayManager->OutputSettingsJsArray($recentSettings->recentSettings);
                                ?>
                            </span>
                            <div style="font-size:18px;  font-weight: bold; text-align: left;">Recent Settings</div>
                            <div style="border: 1px solid #CCCCCC; width: 100%; margin-top: 15px;" id="settingsDiv">
                                <div id="recentSettingsDiv">
                                    <?php
                                        $displayManager->OutputRecentSettings($recentSettings->recentSettings);
                                    ?>
                                </div>
                            </div>
                        </td>
                    </tr>
                </table>
            </div>
            
            <div id="resultsTab" style=" display: none;">
            </div>        
            <?php EndContentArea( true, $formName, "" ); ?>
            <!-- End of Contents Area -->
            <!-- This form is used to export the profiling results as .csv file -->
            <!-- If we put the form inside the innerHTML and inside another form, the Chrome and safari will ignore this form-->
            <form id="getCSVFileForm" action="performanceReport_Export.php" method="post" >
            </form>
        </tr>
    </table>
</body>
</html>

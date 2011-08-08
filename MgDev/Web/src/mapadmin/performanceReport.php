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
    $helpPage = 'HelpDocs/performanceReport.htm';
    $pageName = 'PerformanceReportPage';
    $formName = 'PerformanceReportForm';
    $homePage = NULL;
    $confirmationMsg = "";
    $errorMsg = "";
    $mapProfileResult=new MapProfileResult();
    $mapResources;
    $mapResourceShortNames;
    $displayManager= new DisplayProfileResultManager();

    function GetAllMapResources()
    {
        try
        {
            global $site;
            global $userInfo;
            global $mapResources;
            global $mapResourceShortNames;
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
                $mapResources[$i] = $mapResourceID;
                $mapResourceShortNames[$i] = $shortMapName;
            }

            array_multisort($mapResourceShortNames, $mapResources);
        }
        catch (Exception $exc)
        {
            $errorMsg = $exc->getMessage();
        }
    }

    GetAllMapResources();
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
                    width: 20%;
                    border:1px solid #CCCCCC;
                    background-color:#EEEEEE;
                    border-bottom: none;
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
                    padding:.5em;
                    border:1px solid #CCCCCC;
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

                .layerResultsStyle th.over, .layerResultsStyle tr.even th.over,.layerResultsStyle tr.odd th.over
                {
                    background:#b4c6de;
                }


                .layerResultsStyle td.over,.layerResultsStyle tr.even td.over,.layerResultsStyle tr.odd td.over
                {
                    background:#b4c6de;
                }

                /* use this if you want to apply different styleing to empty table cells*/
                .layerResultsStyle td.empty,.layerResultsStyle tr.odd td.empty,.layerResultsStyle tr.even td.empty
                {
                    background:#FFFFFF;
                }

                #layerBody
                {
                     Height:    200px;
                     margin:    0px;
                     padding:   0px;
                     overFlow:  auto;
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
                    white-space: normal;
                    font-weight: normal;
                }

                .mapNameStyle
                {
                    background: #EEEEEE url('images/mapDefinitionNameInfo.png') no-repeat right;
                    padding-right: 20px;
                    cursor:default;
                }

                .warnMsgStyle
                {
                    border-color: red;
                }

                .bgDivStyle
                {
                    position:fixed;
                    left:0;
                    top:0;
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

            </style>
        
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
                            var _x = 0, _y = 0;
                            do
                            {
                                _x += el.offsetLeft;
                                _y += el.offsetTop;
                            } while ((el = el.offsetParent));

                            return { x: _x, y: _y };
                        }
                    }

                function CollapsibleTabClick(tabId,contentId)
                {
                    var collapseImage = document.getElementById(tabId);
                    var content = document.getElementById(contentId);

                    if (collapseImage.alt == "down")
                    {
                        collapseImage.alt = "left";
                        collapseImage.src = "images/testImages/left_18.jpg";
                        content.style.display = "none";
                    }
                    else
                    {
                        collapseImage.alt = "down";
                        collapseImage.src = "images/testImages/down_17.jpg";
                        content.style.display = "block";
                    }
                }

                function CollapseSettingTab()
                {
                    var collapseImage = document.getElementById("settings_CollapseImage_ID");
                    var content = document.getElementById("settingsContent");

                    collapseImage.alt = "left";
                    collapseImage.src = "images/testImages/left_18.jpg";

                    content.style.display = "none";
                }

                function SetResultNotMatchWarningMsg(visible)
                {
                    var wrnMsg=document.getElementById("ResultNotMatchWrn");
                    if(visible)
                    {
                        wrnMsg.style.display = "block";
                    }
                    else
                    {
                        wrnMsg.style.display = "none";
                    }
                }

                function ShowReportWarningMsg()
                {
                    var reportTab=document.getElementById("resultsTab");
                    if(reportTab.style.display=="block")
                    {
                        SetResultNotMatchWarningMsg(true);
                    }
                }

                function ExpandResultsTab()
                {
                    var content = document.getElementById("resultsTab");
                    content.style.display = "block";
                }

                function MapResoucesNameSelectChange()
                {
                    var tipDiv=document.getElementById("mapResourceNameTip");
                    var mapDefinitonSelector=document.getElementById("mapSelector_DO_NOT_PERSIST");

                    var settingsBtn=document.getElementById("mapViewerBtn");
                    var centerPoint=document.getElementById("txtCenterPoint");
                    var scale=document.getElementById("txtScale");

                    //If another map is selected, then the center point and scale should be cleared of values,
                    //that is return to default values “enter center point”  and “enter scale”.
                    if(centerPoint.value!="Enter center point")
                    {
                        centerPoint.value="Enter center point";
                    }

                    if( scale.value!="Enter scale")
                    {
                        scale.value="Enter scale";
                    }

                    //show the map definition ID by the side of the map selector
                    if(mapDefinitonSelector.selectedIndex>=0)
                    {
                        tipDiv.innerHTML=mapDefinitonSelector.options[mapDefinitonSelector.selectedIndex].value;
                        //clear warning message
                        var mapSelectorWarnMsg=document.getElementById("selectMapResourceWarningMessage");
                        mapSelectorWarnMsg.innerHTML = "";

                        //text input Controls in Step 2 are enabled once a map resource is selected in Step 1.
                        centerPoint.removeAttribute("disabled");
                        scale.removeAttribute("disabled");
                        settingsBtn.removeAttribute("disabled");

                        var visible=ValidateScale(false);
                        if(visible)
                        {
                           visible=ValidateCenterPoint(false);
                        }

                        SetRunButtonState(visible);
                    }
                    else
                    {
                        tipDiv.innerHTML="";
                        //when user clear the settings, the mapselector is not seleced and the text area is disabled
                        settingsBtn.setAttribute("disabled","disabled");
                        centerPoint.setAttribute("disabled", "disabled");
                        scale.setAttribute("disabled", "disabled");

                        SetRunButtonState(false);
                    }

                    ClearWrnMsg();
                    ShowReportWarningMsg();
                }

                function ValidateScale(needFormat)
                {
                    var result=false;
                   
                    var scale=document.getElementById("txtScale");
                    var scaleWarnMsg=document.getElementById("scaleWarnMessage");
                    var scaleValue=RemoveSpace(scale.value);

                    if("" == scaleValue)
                    {
                        result=false;
                        scaleWarnMsg.innerHTML = "A map scale was not entered.";
                        scale.className="warnMsgStyle";
                    }
                    else
                    {
                        //positive float
                        var regFloat =/^(\+)?\d+(\.(\d)+)?$/;
                        result = regFloat.test(scaleValue);

                        if(!result)
                        {
                            scaleWarnMsg.innerHTML = "Not a valid map scale.";
                            scale.className="warnMsgStyle";
                        }
                        else
                        {
                            if(needFormat)
                            {
                                scale.value=formatNumber(scaleValue,2);
                            }
                                

                            scaleWarnMsg.innerHTML = "";
                            scale.className="";
                        }
                    }
                    return result;
                }

                function ValidateCenterPoint(needFormat)
                {
                    var result=false;

                    var centerPoint=document.getElementById("txtCenterPoint");
                    var centerPointWarnMsg=document.getElementById("centerPointWarnMessage");
                    var centerPointValue=RemoveSpace(centerPoint.value);

                    if("" == centerPointValue)
                    {
                        result=false;
                        centerPointWarnMsg.innerHTML = "A center point was not entered.";
                        centerPoint.className="warnMsgStyle";
                    }
                    else
                    {
                        try
                        {
                            var point=centerPointValue.split("*");
                            if(point.length!=2)
                            {
                                result=false;
                                centerPointWarnMsg.innerHTML = "Not a valid center point.";
                                centerPoint.className="warnMsgStyle";
                            }
                            else
                            {
                                var x=point[0];
                                var y=point[1];
                              
                                result=!(IsNotFloat(x)||IsNotFloat(y));

                                if(!result)
                                {
                                    centerPointWarnMsg.innerHTML = "Not a valid center point.";
                                    centerPoint.className="warnMsgStyle";
                                }
                                else
                                {
                                    if(needFormat)
                                    {
                                        centerPoint.value=formatCenterPoint(x,y,2);
                                    }
                                        
                                    centerPointWarnMsg.innerHTML = "";
                                    centerPoint.className="";
                                }
                            }
                        }
                        catch(e)
                        {
                            result=false;
                            centerPointWarnMsg.innerHTML = "Not a valid center point.";
                            centerPoint.className="warnMsgStyle";
                        }
                    }

                    return result;
                }

                function ScaleTxtKeyUp()
                {
                    var visible=ValidateScale(false);

                    if(visible)
                    {
                        var mapDefinitonSelector=document.getElementById("mapSelector_DO_NOT_PERSIST");
                        if(mapDefinitonSelector.selectedIndex>=0)
                        {
                            visible=ValidateCenterPoint(false);
                        }
                        else
                        {
                            visible=false;
                        }
                    }

                    SetRunButtonState(visible);
                }

                function CenterPointTxtKeyUp()
                {
                    var visible=ValidateCenterPoint(false);

                    if(visible)
                    {
                        var mapDefinitonSelector=document.getElementById("mapSelector_DO_NOT_PERSIST");
                        if(mapDefinitonSelector.selectedIndex>=0)
                        {
                            visible=ValidateScale(false);
                        }
                        else
                        {
                            visible=false;
                        }
                    }

                    SetRunButtonState(visible);
                }

                function ScaleFocus()
                {
                    var scale=document.getElementById("txtScale");

                    if( scale.value=="Enter scale")
                    {
                        scale.value="";
                    }
                }

                function CenterPointFocus()
                {
                    var centerPoint=document.getElementById("txtCenterPoint");

                    if( centerPoint.value=="Enter center point")
                    {
                        centerPoint.value="";
                    }
                }

                var checkMapFrameLoadedInterval = null;
                function SelectMapSettings()
                {
                    var mapFrame=document.getElementById("mapViewerFrame");
                    var mapDefinitonSelector=document.getElementById("mapSelector_DO_NOT_PERSIST");
                    if(mapDefinitonSelector.selectedIndex>=0)
                    {
                        mapFrame.src="performanceReport_MapViewer.php?mapDefinition="+mapDefinitonSelector.options[mapDefinitonSelector.selectedIndex].value;
                    }

                    var bgDiv=document.getElementById("bgDiv");
                    var mapViewerDiv=document.getElementById("mapViewerDialog");

                    bgDiv.style.display = "block";
                    SetBgDivSize(bgDiv);
                    
                    mapViewerDiv.style.display = "block";
                    SetMapViewerLoaction(mapViewerDiv);

                    checkMapFrameLoadedInterval = setInterval(CheckMapFrameLoaded, 100);
                }

                function SetBgDivSize(bgDiv)
                {
                    bgDiv.style.height = document.body.offsetHeight+"px";
                    bgDiv.style.width = document.body.offsetWidth+"px";
                }

                var mapViewerWidth=800;
                var mapViewerHeight=600;
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

                window.onresize=WindowResized;

                function WindowResized()
                {
                    var mapViewerDiv=document.getElementById("mapViewerDialog");

                    if("block" == mapViewerDiv.style.display.toLowerCase())
                    {
                        SetMapViewerLoaction(mapViewerDiv);
                    }

                    var bgDiv=document.getElementById("bgDiv");
                    if("block" == bgDiv.style.display.toLowerCase())
                    {
                        SetBgDivSize(bgDiv);
                    }
                }

                function CheckMapFrameLoaded()
                {
                    if(!IsMapFrameLoaded())
                        return;
                    
                    clearInterval(checkMapFrameLoadedInterval);
                    
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
                    var bgDiv=document.getElementById("bgDiv");
                    var msgDiv=document.getElementById("mapViewerDialog");

                    bgDiv.style.display = "none";
                    msgDiv.style.display="none";

                    //must destory the old page, if not, the CheckMapFrameLoaded
                    //will get the old page's frame and then the buttons will not be displayed in new page
                    var mapFrame=document.getElementById("mapViewerFrame");
                    mapFrame.src="";
                }

                var messageShorterInterval = null;
                
                function CreateButtons()
                {
                    var mapViewerFrame=window.frames["mapViewerFrame"];
                    
                    var mapFrame = mapViewerFrame.GetMapFrame();
                    var mapFrameDocument=mapFrame.document;

                    mapFrame.ShowMapMessage("Use the map controls in the toolbar above to specify the center point and scale.", "info");
                    //use setInterval to make sure that the message div has been created.
                    messageShorterInterval = window.setInterval(function(){ makeMessageShorter(mapFrame); }, 100);

                    mapSpace = mapFrameDocument.getElementById("mapSpace");
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
                    buttonPanelInnerHtml+='<button type="button" style="width: 57px; height: 22px; border-style:none;border: 1px solid #000000;" onclick="window.parent.parent.MapViewerBtnOKClicked()">OK</button>';
                    buttonPanelInnerHtml+="</td>";
                    buttonPanelInnerHtml+="<td>";
                    buttonPanelInnerHtml+='<button type="button" style="width: 57px; height: 22px; border-style:none;border: 1px solid #000000;margin-left:10px;" onclick="window.parent.parent.MapViewerBtnCloseClicked()">Cancel</button>';
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
                   var mapViewerFrame=window.frames["mapViewerFrame"];
                   var mapFrame = mapViewerFrame.GetMapFrame();
                   var center = mapFrame.GetCenter();
                   var scale = mapFrame.GetScale();

                   var centerPoint=document.getElementById("txtCenterPoint");
                   centerPoint.value=center.X+"*"+center.Y;
                   var centerPointValidate=ValidateCenterPoint(true);

                   var txtScale=document.getElementById("txtScale");
                   txtScale.value=scale;
                   var scaleValidate=ValidateScale(true);

                   var visible=centerPointValidate&&scaleValidate;
                   if(visible)
                   {
                       var mapDefinitonSelector=document.getElementById("mapSelector_DO_NOT_PERSIST");
                       if(mapDefinitonSelector.selectedIndex>=0)
                       {
                           visible=true;
                       }
                       else
                       {
                           visible=false;
                       }
                   }

                   SetRunButtonState(visible);

                   CloseMapViewer();
                }

                function makeMessageShorter(mapFrame)
                {
                    if(!mapFrame.mapMessage || !mapFrame.mapMessage.container)
                        return;
                    
                    window.clearInterval(messageShorterInterval);
                    // make it shorter
                    mapFrame.mapMessage.container.style.padding="1px";
                }

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

                function formatNumber(s, n)
                {
                   var n = n > 0 && n <= 20 ? n : 2;

                   var s = parseFloat((s + "").replace(/[^\d\.-]/g, "")).toFixed(n) + "";

                   var l = s.split(".")[0].split("").reverse() , r = s.split(".")[1];

                   var t = "";

                   for(i = 0; i < l.length; i ++ )
                   {
                      t += l[i] + ((i + 1) % 3 == 0 && (i + 1) != l.length ? " " : "");
                   }

                   return t.split("").reverse().join("") + "." + r;
                }

                function formatCenterPoint(x,y,fractionDigits)
                {
                    return parseFloat(x).toExponential(fractionDigits)+"*"+parseFloat(y).toExponential(fractionDigits);
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

                  for(var i = 0; i < str.length;i++)
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

                    for(var i = 0;i < len;i++)
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

                function HighlightRow(tableRow)
                {
                    for (var i = 0; i < tableRow.childNodes.length; i++)
                    {
                        var obj = tableRow.childNodes[i];
                        obj.className = "over";
                    }
                }

                function UnhighlightRow(tableRow)
                {
                    for (var i = 0;i < tableRow.childNodes.length;i++)
                    {
                        var obj = tableRow.childNodes[i];
                        obj.className = "";
                    }
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
                    var mapDefinitonSelector=document.getElementById("mapSelector_DO_NOT_PERSIST");

                    if(mapDefinitonSelector.selectedIndex<0)
                    {
                        var mapResourceWarningMessage=document.getElementById("selectMapResourceWarningMessage");
                        mapResourceWarningMessage.innerHTML="A map resource was not selected.";
                        return false;
                    }

                    if(!ValidateCenterPoint(true))
                        return false;

                    if(!ValidateScale(true))
                        return false;

                    SetResultNotMatchWarningMsg(false);

                    var ajax_loading_img=document.getElementById("ajax_loading_img");
                    ajax_loading_img.style.display="inline";

                    var btnClear=document.getElementById("btnClearSpan");
                    btnClear.style.display="none";

                    var runButton=document.getElementById("runBtn");
                    runButton.setAttribute("disabled", "disabled");

                    xmlHttp=GetXmlHttpObject();
                    if (xmlHttp==null)
                    {
                       alert ("Browser does not support HTTP Request!");
                       return;
                    }

                    var scale=document.getElementById("txtScale");
                    var mapSelector=document.getElementById("mapSelector_DO_NOT_PERSIST");
                    var url="performanceReport_GetResult.php";

                    url+="?scale="+scale.value;
                    url+="&mapDefinition="+mapSelector.value;
                    url+="&sid="+Math.random();
                    xmlHttp.onreadystatechange=stateChanged;
                    xmlHttp.open("POST",url,true);
                    xmlHttp.send(null);
                }

                function stateChanged()
                {
                    if ((4 == xmlHttp.readyState || "complete" == xmlHttp.readyState)&& 200 == xmlHttp.status)
                     {
                        var profileResult=document.getElementById("resultsTab");
                        profileResult.innerHTML=xmlHttp.responseText;

                        var btnClear=document.getElementById("btnClearSpan");
                        btnClear.style.display="inline";

                        var ajax_loading_img=document.getElementById("ajax_loading_img");
                        ajax_loading_img.style.display="none";
                        
                        CollapseSettingTab();
                        ExpandResultsTab();
                        SetRunButtonState(true);
                     }
                }
                
                //End AJAX part

                function exportCSV()
                {
                    var form=document.getElementById("getCSVFileForm");
                    form.submit();
                }

                function ClearSettings()
                {
                    //When Clear button clicked, removes the settings from
                    //map selector, scale textbox and center point textbox.
                    //Also removes any validation messages that are visible.
                    var mapDefinitonSelector=document.getElementById("mapSelector_DO_NOT_PERSIST");

                    if(mapDefinitonSelector.selectedIndex>=0)
                    {
                        mapDefinitonSelector.selectedIndex=-1;
                        //change the selectedIndex valule by the js will not fire the event "onchange",
                        //so we manually do it.
                        MapResoucesNameSelectChange();
                    }

                    //Also removes any validation messages that are visible.
                    ClearWrnMsg();

                    var mapSelectorWarnMsg=document.getElementById("selectMapResourceWarningMessage");
                    mapSelectorWarnMsg.innerHTML = "";
                 
                    SetRunButtonState(false);
                }

                function ClearWrnMsg()
                {
                    var scale=document.getElementById("txtScale");
                    scale.className="";

                    var centerPoint=document.getElementById("txtCenterPoint");
                    centerPoint.className="";

                    var scaleWarnMsg=document.getElementById("scaleWarnMessage");
                    scaleWarnMsg.innerHTML = "";

                    var centerPointWarnMsg=document.getElementById("centerPointWarnMessage");
                    centerPointWarnMsg.innerHTML = "";
                }

                function SetRunButtonState(visible)
                {                 
                    var runButton=document.getElementById("runBtn");

                    if(visible)
                    {
                        runButton.removeAttribute("disabled");
                    }
                    else
                    {
                        runButton.setAttribute("disabled", "disabled");
                    }
                }     
            </script>

            <div id="settingsTitle">
             <table>
                 <tr>
                     <td style=" width: 20px;">
                         <img src="images/testImages/down_17.jpg" alt="down" style="cursor:pointer;"
                              id="settings_CollapseImage_ID" onclick="CollapsibleTabClick('settings_CollapseImage_ID','settingsContent')"/>
                     </td>
                     <td style=" font-size:22px; color: orange; text-align: left;"  >
                         <span style="cursor:pointer;" onclick="CollapsibleTabClick('settings_CollapseImage_ID','settingsContent')">Settings</span>
                     </td>
                 </tr>
             </table>
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
                                                                            echo "<option value=";
                                                                            echo "'$mapResources[$i]'";
                                                                            echo ">";
                                                                            echo $mapResourceShortNames[$i];
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
                                                            <input type="button" id="mapViewerBtn" value="Select Settings" onClick="SelectMapSettings();"  disabled="disabled" style="width:160px; height: 28px;font-weight: bold;">
                                                            <div id="mapViewerDialog" align="center" class="mapViewerDialogStyle">
                                                                <table width="100%" style="margin:0px; padding: 0px;" cellspacing="0" cellpadding="0">
                                                                    <tr>
                                                                        <td style="border-bottom:1px solid #000000; " >
                                                                            <span id="msgTitle" style=" cursor: default;margin:0px; padding:3px; height:24px; text-align: left; font-weight: bold; vertical-align: middle;">
                                                                            Select a Center Point and Map Scale
                                                                            </span>
                                                                        </td>
                                                                        <td style=" text-align: right;border-bottom:1px solid #000000;">
                                                                            <img alt="Close" src="images/close.png" onclick="CloseMapViewer();"/>
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
                                                        </td>
                                                    </tr>
                                                    <tr>
                                                        <td style="padding:5px;">
                                                            <div style=" margin-bottom: 5px; font-weight: bold;">
                                                                Center Point
                                                            </div>
                                                            <input type="textbox" name="txtCenterPoint" id="txtCenterPoint" style="width:220px;"
                                                                   onFocus="CenterPointFocus();" onKeyUp="CenterPointTxtKeyUp();" value="Enter center point" disabled="disabled"/>
                                                        </td>
                                                        <td style="padding:5px; width: 60%">
                                                            <div style=" margin-bottom: 5px; font-weight: bold;">
                                                                Scale
                                                            </div>
                                                            <label for="txtScale">1: &nbsp;&nbsp;</label>
                                                            <input type="textbox" name="txtScale" id="txtScale" onFocus="ScaleFocus();"
                                                                   onKeyUp="ScaleTxtKeyUp();" value="Enter scale" disabled="disabled"/>
                                                        </td>
                                                    </tr>
                                                    <tr>
                                                        <td>
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
                                        <td style=" padding-top: 80px;">
                                            <table>
                                                <tr>
                                                    <td style="padding-right:5px;">
                                                        <input id="runBtn" type="button" value="Run" style="width:80px; font-weight: bold;height: 28px;"
                                                               onclick="GetResult();" disabled="disabled"/>
                                                    </td>
                                                    <td>
                                                        <span id="ajax_loading_img" style="display:none;">
                                                            <input type="image" src="images/ajax_loading.gif"></span>
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
                                            <div id="ResultNotMatchWrn"
                                                 style="border: 3px solid #CCCCCC;background: #FFFEBB url('images/warning.png') no-repeat left top;padding:0px 0px 10px 20px; margin-top: 10px;display: none;">
                                                <span style="font-size:12pt; font-weight: bold;">
                                                    The performance report results no longer match the settings found in Steps 1 and 2.
                                                </span>
                                                <br/>
                                                Return the settings found in Steps 1 and 2 to the previous settings or run a new performance report.
                                            </div>
                                        </td>
                                    </tr>
                                </table>
                            </div>
                        </td>
                        <td style=" vertical-align: top; width: 30%;">
                            <h2>Recent Settings</h2>
                            <table style="padding: 5px;">
                                <tr>
                                    <td>
                                        CEVEK
                                        <br>
                                        MAY 12,2012&nbsp;&nbsp;&nbsp;&nbsp;11:35:52
                                    </td>
                                </tr>
                                <tr>
                                    <td style=" background-color: #EEEEEE;">
                                        CEVEK
                                        <br>
                                        MAY 12,2012&nbsp;&nbsp;&nbsp;&nbsp;11:35:52
                                    </td>
                                </tr>
                                <tr>
                                    <td>
                                        CEVEK
                                        <br>
                                        MAY 12,2012&nbsp;&nbsp;&nbsp;&nbsp;11:35:52
                                    </td>
                                </tr>
                                <tr>
                                    <td style=" background-color: #EEEEEE;">
                                        CEVEK
                                        <br>
                                        MAY 12,2012&nbsp;&nbsp;&nbsp;&nbsp;11:35:52
                                    </td>
                                </tr>
                                <tr>
                                    <td>
                                        CEVEK
                                        <br>
                                        MAY 12,2012&nbsp;&nbsp;&nbsp;&nbsp;11:35:52
                                    </td>
                                </tr>
                            </table>
                        </td>
                    </tr>
                </table>
            </div>
            <div id="resultsTab" style=" display: none;">
            </div>        
            <?php EndContentArea( true, $formName, "" ); ?>
            <!-- End of Contents Area -->
        </tr>
    </table>
</body>
</html>

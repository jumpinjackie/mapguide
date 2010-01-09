<?php

//
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
//

    try
    {
        include 'resizableadmin.php';

        LoadSessionVars();

        CheckForPopupRegistration();

        define( 'DOWNLOAD_ACTION',      "Download" );
        define( 'CLOSE_ACTION',         "Close" );
        define( 'REFRESH_ACTION',       "Refresh" );
        define( 'START_AUTO_ACTION',    "Start Auto-Refresh" );
        define( 'STOP_AUTO_ACTION',     "Stop Auto-Refresh" );

        // Define Local values
        $pageTitle = 'View Current Log';
        $helpPage = 'HelpDocs/configuring_and_viewing_logs.htm';
        $formName = 'ViewCurrentLogTopForm';
        $pageName = 'ViewCurrentLogTopPage';
        $selectedServerName = '';
        $logToViewID = 'logToView';
        $logToView = '';
        $logFileToDownloadID = 'logFileToDownload';
        $logFileToDownload = "";
        $autoRefresh = false;
        $limitRefresh = false;
        $numRecsToRefresh = 10;
        $autoRefreshFrequency = 5;
        $limitRefreshID = "limitRefresh";
        $numRecsToRefreshID = 'numRecsToRefresh';
        $autoRefreshFrequencyID = "autoRefreshFrequency";
        $actionID = 'action';
        $action = REFRESH_ACTION;
        $enabledStr = "";
        $confirmationMsg = "";
        $errorMsg = "";

        // Get inputs
        if ( array_key_exists( $logToViewID, $_GET ) )
            $logToView = $_GET[ $logToViewID ];
        else
        if ( array_key_exists( $logToViewID, $_POST ) )
            $logToView = $_POST[ $logToViewID ];
        if ( empty( $logToView ) )
            throw new Exception( $errNoLogSpecified );

        if ( array_key_exists( $actionID, $_POST ) )
            $action = $_POST[ $actionID ];
        $autoRefresh = $action == START_AUTO_ACTION;
        $enabledStr = $autoRefresh ? " disabled " : "";

        if ( array_key_exists( $limitRefreshID, $_POST ) )
            $limitRefresh = true;

        if ( array_key_exists( $numRecsToRefreshID, $_POST ) )
            $numRecsToRefresh = $_POST[ $numRecsToRefreshID ];

        if ( array_key_exists( $autoRefreshFrequencyID, $_POST ) )
            $autoRefreshFrequency = $_POST[ $autoRefreshFrequencyID ];

        // Create a ServerAdmin object and get additional data from server
        $serverAdmin = new MgServerAdmin();
        $serverAdmin->Open( $selectedServer, $userInfo );
        $logFileToDownload = GetActiveLogFilename( $serverAdmin, $logToView );
        $selectedServerName = GetServerName( $selectedServer );

        // Set page title
        $pageTitle = 'View Current '.$logLabels[ $logToView ].' Log ';
    }
    catch ( MgException $e )
    {
        CheckForFatalMgException( $e );
        if ( empty( $errorMsg ) )
            $errorMsg = $e->GetMessage();
    }
    catch ( Exception $e )
    {
        if ( empty( $errorMsg ) )
            $errorMsg = $e->getMessage();
    }

?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>

<head>
    <?php
        OutputHeader( $pageTitle );
    ?>
</head>

<?php
    $onLoadStr = "";
    if ( FirstTimeHere( $pageName ) )
    {
        $valuesToPersist = array();
        $valuesToPersist[ $logToViewID ] = $logToView;
        RegisterPopup( $pageName, false, $valuesToPersist );
    }
    else
    if ( $action == DOWNLOAD_ACTION )
        $onLoadStr = "DownloadLogFile('".$logFileToDownloadID."','".$logFileToDownload."' )";
    else
    if ( $action == REFRESH_ACTION || $autoRefresh )
        $onLoadStr = "window.open( 'viewcurrentlogbottom.php?".$logToViewID."=".$logToView."', 'ViewCurrentLogBottom' )";
?>

<body class="ViewCurrentLogTop" onLoad="<?php echo $onLoadStr ?>">

    <form name="<?php echo $formName?>" action="<?php echo $_SERVER['PHP_SELF']?>" method="post" onSubmit="PersistAllInputFields();">
    <?php DeclareHiddenVars( $pageName ); ?>

    <input type="hidden" name="<?php echo $logToViewID ?>" value="<?php echo $logToView ?>">

    <table border="0" cellspacing="0" class="inputForm">
        <tr>
            <td class="logDisplayForm" colspan="2">
                <?php DisplayTitleBar( $pageTitle, $helpPage ); ?>
            </td>
        </tr>
        <tr>
            <td class="inputFormLabel">
                Server:&nbsp;&nbsp;<?php echo $selectedServerName ?><br>
            </td>
            <td align="right">
                <input type="submit" value="<?php echo CLOSE_ACTION ?>"  onClick="parent.close();" >
            </td>
        </tr>
        <tr>
            <td colspan="2">&nbsp;</td>
        </tr>
        <tr>
            <td  class="inputFormLabel" colspan="2">
                <input type="checkbox" name="<?php echo $limitRefreshID ?>" <?php if ( $limitRefresh ) echo 'checked' ?> <?php echo $enabledStr ?>  >
                &nbsp;View only the last&nbsp;
                <input type="text" name="<?php echo $numRecsToRefreshID ?>" value="<?php echo $numRecsToRefresh ?>" <?php echo $enabledStr ?> size="3" >
                &nbsp;records
            </td>
        </tr>
        <tr>
            <td class="inputFormLabel" colspan="2">
                Auto-refresh every&nbsp;
                <input type="text" name="<?php echo $autoRefreshFrequencyID ?>" value="<?php echo $autoRefreshFrequency ?>"  <?php echo $enabledStr ?> size="3" >
                &nbsp;seconds
            </td>
        </tr>
        <tr>
            <td colspan="2">&nbsp;</td>
        </tr>
        <tr>
            <td align="left" valign="bottom" colspan="2" >
                <input type="hidden" name="<?php echo $actionID ?>" value="">
                <?php
                    // Toolbar
                    $autoAction = $autoRefresh ? STOP_AUTO_ACTION : START_AUTO_ACTION;
                    $refreshIcon = $autoRefresh ? "images/refresh_disabled.gif" : "images/refresh.gif";
                    $autoIcon = $autoRefresh ? "images/stop.gif" : "images/startautorefresh.gif";
                    $downloadIcon = $autoRefresh ? "images/save_disabled.gif" : "images/save.gif";
                    $buttons = array();
                    $button = new ToolbarButtonRecord();
                        $button->label = REFRESH_ACTION;
                        $button->icon = $refreshIcon;
                        $button->action = "SetElementValue('".$actionID."', '".REFRESH_ACTION."' );";
                        $button->disabled = $autoRefresh;
                    $buttons[0] = $button;
                    $button = new ToolbarButtonRecord();
                        $button->label = $autoAction;
                        $button->icon = $autoIcon;
                        $button->action = "SetElementValue('".$actionID."', '".$autoAction."' );";
                    $buttons[1] = $button;
                    $button = new ToolbarButtonRecord();
                        $button->label = DOWNLOAD_ACTION;
                        $button->icon = $downloadIcon;
                        $button->action = "SetElementValue('".$actionID."', '".DOWNLOAD_ACTION."' );";
                        $button->disabled = $autoRefresh;
                    $buttons[2] = $button;
                    DisplayStandaloneToolbar( $buttons, $formName, 'ViewCurrentLogToolbar' );
                ?>
            </td>
        </tr>
    </table>

    <?php
        $errorMsg = "";
        SaveSessionVars();
    ?>
    </form>

</body>

</html>

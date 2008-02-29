<?php

//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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

        define( 'CLOSE_ACTION',         "Close" );
        define( 'REFRESH_ACTION',       "Refresh" );
        define( 'START_AUTO_ACTION',    "Start Auto-Refresh" );
        define( 'STOP_AUTO_ACTION',     "Stop Auto-Refresh" );

        // Define Local values
        $pageTitle = 'View Server Status';
        $helpPage = 'HelpDocs/monitor_server_status.htm';
        $pageName = 'ViewServerStatusPage';
        $formName = 'ViewServerStatusForm';
        $selectedServerName = '';
        $autoRefresh = false;
        $autoRefreshFrequency = 5;
        $autoRefreshFrequencyID = "autoRefreshFrequency";
        $actionID = 'action';
        $action = REFRESH_ACTION;
        $enabledStr = "";
        $confirmationMsg = "";
        $errorMsg = "";

        // Get inputs
        if ( array_key_exists( $actionID, $_POST ) )
            $action = $_POST[ $actionID ];
        $autoRefresh = $action == START_AUTO_ACTION;
        $enabledStr = $autoRefresh ? " disabled " : "";

        if ( array_key_exists( $autoRefreshFrequencyID, $_POST ) )
            $autoRefreshFrequency = $_POST[ $autoRefreshFrequencyID ];

        // Create a ServerAdmin object and get additional data from server
        $serverAdmin = new MgServerAdmin();
        $serverAdmin->Open( $selectedServer, $userInfo );
        $selectedServerName = GetServerName( $selectedServer );
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
    if ( FirstTimeHere( $pageName ) )
        RegisterPopup( $pageName, false, NULL );
?>

<body class="ViewCurrentLogTop" onLoad="window.open( 'viewserverstatusbottom.php', 'ViewServerStatusBottom' )" >

    <form name="<?php echo $formName?>" action="<?php echo $_SERVER['PHP_SELF']?>" method="post" onSubmit="PersistAllInputFields();">
    <?php DeclareHiddenVars( $pageName ); ?>

    <table border="0" cellspacing="0" class="inputForm">
        <tr>
            <td class="serverStatusForm" colspan="2">
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

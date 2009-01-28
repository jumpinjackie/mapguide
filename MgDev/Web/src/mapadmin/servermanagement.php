<?php

//
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
//

try
{
    include 'resizableadmin.php';

    LoadSessionVars();

    // Did the user logout?
    CheckForLogout();

    // Are we cancelling?  If so, there is nothing to do.
    CheckForCancel( 'servermanagement.php' );

    // Define Local values
    $menuCategory = SERVER_MANAGEMENT_MENU_ITEM;
    $pageTitle = SERVER_MANAGEMENT_MENU_ITEM;
    $helpPage = 'HelpDocs/managing_servers.htm';
    $pageName = 'ServerManagementPage';
    $formName = 'ServerManagementForm';
    $homePage = NULL;
    $siteServerAddress = "";
    $serverAdmin = null;
    $selectedServerID = 'Server';
    $drawingServiceOnID = 'drawingServiceOn';
    $drawingServiceOnList = array();
    $featureServiceOnID = 'featureServiceOn';
    $featureServiceOnList = array();
    $mappingServiceOnID = 'mappingServiceOn';
    $mappingServiceOn = array();
    $renderingServiceOnID = 'renderingServiceOn';
    $renderingServiceOn = array();
    $tileServiceOnID = 'tileServiceOn';
    $tileServiceOn = array();
    $serverUpdateID = 'serversToUpdate';
    $serverUpdateList = array();
    $okToDeleteID = 'okToDelete';
    $okToDeleteVal = 'true';
    $serverToMonitorID = 'serverToMonitor';
    $onlineID = 'online';
    $onlineList = array();
    $serverTable = array();
    $pageSize = 8;
    $firstPageIndex = -1;
    $lastPageIndex = -1;
    $servers = array();
    $numServers = 0;
    $currPage = 1;

    $siteServerAddress = $site->GetCurrentSiteAddress();

    if ( array_key_exists( $selectedServerID, $_POST ) )
    {
        $selectedServer = $_POST[ $selectedServerID ];
    }

    // Are we switching to another page?
    CheckForPageSwitch();

    // Are we deleting the selected server?
    if ( CheckForDeleteSelection() )
    {
        $selectedServerName = GetServerName( $selectedServer );

        if ( DeleteServer( $selectedServer ) )
        {
            $confirmationMsg = sprintf( $confSuccessfulDeletion, $selectedServerName );
        }
    }

    // Get submitted data
    if ( !FirstTimeHere( $pageName ) )
    {
        $serverUpdateList = array_key_exists( $serverUpdateID, $_POST ) ? $_POST[ $serverUpdateID ] : array();
        $drawingServiceOnList = array_key_exists( $drawingServiceOnID, $_POST ) ? $_POST[ $drawingServiceOnID ] : array();
        $featureServiceOnList = array_key_exists( $featureServiceOnID, $_POST ) ? $_POST[ $featureServiceOnID ] : array();
        $mappingServiceOnList = array_key_exists( $mappingServiceOnID, $_POST ) ? $_POST[ $mappingServiceOnID ] : array();
        $renderingServiceOnList = array_key_exists( $renderingServiceOnID, $_POST ) ? $_POST[ $renderingServiceOnID ] : array();
        $tileServiceOnList = array_key_exists( $tileServiceOnID, $_POST ) ? $_POST[ $tileServiceOnID ] : array();
        $onlineList = array_key_exists( $onlineID, $_POST ) ? $_POST[ $onlineID ] : array();
    }

}
catch ( MgException $e )
{
    CheckForFatalMgException( $e );
    $errorMsg = $e->GetMessage();
}
catch ( Exception $e )
{
    $errorMsg = $e->getMessage();
}

// Load the display table and save data, if requested.
try
{
    $servers = GetServers();
    $numServers = GetServerCount();
    $currPage = GetPageNumber( $servers, $selectedServer, $pageSize );
    GetPageRange( $currPage, $numServers, $pageSize, $firstPageIndex, $lastPageIndex );
    LoadServerTable( $serverTable, $firstPageIndex, $lastPageIndex, true );
    if ( !array_key_exists( $selectedServer, $serverTable ) && $numServers > 0 )
    $selectedServer = $servers[ $firstPageIndex ];

    // Get input values
    if ( !FirstTimeHere( $pageName ) )
    {
        $serverNum = -1;
        foreach ( $serverTable as $key => $val )
        {
            if ( in_array( $key, $serverUpdateList ) )
            {
                $val->services[ DRAWING_SERVICE   ] = in_array( $key, $drawingServiceOnList ) ? true : false;
                $val->services[ FEATURE_SERVICE   ] = in_array( $key, $featureServiceOnList ) ? true : false;
                $val->services[ MAPPING_SERVICE   ] = in_array( $key, $mappingServiceOnList ) ? true : false;
                $val->services[ RENDERING_SERVICE ] = in_array( $key, $renderingServiceOnList ) ? true : false;
                $val->services[ TILE_SERVICE      ] = in_array( $key, $tileServiceOnList ) ? true : false;
                $val->online = in_array( $key, $onlineList ) ? true : false;
            }
        }
    }

    if ( CheckForSaveData() )
    {
        foreach ( $serverTable as $key => $val )
        {
            if ( $val->poweredUp )
            {
                $serverAdmin = new MgServerAdmin();
                $serverAdmin->Open( $key, $userInfo );
                SetOnline( $serverAdmin, $val->online );
                SetOptionalServerServices( $serverAdmin, $val->services );
                $serverAdmin->Close();
            }
        }

        $confirmationMsg = $confSuccessfulServersUpdate;
    }
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

<!-- PAGE DEFINITION -->
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>

    <?php OutputHeader( $pageTitle ); ?>

<body>
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
                DisplaySaveCancelBar();
                DisplayConfirmationMsg( $confirmationMsg );
                DisplayErrorMsg( $errorMsg );

                // Toolbar
                $buttons = array();
                $button = new ToolbarButtonRecord();
                $button->label = "Status";
                $button->icon = "images/monitor_status.gif";
                $button->submitForm = false;
                $button->action = "DisplayServerStatus( '".$serverToMonitorID."' );";
                $buttons[0] = $button;
                $button = new ToolbarButtonRecord();
                $button->id = 'ConfigureButton';
                $button->icon = "images/configure.gif";
                $button->label = "Configure";
                $button->action = "SetElementValue('".NEXT_PAGE_ID."', 'serverproperties.php');";
                $buttons[1] = $button;
                $button = new ToolbarButtonRecord();
                $button->label = "Logs";
                $button->icon = "images/logs.gif";
                        $button->action = "SetElementValue('".NEXT_PAGE_ID."', 'logmanagement.php');";
                $buttons[2] = $button;
                DisplayToolbar( $buttons, $formName );
                ?>

                <!-- Server Table-->
                <table border="0" cellspacing="2" cellpadding="2" class="dataTable">
                    <tr>
                        <td class="dataHeader">&nbsp;</td>
                        <td class="dataHeader">Server</td>
                        <td class="dataHeader">IP Address</td>
                        <td class="dataHeader">Online</td>
                    </tr>
                    <?php
                    if ( $selectedServer == $siteServerAddress )
                    $okToDeleteVal = 'false';
                    else
                    $okToDeleteVal = 'true';

                    $serverNum = -1;
                    $serviceSelector = new ServiceSelectorRecord();
                    foreach ( $serverTable as $key => $val )
                    {
                        if ( $selectedServer == $key )
                        $checkedStr = " checked ";
                        else
                        $checkedStr = "";

                        $serverHrefStr = 'serverproperties.php?'.$selectedServerID.'='.$key;

                        if ( $key == $siteServerAddress )
                        $serverSelectionOnClickStr = "SetElementValue( '".$okToDeleteID."', 'false' ); SetElementValue( '".$serverToMonitorID."', '".$key."' );";
                        else
                        $serverSelectionOnClickStr = "SetElementValue( '".$okToDeleteID."', 'true' );  SetElementValue( '".$serverToMonitorID."', '".$key."' );";

                        $serviceSelector->serverAddress = $key;
                        $serviceSelector->drawingOnID = $drawingServiceOnID.'[]';
                        $serviceSelector->drawingOn = $val->services[ DRAWING_SERVICE ];
                        $serviceSelector->featureOnID = $featureServiceOnID.'[]';
                        $serviceSelector->featureOn = $val->services[ FEATURE_SERVICE ];
                        $serviceSelector->mappingOnID = $mappingServiceOnID.'[]';
                        $serviceSelector->mappingOn = $val->services[ MAPPING_SERVICE ];
                        $serviceSelector->renderingOnID = $renderingServiceOnID.'[]';
                        $serviceSelector->renderingOn = $val->services[ RENDERING_SERVICE ];
                        $serviceSelector->tileOnID = $tileServiceOnID.'[]';
                        $serviceSelector->tileOn = $val->services[ TILE_SERVICE ];
                        $serviceSelector->enabled = $val->poweredUp;
                    ?>
                            <tr>
                                <td class="dataCell"><input name="<?php echo $selectedServerID ?>" type="radio" value="<?php echo $key ?>" <?php echo $checkedStr ?> onClick="<?php echo $serverSelectionOnClickStr ?>" ></td>
                                <td class="dataCell">
                                    <input type="hidden" name="<?php echo $serverUpdateID.'[]'?>" value="<?php echo $key?>" >
                                    <a href="<?php echo $serverHrefStr?>"><?php echo $val->name; ?></a>
                                </td>
                                <td class="dataCell"><?php echo $key ?></td>
                                <td class="dataCell">
                                    <input name="<?php echo $onlineID.'[]'?>" type="checkbox" value="<?php echo $key ?>" <?php if ( $val->online ) echo 'checked'; if ( !$val->poweredUp ) echo ' disabled '; ?> >
                                </td>
                            </tr>
                    <?php
                    }
                    ?>
                </table>

                <!-- Hidden flags -->
                <input type="hidden" name="<?php echo $okToDeleteID; ?>" value="<?php echo $okToDeleteVal; ?>" >
                <input type="hidden" name="<?php echo $serverToMonitorID ?>" value="<?php echo $selectedServer ?>" >

                <?php
                DisplayPaginationControl( $currPage, $numServers, $pageSize, 'servermanagement.php' );
                ?>

                <?php
                DisplaySaveCancelBar();
                EndContentArea( true, $formName, "" );
                ?>
            <!-- End of Contents Area -->

        </tr>

    </table>

</body>
</html>

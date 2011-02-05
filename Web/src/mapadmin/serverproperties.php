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
    CheckForCancel( 'servermanagement.php' );

    // Define the property sections
    define( 'GENERAL_PROPS', "GENERAL" );
    define( 'ADMIN_CONN_PROPS', "ADMIN_CONN" );
    define( 'CLIENT_CONN_PROPS', "CLIENT_CONN" );
    define( 'SITE_CONN_PROPS', "SITE_CONN" );

    // Define local vars
    $menuCategory = SERVER_MANAGEMENT_MENU_ITEM;
    $menuItem = CONFIGURE_SERVER_MENU_ITEM;
    $pageTitle = CONFIGURE_SERVER_TITLE;
    $helpPage = 'HelpDocs/configuring_servers.htm';
    $pageName = 'ServerPropertiesPage';
    $formName = 'ServerPropertiesForm';
    $homePage = 'servermanagement.php';
    $serverAdmin = 0;
    $serverName = "";
    $serverAddress = "";
    $serverVersion = "";
    $serverDescription = "";
    $serverDescriptionID = 'serverDescription';
    $serverOnline = false;
    $serverOnlineID = 'serverOnline';
    $serverServices = array();
    $serverServices[ DRAWING_SERVICE ] = false;
    $serverServices[ FEATURE_SERVICE ] = false;
    $serverServices[ MAPPING_SERVICE ] = false;
    $serverServices[ RENDERING_SERVICE ] = false;
    $serverServices[ TILE_SERVICE ] = false;
    $serverProps = array();
    $serverProps[ GENERAL_PROPS ] = new GeneralPropsRecord();
    $serverProps[ ADMIN_CONN_PROPS ] = new ConnectionPropsRecord( 'admin', MgConfigProperties::AdministrativeConnectionPropertiesSection, MgConfigProperties::AdministrativeConnectionPropertyMaxConnections, MgConfigProperties::AdministrativeConnectionPropertyPort, MgConfigProperties::AdministrativeConnectionPropertyQueueSize, MgConfigProperties::AdministrativeConnectionPropertyThreadPoolSize );
    $serverProps[ CLIENT_CONN_PROPS ] = new ConnectionPropsRecord( 'client', MgConfigProperties::ClientConnectionPropertiesSection, MgConfigProperties::ClientConnectionPropertyMaxConnections, MgConfigProperties::ClientConnectionPropertyPort, MgConfigProperties::ClientConnectionPropertyQueueSize, MgConfigProperties::ClientConnectionPropertyThreadPoolSize );
    $serverProps[ SITE_CONN_PROPS ] = new ConnectionPropsRecord( 'site', MgConfigProperties::SiteConnectionPropertiesSection, MgConfigProperties::SiteConnectionPropertyMaxConnections, MgConfigProperties::SiteConnectionPropertyPort, MgConfigProperties::SiteConnectionPropertyQueueSize, MgConfigProperties::SiteConnectionPropertyThreadPoolSize );
    $drawingServiceOnID = 'drawingServiceOn';
    $featureServiceOnID = 'featureServiceOn';
    $mappingServiceOnID = 'mappingServiceOn';
    $renderingServiceOnID = 'renderingServiceOn';
    $tileServiceOnID = 'tileServiceOn';
    $selectedServerID = "Server";
    $confirmationMsg = "";
    $errorMsg = "";
    $pageEnabled = false;
    $configuringSupportServer = false;
    $refreshRequested = false;


    // Get Server
    $siteServerAddress = $site->GetCurrentSiteAddress();
    GetServerSelection( $selectedServerID, $selectedServer );
    $serverRec = GetDataForServer( $selectedServer );
    if ( $serverRec == NULL )
    throw new Exception( sprintf( $errNotFound, $selectedServer ) );
    if ( !$serverRec->poweredUp )
    throw new Exception( sprintf( $errServerIsDown, $selectedServer ) );

    // Are we configuring the site or a support server?
    $pageEnabled = true;
    $configuringSupportServer = $siteServerAddress != $selectedServer;

    // Is a refresh needed?
    if ( FirstTimeHere( $pageName ) )
        $refreshRequested = true;
    else
        $refreshRequested = CheckForRefresh();

    // Get static data from serverRec
    $serverName = $serverRec->name;
    $serverAddress = $selectedServer;
    if ( $configuringSupportServer )
    $serverDescription = $serverRec->description;
    $serverVersion = $serverRec->version;

    if ( $refreshRequested )
    {
        $serverOnline = $serverRec->online;
        $serverServices = $serverRec->services;

        $serverAdmin = new MgServerAdmin();
        $serverAdmin->Open( $serverAddress, $userInfo );
        foreach ( $serverProps as $props )
        $props->GetProps( $serverAdmin );
        $serverAdmin->Close();
    }
    else
    {
        // Get submitted data
        if ( $configuringSupportServer && array_key_exists( $serverDescriptionID, $_POST ) )
        $serverDescription = $_POST[ $serverDescriptionID ];
        if ( array_key_exists( $serverOnlineID, $_POST ) )
        $serverOnline = ( $_POST[ $serverOnlineID ] == 'Online' );
        $serverServices = array();
        if ( array_key_exists( $drawingServiceOnID, $_POST ) )
        $serverServices[ DRAWING_SERVICE ] = true;
        else
        $serverServices[ DRAWING_SERVICE ] = false;
        if ( array_key_exists( $featureServiceOnID, $_POST ) )
        $serverServices[ FEATURE_SERVICE ] = true;
        else
        $serverServices[ FEATURE_SERVICE ] = false;
        if ( array_key_exists( $mappingServiceOnID, $_POST ) )
        $serverServices[ MAPPING_SERVICE ] = true;
        else
        $serverServices[ MAPPING_SERVICE ] = false;
        if ( array_key_exists( $renderingServiceOnID, $_POST ) )
        $serverServices[ RENDERING_SERVICE ] = true;
        else
        $serverServices[ RENDERING_SERVICE ] = false;
        if ( array_key_exists( $tileServiceOnID, $_POST ) )
        $serverServices[ TILE_SERVICE ] = true;
        else
        $serverServices[ TILE_SERVICE ] = false;
        GetServerGeneralPropVals( $_POST, $serverProps[ GENERAL_PROPS ] );
        GetServerConnectionPropVals( $_POST, $serverProps[ ADMIN_CONN_PROPS ], 'admin' );
        GetServerConnectionPropVals( $_POST, $serverProps[ CLIENT_CONN_PROPS ], 'client' );
        GetServerConnectionPropVals( $_POST, $serverProps[ SITE_CONN_PROPS ], 'site' );

        // Validate inputs.
        foreach ( $serverProps as $props )
        $props->ValidateProps();

        // Update description for support server
        if ( $configuringSupportServer )
        $site->UpdateServer( $serverName, "", $serverDescription, $serverAddress );

        // Set new properties
        $serverAdmin = new MgServerAdmin();
        $serverAdmin->Open( $serverAddress, $userInfo );
        SetOnline( $serverAdmin, $serverOnline );
        SetOptionalServerServices( $serverAdmin, $serverServices );
        foreach ( $serverProps as $props )
        $props->SetProps( $serverAdmin );
        $serverAdmin->Close();

        $confirmationMsg = sprintf( $confSuccessfulUpdate, $serverName );
        SaveSessionVars();
        header( 'Location:  servermanagement.php?'.strip_tags(SID) );
        exit();
    }
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
    <table cellspacing="0" cellpadding="0" border="0" class="main">
        <tr>
            <?php DisplaySiteHeader( false, true, true, true, $formName, $homePage ); ?>
        </tr>
        <tr>
            <?php DisplayLefthandSash( $menuItem ); ?>

            <!-- Contents Area -->
            <?php BeginContentArea( $formName ); ?>

                <?php
                DeclareHiddenVars( $pageName );
                DisplayTitleBar( $pageTitle, $helpPage );
                DisplaySaveCancelBar();
                DisplayConfirmationMsg( $confirmationMsg );
                DisplayErrorMsg( $errorMsg );
                ?>

                <!-- Input Area -->
                <?php
                if ( $pageEnabled )
                $enabledStr = "";
                else
                $enabledStr = " disabled ";
                ?>
                <br>
                <!--TODO: What class is that table(KNN) -->
                <table class="serviceSelector">
                    <tr>
                        <td class="serviceSelectorLabel" >IP address:</td>
                        <td class="serviceSelectorValue"><?php echo $serverAddress;?><br></td>
                    </tr>
                    <tr>
                        <td class="serviceSelectorLabel" >Version:</td>
                        <td class="serviceSelectorValue"><?php echo $serverVersion;?><br></td>
                    </tr>
                    <tr>
                        <td colspan="2">&nbsp;</td>
                    </tr>
                    <tr>
                        <td class="connectionStatusLabel">Connection status:</td>
                        <td class="connectionsStatusValue" >
                            <?php
                            $onlineSelector = new OnlineSelectorRecord();
                            $onlineSelector->onlineID = $serverOnlineID;
                            $onlineSelector->onlineVal = $serverOnline;
                            $onlineSelector->enabled = $pageEnabled;
                            DisplayOnlineSelector( $onlineSelector );
                            ?>
                        </td>
                    </tr>
                </table>

                <?php
                DisplayServerGeneralPropVals( $pageEnabled, $serverProps[ GENERAL_PROPS ] );
                DisplayServerConnectionPropVals( $pageEnabled, $serverProps[ ADMIN_CONN_PROPS ], 'admin', 'Administrative Connection Properties' );
                DisplayServerConnectionPropVals( $pageEnabled, $serverProps[ CLIENT_CONN_PROPS ], 'client', 'Client Connection Properties' );
                DisplayServerConnectionPropVals( $pageEnabled, $serverProps[ SITE_CONN_PROPS ], 'site', 'Site Connection Properties' );
                ?>
                <br>

                <?php
                DisplaySaveCancelBar();
                ?>
                <?php
                if ( $serverRec->poweredUp )
                    EndContentArea( true, $formName, DISPLAY_NAME_ID);
                else
                    EndContentArea( true, $formName, "");
                ?>

            </td>
            <!-- End of Contents Area -->

        </tr>

    </table>

</body>
</html>

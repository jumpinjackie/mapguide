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

    // Define local vars
    $menuCategory = SERVER_MANAGEMENT_MENU_ITEM;
    $menuItem = CONFIGURE_SERVICES_MENU_ITEM;
    $pageTitle = CONFIGURE_SERVICES_TITLE;
    $helpPage = 'HelpDocs/configuring_services.htm';
    $pageName = 'ServicesPropertiesPage';
    $formName = 'ServicesPropertiesForm';
    $homePage = 'servermanagement.php';
    $siteServerAddress = "";
    $selectedServerID = "Server";
    $serverAdmin = 0;
    $featureServiceProps = new FeatureServicePropsRecord();
    $resourceServiceProps = new ResourceServicePropsRecord();
    $siteServiceProps = new SiteServicePropsRecord();
    $tileServiceProps = new TileServicePropsRecord();
    $refreshRequested = false;
    $confirmationMsg = "";
    $errorMsg = "";
    $pageEnabled = false;
    $bSiteServer = false;

    // Get Server
    $siteServerAddress = $site->GetCurrentSiteAddress();
    GetServerSelection( $selectedServerID, $selectedServer );
    $serverRec = GetDataForServer( $selectedServer );
    if ( $serverRec == NULL )
        throw new Exception( sprintf( $errNotFound, $selectedServer ) );
    if ( !$serverRec->poweredUp )
        throw new Exception( sprintf( $errServerIsDown, $selectedServer ) );
    $bSiteServer = $siteServerAddress == $selectedServer;

    $serverRec = GetDataForServer( $selectedServer );
    $pageEnabled = $serverRec->poweredUp;

    // Is this our first time here?
    if ( FirstTimeHere( $pageName ) )
        $refreshRequested = true;
    else
        $refreshRequested = CheckForRefresh();

    // Get services for this server
    if ( $refreshRequested )
    {
        // Get props values from server
        $serverAdmin = new MgServerAdmin();
        $serverAdmin->Open( $selectedServer, $userInfo );
        $featureServiceProps->GetProps( $serverAdmin );
        $tileServiceProps->GetProps( $serverAdmin );
        if ( $bSiteServer )
        {
            $resourceServiceProps->GetProps( $serverAdmin );
            $siteServiceProps->GetProps( $serverAdmin );
        }
        $optionalServices = GetOptionalServerServices( $serverAdmin );
        $featureServiceProps->enabled = $optionalServices[ FEATURE_SERVICE ];
        $tileServiceProps->enabled = $optionalServices[ TILE_SERVICE ];

        $serverAdmin->Close();
    }

    // Get current data.
    else
    {
        // Get data from page.
        GetFeatureServicePropVals( $_POST, $featureServiceProps );
        GetTileServicePropVals( $_POST, $tileServiceProps );
        if ( $bSiteServer )
        {
            GetResourceServicePropVals( $_POST, $resourceServiceProps );
            GetSiteServicePropVals( $_POST, $siteServiceProps );
        }
    }

    // Are we saving?
    if ( CheckForSaveData() )
    {
        // Validate inputs.
        $featureServiceProps->ValidateProps();
        $tileServiceProps->ValidateProps();
        if ( $bSiteServer )
        {
            $resourceServiceProps->ValidateProps();
            $siteServiceProps->ValidateProps();
        }

        // Set new property values
        $serverAdmin = new MgServerAdmin();
        $serverAdmin->Open( $selectedServer, $userInfo );
        $featureServiceProps->SetProps( $serverAdmin );
        $tileServiceProps->SetProps( $serverAdmin );
        if ( $bSiteServer )
        {
            $resourceServiceProps->SetProps( $serverAdmin );
            $siteServiceProps->SetProps( $serverAdmin );
        }
        $optionalServices = GetOptionalServerServices( $serverAdmin );
        $optionalServices[ FEATURE_SERVICE ] = $featureServiceProps->enabled;
        $optionalServices[ TILE_SERVICE ] = $tileServiceProps->enabled;
        SetOptionalServerServices( $serverAdmin, $optionalServices );
        $serverAdmin->Close();

        $confirmationMsg = sprintf( $confSuccessfulUpdate, $selectedServer );
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
                <br>

                <?php
                if ( $bSiteServer )
                {
                    DisplaySiteServicePropVals( $pageEnabled, $siteServiceProps );
                    DisplayResourceServicePropVals( $pageEnabled, $resourceServiceProps );
                }
                DisplayFeatureServicePropVals( $pageEnabled, $featureServiceProps );
                DisplayTileServicePropVals( $pageEnabled, $tileServiceProps );
                ?>

                <br>

                <?php DisplaySaveCancelBar(); ?>

                <?php
                $initialFocus = "";
                if ( $serverRec->poweredUp )
                {
                    if ( $bSiteServer )
                        $initialFocus = SITE_SERVICE_SESSION_TIMEOUT_ID;
                    else
                        $initialFocus = FEATURE_SERVICE_CONN_POOL_ENABLED_ID;
                }
                EndContentArea( true, $formName, $initialFocus );
                ?>
            <!-- End of Contents Area -->

        </tr>
    </table>

</body>
</html>

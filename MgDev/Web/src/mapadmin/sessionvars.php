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

    ini_set ('session.use_trans_sid', 1);
    session_start();

    // Session variables.
    $webConfigFile = "";
    $adminSession = "";
    $selectedUser = "";
    $selectedGroup = "";
    $selectedRole = "";
    $selectedServer = "";
    $selectedService = "";
    $selectedLog = ACCESS_LOG;
    $selectedPackage = "";
    $selectedMapping = "";
    $packageLoadErrorMsg = "";
    $packageMakeErrorMsg = "";
    $packageLoadConfirmationMsg = "";
    $packageMakeConfirmationMsg = "";
    $confirmationMsg = "";
    $userInfo = NULL;
    $clientAgent = "";
    $clientIp = "";
    $site = NULL;
    $popups = array();

    // Session vars which support refreshing of logs or server status.  These values cannot be passed in GET or POST arrays because
    // the refreshing frame may submit old values after the top frame (input frame for these values) sets new values.
    $autoRefresh = false;
    $autoRefreshFrequency = 5;
    $limitRefresh = false;
    $numRecsToRefresh = 10;

    function RefreshSessionVars()
    {
        global $webConfigFile;
        global $adminSession;
        global $selectedUser;
        global $selectedGroup;
        global $selectedRole;
        global $selectedServer;
        global $selectedService;
        global $selectedLog;
        global $selectedPackage;
        global $selectedMapping;
        global $packageLoadErrorMsg;
        global $packageMakeErrorMsg;
        global $packageLoadConfirmationMsg;
        global $packageMakeConfirmationMsg;
        global $confirmationMsg;
        global $userInfo;
        global $clientAgent;
        global $clientIp;
        global $site;
        global $autoRefresh;
        global $autoRefreshFrequency;
        global $limitRefresh;
        global $numRecsToRefresh;
        global $popups;

        // Session variables.
        $webConfigFile = "";
        $adminSession = "";
        $selectedUser = "";
        $selectedGroup = "";
        $selectedRole = "";
        $selectedServer = "";
        $selectedService = "";
        $selectedLog = ACCESS_LOG;
        $selectedPackage = "";
        $selectedMapping = "";
        $packageLoadErrorMsg = "";
        $packageMakeErrorMsg = "";
        $packageLoadConfirmationMsg = "";
        $packageMakeConfirmationMsg = "";
        $confirmationMsg = "";
        $userInfo = NULL;
        $clientAgent = "";
        $clientIp = "";
        $autoRefresh = false;
        $autoRefreshFrequency = 5;
        $limitRefresh = false;
        $numRecsToRefresh = 10;

        // If a site was created, clear it.
        if ( $site != NULL )
        {
            $site->Close();
            $site=NULL;
        }

        // If a popups array exists we need to preserve this so that all popups can be closed.
        if ( array_key_exists( 'popups', $_SESSION ) )
            $popups = $_SESSION[ 'popups' ];
        else
            $popups = array();
    }

    // Load values for the session vars.
    function LoadSessionVars()
    {
        global $webConfigFile;
        global $adminSession;
        global $selectedUser;
        global $selectedGroup;
        global $selectedRole;
        global $selectedServer;
        global $selectedService;
        global $selectedLog;
        global $selectedPackage;
        global $selectedMapping;
        global $packageLoadErrorMsg;
        global $packageMakeErrorMsg;
        global $packageLoadConfirmationMsg;
        global $packageMakeConfirmationMsg;
        global $confirmationMsg;
        global $userInfo;
        global $clientAgent;
        global $clientIp;
        global $site;
        global $autoRefresh;
        global $autoRefreshFrequency;
        global $limitRefresh;
        global $numRecsToRefresh;
        global $popups;

        try
        {
            if ( !array_key_exists( 'webConfigFile', $_SESSION ) )
                return;

            // Load the current vals of session vars.
            $webConfigFile = $_SESSION[ 'webConfigFile' ];
            $adminSession = $_SESSION[ 'adminSession' ];
            $selectedUser = $_SESSION[ 'selectedUser' ];
            $selectedGroup = $_SESSION[ 'selectedGroup' ];
            $selectedRole = $_SESSION[ 'selectedRole' ];
            $selectedServer = $_SESSION[ 'selectedServer' ];
            $selectedService = $_SESSION[ 'selectedService' ];
            $selectedLog = $_SESSION[ 'selectedLog' ];
            $selectedPackage = $_SESSION[ 'selectedPackage' ];
            $selectedMapping = $_SESSION[ 'selectedMapping' ];
            $packageLoadErrorMsg = $_SESSION[ 'packageLoadErrorMsg' ];
            $packageMakeErrorMsg = $_SESSION[ 'packageMakeErrorMsg' ];
            $packageLoadConfirmationMsg = $_SESSION[ 'packageLoadConfirmationMsg' ];
            $packageMakeConfirmationMsg = $_SESSION[ 'packageMakeConfirmationMsg' ];
            $confirmationMsg = $_SESSION[ 'confirmationMsg' ];
            $clientAgent = $_SESSION[ 'clientAgent' ];
            $clientIp = $_SESSION[ 'clientIp' ];
            $autoRefresh = $_SESSION[ 'autoRefresh' ];
            $autoRefreshFrequency = $_SESSION[ 'autoRefreshFrequency' ];
            $limitRefresh = $_SESSION[ 'limitRefresh' ];
            $numRecsToRefresh = $_SESSION[ 'numRecsToRefresh' ];
            $popups = $_SESSION[ 'popups' ];

            if ( empty( $webConfigFile ) )
                return; // There is nothing to initialize

            // Initialize web tier with the site configuration file.
            MgInitializeWebTier( $webConfigFile );

            // Set up user info
            $userInfo = new MgUserInformation();
            $userInfo->SetMgSessionId( $adminSession );
            $userInfo->SetClientAgent( $clientAgent );
            $userInfo->SetClientIp( $clientIp );

            // Create a Site object and open the Site Server.
            $site = new MgSite();
            $site->Open( $userInfo );
        }
        catch( MgException $e )
        {
            LogoutWithFatalException();
        }
    }

    // Save Session var values for next page.
    function SaveSessionVars()
    {
        global $webConfigFile;
        global $adminSession;
        global $selectedUser;
        global $selectedGroup;
        global $selectedRole;
        global $selectedServer;
        global $selectedService;
        global $selectedLog;
        global $selectedPackage;
        global $selectedMapping;
        global $packageLoadErrorMsg;
        global $packageMakeErrorMsg;
        global $packageLoadConfirmationMsg;
        global $packageMakeConfirmationMsg;
        global $confirmationMsg;
        global $userInfo;
        global $clientAgent;
        global $clientIp;
        global $site;
        global $autoRefresh;
        global $autoRefreshFrequency;
        global $limitRefresh;
        global $numRecsToRefresh;
        global $popups;

        // Saves session vars for other pages.
        $_SESSION[ 'webConfigFile' ] = $webConfigFile;
        $_SESSION[ 'adminSession' ] = $adminSession;
        $_SESSION[ 'selectedUser' ] = $selectedUser;
        $_SESSION[ 'selectedGroup' ] = $selectedGroup;
        $_SESSION[ 'selectedRole' ] = $selectedRole;
        $_SESSION[ 'selectedServer' ] = $selectedServer;
        $_SESSION[ 'selectedService' ] = $selectedService;
        $_SESSION[ 'selectedLog' ] = $selectedLog;
        $_SESSION[ 'selectedPackage' ] = $selectedPackage;
        $_SESSION[ 'selectedMapping' ] = $selectedMapping;
        $_SESSION[ 'packageLoadErrorMsg' ] = $packageLoadErrorMsg;
        $_SESSION[ 'packageMakeErrorMsg' ] = $packageMakeErrorMsg;
        $_SESSION[ 'packageLoadConfirmationMsg' ] = $packageLoadConfirmationMsg;
        $_SESSION[ 'packageMakeConfirmationMsg' ] = $packageMakeConfirmationMsg;
        $_SESSION[ 'confirmationMsg' ] = $confirmationMsg;
        $_SESSION[ 'clientAgent' ] = $clientAgent;
        $_SESSION[ 'clientIp' ] = $clientIp;
        $_SESSION[ 'autoRefresh' ] = $autoRefresh;
        $_SESSION[ 'autoRefreshFrequency' ] = $autoRefreshFrequency;
        $_SESSION[ 'limitRefresh' ] = $limitRefresh;
        $_SESSION[ 'numRecsToRefresh' ] = $numRecsToRefresh;
        $_SESSION[ 'popups' ] = $popups;

        session_write_close();
        if ( $site != NULL )
            $site->Close();
    }
?>

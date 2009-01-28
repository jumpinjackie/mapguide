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

        // Define Local Vars
        $pageTitle = LOGIN_TITLE;
        $helpPage = 'HelpDocs/about_sites_and_servers.htm';
        $pageName = 'LoginPage';
        $formName = 'LoginForm';
        $homePage = NULL;
        $adminID = "";
        $adminIDID = 'adminID';
        $serverSelectionID = 'SiteServer';
        $adminPassword = "";
        $adminPasswordID = 'adminPassword';
        $locale = "en"; // localizable string
        $errorMsg = "";
        $numSites = 0;

        if ( FirstTimeHere( $pageName ) )
        {
            // Clear session vars to insure that everything starts fresh.
            RefreshSessionVars();

            // Check for error message
            if ( array_key_exists( ERROR_MSG_ID, $_GET ) )
            {
                $errorMsg = $_GET[ ERROR_MSG_ID ];
            }
        }
        else
        {
            if ( array_key_exists( $adminIDID, $_POST ) )
                $adminID = $_POST[ $adminIDID ];
            if ( array_key_exists( $adminPasswordID, $_POST ) )
                $adminPassword = $_POST[ $adminPasswordID ];

            if ( empty( $adminID ) )
                throw new Exception( $errAdminIDMissing );
            if ( empty( $adminPassword ) )
                throw new Exception( $errPasswordMissing );

            // Initialize web tier with the site configuration file.  The config
            // file should be in the directory above as this script.
            $webConfigFile = __FILE__;
            $pos = strrpos($webConfigFile, '\\');
            if ($pos == false)
                $pos = strrpos($webConfigFile, '/');
            $relativeLocation = '../'.MgConfigProperties::DefaultConfigurationFilename;
            $webConfigFile = substr_replace($webConfigFile, $relativeLocation, $pos+1);
            MgInitializeWebTier($webConfigFile);

            // Set up user info
            $userInfo = new MgUserInformation();
            $userInfo->SetMgUsernamePassword( $adminID, $adminPassword );
            $userInfo->SetLocale( $locale );
            $clientAgent = GetClientAgent();
            $clientIp = GetClientIp();
            $userInfo->SetClientAgent( $clientAgent );
            $userInfo->SetClientIp( $clientIp );

            // Create a Site object and open the Site Server.
            $site = new MgSite();
            $site->Open( $userInfo );

            // Check that we have Administrator permission.
            $servers = $site->EnumerateServers();

            // Determine the available site servers
            $numSites = $site->GetSiteServerCount();

            if($numSites == 1)
            {
                // Create session
                $adminSession = $site->CreateSession();

                // Save session variables
                SaveSessionVars();

                // Success - switch to ServerManagement page.
                header( 'Location:  servermanagement.php?'.strip_tags(SID) );
                exit();
            }
            else
            {
                $siteInfo = GetSiteServerSelection($serverSelectionID);
                if($siteInfo != NULL)
                {
                    // Close the existing site connection
                    if($site != NULL)
                    {
                        $site->Close();
                    }

                    // Determine the selected site
                    $site = new MgSite();

                    // Create a connection to the selected site
                    $site->Open($userInfo, $siteInfo);

                    // Create a session
                    $adminSession = $site->CreateSession();
                    $userInfo->SetMgSessionId($adminSession);

                    // Save the session variables. All requests will now be directed to
                    // our selected server since it hosts the current session.
                    SaveSessionVars();
                    header( 'Location:  servermanagement.php?'.strip_tags(SID) );
                    exit();
                }
            }
        }
    }
    catch ( MgConnectionFailedException $e)
    {
        $errorMsg = $errConnectionFailed;
    }
    catch ( MgException $e )
    {
        $errorMsg = $errAuthenticationFailed;
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

<body onLoad="<?php echo CreateJavaScriptArrayStr( 'popups', $popups );?>ClosePopups( popups );<?php $popups=array(); ?>">

    <table cellspacing="0" cellpadding="0" border="0" class="main">
        <tr>
            <?php DisplaySiteHeader( false, true, false, false, $formName, $homePage ); ?>
        </tr>
        <tr>
            <td class="lhSash"></td>

            <!-- Contents Area -->
            <?php BeginContentArea( $formName ); ?>

                <?php
                    DeclareHiddenVars( $pageName );
                    DisplayTitleBar( $pageTitle, $helpPage );
                    DisplayErrorMsg( $errorMsg );
                ?>

                <!-- Input Area -->
                <table border="0" cellspacing="0" class="inputForm">
                    <?php
                    if($site != NULL && $numSites > 1)
                    {
                        echo '<tr><td class="loginInputLabel">Site Server:</td>', "\n";
                        echo '<td class="loginInputFormValue">';
                        DisplaySiteServerSelector($serverSelectionID);
                        echo '</td></tr>', "\n";
                    }
                    ?>
                    <tr>
                        <td class="loginInputLabel">Administrator ID:</td>
                        <td class="loginInputFormValue"><input class="loginInputFormValue" name="<?php echo $adminIDID?>" type="text" class="TableCell" size="50" value="<?php echo $adminID ?>"></td>
                    </tr>
                    <tr>
                        <td class="loginInputLabel">Password:</td>
                        <td class="loginInputFormValue"><input class="loginInputFormValue" name="<?php echo $adminPasswordID?>" type="password" class="TableCell" size="50" value=""></td>
                    </tr>
                </table>

                <!-- Login Bar -->
                    &nbsp;
                    <br>
                    <table class="saveCancel" border="0" cellspacing="0" cellpadding="4">
                        <tr>
                            <td class="resizableButton"><input class="resizableButton" name="Login" type="submit" value="Login"></td>
                        </tr>
                    </table>

            <?php EndContentArea( true, $formName, $adminIDID ); ?>
            <!-- End of Contents Area -->

        </tr>

    </table>
</body>
</html>

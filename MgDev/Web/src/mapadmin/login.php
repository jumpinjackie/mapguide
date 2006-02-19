<?php  // PHP Preprocessing --------------------------------------------------------------------------------------------------------------

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
        $adminPassword = "";
        $adminPasswordID = 'adminPassword';
        $locale = "eng";
        $errorMsg = "";

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
            $clientAgent = array_key_exists( 'HTTP_USER_AGENT', $_SERVER ) ? $_SERVER['HTTP_USER_AGENT'] : "";
            $userInfo->SetClientAgent( $clientAgent );
            $clientIp = array_key_exists( 'REMOTE_ADDR', $_SERVER ) ?  $_SERVER['REMOTE_ADDR'] : "";
            $userInfo->SetClientIp( $clientIp );

            // Create a Site object and open the Site Server.
            $site = new MgSite();
            $site->Open( $userInfo );

            // Check that we have Administrator permission.
            $servers = $site->EnumerateServers();

            // Create session
            $adminSession = $site->CreateSession();

            // Success - switch to ServerManagement page.
            SaveSessionVars();
            header( 'Location:  servermanagement.php?'.strip_tags(SID) );
            exit();
        }
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

<!-- PAGE DEFINITION ----------------------------------------------------------------------------------------------------->
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>

    <?php OutputHeader( $pageTitle ); ?>

<body onLoad="<?php echo CreateJavaScriptArrayStr( 'popups', $popups );?>ClosePopups( popups );<?php $popups=array(); ?>">

    <table cellspacing="0" cellpadding="0" border="0" class="main">
        <tr>
            <?php DisplaySiteHeader( true, true, false, false, $formName, $homePage ); ?>
        </tr>
        <tr>
            <td class="lhSash"></td>

            <!-- Contents Area ------------------------------------------------------------------------------------------------->
            <?php BeginContentArea( $formName ); ?>

                <?php
                    DeclareHiddenVars( $pageName );
                    DisplayTitleBar( $pageTitle, $helpPage );
                    DisplayErrorMsg( $errorMsg );
                ?>

                <!-- Input Area -->
                <table border="0" cellspacing="0" class="inputForm">
                    <tr>
                        <td class="loginInputLabel">Administrator ID:</td>
                        <td class="loginInputFormValue"><input class="loginInputFormValue" name="<?php echo $adminIDID?>" type="text" class="TableCell" size="50" value="<?php echo $adminID ?>"></td>
                    </tr>
                    <tr>
                        <td class="loginInputLabel">Password:</td>
                        <td class="loginInputFormValue"><input class="loginInputFormValue" name="<?php echo $adminPasswordID?>" type="password" class="TableCell" size="50" value="<?php echo $adminPassword ?>"></td>
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
            <!-- End of Contents Area -------------------------------------------------------------------------------------------------------------------->

        </tr>

    </table>
</body>
</html>

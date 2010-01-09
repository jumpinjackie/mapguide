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

    // Did the user logout?
    CheckForLogout();

    // Local global values
    $menuCategory = SERVER_MANAGEMENT_MENU_ITEM;
    $menuItem = CONFIGURE_LOGS_MENU_ITEM;
    $pageTitle = CONFIGURE_LOGS_TITLE;
    $helpPage = 'HelpDocs/configuring_and_viewing_logs.htm';
    $pageName = 'LogPropertiesPage';
    $formName = 'LogPropertiesForm';
    $homePage = 'servermanagement.php';
    $selectedServerID = 'SelectedServer';
    $selectedLogID = 'selectedLog';
    $logRootFolder = "";
    $logRootFolderID = 'logRootFolder';
    $maxLogSizeID = 'maxLogSize';
    $maxLogSize = 16;
    $maxLogSizeEnabledID = 'maxLogSizeEnabled';
    $maxLogSizeEnabled = false;
    $logDelimiterID = 'logDelimiter'.NO_PERSISTENCE_FLAG;
    $logDelimiter = NO_DELIMITER;
    $archiveFrequencyID = 'archiveFrequency'.NO_PERSISTENCE_FLAG;
    $archiveFrequency = ARCHIVE_DAILY;
    $selectedLogProps = new LogPropsRecord( MgConfigProperties::AccessLogPropertiesSection, MgConfigProperties::AccessLogPropertyEnabled, MgConfigProperties::AccessLogPropertyFilename, MgConfigProperties::AccessLogPropertyParameters, $allAccessLogParms );
    $serverAdmin = 0;
    $refreshRequested = false;
    $confirmationMsg = "";
    $errorMsg = "";
    $pageEnabled = false;


    function AddFrequency( &$filename, $frequency )
    {
        global $archiveFrequencies;

        $pos = strpos( $filename, '.' );
        if ( $pos === false )
        $filename = $filename.$archiveFrequencies[ $frequency ];
        else
        $filename = substr_replace( $filename, $archiveFrequencies[ $frequency ].'.', $pos, 1 );
    }

    function StripAndReturnFrequency( &$filename )
    {
        global $archiveFrequencies;

        $frequency = ARCHIVE_NONE;
        foreach( $archiveFrequencies as $key => $val )
        {
            if ( $key == ARCHIVE_NONE )
                continue;

            $pos = strpos( $filename, $val );
            if ( $pos !== false )
            {
                $frequency = $key;
                $filename = substr_replace( $filename, '', $pos, 2 );
                break;
            }
        }

        return $frequency;
    }

    GetLogSelection( $selectedLogID, $selectedLog );
    if ( $selectedLog == ACTIVE_LOGS )
        $selectedLog = ACCESS_LOG;

    // Get Server
    GetServerSelection( $selectedServerID, $selectedServer );
    $serverRec = GetDataForServer( $selectedServer );
    if ( $serverRec == NULL )
        throw new Exception( sprintf( $errNotFound, $selectedServer ) );
    if ( !$serverRec->poweredUp )
        throw new Exception( sprintf( $errServerIsDown, $selectedServer ) );

    $pageEnabled = true;

    // Is this our first time here?
    if ( FirstTimeHere( $pageName ) )
        $refreshRequested = true;
    else
        $refreshRequested = CheckForRefresh();

    // Are we cancelling?
    CheckForCancel( 'logproperties.php?'.$selectedLogID.'='.$selectedLog );

    // Set up class for selected log properties
    if ( $selectedLog == ACCESS_LOG )
        $selectedLogProps = new LogPropsRecord( MgConfigProperties::AccessLogPropertiesSection, MgConfigProperties::AccessLogPropertyEnabled, MgConfigProperties::AccessLogPropertyFilename, MgConfigProperties::AccessLogPropertyParameters, $allAccessLogParms );
    else
    if ( $selectedLog == ADMIN_LOG )
        $selectedLogProps = new LogPropsRecord( MgConfigProperties::AdminLogPropertiesSection, MgConfigProperties::AdminLogPropertyEnabled, MgConfigProperties::AdminLogPropertyFilename, MgConfigProperties::AdminLogPropertyParameters, $allAdminLogParms );
    else
    if ( $selectedLog == AUTHENTICATION_LOG )
        $selectedLogProps = new LogPropsRecord( MgConfigProperties::AuthenticationLogPropertiesSection, MgConfigProperties::AuthenticationLogPropertyEnabled, MgConfigProperties::AuthenticationLogPropertyFilename, MgConfigProperties::AuthenticationLogPropertyParameters, $allAuthenticationLogParms );
    else
    if ( $selectedLog == ERROR_LOG )
        $selectedLogProps = new LogPropsRecord( MgConfigProperties::ErrorLogPropertiesSection, MgConfigProperties::ErrorLogPropertyEnabled, MgConfigProperties::ErrorLogPropertyFilename, MgConfigProperties::ErrorLogPropertyParameters, $allErrorLogParms );
    else
    if ( $selectedLog == SESSION_LOG )
        $selectedLogProps = new LogPropsRecord( MgConfigProperties::SessionLogPropertiesSection, MgConfigProperties::SessionLogPropertyEnabled, MgConfigProperties::SessionLogPropertyFilename, MgConfigProperties::SessionLogPropertyParameters, $allSessionLogParms );
    else
    if ( $selectedLog == TRACE_LOG )
        $selectedLogProps = new LogPropsRecord( MgConfigProperties::TraceLogPropertiesSection, MgConfigProperties::TraceLogPropertyEnabled, MgConfigProperties::TraceLogPropertyFilename, MgConfigProperties::TraceLogPropertyParameters, $allTraceLogParms );

    // Get log props for this server
    if ( $refreshRequested )
    {
        // Create a ServerAdmin object.
        $serverAdmin = new MgServerAdmin();
        $serverAdmin->Open( $selectedServer, $userInfo );

        $generalProps = $serverAdmin->GetConfigurationProperties( MgConfigProperties::GeneralPropertiesSection );
        $logRootFolderProp = $generalProps->GetItem( MgConfigProperties::GeneralPropertyLogsPath );
        $logRootFolder = $logRootFolderProp->GetValue();
        CleanUpPath( $logRootFolder );
        $maxLogSizeProp = $generalProps->GetItem( MgConfigProperties::GeneralPropertyMaxLogFileSize );
        $maxLogSize = $maxLogSizeProp->GetValue() / 1024.0;
        $maxLogSizeEnabledProp = $generalProps->GetItem( "MaxLogFileSizeEnabled" );
        $maxLogSizeEnabled = $maxLogSizeEnabledProp->GetValue() == "1" ? true : false;
        $theDelimiterProp = $generalProps->GetItem( MgConfigProperties::GeneralPropertyLogsDelimiter );
        $theDelimiter = $theDelimiterProp->GetValue();
        foreach ( $logDelimiters as $key => $val )
        {
            if ( $theDelimiter == $val )
            {
                $logDelimiter = $key;
                break;
            }
        }

        $selectedLogProps->GetProps( $serverAdmin );
        $archiveFrequency = StripAndReturnFrequency( $selectedLogProps->filename );

        $serverAdmin->Close();
    }

    // Get current data.
    else
    {
        if ( array_key_exists( $logRootFolderID, $_POST ) )
        {
            $logRootFolder = $_POST[ $logRootFolderID ];
            CleanUpMagicQuotes( $logRootFolder );
        }
        if ( array_key_exists( $maxLogSizeID, $_POST ) )
            $maxLogSize = $_POST[ $maxLogSizeID ];
        if ( array_key_exists( $maxLogSizeEnabledID, $_POST ) )
            $maxLogSizeEnabled = true;
        else
            $maxLogSizeEnabled = false;
        if ( array_key_exists( $logDelimiterID, $_POST ) )
            $logDelimiter = $_POST[ $logDelimiterID ];
        if ( array_key_exists( $archiveFrequencyID, $_POST ) )
            $archiveFrequency = $_POST[ $archiveFrequencyID ];

        GetLogPropVals( $_POST, $selectedLogProps, $selectedLog );
    }

    // Are we saving?
    if ( CheckForSaveData() )
    {
        // Validate inputs.
        if ( empty( $logRootFolder ) )
            throw new Exception( $errInvalidLogRootFolder );
        if ( $maxLogSize <= 0 )
            throw new Exception( $errInvalidMaxLogSize );
        $selectedLogProps->ValidateProps();

        // Create a ServerAdmin object for the Site Server.
        $serverAdmin = new MgServerAdmin();
        $serverAdmin->Open( $selectedServer, $userInfo );

        // Set general props
        $props = new MgPropertyCollection();
        $prop = new MgStringProperty( MgConfigProperties::GeneralPropertyLogsPath, $logRootFolder );
        $props->Add( $prop );
        $prop = new MgStringProperty( MgConfigProperties::GeneralPropertyMaxLogFileSize, round( $maxLogSize * 1024.0 ) );
        $props->Add( $prop );
        $prop = new MgStringProperty( MgConfigProperties::GeneralPropertyLogsDelimiter, $logDelimiters[ $logDelimiter ] );
        $props->Add( $prop );
        $prop = new MgStringProperty( "MaxLogFileSizeEnabled", $maxLogSizeEnabled ? "1" : "0" );
        $props->Add( $prop );
        $serverAdmin->SetConfigurationProperties( MgConfigProperties::GeneralPropertiesSection , $props );

        // Set properties of selected log
        AddFrequency( $selectedLogProps->filename, $archiveFrequency );
        $selectedLogProps->SetProps( $serverAdmin );
        StripAndReturnFrequency( $selectedLogProps->filename );

        $serverAdmin->Close();

        $confirmationMsg = sprintf( $confSuccessfulUpdate, $logLabels[ $selectedLog ] );
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
                ?>
                <input type="hidden" name="<?php echo $selectedLogID?>" value="<?php echo $selectedLog?>" >

                <!-- Input Area -->
                <br>
                <table border="0" cellspacing="0" class="inputForm">
                    <tr>
                        <td class="logPropsInputLabel">
                            <input name="<?php echo $maxLogSizeEnabledID?>" type="checkbox" <?php if ($maxLogSizeEnabled) echo ' checked'; if (!$pageEnabled) echo ' disabled';?> onClick="var fields=new Array( '<?php echo $maxLogSizeID;?>' ); ChangeEnable( fields );" >
                            Maximum file size:
                        </td>
                        <td  class="logPropsInputValue" width="25%">
                            <input class="logPropsInputValue" name="<?php echo $maxLogSizeID?>" type="text" size="12" value="<?php echo $maxLogSize; ?>"<?php if ( !$pageEnabled || !$maxLogSizeEnabled ) echo ' disabled';?> >&nbsp;MB
                        </td>
                        <td class="LogFilenameExample">Example filename: Name_20051105_1.log</td>
                    </tr>
                    <tr>
                        <td class="logPropsInputLabel">Delimiter:</td>
                        <td colspan="2" class="logPropsInputValue">
                            <select class="logPropsInputValue" name="<?php echo $logDelimiterID?>" size="1" <?php if ( !$pageEnabled ) echo ' disabled';?> >
                                <?php foreach( $logDelimiters as $key => $val ) { ?>
                                    <option  <?php if ( $key == $logDelimiter ) echo 'selected'; ?> ><?php echo $key ?></option>
                                <?php } ?>
                            </select>
                        </td>
                    </tr>
                    <tr>
                        <td class="logPropsInputLabel">Log root folder:</td>
                        <td class="logPropsInputValue" colspan="2"><input class="logPropsInputValue" name="<?php echo $logRootFolderID?>" type="text" value="<?php echo $logRootFolder?>"<?php if ( !$pageEnabled ) echo ' disabled';?> ></td>
                    </tr>
                </table>
                <br><br>

                <!-- Tabs -->
                <table cellspacing="0" cellpadding="2" class="LogTabTable">

                    <tr>
                        <?php DisplayLogSelector( $selectedLogID, $selectedLog, 'logproperties.php' ); ?>
                    </tr>

                    <tr>
                        <td class="LogTabTableBody" colspan="<?php echo (sizeof( $logLabels ) + 1);?>">
                            <?php
                            DisplaySaveCancelBar();
                            DisplayConfirmationMsg( $confirmationMsg );
                            DisplayErrorMsg( $errorMsg );
                            ?>
                            <table border="0" class="logPropsTableBody">
                                <tr>
                                    <td width="100">&nbsp;</td>
                                    <td class="logPropsInputFormValue" colspan="2">
                                        <input class="logPropsInputFormValue"
                                            name="<?php echo $selectedLog.LOG_ENABLED_ID;?>"
                                            type="checkbox"
                                            <?php if ( $selectedLogProps->enabled ) { echo 'checked'; }?>
                                            onClick="var fields=new Array( '<?php echo ADD_BUTTON_ID;?>','<?php echo REMOVE_BUTTON_ID; ?>','<?php echo $selectedLog.LOG_FILENAME_ID;?>','<?php echo $archiveFrequencyID;?>','<?php echo $selectedLog.LOG_PARMS_SELECTED_ID."[]"?>','<?php echo $selectedLog.LOG_PARMS_NOT_SELECTED_ID."[]"?>' ); ChangeEnable( fields );"
                                            <?php if ( !$pageEnabled ) echo ' disabled';?>
                                        >Logging enabled
                                    </td>
                                </tr>
                                <tr>
                                    <td class="inputLabel">Filename:</td>
                                    <td class="logPropsInputFormValue" align="left" colspan="2">
                                        <input class="logPropsInputFormValue"
                                            name="<?php echo $selectedLog.LOG_FILENAME_ID; ?>"
                                            type="text"
                                            <?php if ( !$selectedLogProps->enabled ) {echo 'disabled';} ?>
                                            size="40"
                                            value="<?php echo $selectedLogProps->filename;?>"
                                            <?php if ( !$pageEnabled ) echo ' disabled';?>
                                        >
                                    </td>
                                </tr>
                                <tr>
                                    <td class="inputLabel" align="left">Archive file every:</td>
                                    <td class="logPropsInputFormValue" align="left">
                                        <select class="logPropsInputFormValue" name="<?php echo $archiveFrequencyID?>" size="1" <?php if ( !$selectedLogProps->enabled ) { echo ' disabled'; }?> >
                                            <?php foreach( $archiveFrequencies as $key => $val ) { ?>
                                                <option <?php if ( $key == $archiveFrequency ) echo 'selected'; ?> ><?php echo $key ?></option>
                                            <?php } ?>
                                        </select>
                                    </td>
                                    <td class="LogFilenameExample">Example filename: Name_20051105.log</td>
                                </tr>
                                <tr><td colspan="3">&nbsp;</td></tr>
                                <tr>
                                    <td colspan="3">
                                        <?php
                                        $control = new AddRemoveControlRecord();
                                        $control->col1Name = $selectedLog.LOG_PARMS_NOT_SELECTED_ID.'[]';
                                        $control->col3Name = $selectedLog.LOG_PARMS_SELECTED_ID.'[]';
                                        $control->col1Heading = 'Available parameters';
                                        $control->col3Heading = 'Selected parameters';
                                        $control->col1List = $selectedLogProps->parmsNotSelected;
                                        $control->col3List = $selectedLogProps->parmsSelected;
                                        $control->enabled = $pageEnabled && $selectedLogProps->enabled;
                                        $control->size = 6;
                                        DisplayAddRemoveControl( $control );
                                        ?>
                                    </td>
                                </tr>
                            </table>
                            <?php
                                DisplaySaveCancelBar();
                            ?>
                        </td>
                    </tr>
                </table>

            <?php
            if ( $serverRec->poweredUp )
                EndContentArea( true, $formName, $selectedLog.LOG_ENABLED_ID );
            else
                EndContentArea( true, $formName, "" );
            ?>
            <!-- End of Contents Area -->

        </tr>

    </table>

</body>
</html>

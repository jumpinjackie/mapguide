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

        // Define Local values
        $menuCategory = SERVER_MANAGEMENT_MENU_ITEM;
        $menuItem = LOG_MANAGEMENT_MENU_ITEM;
        $pageTitle = LOG_MANAGEMENT_TITLE;
        $helpPage = 'HelpDocs/configuring_and_viewing_logs.htm';
        $pageName = 'ManageLogsPage';
        $formName = 'ManageLogsForm';
        $homePage = 'servermanagement.php';
        $logFiles = array();
        $logFilesTable = array();
        $numLogFiles = 0;
        $selectedServerID = "Server";
        $viewSelectionID = 'ViewSelection';
        $viewSelection = false;
        $serverAdmin = 0;
        $selectedLogID = 'selectedLog';
        $logToViewID = 'logToView';
        $logFileToDownloadID = 'logFileToDownload';
        $selectedItemID = 'selectedItem';
        $selectedItem = '';
        $initialLogSelection = $selectedLog != ACTIVE_LOGS ? $selectedLog : ACCESS_LOG;
        $errorMsg = "";
        $confirmationMsg = "";

        $pageSize = 25;
        $firstPageIndex = -1;
        $lastPageIndex = -1;
        $currPage = 1;

        // Get the log selection.
        GetLogSelection( $selectedLogID, $selectedLog );

        // Get Server
        GetServerSelection( $selectedServerID, $selectedServer );
        $serverRec = GetDataForServer( $selectedServer );
        if ( $serverRec == NULL )
            throw new Exception( sprintf( $errNotFound, $selectedServer ) );
        if ( !$serverRec->poweredUp )
            throw new Exception( sprintf( $errServerIsDown, $selectedServer ) );

        // Get submitted data
        if ( array_key_exists( $selectedItemID, $_POST ) )
            $selectedItem = $_POST[ $selectedItemID ];
        if ( array_key_exists( $selectedItemID, $_POST ) )
            $selectedItem = $_POST[ $selectedItemID ];
        if ( array_key_exists( $viewSelectionID, $_POST ) )
            $viewSelection = $_POST[ $viewSelectionID ] == 'true';

        // Are we switching to another page?
        $nextPage = GetHiddenVar( NEXT_PAGE_ID );
        if ( $nextPage != "" )
        {
            $selectedLog = $selectedItem;
            SwitchToPage( $nextPage );
        }

        // Create a ServerAdmin object.
        $serverAdmin = new MgServerAdmin();
        $serverAdmin->Open( $selectedServer, $userInfo );

        // Are we deleting the selected file?
        if ( CheckForDeleteSelection() )
        {
            $serverAdmin->DeleteLog( $selectedItem );
            $confirmationMsg = sprintf( $confSuccessfulDeletion, $selectedItem );
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

    // Load display data
    try
    {
        if ( !$serverRec->poweredUp )
            throw new Exception( sprintf( $errServerIsDown, $selectedServer ) );

        if ( $selectedLog != ACTIVE_LOGS )
        {
            $logFiles = GetLogFiles( $serverAdmin, $selectedLog );
            $numLogFiles = count( $logFiles );
            $currPage = GetPageNumber( $logFiles, $selectedItem, $pageSize );
            GetPageRange( $currPage, $numLogFiles, $pageSize, $firstPageIndex, $lastPageIndex );
            LoadLogFileTable( $logFilesTable, $firstPageIndex, $lastPageIndex, $serverAdmin, $selectedLog );
            if ( !array_key_exists( $selectedItem, $logFilesTable ) && $numLogFiles > 0 )
                $selectedItem = $logFiles[$firstPageIndex];
        }
        $serverAdmin->Close();
    }
    catch ( MgException $e )
    {
        CheckForFatalMgException( $e );
        if ( empty( $errorMsg ) )
            $errorMsg = $e->GetExceptionMessage();
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

    <?php
        OutputHeader( $pageTitle );

        if ( $viewSelection && $selectedLog == ACTIVE_LOGS )
            $onLoadStr = "ViewCurrentLogFile('".$logToViewID."','".$selectedItem."' );";
        else
        if ( $viewSelection && $selectedLog != ACTIVE_LOGS )
            $onLoadStr = "DownloadLogFile('".$logFileToDownloadID."','".$selectedItem."' );";
        else
            $onLoadStr = "";
    ?>

<body onLoad="<?php echo $onLoadStr ?>">

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
                    DisplayConfirmationMsg( $confirmationMsg );
                    DisplayErrorMsg( $errorMsg );
                ?>
                <input type="hidden" name="<?php echo $viewSelectionID ?>" value="false">

                <!-- Input Area -->
               <br>

                <!-- Tabs -->
                <table cellspacing="0" cellpadding="2" class="LogTabTable">
                    <tr>
                        <?php DisplayLogSelectorWithActiveTab( $selectedLogID, $selectedLog, 'logmanagement.php' ); ?>
                    </tr>
                    <tr>
                        <td class="LogTabTableBody" colspan="<?php echo (sizeof( $logLabelsWithActiveLabel )+1);?>">
                            <br>
                            <?php
                                if ( $selectedLog != ACTIVE_LOGS )
                                {
                                    DisplayPaginationControl( $currPage, $numLogFiles, $pageSize, 'logmanagement.php' );

                                    // Toolbar
                                    $disabled = $numLogFiles <= 0;
                                    $buttons = array();
                                    $button = new ToolbarButtonRecord();
                                        $button->label = "View";
                                        $button->icon = "images/view_log.gif";
                                        $button->id = 'ViewButton';
                                        $button->action = "SetElementValue('".$viewSelectionID."', 'true');";
                                        $button->disabled = $disabled;
                                    $buttons[0] = $button;
                                    $button = new ToolbarButtonRecord();
                                        $button->label = "Delete";
                                        $button->icon = "images/delete.gif";
                                        $button->id = 'DeleteButton';
                                        $button->action = "if ( window.confirm('Delete the selected Log File?') ) {SetElementValue('".DELETE_SELECTION_ID."', 'true');}";
                                        $button->disabled = $disabled;
                                    $buttons[1] = $button;
                                    DisplayToolbar( $buttons, $formName ); ?>

                                    <!-- Log Files Table-->
                                    <table border="0" cellspacing="2" cellpadding="2" class="dataTable">
                                        <tr>
                                            <td class="dataHeader">&nbsp;</td>
                                            <td class="dataHeader">Log Filename</td>
                                        </tr>
                                        <?php
                                        if ( $numLogFiles == 0 ) { ?>
                                            <tr>
                                                <td class="dataCell">&nbsp;</td>
                                                <td class="dataCell">&nbsp;</td>
                                            </tr>
                                        <?php }
                                        else
                                        foreach ( $logFilesTable as $key => $val )
                                        {
                                            $onClick = "DownloadLogFile('".$logFileToDownloadID."','".$key."' );"; ?>
                                            <tr>
                                                <td width="5%" align="center" class="dataCell"><input name="<?php echo $selectedItemID?>" type="radio" value="<?php echo $key?>" <?php if ( $selectedItem == $key ) echo 'checked' ?> ></td>
                                                <td class="dataCell"><a href="#" onClick="<?php echo $onClick ?>"><?php echo $key ?></a></td>
                                            </tr>
                                        <?php } ?>
                                    </table>
                                    <?php DisplayPaginationControl( $currPage, $numLogFiles, $pageSize, 'logmanagement.php' ); ?>

                            <?php
                                }
                                else
                                {
                                    $disabled = !$serverRec->poweredUp;

                                    // Toolbar
                                    $buttons = array();
                                    $button = new ToolbarButtonRecord();
                                        $button->label = "View";
                                        $button->icon = "images/view_log.gif";
                                        $button->id = 'ViewButton';
                                        $button->action = "SetElementValue('".$viewSelectionID."', 'true');";
                                        $button->disabled = $disabled;
                                    $buttons[0] = $button;
                                    $button = new ToolbarButtonRecord();
                                        $button->label = "Configure";
                                        $button->icon = "images/configure.gif";
                                        $button->id = 'ConfigureButton';
                                        $button->action = "SetElementValue('".NEXT_PAGE_ID."', 'logproperties.php');";
                                        $button->disabled = $disabled;
                                    $buttons[1] = $button;
                                    DisplayToolbar( $buttons, $formName ); ?>

                                    <!-- Log Files Table-->
                                    <table border="0" cellspacing="2" cellpadding="2" class="dataTable">
                                        <tr>
                                            <td class="dataHeader">&nbsp;</td>
                                            <td class="dataHeader">Log Filename</td>
                                        </tr>
                                            <?php
                                            if ( empty( $selectedItem ) )
                                                $selectedItem = $initialLogSelection;
                                            if ( $serverRec->poweredUp )
                                            {
                                                foreach ( $logLabels as $key => $label )
                                                {
                                                    $onClick = "ViewCurrentLogFile('".$logToViewID."','".$key."' );"; ?>
                                                    <tr>
                                                        <td width="5%" align="center" class="dataCell"><input name="<?php echo $selectedItemID?>" type="radio" value="<?php echo $key?>" <?php if ( $selectedItem == $key ) echo 'checked' ?> ></td>
                                                        <td class="dataCell">
                                                            <a href="#" onClick="<?php echo $onClick ?>"><?php echo $label ?></a>
                                                        </td>
                                                    </tr>
                                            <?php
                                                }
                                            }
                                            else
                                            {?>
                                                <tr>
                                                    <td class="dataCell">&nbsp;</td>
                                                    <td class="dataCell">&nbsp;</td>
                                                </tr>
                                            <?php }?>
                                    </table>
                                <?php } ?>
                            <br>
                        </td>
                    </tr>
                </table>

                <?php EndContentArea( true, $formName, "" );  ?>

            <!-- End of Contents Area -->

        </tr>

    </table>

</body>
</html>

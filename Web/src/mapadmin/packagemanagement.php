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
    $menuCategory = PACKAGE_MANAGEMENT_MENU_ITEM;
    $pageTitle = PACKAGE_MANAGEMENT_MENU_ITEM;
    $pageName = 'PackageManagementPage';
    $formName = 'PackageManagementForm';
    $homePage = NULL;

    // Make values
    $makePackageID = 'MakePackage';
    $makePackage = false;
    $folderNameID = 'MakePackageFolderNameID';
    $folderName = "";
    $packageNameID = 'MakePackageNameID';
    $packageName = "";
    $escapedPackageName = "";
    $duplicatePackageName = false;
    $overwriteID = 'OverwritePackage';
    $overwrite = false;
    $dateStr = '';
    $sourceStr = '';
    $suggestedPackageName = "";
    $escapedSuggestedPackageName = "";

    // Load values
    $viewLogID = 'ViewLogID';
    $deletePackage = false;
    $packageTable = array();
    $numPackages = 0;
    $packages = array();
    $selectedPackageID = "selectedPackageID";
    $pageSize = 8;
    $firstPageIndex = -1;
    $lastPageIndex = -1;
    $currPage = 1;

    // Construct suggestedPackageName
    $dateData = getdate();
    $dateStr = sprintf( "%04u%02u%02u", $dateData[ 'year' ], $dateData[ 'mon' ], $dateData[ 'mday' ] );
    $siteServer = $site->GetCurrentSiteAddress();
    $serverAdmin = new MgServerAdmin();
    $serverAdmin->Open( $userInfo );
    $genProps = new GeneralPropsRecord();
    $genProps->GetProps( $serverAdmin );
    $serverAdmin->Close();
    if ( !empty( $genProps->displayName ) )
        $sourceStr = $genProps->displayName;
    else
    if ( strcmp( '127.0.0.1', $siteServer ) != 0 )
        $sourceStr = str_replace( '.', '-', $siteServer );
    else
    if ( array_key_exists( 'COMPUTERNAME', $_SERVER ) )
        $sourceStr = $_SERVER[ 'COMPUTERNAME' ];
    else
        $sourceStr = 'SiteServer';
    $suggestedPackageName = "From_".$sourceStr."_".$dateStr."_<FOLDER_NAME>";
    $escapedSuggestedPackageName = str_replace( "'", "\'", $suggestedPackageName );

    // Get submitted data
    if ( array_key_exists( $selectedPackageID, $_POST ) )
        $selectedPackage = $_POST[ $selectedPackageID ];

    $packages = GetPackageList();
    $numPackages = GetPackageCount();

    // Are we making a new package?
    if ( array_key_exists( $makePackageID, $_POST ) )
        $makePackage = $_POST[ $makePackageID ] == 'true';
    if ( array_key_exists( $overwriteID, $_POST ) )
        $overwrite = $_POST[ $overwriteID ] == 'true';
    if ( $makePackage || $overwrite )
    {
        try
        {
            if ( array_key_exists( $folderNameID, $_POST ) )
                $folderName = $_POST[ $folderNameID ];
            if ( array_key_exists( $packageNameID, $_POST ) )
                $packageName = $_POST[ $packageNameID ];

            if ( empty( $folderName ) )
                throw new Exception( $errNoResourceSpecified );
            if ( empty( $packageName ) )
                throw new Exception( $errNoPackageSpecified );

            $escapedPackageName = str_replace( "'", "\'", $packageName );
            $duplicatePackageName = false;
            if ( !$overwrite )
            {
                //Check for duplicate name
                $packageNameLen = strlen( $packageName );
                if ( $packageNameLen < 4 || substr( $packageName, $packageNameLen - 4, 4 ) != '.mgp' )
                    $fullPackageName = $packageName.'.mgp';
                else
                    $fullPackageName = $packageName;

                foreach ( $packages as $package )
                {
                    if ( strcasecmp( $package, $fullPackageName ) == 0 )
                    {
                        $duplicatePackageName = true;
                        break;
                    }
                }
            }

            if ( !$duplicatePackageName || $overwrite )
            {
                $folderName  = str_replace( ' ', '%20', $folderName );
                $packageName = str_replace( ' ', '%20', $packageName );
                SwitchToPage( 'makepackage.php?'.$folderNameID.'='.$folderName.'&'.$packageNameID.'='.$packageName );
            }
        }
        catch ( Exception $e )
        {
            if ( empty( $makePackageErrorMsg ) )
                $packageMakeErrorMsg = $e->getMessage();
        }
    }

    else
    // We are in the load area of the page
    {
        try
        {
            // Are we switching to another page?
            CheckForPageSwitch();

            // Are we deleting the selected package?
            if ( CheckForDeleteSelection() && DeletePackage( $selectedPackage ) )
               $packageLoadConfirmationMsg = sprintf( $confSuccessfulDeletion, $selectedPackage );
        }
        catch ( MgException $e )
        {
            CheckForFatalMgException( $e );
            if ( empty( $packageLoadErrorMsg ) )
                $packageLoadErrorMsg = $e->GetExceptionMessage();
        }
        catch ( Exception $e )
        {
            if ( empty( $packageLoadErrorMsg ) )
                $packageLoadErrorMsg = $e->getMessage();
        }
    }

    // Load display data
    if ( $numPackages == 0 )
        throw new Exception( $errNoPackagesFound );
    $currPage = GetPageNumber( $packages, $selectedPackage, $pageSize );
    GetPageRange( $currPage, $numPackages, $pageSize, $firstPageIndex, $lastPageIndex );
    LoadPackageTable( $packageTable, $firstPageIndex, $lastPageIndex );
    if ( !array_key_exists( $selectedPackage, $packageTable ) && $numPackages > 0 )
        $selectedPackage = $packages[$firstPageIndex];
}
catch ( MgException $e )
{
    CheckForFatalMgException( $e );
    if ( empty( $packageLoadErrorMsg ) )
        $packageLoadErrorMsg = $e->GetExceptionMessage();
}
catch ( Exception $e )
{
    if ( empty( $packageLoadErrorMsg ) )
        $packageLoadErrorMsg = $e->getMessage();
}
?>

<!-- PAGE DEFINITION -->
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>

    <?php OutputHeader( $pageTitle ); ?>

<?php if ( $duplicatePackageName ) { ?>
    <body onLoad="ConditionalSubmitForm( '<?php echo $formName ?>', 'Package name <?php echo $escapedPackageName?> already exists.  Overwrite?', '<?php echo $overwriteID ?>', 'true' )" >
<?php } else {?>
    <body>
<?php } ?>


    <table cellspacing="0" cellpadding="0" border="0" class="main">
        <tr>
            <?php DisplaySiteHeader( false, true, true, true, $formName, $homePage ); ?>
        </tr>
        <tr>
            <?php DisplayLefthandSash( $menuCategory ); ?>

            <!-- Contents Area -->
            <?php BeginContentArea( $formName ); ?>

                <?php DeclareHiddenVars( $pageName ); ?>
                <input type="hidden" name="<?php echo $viewLogID ?>" value="<?php echo $selectedPackage?>" >
                <input type="hidden" name="<?php echo $makePackageID ?>" value="false" >
                <input type="hidden" name="<?php echo $overwriteID ?>" value="false" >

                <?php
                DisplayTitleBar( 'Make Package', 'HelpDocs/managing_packages.htm' );
                DisplayConfirmationMsg( $packageMakeConfirmationMsg );
                DisplayErrorMsg( $packageMakeErrorMsg );
                ?>

                <div class="textMsg">
                    <br>
                    You can package a section of this site repository and store in the package folder for moving to another
                    site server.  Enter a repository folder path.  Everything below that folder will be packaged.
                    <br>
                    <a href="servicesproperties.php">Configure Packages directory.</a>
                    <br><br>
                </div>

                <table border="0" cellspacing="0" class="inputForm">
                    <tr>
                        <td class="makePackageInputLabel">Folder name (e.g. Library://&lt;root_folder&gt;/&lt;folder1&gt;):</td>
                        <td class="inputFormValue"><input onChange="SuggestPackageName('<?php echo $escapedSuggestedPackageName?>', this.value, '<?php echo $packageNameID?>')" class="inputFormValue" name="<?php echo $folderNameID?>" type="text" value="<?php echo $folderName?>"></td>
                    </tr>
                    <tr>
                        <td class="makePackageInputLabel">Resulting package name:</td>
                        <td class="inputFormValue"><input onChange="if ( this.value.length == 0 ) { SuggestPackageName('<?php echo $escapedSuggestedPackageName?>', document.<?php echo $formName.'.'.$folderNameID?>.value, '<?php echo $packageNameID?>')}" class="inputFormValue" name="<?php echo $packageNameID?>" type="text" value="<?php echo $packageName?>"></td>
                    </tr>
                </table>

                <br>
                <table class="saveCancel" border="0" cellspacing="0" cellpadding="4">
                    <tr>
                        <td class="saveCancel">
                            <input class="saveCancelButtons" name="Make" type="submit"  onClick="if ( window.confirm('Make Package may take a significant amount of time to execute.  Do you wish to continue?') ) {SetElementValue('<?php echo $makePackageID ?>', 'true' ); SubmitForm( '<?php echo $formName ?>' );}" value="Make">
                        </td>
                    </tr>
                </table>
                <br><br>

               <?php
                DisplayTitleBar( 'Load Package', 'HelpDocs/managing_packages.htm' );
                DisplayConfirmationMsg( $packageLoadConfirmationMsg );
                DisplayErrorMsg( $packageLoadErrorMsg );
                ?>

                <div class="textMsg">
                    <br>
                    Packaged resources or data in the package folder can be loaded to this site server.
                    <br>
                    <a href="servicesproperties.php">Configure Packages directory.</a>
                    <br><br>
                </div>

                <?php
                // Toolbar
                $disabled = $numPackages <= 0;
                $buttons = array();
                $button = new ToolbarButtonRecord();
                $button->label = "Load Package";
                $button->icon = "images/load_package.gif";
                $button->id = 'LoadPackageButton';
                $button->action = "if ( window.confirm('Load Package may take a significant amount of time to execute.  Do you wish to continue?') ) {SetElementValue('".NEXT_PAGE_ID."', 'loadpackage.php');}";
                $button->disabled = $disabled;
                $buttons[0] = $button;
                $button = new ToolbarButtonRecord();
                $button->label = "View Log";
                $button->icon = "images/view_log.gif";
                $button->id = 'ViewLogButton';
                $button->action = "DisplayPackageLog('".$viewLogID."');";
                $button->disabled = $disabled;
                $buttons[1] = $button;
                $button = new ToolbarButtonRecord();
                $button->label = "Delete";
                $button->icon = "images/delete.gif";
                $button->id = 'DeleteButton';
                $button->action = "if ( window.confirm('Are you sure you want to delete the selected Package?') ) {SetElementValue('".DELETE_SELECTION_ID."', 'true');}";
                $button->disabled = $disabled;
                $buttons[2] = $button;
                $button = new ToolbarButtonRecord();
                $button->label = "Refresh";
                $button->icon = "images/refresh.gif";
                $button->id = "RefreshButton";
                $button->action = "";
                $buttons[3] = $button;
                DisplayToolbar( $buttons, $formName );
                ?>

                <!-- Server Table-->
                <table border="0" cellspacing="2" cellpadding="2" class="dataTable">
                    <tr>
                        <td class="dataHeader">&nbsp;</td>
                        <td class="dataHeader">Package Name</td>
                        <td class="dataHeader">Size (bytes)</td>
                        <td class="dataHeader">Status</td>
                    </tr>
                    <?php
                    if ( $numPackages == 0 )
                    {
                    ?>
                            <tr>
                                <td class="dataCell">&nbsp;</td>
                                <td class="dataCell">&nbsp;</td>
                                <td class="dataCell">&nbsp;</td>
                                <td class="dataCell">&nbsp;</td>
                            </tr>
                    <?php
                    }
                    else
                    foreach ( $packageTable as $key => $val )
                    {
                        $escapedKey = str_replace( "'", "\'", $key );
                        if ( $selectedPackage == $key )
                        $checkedStr = " checked ";
                        else
                        $checkedStr = "";
                    ?>
                            <tr>
                                <td class="dataCell"><input name="<?php echo $selectedPackageID ?>" onClick="SetElementValue('<?php echo $viewLogID?>', '<?php echo $escapedKey?>');" type="radio" value="<?php echo $key ?>" <?php echo $checkedStr ?> ></td>
                                <td class="dataCell"><?php echo $key ?></td>
                                <td class="dataCell"><?php echo number_format($val->size) ?></td>
                                <td class="dataCell"><?php echo $val->status ?></td>
                            </tr>
                    <?php
                    }
                    ?>
                </table>

                <?php
                DisplayPaginationControl( $currPage, $numPackages, $pageSize, 'packagemanagement.php' );
                ?>

                <?php
                $packageLoadErrorMsg = "";
                $packageMakeErrorMsg = "";
                $packageLoadConfirmationMsg = "";
                $packageMakeConfirmationMsg = "";
                EndContentArea( true, $formName, "" );
                ?>

            <!-- End of Contents Area -->

        </tr>

    </table>

</body>
</html>

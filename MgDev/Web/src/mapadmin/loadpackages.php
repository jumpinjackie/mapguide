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

    LoadSessionVars();

    // Did the user logout?
    CheckForLogout();

    // Define Local values
    $menuCategory = LOAD_PACKAGES_MENU_ITEM;
    $pageTitle = LOAD_PACKAGES_MENU_ITEM;
    $helpPage = 'HelpDocs/loading_packages.htm';
    $pageName = 'LoadPackagesPage';
    $formName = 'LoadPackagesForm';
    $homePage = NULL;
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


    // Get submitted data
    if ( array_key_exists( $selectedPackageID, $_POST ) )
    $selectedPackage = $_POST[ $selectedPackageID ];

    // Are we switching to another page?
    CheckForPageSwitch();

    // Are we deleting the selected package?
    if ( CheckForDeleteSelection() && DeletePackage( $selectedPackage ) )
    $confirmationMsg = sprintf( $confSuccessfulDeletion, $selectedPackage );
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

// Load display data
try
{
    $packages = GetPackageList();
    $numPackages = GetPackageCount();
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
    if ( empty( $errorMsg ) )
    $errorMsg = $e->GetMessage();
}
catch ( Exception $e )
{
    if ( empty( $errorMsg ) )
    $errorMsg = $e->getMessage();
}
?>

<!-- PAGE DEFINITION ----------------------------------------------------------------------------------------------------->
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>

    <?php
    OutputHeader( $pageTitle );
    ?>

<body>

    <table cellspacing="0" cellpadding="0" border="0" class="main">
        <tr>
            <?php DisplaySiteHeader( true, true, true, true, $formName, $homePage ); ?>
        </tr>
        <tr>
            <?php DisplayLefthandSash( $menuCategory ); ?>

            <!-- Contents Area ------------------------------------------------------------------------------------------------->
            <?php BeginContentArea( $formName ); ?>

                <?php
                DeclareHiddenVars( $pageName );
                DisplayTitleBar( $pageTitle, $helpPage );
                DisplayConfirmationMsg( $confirmationMsg );
                DisplayErrorMsg( $errorMsg );
                DisplayErrorMsg( $packageLoadErrorMsg );
                ?>
                <input type="hidden" name="<?php echo $viewLogID ?>" value="<?php echo $selectedPackage?>" >

                <div class="textMsg">
                    <br>
                    In Studio, large-scale resources can be packaged and saved to a network location for loading directly onto the site.
                    Here you specify a package to be unzipped to the site.
                    <br><br><br>
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
                DisplayToolbar( $buttons, $formName );
                ?>

                <!-- Server Table-->
                <table border="0" cellspacing="2" cellpadding="2" class="dataTable">
                    <tr>
                        <td class="dataHeader">&nbsp;</td>
                        <td class="dataHeader">Package Name</td>
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
                            </tr>
                    <?php
                    }
                    else
                    foreach ( $packageTable as $key => $val )
                    {
                        if ( $selectedPackage == $key )
                        $checkedStr = " checked ";
                        else
                        $checkedStr = "";
                    ?>
                            <tr>
                                <td class="dataCell"><input name="<?php echo $selectedPackageID ?>" onClick="SetElementValue('<?php echo $viewLogID?>', '<?php echo $key?>');" type="radio" value="<?php echo $key ?>" <?php echo $checkedStr ?> ></td>
                                <td class="dataCell"><?php echo $key ?></td>
                                <td class="dataCell"><?php echo $val->status ?></td>
                            </tr>
                    <?php
                    }
                    ?>
                </table>

                <?php
                DisplayPaginationControl( $currPage, $numPackages, $pageSize, 'loadpackages.php' );
                ?>

                <?php
                EndContentArea( true, $formName, "" );
                ?>

            <!-- End of Contents Area --------------------------------------------------------------------------------------------->

        </tr>

    </table>

</body>
</html>

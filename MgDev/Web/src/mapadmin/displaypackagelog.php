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

    CheckForPopupRegistration();

    function GetPackageLogContents( $packageName )
    {
        global $site;
        global $userInfo;

        $serverAdmin = new MgServerAdmin();
        $serverAdmin->Open( $userInfo );
        $logReader = $serverAdmin->GetPackageLog( $packageName );
        $serverAdmin->Close();

        $theLog = "";
        $chunk = "";
        do {
            $chunkSize = $logReader->Read( $chunk, 4096 );
            $theLog = $theLog.$chunk;
        } while ( $chunkSize != 0 );

        return $theLog;
    }

    // Define Local values
    $pageTitle = "Package Log";
    $pageName = 'DisplayPackageLogPage';
    $formName = 'DisplayPackageLogForm';
    $selectedPackageID = "selectedPackageID";
    $logContents = "";
    $confirmationMsg = "";
    $errorMsg = "";

    if ( array_key_exists( $selectedPackageID, $_GET ) )
        $selectedPackage = $_GET[ $selectedPackageID ];
    if ( array_key_exists( $selectedPackageID, $_POST ) )
        $selectedPackage = $_POST[ $selectedPackageID ];
    if ( empty( $selectedPackage ) )
        throw new Exception( $errNoPackageLog );

    $firstTimeHere = FirstTimeHere( $pageName );
    $pageTitle = "Package Log for ".$selectedPackage;

    if ( !$firstTimeHere)
        $logContents = GetPackageLogContents( $selectedPackage );
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

<?php
    OutputHeader( $pageTitle );

    if ( $firstTimeHere )
    {
        $valuesToPersist = array();
        $valuesToPersist[ $selectedPackageID ] = $selectedPackage;
        RegisterPopup( $pageName, true, $valuesToPersist );
    }
?>

<body>
    <table cellspacing="0" cellpadding="0" border="0" class="main">
        <tr>
            <?php DisplaySiteHeader( false, true, false, false, $formName, NULL ); ?>
        </tr>
        <tr class="logDisplayTable"> 
            <!-- Contents Area -->
                <?php
                BeginContentArea( null );
                DisplayTitleBar( $pageTitle, NULL );
                DisplayConfirmationMsg( $confirmationMsg );
                DisplayErrorMsg( $errorMsg );
                ?>

                <table class="logDisplayTable" align="center" >
                    <tr>
                        <td class="logDisplayTable"><textarea class="logDisplayTable"><?php echo $logContents ?></textarea></td>
                    </tr>
                </table>

                <?php
                EndContentArea( true, null, "" );
                ?>
            <!-- End of Contents Area -->

        </tr>

    </table>

</body>
</html>

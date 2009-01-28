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

    function MakeResourcePackage( $folderName, $packageName )
    {
        global $site;
        global $userInfo;

        $serverAdmin = new MgServerAdmin();
        $serverAdmin->Open( $userInfo );

        // Make sure folder name begins with "Library://" and ends with "/"
        if ( ( strcasecmp( "Library", $folderName ) == 0 ) || ( strcasecmp( "Library:", $folderName ) == 0 ) )
            $completeFolderName = "Library://";
        else
        if ( strncasecmp( "Library://", $folderName, 10 ) == 0 )
            $completeFolderName = $folderName;
        else
        if ( strncmp( "//", $folderName, 2 ) == 0 )
            $completeFolderName = "Library:".$folderName;
        else
            $completeFolderName = "Library://".$folderName;
        if ( strrpos( $completeFolderName, "/" ) != ( strlen( $completeFolderName ) -1 ) )
            $completeFolderName = $completeFolderName.'/';

        set_time_limit(0);
        $resource = new MgResourceIdentifier( $completeFolderName );
        $serverAdmin->MakePackage( $resource, $packageName, "" );

        $serverAdmin->Close();
    }

    // Define Local values
    $pageName = "MakePackage";
    $formName = "MakePackage";
    $pageTitle = "Make Package";
    $folderNameID = 'MakePackageFolderNameID';
    $folderName = "";
    $packageNameID = 'MakePackageNameID';
    $packageName = "";
    $packageMakeErrorMsg = "";
    $packageMakeConfirmationMsg = "";

    if ( !FirstTimeHere( $pageName ) )
    {
        if ( array_key_exists( $folderNameID, $_POST ) )
            $folderName = $_POST[ $folderNameID ];
        if ( array_key_exists( $packageNameID, $_POST ) )
            $packageName = $_POST[ $packageNameID ];

        if ( empty( $folderName ) )
            throw new Exception( $errNoResourceSpecified );
        if ( empty( $packageName ) )
            throw new Exception( $errNoPackageSpecified );

        MakeResourcePackage( $folderName, $packageName );
        $packageMakeConfirmationMsg = sprintf( $confPackageMakeCompleted, $packageName, $folderName );
        $packageMakeErrorMsg = "";
        SaveSessionVars();
        header( 'Location:  packagemanagement.php?'.strip_tags(SID) );
        exit();
    }
    else
    {
        if ( array_key_exists( $folderNameID, $_GET ) )
            $folderName = $_GET[ $folderNameID ];
        if ( array_key_exists( $packageNameID, $_GET ) )
            $packageName = $_GET[ $packageNameID ];

        if ( empty( $folderName ) )
            throw new Exception( $errNoResourceSpecified );
        if ( empty( $packageName ) )
            throw new Exception( $errNoPackageSpecified );
    }
}
catch ( MgException $e )
{
    CheckForFatalMgException( $e );
    $packageMakeErrorMsg = sprintf( $errCannotCreatePackage, $packageName, $folderName ).$e->GetMessage();
}
catch ( Exception $e )
{
    $packageMakeErrorMsg = sprintf( $errCannotCreatePackage, $packageName, $folderName ).$e->getMessage();
}

// If an error was encountered, return to the packages table and report the error.
if ( !empty( $packageMakeErrorMsg ) )
{
    SaveSessionVars();
    header( 'Location:  packagemanagement.php?'.strip_tags(SID) );
    exit();
}

?>

<!-- PAGE DEFINITION -->
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
    <?php
    OutputHeader( $pageTitle );
    ?>

<body onLoad="document.body.style.cursor='wait'; window.setTimeout('document.<?php echo $formName ?>.submit();', 1);" >
    <table cellspacing="0" cellpadding="0" border="0" class="main">
        <tr>
            <?php DisplaySiteHeader( false, true, false, false, $formName, NULL ); ?>
        </tr>
        <tr>
            <!-- Contents Area -->
                <?php
                BeginContentArea( $formName );
                DeclareHiddenVars( $pageName );
                DisplayTitleBar( $pageTitle, NULL );
                ?>
                <input type="hidden" name="<?php echo $folderNameID?>"  value="<?php echo $folderName ?>" >
                <input type="hidden" name="<?php echo $packageNameID?>" value="<?php echo $packageName ?>" >

                The package is currently building which may take some time.  If you wish to continue with Site Administration work, you may open another browser and login to another instance of the Site Administrator.

                <?php
                EndContentArea( true, $formName, "" );
                ?>
            <!-- End of Contents Area -->

        </tr>
    </table>

</body>
</html>

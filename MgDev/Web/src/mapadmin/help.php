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

        CheckForPopupRegistration();

        // Define Local values
        $pageName = "HelpPage";
        $formName = "HelpForm";
        $pageTitle = HELP_TITLE;
        $helpPage = "";

        $firstTimeHere = FirstTimeHere( $pageName );

        if ( array_key_exists( HELP_PAGE_ID, $_GET ) )
            $helpPage = $_GET[ HELP_PAGE_ID ];
        if ( array_key_exists( HELP_PAGE_ID, $_POST ) )
            $helpPage = $_POST[ HELP_PAGE_ID ];

        if ( !$firstTimeHere )
        {
            if ( empty( $helpPage ) )
                throw new Exception( $errHelpPageNotFound );
            SaveSessionVars();
            header( 'Location:  '.$helpPage.'?'.strip_tags(SID) );
            exit();
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

<?php
    OutputHeader( $pageTitle );

    if ( $firstTimeHere )
    {
        $valuesToPersist = array();
        $valuesToPersist[ HELP_PAGE_ID ] = $helpPage;
        RegisterPopup( $pageName, true, $valuesToPersist );
    }
?>

<body>
<?php
    DisplayErrorMsg( $errorMsg );
?>
</body>

</html>

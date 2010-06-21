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

        function GetLogContents( $logFileName )
        {
            global $selectedServer;
            global $userInfo;

            $serverAdmin = new MgServerAdmin();
            $serverAdmin->Open( $selectedServer, $userInfo );
            $logReader = $serverAdmin->GetLogFile( $logFileName );
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
        $pageTitle = "";
        $formName = 'ViewCurrentLogBottomForm';
        $pageName = 'ViewCurrentLogBottomPage';
        $logToViewID = 'logToView';
        $logToView = '';
        $theLog = "";
        $timestamp = "";
        $confirmationMsg = "";
        $errorMsg = "";

        // Get inputs
        if ( array_key_exists( $logToViewID, $_GET ) )
            $logToView = $_GET[ $logToViewID ];
        else
        if ( array_key_exists( $logToViewID, $_POST ) )
            $logToView = $_POST[ $logToViewID ];
        if ( empty( $logToView ) )
            throw new Exception( $errNoLogSpecified );

        // Create a ServerAdmin object.
        $serverAdmin = new MgServerAdmin();
        $serverAdmin->Open( $selectedServer, $userInfo );

        // Get log
        if ( $limitRefresh )
            $logReader = $serverAdmin->GetLog( $logToView, $numRecsToRefresh );
        else
            $logReader = $serverAdmin->GetLog( $logToView );
        $theLog = "";
        $chunk = "";
        do {
            $chunkSize = $logReader->Read( $chunk, 4096 );
            $theLog = $theLog.$chunk;
        } while ( $chunkSize != 0 );
        $timestamp = date( "D, j M Y, g:i:s a T" );

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

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
    <?php
        OutputHeader( $pageTitle );
    ?>
</head>

<?php
    $onLoadStr = 'onLoad="ScrollToBottomOfPage();';
    if ( $autoRefresh )
    {
        $timeoutFmt = 'window.setTimeout('."'document.".$formName.".submit();', %d);";
        $onLoadStr = $onLoadStr.sprintf( $timeoutFmt, $autoRefreshFrequency * 1000 );
    }
    $onLoadStr = $onLoadStr.'"';
?>

<body class="ViewCurrentLogBottom" <?php echo $onLoadStr; ?> >

    <?php
        BeginContentArea( $formName );
        DeclareHiddenVars( $pageName );
    ?>

        <input type="hidden" name="<?php echo $logToViewID ?>" value="<?php echo $logToView?>" ?>

        <?php
            if ( !empty( $errorMsg ) )
                DisplayErrorMsg( $errorMsg );
            else
            { ?>
                <div class="ViewCurrentLogBottomTimestamp">
                    <?php if ( $autoRefresh ) { ?>
                        This window is in auto refresh mode.<br>
                    <?php } else { ?>
                        This window is in manual refresh mode.<br>
                    <?php } ?>
                        Refreshed: <?php echo $timestamp; ?>
                </div>
                <pre class="logBody"><?php echo $theLog; ?></pre>
        <?php } ?>

    <?php EndContentArea( true, $formName, NULL ); ?>

</body>
</html>

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
        $pageName = "DownloadLogPage";
        $formName = "DownloadLogForm";
        $logFileToDownloadID = 'logFileToDownload';
        $logFileToDownload = "";
        $selectedServerName = "";
        $logContents = "";
        $errorMsg = "";

        $firstTimeHere = FirstTimeHere( $pageName );

        // Get log filename
        if ( array_key_exists( $logFileToDownloadID, $_GET ) )
            $logFileToDownload = $_GET[ $logFileToDownloadID ];
        if ( array_key_exists( $logFileToDownloadID, $_POST ) )
            $logFileToDownload = $_POST[ $logFileToDownloadID ];
        if ( empty( $logFileToDownload ) )
            throw new Exception( $errNoLogFileSpecified );

        // Get the log contents
        $logContents = GetLogContents( $logFileToDownload );
        $selectedServerName = GetServerName( $selectedServer );
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

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<html>
<head>
    <?php
        $pageTitle = $selectedServerName.' - '.$logFileToDownload;
        OutputHeader( $pageTitle );
    ?>
</head>

<?php
    if ( $firstTimeHere )
    {
        $valuesToPersist = array();
        $valuesToPersist[ $logFileToDownloadID ] = $logFileToDownload;
        RegisterPopup( $pageName, true, $valuesToPersist );
    }
?>
<body class="DownloadLogFile">

    <?php
        BeginContentArea( $formName );
        DeclareHiddenVars( $pageName );
    ?>
    <br>
    <pre class="logBody"><?php
        if ( empty( $errorMsg ) )
            echo $logContents;
        else
            echo $errorMsg;
    ?></pre>
    <?php
        EndContentArea( true, $formName, NULL );
    ?>
</body>

</html>

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

        // Time Units
        define( 'MILLISECONDS', "milliseconds" );
        define( 'SECONDS',      "seconds" );
        define( 'MINUTES',      "minutes" );
        define( 'HOURS',        "hours" );
        define( 'DAYS',         "days" );
        define( 'WEEKS',        "weeks" );

        class TimeUnitFormattingData
        {
            var $divisor;
            var $nextUnit;

            function TimeUnitFormattingData( $divisor, $nextUnit )
            {
                $this->divisor = $divisor;
                $this->nextUnit = $nextUnit;
            }
        }

        $timeFormattingData = array();
        $timeFormattingData[ MILLISECONDS ] = new TimeUnitFormattingData( 1000, SECONDS );
        $timeFormattingData[ SECONDS      ] = new TimeUnitFormattingData( 60,   MINUTES );
        $timeFormattingData[ MINUTES      ] = new TimeUnitFormattingData( 60,   HOURS );
        $timeFormattingData[ HOURS        ] = new TimeUnitFormattingData( 24,   DAYS );
        $timeFormattingData[ DAYS         ] = new TimeUnitFormattingData( 7,    WEEKS );

        function FormatTime( $originalTime, $originalUnit )
        {
            global $timeFormattingData;

            $formattedTime = $originalTime;
            $formattedUnit = $originalUnit;
            $timeFormattingStr = "%f %s";

            foreach ( $timeFormattingData as $unit => $data )
            {
                if ( $formattedUnit == $unit && $formattedTime > $data->divisor )
                {
                    $formattedTime /= $data->divisor;
                    $formattedUnit = $data->nextUnit;
                }
            }

            return sprintf( $timeFormattingStr, $formattedTime, $formattedUnit );
        }

        class ServerHealthPageRecord
        {
            var $timeStamp;
            var $cpuUtilization;
            var $upTime;
            var $adminOpsQueueSize;
            var $clientOpsQueueSize;
            var $siteOpsQueueSize;
            var $avgClientOpTime;
            var $totalOpTime;
            var $totalPhysicalMemory;
            var $physicalMemoryUsed;
            var $totalVirtualMemory;
            var $virtualMemoryUsed;

            function ServerHealthPageRecord()
            {
                $this->timeStamp = 0;
                $this->cpuUtilization = 0;
                $this->upTime = 0;
                $this->adminOpsQueueSize = 0;
                $this->clientOpsQueueSize = 0;
                $this->siteOpsQueueSize = 0;
                $this->avgClientOpTime = 0;
                $this->totalOpTime = 0;
                $this->totalPhysicalMemory = 0;
                $this->physicalMemoryUsed = 0;
                $this->totalVirtualMemory = 0;
                $this->virtualMemoryUsed = 0;
            }

            function GetProps( $serverAdmin )
            {
                $this->timeStamp = date( "D, j M Y, g:i:s a T" );

                $props = $serverAdmin->GetInformationProperties();

                $prop = $props->GetItem( MgServerInformationProperties::CpuUtilization );
                $this->cpuUtilization = $prop->GetValue();

                $prop = $props->GetItem( MgServerInformationProperties::Uptime );
                $this->upTime = $prop->GetValue();

                $prop = $props->GetItem( MgServerInformationProperties::AdminOperationsQueueCount );
                $this->adminOpsQueueSize = $prop->GetValue();

                $prop = $props->GetItem( MgServerInformationProperties::ClientOperationsQueueCount );
                $this->clientOpsQueueSize = $prop->GetValue();

                $prop = $props->GetItem( MgServerInformationProperties::SiteOperationsQueueCount );
                $this->siteOpsQueueSize = $prop->GetValue();

                $prop = $props->GetItem( MgServerInformationProperties::AverageOperationTime );
                $this->avgClientOpTime = $prop->GetValue();

                $prop = $props->GetItem( MgServerInformationProperties::TotalOperationTime );
                $this->totalOpTime = $prop->GetValue();

                $prop = $props->GetItem( MgServerInformationProperties::TotalProcessedOperations );
                $this->totalOpsProcessed = $prop->GetValue();

                $prop = $props->GetItem( MgServerInformationProperties::TotalReceivedOperations );
                $this->totalOpsReceived = $prop->GetValue();

                $prop = $props->GetItem( MgServerInformationProperties::TotalPhysicalMemory );
                $this->totalPhysicalMemory = $prop->GetValue();

                $prop = $props->GetItem( MgServerInformationProperties::AvailablePhysicalMemory );
                $physicalMemoryAvailable = $prop->GetValue();
                $this->physicalMemoryUsed = $this->totalPhysicalMemory - $physicalMemoryAvailable;

                $prop = $props->GetItem( MgServerInformationProperties::TotalVirtualMemory );
                $this->totalVirtualMemory = $prop->GetValue();

                $prop = $props->GetItem( MgServerInformationProperties::AvailableVirtualMemory );
                $virtualMemoryAvailable = $prop->GetValue();
                $this->virtualMemoryUsed = $this->totalVirtualMemory - $virtualMemoryAvailable;
            }

            function DisplayValsOnPage()
            {
                $formattedUpTime = FormatTime( $this->upTime, SECONDS );

                echo '  <table width="100%" border="0" cellspacing="2" cellpadding="2" class="dataTable">', "\n";
                echo '      <tr>',"\n";
                echo '          <td class="serverStatusLabel">Time of status check:</td>',"\n";
                echo '          <td class="serverStatusValue">',$this->timeStamp,'</td>',"\n";
                echo '      </tr>',"\n";
                echo '      <tr>',"\n";
                echo '          <td colspan="2">&nbsp;</td>',"\n";
                echo '      </tr>',"\n";
                echo '      <tr>',"\n";
                echo '          <td class="serverStatusLabel">CPU usage:</td>',"\n";
                echo '          <td class="serverStatusValue">',$this->cpuUtilization,'%</td>',"\n";
                echo '      </tr>',"\n";
                echo '      <tr>',"\n";
                echo '          <td class="serverStatusLabel">Up time:</td>',"\n";
                echo '          <td class="serverStatusValue">',$formattedUpTime,'</td>',"\n";
                echo '      </tr>',"\n";
                echo '      <tr>',"\n";
                echo '          <td colspan="2">&nbsp;</td>',"\n";
                echo '      </tr>',"\n";
                echo '      <tr>',"\n";
                echo '          <td class="serverStatusLabel">Admin operations queued:</td>',"\n";
                echo '          <td class="serverStatusValue">',$this->adminOpsQueueSize,'</td>',"\n";
                echo '      </tr>',"\n";
                echo '      <tr>',"\n";
                echo '          <td class="serverStatusLabel">Client operations queued:</td>',"\n";
                echo '          <td class="serverStatusValue">',$this->clientOpsQueueSize,'</td>',"\n";
                echo '      </tr>',"\n";
                echo '      <tr>',"\n";
                echo '          <td class="serverStatusLabel">Site operations queued:</td>',"\n";
                echo '          <td class="serverStatusValue">',$this->siteOpsQueueSize,'</td>',"\n";
                echo '      </tr>',"\n";
                echo '      <tr>',"\n";
                echo '          <td class="serverStatusLabel">Average client operation time:</td>',"\n";
                echo '          <td class="serverStatusValue">',$this->avgClientOpTime,' milliseconds</td>',"\n";
                echo '      </tr>',"\n";
                echo '      <tr>',"\n";
                echo '          <td class="serverStatusLabel">Total operation time:</td>',"\n";
                echo '          <td class="serverStatusValue">',$this->totalOpTime,' seconds</td>',"\n";
                echo '      </tr>',"\n";
                echo '      <tr>',"\n";
                echo '          <td class="serverStatusLabel">Total operations processed:</td>',"\n";
                echo '          <td class="serverStatusValue">',$this->totalOpsProcessed,'</td>',"\n";
                echo '      </tr>',"\n";
                echo '      <tr>',"\n";
                echo '          <td class="serverStatusLabel">Total operations received:</td>',"\n";
                echo '          <td class="serverStatusValue">',$this->totalOpsReceived,'</td>',"\n";
                echo '      </tr>',"\n";
                echo '      <tr>',"\n";
                echo '          <td colspan="2">&nbsp;</td>',"\n";
                echo '      </tr>',"\n";
                echo '      <tr>',"\n";
                echo '          <td class="serverStatusLabel">Total physical memory:</td>',"\n";
                echo '          <td class="serverStatusValue">',$this->totalPhysicalMemory/1024,' KB</td>',"\n";
                echo '      </tr>',"\n";
                echo '      <tr>',"\n";
                echo '          <td class="serverStatusLabel">Physical memory used:</td>',"\n";
                echo '          <td class="serverStatusValue">',$this->physicalMemoryUsed/1024,' KB</td>',"\n";
                echo '      </tr>',"\n";
                echo '      <tr>',"\n";
                echo '          <td class="serverStatusLabel">Total virtual memory:</td>',"\n";
                echo '          <td class="serverStatusValue">',$this->totalVirtualMemory/1024,' KB</td>',"\n";
                echo '      </tr>',"\n";
                echo '      <tr>',"\n";
                echo '          <td class="serverStatusLabel">Virtual memory used:</td>',"\n";
                echo '          <td class="serverStatusValue">',$this->virtualMemoryUsed/1024,' KB</td>',"\n";
                echo '      </tr>',"\n";
                echo '  </table>',"\n";
            }
        }

        // Define Local values
        $pageTitle = "";
        $pageName = 'ViewServerBottomPage';
        $formName = 'ViewServerBottomForm';
        $confirmationMsg = "";
        $errorMsg = "";
        $serverStatus = new ServerHealthPageRecord();

        // Get Server
        $serverRec = GetDataForServer( $selectedServer );
        if ( $serverRec == NULL )
            throw new Exception( sprintf( $errNotFound, $selectedServer ) );
        if ( !$serverRec->poweredUp )
            throw new Exception( sprintf( $errServerIsDown, $selectedServer ) );

        // Get Server Status
        $serverAdmin = new MgServerAdmin();
        $serverAdmin->Open( $selectedServer, $userInfo );
        $serverStatus->GetProps( $serverAdmin );
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
    if ( $autoRefresh )
    {
        $onLoadFmt = 'onLoad="window.setTimeout('."'document.".$formName.".submit();', %d);".'"';
        $onLoadStr = sprintf( $onLoadFmt, $autoRefreshFrequency * 1000 );
    }
    else
        $onLoadStr = "";
?>

<body class="ViewServerStatusBottom" <?php echo $onLoadStr; ?> >

    <?php
        BeginContentArea( $formName );
        DeclareHiddenVars( $pageName );
    ?>

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
                </div>
                <pre><?php echo $serverStatus->DisplayValsOnPage();  ?></pre>
        <?php } ?>

    <?php EndContentArea( true, $formName, NULL ); ?>

</body>
</html>

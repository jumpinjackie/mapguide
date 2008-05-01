<html>

<!--
Copyright (C) 2004-2008 by Autodesk, Inc.

This library is free software; you can redistribute it and/or
modify it under the terms of version 2.1 of the GNU Lesser
General Public License as published by the Free Software Foundation.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
-->

<head>
<title>PHP Test</title>
</head>
<body>

<?php
    include 'common.php';
    echo "<b>Server Admin Service Tests</b><p>";

    // Initialize web tier with the site configuration file.
    InitializeWebTier();

    $cred = new MgUserInformation();
    $cred->SetMgUsernamePassword("Administrator","admin");
    $cred->SetLocale(GetDefaultLocale());
	$cred->SetClientIp(GetClientIp());
    $cred->SetClientAgent(GetClientAgent());

    $serverAdmin = new MgServerAdmin();
    $serverAdmin->Open("localhost", $cred);

    $operation = $_GET['OPERATION'];

    //Get all information properties
    $infoProp = $serverAdmin->GetInformationProperties();

    try
    {
        switch($operation)
        {
            case "ONLINE":
                $serverAdmin->BringOnline();
                if ($serverAdmin->IsOnline())
                {
                  echo "Server online<p>";
                }
                else
                {
                  echo "Server offline<p>";
                }
                break;
            case "OFFLINE":
                $serverAdmin->TakeOffline();
                if ($serverAdmin->IsOnline())
                {
                    echo "Server online<p>";
                }
                else
                {
                    echo "Server offline<p>";
                }
                break;
            case "GETTOTALPHYSICALMEMORY":
                $nMemory = $infoProp->GetItem("TotalPhysicalMemory");
                echo "Total Physical Memory: ". $nMemory->GetValue()." bytes";
                break;
            case "GETAVAILABLEPHYSICALMEMORY":
                $nMemory = $infoProp->GetItem("AvailablePhysicalMemory");
                echo "Available Physical Memory: ".$nMemory->GetValue()." bytes";
                break;
            case "GETTOTALVIRTUALMEMORY":
                $nMemory = $infoProp->GetItem("TotalVirtualMemory");
                echo "Total Virtual Memory: ".$nMemory->GetValue()." bytes";
                break;
            case "GETAVAILABLEVIRTUALMEMORY":
                $nMemory = $infoProp->GetItem("AvailableVirtualMemory");
                echo "Total Available Virtual Memory: ".$nMemory->GetValue()." bytes";
                break;
            case "GETCPUUTILIZATION":
                $nCpuLoad = $infoProp->GetItem("CpuUtilization");
                echo "Current CPU Utilization: ". $nCpuLoad->GetValue()."%";
                break;
            case "GETUPTIME":
                $nUptime = $infoProp->GetItem("Uptime");
                echo "Server uptime: ".$nUptime->GetValue()." seconds";
                break;
            case "GETAVERAGEOPERATIONTIME":
                $nTime = $infoProp->GetItem("AverageOperationTime");
                echo "Average Operation Time: ".$nTime->GetValue()." milliseconds";
                break;
            case "GETTOTALOPERATIONTIME":
                $nTime = $infoProp->GetItem("TotalOperationTime");
                echo "Total Operation Time: ".$nTime->GetValue()." seconds";
                break;
            case "GETADMINQUEUECOUNT":
                $nCount = $infoProp->GetItem("AdminOperationsQueueCount");
                echo "Admin queue count: ".$nCount->GetValue();
                break;
            case "GETCLIENTQUEUECOUNT":
                $nCount = $infoProp->GetItem("ClientOperationsQueueCount");
                echo "Client queue count: ".$nCount->GetValue();
                break;
            case "GETSITEQUEUECOUNT":
                $nCount = $infoProp->GetItem("SiteOperationsQueueCount");
                echo "Site queue count: ".$nCount->GetValue();
                break;
            case "SERVERVERSION":
                $version = $infoProp->GetItem("ServerVersion");
                echo "Server version: ".$version->GetValue();
                break;
            default:
                echo "Error.  Unknown operation: $operation";
        }
    }
    catch (MgException $e)
    {
         print $e->GetMessage('eng');
         print $e->GetDetails('eng');
    }
?>

</body>
</html>

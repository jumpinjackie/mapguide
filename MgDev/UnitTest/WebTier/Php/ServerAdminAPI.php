<?php

//
//  Copyright (C) 2004-2011 by Autodesk, Inc.
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

require_once("Result.php");
require_once("Utils.php");

class ServerAdminAPI
{
    private $cred;
    private $unitTestParamVm;
    private $arrayParam;
    private $serverAdmin;

    function __construct($db)
    {
        Utils::MapAgentInit("../../../Web/src/webconfig.ini");

        $this->cred = new MgUserInformation();
        $this->cred->SetMgUsernamePassword("Administrator","admin");
        $this->cred->SetLocale("en");


        $this->serverAdmin = new MgServerAdmin();
        $this->serverAdmin->Open("localhost", $this->cred);

        $this->unitTestParamVm = new SqliteVM($db, true);
        $this->arrayParam = array();

    }

    function __destruct()
    {
        unset($this->cred);
        unset($this->unitTestParamVm);
        unset($this->arrayParam);
    }

    //Unused for now. Will be used when GetProperties() is implemented in server admin
    function GetProperties($paramSet)
    {
        try
        {
            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"SECTION\"");
            $section = $this->unitTestParamVm->GetString("ParamValue");

            $properties = $this->serverAdmin->GetProperties($section);

            return new Result(Utils::MgStringCollectionToString($properties), "text/plain");
        }
        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
    }

    function TakeOffline($paramSet)
    {
        try
        {
            $this->serverAdmin->TakeOffline();
            $isOnline = $this->serverAdmin->IsOnline();

            return new Result(Utils::BooleanToString($isOnline), "text/plain");
        }
        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
    }

    function BringOnline($paramSet)
    {
        try
        {
            $this->serverAdmin->BringOnline();
            $isOnline = $this->serverAdmin->IsOnline();

            return new Result(Utils::BooleanToString($isOnline), "text/plain");
        }
        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
    }

    function GetLog($paramSet)
    {
        try
        {
            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"LOGTYPE\"");
            $logtype = $this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"NUMENTRIES\"");
            $numEntries = $this->unitTestParamVm->GetString("ParamValue");

            if ($numEntries == null)
            {
                $byteReader = $this->serverAdmin->GetLog($logtype);
            }
            else
            {
                $byteReader = $this->serverAdmin->GetLog($logtype, $numEntries);
            }

            return Utils::GetResponse($byteReader);
        }
        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
    }

    function GetLogByDate($paramSet)
    {
        try
        {
            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"LOGTYPE\"");
            $logtype = $this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"FROMDATE\"");
            $fromDateString = $this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"TODATE\"");
            $toDateString = $this->unitTestParamVm->GetString("ParamValue");

            $fromDatePieces = explode(",", $fromDateString);
            $toDatePieces = explode(",", $toDateString);

            // Build an MgDateTime object based on a comma deliminated string.  If the string does not explode into
            // the expected number of parts, a NULL value will be assigned
            $fromDate = NULL;
            $toDate = NULL;

            if (sizeof($fromDatePieces) == 3)
            {
                $fromDate = new MgDateTime($fromDatePieces[0], $fromDatePieces[1], $fromDatePieces[2]);
            }
            else if (sizeof($fromDatePieces) == 7)
            {
                $fromDate = new MgDateTime($fromDatePieces[0], $fromDatePieces[1], $fromDatePieces[2], $fromDatePieces[3], $fromDatePieces[4], $fromDatePieces[5], $fromDatePieces[6]);
            }

            if (sizeof($toDatePieces) == 3)
            {
                $toDate = new MgDateTime($toDatePieces[0], $toDatePieces[1], $toDatePieces[2]);
            }
            else if (sizeof($toDatePieces) == 7)
            {
                $toDate = new MgDateTime($toDatePieces[0], $toDatePieces[1], $toDatePieces[2], $toDatePieces[3], $toDatePieces[4], $toDatePieces[5], $toDatePieces[6]);
            }

            $byteReader = $this->serverAdmin->GetLog($logtype, $fromDate, $toDate);

            return Utils::GetResponse($byteReader);
        }
        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
    }

    function ClearLog($paramSet)
    {
        try
        {
            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"LOGTYPE\"");
            $logtype = $this->unitTestParamVm->GetString("ParamValue");

            $cleared = $this->serverAdmin->ClearLog($logtype);
            return new Result(Utils::BooleanToString($cleared), "text/plain");
        }
        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
    }

    function EnumerateLogs($paramSet)
    {
        try
        {
            $logs = $this->serverAdmin->EnumerateLogs();
            return new Result(Utils::MgStringCollectionToString($logs), "text/plain");
        }
        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
    }

    function DeleteLog($paramSet)
    {
        try
        {
            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"FILENAME\"");
            $fileName = $this->unitTestParamVm->GetString("ParamValue");

            $byteReader = $this->serverAdmin->DeleteLog($fileName);
            $result = Utils::GetResponse($byteReader);
            return $result;
        }
        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
    }

    function RenameLog($paramSet)
    {
        try
        {
            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"OLDFILENAME\"");
            $oldFileName = $this->unitTestParamVm->GetString("ParamValue");
            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"NEWFILENAME\"");
            $newFileName = $this->unitTestParamVm->GetString("ParamValue");

            $byteReader = $this->serverAdmin->RenameLog($oldFileName, $newFileName);
            $result = Utils::GetResponse($byteReader);
            return $result;
        }
        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
    }

    function EnumeratePackages($paramSet)
    {
        try
        {
            $packages = $this->serverAdmin->EnumeratePackages();
            return new Result(Utils::MgStringCollectionToString($packages), "text/plain");
        }
        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
    }

    function DeletePackage($paramSet)
    {
        try
        {
            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"PACKAGENAME\"");
            $packageName = $this->unitTestParamVm->GetString("ParamValue");

            $byteReader = $this->serverAdmin->DeletePackage($packageName);
            $result = Utils::GetResponse($byteReader);
            return $result;
        }
        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
    }

    function LoadPackage($paramSet)
    {
        try
        {
            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"PACKAGENAME\"");
            $packageName = $this->unitTestParamVm->GetString("ParamValue");

            $byteReader = $this->serverAdmin->LoadPackage($packageName);
            $result = Utils::GetResponse($byteReader);
            return $result;
        }
        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
    }

    function GetPackageStatus($paramSet)
    {
        try
        {
            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"PACKAGENAME\"");
            $packageName = $this->unitTestParamVm->GetString("ParamValue");

            $statusInfo = $this->serverAdmin->GetPackageStatus($packageName);
            $statusCode = $statusInfo->GetStatusCode();

            return new Result($statusCode, "text/plain");
        }
        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
    }

    function GetPackageLog($paramSet)
    {
        try
        {
            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"PACKAGENAME\"");
            $packageName = $this->unitTestParamVm->GetString("ParamValue");

            $byteReader = $this->serverAdmin->GetPackageLog($packageName);
            return Utils::GetResponse($byteReader);
        }
        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
    }
}
?>

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


//This class defines the operations that are currently supported by the unit test infrastructure for Resource Service methods exposed through Htpp.
//There is one function per operation. Each function tries to retrieve the parameters that are required for its corresponding
//operation and send the Http request
//Edit that file to add more operations from Resource Service that are exposed through Http

require_once("HttpRequest.php");
require_once("Utils.php");
require_once("Result.php");

class ResourceServiceHttpRequests
{
    private $unitTestParamVm;
    private $httpRequest;
    private $URL;
    private $db;

    function __construct($db, $newURL)
    {
        $this->db = $db;
        $this->unitTestParamVm = new SqliteVM($db, true);
        $this->httpRequest = new HttpRequest();
        $this->URL = $newURL;
    }

    function __destruct()
    {
        $this->unitTestParamVm->SqlFinalize();
        unset($this->httpRequest);
        unset($this->db);
    }

    function EnumerateResources ($paramSet)
    {
        try
        {
            $arrayParam = array();
            $arrayParam = Utils::SetCommonParams($paramSet, $this->db);

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"RESOURCEID\"");
            $arrayParam["RESOURCEID"]=$this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"TYPE\"");
            $arrayParam["TYPE"]=$this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"DEPTH\"");
            $arrayParam["DEPTH"]=$this->unitTestParamVm->GetString("ParamValue");

            return $this->httpRequest->SendRequest($this->URL, $arrayParam);
        }
        catch (SqliteException $s)
        {
            return new Result($s->GetExceptionMessage(), "text/plain");
        }
    }

    function SetResource($paramSet)
    {
        try
        {
            $arrayParam = array();
            $arrayParam = Utils::SetCommonParams($paramSet, $this->db);

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"RESOURCEID\"");
            $arrayParam['RESOURCEID']=$this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"CONTENT\"");
            if ($this->unitTestParamVm->GetString("ParamValue") != "")
            {
                $arrayParam['CONTENT']=$this->unitTestParamVm->GetString("ParamValue");
                $arrayParam['CONTENT']="@".Utils::GetPath($arrayParam['CONTENT']);
            }

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"HEADER\"");
            if ($this->unitTestParamVm->GetString("ParamValue") != "")
            {
                $arrayParam['HEADER']=$this->unitTestParamVm->GetString("ParamValue");
                $arrayParam['HEADER']="@".Utils::GetPath($arrayParam['HEADER']);

            }

            return $this->httpRequest->SendRequest($this->URL, $arrayParam, "POST");
        }
        catch (SqliteException $s)
        {
            return new Result($s->GetExceptionMessage(), "text/plain");
        }
    }

    function DeleteResource($paramSet)
    {
        try
        {
            $arrayParam = array();
            $arrayParam = Utils::SetCommonParams($paramSet, $this->db);

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"RESOURCEID\"");
            $arrayParam["RESOURCEID"]=$this->unitTestParamVm->GetString("ParamValue");

            return $this->httpRequest->SendRequest($this->URL, $arrayParam);
        }
        catch (SqliteException $s)
        {
            return new Result($s->GetExceptionMessage(), "text/plain");
        }
    }

    function GetResourceContent($paramSet)
    {
        try
        {
            $arrayParam = array();
            $arrayParam = Utils::SetCommonParams($paramSet, $this->db);

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"RESOURCEID\"");
            $arrayParam["RESOURCEID"]=$this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"PREPROCESS\"");
            $arrayParam["PREPROCESS"]=$this->unitTestParamVm->GetString("ParamValue");

            return $this->httpRequest->SendRequest($this->URL, $arrayParam);
        }
        catch (SqliteException $s)
        {
            return new Result($s->GetExceptionMessage(), "text/plain");
        }
    }

    function GetResourceHeader($paramSet)
    {
        try
        {
            $arrayParam = array();
            $arrayParam = Utils::SetCommonParams($paramSet, $this->db);

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"RESOURCEID\"");
            $arrayParam["RESOURCEID"]=$this->unitTestParamVm->GetString("ParamValue");

            return $this->httpRequest->SendRequest($this->URL, $arrayParam);
        }
        catch (SqliteException $s)
        {
            return new Result($s->GetExceptionMessage(), "text/plain");
        }
    }

    function EnumerateResourceData($paramSet)
    {
        try
        {
            $arrayParam = array();
            $arrayParam = Utils::SetCommonParams($paramSet, $this->db);

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"RESOURCEID\"");
            $arrayParam["RESOURCEID"]=$this->unitTestParamVm->GetString("ParamValue");

            return $this->httpRequest->SendRequest($this->URL, $arrayParam);
        }
        catch (SqliteException $s)
        {
            return new Result($s->GetExceptionMessage(), "text/plain");
        }
    }

    function GetResourceData($paramSet)
    {
        try
        {
            $arrayParam = array();
            $arrayParam = Utils::SetCommonParams($paramSet, $this->db);

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"RESOURCEID\"");
            $arrayParam["RESOURCEID"]=$this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"DATANAME\"");
            $arrayParam["DATANAME"]=$this->unitTestParamVm->GetString("ParamValue");

            return $this->httpRequest->SendRequest($this->URL, $arrayParam);
        }
        catch (SqliteException $s)
        {
            return new Result($s->GetExceptionMessage(), "text/plain");
        }
    }

    function SetResourceData($paramSet)
    {
        try
        {
            $arrayParam = array();
            $arrayParam = Utils::SetCommonParams($paramSet, $this->db);

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"RESOURCEID\"");
            $arrayParam["RESOURCEID"]=$this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"DATANAME\"");
            $arrayParam["DATANAME"]=$this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"DATATYPE\"");
            $arrayParam["DATATYPE"]=$this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"DATALENGTH\"");
            $arrayParam["DATALENGTH"]=$this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"DATA\"");
            $arrayParam["DATA"]=$this->unitTestParamVm->GetString("ParamValue");
            $arrayParam["DATA"]="@".Utils::GetPath($arrayParam["DATA"]);

            return $this->httpRequest->SendRequest($this->URL, $arrayParam, "POST");
        }
        catch (SqliteException $s)
        {
            return new Result($s->GetExceptionMessage(), "text/plain");
        }
    }

    function RenameResourceData($paramSet)
    {
        try
        {
            $arrayParam = array();
            $arrayParam = Utils::SetCommonParams($paramSet, $this->db);

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"RESOURCEID\"");
            $arrayParam["RESOURCEID"]=$this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"OLDDATANAME\"");
            $arrayParam["OLDDATANAME"]=$this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"NEWDATANAME\"");
            $arrayParam["NEWDATANAME"]=$this->unitTestParamVm->GetString("ParamValue");

            return $this->httpRequest->SendRequest($this->URL, $arrayParam);
        }
        catch (SqliteException $s)
        {
            return new Result($s->GetExceptionMessage(), "text/plain");
        }
    }

    function DeleteResourceData($paramSet)
    {
        try
        {
            $arrayParam = array();
            $arrayParam = Utils::SetCommonParams($paramSet, $this->db);

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"RESOURCEID\"");
            $arrayParam["RESOURCEID"]=$this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"DATANAME\"");
            $arrayParam["DATANAME"]=$this->unitTestParamVm->GetString("ParamValue");

            return $this->httpRequest->SendRequest($this->URL, $arrayParam);
        }
        catch (SqliteException $s)
        {
            return new Result($s->GetExceptionMessage(), "text/plain");
        }
    }

    function MoveResource($paramSet)
    {
        try
        {
            $arrayParam = array();
            $arrayParam = Utils::SetCommonParams($paramSet, $this->db);

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"SOURCE\"");
            $arrayParam["SOURCE"]=$this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"DESTINATION\"");
            $arrayParam["DESTINATION"]=$this->unitTestParamVm->GetString("ParamValue");

            return $this->httpRequest->SendRequest($this->URL, $arrayParam);
        }
        catch (SqliteException $s)
        {
            return new Result($s->GetExceptionMessage(), "text/plain");
        }
    }

    function ApplyResourcePackage($paramSet)
    {
        try
        {
            $arrayParam = array();
            $arrayParam = Utils::SetCommonParams($paramSet, $this->db);

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"PACKAGE\"");

            if ($this->unitTestParamVm->GetString("ParamValue") != "")
            {
                $arrayParam['PACKAGE']=$this->unitTestParamVm->GetString("ParamValue");
                $arrayParam['PACKAGE']="@".Utils::GetPath($arrayParam['PACKAGE']);
            }

            return $this->httpRequest->SendRequest($this->URL, $arrayParam, "POST");
        }
        catch (SqliteException $s)
        {
            return new Result($s->GetExceptionMessage(), "text/plain");
        }
    }

    function UpdateRepository($paramSet)
    {
        try
        {
            $arrayParam = array();
            $arrayParam = Utils::SetCommonParams($paramSet, $this->db);

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"RESOURCEID\"");
            $arrayParam["RESOURCEID"]=$this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"CONTENT\"");
            if ($this->unitTestParamVm->GetString("ParamValue") != "")
            {
                $arrayParam['CONTENT']=$this->unitTestParamVm->GetString("ParamValue");
                $arrayParam['CONTENT']="@".Utils::GetPath($arrayParam['CONTENT']);
            }

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"HEADER\"");
            if ($this->unitTestParamVm->GetString("ParamValue") != "")
            {
                $arrayParam['HEADER']=$this->unitTestParamVm->GetString("ParamValue");
                $arrayParam['HEADER']="@".Utils::GetPath($arrayParam['HEADER']);
            }

            return $this->httpRequest->SendRequest($this->URL, $arrayParam, "POST");
        }
        catch (SqliteException $s)
        {
            return new Result($s->GetExceptionMessage(), "text/plain");
        }
    }

    function GetRepositoryContent($paramSet)
    {
        try
        {
            $arrayParam = array();
            $arrayParam = Utils::SetCommonParams($paramSet, $this->db);

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"RESOURCEID\"");
            $arrayParam["RESOURCEID"]=$this->unitTestParamVm->GetString("ParamValue");

            return $this->httpRequest->SendRequest($this->URL, $arrayParam);
        }
        catch (SqliteException $s)
        {
            return new Result($s->GetExceptionMessage(), "text/plain");
        }
    }

    function GetRepositoryHeader($paramSet)
    {
        try
        {
            $arrayParam = array();
            $arrayParam = Utils::SetCommonParams($paramSet, $this->db);

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"RESOURCEID\"");
            $arrayParam["RESOURCEID"]=$this->unitTestParamVm->GetString("ParamValue");

            return $this->httpRequest->SendRequest($this->URL, $arrayParam);
        }
        catch (SqliteException $s)
        {
            return new Result($s->GetExceptionMessage(), "text/plain");
        }
    }

    function EnumerateResourceReferences($paramSet)
    {
        try
        {
            $arrayParam = array();
            $arrayParam = Utils::SetCommonParams($paramSet, $this->db);

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"RESOURCEID\"");
            $arrayParam["RESOURCEID"]=$this->unitTestParamVm->GetString("ParamValue");

            return $this->httpRequest->SendRequest($this->URL, $arrayParam);
        }
        catch (SqliteException $s)
        {
            return new Result($s->GetExceptionMessage(), "text/plain");
        }
    }

    function CopyResource($paramSet)
    {
        try
        {
            $arrayParam = array();
            $arrayParam = Utils::SetCommonParams($paramSet, $this->db);

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"SOURCE\"");
            $arrayParam["SOURCE"]=$this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"DESTINATION\"");
            $arrayParam["DESTINATION"]=$this->unitTestParamVm->GetString("ParamValue");

            return $this->httpRequest->SendRequest($this->URL, $arrayParam);
        }
        catch (SqliteException $s)
        {
            return new Result($s->GetExceptionMessage(), "text/plain");
        }
    }

    function ChangeResourceOwner($paramSet)
    {
        try
        {
            $arrayParam = array();
            $arrayParam = Utils::SetCommonParams($paramSet, $this->db);

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"RESOURCEID\"");
            $arrayParam["RESOURCEID"]=$this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"OWNER\"");
            $arrayParam["OWNER"]=$this->unitTestParamVm->GetString("ParamValue");

            return $this->httpRequest->SendRequest($this->URL, $arrayParam);
        }
        catch (SqliteException $s)
        {
            return new Result($s->GetExceptionMessage(), "text/plain");
        }
    }

    function InheritPermissionsFrom($paramSet)
    {
        try
        {
            $arrayParam = array();
            $arrayParam = Utils::SetCommonParams($paramSet, $this->db);

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"RESOURCEID\"");
            $arrayParam["RESOURCEID"]=$this->unitTestParamVm->GetString("ParamValue");

            return $this->httpRequest->SendRequest($this->URL, $arrayParam);
        }
        catch (SqliteException $s)
        {
            return new Result($s->GetExceptionMessage(), "text/plain");
        }
    }
}
?>

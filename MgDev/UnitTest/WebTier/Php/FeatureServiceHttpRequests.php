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


require_once("HttpRequest.php");
require_once("Utils.php");

//This file defines the operations that are currently supported by the unit test infrastructure for Feature Service methods exposed through Htpp.
//There is one function per operation. Each function tries to retrieve the parameters that are required for its corresponding
//operation and send the Http request
//Edit that file to add more operations from Feature Service that are exposed through Http

class FeatureServiceHttpRequests
{
    private $unitTestParamVm;
    private $URL;
    private $db;


    function __construct($db, $newURL)
    {
        $this->db=$db;
        $this->unitTestParamVm = new SqliteVM($db, true);
        $this->httpRequest = new HttpRequest();
        $this->URL = $newURL;
    }


    function __destruct()
    {
        $this->unitTestParamVm->SqlFinalize();
        unset($this->httpRequest);
    }


    function GetFeatureProviders($paramSet)
    {
        try
        {
            $arrayParam = array();
            $arrayParam = Utils::SetCommonParams($paramSet, $this->db);

            return $this->httpRequest->SendRequest($this->URL, $arrayParam);
        }
        catch (SqliteException $s)
        {
            return new Result($s->GetMessage(), "text/plain");
        }
    }


    function GetProviderCapabilities($paramSet)
    {
        try
        {
            $arrayParam = array();
            $arrayParam = Utils::SetCommonParams($paramSet, $this->db);

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"PROVIDER\"");
            $arrayParam["PROVIDER"]=$this->unitTestParamVm->GetString("ParamValue");

            return $this->httpRequest->SendRequest($this->URL, $arrayParam);
        }
        catch (SqliteException $s)
        {
            return new Result($s->GetMessage(), "text/plain");
        }
    }


    function GetConnectionPropertyValues($paramSet)
    {
        try
        {
            $arrayParam = array();
            $arrayParam = Utils::SetCommonParams($paramSet, $this->db);

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"PROVIDER\"");
            $arrayParam["PROVIDER"]=$this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"PROPERTY\"");
            $arrayParam["PROPERTY"]=$this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"CONNECTIONSTRING\"");
            $arrayParam["CONNECTIONSTRING"]=$this->unitTestParamVm->GetString("ParamValue");

            return $this->httpRequest->SendRequest($this->URL, $arrayParam);
        }
        catch (SqliteException $s)
        {
            return new Result($s->GetMessage(), "text/plain");
        }
    }


    function DescribeFeatureSchema($paramSet)
    {
        try
        {
            $arrayParam = array();
            $arrayParam = Utils::SetCommonParams($paramSet, $this->db);

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"RESOURCEID\"");
            $arrayParam["RESOURCEID"]=$this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"SCHEMA\"");
            $arrayParam["SCHEMA"]=$this->unitTestParamVm->GetString("ParamValue");

            return $this->httpRequest->SendRequest($this->URL, $arrayParam);
        }
        catch (SqliteException $s)
        {
            return new Result($s->GetMessage(), "text/plain");
        }
    }


    function SelectFeatures($paramSet)
    {
        try
        {
            $arrayParam = array();
            $arrayParam = Utils::SetCommonParams($paramSet, $this->db);

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"RESOURCEID\"");
            $arrayParam["RESOURCEID"]=$this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"CLASSNAME\"");
            $arrayParam["CLASSNAME"]=$this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"PROPERTIES\"");
            $arrayParam["PROPERTIES"]=$this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"FILTER\"");
            $arrayParam["FILTER"]=$this->unitTestParamVm->GetString("ParamValue");

            return $this->httpRequest->SendRequest($this->URL, $arrayParam);
        }
        catch (SqliteException $s)
        {
            return new Result($s->GetMessage(), "text/plain");
        }
    }


    function SelectAggregates($paramSet)
    {
        try
        {
            $arrayParam = array();
            $arrayParam = Utils::SetCommonParams($paramSet, $this->db);

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"RESOURCEID\"");
            $arrayParam["RESOURCEID"]=$this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"CLASSNAME\"");
            $arrayParam["CLASSNAME"]=$this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"GEOMPROPERTY\"");
            $arrayParam["GEOMPROPERTY"]=$this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"PROPERTIES\"");
            $arrayParam["PROPERTIES"]=$this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"FILTER\"");
            $arrayParam["FILTER"]=$this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"SPATIALOP\"");
            $arrayParam["SPATIALOP"]=$this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"GEOMETRY\"");
            $arrayParam["GEOMETRY"]=$this->unitTestParamVm->GetString("ParamValue");

            return $this->httpRequest->SendRequest($this->URL, $arrayParam);
        }
        catch (SqliteException $s)
        {
            return new Result($s->GetMessage(), "text/plain");
        }
    }


    function ExecuteSqlQuery($paramSet)
    {
        try
        {
            $arrayParam = array();
            $arrayParam = Utils::SetCommonParams($paramSet, $this->db);

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"RESOURCEID\"");
            $arrayParam["RESOURCEID"]=$this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"SQL\"");
            $arrayParam["SQL"]=$this->unitTestParamVm->GetString("ParamValue");

            return $this->httpRequest->SendRequest($this->URL, $arrayParam);
        }
        catch (SqliteException $s)
        {
            return new Result($s->GetMessage(), "text/plain");
        }
    }


    //TODO:How to represent the check box in the request
    function GetSpatialContexts($paramSet)
    {
        try
        {
            $arrayParam = array();
            $arrayParam = Utils::SetCommonParams($paramSet, $this->db);

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"RESOURCEID\"");
            $arrayParam["RESOURCEID"]=$this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"ACTIVEONLY\"");
            $arrayParam["ACTIVEONLY"]=$this->unitTestParamVm->GetString("ParamValue");

            return $this->httpRequest->SendRequest($this->URL, $arrayParam);
        }
        catch (SqliteException $s)
        {
            return new Result($s->GetMessage(), "text/plain");
        }
    }


    //TODO:How to represent the check box in the request
    function GetLongTransactions($paramSet)
    {
        try
        {
            $arrayParam = array();
            $arrayParam = Utils::SetCommonParams($paramSet, $this->db);

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"RESOURCEID\"");
            $arrayParam["RESOURCEID"]=$this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"ACTIVEONLY\"");
            $this->arrayParam['ACTIVEONLY']=$this->unitTestParamVm->GetString("ParamValue")."";

            return $this->httpRequest->SendRequest($this->URL, $arrayParam);
        }
        catch (SqliteException $s)
        {
            return new Result($s->GetMessage(), "text/plain");
        }
    }


    function TestConnection($paramSet)
    {
        try
        {
            $arrayParam = array();
            $arrayParam = Utils::SetCommonParams($paramSet, $this->db);

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"RESOURCEID\"");
            if ($this->unitTestParamVm->GetString("ParamValue") != "")
            {
                $arrayParam['RESOURCEID']=$this->unitTestParamVm->GetString("ParamValue");
            }
            else
            {
                $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"PROVIDER\"");
                $arrayParam["PROVIDER"]=$this->unitTestParamVm->GetString("ParamValue");

                $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"CONNECTIONSTRING\"");
                $arrayParam["CONNECTIONSTRING"]=$this->unitTestParamVm->GetString("ParamValue");
            }

            return $this->httpRequest->SendRequest($this->URL, $arrayParam);
        }
        catch (SqliteException $s)
        {
            return new Result($s->GetMessage(), "text/plain");
        }
    }
}
?>

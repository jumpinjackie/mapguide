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


//This file defines the operations that are currently supported by the unit test infrastructure for Site Service methods exposed through Htpp.
//There is one function per operation. Each function tries to retrieve the parameters that are required for its corresponding
//operation and send the Http request
//Edit that file to add more operations from Site Service that are exposed through Http

require_once("HttpRequest.php");
require_once("Utils.php");

class SiteServiceHttpRequests
{
    private $db;
    private $unitTestParamVm;
    private $URL;

    function __construct($db, $newURL)
    {
        $this->db = $db;
        $this->unitTestParamVm = new SqliteVM($db, true);
        $this->httpRequest = new HttpRequest();
        $this->URL = $newURL;
    }

    function __destruct()
    {
        unset($this->unitTestParamVm);
        unset($this->httpRequest);
        unset($this->db);
    }

    function EnumerateUsers($paramSet)
    {
        try
        {
            $arrayParam = array();
            $arrayParam = Utils::SetCommonParams($paramSet, $this->db);

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"GROUP\"");
            $arrayParam["GROUP"]=$this->unitTestParamVm->GetString("ParamValue");

            return $this->httpRequest->SendRequest($this->URL, $arrayParam);
        }
        catch (SqliteException $s)
        {
            return new Result($s->GetMessage(), "text/plain");
        }
    }

    function EnumerateGroups($paramSet)
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
}
?>

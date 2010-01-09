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

require_once("HttpRequest.php");
require_once("Utils.php");
require_once("../../../Web/src/PhpApi/Constants.php");

//This file defines the operations that are currently supported by the unit test infrastructure for Drawing Service methods exposed through Htpp.
//There is one function per operation. Each function tries to retrieve the parameters that are required for its corresponding
//operation and send the Http request
//Edit that file to add more operations from Drawing Service that are exposed through Http

class MappingServiceHttpRequests
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

        // get rid of the cookie jar...
        if (file_exists($this->httpRequest->cookieJar))
        {
            unlink($this->httpRequest->cookieJar);
        }
    }

    function __destruct()
    {
        unset($this->unitTestParamVm);
        unset($this->httpRequest);
        unset($this->db);
    }

    function GetParam(&$arrayParam, $paramSet, $paramName)
    {
        $select = "Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"$paramName\"";
        $this->unitTestParamVm->Execute($select);
        $paramValue = $this->unitTestParamVm->GetString("ParamValue");
        if (isset($paramValue) && strlen($paramValue) > 0)
        {
            $arrayParam[$paramName] = $paramValue;
        }
    }

    function GetMap($paramSet)
    {
        try
        {
            $arrayParam = array();
            $arrayParam = Utils::SetCommonParams($paramSet, $this->db);

            $this->GetParam($arrayParam, $paramSet, "MAPDEFINITION");
            $this->GetParam($arrayParam, $paramSet, "DWFVERSION");
            $this->GetParam($arrayParam, $paramSet, "EMAPVERSION");
            $this->GetParam($arrayParam, $paramSet, "SESSION");

            $result = $this->httpRequest->SendRequest($this->URL, $arrayParam);

            if ($result->GetHttpStatusCode() != 559)
            {
                //$cookie = $this->httpRequest->GetCookie();
                //if (strlen($cookie) <= 0)
                //{
                //    $result = new Result("No cookie created in GetMap", "text/plain", 559);
                //}
            }

            return $result;
        }
        catch (SqliteException $s)
        {
            return new Result($s->GetMessage().$this->unitTestParamVm->GetErrMsg(), "text/plain");
        }
    }
}
?>

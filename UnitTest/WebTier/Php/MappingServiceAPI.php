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

require_once("Result.php");
require_once("Utils.php");
require_once("../../../Web/src/PhpApi/Constants.php");

//This file defines the operations that are currently supported for Mapping Service API by the unit test infrastructure.
//There is one function per operation. Each function tries to retrieve the parameters that are required for its corresponding
//operation and execute the operation
//Edit that file to add more operations for Drawing Service API

class MappingServiceAPI
{
    private $unitTestParamVm;
    private $cred;
    private $site;
    private $arrayParam;
    private $mappingSrvc;
    private $mimeType;

    function __construct($db)
    {
        $this->mimeType="text/xml";
        Utils::MapAgentInit("../../../Web/src/webconfig.ini");

        $this->cred = new MgUserInformation();
        $this->cred->SetMgUsernamePassword("Administrator","admin");
        $this->cred->SetLocale("en");

        $this->site = new MgSiteConnection();
        $this->site->Open($this->cred);
        $this->mappingSrvc = $this->site->CreateService(MgServiceType::MappingService);

        $this->unitTestParamVm = new SqliteVM($db, true);
        $this->arrayParam = array();
    }

    function __destruct()
    {
        unset($this->site);
        unset($this->cred);
        unset($this->unitTestParamVm);
        unset($this->arrayParam);
        unset($this->mappingSrvc);
    }

    function GetParam($arrayParam, $paramSet, $paramName)
    {
        $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=$paramName");
        $paramValue = $this->unitTestParamVm->GetString("ParamValue");
        if (isset($paramValue))
        {
            $arrayParam[$paramName] = $paramValue;
        }
    }

    //TODO: Not completed yet. Converts $xmlString into a StringCollection object
    function ToMgStringCollection($xmlString)
    {
        $stringCollection = new MgStringCollection();
        while ( (substr_count($xmlString, "<item>")) > 0)
        {
            $start = strpos($xmlString, "<item>");
            $end = strpos($xmlString, "</item>");
        }
    }
}
?>

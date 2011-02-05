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

require_once("../../../Web/src/PhpApi/Constants.php");
require_once("Result.php");
require_once("Utils.php");

//This file defines the operations that are currently supported for Drawing Service API by the unit test infrastructure.
//There is one function per operation. Each function tries to retrieve the parameters that are required for its corresponding
//operation and execute the operation
//Edit that file to add more operations for Drawing Service API

class DrawingServiceAPI
{
    private $unitTestParamVm;
    private $cred;
    private $site;
    private $arrayParam;
    private $drawingSrvc;
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
        $this->drawingSrvc = $this->site->CreateService(MgServiceType::DrawingService);

        $this->unitTestParamVm = new SqliteVM($db, true);
        $this->arrayParam = array();
    }

    function __destruct()
    {
        unset($this->site);
        unset($this->cred);
        unset($this->unitTestParamVm);
        unset($this->arrayParam);
    }

    function DescribeDrawing ($paramSet)
    {
        try
        {
            $repId = null;
            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"RESOURCEID\"");
            $this->arrayParam["RESOURCEID"]=$this->unitTestParamVm->GetString("ParamValue");

            if (array_key_exists("RESOURCEID", $this->arrayParam))
            {
                $repId = new MgResourceIdentifier($this->arrayParam["RESOURCEID"]);
            }

            $byteReader = $this->drawingSrvc->DescribeDrawing($repId);

            return Utils::GetResponse($byteReader);
        }
        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
        catch (SqliteException $s)
        {
            return new Result($s->GetExceptionMessage(), "text/plain");
        }
    }

    function GetDrawing($paramSet)
    {
        try
        {
            $repId = null;
            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"RESOURCEID\"");
            $this->arrayParam['RESOURCEID']=$this->unitTestParamVm->GetString("ParamValue")."";

            if (array_key_exists("RESOURCEID", $this->arrayParam))
            {
                $repId = new MgResourceIdentifier($this->arrayParam["RESOURCEID"]);
            }

            $byteReader = $this->drawingSrvc->GetDrawing($repId);

            return Utils::GetResponse($byteReader);
        }
        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
        catch (SqliteException $s)
        {
            return new Result($s->GetExceptionMessage(), "text/plain");
        }
    }

    function EnumerateDrawingLayers($paramSet)
    {
        try
        {
            $contentReader=null;
            $headerReader=null;
            $repId = null;

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"RESOURCEID\"");
            $this->arrayParam['RESOURCEID']=$this->unitTestParamVm->GetString("ParamValue")."";

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"SECTION\"");
            $this->arrayParam['SECTION']=$this->unitTestParamVm->GetString("ParamValue")."";

            if (array_key_exists("RESOURCEID", $this->arrayParam))
            {
                $repId = new MgResourceIdentifier($this->arrayParam["RESOURCEID"]);
            }

            $collection = $this->drawingSrvc->EnumerateLayers($repId, $this->arrayParam['SECTION']);
            $byteReader = $collection->ToXml();

            return Utils::GetResponse($byteReader);
        }
        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
        catch (SqliteException $s)
        {
            return new Result($s->GetExceptionMessage(), "text/plain");
        }
    }
    function GetDrawingLayer($paramSet)
    {
        try
        {
            $repId = null;
            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"RESOURCEID\"");
            $this->arrayParam["RESOURCEID"]=$this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"SECTION\"");
            $this->arrayParam["SECTION"]=$this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"LAYER\"");
            $this->arrayParam["LAYER"]=$this->unitTestParamVm->GetString("ParamValue");

            if (array_key_exists("RESOURCEID", $this->arrayParam))
            {
                $repId = new MgResourceIdentifier($this->arrayParam["RESOURCEID"]);
            }

            $byteReader = $this->drawingSrvc->GetLayer($repId, $this->arrayParam['SECTION'], $this->arrayParam['LAYER']);

            $this->mimeType = $byteReader->GetMimeType();


            return Utils::GetResponse($byteReader);
        }

        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
        catch (SqliteException $s)
        {
            return new Result($s->GetExceptionMessage(), "text/plain");
        }
    }

    function GetDrawingSection($paramSet)
    {
        try
        {
            $repId = null;

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"RESOURCEID\"");
            $this->arrayParam["RESOURCEID"]=$this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"SECTION\"");
            $this->arrayParam['SECTION']=$this->unitTestParamVm->GetString("ParamValue")."";

            if (array_key_exists("RESOURCEID", $this->arrayParam))
            {
                $repId = new MgResourceIdentifier($this->arrayParam["RESOURCEID"]);
            }
            $byteReader = $this->drawingSrvc->GetSection($repId, $this->arrayParam['SECTION']);

            return Utils::GetResponse($byteReader);
        }

        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
        catch (SqliteException $s)
        {
            return new Result($s->GetExceptionMessage(), "text/plain");
        }
    }

    function EnumerateDrawingSections($paramSet)
    {
        try
        {
            $repId = null;
            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"RESOURCEID\"");
            $this->arrayParam["RESOURCEID"]=$this->unitTestParamVm->GetString("ParamValue");

            if (array_key_exists("RESOURCEID", $this->arrayParam))
            {
                $repId = new MgResourceIdentifier($this->arrayParam["RESOURCEID"]);
            }

            $byteReader = $this->drawingSrvc->EnumerateSections($repId);

            return Utils::GetResponse($byteReader);
        }
        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
        catch (SqliteException $s)
        {
            return new Result($s->GetExceptionMessage(), "text/plain");
        }

    }

    function EnumerateDrawingSectionResources($paramSet)
    {
        try
        {
            $repId = null;

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"RESOURCEID\"");
            $this->arrayParam["RESOURCEID"]=$this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"SECTION\"");
            $this->arrayParam['SECTION']=$this->unitTestParamVm->GetString("ParamValue")."";

            if (array_key_exists("RESOURCEID", $this->arrayParam))
            {
                $repId = new MgResourceIdentifier($this->arrayParam["RESOURCEID"]);
            }

            $byteReader = $this->drawingSrvc->EnumerateSectionResources($repId, $this->arrayParam['SECTION']);

            return Utils::GetResponse($byteReader);
        }
        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
        catch (SqliteException $s)
        {
            return new Result($s->GetExceptionMessage(), "text/plain");
        }
    }

    function GetDrawingSectionResource($paramSet)
    {
        try
        {
            $repId = null;
            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"RESOURCEID\"");
            $this->arrayParam["RESOURCEID"]=$this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"RESOURCENAME\"");
            $this->arrayParam['RESOURCENAME']=$this->unitTestParamVm->GetString("ParamValue")."";

            if (array_key_exists("RESOURCEID", $this->arrayParam))
            {
                $repId = new MgResourceIdentifier($this->arrayParam["RESOURCEID"]);
            }

            $byteReader = $this->drawingSrvc->GetSectionResource($repId, $this->arrayParam['RESOURCENAME']);

            return Utils::GetResponse($byteReader);
        }

        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
        catch (SqliteException $s)
        {
            return new Result($s->GetExceptionMessage(), "text/plain");
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

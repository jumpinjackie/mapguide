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

//This class defines the operations that are currently supported by the unit test infrastructure for Resource Service API .
//There is one function per operation. Each function tries to retrieve the parameters that are required for its corresponding
//operation and executes the operation
//Edit that file to add more operations for Resource Service API

require_once("../../../Web/src/PhpApi/Constants.php");
require_once("Utils.php");
require_once("Result.php");

class ResourceServiceAPI
{
    private $unitTestParamVm;
    private $cred;
    private $site;
    private $resourceSrvc;
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
        $this->resourceSrvc = $this->site->CreateService(MgServiceType::ResourceService);

        $this->unitTestParamVm = new SqliteVM($db, true);
    }

    function __destruct()
    {
        unset($this->site);
        unset($this->cred);
        unset($this->unitTestParamVm);
    }

    function EnumerateResources ($paramSet)
    {
        try
        {
            $arrayParam = array();
            $repId = null;

            if (SQLITE_ROW == $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"RESOURCEID\""))
            {
                $arrayParam["RESOURCEID"]=$this->unitTestParamVm->GetString("ParamValue");
            }

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"TYPE\"");
            $arrayParam["TYPE"]=$this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"DEPTH\"");
            $arrayParam["DEPTH"]=$this->unitTestParamVm->GetString("ParamValue");

            if (array_key_exists("RESOURCEID", $arrayParam))
            {
                $repId = new MgResourceIdentifier($arrayParam["RESOURCEID"]);
            }

            $byteReader = $this->resourceSrvc->EnumerateResources( $repId, $arrayParam["DEPTH"], $arrayParam["TYPE"]);

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

    function SetResource($paramSet)
    {
        try
        {
            $arrayParam = array();
            $repId = null;
            $contentReader=null;
            $headerReader=null;

            if (SQLITE_ROW == $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"RESOURCEID\""))
            {
                $arrayParam["RESOURCEID"]=$this->unitTestParamVm->GetString("ParamValue");
            }

            if (array_key_exists("RESOURCEID", $arrayParam))
            {
                $repId = new MgResourceIdentifier($arrayParam["RESOURCEID"]);
            }

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"CONTENT\"");
            if ($this->unitTestParamVm->GetString("ParamValue") != "")
            {
                $arrayParam['CONTENT']=$this->unitTestParamVm->GetString("ParamValue");
                $arrayParam['CONTENT']=Utils::GetPath($arrayParam['CONTENT']);
                if (file_exists($arrayParam['CONTENT']))
                {
                    //TODO add mime type
                    $contentSource = new MgByteSource($arrayParam['CONTENT']);
                    $contentReader = $contentSource->GetReader();
                }
            }

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"HEADER\"");
            if ($this->unitTestParamVm->GetString("ParamValue") != "")
            {
                $arrayParam['HEADER']=$this->unitTestParamVm->GetString("ParamValue");
                $arrayParam['HEADER']=Utils::GetPath($arrayParam['HEADER']);
                if (file_exists($arrayParam['HEADER']))
                {
                    $headerSource = new MgByteSource($arrayParam['HEADER']);
                    $headerReader = $headerSource->GetReader();
                }
            }

            $byteReader = $this->resourceSrvc->SetResource( $repId, $contentReader, $headerReader);

            $result = Utils::GetResponse($byteReader);

            return $result;
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

    function DeleteResource($paramSet)
    {
        try
        {
            $arrayParam = array();
            $repId = null;

            if (SQLITE_ROW == $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"RESOURCEID\""))
            {
                $arrayParam["RESOURCEID"]=$this->unitTestParamVm->GetString("ParamValue");
            }
            if (array_key_exists("RESOURCEID", $arrayParam))
            {
                $repId = new MgResourceIdentifier($arrayParam["RESOURCEID"]);
            }

            $byteReader = $this->resourceSrvc->DeleteResource($repId);

            $result = Utils::GetResponse($byteReader);

            return $result;
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

    function GetResourceContent($paramSet)
    {
        try
        {
            $arrayParam = array();
            $repId = null;

            if (SQLITE_ROW == $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"RESOURCEID\""))
            {
                $arrayParam["RESOURCEID"]=$this->unitTestParamVm->GetString("ParamValue");
            }

            if (array_key_exists("RESOURCEID", $arrayParam))
            {
                $repId = new MgResourceIdentifier($arrayParam["RESOURCEID"]);
            }
            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"PREPROCESS\"");
            $arrayParam["PREPROCESS"]=$this->unitTestParamVm->GetString("ParamValue");

            $byteReader = $this->resourceSrvc->GetResourceContent($repId);

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

    function GetResourceHeader($paramSet)
    {
        try
        {
            $arrayParam = array();
            $repId = null;

            if (SQLITE_ROW == $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"RESOURCEID\""))
            {
                $arrayParam["RESOURCEID"]=$this->unitTestParamVm->GetString("ParamValue");
            }

            if (array_key_exists("RESOURCEID", $arrayParam))
            {
                $repId = new MgResourceIdentifier($arrayParam["RESOURCEID"]);
            }

            $byteReader = $this->resourceSrvc->GetResourceHeader($repId);

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

    function EnumerateResourceData($paramSet)
    {
        try
        {
            $arrayParam = array();
            $repId = null;

            if (SQLITE_ROW == $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"RESOURCEID\""))
            {
                $arrayParam["RESOURCEID"]=$this->unitTestParamVm->GetString("ParamValue");
            }

            if (array_key_exists("RESOURCEID", $arrayParam))
            {
                $repId = new MgResourceIdentifier($arrayParam["RESOURCEID"]);
            }

            $byteReader = $this->resourceSrvc->EnumerateResourceData($repId);
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

    function GetResourceData($paramSet)
    {
        try
        {
            $arrayParam = array();
            $repId = null;

            if (SQLITE_ROW == $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"RESOURCEID\""))
            {
                $arrayParam["RESOURCEID"]=$this->unitTestParamVm->GetString("ParamValue");
            }

            if (array_key_exists("RESOURCEID", $arrayParam))
            {
                $repId = new MgResourceIdentifier($arrayParam["RESOURCEID"]);
            }

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"DATANAME\"");
            $arrayParam["DATANAME"]=$this->unitTestParamVm->GetString("ParamValue");

            $byteReader = $this->resourceSrvc->GetResourceData($repId, $arrayParam["DATANAME"]);
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

    function SetResourceData($paramSet)
    {
        try
        {
            $arrayParam = array();
            $repId = null;

            if (SQLITE_ROW == $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"RESOURCEID\""))
            {
                $arrayParam["RESOURCEID"]=$this->unitTestParamVm->GetString("ParamValue");
            }

            if (array_key_exists("RESOURCEID", $arrayParam))
            {
                $repId = new MgResourceIdentifier($arrayParam["RESOURCEID"]);
            }

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"DATANAME\"");
            $arrayParam["DATANAME"]=$this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"DATATYPE\"");
            $arrayParam["DATATYPE"]=$this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"DATA\"");
            $arrayParam["DATA"]=$this->unitTestParamVm->GetString("ParamValue");
            $arrayParam["DATA"]=Utils::GetPath($arrayParam["DATA"]);

            $extension = self::GetExtension($arrayParam["DATANAME"]);
            $mimeType = self::GetMimeType($extension);
            $dataSource = new MgByteSource($arrayParam["DATA"]);
            $dataSource->SetMimeType($mimeType);
            $dataReader = $dataSource->GetReader();

            $byteReader = $this->resourceSrvc->SetResourceData($repId, $arrayParam["DATANAME"], $arrayParam["DATATYPE"], $dataReader);

            $result = Utils::GetResponse($byteReader);

            return $result;
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

    function RenameResourceData($paramSet)
    {
        try
        {
            $arrayParam = array();
            $repId = null;

            if (SQLITE_ROW == $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"RESOURCEID\""))
            {
                $arrayParam["RESOURCEID"]=$this->unitTestParamVm->GetString("ParamValue");
            }

            if (array_key_exists("RESOURCEID", $arrayParam))
            {
                $repId = new MgResourceIdentifier($arrayParam["RESOURCEID"]);
            }

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"OLDDATANAME\"");
            $arrayParam["OLDDATANAME"]=$this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"NEWDATANAME\"");
            $arrayParam["NEWDATANAME"]=$this->unitTestParamVm->GetString("ParamValue");

            $byteReader = $this->resourceSrvc->RenameResourceData($repId, $arrayParam["OLDDATANAME"], $arrayParam["NEWDATANAME"], false);

            $result = Utils::GetResponse($byteReader);

            return $result;
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


    function DeleteResourceData($paramSet)
    {
        try
        {
            $arrayParam = array();
            $repId = null;

            if (SQLITE_ROW == $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"RESOURCEID\""))
            {
                $arrayParam["RESOURCEID"]=$this->unitTestParamVm->GetString("ParamValue");
            }

            if (array_key_exists("RESOURCEID", $arrayParam))
            {
                $repId = new MgResourceIdentifier($arrayParam["RESOURCEID"]);
            }

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"DATANAME\"");
            $arrayParam["DATANAME"]=$this->unitTestParamVm->GetString("ParamValue");

            $byteReader = $this->resourceSrvc->DeleteResourceData($repId, $arrayParam["DATANAME"]);

            $result = Utils::GetResponse($byteReader);

            return $result;
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

    function GetRepositoryContent($paramSet)
    {
        try
        {
            $arrayParam = array();
            $repId = null;

            if (SQLITE_ROW == $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"RESOURCEID\""))
            {
                $arrayParam["RESOURCEID"]=$this->unitTestParamVm->GetString("ParamValue");
            }

            if (array_key_exists("RESOURCEID", $arrayParam))
            {
                $repId = new MgResourceIdentifier($arrayParam["RESOURCEID"]);
            }

            $byteReader = $this->resourceSrvc->GetRepositoryContent($repId);

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

    function GetRepositoryHeader($paramSet)
    {
        try
        {
            $arrayParam = array();
            $repId = null;

            if (SQLITE_ROW == $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"RESOURCEID\""))
            {
                $arrayParam["RESOURCEID"]=$this->unitTestParamVm->GetString("ParamValue");
            }

            if (array_key_exists("RESOURCEID", $arrayParam))
            {
                $repId = new MgResourceIdentifier($arrayParam["RESOURCEID"]);
            }

            $byteReader = $this->resourceSrvc->GetRepositoryHeader($repId);

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


    function UpdateRepository($paramSet)
    {
        try
        {
            $arrayParam = array();
            $repId = null;
            $contentReader = null;
            $headerReader = null;

            if (SQLITE_ROW == $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"RESOURCEID\""))
            {
                $arrayParam["RESOURCEID"]=$this->unitTestParamVm->GetString("ParamValue");
            }
            if (array_key_exists("RESOURCEID", $arrayParam))
            {
                $repId = new MgResourceIdentifier($arrayParam["RESOURCEID"]);
            }

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"CONTENT\"");
            if ($this->unitTestParamVm->GetString("ParamValue") != "")
            {
                $arrayParam['CONTENT']=$this->unitTestParamVm->GetString("ParamValue");
                $arrayParam['CONTENT']=Utils::GetPath($arrayParam['CONTENT']);
                if (file_exists($arrayParam['CONTENT']))
                {
                    //TODO add mime type
                    $contentSource = new MgByteSource($arrayParam['CONTENT']);
                    $contentReader = $contentSource->GetReader();
                }
            }

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"HEADER\"");
            if ($this->unitTestParamVm->GetString("ParamValue") != "")
            {
                $arrayParam['HEADER']=$this->unitTestParamVm->GetString("ParamValue");
                $arrayParam['HEADER']=Utils::GetPath($arrayParam['HEADER']);
                if (file_exists($arrayParam['HEADER']))
                {
                    $headerSource = new MgByteSource($arrayParam['HEADER']);
                    $headerReader = $headerSource->GetReader();
                }
            }
            $byteReader = $this->resourceSrvc->UpdateRepository( $repId, $contentReader, $headerReader);

            $result = Utils::GetResponse($byteReader);

            return $result;
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

    function EnumerateResourceReferences($paramSet)
    {
        try
        {
            $arrayParam = array();
            $repId = null;

            if (SQLITE_ROW == $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"RESOURCEID\""))
            {
                $arrayParam["RESOURCEID"]=$this->unitTestParamVm->GetString("ParamValue");
            }

            if (array_key_exists("RESOURCEID", $arrayParam))
            {
                $repId = new MgResourceIdentifier($arrayParam["RESOURCEID"]);
            }

            $byteReader = $this->resourceSrvc->EnumerateReferences( $repId );

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

    function MoveResource($paramSet)
    {
        try
        {
            $arrayParam = array();
            $sourceId = null;
            $destId = null;

            if (SQLITE_ROW == $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"SOURCE\""))
            {
                $arrayParam["SOURCE"]=$this->unitTestParamVm->GetString("ParamValue");
            }

            if (SQLITE_ROW == $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"DESTINATION\""))
            {
                $arrayParam["DESTINATION"]=$this->unitTestParamVm->GetString("ParamValue");
            }

            if (array_key_exists("SOURCE", $arrayParam))
            {
                $sourceId = new MgResourceIdentifier($arrayParam["SOURCE"]);
            }

            if (array_key_exists("DESTINATION", $arrayParam))
            {
                $destId = new MgResourceIdentifier($arrayParam["DESTINATION"]);
            }

            $byteReader = $this->resourceSrvc->MoveResource($sourceId, $destId, false);

            $result = Utils::GetResponse($byteReader);

            return $result;
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

    function CopyResource($paramSet)
    {
        try
        {
            $arrayParam = array();
            $sourceId = null;
            $destId = null;

            if (SQLITE_ROW == $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"SOURCE\""))
            {
                $arrayParam["SOURCE"]=$this->unitTestParamVm->GetString("ParamValue");
            }

            if (SQLITE_ROW == $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"DESTINATION\""))
            {
                $arrayParam["DESTINATION"]=$this->unitTestParamVm->GetString("ParamValue");
            }

            if (array_key_exists("SOURCE", $arrayParam))
            {
                $sourceId = new MgResourceIdentifier($arrayParam["SOURCE"]);
            }

            if (array_key_exists("DESTINATION", $arrayParam))
            {
                $destId = new MgResourceIdentifier($arrayParam["DESTINATION"]);
            }

            $byteReader = $this->resourceSrvc->CopyResource($sourceId, $destId, false);

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

    function ChangeResourceOwner($paramSet)
    {
        try
        {
            $arrayParam = array();
            $repId = null;

            if (SQLITE_ROW == $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"RESOURCEID\""))
            {
                $arrayParam["RESOURCEID"]=$this->unitTestParamVm->GetString("ParamValue");
            }

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"OWNER\"");
            $arrayParam["OWNER"]=$this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"INCLUDEDESCENDANTS\"");
            $arrayParam["INCLUDEDESCENDANTS"]=$this->unitTestParamVm->GetString("ParamValue");

            if (array_key_exists("RESOURCEID", $arrayParam))
            {
                $repId = new MgResourceIdentifier($arrayParam["RESOURCEID"]);
            }

            $byteReader = $this->resourceSrvc->ChangeResourceOwner($repId, $arrayParam["OWNER"], $arrayParam["INCLUDEDESCENDANTS"]);

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

    function InheritPermissionsFrom($paramSet)
    {
        try
        {
            $arrayParam = array();
            $repId = null;

            if (SQLITE_ROW == $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"RESOURCEID\""))
            {
                $arrayParam["RESOURCEID"]=$this->unitTestParamVm->GetString("ParamValue");
            }

            if (array_key_exists("RESOURCEID", $arrayParam))
            {
                $repId = new MgResourceIdentifier($arrayParam["RESOURCEID"]);
            }

            $byteReader = $this->resourceSrvc->InheritPermissionsFrom($repId);

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

    function ApplyResourcePackage($paramSet)
    {
        try
        {
            $arrayParam = array();
            $package = null;
            $packageSource = null;

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"PACKAGE\"");
            $arrayParam["PACKAGE"]=Utils::GetPath($this->unitTestParamVm->GetString("ParamValue"));

            $packageSource = new MgByteSource($arrayParam["PACKAGE"]);
            $package = $packageSource->GetReader();

            $byteReader = $this->resourceSrvc->ApplyResourcePackage($package);

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

    public function GetExtension($name)
    {
        $extension = "";
        $pos = strrpos($name, ".");

        if ($pos === false) // note: three equal signs
        {
            if ("MG_USER_CREDENTIALS" == $name)
            {
                $extension = "txt";
            }
            else
            {
                $extension = "bin";
            }
        }
        else
        {
            $extension = substr($name, $pos - strlen($name) + 1);
        }

        return $extension;
    }

    public function GetMimeType($extension)
    {
        $mimeType = "application/octet-stream";
        $extension = strtolower($extension);

        if ($extension == "agf")
        {
            $mimeType = "application/agf";
        }
        elseif ($extension == "bin")
        {
            $mimeType = "application/octet-stream";
        }
        elseif ($extension == "dwf")
        {
            $mimeType = "model/vnd.dwf";
        }
        elseif ($extension == "jpg" || $extension == "jpeg")
        {
            $mimeType = "image/jpeg";
        }
        elseif ($extension == "png")
        {
            $mimeType = "image/png";
        }
        elseif ($extension == "tif" || $extension == "tiff")
        {
            $mimeType = "image/tiff";
        }
        elseif ($extension == "html")
        {
            $mimeType = "text/html";
        }
        elseif ($extension == "txt")
        {
            $mimeType = "text/plain";
        }
        elseif ($extension == "xml")
        {
            $mimeType = "text/xml";
        }

        return $mimeType;
    }
}
?>

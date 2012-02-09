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
require_once("Utils.php");
require_once("Result.php");

//This file defines the operations that are currently supported by the unit test infrastructure for Feature Service API .
//There is one function per operation. Each function tries to retrieve the parameters that are required for its corresponding
//operation and executes the operation
//Edit that file to add more operations for Feature Service API

class FeatureServiceAPI
{
    private $unitTestParamVm;
    private $cred;
    private $site;
    private $arrayParam;
    private $featureSrvc;
    private $mimeType;


    function __construct($db)
    {
        $this->mimeType="text/xml";
        Utils::MapAgentInit(WEBCONFIGINI);

        $this->cred = new MgUserInformation();
        $this->cred->SetMgUsernamePassword("Administrator","admin");
        $this->cred->SetLocale("en");

        $this->site = new MgSiteConnection();
        $this->site->Open($this->cred);
        $this->featureSrvc = $this->site->CreateService(MgServiceType::FeatureService);

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


    function GetFeatureProviders($paramSet)
    {
        try
        {
            $byteReader = $this->featureSrvc->GetFeatureProviders();
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


    function GetProviderCapabilities($paramSet)
    {
        try
        {
            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"PROVIDER\"");
            $this->arrayParam['PROVIDER']=$this->unitTestParamVm->GetString("ParamValue")."";

            $byteReader = $this->featureSrvc->GetCapabilities($this->arrayParam['PROVIDER']);

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


    function GetConnectionPropertyValues($paramSet)
    {
        try
        {
            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"PROVIDER\"");
            $this->arrayParam['PROVIDER']=$this->unitTestParamVm->GetString("ParamValue")."";

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"PROPERTY\"");
            $arrayParam["PROPERTY"]=$this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"CONNECTIONSTRING\"");
            $arrayParam["CONNECTIONSTRING"]=$this->unitTestParamVm->GetString("ParamValue");

            $connectionProperties = $this->featureSrvc->GetConnectionPropertyValues($this->arrayParam['PROVIDER'], $arrayParam["PROPERTY"], $arrayParam["CONNECTIONSTRING"]);

            return new Result(Utils::MgStringCollectionToString($connectionProperties), "text/plain");
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


    function DescribeFeatureSchema($paramSet)
    {
        try
        {
            $repId = null;

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"RESOURCEID\"");
            $this->arrayParam['RESOURCEID']=$this->unitTestParamVm->GetString("ParamValue")."";

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"SCHEMA\"");
            $this->arrayParam['SCHEMA']=$this->unitTestParamVm->GetString("ParamValue")."";

            if (array_key_exists("RESOURCEID", $this->arrayParam))
            {
                $repId = new MgResourceIdentifier($this->arrayParam["RESOURCEID"]);
            }

            $xmlSchema = $this->featureSrvc->DescribeSchemaAsXml($repId, $this->arrayParam['SCHEMA']);

            return new Result($xmlSchema, "text/xml");
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


    function SelectFeatures($paramSet)
    {
        try
        {
            $repId = null;
            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"RESOURCEID\"");
            $this->arrayParam["RESOURCEID"]=$this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"CLASSNAME\"");
            $this->arrayParam["CLASSNAME"]=$this->unitTestParamVm->GetString("ParamValue");

            //TODO: used the value retrieved from the database to create the MgStringCollection object
            if (SQLITE_ROW == $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"PROPERTIES\""))
            {
                $this->arrayParam["PROPERTIES"]=$this->unitTestParamVm->GetString("ParamValue");
            }

            if (SQLITE_ROW == $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"FILTER\""))
            {
                $this->arrayParam["FILTER"]=$this->unitTestParamVm->GetString("ParamValue");
            }

            $queryOptions = new MgFeatureQueryOptions();
            //TODO: Set query options
            if (array_key_exists("FILTER", $this->arrayParam))
            {
                $queryOptions->SetFilter($this->arrayParam["FILTER"]);
            }

            //PROPERTIES should be stored in the database as comma separated string without spaces
            if (array_key_exists("PROPERTIES", $this->arrayParam))
            {
                $stringArray = explode(",", $this->arrayParam["PROPERTIES"]);

                for ($i=0; $i<count($stringArray); $i++)
                {
                    $queryOptions->AddFeatureProperty($stringArray[$i]);
                }
            }

            if (array_key_exists("RESOURCEID", $this->arrayParam))
            {
                $repId = new MgResourceIdentifier($this->arrayParam["RESOURCEID"]);
            }

            $featureReader = $this->featureSrvc->SelectFeatures($repId, $this->arrayParam['CLASSNAME'], $queryOptions);
            $byteReader = $featureReader->ToXml();
            $featureReader->Close();
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


    //TODO: Verify the implementation is correct
    function SelectAggregates($paramSet)
    {
        try
        {
            $repId = null;
            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"RESOURCEID\"");
            $this->arrayParam["RESOURCEID"]=$this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"CLASSNAME\"");
            $this->arrayParam["CLASSNAME"]=$this->unitTestParamVm->GetString("ParamValue");

            $collection = new MgStringCollection();

            if (array_key_exists("RESOURCEID", $this->arrayParam))
            {
                $repId = new MgResourceIdentifier($this->arrayParam["RESOURCEID"]);
            }

            $options = new MgFeatureAggregateOptions();

            $featureReader = $this->featureSrvc->SelectAggregate($repId, $this->arrayParam['CLASSNAME'], $options);

            $byteReader = $featureReader->ToXml();
            $featureReader->Close();

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


    function ExecuteSqlQuery($paramSet)
    {
        try
        {
            $repId = null;
            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"RESOURCEID\"");
            $this->arrayParam['RESOURCEID']=$this->unitTestParamVm->GetString("ParamValue")."";

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"SQL\"");
            $this->arrayParam['SQL']=$this->unitTestParamVm->GetString("ParamValue")."";

            if (array_key_exists("RESOURCEID", $this->arrayParam))
            {
                $repId = new MgResourceIdentifier($this->arrayParam["RESOURCEID"]);
            }

            $sqlReader = $this->featureSrvc->ExecuteSqlQuery($repId, $this->arrayParam['SQL']);
            $byteReader = $sqlReader->ToXml();
            $sqlReader->Close();

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


    function GetSpatialContexts($paramSet)
    {
        try
        {
            $repId = null;
            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"RESOURCEID\"");
            $this->arrayParam['RESOURCEID']=$this->unitTestParamVm->GetString("ParamValue")."";

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"ACTIVEONLY\"");
            $this->arrayParam['ACTIVEONLY']=$this->unitTestParamVm->GetString("ParamValue")."";

            if ($this->arrayParam['ACTIVEONLY']== "0")
            {
                $active=false;
            }
            else
            {
                $active=true;
            }

            if (array_key_exists("RESOURCEID", $this->arrayParam))
            {
                $repId = new MgResourceIdentifier($this->arrayParam["RESOURCEID"]);
            }

            $spatialContextReader = $this->featureSrvc->GetSpatialContexts($repId, $active);

            $byteReader = $spatialContextReader->ToXml();
            $spatialContextReader->Close();

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


    function GetLongTransactions($paramSet)
    {
        try
        {
            $repId = null;

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"RESOURCEID\"");
            $this->arrayParam['RESOURCEID']=$this->unitTestParamVm->GetString("ParamValue")."";

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"ACTIVEONLY\"");
            $this->arrayParam['ACTIVEONLY']=$this->unitTestParamVm->GetString("ParamValue")."";

            if ($this->arrayParam['ACTIVEONLY']=="0")
            {
                $active=true;
            }
            else
            {
                $active=false;
            }

            if (array_key_exists("RESOURCEID", $this->arrayParam))
            {
                $repId = new MgResourceIdentifier($this->arrayParam["RESOURCEID"]);
            }

            $longTransactionReader = $this->featureSrvc->GetLongTransactions($repId, $active);
            $byteReader = $longTransactionReader->ToXml();
            $longTransactionReader->Close();

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


    function SetLongTransaction($paramSet)
    {
        try
        {
            $repId = null;

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"RESOURCEID\"");
            $this->arrayParam['RESOURCEID']=$this->unitTestParamVm->GetString("ParamValue")."";

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"LONGTRANSACTIONNAME\"");
            $this->arrayParam['LONGTRANSACTIONNAME']=$this->unitTestParamVm->GetString("ParamValue")."";

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"CREATESESSION\"");
            $this->arrayParam['CREATESESSION']=$this->unitTestParamVm->GetString("ParamValue")."";

            if (array_key_exists("RESOURCEID", $this->arrayParam))
            {
                $repId = new MgResourceIdentifier($this->arrayParam["RESOURCEID"]);
            }

            if ($this->arrayParam['CREATESESSION']=="TRUE")
            {
                $mgsite = $this->site->GetSite();
                $sessionID = $mgsite->CreateSession();
                $this->cred->SetMgSessionId($sessionID);
            }

            $response = $this->featureSrvc->SetLongTransaction($repId, $this->arrayParam['LONGTRANSACTIONNAME']);
            $result = new Result(Utils::BooleanToString($response), "text/plain");

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


    function TestConnection($paramSet)
    {
        try
        {
            $response=false;
            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"RESOURCEID\"");
            if ($this->unitTestParamVm->GetString("ParamValue") != "")
            {
                $repId = null;
                $this->arrayParam['RESOURCEID']=$this->unitTestParamVm->GetString("ParamValue");

                if (array_key_exists("RESOURCEID", $this->arrayParam))
                {
                    $repId = new MgResourceIdentifier($this->arrayParam["RESOURCEID"]);
                }

                $response = $this->featureSrvc->TestConnection($repId);
            }
            else
            {
                $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"PROVIDER\"");
                $this->arrayParam["PROVIDER"]=$this->unitTestParamVm->GetString("ParamValue");

                $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"CONNECTIONSTRING\"");
                $this->arrayParam["CONNECTIONSTRING"]=$this->unitTestParamVm->GetString("ParamValue");

                $response = $this->featureSrvc->TestConnection($this->arrayParam["PROVIDER"], $this->arrayParam["CONNECTIONSTRING"]);
            }
            $result = new Result(Utils::BooleanToString($response), "text/plain");

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


    function GetSchemas($paramSet)
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

            $schemas = $this->featureSrvc->GetSchemas($repId);

            return new Result(Utils::MgStringCollectionToString($schemas), "text/plain");
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


    function GetClasses($paramSet)
    {
        try
        {
            $repId = null;

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"RESOURCEID\"");
            $this->arrayParam['RESOURCEID']=$this->unitTestParamVm->GetString("ParamValue")."";

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"SCHEMANAME\"");
            $this->arrayParam['SCHEMANAME']=$this->unitTestParamVm->GetString("ParamValue")."";

            if (array_key_exists("RESOURCEID", $this->arrayParam))
            {
                $repId = new MgResourceIdentifier($this->arrayParam["RESOURCEID"]);
            }

            $classes = $this->featureSrvc->GetClasses($repId, $this->arrayParam['SCHEMANAME']);

            return new Result(Utils::MgStringCollectionToString($classes), "text/plain");
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
}
?>

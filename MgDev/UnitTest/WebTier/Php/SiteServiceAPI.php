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

//This class defines the operations that are currently supported by the unit test infrastructure for Site Service API .
//There is one function per operation. Each function tries to retrieve the parameters that are required for its corresponding
//operation and executes the operation
//Edit that file to add more operations for Site Service API

require_once("Result.php");
require_once("Utils.php");

$previousSession = " ";

class SiteServiceAPI
{
    private $unitTestParamVm;
    private $cred;
    private $site;
    private $arrayParam;
    private $drawingSrvc;

    function __construct($db)
    {
        Utils::MapAgentInit(WEBCONFIGINI);

        $this->cred = new MgUserInformation();
        $this->cred->SetMgUsernamePassword("Administrator","admin");
        $this->cred->SetLocale("en");

        $this->site = new MgSite();
        $this->site->Open($this->cred);

        $this->unitTestParamVm = new SqliteVM($db, true);
        $this->arrayParam = array();
    }

    function __destruct()
    {
        $this->site->Close();
        unset($this->site);
        unset($this->cred);
        unset($this->unitTestParamVm);
        unset($this->arrayParam);
    }

    function CreateSession ($paramSet)
    {
        try
        {
            global $previousSession;
            $userInfo = new MgUserInformation();
            $userInfo->SetMgUsernamePassword("Administrator","admin");
            $userInfo->SetLocale("en");

            $site2 = new MgSite();
            $site2->Open($userInfo);
            $sess = $site2->CreateSession();
            $previousSession = $sess;
            $site2->Close();
            return (new Result($sess, "text/plain"));
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

    function DestroySession($paramSet)
    {
        try
        {
            return (new Result("Not Implemented Yet", "text/plain"));
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

    function GetUserForSession($paramSet)
    {
        try
        {
            global $previousSession;
            $site2 = new MgSite();
            $userInfo = new MgUserInformation();
            $userInfo->SetMgSessionId($previousSession);
            $site2->Open($userInfo);
            $userId = $site2->GetUserForSession();
            $site2->Close();
            return (new Result($userId, "text/plain"));
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


    function GetSiteServerAddress($paramSet)
    {
        try
        {
            $result = $this->site->GetSiteServerAddress();

            return (new Result($result, "text/plain"));
        }
        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
    }
    function EnumerateUsers($paramSet)
    {
        try
        {
            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"GROUP\"");
            $this->arrayParam["GROUP"]=$this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"ROLE\"");
            $this->arrayParam["ROLE"]=$this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"INCLUDEGROUPS\"");
            $this->arrayParam["INCLUDEGROUPS"]=$this->unitTestParamVm->GetString("ParamValue");

            if ($this->arrayParam["ROLE"]!="")
            {
                $byteReader = $this->site->EnumerateUsers($this->arrayParam['GROUP'], $this->arrayParam['ROLE'], $this->arrayParam['INCLUDEGROUPS']);
            }
            else
            {
                $byteReader = $this->site->EnumerateUsers($this->arrayParam['GROUP']);
            }
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

    function AddUser($paramSet)
    {
        try
        {
            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"USERID\"");
            $this->arrayParam["USERID"]=$this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"USERNAME\"");
            $this->arrayParam['USERNAME']=$this->unitTestParamVm->GetString("ParamValue")."";

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"PASSWORD\"");
            $this->arrayParam['PASSWORD']=$this->unitTestParamVm->GetString("ParamValue")."";

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"DESCRIPTION\"");
            $this->arrayParam['DESCRIPTION']=$this->unitTestParamVm->GetString("ParamValue")."";

            $byteReader = $this->site->AddUser($this->arrayParam["USERID"], $this->arrayParam['USERNAME'], $this->arrayParam['PASSWORD'], $this->arrayParam['DESCRIPTION']);

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

    function UpdateUser($paramSet)
    {
        try
        {
            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"USERID\"");
            $this->arrayParam["USERID"]=$this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"NEWUSERID\"");
            $this->arrayParam["NEWUSERID"]=$this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"NEWUSERNAME\"");
            $this->arrayParam['NEWUSERNAME']=$this->unitTestParamVm->GetString("ParamValue")."";

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"NEWPASSWORD\"");
            $this->arrayParam['NEWPASSWORD']=$this->unitTestParamVm->GetString("ParamValue")."";

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"NEWDESCRIPTION\"");
            $this->arrayParam['NEWDESCRIPTION']=$this->unitTestParamVm->GetString("ParamValue")."";

            $byteReader = $this->site->UpdateUser($this->arrayParam["USERID"], $this->arrayParam["NEWUSERID"],  $this->arrayParam['NEWUSERNAME'], $this->arrayParam['NEWPASSWORD'], $this->arrayParam['NEWDESCRIPTION']);

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

    function DeleteUsers($paramSet)
    {
        try
        {
            //TODO: This will not work. How to create a string collection?
            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"USERS\"");
            $this->arrayParam["USERS"]=Utils::StringToMgStringCollection($this->unitTestParamVm->GetString("ParamValue"));

            $byteReader = $this->site->DeleteUsers($this->arrayParam["USERS"]);

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

    function GrantRoleMembershipsToUsers($paramSet)
    {
        try
        {
            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"ROLES\"");
            $this->arrayParam["ROLES"]=Utils::StringToMgStringCollection($this->unitTestParamVm->GetString("ParamValue"));

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"USERS\"");
            $this->arrayParam['USERS']=Utils::StringToMgStringCollection($this->unitTestParamVm->GetString("ParamValue"));

            $byteReader = $this->site->GrantRoleMembershipsToUsers($this->arrayParam['ROLES'], $this->arrayParam['USERS']);

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
    function RevokeRoleMembershipsFromUsers($paramSet)
    {
        try
        {
            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"ROLES\"");
            $this->arrayParam["ROLES"]=Utils::StringToMgStringCollection($this->unitTestParamVm->GetString("ParamValue"));

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"USERS\"");
            $this->arrayParam['USERS']=Utils::StringToMgStringCollection($this->unitTestParamVm->GetString("ParamValue"));

            $byteReader = $this->site->RevokeRoleMembershipsFromUsers($this->arrayParam['ROLES'], $this->arrayParam['USERS']);

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

    function GrantGroupMembershipsToUsers($paramSet)
    {
        try
        {
            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"GROUPS\"");
            $this->arrayParam["GROUPS"]=Utils::StringToMgStringCollection($this->unitTestParamVm->GetString("ParamValue"));

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"USERS\"");
            $this->arrayParam['USERS']=Utils::StringToMgStringCollection($this->unitTestParamVm->GetString("ParamValue"));

            $byteReader = $this->site->GrantGroupMembershipsToUsers($this->arrayParam['GROUPS'], $this->arrayParam['USERS']);

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

    function RevokeGroupMembershipsFromUsers($paramSet)
    {
        try
        {
            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"GROUPS\"");
            $this->arrayParam["GROUPS"]=Utils::StringToMgStringCollection($this->unitTestParamVm->GetString("ParamValue"));

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"USERS\"");
            $this->arrayParam['USERS']=Utils::StringToMgStringCollection($this->unitTestParamVm->GetString("ParamValue"));

            $byteReader = $this->site->RevokeGroupMembershipsFromUsers($this->arrayParam['GROUPS'], $this->arrayParam['USERS']);

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

    function EnumerateGroups($paramSet)
    {
        try
        {
            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"USER\"");
            $this->arrayParam["USER"]=$this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"ROLE\"");
            $this->arrayParam['ROLE']=$this->unitTestParamVm->GetString("ParamValue")."";

            $byteReader = $this->site->EnumerateGroups($this->arrayParam['USER'], $this->arrayParam['ROLE']);

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

    function EnumerateGroups2($paramSet)
    {
        try
        {
            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"USER\"");
            $this->arrayParam["USER"]=$this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"LOGIN\"");
            $this->arrayParam['LOGIN']=$this->unitTestParamVm->GetString("ParamValue")."";

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"PASSWORD\"");
            $this->arrayParam['PASSWORD']=$this->unitTestParamVm->GetString("ParamValue")."";

            $userInfo = new MgUserInformation();
            $userInfo->SetMgUsernamePassword($this->arrayParam['LOGIN'], $this->arrayParam['PASSWORD']);
            $userInfo->SetLocale("en");

            $site2 = new MgSite();
            $site2->Open($userInfo);

            $byteReader = $site2->EnumerateGroups($this->arrayParam['USER']);

            $site2->Close();

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

    function EnumerateRoles2($paramSet)
    {
        try
        {
            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"USER\"");
            $this->arrayParam["USER"]=$this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"LOGIN\"");
            $this->arrayParam['LOGIN']=$this->unitTestParamVm->GetString("ParamValue")."";

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"PASSWORD\"");
            $this->arrayParam['PASSWORD']=$this->unitTestParamVm->GetString("ParamValue")."";

            $userInfo = new MgUserInformation();
            $userInfo->SetMgUsernamePassword($this->arrayParam['LOGIN'], $this->arrayParam['PASSWORD']);
            $userInfo->SetLocale("en");

            $site2 = new MgSite();
            $site2->Open($userInfo);

            $collection = $site2->EnumerateRoles($this->arrayParam['USER']);

            $site2->Close();

            return new Result(Utils::MgStringCollectionToString($collection), "text/plain");
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

    function AddGroup($paramSet)
    {
        try
        {
            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"GROUP\"");
            $this->arrayParam["GROUP"]=$this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"DESCRIPTION\"");
            $this->arrayParam['DESCRIPTION']=$this->unitTestParamVm->GetString("ParamValue")."";

            $byteReader = $this->site->AddGroup($this->arrayParam['GROUP'], $this->arrayParam['DESCRIPTION']);

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

    function UpdateGroup($paramSet)
    {
        try
        {
            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"GROUP\"");
            $this->arrayParam["GROUP"]=$this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"NEWGROUP\"");
            $this->arrayParam["NEWGROUP"]=$this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"NEWDESCRIPTION\"");
            $this->arrayParam['NEWDESCRIPTION']=$this->unitTestParamVm->GetString("ParamValue")."";

            $byteReader = $this->site->UpdateGroup($this->arrayParam['GROUP'], $this->arrayParam['NEWGROUP'], $this->arrayParam['NEWDESCRIPTION']);

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

    function DeleteGroups($paramSet)
    {
        try
        {
            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"GROUPS\"");
            $this->arrayParam["GROUPS"]=Utils::StringToMgStringCollection($this->unitTestParamVm->GetString("ParamValue"));

            $byteReader = $this->site->DeleteGroups($this->arrayParam['GROUPS']);

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

    function GrantRoleMembershipsToGroups($paramSet)
    {
        try
        {
            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"ROLES\"");
            $this->arrayParam["ROLES"]=Utils::StringToMgStringCollection($this->unitTestParamVm->GetString("ParamValue"));

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"GROUPS\"");
            $this->arrayParam['GROUPS']=Utils::StringToMgStringCollection($this->unitTestParamVm->GetString("ParamValue"));

            $byteReader = $this->site->GrantRoleMembershipsToGroups($this->arrayParam['ROLES'], $this->arrayParam['GROUPS']);

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

    function RevokeRoleMembershipsFromGroups($paramSet)
    {
        try
        {
            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"ROLES\"");
            $this->arrayParam["ROLES"]=Utils::StringToMgStringCollection($this->unitTestParamVm->GetString("ParamValue"));

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"GROUPS\"");
            $this->arrayParam['GROUPS']=Utils::StringToMgStringCollection($this->unitTestParamVm->GetString("ParamValue"));

            $byteReader = $this->site->RevokeRoleMembershipsFromGroups($this->arrayParam['ROLES'], $this->arrayParam['GROUPS']);

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

    function EnumerateRoles($paramSet)
    {
        try
        {
            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"USER\"");
            $this->arrayParam["USER"]=$this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"GROUP\"");
            $this->arrayParam['GROUP']=$this->unitTestParamVm->GetString("ParamValue")."";

            $collection = $this->site->EnumerateRoles($this->arrayParam['USER'], $this->arrayParam['GROUP']);

            return new Result(Utils::MgStringCollectionToString($collection), "text/plain");
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

    function EnumerateServers($paramSet)
    {
        try
        {
            $byteReader = $this->site->EnumerateServers();

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

    function AddServer($paramSet)
    {
        try
        {
            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"NAME\"");
            $this->arrayParam["NAME"]=$this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"DESCRIPTION\"");
            $this->arrayParam['DESCRIPTION']=$this->unitTestParamVm->GetString("ParamValue")."";

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"ADDRESS\"");
            $this->arrayParam["ADDRESS"]=$this->unitTestParamVm->GetString("ParamValue");

            $byteReader = $this->site->AddServer($this->arrayParam['NAME'], $this->arrayParam['DESCRIPTION'], $this->arrayParam['ADDRESS']);

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

    function UpdateServer($paramSet)
    {
        try
        {
            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"OLDNAME\"");
            $this->arrayParam["OLDNAME"]=$this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"NEWNAME\"");
            $this->arrayParam['NEWNAME']=$this->unitTestParamVm->GetString("ParamValue")."";

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"NEWDESCRIPTION\"");
            $this->arrayParam["NEWDESCRIPTION"]=$this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"NEWADDRESS\"");
            $this->arrayParam["NEWADDRESS"]=$this->unitTestParamVm->GetString("ParamValue");

            $byteReader = $this->site->UpdateServer($this->arrayParam['OLDNAME'], $this->arrayParam['NEWNAME'], $this->arrayParam['NEWDESCRIPTION'], $this->arrayParam['NEWADDRESS']);

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

    function RemoveServer($paramSet)
    {
        try
        {
            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"NAME\"");
            $this->arrayParam["NAME"]=$this->unitTestParamVm->GetString("ParamValue");

            $byteReader = $this->site->RemoveServer($this->arrayParam['NAME']);

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

    function EnumerateServicesOnServer($paramSet)
    {
        try
        {
            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"NAME\"");
            $this->arrayParam["NAME"]=$this->unitTestParamVm->GetString("ParamValue");

            $collection = $this->site->EnumerateServicesOnServer($this->arrayParam['NAME']);
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

    function AddServicesToServer($paramSet)
    {
        try
        {
            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"NAME\"");
            $this->arrayParam["NAME"]=$this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"SERVICES\"");
            $this->arrayParam["SERVICES"]=$this->unitTestParamVm->GetString("ParamValue");

            $byteReader = $this->site->AddServicesToServer($this->arrayParam['NAME'], $this->arrayParam['SERVICES']);

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

    function RemoveServicesFromServer($paramSet)
    {
        try
        {
            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"NAME\"");
            $this->arrayParam["NAME"]=$this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"SERVICES\"");
            $this->arrayParam["SERVICES"]=$this->unitTestParamVm->GetString("ParamValue");

            $byteReader = $this->site->RemoveServicesFromServer($this->arrayParam['NAME'], $this->arrayParam['SERVICES']);

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
}
?>

<?php

//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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

class MapLayerAPI
{
    private $cred;
    private $resSrvc;
    private $unitTestParamVm;
    private $arrayParam;
    private $site;
    private $map;

    function __construct($db)
    {
        $map = null;

        Utils::MapAgentInit("../../../Web/src/webconfig.ini");

        $this->cred = new MgUserInformation();
        $this->cred->SetMgUsernamePassword("Administrator","admin");
        $this->cred->SetLocale("en");

        $this->site = new MgSiteConnection();
        $this->site->Open($this->cred);
        $this->resSrvc = $this->site->CreateService(MgServiceType::ResourceService);

        $this->unitTestParamVm = new SqliteVM($db, true);
        $this->arrayParam = array();
    }

    function __destruct()
    {
        unset($this->map);
        unset($this->resSrvc);
        unset($this->site);
        unset($this->cred);
        unset($this->unitTestParamVm);
        unset($this->arrayParam);
    }

    function GetMapExtent($paramSet)
    {
        try
        {
            $this->CreateMapFromResource($paramSet);

            $extent = $this->map->GetMapExtent();
            return new Result(Utils::MgEnvelopeToString($extent), "text/plain");
        }
        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
    }

    function GetCoordinateSystem($paramSet)
    {
        try
        {
            $this->CreateMapFromResource($paramSet);

            return new Result($this->map->GetMapSRS(), "text/plain");
        }
        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
    }

    function GetName($paramSet)
    {
        try
        {
            $this->CreateMapFromResource($paramSet);

            return new Result($this->map->GetName(), "text/plain");
        }
        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
    }

    function GetLayers($paramSet)
    {
        try
        {
            $this->CreateMapFromResource($paramSet);

            $layers = $this->map->GetLayers();
            $layerNames = "";
            for($i = 0; $i < $layers->GetCount(); $i ++)
            {
                if($i > 0)
                    $layerNames .= "/";
                $layerNames .= $layers->GetItem($i)->GetName();
            }
            return new Result($layerNames, "text/plain");
        }
        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
    }

    function GetLayerGroups($paramSet)
    {
        try
        {
            $this->CreateMapFromResource($paramSet);

            $groups = $this->map->GetLayerGroups();
            $groupNames = "";
            for($i = 0; $i < $groups->GetCount(); $i ++)
            {
                if($i > 0)
                    $groupNames .= "/";
                $groupNames .= $groups->GetItem($i)->GetName();
            }
            return new Result($groupNames, "text/plain");
        }
        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
    }

    function GetLayerVisibility($paramSet)
    {
        try
        {
            $this->CreateMapFromResource($paramSet);

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"LAYERNAME\"");
            $layerName = $this->unitTestParamVm->GetString("ParamValue");

            $layer = $this->FindLayer($layerName);
            if($layer == null)
                return new Result("***Unknown Layer***", "text/plain");
            return new Result(Utils::BooleanToString($layer->GetVisible()), "text/plain");
        }
        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
    }

    function GetDisplayInLegend($paramSet)
    {
        try
        {
            $this->CreateMapFromResource($paramSet);

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"LAYERNAME\"");
            $layerName = $this->unitTestParamVm->GetString("ParamValue");

            $layer = $this->FindLayer($layerName);
            if($layer == null)
                return new Result("***Unknown Layer***", "text/plain");
            return new Result(Utils::BooleanToString($layer->GetDisplayInLegend()), "text/plain");
        }
        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
    }

    function GetLegendLabel($paramSet)
    {
        try
        {
            $this->CreateMapFromResource($paramSet);

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"LAYERNAME\"");
            $layerName = $this->unitTestParamVm->GetString("ParamValue");

            $layer = $this->FindLayer($layerName);
            if($layer == null)
                return new Result("***Unknown Layer***", "text/plain");
            return new Result($layer->GetLegendLabel(), "text/plain");
        }
        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
    }

    function GetLayerDefinition($paramSet)
    {
        try
        {
            $this->CreateMapFromResource($paramSet);
            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"LAYERNAME\"");
            $layerName = $this->unitTestParamVm->GetString("ParamValue");

            $layer = $this->FindLayer($layerName);
            if($layer == null)
                return new Result("***Unknown Layer***", "text/plain");
            return new Result($layer->GetLayerDefinition()->ToString(), "text/plain");
        }
        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
    }

    function IsLayerVisible($paramSet)
    {
        try
        {
            $this->CreateMapFromResource($paramSet);

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"LAYERNAME\"");
            $layerName = $this->unitTestParamVm->GetString("ParamValue");

            $layer = $this->FindLayer($layerName);
            if($layer == null)
                return new Result("***Unknown Layer***", "text/plain");
            return new Result(Utils::BooleanToString($layer->IsVisible()), "text/plain");
        }
        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
    }

    function AddGroup($paramSet)
    {
        try
        {
            $this->CreateMapFromResource($paramSet);

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"GROUPNAME\"");
            $groupName = $this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"PARENTGROUPNAME\"");
            $parentGroupName = $this->unitTestParamVm->GetString("ParamValue");

            $groups = $this->map->GetLayerGroups();
            $group = new MgLayerGroup($groupName);
            if($parentGroupName != "")
            {
                $parentGroup = $group->SetGroup($groups->GetItem($parentGroupName));
                $group->SetGroup($parentGroup);
            }
            $groups->Add($group);

            return new Result($groups->GetCount(), "text/plain");
        }
        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
    }

    function AddLayer($paramSet)
    {
        try
        {
            $this->CreateMapFromResource($paramSet);

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"LAYERDEFINITION\"");
            $layerDef = $this->unitTestParamVm->GetString("ParamValue");

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"GROUPNAME\"");
            $groupName = $this->unitTestParamVm->GetString("ParamValue");

            $layer = new MgLayer(new MgResourceIdentifier($layerDef), $this->resSrvc);
            if($groupName != "")
            {
                $groups = $this->map->GetLayerGroups();
                $layer->SetGroup($groups->GetItem($groupName));
            }

            $layers = $this->map->GetLayers();
            $layers->Add($layer);

            return new Result($layers->GetCount(), "text/plain");
        }
        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
    }

    function HideGroup($paramSet)
    {
        try
        {
            $this->CreateMapFromResource($paramSet);

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"GROUPNAME\"");
            $groupName = $this->unitTestParamVm->GetString("ParamValue");

            $groups = $this->map->GetLayerGroups();
            $group = $groups->GetItem($groupName);

            $group->SetVisible(false);

            return new Result(Utils::BooleanToString($group->GetVisible()), "text/plain");
        }
        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
    }

    function ShowGroup($paramSet)
    {
        try
        {
            $this->CreateMapFromResource($paramSet);

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"GROUPNAME\"");
            $groupName = $this->unitTestParamVm->GetString("ParamValue");

            $groups = $this->map->GetLayerGroups();
            $group = $groups->GetItem($groupName);

            $group->SetVisible(true);

            return new Result(Utils::BooleanToString($group->GetVisible()), "text/plain");
        }
        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
    }

    function GetViewCenter($paramSet)
    {
        try
        {
            $this->CreateMapFromResource($paramSet);

            return new Result(Utils::MgPointToString($this->map->GetViewCenter()), "text/plain");
        }
        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
    }

    function GetViewScale($paramSet)
    {
        try
        {
            $this->CreateMapFromResource($paramSet);

            return new Result($this->map->GetViewScale(), "text/plain");
        }
        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
    }

    function GetDataExtent($paramSet)
    {
        try
        {
            $this->CreateMapFromResource($paramSet);

            $extent = $this->map->GetDataExtent();
            return new Result(Utils::MgEnvelopeToString($extent), "text/plain");
        }
        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
    }

    function RemoveGroup($paramSet)
    {
        try
        {
            $this->CreateMapFromResource($paramSet);

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"GROUPNAME\"");
            $groupName = $this->unitTestParamVm->GetString("ParamValue");

            $groups = $this->map->GetLayerGroups();
            $group = $groups->GetItem($groupName);

            $groups->Remove($group);

            return new Result("", "text/plain");
        }
        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
    }

    function LayerExists($paramSet)
    {
        try
        {
            $this->CreateMapFromResource($paramSet);
            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"LAYERNAME\"");
            $layerName = $this->unitTestParamVm->GetString("ParamValue");

            $exists = false;
            $layers = $this->map->GetLayers();
            for($i = 0; $i < $layers->GetCount(); $i++)
            {
                $layer = $layers->GetItem($i);
                if($layer->GetName() == $layerName)
                {
                    $exists = true;
                    break;
                }
            }

            return new Result(Utils::BooleanToString($exists, "text/plain"));
        }
        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
    }

    function GetFeatureSourceId($paramSet)
    {
        try
        {
            $this->CreateMapFromResource($paramSet);

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"LAYERNAME\"");
            $layerName = $this->unitTestParamVm->GetString("ParamValue");

            $layer = $this->FindLayer($layerName);
            if($layer == null)
                return new Result("***Unknown Layer***", "text/plain");
            return new Result($layer->GetFeatureSourceId(), "text/plain");
        }
        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
    }

    function GetFeatureClassName($paramSet)
    {
        try
        {
            $this->CreateMapFromResource($paramSet);

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"LAYERNAME\"");
            $layerName = $this->unitTestParamVm->GetString("ParamValue");

            $layer = $this->FindLayer($layerName);
            return new Result($layer->GetFeatureClassName(), "text/plain");
        }
        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
    }

    function FindLayer($name)
    {
        $layers = $this->map->GetLayers();
        for($i = 0; $i < $layers->GetCount(); $i++)
        {
            $layer = $layers->GetItem($i);
            if($layer->GetName() == $name)
                return $layer;
        }
        return null;
    }

    function CreateMapFromResource($paramSet)
    {
        if($this->map != null)
            return;

        $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"MAPDEFINITION\"");
        $mapName = $this->unitTestParamVm->GetString("ParamValue");
        if ($mapName == null || strlen($mapName) < 1)
        {
            $mapName = "Library://maplayertest/World.MapDefinition";
        }

        $mapDefId = new MgResourceIdentifier($mapName);
        $this->map = new MgMap();
        $this->map->Create($this->resSrvc, $mapDefId, $mapDefId->GetName());
    }
}
?>

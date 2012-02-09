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

class WebLayoutAPI
{
    private $cred;
    private $resSrvc;
    private $unitTestParamVm;
    private $arrayParam;
    private $site;
    private $wl;

    function __construct($db)
    {
        $wl = null;

        Utils::MapAgentInit(WEBCONFIGINI);

        $this->cred = new MgUserInformation();
        $this->cred->SetMgUsernamePassword("Administrator","admin");
        $this->cred->SetLocale("en");

        $this->site = new MgSiteConnection();
        $this->site->Open($this->cred);
        $this->resSrvc = $this->site->CreateService(MgServiceType::ResourceService);

        $this->unitTestParamVm = new SqliteVM($db, true);
        $this->arrayParam = array();

    }

    function GetTitle($paramSet)
    {
        try
        {
            $this->CreateWebLayoutFromResource($paramSet);

            return new Result($this->wl->GetTitle(), "text/plain");
        }
        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
    }

    function GetMapDefinition($paramSet)
    {
        try
        {
            $this->CreateWebLayoutFromResource($paramSet);

            return new Result($this->wl->GetMapDefinition(), "text/plain");
        }
        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
    }

    function GetScale($paramSet)
    {
        try
        {
            $this->CreateWebLayoutFromResource($paramSet);

            return new Result($this->wl->GetScale(), "text/plain");
        }
        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
    }

    function GetCenter($paramSet)
    {
        try
        {
            $this->CreateWebLayoutFromResource($paramSet);
            $center = $this->wl->GetCenter();

            if($center == null)
                return new Result("", "text/plain");
            else
                return new Result($center->GetCoordinate()->GetX() . "/" . $center->GetCoordinate()->GetY(), "text/plain");
        }
        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
    }

    function ShowToolbar($paramSet)
    {
        try
        {
            $this->CreateWebLayoutFromResource($paramSet);
            $toolBar = $this->wl->GetToolBar();

            return new Result(Utils::BooleanToString($toolBar->IsVisible()), "text/plain");
        }
        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
    }

    function ShowStatusbar($paramSet)
    {
        try
        {
            $this->CreateWebLayoutFromResource($paramSet);
            $statusBar = $this->wl->GetStatusBar();

            return new Result(Utils::BooleanToString($statusBar->IsVisible()), "text/plain");
        }
        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
    }

    function ShowTaskpane($paramSet)
    {
        try
        {
            $this->CreateWebLayoutFromResource($paramSet);
            $taskPane = $this->wl->GetTaskPane();

            return new Result(Utils::BooleanToString($taskPane->IsVisible()), "text/plain");
        }
        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
    }

    function ShowTaskbar($paramSet)
    {
        try
        {
            $this->CreateWebLayoutFromResource($paramSet);
            $taskPane = $this->wl->GetTaskPane();

            return new Result(Utils::BooleanToString($taskPane->GetTaskBar()->IsVisible()), "text/plain");
        }
        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
    }

    function ShowLegend($paramSet)
    {
        try
        {
            $this->CreateWebLayoutFromResource($paramSet);
            $infoPane = $this->wl->GetInformationPane();

            return new Result(Utils::BooleanToString($infoPane->IsLegendBandVisible()), "text/plain");
        }
        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
    }

    function ShowProperties($paramSet)
    {
        try
        {
            $this->CreateWebLayoutFromResource($paramSet);
            $infoPane = $this->wl->GetInformationPane();

            return new Result(Utils::BooleanToString($infoPane->IsPropertiesBandVisible()), "text/plain");
        }
        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
    }

    function GetTaskPaneWidth($paramSet)
    {
        try
        {
            $this->CreateWebLayoutFromResource($paramSet);
            $taskPane = $this->wl->GetTaskPane();

            return new Result($taskPane->GetWidth(), "text/plain");
        }
        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
    }

    function GetInformationPaneWidth($paramSet)
    {
        try
        {
            $this->CreateWebLayoutFromResource($paramSet);
            $infoPane = $this->wl->GetInformationPane();

            return new Result($infoPane->GetWidth(), "text/plain");
        }
        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
    }

    function GetInitialTaskUrl($paramSet)
    {
        try
        {
            $this->CreateWebLayoutFromResource($paramSet);
            $taskPane = $this->wl->GetTaskPane();

            return new Result($taskPane->GetInitialTaskUrl(), "text/plain");
        }
        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
    }

    function ShowContextMenu($paramSet)
    {
        try
        {
            $this->CreateWebLayoutFromResource($paramSet);
            $ctxMenu = $this->wl->GetTaskPane();

            return new Result(Utils::BooleanToString($ctxMenu->IsVisible()), "text/plain");
        }
        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
    }

    function TestWidget($paramSet)
    {
        try
        {
            $this->CreateWebLayoutFromResource($paramSet);

            $coll = null;

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"CONTAINER\"");
            $container = $this->unitTestParamVm->GetString("ParamValue");
            if($container == "toolbar")
                $coll = $this->wl->GetToolbar()->GetWidgets();
            else if($container == "tasklist")
                $coll = $this->wl->GetTaskPane()->GetTaskBar()->GetTaskList();
            else if($container == "contextmenu")
                $coll = $this->wl->GetContextMenu();

            $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"INDEX\"");
            $index = intval($this->unitTestParamVm->GetString("ParamValue"));

            $widget = $coll->GetWidget($index);
            if($widget == null)
                $str = "Null widget";
            else
                $str = $this->FormatProperties($widget);

            return new Result($str, "text/plain");
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

    function HomeTooltip($paramSet)
    {
        try
        {
            $this->CreateWebLayoutFromResource($paramSet);
            $taskBar = $this->wl->GetTaskPane()->GetTaskBar();
            $home = $taskBar->GetTaskButtons()->GetWidget(MgWebTaskButtonType::Home);

            return new Result($home->GetTooltip(), "text/plain");
        }
        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
    }

    function HomeDescription($paramSet)
    {
        try
        {
            $this->CreateWebLayoutFromResource($paramSet);
            $taskBar = $this->wl->GetTaskPane()->GetTaskBar();
            $home = $taskBar->GetTaskButtons()->GetWidget(MgWebTaskButtonType::Home);

            return new Result($home->GetDescription(), "text/plain");
        }
        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
    }

    function BackTooltip($paramSet)
    {
        try
        {
            $this->CreateWebLayoutFromResource($paramSet);
            $taskBar = $this->wl->GetTaskPane()->GetTaskBar();
            $back = $taskBar->GetTaskButtons()->GetWidget(MgWebTaskButtonType::Back);

            return new Result($back->GetTooltip(), "text/plain");
        }
        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
    }

    function BackDescription($paramSet)
    {
        try
        {
            $this->CreateWebLayoutFromResource($paramSet);
            $taskBar = $this->wl->GetTaskPane()->GetTaskBar();
            $back = $taskBar->GetTaskButtons()->GetWidget(MgWebTaskButtonType::Back);

            return new Result($back->GetDescription(), "text/plain");
        }
        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
    }

    function ForwardTooltip($paramSet)
    {
        try
        {
            $this->CreateWebLayoutFromResource($paramSet);
            $taskBar = $this->wl->GetTaskPane()->GetTaskBar();
            $forward = $taskBar->GetTaskButtons()->GetWidget(MgWebTaskButtonType::Forward);

            return new Result($forward->GetTooltip(), "text/plain");
        }
        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
    }

    function ForwardDescription($paramSet)
    {
        try
        {
            $this->CreateWebLayoutFromResource($paramSet);
            $taskBar = $this->wl->GetTaskPane()->GetTaskBar();
            $forward = $taskBar->GetTaskButtons()->GetWidget(MgWebTaskButtonType::Forward);

            return new Result($forward->GetDescription(), "text/plain");
        }
        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
    }

    function TasksName($paramSet)
    {
        try
        {
            $this->CreateWebLayoutFromResource($paramSet);
            $taskBar = $this->wl->GetTaskPane()->GetTaskBar();
            $tasks = $taskBar->GetTaskButtons()->GetWidget(MgWebTaskButtonType::Tasks);

            return new Result($tasks->GetName(), "text/plain");
        }
        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
    }

    function TasksTooltip($paramSet)
    {
        try
        {
            $this->CreateWebLayoutFromResource($paramSet);
            $taskBar = $this->wl->GetTaskPane()->GetTaskBar();
            $tasks = $taskBar->GetTaskButtons()->GetWidget(MgWebTaskButtonType::Tasks);

            return new Result($tasks->GetTooltip(), "text/plain");
        }
        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
    }

    function TasksDescription($paramSet)
    {
        try
        {
            $this->CreateWebLayoutFromResource($paramSet);
            $taskBar = $this->wl->GetTaskPane()->GetTaskBar();
            $tasks = $taskBar->GetTaskButtons()->GetWidget(MgWebTaskButtonType::Tasks);

            return new Result($tasks->GetDescription(), "text/plain");
        }
        catch (MgException $e)
        {
            return new Result(get_class($e), "text/plain");
        }
    }

    function CreateWebLayoutFromResource($paramSet)
    {
        if($this->wl != null)
            return;

        $this->unitTestParamVm->Execute("Select ParamValue from Params WHERE ParamSet=$paramSet AND ParamName=\"WEBLAYOUT\"");
        $wlId = $this->unitTestParamVm->GetString("ParamValue");
        if ($wlId == null || strlen($wlId) < 1)
        {
            $wlId = "Library://UnitTest/layouts/Test.WebLayout";
        }

        $resId = new MgResourceIdentifier($wlId);

        $this->wl = new MgWebLayout($this->resSrvc,$resId);
    }

    function FormatProperties($it)
    {
        // recursively goes into subitems definition
        //
        switch($it->GetType())
        {
            case MgWebWidgetType::Separator:
                $name = "";
                break;
            case MgWebWidgetType::Command:
                $name = $it->GetCommand()->GetLabel();
                break;
            case MgWebWidgetType::Flyout:
                $name = $it->GetLabel();
                break;
        }
        $str = "[" . $name . "/" . $it->GetType();
        if($it->GetType() == MgWebWidgetType::Separator)
            return $str . "]";
        else if($it->GetType() == MgWebWidgetType::Command)
            return $str . "/" . $it->GetCommand()->GetName() . "]";
        else if($it->GetType() == MgWebWidgetType::Flyout)
        {
            $str = $str . "/" . $it->GetIconUrl();
            $coll = $it->GetSubItems();
            for($i = 0; $i < $coll->GetCount(); $i++)
            {
                $str = $str . "/" . $this->FormatProperties($coll->GetWidget($i));
            }
            $str = $str . "]";
        }
        else
            return "[** error **]";

        return $str;
    }

    function __destruct()
    {
        unset($this->wl);
        unset($this->resSrvc);
        unset($this->site);
        unset($this->cred);
        unset($this->unitTestParamVm);
        unset($this->arrayParam);
    }
}
?>

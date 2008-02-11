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


require_once("DrawingServiceAPI.php");
require_once("FeatureServiceAPI.php");
require_once("ResourceServiceAPI.php");
require_once("SiteServiceAPI.php");
require_once("ServerAdminAPI.php");
require_once("MapLayerAPI.php");
require_once("MappingServiceAPI.php");
require_once("WebLayoutAPI.php");

require_once("ResourceServiceHttpRequests.php");
require_once("DrawingServiceHttpRequests.php");
require_once("FeatureServiceHttpRequests.php");
require_once("SiteServiceHttpRequests.php");
require_once("MappingServiceHttpRequests.php");
require_once("WfsHttpRequests.php");
require_once("WmsHttpRequests.php");

require_once("Result.php");

//This class creates an object for each possible service that is supported by the infrastructure
//Then, it parses the operation name and based on that executes the operation
//Edit this file to add more services and/or operations to the infrastructure
class ExecuteOperation
{
    private $resourceServiceOperation;
    private $drawingServiceOperation;
    private $featureServiceOperation;
    private $mappingServiceOperation;
    private $siteServiceOperation;
    private $serverAdminOperation;
    private $mapLayerOperation;
    private $webLayoutOperation;
    private $wfsOperation;
    private $wmsOperation;

    public function __construct($requestType, $db, $Url)
    {
        $this->contentType="";
        if (substr_count($requestType, "Http"))
        {
            $this->resourceServiceOperation = new ResourceServiceHttpRequests($db, $Url);
            $this->drawingServiceOperation = new DrawingServiceHttpRequests($db, $Url);
            $this->featureServiceOperation = new FeatureServiceHttpRequests($db, $Url);
            $this->siteServiceOperation = new SiteServiceHttpRequests($db, $Url);
            $this->mappingServiceOperation = new MappingServiceHttpRequests($db, $Url);
            $this->wfsOperation = new WfsHttpRequests($db, $Url);
            $this->wmsOperation = new WmsHttpRequests($db, $Url);
        }
        elseif (substr_count($requestType,"Api"))
        {
            $this->resourceServiceOperation = new ResourceServiceAPI($db);
            $this->drawingServiceOperation = new DrawingServiceAPI($db);
            $this->featureServiceOperation = new FeatureServiceAPI($db);
            $this->siteServiceOperation = new SiteServiceAPI($db);
            $this->serverAdminOperation = new ServerAdminAPI($db);
            $this->mapLayerOperation = new MapLayerAPI($db);
            $this->mappingServiceOperation = new MappingServiceAPI($db);
            $this->webLayoutOperation = new WebLayoutAPI($db);
        }
        else
        {
            //Error
        }
    }

    public function __destruct()
    {
        unset($this->resourceServiceOperation);
        unset($this->drawingServiceOperation);
        unset($this->featureServiceOperation);
        unset($this->serverAdminOperation);
        unset($this->mapLayerOperation);
        unset($this->mappingServiceOperation);
        unset($this->webLayoutOperation);
    }

    public function Execute($paramSet, $operationName)
    {
        $actualResult=new Result();
        //Start Resource Service
        if ($operationName == "ENUMERATERESOURCES")
        {
            $actualResult = $this->resourceServiceOperation->EnumerateResources($paramSet);
        }
        elseif ($operationName == "DELETERESOURCE")
        {
            $actualResult = $this->resourceServiceOperation->DeleteResource($paramSet);
        }
        elseif ($operationName == "GETRESOURCECONTENT")
        {
            $actualResult = $this->resourceServiceOperation->GetResourceContent($paramSet);
        }
        elseif ($operationName == "GETRESOURCEHEADER")
        {
            $actualResult = $this->resourceServiceOperation->GetResourceHeader($paramSet);
        }
        elseif ($operationName == "ENUMERATERESOURCEDATA")
        {
            $actualResult = $this->resourceServiceOperation->EnumerateResourceData($paramSet);
        }
        elseif ($operationName == "GETRESOURCEDATA")
        {
            $actualResult = $this->resourceServiceOperation->GetResourceData($paramSet);
        }
        elseif ($operationName =="SETRESOURCEDATA")
        {
            $actualResult = $this->resourceServiceOperation->SetResourceData($paramSet);
        }
        elseif ($operationName =="UPDATERESOURCEDATA")
        {
            $actualResult = $this->resourceServiceOperation->UpdateResourceData($paramSet);
        }
        elseif ($operationName == "DELETERESOURCEDATA")
        {
            $actualResult = $this->resourceServiceOperation->DeleteResourceData($paramSet);
        }
        elseif ($operationName == "MOVERESOURCE")
        {
            $actualResult = $this->resourceServiceOperation->MoveResource($paramSet);
        }
        elseif ($operationName == "APPLYRESOURCEPACKAGE")
        {
            $actualResult = $this->resourceServiceOperation->ApplyResourcePackage($paramSet);
        }
        elseif ($operationName == "UPDATEREPOSITORY")
        {
            $actualResult = $this->resourceServiceOperation->UpdateRepository($paramSet);
        }
        elseif ($operationName == "ENUMERATEREPOSITORIES")
        {
            $actualResult = $this->resourceServiceOperation->EnumerateRepository($paramSet);
        }
        elseif ($operationName == "GETREPOSITORYCONTENT")
        {
            $actualResult = $this->resourceServiceOperation->GetRepositoryContent($paramSet);
        }
        elseif ($operationName == "GETREPOSITORYHEADER")
        {
            $actualResult = $this->resourceServiceOperation->GetRepositoryHeader($paramSet);
        }
        elseif ($operationName == "RENAMERESOURCEDATA")
        {
            $actualResult = $this->resourceServiceOperation->RenameResourceData($paramSet);
        }
        elseif ($operationName == "ENUMERATERESOURCEREFERENCES")
        {
            $actualResult = $this->resourceServiceOperation->EnumerateResourceReferences($paramSet);
        }
        elseif ($operationName == "COPYRESOURCE")
        {
            $actualResult = $this->resourceServiceOperation->CopyResource($paramSet);
        }
        elseif ($operationName == "CHANGERESOURCEOWNER")
        {
            $actualResult = $this->resourceServiceOperation->ChangeResourceOwner($paramSet);
        }
        elseif ($operationName == "INHERITPERMISSIONSFROM")
        {
            $actualResult = $this->resourceServiceOperation->InheritPermissionsFrom($paramSet);
        }
        elseif ($operationName == "SETRESOURCE")
        {
            $actualResult = $this->resourceServiceOperation->SetResource($paramSet);
        }
        //End Resource Service
        //Start Drawing Service
        elseif ($operationName =="DESCRIBEDRAWING")
        {
            $actualResult = $this->drawingServiceOperation->DescribeDrawing($paramSet);
        }
        elseif ($operationName == "GETDRAWING")
        {
            $actualResult = $this->drawingServiceOperation->GetDrawing($paramSet);
        }
        elseif ($operationName == "ENUMERATEDRAWINGLAYERS")
        {
            $actualResult = $this->drawingServiceOperation->EnumerateDrawingLayers($paramSet);
        }
        elseif ($operationName == "GETDRAWINGLAYER")
        {
            $actualResult = $this->drawingServiceOperation->GetDrawingLayer($paramSet);
        }
        elseif ($operationName == "GETDRAWINGSECTION")
        {
            $actualResult = $this->drawingServiceOperation->GetDrawingSection($paramSet);
        }
        elseif ($operationName == "GETDRAWINGSECTIONRESOURCE")
        {
            $actualResult = $this->drawingServiceOperation->GetDrawingSectionResource($paramSet);
        }
        elseif ($operationName == "ENUMERATEDRAWINGSECTIONS")
        {
            $actualResult = $this->drawingServiceOperation->EnumerateDrawingSections($paramSet);
        }
        elseif ($operationName == "ENUMERATEDRAWINGSECTIONRESOURCES")
        {
            $actualResult = $this->drawingServiceOperation->EnumerateDrawingSectionResources($paramSet);
        }
        //End Drawing Service
        //Start Feature Service
        elseif ($operationName =="GETFEATUREPROVIDERS")
        {
            $actualResult = $this->featureServiceOperation->GetFeatureProviders($paramSet);
        }
        elseif ($operationName == "GETPROVIDERCAPABILITIES")
        {
            $actualResult = $this->featureServiceOperation->GetProviderCapabilities($paramSet);
        }
        elseif ($operationName == "GETCONNECTIONPROPERTYVALUES")
        {
            $actualResult = $this->featureServiceOperation->GetConnectionPropertyValues($paramSet);
        }
        elseif ($operationName == "DESCRIBEFEATURESCHEMA")
        {
            $actualResult = $this->featureServiceOperation->DescribeFeatureSchema($paramSet);
        }
        elseif ($operationName == "SELECTFEATURES")
        {
            $actualResult = $this->featureServiceOperation->SelectFeatures($paramSet);
        }
        elseif ($operationName == "SELECTAGGREGATES")
        {
            $actualResult = $this->featureServiceOperation->SelectAggregates($paramSet);
        }
        elseif ($operationName == "EXECUTESQLQUERY")
        {
            $actualResult = $this->featureServiceOperation->ExecuteSqlQuery($paramSet);
        }
        elseif ($operationName == "GETSPATIALCONTEXTS")
        {
            $actualResult = $this->featureServiceOperation->GetSpatialContexts($paramSet);
        }
        elseif ($operationName == "GETLONGTRANSACTIONS")
        {
            $actualResult = $this->featureServiceOperation->GetLongTransactions($paramSet);
        }
        elseif ($operationName == "SETLONGTRANSACTION")
        {
            $actualResult = $this->featureServiceOperation->SetLongTransaction($paramSet);
        }
        elseif ($operationName == "TESTCONNECTION")
        {
            $actualResult = $this->featureServiceOperation->TestConnection($paramSet);
        }
        elseif ($operationName == "GETSCHEMAS")
        {
            $actualResult = $this->featureServiceOperation->GetSchemas($paramSet);
        }
        elseif ($operationName == "GETCLASSES")
        {
            $actualResult = $this->featureServiceOperation->GetClasses($paramSet);
        }
        //End Feature Service
        //Start Site Service
        elseif ($operationName=="CREATESESSION")
        {
            $actualResult = $this->siteServiceOperation->CreateSession($paramSet);
        }
        elseif ($operationName=="GETUSERFORSESSION")
        {
            $actualResult = $this->siteServiceOperation->GetUserForSession($paramSet);
        }
        elseif ($operationName=="ENUMERATEUSERS")
        {
            $actualResult = $this->siteServiceOperation->EnumerateUsers($paramSet);
        }
        elseif ($operationName=="ADDUSER")
        {
            $actualResult = $this->siteServiceOperation->AddUser($paramSet);
        }
        elseif ($operationName=="UPDATEUSER")
        {
            $actualResult = $this->siteServiceOperation->UpdateUser($paramSet);
        }
        elseif ($operationName=="DELETEUSERS")
        {
            $actualResult = $this->siteServiceOperation->DeleteUsers($paramSet);
        }
        elseif ($operationName=="ENUMERATEGROUPS")
        {
            $actualResult = $this->siteServiceOperation->EnumerateGroups($paramSet);
        }
        elseif ($operationName=="ENUMERATEGROUPS2")
        {
            $actualResult = $this->siteServiceOperation->EnumerateGroups2($paramSet);
        }
        elseif ($operationName=="ADDGROUP")
        {
            $actualResult = $this->siteServiceOperation->AddGroup($paramSet);
        }
        elseif ($operationName=="UPDATEGROUP")
        {
            $actualResult = $this->siteServiceOperation->UpdateGroup($paramSet);
        }
        elseif ($operationName=="DELETEGROUPS")
        {
            $actualResult = $this->siteServiceOperation->DeleteGroups($paramSet);
        }
        elseif ($operationName=="GRANTGROUPMEMBERSHIPSTOUSERS")
        {
            $actualResult = $this->siteServiceOperation->GrantGroupMembershipsToUsers($paramSet);
        }
        elseif ($operationName=="GRANTROLEMEMBERSHIPSTOUSERS")
        {
            $actualResult = $this->siteServiceOperation->GrantRoleMembershipsToUsers($paramSet);
        }
        elseif ($operationName=="REVOKEGROUPMEMBERSHIPSFROMUSERS")
        {
            $actualResult=$this->siteServiceOperation->RevokeGroupMembershipsFromUsers($paramSet);
        }
        elseif ($operationName=="REVOKEROLEMEMBERSHIPSFROMUSERS")
        {
            $actualResult=$this->siteServiceOperation->RevokeRoleMembershipsFromUsers($paramSet);
        }
        elseif ($operationName=="GRANTROLEMEMBERSHIPSTOGROUPS")
        {
            $actualResult = $this->siteServiceOperation->GrantRoleMembershipsToGroups($paramSet);
        }
        elseif ($operationName=="REVOKEROLEMEMBERSHIPSFROMGROUPS")
        {
            $actualResult = $this->siteServiceOperation->RevokeRoleMembershipsFromGroups($paramSet);
        }
        elseif ($operationName=="ENUMERATEROLES")
        {
            $actualResult = $this->siteServiceOperation->EnumerateRoles($paramSet);
        }
        elseif ($operationName=="ENUMERATEROLES2")
        {
            $actualResult = $this->siteServiceOperation->EnumerateRoles2($paramSet);
        }

        //End Site Service
        //Start Server Admin
        elseif ($operationName=="GETINFO")
        {
            $actualResult = $this->serverAdminOperation->GetProperties($paramSet);
        }
        elseif ($operationName=="GETSTATISTICS")
        {
            $actualResult = $this->serverAdminOperation->GetProperties($paramSet);
        }
        elseif ($operationName=="OFFLINE")
        {
            $actualResult = $this->serverAdminOperation->TakeOffline($paramSet);
        }
        elseif ($operationName=="ONLINE")
        {
            $actualResult = $this->serverAdminOperation->BringOnline($paramSet);
        }
        elseif ($operationName=="GETLOG")
        {
            $actualResult = $this->serverAdminOperation->GetLog($paramSet);
        }
        elseif ($operationName=="GETLOGBYDATE")
        {
            $actualResult = $this->serverAdminOperation->GetLogByDate($paramSet);
        }
        elseif ($operationName=="CLEARLOG")
        {
            $actualResult = $this->serverAdminOperation->ClearLog($paramSet);
        }
        elseif ($operationName=="ENUMERATELOGS")
        {
            $actualResult = $this->serverAdminOperation->EnumerateLogs($paramSet);
        }
        elseif ($operationName=="RENAMELOG")
        {
            $actualResult = $this->serverAdminOperation->RenameLog($paramSet);
        }
        elseif ($operationName=="DELETELOG")
        {
            $actualResult = $this->serverAdminOperation->DeleteLog($paramSet);
        }
        elseif ($operationName=="ENUMERATEPACKAGES")
        {
            $actualResult = $this->serverAdminOperation->EnumeratePackages($paramSet);
        }
        elseif ($operationName=="DELETEPACKAGE")
        {
            $actualResult = $this->serverAdminOperation->DeletePackage($paramSet);
        }
        elseif ($operationName=="LOADPACKAGE")
        {
            $actualResult = $this->serverAdminOperation->LoadPackage($paramSet);
        }
        elseif ($operationName=="GETPACKAGELOG")
        {
            $actualResult = $this->serverAdminOperation->GetPackageLog($paramSet);
        }
        elseif ($operationName=="GETPACKAGESTATUS")
        {
            $actualResult = $this->serverAdminOperation->GetPackageStatus($paramSet);
        }
        //End Server Admin
        //Start MapLayer API
        elseif ($operationName=="GETMAPEXTENT")
        {
            $actualResult = $this->mapLayerOperation->GetMapExtent($paramSet);
        }
        elseif ($operationName=="GETCOORDINATESYSTEM")
        {
            $actualResult = $this->mapLayerOperation->GetCoordinateSystem($paramSet);
        }
        elseif ($operationName=="GETMAPNAME")
        {
            $actualResult = $this->mapLayerOperation->GetName($paramSet);
        }
        elseif ($operationName=="GETLAYERS")
        {
            $actualResult = $this->mapLayerOperation->GetLayers($paramSet);
        }
        elseif ($operationName=="GETGROUPS")
        {
            $actualResult = $this->mapLayerOperation->GetLayerGroups($paramSet);
        }
        elseif ($operationName=="GETLAYERVISIBILITY")
        {
            $actualResult = $this->mapLayerOperation->GetLayerVisibility($paramSet);
        }
        elseif ($operationName=="GETDISPLAYINLEGEND")
        {
            $actualResult = $this->mapLayerOperation->GetDisplayInLegend($paramSet);
        }
        elseif ($operationName=="GETLEGENDLABEL")
        {
            $actualResult = $this->mapLayerOperation->GetLegendLabel($paramSet);
        }
        elseif ($operationName=="GETLAYERDEFINITION")
        {
            $actualResult = $this->mapLayerOperation->GetLayerDefinition($paramSet);
        }
        elseif ($operationName=="ISLAYERVISIBLE")
        {
            $actualResult = $this->mapLayerOperation->IsLayerVisible($paramSet);
        }
        elseif ($operationName=="ADDLAYERGROUP")
        {
            $actualResult = $this->mapLayerOperation->AddGroup($paramSet);
        }
        elseif ($operationName=="ADDLAYER")
        {
            $actualResult = $this->mapLayerOperation->AddLayer($paramSet);
        }
        elseif ($operationName=="HIDEGROUP")
        {
            $actualResult = $this->mapLayerOperation->HideGroup($paramSet);
        }
        elseif ($operationName=="SHOWGROUP")
        {
            $actualResult = $this->mapLayerOperation->ShowGroup($paramSet);
        }
        elseif ($operationName=="GETVIEWCENTER")
        {
            $actualResult = $this->mapLayerOperation->GetViewCenter($paramSet);
        }
        elseif ($operationName=="GETVIEWSCALE")
        {
            $actualResult = $this->mapLayerOperation->GetViewScale($paramSet);
        }
        elseif ($operationName=="GETDATAEXTENT")
        {
            $actualResult = $this->mapLayerOperation->GetDataExtent($paramSet);
        }
        elseif ($operationName=="REMOVEGROUP")
        {
            $actualResult = $this->mapLayerOperation->RemoveGroup($paramSet);
        }
        elseif ($operationName=="LAYEREXISTS")
        {
            $actualResult = $this->mapLayerOperation->LayerExists($paramSet);
        }
        elseif ($operationName == "GETLAYERFEATURESOURCE")
        {
            $actualResult = $this->mapLayerOperation->GetFeatureSourceId($paramSet);
        }
        elseif ($operationName == "GETLAYERFEATURECLASS")
        {
            $actualResult = $this->mapLayerOperation->GetFeatureClassName($paramSet);
        }
        elseif ($operationName == "GETMAP")
        {
            $actualResult = $this->mappingServiceOperation->GetMap($paramSet);
        }
        //End MapLayer API
        //Start WebLayout API
        elseif ($operationName == "WL_GETTITLE")
        {
            $actualResult = $this->webLayoutOperation->GetTitle($paramSet);
        }
        elseif ($operationName == "WL_GETMAPDEFINITION")
        {
            $actualResult = $this->webLayoutOperation->GetMapDefinition($paramSet);
        }
        elseif ($operationName == "WL_GETSCALE")
        {
            $actualResult = $this->webLayoutOperation->GetScale($paramSet);
        }
        elseif ($operationName == "WL_GETCENTER")
        {
            $actualResult = $this->webLayoutOperation->GetCenter($paramSet);
        }
        elseif ($operationName == "WL_SHOWTOOLBAR")
        {
            $actualResult = $this->webLayoutOperation->ShowToolbar($paramSet);
        }
        elseif ($operationName == "WL_SHOWSTATUSBAR")
        {
            $actualResult = $this->webLayoutOperation->ShowStatusbar($paramSet);
        }
        elseif ($operationName == "WL_SHOWTASKPANE")
        {
            $actualResult = $this->webLayoutOperation->ShowTaskpane($paramSet);
        }
        elseif ($operationName == "WL_SHOWTASKBAR")
        {
            $actualResult = $this->webLayoutOperation->ShowTaskbar($paramSet);
        }
        elseif ($operationName == "WL_SHOWLEGEND")
        {
            $actualResult = $this->webLayoutOperation->ShowLegend($paramSet);
        }
        elseif ($operationName == "WL_SHOWPROPERTIES")
        {
            $actualResult = $this->webLayoutOperation->ShowProperties($paramSet);
        }
        elseif ($operationName == "WL_GETTASKPANEWIDTH")
        {
            $actualResult = $this->webLayoutOperation->GetTaskPaneWidth($paramSet);
        }
        elseif ($operationName == "WL_GETINFORMATIONPANEWIDTH")
        {
            $actualResult = $this->webLayoutOperation->GetInformationPaneWidth($paramSet);
        }
        elseif ($operationName == "WL_GETINITIALTASKURL")
        {
            $actualResult = $this->webLayoutOperation->GetInitialTaskUrl($paramSet);
        }
        elseif ($operationName == "WL_SHOWCONTEXTMENU")
        {
            $actualResult = $this->webLayoutOperation->ShowContextMenu($paramSet);
        }
        elseif ($operationName == "WL_TESTUIITEM")
        {
            $actualResult = $this->webLayoutOperation->TestWidget($paramSet);
        }
        elseif ($operationName == "WL_HOMETOOLTIP")
        {
            $actualResult = $this->webLayoutOperation->HomeTooltip($paramSet);
        }
        elseif ($operationName == "WL_HOMEDESCRIPTION")
        {
            $actualResult = $this->webLayoutOperation->HomeDescription($paramSet);
        }
        elseif ($operationName == "WL_BACKTOOLTIP")
        {
            $actualResult = $this->webLayoutOperation->BackTooltip($paramSet);
        }
        elseif ($operationName == "WL_BACKDESCRIPTION")
        {
            $actualResult = $this->webLayoutOperation->BackDescription($paramSet);
        }
        elseif ($operationName == "WL_FORWARDTOOLTIP")
        {
            $actualResult = $this->webLayoutOperation->ForwardTooltip($paramSet);
        }
        elseif ($operationName == "WL_FORWARDDESCRIPTION")
        {
            $actualResult = $this->webLayoutOperation->ForwardDescription($paramSet);
        }
        elseif ($operationName == "WL_TASKSNAME")
        {
            $actualResult = $this->webLayoutOperation->TasksName($paramSet);
        }
        elseif ($operationName == "WL_TASKSTOOLTIP")
        {
            $actualResult = $this->webLayoutOperation->TasksTooltip($paramSet);
        }
        elseif ($operationName == "WL_TASKSDESCRIPTION")
        {
            $actualResult = $this->webLayoutOperation->TasksDescription($paramSet);
        }

        //End WebLayout API
        //Start Wfs
        elseif($operationName == "WfsGetCapabilities")
        {
            $actualResult = $this->wfsOperation->GetCapabilities($paramSet);
        }
        elseif($operationName == "WfsDescribeFeatureType")
        {
            $actualResult = $this->wfsOperation->DescribeFeatureType($paramSet);
        }
        elseif($operationName == "WfsGetFeature_GET")
        {
            $actualResult = $this->wfsOperation->GetFeature($paramSet);
        }
        elseif($operationName == "WfsGetFeature_POST")
        {
            $actualResult = $this->wfsOperation->GetFeature($paramSet, "POST");
        }
        //End Wfs
        //Start Wms
        elseif($operationName == "WmsGetCapabilities")
        {
            $actualResult = $this->wmsOperation->GetCapabilities($paramSet);
        }
        elseif($operationName == "WmsGetMap")
        {
            
            $actualResult = $this->wmsOperation->GetMap($paramSet);
        }
        elseif($operationName == "WmsGetFeatureInfo")
        {
            
            $actualResult = $this->wmsOperation->GetFeatureInfo($paramSet);
        }
        //End Wms
        else
        {
            $actualResult = new Result ("Unknown operation", "text/plain");
        }
    
        return $actualResult;
    }
}
?>

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

#ifndef MGHTTPRESOURCESTRINGS_H_
#define MGHTTPRESOURCESTRINGS_H_

class MG_MAPAGENT_API MgHttpResourceStrings
{
public:

    static const STRING hrhnFrom;
    static const STRING hrhnAccept;
    static const STRING hrhnAccept_Encoding;
    static const STRING hrhnAccept_Language;
    static const STRING hrhnUser_Agent;
    static const STRING hrhnReferer;
    static const STRING hrhnAuthorization;
    static const STRING hrhnCharge_To;
    static const STRING hrhnIf_Modified_Since;
    static const STRING hrhnPragma;
    static const STRING hrhnConnection;
    static const STRING hrhnAcceptCharset;

    // PREDEFINED HTTP RESPONSE HEADER NAMES
    static const STRING hrhnAllowed;
    static const STRING hrhnPublic;
    static const STRING hrhnContent_Length;
    static const STRING hrhnContent_Type;
    static const STRING hrhnContent_Transfer_Encoding;
    static const STRING hrhnContent_Encoding;
    static const STRING hrhnDate;
    static const STRING hrhnExpires;
    static const STRING hrhnLast_Modified;
    static const STRING hrhnMessage_Id;
    static const STRING hrhnVersion;
    static const STRING hrhnDerived_From;
    static const STRING hrhnContent_Language;
    static const STRING hrhnCost;
    static const STRING hrhnLink;
    static const STRING hrhnTitle;

    // PREDEFINED HTTP REQUEST METADATA
    static const STRING hrmServerName;
    static const STRING hrmServerSoftware;
    static const STRING hrmServerProtocol;
    static const STRING hrmRequestMethod;
    static const STRING hrmQueryString;
    static const STRING hrmDocumentRoot;
    static const STRING hrmRemoteAddr;
    static const STRING hrmRemoteHost;
    static const STRING hrmRemotePort;
    static const STRING hrmScriptFileName;
    static const STRING hrmServerAdmin;
    static const STRING hrmServerPort;
    static const STRING hrmServerSignature;
    static const STRING hrmPathTranslated;
    static const STRING hrmScriptName;
    static const STRING hrmRequestUri;
    static const STRING hrmAuthUser;
    static const STRING hrmAuthPwd;
    static const STRING hrmAuthType;

    // PREDEFINED COMMON REQUEST PARAMETERS
    static const STRING reqOperation;
    static const STRING reqVersion;
    static const STRING reqSession;
    static const STRING reqUsername;
    static const STRING reqPassword;
    static const STRING reqLocale;
    static const STRING reqClientAgent;
    static const STRING reqClientIp;
    static const STRING reqResponseFormat;

    // PREDEFINED RESOURCE REQUEST PARAMETERS
    static const STRING reqType;
    static const STRING reqComputeChildren;
    static const STRING reqDepth;
    static const STRING reqResourceId;
    static const STRING reqResourceContent;
    static const STRING reqResourceHeader;
    static const STRING reqSourceResourceId;
    static const STRING reqDestinationResourceId;
    static const STRING reqDataName;
    static const STRING reqDataType;
    static const STRING reqDataLength;
    static const STRING reqData;
    static const STRING reqOldDataName;
    static const STRING reqNewDataName;
    static const STRING reqOverwrite;
    static const STRING reqCascade;
    static const STRING reqPackage;
    static const STRING reqOwner;
    static const STRING reqIncludeDescendants;
    static const STRING reqPath;
    static const STRING reqRecursive;
    static const STRING reqFilter;

    // PREDEFINED FEATURE REQUEST PARAMETERS
    static const STRING reqFeatProperty;
    static const STRING reqFeatConnectionString;
    static const STRING reqFeatResourceId;
    static const STRING reqFeatProvider;
    static const STRING reqFeatSchema;
    static const STRING reqFeatClass;
    static const STRING reqFeatClassNames;
    static const STRING reqFeatFilter;
    static const STRING reqFeatProperties;
    static const STRING reqFeatGeomProperty;
    static const STRING reqFeatSpatialOp;
    static const STRING reqFeatGeometry;
    static const STRING reqFeatSql;
    static const STRING reqFeatActiveOnly;
    static const STRING reqFeatComputedProperties;
    static const STRING reqFeatComputedAliases;

    // PREDEFINED DRAWING REQUEST PARAMETERS
    static const STRING reqDrawingResourceId;
    static const STRING reqDrawingResourceName;
    static const STRING reqDrawingSectionName;
    static const STRING reqDrawingLayerName;

    // PREDEFINED MAPPING REQUEST PARAMETERS
    static const STRING reqMappingMapDefinition;
    static const STRING reqMappingMapName;
    static const STRING reqMappingFeatureSetXml;
    static const STRING reqMappingSeqNo;
    static const STRING reqMappingDwfVersion;
    static const STRING reqMappingEmapVersion;
    static const STRING reqMappingEplotVersion;
    static const STRING reqMappingLayout;
    static const STRING reqMappingWidth;
    static const STRING reqMappingHeight;
    static const STRING reqMappingUnits;
    static const STRING reqMappingLayoutTitle;
    static const STRING reqMappingLayoutUnits;
    static const STRING reqMappingMargins;
    static const STRING reqMappingLayerDefinition;
    static const STRING reqMappingScale;
    static const STRING reqMappingFormat;
    static const STRING reqMappingType;
    static const STRING reqMappingThemeCategory;

    // PREDEFINED RENDERING REQUEST PARAMETERS
    static const STRING reqRenderingMapDefinition;
    static const STRING reqRenderingMapName;
    static const STRING reqRenderingFormat;
    static const STRING reqRenderingWidth;
    static const STRING reqRenderingHeight;
    static const STRING reqRenderingLayerName;
    static const STRING reqRenderingLayerNames;
    static const STRING reqRenderingMaxFeatures;
    static const STRING reqRenderingSelection;
    static const STRING reqRenderingSelectionVariant;
    static const STRING reqRenderingGeometry;
    static const STRING reqRenderingFeatureFilter;
    static const STRING reqRenderingBaseMapLayerGroupName;
    static const STRING reqRenderingTileColumn;
    static const STRING reqRenderingTileRow;
    static const STRING reqRenderingPersist;
    static const STRING reqRenderingScaleIndex;
    static const STRING reqRenderingLayerAttributeFilter;
    static const STRING reqRenderingKeepSelection;
    static const STRING reqRenderingClip;
    static const STRING reqRenderingBehavior;
    static const STRING reqRenderingSelectionColor;

    // PREDEFINED TILE REQUEST PARAMETERS
    static const STRING reqTileMapDefinition;

    // PREDEFINED KML REQUEST PARAMETERS
    static const STRING reqKmlMapDefinition;
    static const STRING reqKmlLayerDefinition;
    static const STRING reqKmlBoundingBox;
    static const STRING reqKmlFormat;
    static const STRING reqKmlWidth;
    static const STRING reqKmlHeight;
    static const STRING reqKmlDpi;
    static const STRING reqKmlDrawOrder;

    // PREDEFINED WEB MAP SERVICE REQUEST PARAMETERS
    static const STRING reqWmsRequest;
    static const STRING reqWmsService;
    static const STRING reqWmsVersion;
    static const STRING reqWmsLayers;
    static const STRING reqWmsStyles;
    static const STRING reqWmsCrs;
    static const STRING reqWmsSrs;
    static const STRING reqWmsBbox;
    static const STRING reqWmsWidth;
    static const STRING reqWmsHeight;
    static const STRING reqWmsFormat;
    static const STRING reqWmsQueryLayers;
    static const STRING reqWmsInfoFormat;
    static const STRING reqWmsICoord;
    static const STRING reqWmsJCoord;
    static const STRING reqWmsXCoord;
    static const STRING reqWmsYCoord;
    static const STRING reqWmsTransparent;
    static const STRING reqWmsBackgroundColor;
    static const STRING reqWmsFeatureCount;

    // PREDEFINED WEB FEATURE SERVICE REQUEST PARAMETERS
    static const STRING reqWfsVersion;
    static const STRING reqWfsPropertyName;
    static const STRING reqWfsMaxFeatures;
    static const STRING reqWfsSrsName;
    static const STRING reqWfsTypeName;
    static const STRING reqWfsFeatureId;
    static const STRING reqWfsFilter;
    static const STRING reqWfsBbox;
    static const STRING reqWfsOutputFormat;
    static const STRING reqWfsSortBy;

    // PREDEFINED WEB APPLICATION REQUEST PARAMETERS
    static const STRING reqFormat;
    static const STRING reqRefresh;

    // PREDEFINED OPERATIONS

    // Resource service
    static const STRING opEnumerateRepositories;
    static const STRING opCreateRepository;
    static const STRING opDeleteRepository;
    static const STRING opUpdateRepository;
    static const STRING opGetRepositoryContent;
    static const STRING opGetRepositoryHeader;
    static const STRING opApplyResourcePackage;

    static const STRING opResourceExists;
    static const STRING opEnumerateResources;
    static const STRING opSetResource;
    static const STRING opDeleteResource;
    static const STRING opMoveResource;
    static const STRING opCopyResource;
    static const STRING opGetResourceContent;
    static const STRING opGetResourceHeader;
    static const STRING opEnumerateResourceReferences;
    static const STRING opChangeResourceOwner;
    static const STRING opInheritPermissionsFrom;

    static const STRING opEnumerateResourceData;
    static const STRING opSetResourceData;
    static const STRING opDeleteResourceData;
    static const STRING opRenameResourceData;
    static const STRING opGetResourceData;

    static const STRING opEnumerateUnmanagedData;

    // Feature services
    static const STRING opGetFeatureProviders;
    static const STRING opGetConnectionPropertyValues;
    static const STRING opTestConnection;
    static const STRING opGetCapabilities;
    static const STRING opDescribeSchema;
    static const STRING opSelectFeatures;
    static const STRING opSelectFeaturesSpatially;
    static const STRING opExecuteSqlQuery;
    static const STRING opGetSpatialContexts;
    static const STRING opGetLongTransactions;
    static const STRING opEnumerateDataStores;
    static const STRING opGetSchemaMapping;
    static const STRING opGetSchemas;
    static const STRING opGetClasses;
    static const STRING opGetClassDefinition;
    static const STRING opGetIdentityProperties;
    static const STRING opGetFdoCacheInfo;

    // Drawing services
    static const STRING opGetDrawingSectionResource;
    static const STRING opGetDrawingSection;
    static const STRING opGetDrawingLayer;
    static const STRING opGetDrawing;
    static const STRING opEnumerateDrawingLayers;
    static const STRING opDescribeDrawing;
    static const STRING opEnumerateDrawingSections;
    static const STRING opEnumerateDrawingSectionResources;
    static const STRING opGetDrawingCoordinateSpace;

    // Mapping services
    static const STRING opGetMap;
    static const STRING opGetMapUpdate;
    static const STRING opGetPlot;
    static const STRING opGetLegendImage;
    static const STRING opGetFeatureSetEnvelope;

    // Rendering services
    static const STRING opGetDynamicMapOverlayImage;
    static const STRING opGetMapImage;
    static const STRING opGetMapLegendImage;
    static const STRING opGetVisibleMapExtent;
    static const STRING opQueryMapFeatures;

    // Tile services
    static const STRING opGetTileImage;
    static const STRING opClearTileCache;
    static const STRING opGetDefaultTileSizeX;
    static const STRING opGetDefaultTileSizeY;

    // Kml services
    static const STRING opGetMapKml;
    static const STRING opGetLayerKml;
    static const STRING opGetFeaturesKml;

    // WMS Operation Requests
    static const STRING opWmsGetCapabilities;
    static const STRING opWmsGetMap;
    static const STRING opWmsCapabilities;
    static const STRING opWmsMap;
    static const STRING opWmsGetFeatureInfo;

    // WFS Operation Requests
    static const STRING opWfsGetCapabilities;
    static const STRING opWfsGetFeature;
    static const STRING opWfsDescribeFeatureType;

    // Site service
    static const STRING opEnumerateUsers;
    static const STRING opEnumerateGroups;
    static const STRING opGetSiteVersion;
    static const STRING opGetSiteInfo;
    static const STRING opGetSiteStatus;

    // Other requests
    static const STRING opCreateSession;
    static const STRING opGetSessionTimeout;
    static const STRING opGetDwfViewerPage;
    static const STRING opGetHtmlViewerPage;

    // Coordinate System Operation Requests
    static const STRING opCsConvertWktToCoordinateSystemCode;
    static const STRING opCsConvertCoordinateSystemCodeToWkt;
    static const STRING opCsConvertEpsgCodeToWkt;
    static const STRING opCsConvertWktToEpsgCode;
    static const STRING opCsEnumerateCategories;
    static const STRING opCsEnumerateCoordinateSystems;
    static const STRING opCsGetBaseLibrary;
    static const STRING opCsIsValid;

    // Web Applications Operations
    static const STRING opEnumerateApplicationTemplates;
    static const STRING opEnumerateApplicationWidgets;
    static const STRING opEnumerateApplicationContainers;


    // Coordinate System Request Parameters
    static const STRING reqCsWkt;
    static const STRING reqCsCode;
    static const STRING reqCsCategory;

    // PREDEFINED SITE REQUEST PARAMETERS
    static const STRING reqGroup;
};

#endif

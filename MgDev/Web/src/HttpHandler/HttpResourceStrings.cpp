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

#include "HttpHandler.h"

// IMPORTANT NOTE: All operation names must be uppercase

// HTTP Header Strings
const STRING MgHttpResourceStrings::hrhnFrom = L"From";
const STRING MgHttpResourceStrings::hrhnAccept = L"Accept";
const STRING MgHttpResourceStrings::hrhnAccept_Encoding = L"Accept-Encoding";
const STRING MgHttpResourceStrings::hrhnAccept_Language = L"Accept-Language";
const STRING MgHttpResourceStrings::hrhnUser_Agent = L"User-Agent";
const STRING MgHttpResourceStrings::hrhnReferer = L"Referer";
const STRING MgHttpResourceStrings::hrhnAuthorization = L"Authorization";
const STRING MgHttpResourceStrings::hrhnCharge_To = L"Charge-To";
const STRING MgHttpResourceStrings::hrhnIf_Modified_Since = L"If-Modified-Since";
const STRING MgHttpResourceStrings::hrhnPragma = L"Pragma";
const STRING MgHttpResourceStrings::hrhnConnection = L"Connection";
const STRING MgHttpResourceStrings::hrhnAcceptCharset = L"Accept-Charset";

const STRING MgHttpResourceStrings::hrhnAllowed = L"Allowed";
const STRING MgHttpResourceStrings::hrhnPublic = L"Public";
const STRING MgHttpResourceStrings::hrhnContent_Length = L"Content-Length";
const STRING MgHttpResourceStrings::hrhnContent_Type = L"Content-Type";
const STRING MgHttpResourceStrings::hrhnContent_Transfer_Encoding = L"Content-Transfer-Encoding";
const STRING MgHttpResourceStrings::hrhnContent_Encoding = L"Content-Encoding";
const STRING MgHttpResourceStrings::hrhnDate = L"Date";
const STRING MgHttpResourceStrings::hrhnExpires = L"Expires";
const STRING MgHttpResourceStrings::hrhnLast_Modified = L"Last-Modified";
const STRING MgHttpResourceStrings::hrhnMessage_Id = L"Message-Id";
const STRING MgHttpResourceStrings::hrhnVersion = L"Version";
const STRING MgHttpResourceStrings::hrhnDerived_From = L"Derived-From";
const STRING MgHttpResourceStrings::hrhnContent_Language = L"Content-Language";
const STRING MgHttpResourceStrings::hrhnCost = L"Cost";
const STRING MgHttpResourceStrings::hrhnLink = L"Link";
const STRING MgHttpResourceStrings::hrhnTitle = L"Title";

const STRING MgHttpResourceStrings::hrmServerName = L"ServerName";
const STRING MgHttpResourceStrings::hrmServerSoftware = L"ServerSoftware";
const STRING MgHttpResourceStrings::hrmServerProtocol = L"ServerProtocol";
const STRING MgHttpResourceStrings::hrmRequestMethod = L"RequestMethod";
const STRING MgHttpResourceStrings::hrmQueryString = L"QueryString";
const STRING MgHttpResourceStrings::hrmDocumentRoot = L"DocumentRoot";
const STRING MgHttpResourceStrings::hrmRemoteAddr = L"RemoteAddr";
const STRING MgHttpResourceStrings::hrmRemoteHost = L"RemoteHost";
const STRING MgHttpResourceStrings::hrmRemotePort = L"RemotePort";
const STRING MgHttpResourceStrings::hrmScriptFileName = L"ScriptFileName";
const STRING MgHttpResourceStrings::hrmServerAdmin = L"ServerAdmin";
const STRING MgHttpResourceStrings::hrmServerPort = L"ServerPort";
const STRING MgHttpResourceStrings::hrmServerSignature = L"ServerSignature";
const STRING MgHttpResourceStrings::hrmPathTranslated = L"PathTranslated";
const STRING MgHttpResourceStrings::hrmScriptName = L"ScriptName";
const STRING MgHttpResourceStrings::hrmRequestUri = L"RequestUri";
const STRING MgHttpResourceStrings::hrmAuthUser = L"AuthUser";
const STRING MgHttpResourceStrings::hrmAuthPwd = L"AuthPwd";
const STRING MgHttpResourceStrings::hrmAuthType = L"AuthType";

// Predefined Common Request Parameters
const STRING MgHttpResourceStrings::reqOperation = L"OPERATION";
const STRING MgHttpResourceStrings::reqVersion = L"VERSION";
const STRING MgHttpResourceStrings::reqSession = L"SESSION";
const STRING MgHttpResourceStrings::reqUsername = L"USERNAME";
const STRING MgHttpResourceStrings::reqPassword = L"PASSWORD";
const STRING MgHttpResourceStrings::reqLocale = L"LOCALE";
const STRING MgHttpResourceStrings::reqClientAgent = L"CLIENTAGENT";
const STRING MgHttpResourceStrings::reqClientIp = L"CLIENTIP";
const STRING MgHttpResourceStrings::reqResponseFormat = L"FORMAT";

// Predefined Resource Service Request Parameters
const STRING MgHttpResourceStrings::reqType = L"TYPE";
const STRING MgHttpResourceStrings::reqComputeChildren = L"COMPUTECHILDREN";
const STRING MgHttpResourceStrings::reqDepth = L"DEPTH";
const STRING MgHttpResourceStrings::reqResourceId = L"RESOURCEID";
const STRING MgHttpResourceStrings::reqResourceContent = L"CONTENT";
const STRING MgHttpResourceStrings::reqResourceHeader = L"HEADER";
const STRING MgHttpResourceStrings::reqSourceResourceId = L"SOURCE";
const STRING MgHttpResourceStrings::reqDestinationResourceId = L"DESTINATION";
const STRING MgHttpResourceStrings::reqDataName = L"DATANAME";
const STRING MgHttpResourceStrings::reqDataType = L"DATATYPE";
const STRING MgHttpResourceStrings::reqDataLength = L"DATALENGTH";
const STRING MgHttpResourceStrings::reqData = L"DATA";
const STRING MgHttpResourceStrings::reqOldDataName = L"OLDDATANAME";
const STRING MgHttpResourceStrings::reqNewDataName = L"NEWDATANAME";
const STRING MgHttpResourceStrings::reqOverwrite = L"OVERWRITE";
const STRING MgHttpResourceStrings::reqPackage = L"PACKAGE";
const STRING MgHttpResourceStrings::reqOwner = L"OWNER";
const STRING MgHttpResourceStrings::reqIncludeDescendants = L"INCLUDEDESCENDANTS";
const STRING MgHttpResourceStrings::reqPath = L"PATH";
const STRING MgHttpResourceStrings::reqRecursive = L"RECURSIVE";
const STRING MgHttpResourceStrings::reqFilter = L"FILTER";

// Resource Service Operation Requests
const STRING MgHttpResourceStrings::opEnumerateRepositories = L"ENUMERATEREPOSITORIES";
const STRING MgHttpResourceStrings::opCreateRepository = L"CREATEREPOSITORY";
const STRING MgHttpResourceStrings::opDeleteRepository = L"DELETEREPOSITORY";
const STRING MgHttpResourceStrings::opUpdateRepository = L"UPDATEREPOSITORY";
const STRING MgHttpResourceStrings::opGetRepositoryContent = L"GETREPOSITORYCONTENT";
const STRING MgHttpResourceStrings::opGetRepositoryHeader = L"GETREPOSITORYHEADER";
const STRING MgHttpResourceStrings::opApplyResourcePackage = L"APPLYRESOURCEPACKAGE";

const STRING MgHttpResourceStrings::opResourceExists = L"RESOURCEEXISTS";
const STRING MgHttpResourceStrings::opEnumerateResources = L"ENUMERATERESOURCES";
const STRING MgHttpResourceStrings::opSetResource = L"SETRESOURCE";
const STRING MgHttpResourceStrings::opDeleteResource = L"DELETERESOURCE";
const STRING MgHttpResourceStrings::opMoveResource = L"MOVERESOURCE";
const STRING MgHttpResourceStrings::opCopyResource = L"COPYRESOURCE";
const STRING MgHttpResourceStrings::opGetResourceContent = L"GETRESOURCECONTENT";
const STRING MgHttpResourceStrings::opGetResourceHeader = L"GETRESOURCEHEADER";
const STRING MgHttpResourceStrings::opEnumerateResourceReferences = L"ENUMERATERESOURCEREFERENCES";
const STRING MgHttpResourceStrings::opChangeResourceOwner = L"CHANGERESOURCEOWNER";
const STRING MgHttpResourceStrings::opInheritPermissionsFrom = L"INHERITPERMISSIONSFROM";

const STRING MgHttpResourceStrings::opEnumerateResourceData = L"ENUMERATERESOURCEDATA";
const STRING MgHttpResourceStrings::opSetResourceData = L"SETRESOURCEDATA";
const STRING MgHttpResourceStrings::opDeleteResourceData = L"DELETERESOURCEDATA";
const STRING MgHttpResourceStrings::opRenameResourceData = L"RENAMERESOURCEDATA";
const STRING MgHttpResourceStrings::opGetResourceData = L"GETRESOURCEDATA";

const STRING MgHttpResourceStrings::opEnumerateUnmanagedData = L"ENUMERATEUNMANAGEDDATA";

// Predefined Feature Service Request Parameters
const STRING MgHttpResourceStrings::reqFeatProperty = L"PROPERTY";
const STRING MgHttpResourceStrings::reqFeatConnectionString = L"CONNECTIONSTRING";
const STRING MgHttpResourceStrings::reqFeatResourceId = L"RESOURCEID";
const STRING MgHttpResourceStrings::reqFeatProvider = L"PROVIDER";
const STRING MgHttpResourceStrings::reqFeatSchema = L"SCHEMA";
const STRING MgHttpResourceStrings::reqFeatClass = L"CLASSNAME";
const STRING MgHttpResourceStrings::reqFeatClassNames = L"CLASSNAMES";
const STRING MgHttpResourceStrings::reqFeatFilter = L"FILTER";
const STRING MgHttpResourceStrings::reqFeatGeomProperty = L"GEOMPROPERTY";
const STRING MgHttpResourceStrings::reqFeatSpatialOp = L"SPATIALOP";
const STRING MgHttpResourceStrings::reqFeatGeometry = L"GEOMETRY";
const STRING MgHttpResourceStrings::reqFeatProperties = L"PROPERTIES";
const STRING MgHttpResourceStrings::reqFeatSql = L"SQL";
const STRING MgHttpResourceStrings::reqFeatActiveOnly = L"ACTIVEONLY";
const STRING MgHttpResourceStrings::reqFeatComputedProperties = L"COMPUTED_PROPERTIES";
const STRING MgHttpResourceStrings::reqFeatComputedAliases = L"COMPUTED_ALIASES";

// Feature Service Operation Requests
const STRING MgHttpResourceStrings::opGetFeatureProviders = L"GETFEATUREPROVIDERS";
const STRING MgHttpResourceStrings::opGetConnectionPropertyValues = L"GETCONNECTIONPROPERTYVALUES";
const STRING MgHttpResourceStrings::opTestConnection = L"TESTCONNECTION";
const STRING MgHttpResourceStrings::opGetCapabilities = L"GETPROVIDERCAPABILITIES";
const STRING MgHttpResourceStrings::opDescribeSchema = L"DESCRIBEFEATURESCHEMA";
const STRING MgHttpResourceStrings::opSelectFeatures = L"SELECTFEATURES";
// const STRING MgHttpResourceStrings::opSelectFeaturesSpatially = L"SELECTFEATURESSPATIALLY";
const STRING MgHttpResourceStrings::opSelectFeaturesSpatially = L"SELECTAGGREGATES";
const STRING MgHttpResourceStrings::opExecuteSqlQuery = L"EXECUTESQLQUERY";
const STRING MgHttpResourceStrings::opGetSpatialContexts = L"GETSPATIALCONTEXTS";
const STRING MgHttpResourceStrings::opGetLongTransactions = L"GETLONGTRANSACTIONS";
const STRING MgHttpResourceStrings::opEnumerateDataStores = L"ENUMERATEDATASTORES";
const STRING MgHttpResourceStrings::opGetSchemaMapping = L"GETSCHEMAMAPPING";
const STRING MgHttpResourceStrings::opGetSchemas = L"GETSCHEMAS";
const STRING MgHttpResourceStrings::opGetClasses = L"GETCLASSES";
const STRING MgHttpResourceStrings::opGetClassDefinition = L"GETCLASSDEFINITION";
const STRING MgHttpResourceStrings::opGetIdentityProperties = L"GETIDENTITYPROPERTIES";
const STRING MgHttpResourceStrings::opGetFdoCacheInfo = L"GETFDOCACHEINFO";

// Predefined Drawing Service Request Parameters
const STRING MgHttpResourceStrings::reqDrawingResourceId = L"RESOURCEID";
const STRING MgHttpResourceStrings::reqDrawingResourceName = L"RESOURCENAME";
const STRING MgHttpResourceStrings::reqDrawingSectionName = L"SECTION";
const STRING MgHttpResourceStrings::reqDrawingLayerName = L"LAYER";

// Drawing Service Operation Requests
const STRING MgHttpResourceStrings::opGetDrawingSectionResource = L"GETDRAWINGSECTIONRESOURCE";
const STRING MgHttpResourceStrings::opGetDrawingSection = L"GETDRAWINGSECTION";
const STRING MgHttpResourceStrings::opGetDrawingLayer = L"GETDRAWINGLAYER";
const STRING MgHttpResourceStrings::opGetDrawing = L"GETDRAWING";
const STRING MgHttpResourceStrings::opEnumerateDrawingLayers = L"ENUMERATEDRAWINGLAYERS";
const STRING MgHttpResourceStrings::opDescribeDrawing = L"DESCRIBEDRAWING";
const STRING MgHttpResourceStrings::opEnumerateDrawingSections = L"ENUMERATEDRAWINGSECTIONS";
const STRING MgHttpResourceStrings::opEnumerateDrawingSectionResources = L"ENUMERATEDRAWINGSECTIONRESOURCES";
const STRING MgHttpResourceStrings::opGetDrawingCoordinateSpace = L"GETDRAWINGCOORDINATESPACE";

// Mapping Service Operation Requests
const STRING MgHttpResourceStrings::opGetMap = L"GETMAP";
const STRING MgHttpResourceStrings::opGetMapUpdate = L"GETMAPUPDATE";
const STRING MgHttpResourceStrings::opGetPlot = L"GETPLOT";
const STRING MgHttpResourceStrings::opGetLegendImage = L"GETLEGENDIMAGE";
const STRING MgHttpResourceStrings::opGetFeatureSetEnvelope = L"GETFEATURESETENVELOPE";

// Predefined Mapping Service Request Parameters
const STRING MgHttpResourceStrings::reqMappingMapDefinition = L"MAPDEFINITION";
const STRING MgHttpResourceStrings::reqMappingMapName = L"MAPNAME";
const STRING MgHttpResourceStrings::reqMappingFeatureSetXml = L"FEATURESET";
const STRING MgHttpResourceStrings::reqMappingSeqNo = L"SEQNO";
const STRING MgHttpResourceStrings::reqMappingDwfVersion = L"DWFVERSION";
const STRING MgHttpResourceStrings::reqMappingEmapVersion = L"EMAPVERSION";
const STRING MgHttpResourceStrings::reqMappingEplotVersion = L"EPLOTVERSION";
const STRING MgHttpResourceStrings::reqMappingLayout = L"LAYOUT";
const STRING MgHttpResourceStrings::reqMappingWidth = L"WIDTH";
const STRING MgHttpResourceStrings::reqMappingHeight = L"HEIGHT";
const STRING MgHttpResourceStrings::reqMappingUnits = L"UNITS";
const STRING MgHttpResourceStrings::reqMappingLayoutTitle = L"LAYOUTTITLE";
const STRING MgHttpResourceStrings::reqMappingLayoutUnits = L"LAYOUTUNITTYPE";
const STRING MgHttpResourceStrings::reqMappingMargins = L"MARGINS";
const STRING MgHttpResourceStrings::reqMappingLayerDefinition = L"LAYERDEFINITION";
const STRING MgHttpResourceStrings::reqMappingScale = L"SCALE";
const STRING MgHttpResourceStrings::reqMappingFormat = L"FORMAT";
const STRING MgHttpResourceStrings::reqMappingType = L"TYPE";
const STRING MgHttpResourceStrings::reqMappingThemeCategory = L"THEMECATEGORY";

// Predefined Rendering Service Request Parameters
const STRING MgHttpResourceStrings::reqRenderingMapDefinition = L"MAPDEFINITION";
const STRING MgHttpResourceStrings::reqRenderingMapName = L"MAPNAME";
const STRING MgHttpResourceStrings::reqRenderingLayerName = L"LAYERNAME";
const STRING MgHttpResourceStrings::reqRenderingLayerNames = L"LAYERNAMES";
const STRING MgHttpResourceStrings::reqRenderingMaxFeatures = L"MAXFEATURES";
const STRING MgHttpResourceStrings::reqRenderingFormat = L"FORMAT";
const STRING MgHttpResourceStrings::reqRenderingWidth = L"WIDTH";
const STRING MgHttpResourceStrings::reqRenderingHeight = L"HEIGHT";
const STRING MgHttpResourceStrings::reqRenderingSelection = L"SELECTION";
const STRING MgHttpResourceStrings::reqRenderingSelectionVariant = L"SELECTIONVARIANT";
const STRING MgHttpResourceStrings::reqRenderingGeometry = L"GEOMETRY";
const STRING MgHttpResourceStrings::reqRenderingFeatureFilter = L"FEATUREFILTER";
const STRING MgHttpResourceStrings::reqRenderingBaseMapLayerGroupName = L"BASEMAPLAYERGROUPNAME";
const STRING MgHttpResourceStrings::reqRenderingTileColumn = L"TILECOL";
const STRING MgHttpResourceStrings::reqRenderingTileRow = L"TILEROW";
const STRING MgHttpResourceStrings::reqRenderingPersist = L"PERSIST";
const STRING MgHttpResourceStrings::reqRenderingScaleIndex = L"SCALEINDEX";
const STRING MgHttpResourceStrings::reqRenderingLayerAttributeFilter = L"LAYERATTRIBUTEFILTER";
const STRING MgHttpResourceStrings::reqRenderingKeepSelection = L"KEEPSELECTION";
const STRING MgHttpResourceStrings::reqRenderingClip = L"CLIP";
const STRING MgHttpResourceStrings::reqRenderingBehavior = L"BEHAVIOR";
const STRING MgHttpResourceStrings::reqRenderingSelectionColor = L"SELECTIONCOLOR";

// Rendering Service Operation Requests
const STRING MgHttpResourceStrings::opGetDynamicMapOverlayImage = L"GETDYNAMICMAPOVERLAYIMAGE";
const STRING MgHttpResourceStrings::opGetMapImage = L"GETMAPIMAGE";
const STRING MgHttpResourceStrings::opGetMapLegendImage = L"GETMAPLEGENDIMAGE";
const STRING MgHttpResourceStrings::opGetVisibleMapExtent = L"GETVISIBLEMAPEXTENT";
const STRING MgHttpResourceStrings::opQueryMapFeatures = L"QUERYMAPFEATURES";

// Predefined Tile Service Request Parameters
const STRING MgHttpResourceStrings::reqTileMapDefinition = L"MAPDEFINITION";

// Tile Service Operation Requests
const STRING MgHttpResourceStrings::opGetTileImage = L"GETTILEIMAGE";
const STRING MgHttpResourceStrings::opClearTileCache = L"CLEARTILECACHE";

// Predefined Kml Service Request Parameters
const STRING MgHttpResourceStrings::reqKmlMapDefinition = L"MAPDEFINITION";
const STRING MgHttpResourceStrings::reqKmlLayerDefinition = L"LAYERDEFINITION";
const STRING MgHttpResourceStrings::reqKmlBoundingBox = L"BBOX";
const STRING MgHttpResourceStrings::reqKmlFormat = L"FORMAT";
const STRING MgHttpResourceStrings::reqKmlWidth = L"WIDTH";
const STRING MgHttpResourceStrings::reqKmlHeight = L"HEIGHT";
const STRING MgHttpResourceStrings::reqKmlDpi = L"DPI";
const STRING MgHttpResourceStrings::reqKmlDrawOrder = L"DRAWORDER";

// Kml Service Operation Requests
const STRING MgHttpResourceStrings::opGetMapKml = L"GETMAPKML";
const STRING MgHttpResourceStrings::opGetLayerKml = L"GETLAYERKML";
const STRING MgHttpResourceStrings::opGetFeaturesKml = L"GETFEATURESKML";

// Predefined Web Map Service Request Parameters
const STRING MgHttpResourceStrings::reqWmsRequest = L"REQUEST";
const STRING MgHttpResourceStrings::reqWmsService = L"SERVICE";
const STRING MgHttpResourceStrings::reqWmsVersion = L"VERSION";
const STRING MgHttpResourceStrings::reqWmsLayers = L"LAYERS";
const STRING MgHttpResourceStrings::reqWmsStyles = L"STYLES";
const STRING MgHttpResourceStrings::reqWmsCrs = L"CRS";
const STRING MgHttpResourceStrings::reqWmsSrs = L"SRS";
const STRING MgHttpResourceStrings::reqWmsBbox = L"BBOX";
const STRING MgHttpResourceStrings::reqWmsWidth = L"WIDTH";
const STRING MgHttpResourceStrings::reqWmsHeight = L"HEIGHT";
const STRING MgHttpResourceStrings::reqWmsFormat = L"FORMAT";
const STRING MgHttpResourceStrings::reqWmsQueryLayers = L"QUERY_LAYERS";
const STRING MgHttpResourceStrings::reqWmsInfoFormat = L"INFO_FORMAT";
const STRING MgHttpResourceStrings::reqWmsICoord = L"I";
const STRING MgHttpResourceStrings::reqWmsJCoord = L"J";
const STRING MgHttpResourceStrings::reqWmsXCoord = L"X";
const STRING MgHttpResourceStrings::reqWmsYCoord = L"Y";
const STRING MgHttpResourceStrings::reqWmsTransparent = L"TRANSPARENT";
const STRING MgHttpResourceStrings::reqWmsBackgroundColor = L"BGCOLOR";
const STRING MgHttpResourceStrings::reqWmsFeatureCount = L"FEATURE_COUNT";

// WMS Operation Requests
const STRING MgHttpResourceStrings::opWmsGetCapabilities = L"WMS.GETCAPABILITIES";
const STRING MgHttpResourceStrings::opWmsGetMap = L"WMS.GETMAP";
const STRING MgHttpResourceStrings::opWmsCapabilities = L"WMS.CAPABILITIES";
const STRING MgHttpResourceStrings::opWmsMap = L"WMS.MAP";
const STRING MgHttpResourceStrings::opWmsGetFeatureInfo = L"WMS.GETFEATUREINFO";

// Predefined Web Feature Service Request Parameters
const STRING MgHttpResourceStrings::reqWfsPropertyName = L"PROPERTYNAME";
const STRING MgHttpResourceStrings::reqWfsMaxFeatures = L"MAXFEATURES";
const STRING MgHttpResourceStrings::reqWfsSrsName = L"SRSNAME";
const STRING MgHttpResourceStrings::reqWfsTypeName = L"TYPENAME";
const STRING MgHttpResourceStrings::reqWfsFeatureId = L"FEATUREID";
const STRING MgHttpResourceStrings::reqWfsFilter = L"FILTER";
const STRING MgHttpResourceStrings::reqWfsBbox = L"BBOX";

// Web Application Parameters
const STRING MgHttpResourceStrings::reqFormat = L"FORMAT";
const STRING MgHttpResourceStrings::reqRefresh = L"REFRESH";

// WFS Operation Requests
const STRING MgHttpResourceStrings::opWfsGetCapabilities = L"WFS.GETCAPABILITIES";
const STRING MgHttpResourceStrings::opWfsGetFeature = L"WFS.GETFEATURE";
const STRING MgHttpResourceStrings::opWfsDescribeFeatureType = L"WFS.DESCRIBEFEATURETYPE";

// Site Service Operation Requests
const STRING MgHttpResourceStrings::opEnumerateUsers = L"ENUMERATEUSERS";
const STRING MgHttpResourceStrings::opEnumerateGroups = L"ENUMERATEGROUPS";
const STRING MgHttpResourceStrings::opGetSiteVersion = L"GETSITEVERSION";
const STRING MgHttpResourceStrings::opGetSiteInfo = L"GETSITEINFO";

// Other requests
const STRING MgHttpResourceStrings::opCreateSession = L"CREATESESSION";
const STRING MgHttpResourceStrings::opGetSessionTimeout = L"GETSESSIONTIMEOUT";
const STRING MgHttpResourceStrings::opGetDwfViewerPage = L"GETDWFVIEWERPAGE";
const STRING MgHttpResourceStrings::opGetHtmlViewerPage = L"GETHTMLVIEWERPAGE";

// Coordinate System Operation Requests
const STRING MgHttpResourceStrings::opCsConvertWktToCoordinateSystemCode = L"CS.CONVERTWKTTOCOORDINATESYSTEMCODE";
const STRING MgHttpResourceStrings::opCsConvertCoordinateSystemCodeToWkt = L"CS.CONVERTCOORDINATESYSTEMCODETOWKT";
const STRING MgHttpResourceStrings::opCsConvertEpsgCodeToWkt = L"CS.CONVERTEPSGCODETOWKT";
const STRING MgHttpResourceStrings::opCsConvertWktToEpsgCode = L"CS.CONVERTWKTTOEPSGCODE";
const STRING MgHttpResourceStrings::opCsEnumerateCategories = L"CS.ENUMERATECATEGORIES";
const STRING MgHttpResourceStrings::opCsEnumerateCoordinateSystems = L"CS.ENUMERATECOORDINATESYSTEMS";
const STRING MgHttpResourceStrings::opCsGetBaseLibrary = L"CS.GETBASELIBRARY";
const STRING MgHttpResourceStrings::opCsIsValid = L"CS.ISVALID";

// Web Application Operations
const STRING MgHttpResourceStrings::opEnumerateApplicationTemplates = L"ENUMERATEAPPLICATIONTEMPLATES";
const STRING MgHttpResourceStrings::opEnumerateApplicationWidgets = L"ENUMERATEAPPLICATIONWIDGETS";
const STRING MgHttpResourceStrings::opEnumerateApplicationContainers = L"ENUMERATEAPPLICATIONCONTAINERS";

// Coordinate System Request Parameters
const STRING MgHttpResourceStrings::reqCsWkt = L"CSWKT";
const STRING MgHttpResourceStrings::reqCsCode = L"CSCODE";
const STRING MgHttpResourceStrings::reqCsCategory = L"CSCATEGORY";

// Predefined Site Service Request Parameters
const STRING MgHttpResourceStrings::reqGroup = L"GROUP";

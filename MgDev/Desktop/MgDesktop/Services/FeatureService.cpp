#include "FSDSAX2Parser.h"
#include "FeatureService.h"
#include "ResourceService.h"
#include "Services/Feature/CreateFeatureSource.h"
#include "Services/Feature/FeatureConnection.h"
#include "Services/Feature/FeatureDefs.h"
#include "Services/Feature/FeatureServiceCache.h"
#include "Services/Feature/FdoForcedOneToOneFeatureReader.h"
#include "Services/Feature/RdbmsFeatureSourceParams.h"
#include "ServiceFactory.h"
#include "Fdo.h"
#include "FdoExpressionEngine.h"
#include "FdoExpressionEngineCopyFilter.h"
#include "PlatformBase.h"
#include "Services/Feature/FeatureUtil.h"
#include "FeatureReader.h"
#include "ScrollableFeatureReader.h"
#include "DataReader.h"
#include "SqlReader.h"

#include "Services/Feature/ProjectedFeatureReader.h"
#include "Services/Feature/GwsFeatureReader.h"
#include "Services/Feature/GwsConnectionPool.h"
#include "GwsQueryEngineImp.h"

static std::map<FdoThreadCapability, std::string>          s_FdoThreadCapability;
static std::map<FdoSpatialContextExtentType, std::string>  s_FdoSpatialContextExtentType;
static std::map<FdoClassType, std::string>                 s_FdoClassType;
static std::map<FdoDataType,  std::string>                 s_FdoDataType;
static std::map<FdoPropertyType,  std::string>             s_FdoPropertyTypeAsString;
static std::map<FdoFunctionCategoryType,  std::string>     s_FdoFunctionCategoryType;
static std::map<FdoCommandType,  std::string>              s_FdoCommandType;
static std::map<FdoConditionType,  std::string>            s_FdoConditionType;
static std::map<FdoSpatialOperations,  std::string>        s_FdoSpatialOperations;
static std::map<FdoDistanceOperations,  std::string>       s_FdoDistanceOperations;
static std::map<FdoExpressionType,  std::string>           s_FdoExpressionType;
static std::map<FdoGeometryType,  std::string>             s_FdoGeometryType;
static std::map<FdoGeometryComponentType,  std::string>    s_FdoGeometryComponentType;

bool MgdFeatureService::m_isInitialized = MgdFeatureService::Initialize();

MgdFeatureService::MgdFeatureService() 
: MgFeatureService()
{ 
	// Set a default join query batch size
    m_nJoinQueryBatchSize = MgConfigProperties::DefaultFeatureServicePropertyJoinQueryBatchSize;

    MgConfiguration* config = MgConfiguration::GetInstance();
    if(config)
    {
        // Get the join batch size
        config->GetIntValue(MgConfigProperties::FeatureServicePropertiesSection,
                            MgConfigProperties::FeatureServicePropertyJoinQueryBatchSize,
                            m_nJoinQueryBatchSize,
                            MgConfigProperties::DefaultFeatureServicePropertyJoinQueryBatchSize);
        // Get data cache size
        config->GetIntValue(MgConfigProperties::FeatureServicePropertiesSection,
                            MgConfigProperties::FeatureServicePropertyDataCacheSize,
                            m_nDataCacheSize,
                            MgConfigProperties::DefaultFeatureServicePropertyDataCacheSize);
    }
}

MgdFeatureService::~MgdFeatureService() 
{ 
	
}

bool MgdFeatureService::Initialize()
{
    // Thread Capability
    s_FdoThreadCapability[FdoThreadCapability_SingleThreaded]           = "SingleThreaded";
    s_FdoThreadCapability[FdoThreadCapability_PerConnectionThreaded]    = "PerConnectionThreaded";
    s_FdoThreadCapability[FdoThreadCapability_PerCommandThreaded]       = "PerCommandThreaded";
    s_FdoThreadCapability[FdoThreadCapability_MultiThreaded]            = "MultiThreaded";

    // Spatial Extent type
    s_FdoSpatialContextExtentType[FdoSpatialContextExtentType_Static]   = "Static";
    s_FdoSpatialContextExtentType[FdoSpatialContextExtentType_Dynamic]  = "Dynamic";

    // Class Types
    s_FdoClassType[FdoClassType_Class]                  = "Class";
    s_FdoClassType[FdoClassType_FeatureClass]           = "FeatureClass";
    s_FdoClassType[FdoClassType_NetworkClass]           = "NetworkClass";
    s_FdoClassType[FdoClassType_NetworkLayerClass]      = "NetworkLayerClass";
    s_FdoClassType[FdoClassType_NetworkNodeClass]       = "NetworkNodeClass";

    // Data types
    s_FdoDataType[FdoDataType_Boolean]     = "Boolean";
    s_FdoDataType[FdoDataType_Byte]        = "Byte";
    s_FdoDataType[FdoDataType_DateTime]    = "DateTime";
    // Implementation Note:  FdoDataType_Decimal is currently mapped to MgPropertyType::Double.
    // An MgDecimalProperty class should be implemented in a future release.
    s_FdoDataType[FdoDataType_Decimal]     = "Double";
    s_FdoDataType[FdoDataType_Double]      = "Double";
    s_FdoDataType[FdoDataType_Int16]       = "Int16";
    s_FdoDataType[FdoDataType_Int32]       = "Int32";
    s_FdoDataType[FdoDataType_Int64]       = "Int64";
    s_FdoDataType[FdoDataType_Single]      = "Single";
    s_FdoDataType[FdoDataType_String]      = "String";
    s_FdoDataType[FdoDataType_BLOB]        = "BLOB";
    s_FdoDataType[FdoDataType_CLOB]        = "CLOB";

    s_FdoPropertyTypeAsString[FdoPropertyType_DataProperty]     = "Data";
    s_FdoPropertyTypeAsString[FdoPropertyType_GeometricProperty]        = "Geometry";
    s_FdoPropertyTypeAsString[FdoPropertyType_ObjectProperty]        = "Object";
    s_FdoPropertyTypeAsString[FdoPropertyType_AssociationProperty]        = "Association";
    s_FdoPropertyTypeAsString[FdoPropertyType_RasterProperty]        = "Raster";

    s_FdoFunctionCategoryType[FdoFunctionCategoryType_Aggregate] = "Aggregate";
    s_FdoFunctionCategoryType[FdoFunctionCategoryType_Conversion] = "Conversion";
    s_FdoFunctionCategoryType[FdoFunctionCategoryType_Custom] = "Custom";
    s_FdoFunctionCategoryType[FdoFunctionCategoryType_Date] = "Date";
    s_FdoFunctionCategoryType[FdoFunctionCategoryType_Geometry] = "Geometry";
    s_FdoFunctionCategoryType[FdoFunctionCategoryType_Math] = "Math";
    s_FdoFunctionCategoryType[FdoFunctionCategoryType_Numeric] = "Numeric";
    s_FdoFunctionCategoryType[FdoFunctionCategoryType_String] = "String";
    s_FdoFunctionCategoryType[FdoFunctionCategoryType_Unspecified] = "Unspecified";

    // Commands
    s_FdoCommandType[FdoCommandType_Select]                             = "Select";
    s_FdoCommandType[FdoCommandType_Insert]                             = "Insert";
    s_FdoCommandType[FdoCommandType_Delete]                             = "Delete";
    s_FdoCommandType[FdoCommandType_Update]                             = "Update";
    //s_FdoCommandType[FdoCommandType_ExtendedSelect]                     = "ExtendedSelect";
    s_FdoCommandType[FdoCommandType_DescribeSchema]                     = "DescribeSchema";
    s_FdoCommandType[FdoCommandType_DescribeSchemaMapping]              = "DescribeSchemaMapping";
    s_FdoCommandType[FdoCommandType_ApplySchema]                        = "ApplySchema";
    s_FdoCommandType[FdoCommandType_DestroySchema]                      = "DestroySchema";
    s_FdoCommandType[FdoCommandType_ActivateSpatialContext]             = "ActivateSpatialContext";
    s_FdoCommandType[FdoCommandType_CreateSpatialContext]               = "CreateSpatialContext";
    s_FdoCommandType[FdoCommandType_DestroySpatialContext]              = "DestroySpatialContext";
    s_FdoCommandType[FdoCommandType_GetSpatialContexts]                 = "GetSpatialContexts";
    s_FdoCommandType[FdoCommandType_CreateMeasureUnit]                  = "CreateMeasureUnit";
    s_FdoCommandType[FdoCommandType_DestroyMeasureUnit]                 = "DestroyMeasureUnit";
    s_FdoCommandType[FdoCommandType_GetMeasureUnits]                    = "GetMeasureUnits";
    s_FdoCommandType[FdoCommandType_SQLCommand]                         = "SQLCommand";
    s_FdoCommandType[FdoCommandType_AcquireLock]                        = "AcquireLock";
    s_FdoCommandType[FdoCommandType_GetLockInfo]                        = "GetLockInfo";
    s_FdoCommandType[FdoCommandType_GetLockedObjects]                   = "GetLockedObjects";
    s_FdoCommandType[FdoCommandType_GetLockOwners]                      = "GetLockOwners";
    s_FdoCommandType[FdoCommandType_ReleaseLock]                        = "ReleaseLock";
    s_FdoCommandType[FdoCommandType_ActivateLongTransaction]            = "ActivateLongTransaction";
    s_FdoCommandType[FdoCommandType_DeactivateLongTransaction]          = "DeactivateLongTransaction";
    s_FdoCommandType[FdoCommandType_CommitLongTransaction]              = "CommitLongTransaction";
    s_FdoCommandType[FdoCommandType_CreateLongTransaction]              = "CreateLongTransaction";
    s_FdoCommandType[FdoCommandType_GetLongTransactions]                = "GetLongTransactions";
    s_FdoCommandType[FdoCommandType_FreezeLongTransaction]              = "FreezeLongTransaction";
    s_FdoCommandType[FdoCommandType_RollbackLongTransaction]            = "RollbackLongTransaction";
    s_FdoCommandType[FdoCommandType_ActivateLongTransactionCheckpoint]  = "ActivateLongTransactionCheckpoint";
    s_FdoCommandType[FdoCommandType_CreateLongTransactionCheckpoint]    = "CreateLongTransactionCheckpoint";
    s_FdoCommandType[FdoCommandType_GetLongTransactionCheckpoints]      = "GetLongTransactionCheckpoints";
    s_FdoCommandType[FdoCommandType_RollbackLongTransactionCheckpoint]  = "RollbackLongTransactionCheckpoint";
    s_FdoCommandType[FdoCommandType_ChangeLongTransactionPrivileges]    = "ChangeLongTransactionPrivileges";
    s_FdoCommandType[FdoCommandType_GetLongTransactionPrivileges]       = "GetLongTransactionPrivileges";
    s_FdoCommandType[FdoCommandType_ChangeLongTransactionSet]           = "ChangeLongTransactionSet";
    s_FdoCommandType[FdoCommandType_GetLongTransactionsInSet]           = "GetLongTransactionsInSet";
    s_FdoCommandType[FdoCommandType_NetworkShortestPath]                = "NetworkShortestPath";
    s_FdoCommandType[FdoCommandType_NetworkAllPaths]                    = "NetworkAllPaths";
    s_FdoCommandType[FdoCommandType_NetworkReachableNodes]              = "NetworkReachableNodes";
    s_FdoCommandType[FdoCommandType_NetworkReachingNodes]               = "NetworkReachingNodes";
    s_FdoCommandType[FdoCommandType_NetworkNearestNeighbors]            = "NetworkNearestNeighbors";
    s_FdoCommandType[FdoCommandType_NetworkWithinCost]                  = "NetworkWithinCost";
    s_FdoCommandType[FdoCommandType_NetworkTSP]                         = "NetworkTSP";
    s_FdoCommandType[FdoCommandType_ActivateTopologyArea]               = "ActivateTopologyArea";
    s_FdoCommandType[FdoCommandType_DeactivateTopologyArea]             = "DeactivateTopologyArea";
    s_FdoCommandType[FdoCommandType_ActivateTopologyInCommandResult]    = "ActivateTopologyInCommandResult";
    s_FdoCommandType[FdoCommandType_DeactivateTopologyInCommandResults] = "DeactivateTopologyInCommandResults";
    s_FdoCommandType[FdoCommandType_SelectAggregates]                   = "SelectAggregates";
    s_FdoCommandType[FdoCommandType_CreateDataStore]                    = "CreateDataStore";
    s_FdoCommandType[FdoCommandType_DestroyDataStore]                   = "DestroyDataStore";
    s_FdoCommandType[FdoCommandType_ListDataStores]                     = "ListDataStores";
    s_FdoCommandType[FdoCommandType_FirstProviderCommand]               = "FirstProviderCommand";

    // Condition types
    s_FdoConditionType[FdoConditionType_Comparison]                     = "Comparison";
    s_FdoConditionType[FdoConditionType_Like]                           = "Like";
    s_FdoConditionType[FdoConditionType_In]                             = "In";
    s_FdoConditionType[FdoConditionType_Null]                           = "Null";
    s_FdoConditionType[FdoConditionType_Spatial]                        = "Spatial";
    s_FdoConditionType[FdoConditionType_Distance]                       = "Distance";

    // Spatial Operations
    s_FdoSpatialOperations[FdoSpatialOperations_Contains]               = "Contains";
    s_FdoSpatialOperations[FdoSpatialOperations_Crosses]                = "Crosses";
    s_FdoSpatialOperations[FdoSpatialOperations_Disjoint]               = "Disjoint";
    s_FdoSpatialOperations[FdoSpatialOperations_Equals]                 = "Equals";
    s_FdoSpatialOperations[FdoSpatialOperations_Intersects]             = "Intersects";
    s_FdoSpatialOperations[FdoSpatialOperations_Overlaps]               = "Overlaps";
    s_FdoSpatialOperations[FdoSpatialOperations_Touches]                = "Touches";
    s_FdoSpatialOperations[FdoSpatialOperations_Within]                 = "Within";
    s_FdoSpatialOperations[FdoSpatialOperations_CoveredBy]              = "CoveredBy";
    s_FdoSpatialOperations[FdoSpatialOperations_Inside]                 = "Inside";
    s_FdoSpatialOperations[FdoSpatialOperations_EnvelopeIntersects]     = "EnvelopeIntersects";

    // Distance Operation
    s_FdoDistanceOperations[FdoDistanceOperations_Beyond]               = "Beyond";
    s_FdoDistanceOperations[FdoDistanceOperations_Within]               = "Within";

    // Expression Type
    s_FdoExpressionType[FdoExpressionType_Basic]                        = "Basic";
    s_FdoExpressionType[FdoExpressionType_Function]                     = "Function";
    s_FdoExpressionType[FdoExpressionType_Parameter]                    = "Parameter";

    // Geometry Type
    s_FdoGeometryType[FdoGeometryType_None]                     = "None";
    s_FdoGeometryType[FdoGeometryType_Point]                    = "Point";
    s_FdoGeometryType[FdoGeometryType_LineString]               = "LineString";
    s_FdoGeometryType[FdoGeometryType_Polygon]                  = "Polygon";
    s_FdoGeometryType[FdoGeometryType_MultiPoint]               = "MultiPoint";
    s_FdoGeometryType[FdoGeometryType_MultiLineString]          = "MultiLineString";
    s_FdoGeometryType[FdoGeometryType_MultiPolygon]             = "MultiPolygon";
    s_FdoGeometryType[FdoGeometryType_MultiGeometry]            = "MultiGeometry";
    s_FdoGeometryType[FdoGeometryType_CurveString]              = "CurveString";
    s_FdoGeometryType[FdoGeometryType_CurvePolygon]             = "CurvePolygon";
    s_FdoGeometryType[FdoGeometryType_MultiCurveString]         = "MultiCurveString";
    s_FdoGeometryType[FdoGeometryType_MultiCurvePolygon]        = "MultiCurvePolygon";

    // Geometry component type
    s_FdoGeometryComponentType[FdoGeometryComponentType_LinearRing]         = "LinearRing";
    s_FdoGeometryComponentType[FdoGeometryComponentType_CircularArcSegment] = "ArcSegment";
    s_FdoGeometryComponentType[FdoGeometryComponentType_LineStringSegment]  = "LinearSegment";
    s_FdoGeometryComponentType[FdoGeometryComponentType_Ring]               = "CurveRing";

    return true;
}

MgByteReader* MgdFeatureService::GetFeatureProviders() 
{ 
    Ptr<MgByteReader> reader;
    
    MG_FEATURE_SERVICE_TRY()

    STRING xml = L"<FeatureProviderRegistry>\n";

    FdoPtr<IProviderRegistry> reg = FdoFeatureAccessManager::GetProviderRegistry();
    const FdoProviderCollection* providers = reg->GetProviders();
    FdoPtr<IConnectionManager> connMgr = FdoFeatureAccessManager::GetConnectionManager();

    for (INT32 i = 0; i < providers->GetCount(); i++)
    {
        FdoPtr<FdoProvider> provider;
        FdoString* name = NULL;
        FdoString* disp = NULL;
        FdoString* desc = NULL;
        FdoString* ver = NULL;
        FdoString* fdoVer = NULL;

        FdoPtr<FdoIConnection> conn;
        FdoPtr<FdoIConnectionInfo> connInfo;
        FdoPtr<FdoIConnectionPropertyDictionary> connDict;

        bool ok = false;
        try 
        {
            provider = providers->GetItem(i);
            name = provider->GetName();
            disp = provider->GetDisplayName();
            desc = provider->GetDescription();
            ver = provider->GetVersion();
            fdoVer = provider->GetFeatureDataObjectsVersion();

            conn = connMgr->CreateConnection(name);
            connInfo = conn->GetConnectionInfo();
            connDict = connInfo->GetConnectionProperties();

            ok = true;
        }
        catch(FdoException* ex) //Creating a FDO provider connection with missing third party dependencies
        {
            ex->Release();
            ok = false;
        }
        catch (...)
        {
            ok = false;
        }

        if (!ok)
            continue;

        xml += L"\t<FeatureProvider>\n";
        
        xml += L"\t\t<Name>";
        xml += name;
        xml += L"</Name>\n";

        xml += L"\t\t<DisplayName>";
        xml += disp;
        xml += L"</DisplayName>\n";

        xml += L"\t\t<Description>";
        xml += desc;
        xml += L"</Description>\n";

        xml += L"\t\t<Version>";
        xml += ver;
        xml += L"</Version>\n";

        xml += L"\t\t<FeatureDataObjectsVersion>";
        xml += fdoVer;
        xml += L"</FeatureDataObjectsVersion>\n";

        xml += L"\t\t<ConnectionProperties>\n";
        
        FdoInt32 pCount;
        FdoString** propNames = connDict->GetPropertyNames(pCount);
        for (INT32 j = 0; j < pCount; j++)
        {
            FdoString* pName = propNames[j];
            FdoString* plName = connDict->GetLocalizedName(pName);
            FdoString* pDefault = connDict->GetPropertyDefault(pName);

            bool pRequired = connDict->IsPropertyRequired(pName);
            bool pProtected = connDict->IsPropertyProtected(pName);
            bool pEnumerable = connDict->IsPropertyEnumerable(pName);

            xml += L"\t\t\t<ConnectionProperty ";
            xml += L"Required=\"";
            xml += pRequired ? L"true" : L"false";
            xml += L"\" Protected=\"";
            xml += pProtected ? L"true" : L"false";
            xml += L"\" Enumerable=\"";
            xml += pEnumerable ? L"true" : L"false";
            xml += L"\">\n";

            xml += L"\t\t<Name>";
            xml += pName;
            xml += L"</Name>\n";

            xml += L"\t\t<LocalizedName>";
            xml += disp;
            xml += L"</LocalizedName>\n";

            xml += L"\t\t<DefaultValue>";
            xml += desc;
            xml += L"</DefaultValue>\n";

            if (pEnumerable)
            {
                try 
                {
                    FdoInt32 pvCount;
                    FdoString** pValues = connDict->EnumeratePropertyValues(pName, pvCount);

                    for (INT32 k = 0; k < pvCount; k++)
                    {
                        FdoString* pVal = pValues[k];

                        xml += L"\t\t<Value>";
                        xml += pVal;
                        xml += L"</Value>\n";
                    }
                }
                catch (FdoException* ex)
                {
                    ex->Release();
                }
            }
            xml += L"\t\t\t</ConnectionProperty>\n";
        }

        xml += L"\t\t</ConnectionProperties>\n";

        xml += L"\t</FeatureProvider>\n";
    }

    xml += L"</FeatureProviderRegistry>\n";

    std::string cxml = MgUtil::WideCharToMultiByte(xml);

    Ptr<MgByteSource> source = new MgByteSource((unsigned char*)cxml.c_str(), (INT32)cxml.length());
    source->SetMimeType(MgMimeType::Xml);
    reader = source->GetReader();

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureService::GetFeatureProviders")

	return reader.Detach();
}

MgStringCollection* MgdFeatureService::GetConnectionPropertyValues(CREFSTRING providerName,
                                                        CREFSTRING propertyName,
                                                        CREFSTRING partialConnString) 
{ 
    if (providerName.empty())
        throw new MgInvalidArgumentException(L"MgdFeatureService::GetConnectionPropertyValues", __LINE__, __WFILE__, NULL, L"", NULL);

    if (propertyName.empty())
        throw new MgInvalidArgumentException(L"MgdFeatureService::GetConnectionPropertyValues", __LINE__, __WFILE__, NULL, L"", NULL);

    Ptr<MgStringCollection> values;

    MG_FEATURE_SERVICE_TRY()

    Ptr<MgFeatureConnection> connWrapper = new MgFeatureConnection(providerName, partialConnString);
    {
        FdoPtr<FdoIConnection> conn = connWrapper->GetConnection();
        FdoPtr<FdoIConnectionInfo> connInfo = conn->GetConnectionInfo();
        FdoPtr<FdoIConnectionPropertyDictionary> connDict = connInfo->GetConnectionProperties();

        FdoInt32 pCount;
        FdoString** pValues = connDict->EnumeratePropertyValues(propertyName.c_str(), pCount);

        values = new MgStringCollection();

        for (INT32 i = 0; i < pCount; i++)
        {
            STRING val = pValues[i];
            values->Add(val);
        }
    }
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureService::GetConnectionPropertyValues")

	return values.Detach();
}

bool MgdFeatureService::TestConnection(CREFSTRING providerName, CREFSTRING connectionString) 
{
	bool ok = false;

    MG_FEATURE_SERVICE_TRY()

	Ptr<MgFeatureConnection> conn = new MgFeatureConnection(providerName, connectionString);
    ok = conn->IsConnectionOpen();

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureService::TestConnection")

	return ok;
}

bool MgdFeatureService::TestConnection(MgResourceIdentifier* resource) 
{ 
    CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdFeatureService::TestConnection");

	bool ok = false;

	MG_FEATURE_SERVICE_TRY()

	Ptr<MgFeatureConnection> conn = new MgFeatureConnection(resource);
	ok = conn->IsConnectionOpen();

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureService::TestConnection")

	return ok;
}

void MgdFeatureService::WriteCommandCapabilities(MgXmlUtil* caps, FdoICommandCapabilities* fcc)
{
    CHECKNULL(caps, L"MgdFeatureService::WriteCommandCapabilities");

    CHECKNULL(fcc, L"MgdFeatureService::WriteCommandCapabilities");

    DOMElement* root = caps->GetRootNode();
    CHECKNULL(root, L"MgdFeatureService::WriteCommandCapabilities");

    DOMElement* cmdNode = caps->AddChildNode(root, "Command");
    CHECKNULL(cmdNode, L"MgdFeatureService::WriteCommandCapabilities");

    // Add all command types
    FdoInt32 cnt = 0;
    FdoInt32* fcmd = fcc->GetCommands(cnt);
    if (cnt > 0 && fcmd != NULL)
    {
        DOMElement* scNode = caps->AddChildNode(cmdNode, "SupportedCommands");
        CHECKNULL(scNode, L"MgdFeatureService::WriteCommandCapabilities");

        for (FdoInt32 i=0; i < cnt; i++)
        {
            string cmdStr = s_FdoCommandType[(FdoCommandType)fcmd[i]];
            if (!cmdStr.empty())
            {
                switch (fcmd[i])
                {
                    case FdoCommandType_DescribeSchemaMapping:
                    case FdoCommandType_NetworkShortestPath:
                    case FdoCommandType_NetworkAllPaths:
                    case FdoCommandType_NetworkReachableNodes:
                    case FdoCommandType_NetworkReachingNodes:
                    case FdoCommandType_NetworkNearestNeighbors:
                    case FdoCommandType_NetworkWithinCost:
                    case FdoCommandType_NetworkTSP:
                    case FdoCommandType_ActivateTopologyArea:
                    case FdoCommandType_DeactivateTopologyArea:
                    case FdoCommandType_ActivateTopologyInCommandResult:
                    case FdoCommandType_DeactivateTopologyInCommandResults:
                    case FdoCommandType_SelectAggregates:
                    case FdoCommandType_CreateDataStore:
                    case FdoCommandType_DestroyDataStore:
                    case FdoCommandType_ListDataStores:
                        break;
                        // these enumerates were added for version 2
                        // fall thru to write this command
                    default:
                        caps->AddTextNode(scNode, "Name", cmdStr.c_str());
                        break;
                }
            }
        }
    }

    // Supports Parameters
    bool supportsParameters = fcc->SupportsParameters();
    caps->AddTextNode(cmdNode, "SupportsParameters", supportsParameters);

    // Supports Timeout
    bool supportsTimeout = fcc->SupportsTimeout();
    caps->AddTextNode(cmdNode, "SupportsTimeout", supportsTimeout);

    // Supports SupportsSelectExpressions
    bool supportsSelectExpressions = fcc->SupportsSelectExpressions();
    caps->AddTextNode(cmdNode, "SupportsSelectExpressions", supportsSelectExpressions);

    // Supports SupportsSelectFunctions
    bool supportsSelectFunctions = fcc->SupportsSelectFunctions();
    caps->AddTextNode(cmdNode, "SupportsSelectFunctions", supportsSelectFunctions);

    // Supports SupportsSelectDistinct
    bool supportsSelectDistinct = fcc->SupportsSelectDistinct();
    caps->AddTextNode(cmdNode, "SupportsSelectDistinct", supportsSelectDistinct);

    // Supports SupportsSelectOrdering
    bool supportsSelectOrdering = fcc->SupportsSelectOrdering();
    caps->AddTextNode(cmdNode, "SupportsSelectOrdering", supportsSelectOrdering);

    // Supports SupportsSelectGrouping
    bool supportsSelectGrouping = fcc->SupportsSelectGrouping();
    caps->AddTextNode(cmdNode, "SupportsSelectGrouping", supportsSelectGrouping);
}

void MgdFeatureService::WriteConnectionCapabilities(MgXmlUtil* caps, FdoIConnectionCapabilities* ficc)
{
    CHECKNULL(caps, L"MgdFeatureService::WriteConnectionCapabilities");

    DOMElement* root = caps->GetRootNode();
    CHECKNULL(root, L"MgdFeatureService::WriteConnectionCapabilities");

    DOMElement* connNode = caps->AddChildNode(root, "Connection");
    CHECKNULL(connNode, L"MgdFeatureService::WriteConnectionCapabilities");

    CHECKNULL((FdoIConnectionCapabilities*)ficc, L"MgdFeatureService::WriteConnectionCapabilities");

    // Thread
    FdoThreadCapability ftc = ficc->GetThreadCapability();
    string str = s_FdoThreadCapability[ftc];
    caps->AddTextNode(connNode, "ThreadCapability", str.c_str());

    // Spatial Context
    FdoInt32 cnt;
    FdoSpatialContextExtentType* fscet = ficc->GetSpatialContextTypes(cnt);
    if (cnt > 0 && fscet != NULL)
    {
        DOMElement* scNode = caps->AddChildNode(connNode, "SpatialContextExtent");
        CHECKNULL(scNode, L"MgdFeatureService::WriteConnectionCapabilities");

        for (FdoInt32 i = 0; i < cnt; i++)
        {
            string scStr = s_FdoSpatialContextExtentType[fscet[i]];
            caps->AddTextNode(scNode, "Type", scStr.c_str());
        }
    }

    // Locking
    bool supportsLocking = ficc->SupportsLocking();
    caps->AddTextNode(connNode, "SupportsLocking", supportsLocking);

    bool supportsTimeout = ficc->SupportsTimeout();
    caps->AddTextNode(connNode, "SupportsTimeout", supportsTimeout);

    bool supportsTransactions = ficc->SupportsTransactions();
    caps->AddTextNode(connNode, "SupportsTransactions", supportsTransactions);

    bool supportsLongTransactions = ficc->SupportsLongTransactions();
    caps->AddTextNode(connNode, "SupportsLongTransactions", supportsLongTransactions);

    bool supportsSQL = ficc->SupportsSQL();
    caps->AddTextNode(connNode, "SupportsSQL", supportsSQL);

    bool supportsConfiguration = ficc->SupportsConfiguration();
    caps->AddTextNode(connNode, "SupportsConfiguration", supportsConfiguration);
}

void MgdFeatureService::WriteFilterCapabilities(MgXmlUtil* caps, FdoIFilterCapabilities* ffc)
{
    CHECKNULL(caps, L"MgdFeatureService::WriteFilterCapabilities");

    CHECKNULL((FdoIFilterCapabilities*)ffc, L"MgdFeatureService::WriteFilterCapabilities");

    DOMElement* root = caps->GetRootNode();
    CHECKNULL(root, L"MgdFeatureService::WriteFilterCapabilities");

    DOMElement* filterNode = caps->AddChildNode(root, "Filter");
    CHECKNULL(filterNode, L"MgdFeatureService::WriteFilterCapabilities");

    // Add all condition types
    FdoInt32 cnt = 0;
    FdoConditionType* fct = ffc->GetConditionTypes(cnt);
    if (cnt > 0 && fct != NULL)
    {
        DOMElement* condNode = caps->AddChildNode(filterNode, "Condition");
        CHECKNULL(condNode, L"MgdFeatureService::WriteFilterCapabilities");

        for (FdoInt32 i=0; i < cnt; i++)
        {
            string condStr = s_FdoConditionType[fct[i]];
            caps->AddTextNode(condNode, "Type", condStr.c_str());
        }
    }

    // All spatial operations
    cnt = 0;
    FdoSpatialOperations* fso = ffc->GetSpatialOperations(cnt);
    if (cnt > 0 && fso != NULL)
    {
        DOMElement* fsoNode = caps->AddChildNode(filterNode, "Spatial");
        CHECKNULL(fsoNode, L"MgdFeatureService::WriteFilterCapabilities");

        for (FdoInt32 i=0; i < cnt; i++)
        {
            string operStr = s_FdoSpatialOperations[fso[i]];
            caps->AddTextNode(fsoNode, "Operation", operStr.c_str());
        }
    }

    // All distance operations
    cnt = 0;
    FdoDistanceOperations* fdo = ffc->GetDistanceOperations(cnt);
    if (cnt > 0 && fdo != NULL)
    {
        DOMElement* distNode = caps->AddChildNode(filterNode, "Distance");
        CHECKNULL(distNode, L"MgdFeatureService::WriteFilterCapabilities");

        for (FdoInt32 i=0; i < cnt; i++)
        {
            string fdoStr = s_FdoDistanceOperations[fdo[i]];
            caps->AddTextNode(distNode, "Operation", fdoStr.c_str());
        }
    }

    // supports Geodesic Distance
    bool supportsGeodesicDistance = ffc->SupportsGeodesicDistance();
    caps->AddTextNode(filterNode, "SupportsGeodesicDistance", supportsGeodesicDistance);

    // supports NonLiteral Geometric Operations
    bool supportsNonLiteralGeometricOperations = ffc->SupportsNonLiteralGeometricOperations();
    caps->AddTextNode(filterNode, "SupportsNonLiteralGeometricOperations", supportsNonLiteralGeometricOperations);
}

void MgdFeatureService::WriteGeometryCapabilities(MgXmlUtil* caps, FdoIGeometryCapabilities* fgc)
{
    CHECKNULL(caps, L"MgdFeatureService::WriteGeometryCapabilities");

    MG_FEATURE_SERVICE_TRY()

    // Provider has no geometric capabilities
    if (NULL == (FdoIGeometryCapabilities*)fgc)
    {
        return;
    }

    DOMElement* root = caps->GetRootNode();
    CHECKNULL(root, L"MgdFeatureService::WriteGeometryCapabilities");

    DOMElement* geometryNode = caps->AddChildNode(root, "Geometry");
    CHECKNULL(geometryNode, L"MgdFeatureService::WriteGeometryCapabilities");

    FdoInt32 cnt = 0;
    FdoGeometryType* geomType = fgc->GetGeometryTypes(cnt);
    if (cnt > 0 && geomType != NULL)
    {
        DOMElement* geometryTypeNode = caps->AddChildNode(geometryNode, "Types");
        CHECKNULL(geometryTypeNode, L"MgdFeatureService::WriteGeometryCapabilities");

        for (FdoInt32 i=0; i < cnt; i++)
        {
            string geomTypeStr = s_FdoGeometryType[geomType[i]];
            caps->AddTextNode(geometryTypeNode, "Type", geomTypeStr.c_str());
        }
    }

    FdoGeometryComponentType* geomCompType = fgc->GetGeometryComponentTypes(cnt);
    if (cnt > 0 && geomCompType != NULL)
    {
        DOMElement* geometryCompNode = caps->AddChildNode(geometryNode, "Components");
        CHECKNULL(geometryCompNode, L"MgdFeatureService::WriteGeometryCapabilities");

        for (FdoInt32 i=0; i < cnt; i++)
        {
            string geomCompStr = s_FdoGeometryComponentType[geomCompType[i]];
            caps->AddTextNode(geometryCompNode, "Type", geomCompStr.c_str());
        }
    }

    FdoInt32 dim = fgc->GetDimensionalities();

    char buff[8]; buff[0] = 0;
    sprintf(buff, "%d", dim);

    caps->AddTextNode(geometryNode, "Dimensionality", &buff[0]);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureService::WriteGeometryCapabilities")
}

void MgdFeatureService::WriteExpressionCapabilities(MgXmlUtil* caps, FdoIExpressionCapabilities* fec)
{
    CHECKNULL(caps, L"MgdFeatureService::WriteExpressionCapabilities");

    CHECKNULL((FdoIExpressionCapabilities*)fec, L"MgdFeatureService::WriteExpressionCapabilities");

    DOMElement* root = caps->GetRootNode();
    CHECKNULL(root, L"MgdFeatureService::WriteExpressionCapabilities");

    DOMElement* expressionNode = caps->AddChildNode(root, "Expression");
    CHECKNULL(expressionNode, L"MgdFeatureService::WriteExpressionCapabilities");

    // Add all expression types
    FdoInt32 cnt = 0;
    FdoExpressionType* fet = fec->GetExpressionTypes(cnt);
    if (cnt > 0 && fet != NULL)
    {
        DOMElement* typeNode = caps->AddChildNode(expressionNode, "Type");
        CHECKNULL(typeNode, L"MgdFeatureService::WriteExpressionCapabilities");

        for (FdoInt32 i=0; i < cnt; i++)
        {
            string typeStr = s_FdoExpressionType[fet[i]];
            caps->AddTextNode(typeNode, "Name", typeStr.c_str());
        }
    }

    // Add all functions available
    cnt = 0;
    FdoPtr<FdoFunctionDefinitionCollection> ffdc = fec->GetFunctions();
    if (NULL != (FdoFunctionDefinitionCollection*)ffdc)
    {
        FdoInt32 funcCnt = ffdc->GetCount();
        if (funcCnt > 0)
        {
            // Add function definition collection element if there are any functions available
            DOMElement* funcDefColNode = caps->AddChildNode(expressionNode, "FunctionDefinitionList");
            CHECKNULL(funcDefColNode, L"MgdFeatureService::WriteExpressionCapabilities");

            for (FdoInt32 i=0; i < funcCnt; i++)
            {
                // Add function definition element
                FdoPtr<FdoFunctionDefinition> ffd = ffdc->GetItem(i);
                CHECKNULL((FdoFunctionDefinition*)ffd, L"MgdFeatureService::WriteExpressionCapabilities");

                DOMElement* funcDefNode = caps->AddChildNode(funcDefColNode, "FunctionDefinition");
                CHECKNULL(funcDefNode, L"MgdFeatureService::WriteExpressionCapabilities");

                const char* strName = MgUtil::WideCharToMultiByte(ffd->GetName());
                const char* strDesc = MgUtil::WideCharToMultiByte(ffd->GetDescription());

                FdoFunctionCategoryType eFdoFunctionCategoryType = ffd->GetFunctionCategoryType();
                string strFunctionCategoryType = s_FdoFunctionCategoryType[eFdoFunctionCategoryType];

                caps->AddTextNode(funcDefNode, "Name", strName);
                caps->AddTextNode(funcDefNode, "Description", strDesc);
                caps->AddTextNode(funcDefNode, "CategoryType",  strFunctionCategoryType.c_str());
                caps->AddTextNode(funcDefNode, "IsAggregate",  ffd->IsAggregate());
                caps->AddTextNode(funcDefNode, "IsSupportsVariableArgumentsList",  ffd->SupportsVariableArgumentsList());

                delete[] strName;
                delete[] strDesc;

                // Add argument of each functions if there are any
                FdoPtr<FdoReadOnlySignatureDefinitionCollection> signatures = ffd->GetSignatures();
                if (NULL != (FdoReadOnlySignatureDefinitionCollection*)signatures)
                {
                    FdoInt32 signaturesCnt = signatures->GetCount();
                    if (signaturesCnt > 0)
                    {

                        DOMElement* signDefColNode = caps->AddChildNode(funcDefNode, "SignatureDefinitionCollection");
                        CHECKNULL(signDefColNode, L"MgdFeatureService::WriteExpressionCapabilities");
                        for (FdoInt32 j=0; j < signaturesCnt; j++)
                        {

                            // Add SignatureDefinition for each signature
                            FdoPtr<FdoSignatureDefinition> fsd = signatures->GetItem(j);
                            CHECKNULL((FdoSignatureDefinition*)fsd, L"MgdFeatureService::WriteExpressionCapabilities");

                            DOMElement* signDefNode = caps->AddChildNode(signDefColNode, "SignatureDefinition");
                            CHECKNULL(signDefNode, L"MgdFeatureService::WriteExpressionCapabilities");

                            FdoPropertyType eSignPropertyDataType = fsd->GetReturnPropertyType();
                            string strSignPropertyType = s_FdoPropertyTypeAsString[eSignPropertyDataType];
                            string strSignDataType;
                            if (eSignPropertyDataType == FdoPropertyType_DataProperty)
                            {
                                FdoDataType eSignDataType = fsd->GetReturnType();
                                strSignDataType = s_FdoDataType[eSignDataType];
                            }

                            caps->AddTextNode(signDefNode, "PropertyType", strSignPropertyType.c_str());
                            if (eSignPropertyDataType == FdoPropertyType_DataProperty)
                                caps->AddTextNode(signDefNode, "DataType",  strSignDataType.c_str());

                            DOMElement* argDefColNode = caps->AddChildNode(signDefNode, "ArgumentDefinitionList");
                            CHECKNULL(argDefColNode, L"MgdFeatureService::WriteExpressionCapabilities");

                            FdoPtr<FdoReadOnlyArgumentDefinitionCollection> fads = fsd->GetArguments();
                            if (NULL != (FdoReadOnlyArgumentDefinitionCollection *) fads)
                            {
                                FdoInt32 argCnt = fads->GetCount();
                                if (argCnt > 0)
                                {
                                    for (int k=0; k<argCnt; k++)
                                    {
                                        FdoPtr<FdoArgumentDefinition> fad = fads->GetItem(k);
                                        CHECKNULL((FdoArgumentDefinition*)fad, L"MgdFeatureService::WriteExpressionCapabilities");

                                        DOMElement* argDefNode = caps->AddChildNode(argDefColNode, "ArgumentDefinition");
                                        CHECKNULL(argDefNode, L"MgdFeatureService::WriteExpressionCapabilities");

                                        const char* strArgName = MgUtil::WideCharToMultiByte(fad->GetName());
                                        const char* strArgDesc = MgUtil::WideCharToMultiByte(fad->GetDescription());

                                        FdoPropertyType eArgPropertyDataType = fad->GetPropertyType();
                                        string strArgPropertyType = s_FdoPropertyTypeAsString[eArgPropertyDataType];
                                        string strArgDataType;
                                        if (eArgPropertyDataType == FdoPropertyType_DataProperty)
                                        {
                                            FdoDataType eArgDataType = fad->GetDataType();
                                            strArgDataType = s_FdoDataType[eArgDataType];
                                        }

                                        caps->AddTextNode(argDefNode, "Name", strArgName);
                                        caps->AddTextNode(argDefNode, "Description", strArgDesc);
                                        caps->AddTextNode(argDefNode, "PropertyType", strArgPropertyType.c_str());
                                        if (eArgPropertyDataType == FdoPropertyType_DataProperty)
                                            caps->AddTextNode(argDefNode, "DataType",  strArgDataType.c_str());

                                        delete[] strArgName;
                                        delete[] strArgDesc;

                                        FdoPtr<FdoPropertyValueConstraintList> fpvc = fad->GetArgumentValueList();
                                        if (NULL != (FdoPropertyValueConstraintList *) fpvc)
                                        {
                                            if (fpvc->GetConstraintType() == FdoPropertyValueConstraintType_List)
                                            {
                                                DOMElement* propValueConstListNode = caps->AddChildNode(argDefNode, "PropertyValueConstraintList");
                                                CHECKNULL(propValueConstListNode, L"MgdFeatureService::WriteExpressionCapabilities");

                                                FdoPropertyValueConstraintType eConstraintType = fpvc->GetConstraintType();

                                                FdoPtr<FdoDataValueCollection> dvc = fpvc->GetConstraintList();
                                                if (NULL != (FdoDataValueCollection *) dvc)
                                                {
                                                    FdoInt32 dvCnt = dvc->GetCount();
                                                    if (dvCnt > 0)
                                                    {
                                                        for (int l=0; l<dvCnt; l++)
                                                        {
                                                            FdoPtr<FdoDataValue> dv = dvc->GetItem(l);
                                                            CHECKNULL((FdoDataValue*)dv, L"MgdFeatureService::WriteExpressionCapabilities");
                                                            FdoDataType dataType = dv->GetDataType();
                                                            // FdoDataType_String is the only supported type
                                                            if (dataType == FdoDataType_String)
                                                            {
                                                                FdoStringValue *stringValue = (FdoStringValue *) dv.p;
                                                                FdoString* xmlValue = stringValue->GetString();
                                                                const char* strDataValue = MgUtil::WideCharToMultiByte(stringValue->GetString());
                                                                caps->AddTextNode(propValueConstListNode, "Value", xmlValue);
                                                                delete[] strDataValue;
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void MgdFeatureService::WriteSchemaCapabilities(MgXmlUtil* caps, FdoISchemaCapabilities* fsc)
{
    CHECKNULL(caps, L"MgdFeatureService::WriteSchemaCapabilities");
    CHECKNULL((FdoISchemaCapabilities*)fsc, L"MgdFeatureService::WriteSchemaCapabilities");

    DOMElement* root = caps->GetRootNode();
    CHECKNULL(root, L"MgdFeatureService::WriteSchemaCapabilities");

    DOMElement* schemaNode = caps->AddChildNode(root, "Schema");
    CHECKNULL(schemaNode, L"MgdFeatureService::WriteSchemaCapabilities");

    // Add all class types
    FdoInt32 cnt = 0;
    FdoClassType* fct = fsc->GetClassTypes(cnt);
    if (cnt > 0 && fct != NULL)
    {
        DOMElement* classNode = caps->AddChildNode(schemaNode, "Class");
        CHECKNULL(classNode, L"MgdFeatureService::WriteSchemaCapabilities");

        for (FdoInt32 i=0; i < cnt; i++)
        {
            string scStr = s_FdoClassType[fct[i]];
            caps->AddTextNode(classNode, "Type", scStr.c_str());
        }
    }

    // Add all data types
    cnt = 0;
    FdoDataType* fdt = fsc->GetDataTypes(cnt);
    if (cnt > 0 && fdt != NULL)
    {
        DOMElement* dataNode = caps->AddChildNode(schemaNode, "Data");
        CHECKNULL(dataNode, L"MgdFeatureService::WriteSchemaCapabilities");

        for (FdoInt32 i=0; i < cnt; i++)
        {
            string dtStr = s_FdoDataType[fdt[i]];
            caps->AddTextNode(dataNode, "Type", dtStr.c_str());
        }
    }

    // Supports Inheritance
    bool supportsInheritance        = fsc->SupportsInheritance();
    caps->AddTextNode(schemaNode, "SupportsInheritance", supportsInheritance);

    // Supports Multiple Schemas
    bool supportsMultipleSchemas    = fsc->SupportsMultipleSchemas();
    caps->AddTextNode(schemaNode, "SupportsMultipleSchemas", supportsMultipleSchemas);

    bool supportsObjectProperties   = fsc->SupportsObjectProperties();
    caps->AddTextNode(schemaNode, "SupportsObjectProperties", supportsObjectProperties);

    bool supportsAssociationProperties = fsc->SupportsAssociationProperties();
    caps->AddTextNode(schemaNode, "SupportsAssociationProperties", supportsAssociationProperties);

    bool supportsSchemaOverrides =  fsc->SupportsSchemaOverrides();
    caps->AddTextNode(schemaNode, "SupportsSchemaOverrides", supportsSchemaOverrides);

    bool supportsNetworkModel  = fsc->SupportsNetworkModel();
    caps->AddTextNode(schemaNode, "SupportsNetworkModel", supportsNetworkModel);

    bool supportsAutoIdGeneration  = fsc->SupportsAutoIdGeneration();
    caps->AddTextNode(schemaNode, "SupportsAutoIdGeneration", supportsAutoIdGeneration);

    bool supportsDataStoreScopeUniqueIdGeneration  = fsc->SupportsDataStoreScopeUniqueIdGeneration();
    caps->AddTextNode(schemaNode, "SupportsDataStoreScopeUniqueIdGeneration", supportsDataStoreScopeUniqueIdGeneration);

    FdoDataType* sagt = fsc->GetSupportedAutoGeneratedTypes(cnt);
    if (cnt > 0 && sagt != NULL)
    {
        DOMElement* sagtNode = caps->AddChildNode(schemaNode, "SupportedAutoGeneratedTypes");
        CHECKNULL(sagtNode, L"MgdFeatureService::WriteSchemaCapabilities");

        for (FdoInt32 i=0; i < cnt; i++)
        {
            string sagtStr = s_FdoDataType[sagt[i]];
            caps->AddTextNode(sagtNode, "Type", sagtStr.c_str());
        }
    }

    bool supportsSchemaModification  = fsc->SupportsSchemaModification();
    caps->AddTextNode(schemaNode, "SupportsSchemaModification", supportsSchemaModification);
}

void MgdFeatureService::WriteRasterCapabilities(MgXmlUtil* caps, FdoIRasterCapabilities* frc)
{
    CHECKNULL(caps, L"MgdFeatureService::WriteRasterCapabilities");
    CHECKNULL((FdoIRasterCapabilities*)frc, L"MgdFeatureService::WriteRasterCapabilities");

    DOMElement* root = caps->GetRootNode();
    CHECKNULL(root, L"MgdFeatureService::WriteRasterCapabilities");

    DOMElement* rasterNode = caps->AddChildNode(root, "Raster");
    CHECKNULL(rasterNode, L"MgdFeatureService::WriteRasterCapabilities");

    // Supports Raster
    bool supportsRaster = frc->SupportsRaster();
    caps->AddTextNode(rasterNode, "SupportsRaster", supportsRaster);

    // Supports Stitching
    bool supportsStitching = frc->SupportsStitching();
    caps->AddTextNode(rasterNode, "SupportsStitching", supportsStitching);

    bool supportsSubsampling = frc->SupportsSubsampling();
    caps->AddTextNode(rasterNode, "SupportsSubsampling", supportsSubsampling);
}

void MgdFeatureService::WriteTopologyCapabilities(MgXmlUtil* caps, FdoITopologyCapabilities* frc)
{
    CHECKNULL(caps, L"MgdFeatureService::WriteTopologyCapabilities");

    MG_FEATURE_SERVICE_TRY()
    // Provider has no topology capabilities
    if (NULL == (FdoITopologyCapabilities*)frc)
    {
        return;
    }

    DOMElement* root = caps->GetRootNode();
    CHECKNULL(root, L"MgdFeatureService::WriteTopologyCapabilities");

    DOMElement* topologyNode = caps->AddChildNode(root, "Topology");
    CHECKNULL(topologyNode, L"MgdFeatureService::WriteTopologyCapabilities");

    // Supports Topology
    bool supportsTopology = frc->SupportsTopology();
    caps->AddTextNode(topologyNode, "SupportsTopology", supportsTopology);

    // Supports Stitching
    bool supportsTopologicalHierarchy = frc->SupportsTopologicalHierarchy();
    caps->AddTextNode(topologyNode, "SupportsTopologicalHierarchy", supportsTopologicalHierarchy);

    bool breaksCurveCrossingsAutomatically = frc->BreaksCurveCrossingsAutomatically();
    caps->AddTextNode(topologyNode, "BreaksCurveCrossingsAutomatically", breaksCurveCrossingsAutomatically);

    bool activatesTopologyByArea = frc->ActivatesTopologyByArea();
    caps->AddTextNode(topologyNode, "ActivatesTopologyByArea", activatesTopologyByArea);

    bool constrainsFeatureMovements = frc->ConstrainsFeatureMovements();
    caps->AddTextNode(topologyNode, "ConstrainsFeatureMovements", constrainsFeatureMovements);

    // TODO: Change this to CATCH_AND_THROW when SimpleDB stops throwing exception of not implemented
    MG_FEATURE_SERVICE_CATCH(L"MgServerGetProviderCapabilities.CreateTopologyCapabilities")

}

MgByteReader* MgdFeatureService::GetCapabilities(CREFSTRING providerName) 
{
    Ptr<MgByteReader> ret;
    
    MG_FEATURE_SERVICE_TRY()

	ret = GetCapabilities(providerName, L"");

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureService::GetCapabilities")

    return ret.Detach();
}

MgByteReader* MgdFeatureService::GetCapabilities(CREFSTRING providerName, CREFSTRING connectionString) 
{ 
    if (providerName.empty())
        throw new MgInvalidArgumentException(L"MgdFeatureService::GetCapabilities", __LINE__, __WFILE__, NULL, L"", NULL);

    Ptr<MgByteReader> caps;

    MG_FEATURE_SERVICE_TRY()

    Ptr<MgFeatureConnection> connWrap = new MgFeatureConnection(providerName, connectionString);
    {
        FdoPtr<FdoIConnection> conn = connWrap->GetConnection();

        // Root node element created
        // this XML follows the FdoProviderCapabilities-1.0.0.xsd schema
        std::auto_ptr<MgXmlUtil> xml(new MgXmlUtil("FeatureProviderCapabilities"));
        CHECKNULL(xml.get(), L"MgdFeatureService::GetCapabilities");
        DOMElement* root = xml->GetRootNode();

        // Provide name element and its attribute
        DOMElement* providerNode = xml->AddChildNode(root, "Provider");
        xml->SetAttribute(providerNode, "Name", providerName.c_str());

        FdoPtr<FdoICommandCapabilities> cmdCaps = conn->GetCommandCapabilities();
        FdoPtr<FdoIConnectionCapabilities> connCaps = conn->GetConnectionCapabilities();
        FdoPtr<FdoIExpressionCapabilities> exprCaps = conn->GetExpressionCapabilities();
        FdoPtr<FdoIFilterCapabilities> filterCaps = conn->GetFilterCapabilities();
        FdoPtr<FdoIGeometryCapabilities> geomCaps = conn->GetGeometryCapabilities();
        FdoPtr<FdoIRasterCapabilities> rasterCaps = conn->GetRasterCapabilities();
        FdoPtr<FdoISchemaCapabilities> schemaCaps = conn->GetSchemaCapabilities();
        FdoPtr<FdoITopologyCapabilities> topoCaps = conn->GetTopologyCapabilities();

        WriteConnectionCapabilities(xml.get(), connCaps);
        WriteSchemaCapabilities(xml.get(), schemaCaps);
        WriteCommandCapabilities(xml.get(), cmdCaps);
        WriteFilterCapabilities(xml.get(), filterCaps);
        WriteExpressionCapabilities(xml.get(), exprCaps);
        WriteRasterCapabilities(xml.get(), rasterCaps);
        WriteTopologyCapabilities(xml.get(), topoCaps);
        WriteGeometryCapabilities(xml.get(), geomCaps);

        caps = xml->ToReader();
    }
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureService::GetCapabilities")

	return caps.Detach();
}

void MgdFeatureService::ApplySchema(MgResourceIdentifier* resource, MgFeatureSchema* schema) 
{ 
    CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdFeatureService::ApplySchema");
    CHECKARGUMENTNULL(schema, L"MgdFeatureService::ApplySchema");

	MG_FEATURE_SERVICE_TRY()

    // Connect to provider
    Ptr<MgFeatureConnection> connWrap = new MgFeatureConnection(resource);
    {
        FdoPtr<FdoIConnection> fdoConn = connWrap->GetConnection();
        
        FdoPtr<FdoICommandCapabilities> cmdCaps = fdoConn->GetCommandCapabilities();

        // connection must be open
        bool bRefresh = false;
        bool bApplySchema = connWrap->SupportsCommand(FdoCommandType_ApplySchema);
        bool bDescribeSchema = connWrap->SupportsCommand(FdoCommandType_DescribeSchema);;

        // Check whether command is supported by provider
        if (!bApplySchema || !bDescribeSchema)
        {
            // TODO: specify which argument and message, once we have the mechanism
            STRING message = MgFeatureUtil::GetMessage(L"MgCommandNotSupported");
            throw new MgInvalidOperationException(L"MgdFeatureService::ApplySchema", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        FdoPtr<FdoIDescribeSchema> fdoDecribeSchemaCmd = (FdoIDescribeSchema*) fdoConn->CreateCommand(FdoCommandType_DescribeSchema);
        CHECKNULL((FdoIDescribeSchema*)fdoDecribeSchemaCmd, L"MgdFeatureService::ApplySchema");

        FdoPtr<FdoFeatureSchemaCollection> schemas = fdoDecribeSchemaCmd->Execute();
        CHECKNULL((FdoFeatureSchemaCollection*)schemas, L"MgdFeatureService::ApplySchema");

        FdoPtr<FdoIApplySchema> fdoApplySchemaCmd = (FdoIApplySchema*)fdoConn->CreateCommand(FdoCommandType_ApplySchema);
        CHECKNULL((FdoIApplySchema*)fdoApplySchemaCmd, L"MgdFeatureService::ApplySchema");

        STRING schemaName = schema->GetName();
        FdoPtr<FdoFeatureSchema> fdoOldSchema = schemas->FindItem(schemaName.c_str());
        if (NULL == fdoOldSchema)
        {
            if (!schema->IsDeleted())
            {
                FdoPtr<FdoFeatureSchema> fdoNewSchema = MgFeatureUtil::GetFdoFeatureSchema(schema);
                fdoApplySchemaCmd->SetFeatureSchema(fdoNewSchema);
                fdoApplySchemaCmd->Execute();
                bRefresh = true;
            }
        }
        else
        {
            if (!schema->IsDeleted())
                MgFeatureUtil::UpdateFdoFeatureSchema(schema, fdoOldSchema);
            else
                fdoOldSchema->Delete();

            fdoApplySchemaCmd->SetFeatureSchema(fdoOldSchema);
            fdoApplySchemaCmd->Execute();
            bRefresh = true;
        }

        // Clear the cached schema so that MgFeatureService::DescribeSchema
        // can return the correct schema
        if (bRefresh)
        {
            MgFeatureServiceCache* cache = MgFeatureServiceCache::GetInstance();
            cache->RemoveEntry(resource);
        }
    }
    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH_AND_THROW(resource, L"MgdFeatureService::ApplySchema")
}

MgFeatureSchemaCollection* MgdFeatureService::DescribeSchema(MgResourceIdentifier* resource,
                                                             CREFSTRING schemaName,
                                                             MgStringCollection* classNames) 
{
    Ptr<MgFeatureSchemaCollection> ret;

    MG_FEATURE_SERVICE_TRY()

    ret = DescribeSchema(resource, schemaName, classNames, true);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureService::DescribeSchema")

    return ret.Detach();
}

MgFeatureSchemaCollection* MgdFeatureService::DescribeSchema(MgResourceIdentifier* resource,
                                                             CREFSTRING schemaName,
                                                             MgStringCollection* classNames,
                                                             bool serialize) 
{ 
	CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdFeatureService::DescribeSchema");

    Ptr<MgFeatureSchemaCollection> fsCollection;
    Ptr<MgFeatureSchemaCollection> clone;
    MG_FEATURE_SERVICE_TRY()

    MgFeatureServiceCache* cache = MgFeatureServiceCache::GetInstance();
    fsCollection = cache->GetSchemas(resource, schemaName, classNames, serialize);

    if (NULL == fsCollection.p)
    {
        fsCollection = new MgFeatureSchemaCollection();

        bool classNameHintUsed = true;
        FdoPtr<FdoFeatureSchemaCollection> ffsc =
            DescribeFdoSchema(resource, schemaName, classNames, classNameHintUsed);
        CHECKNULL(ffsc.p, L"MgdFeatureService::DescribeSchema");

        // Get schema count
        FdoInt32 schemaCount = ffsc->GetCount();

        //
        // A new MgFeatureSchema needs to be created for each primary schema in FDO schemas.
        //

        Ptr<MgFeatureSchema> schema;
        Ptr<MgClassDefinitionCollection> classCol;

        for (int nSchemaIndex = 0; nSchemaIndex < schemaCount; nSchemaIndex++)
        {
            // Retrieve schema from the collection
            FdoPtr<FdoFeatureSchema> ffs = ffsc->GetItem(nSchemaIndex);
            STRING currSchemaName = (wchar_t*)ffs->GetName();

            // Check if this schema is from secondary source which will be prefixed with [ExtensionName][RelationName],
            // ie [ExtensionName][RelationName]SchemaName
            if (currSchemaName.find(L"[") == 0)
            {
                // Found a schema for secondary source, so just skip over it for now
                continue;
            }

            schema = new MgFeatureSchema();
            classCol = schema->GetClasses();

            // Set the schema name and description
            FdoStringP fSchemaName = ffs->GetName();
            schema->SetName(STRING(fSchemaName));

            FdoStringP fSchemaDesc = ffs->GetDescription();
            schema->SetDescription(STRING(fSchemaDesc));

            // Get all classes for the schema
            FdoPtr<FdoClassCollection> fcc = ffs->GetClasses();
            FdoInt32 classCount = fcc->GetCount();

            // Add the primary class definitions to the MgClassDefinitionCollection
            for (FdoInt32 nClassIndex = 0; nClassIndex < classCount; nClassIndex++)
            {
                FdoPtr<FdoClassDefinition> fc = fcc->GetItem(nClassIndex);

                FdoStringP qname = fc->GetQualifiedName();
                FdoStringP name = fc->GetName();

                if (name != NULL && qname != NULL)
                {
                    Ptr<MgClassDefinition> classDefinition = MgFeatureUtil::GetMgClassDefinition(fc, serialize);
                    classCol->Add(classDefinition);
                }
            }

            //
            // A new MgClassDefinition needs to be created for each extension and added to the MgClassCollection
            //

            Ptr<MgFeatureSourceCacheItem> fsCache = cache->GetFeatureSource(resource);

            MdfModel::FeatureSource* featureSource = fsCache->Get();
            CHECKNULL(featureSource, L"MgdFeatureService::DescribeSchema");
            MdfModel::ExtensionCollection* extensions = featureSource->GetExtensions();
            CHECKNULL(extensions, L"MgdFeatureService::DescribeSchema");
            int extensionCount = extensions->GetCount();

            for (int i = 0; i < extensionCount; i++)
            {
                Ptr<MgClassDefinition> extClassDefinition;
                FdoPtr<FdoClassDefinition> originalClassDef;

                MdfModel::Extension* extension = extensions->GetAt(i);
                CHECKNULL(extension, L"MgdFeatureService::DescribeSchema");

                // Get the extension name
                STRING extensionName = (STRING)extension->GetName();

                // get FeatureClass element - this tells us which class is extended (SchemaName:ClassName)
                STRING featureClass = (STRING)extension->GetFeatureClass();

                // Parse the schemaName from the className
                STRING primSchemaName, primClassName;
                MgUtil::ParseQualifiedClassName(featureClass, primSchemaName, primClassName);

                if (currSchemaName != primSchemaName)
                {
                    continue;
                }

                // Cycle thru FDO schemas for schemaName.
                for (int nIndex = 0; nIndex < schemaCount; nIndex++)
                {
                    FdoPtr<FdoFeatureSchema> ffs = ffsc->GetItem(nIndex);
                    STRING currSchemaName = (wchar_t*)ffs->GetName();

                    // Check if this schema is from secondary source
                    if (currSchemaName.find(L"[") == 0)
                    {
                        // Found a schema for secondary source, so just skip over it
                        continue;
                    }

                    // get the class collection for this schema
                    FdoPtr<FdoClassCollection> fcc = ffs->GetClasses();
                    FdoInt32 classCount = fcc->GetCount();

                    for (int nClassIndex = 0; nClassIndex < classCount; nClassIndex++)
                    {
                        originalClassDef = fcc->GetItem(nClassIndex);

                        STRING className = (wchar_t*)originalClassDef->GetName();
                        if (className == primClassName)
                        {
                            // get the class definition
                            extClassDefinition = MgFeatureUtil::GetMgClassDefinition(originalClassDef, serialize);
                            break;
                        }
                    }

                    if (NULL != extClassDefinition)
                    {
                        break;
                    }
                }

                if (NULL == extClassDefinition)
                {
                    continue;
                }

                CalculatedPropertyCollection* calcPropsColl = extension->GetCalculatedProperties();
                if (calcPropsColl != NULL && calcPropsColl->GetCount() != 0)
                {
                    FdoPtr<FdoIdentifierCollection> idList = FdoIdentifierCollection::Create();
                    for (int idx = 0; idx < calcPropsColl->GetCount(); idx++)
                    {
                        CalculatedProperty* calcProp = calcPropsColl->GetAt(idx);
                        FdoPtr<FdoExpression> expressionCalc = FdoExpression::Parse(calcProp->GetExpression().c_str());
                        FdoPtr<FdoComputedIdentifier> idfCalc = FdoComputedIdentifier::Create(calcProp->GetName().c_str(), expressionCalc);
                        idList->Add(idfCalc);
                    }

                    Ptr<MgPropertyDefinitionCollection> mpdcLocal = extClassDefinition->GetProperties();
                    for(int idx = 0; idx < calcPropsColl->GetCount(); idx++)
                    {
                        CalculatedProperty* calcProp = calcPropsColl->GetAt(idx);
                        if (calcProp != NULL)
                        {
                            MdfString nameExpr = calcProp->GetName();
                            MdfString valueExpr = calcProp->GetExpression();
                            if (nameExpr.size() != 0 && valueExpr.size() != 0)
                            {
                                FdoPropertyType retPropType = FdoPropertyType_DataProperty;
                                FdoDataType retDataType = FdoDataType_Double;
                                FdoPtr<FdoExpression> expr = FdoExpression::Parse(valueExpr.c_str());
                                FdoPtr<FdoExpression> expandedExpression = FdoExpressionEngineCopyFilter::Copy(expr, idList);
                                FdoExpressionEngine::GetExpressionType(originalClassDef, expandedExpression, retPropType, retDataType);
                                if (retPropType == FdoPropertyType_DataProperty)
                                {
                                    STRING namePropStr = STRING(nameExpr.c_str());
                                    Ptr<MgDataPropertyDefinition> propDefExpr = new MgDataPropertyDefinition(namePropStr);
                                    propDefExpr->SetDataType(MgFeatureUtil::GetMgPropertyType(retDataType));
                                    propDefExpr->SetNullable(true);
                                    propDefExpr->SetReadOnly(true);
                                    propDefExpr->SetAutoGeneration(false);
                                    mpdcLocal->Add(propDefExpr);
                                }
                            }
                        }
                    }
                }
                //
                // Finished adding primary class properties to the extension class definition
                // Now add the secondary class properties
                //

                // Determine the number of secondary sources (AttributeRelates)
                MdfModel::AttributeRelateCollection* attributeRelates = extension->GetAttributeRelates();
                CHECKNULL(attributeRelates, L"MgDescribeSchema.DescribeSchema");
                int nAttributeRelateCount = attributeRelates->GetCount();

                for (int arIndex = 0; arIndex < nAttributeRelateCount; arIndex++)
                {
                    // get the attribute relate
                    MdfModel::AttributeRelate* attributeRelate = attributeRelates->GetAt(arIndex);
                    CHECKNULL(attributeRelate, L"MgDescribeSchema.DescribeSchema");

                    // Get the name of the secondary feature class (AttributeClass)
                    STRING attributeClass = (STRING)attributeRelate->GetAttributeClass();

                    // Parse the schema name from the class name;
                    STRING secSchemaName, secClassName;
                    MgUtil::ParseQualifiedClassName(attributeClass, secSchemaName, secClassName);

                    // Get the relation name
                    STRING relationName = (STRING)attributeRelate->GetName();

                    // Get the attributeName delimiter ( if none specified, default will be "" (blank) )
                    STRING attributeNameDelimiter = (STRING)attributeRelate->GetAttributeNameDelimiter();

                    // Get the resource id of the secondary feature source
                    STRING secondaryResourceId = (STRING)attributeRelate->GetResourceId();

                    // Establish connection to provider for secondary feature source
                    Ptr<MgResourceIdentifier> secondaryFeatureSource = new MgResourceIdentifier(secondaryResourceId);
                    if (NULL != secondaryFeatureSource)
                    {
                        FdoPtr<FdoFeatureSchemaCollection> ffsc2;
                        Ptr<MgFeatureConnection> connection2 = new MgFeatureConnection(secondaryFeatureSource);

                        if ((NULL != connection2.p) && ( connection2->IsConnectionOpen() ))
                        {
                            // The reference to the FDO connection from the MgFeatureConnection object must be cleaned up before the parent object
                            // otherwise it leaves the FDO connection marked as still in use.
                            FdoPtr<FdoIConnection> fdoConn2 = connection2->GetConnection();
                            // Get the schema collection for the secondary resource
                            FdoPtr<FdoIDescribeSchema> fdoCommand2  = (FdoIDescribeSchema*)fdoConn2->CreateCommand(FdoCommandType_DescribeSchema);
                            CHECKNULL((FdoIDescribeSchema*)fdoCommand2, L"MgDescribeSchema.DescribeSchema");

                            if (!secSchemaName.empty())
                            {
                                fdoCommand2->SetSchemaName(secSchemaName.c_str());
                            }

                            if (!secClassName.empty())
                            {
                                FdoPtr<FdoStringCollection> fdoClassNames2 = FdoStringCollection::Create();

                                fdoClassNames2->Add(secClassName.c_str());
                                fdoCommand2->SetClassNames(fdoClassNames2.p);
                            }

                            // Execute the command
                            ffsc2 = fdoCommand2->Execute();
                            CHECKNULL((FdoFeatureSchemaCollection*)ffsc2, L"MgdFeatureService::DescribeSchema");

                            int nSecSchemaCnt = (int)ffsc2->GetCount();

                            // cycle thru FdoFeatureSchemaCollection for secSchemaName
                            for (int nSecSchemaIndex = 0; nSecSchemaIndex < nSecSchemaCnt; nSecSchemaIndex++)
                            {
                                // retrieve the schema
                                FdoPtr<FdoFeatureSchema> ffs = ffsc2->GetItem(nSecSchemaIndex);
                                STRING fdoSchemaName = (wchar_t*)ffs->GetName();

                                if (fdoSchemaName != secSchemaName)
                                {
                                    continue;
                                }

                                // get the class collection for schema
                                FdoPtr<FdoClassCollection> fcc = ffs->GetClasses();
                                FdoInt32 classCount = fcc->GetCount();

                                // cycle thru class collection for secClassName
                                for (int nClassIndex = 0; nClassIndex < classCount; nClassIndex++)
                                {
                                    Ptr<MgPropertyDefinitionCollection> mpdc = extClassDefinition->GetProperties();

                                    FdoPtr<FdoClassDefinition> fc = fcc->GetItem(nClassIndex);

                                    STRING qualifiedName = (const wchar_t*)fc->GetQualifiedName();
                                    STRING className = (wchar_t*)fc->GetName();

                                    if (className != secClassName)
                                    {
                                        continue;
                                    }

                                    // Set the parent name for the secondary class definition
                                    FdoPtr<FdoSchemaElement> parent = fc->GetParent();
                                    if (!secSchemaName.empty())
                                    {
                                        parent->SetName(secSchemaName.c_str());
                                    }

                                    // get the secondary class definition
                                    Ptr<MgClassDefinition> classDefinition = MgFeatureUtil::GetMgClassDefinition(fc, serialize);

                                    // retrieve the secondary properties and prefix them with the relation name
                                    Ptr<MgPropertyDefinitionCollection> mpdc2 = classDefinition->GetProperties();
                                    INT32 mpdc2Count = mpdc2->GetCount();

                                    // Prefix the secondary properties with relationName and add to the extClassDefinition
                                    for (INT32 secPropIndex = 0; secPropIndex < mpdc2Count; secPropIndex++)
                                    {
                                        Ptr<MgPropertyDefinition> propDef = mpdc2->GetItem(secPropIndex);
                                        if ( MgFeaturePropertyType::GeometricProperty != propDef->GetPropertyType()
                                            && MgFeaturePropertyType::RasterProperty != propDef->GetPropertyType() )
                                        {
                                            STRING secPropName = propDef->GetName();
                                            secPropName = relationName + attributeNameDelimiter + secPropName;
                                            propDef->SetName(secPropName);
                                            mpdc->Add(propDef);
                                        }
                                    }

                                    break;

                                }  // end loop thru secondary class collection

                                break;

                            }  // end loop thru secondary schemas
                        }
                        else
                        {
                            throw new MgConnectionFailedException(L"MgdFeatureService::DescribeSchema", __LINE__, __WFILE__, NULL, L"", NULL);
                        }

                    }  // end if (NULL != secFeatureSource)

                }  // end loop thru all attribute relates (joins)

                if (!extensionName.empty())
                {
                    extClassDefinition->SetName(extensionName);
                }

                // Add the extension class definition to the MgClassDefinitionCollection
                classCol->Add(extClassDefinition);

            }  // Repeat for all extensions

            // Add the schema to the MgFeatureSchemaCollection
            fsCollection->Add(schema);

        }  // End loop thru all schemas

        cache->SetSchemas(resource, schemaName, classNames, serialize, fsCollection.p);
    }
    else
    {
        //m_cacheManager->CheckPermission(resource, MgResourcePermission::ReadOnly);
    }

    //Clone this result and return it
    clone = new MgFeatureSchemaCollection();
    for (INT32 i = 0; i < fsCollection->GetCount(); i++)
    {
        Ptr<MgFeatureSchema> schema = fsCollection->GetItem(i);
        Ptr<MgFeatureSchema> clonedSchema = MgFeatureUtil::CloneMgSchema(schema);
        clone->Add(clonedSchema);
    }

    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH_AND_THROW(resource, L"MgdFeatureService::DescribeSchema")

    return clone.Detach();
}

FdoFeatureSchemaCollection* MgdFeatureService::DescribeFdoSchema(MgResourceIdentifier* resource,
                                                                 CREFSTRING schemaName, 
                                                                 MgStringCollection* classNames, 
                                                                 bool& classNameHintUsed)
{
    // IMPORTANT INFORMATION
    // FDO objects cannot be cached as they are not thread safe.
    // This is not an issue because we create a MapGuide wrapper of this information that is thread safe.
    FdoPtr<FdoFeatureSchemaCollection> ffsc;
    ffsc = NULL;

    MG_FEATURE_SERVICE_TRY()

    // Connect to provider
    Ptr<MgFeatureConnection> connection = new MgFeatureConnection(resource);

    if ((NULL != connection.p) && (connection->IsConnectionOpen()))
    {
        MgFeatureServiceCache* cache = MgFeatureServiceCache::GetInstance();

        // The reference to the FDO connection from the MgFeatureConnection object must be cleaned up before the parent object
        // otherwise it leaves the FDO connection marked as still in use.
        FdoPtr<FdoIConnection> fdoConn = connection->GetConnection();
        FdoPtr<FdoIDescribeSchema> fdoCommand = (FdoIDescribeSchema*)fdoConn->CreateCommand(FdoCommandType_DescribeSchema);

        classNameHintUsed = IsClassNameHintUsed(fdoCommand);

        if (!schemaName.empty())
        {
            fdoCommand->SetSchemaName(schemaName.c_str());
        }

        FdoPtr<FdoStringCollection> fdoClassNames = MgFeatureUtil::MgToFdoStringCollection(classNames, false);

        if (NULL != fdoClassNames.p && fdoClassNames->GetCount() > 0)
        {
            fdoCommand->SetClassNames(fdoClassNames.p);
        }

        // Execute the command
        ffsc = fdoCommand->Execute();
        CHECKNULL((FdoFeatureSchemaCollection*)ffsc, L"MgdFeatureService::DescribeFdoSchema");

        // Finished with primary feature source, so now cycle through any secondary sources
        Ptr<MgFeatureSourceCacheItem> fsCache = cache->GetFeatureSource(resource);

        MdfModel::FeatureSource* featureSource = fsCache->Get();
        CHECKNULL(featureSource, L"MgdFeatureService::DescribeFdoSchema");
        MdfModel::ExtensionCollection* extensions = featureSource->GetExtensions();
        CHECKNULL(extensions, L"MgdFeatureService::DescribeFdoSchema");
        int extensionCount = extensions->GetCount();

        for (int i = 0; i < extensionCount; i++)
        {
            MdfModel::Extension* extension = extensions->GetAt(i);
            CHECKNULL(extension, L"MgdFeatureService::DescribeFdoSchema");

            // Get the extension name
            STRING extensionName = (STRING)extension->GetName();

            // Determine the number of secondary sources (AttributeRelates)
            MdfModel::AttributeRelateCollection* attributeRelates = extension->GetAttributeRelates();
            CHECKNULL(attributeRelates, L"MgdFeatureService::DescribeFdoSchema");
            int nAttributeRelates = attributeRelates->GetCount();

            for (int arIndex = 0; arIndex < nAttributeRelates; arIndex++)
            {
                MdfModel::AttributeRelate* attributeRelate = attributeRelates->GetAt(arIndex);
                CHECKNULL(attributeRelate, L"MgdFeatureService::DescribeFdoSchema");

                // get the resource id of the secondary feature source
                STRING secondaryResourceId = (STRING)attributeRelate->GetResourceId();

                // get the name for the join relationship (attribute relate name)
                STRING attributeRelateName = (STRING)attributeRelate->GetName();

                // Get the secondary feature class (AttributeClass)
                STRING attributeClass = (STRING)attributeRelate->GetAttributeClass();

                // Parse the schema name from the classname;
                STRING secSchemaName, secClassName;
                MgUtil::ParseQualifiedClassName(attributeClass, secSchemaName, secClassName);

                // Establish connection to provider for secondary feature source
                Ptr<MgResourceIdentifier> secondaryFeatureSource = new MgResourceIdentifier(secondaryResourceId);

                if (NULL != secondaryFeatureSource)
                {
                    FdoPtr<FdoFeatureSchemaCollection> ffsc2;
                    Ptr<MgFeatureConnection> connection2 = new MgFeatureConnection(secondaryFeatureSource);

                    if ((NULL != connection2.p) && ( connection2->IsConnectionOpen() ))
                    {
                        // The reference to the FDO connection from the MgFeatureConnection object must be cleaned up before the parent object
                        // otherwise it leaves the FDO connection marked as still in use.
                        FdoPtr<FdoIConnection> fdoConn2 = connection2->GetConnection();
                        // Check whether this command is supported by the provider
                        FdoPtr<FdoIDescribeSchema> fdoCommand2 = (FdoIDescribeSchema*)fdoConn2->CreateCommand(FdoCommandType_DescribeSchema);
                        CHECKNULL((FdoIDescribeSchema*)fdoCommand2, L"MgDescribeSchema.DescribeFdoSchema");

                        if (!secSchemaName.empty())
                        {
                            fdoCommand2->SetSchemaName(secSchemaName.c_str());
                        }

                        if (!secClassName.empty())
                        {
                            FdoPtr<FdoStringCollection> fdoClassNames2 = FdoStringCollection::Create();

                            fdoClassNames2->Add(secClassName.c_str());
                            fdoCommand2->SetClassNames(fdoClassNames2.p);
                        }

                        // Execute the command
                        ffsc2 = fdoCommand2->Execute();
                        CHECKNULL((FdoFeatureSchemaCollection*)ffsc2, L"MgdFeatureService::DescribeFdoSchema");

                        if (!secSchemaName.empty())
                        {
                            fdoCommand2->SetSchemaName(secSchemaName.c_str());
                        }

                        if (!secClassName.empty())
                        {
                            FdoPtr<FdoStringCollection> fdoClassNames2 = FdoStringCollection::Create();

                            fdoClassNames2->Add(secClassName.c_str());
                            fdoCommand2->SetClassNames(fdoClassNames2.p);
                        }

                        // Extract the schemas from the secondary collection and add them to the main collection
                        // Get schema count
                        FdoInt32 cnt = ffsc2->GetCount();
                        for (FdoInt32 i = 0; i < cnt; i++)
                        {
                            FdoPtr<FdoFeatureSchema> ffs = ffsc2->GetItem(i);
                            STRING fdoSchemaName = (wchar_t*)ffs->GetName();

                            if (fdoSchemaName != secSchemaName)
                            {
                                continue;
                            }

                            // Prefix the schema name with the extension and attribute relate names
                            STRING modifiedSchemaName;
                            modifiedSchemaName = L"[" + extensionName + L"]";
                            modifiedSchemaName += L"[" + attributeRelateName + L"]";
                            modifiedSchemaName += fdoSchemaName;
                            FdoString* msn = modifiedSchemaName.c_str();
                            ffs->SetName(msn);

                            // Add this schema to the collection if it isn't already there
                            if (!ffsc->Contains(ffs))
                            {
                                ffsc->Add(ffs);
                            }
                        }
                    }
                    else
                    {
                        throw new MgConnectionFailedException(L"MgdFeatureService::DescribeFdoSchema", __LINE__, __WFILE__, NULL, L"", NULL);
                    }
                }

            }  // End of the for-loop that iterates thru the secondary sources

        }  // End of for loop that iterates thru the extensions in the feature source
    }
    else
    {
        throw new MgConnectionFailedException(L"MgdFeatureService::DescribeFdoSchema", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH_AND_THROW(resource, L"MgdFeatureService::DescribeFdoSchema")

    return ffsc.Detach();
}

bool MgdFeatureService::IsClassNameHintUsed(FdoIDescribeSchema* fdoCommand)
{
    CHECKNULL(fdoCommand, L"MgdFeatureService::IsClassNameHintUsed");

    FdoPtr<FdoStringCollection> classNames = fdoCommand->GetClassNames();
    bool classNameHintUsed = (NULL != classNames.p);

    return classNameHintUsed;
}

MgFeatureSchemaCollection* MgdFeatureService::DescribeSchema(MgResourceIdentifier* resource,
                                                             CREFSTRING schemaName) 
{ 
    Ptr<MgFeatureSchemaCollection> ret;
    MG_FEATURE_SERVICE_TRY()
	ret = DescribeSchema(resource, schemaName, NULL);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureService::DescribeSchema")
    return ret.Detach();
}

STRING MgdFeatureService::DescribeSchemaAsXml(MgResourceIdentifier* resource,
                                              CREFSTRING schemaName,
                                              MgStringCollection* classNames) 
{ 
	CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdFeatureService::DescribeSchemaAsXml");

    STRING schemaXml;

    MG_FEATURE_SERVICE_TRY()

    MgFeatureServiceCache* cache = MgFeatureServiceCache::GetInstance();
    schemaXml = cache->GetSchemaXml(resource, schemaName, classNames);

    if (schemaXml.empty())
    {
        MgStringCollection* classNameCol = NULL;

        // Since the FDO provider does not know about the join information,
        // get the full schema if the feature source has joins.
        if (CheckExtendedFeatureClasses(resource, classNames))
        {
            schemaXml = cache->GetSchemaXml(resource, schemaName, NULL);
        }
        else
        {
            classNameCol = classNames;
        }

        if (schemaXml.empty())
        {
            // The schema XML can be retrieved from either the serialized
            // schemas or the unserialized ones. So, try to get the serialized
            // schemas from the cache first then the unserialized ones later.
            Ptr<MgFeatureSchemaCollection> schemas = cache->GetSchemas(resource, schemaName, classNameCol, true);

            if (NULL == schemas.p)
            {
                schemas = DescribeSchema(resource, schemaName, classNameCol, false);
            }
            else
            {
                //cache->CheckPermission(resource, MgResourcePermission::ReadOnly);
            }

            // Get the schema XML.
            schemaXml = SchemaToXml(schemas);

            cache->SetSchemaXml(resource, schemaName, classNameCol, schemaXml);
        }
        else
        {
            //cache->CheckPermission(resource, MgResourcePermission::ReadOnly);
        }
    }
    else
    {
        //cache->CheckPermission(resource, MgResourcePermission::ReadOnly);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureService::DescribeSchemaAsXml")

    return schemaXml;
}

STRING MgdFeatureService::DescribeSchemaAsXml(MgResourceIdentifier* resource,
                                              CREFSTRING schemaName) 
{ 
    STRING ret;
    MG_FEATURE_SERVICE_TRY()
	ret = DescribeSchemaAsXml(resource, schemaName, NULL);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureService::DescribeSchemaAsXml")
    return ret;
}

bool MgdFeatureService::CheckExtendedFeatureClasses(MgResourceIdentifier* resource, MgStringCollection* classNames)
{
    bool extended = false;

    if (NULL != classNames)
    {
        INT32 classCount = classNames->GetCount();

        for (INT32 i = 0; i < classCount; ++i)
        {
            STRING currClassName = classNames->GetItem(i);

            if (CheckExtendedFeatureClass(resource, currClassName))
            {
                extended = true;
                break;
            }
        }
    }

    return extended;
}

bool MgdFeatureService::CheckExtendedFeatureClass(MgResourceIdentifier* resource, CREFSTRING className)
{
    bool extended = false;

    if (!className.empty())
    {
        STRING parsedSchemaName, parsedClassName;
        MgUtil::ParseQualifiedClassName(className, parsedSchemaName, parsedClassName);

        MgFeatureServiceCache* cache = MgFeatureServiceCache::GetInstance();

        Ptr<MgFeatureSourceCacheItem> cacheItem = cache->GetFeatureSource(resource);

        MdfModel::FeatureSource* featureSource = cacheItem->Get();
        CHECKNULL(featureSource, L"MgdFeatureService::CheckExtendedFeatureClass");
        MdfModel::ExtensionCollection* extensions = featureSource->GetExtensions();
        CHECKNULL(extensions, L"MgdFeatureService::CheckExtendedFeatureClass");
        int extensionCount = extensions->GetCount();

        for (int i = 0; i < extensionCount; ++i)
        {
            MdfModel::Extension* extension = extensions->GetAt(i);
            CHECKNULL(extension, L"MgdFeatureService::CheckExtendedFeatureClass");
            STRING extensionName = (STRING)extension->GetName();

            STRING currSchemaName, currClassName;
            MgUtil::ParseQualifiedClassName(extensionName, currSchemaName, currClassName);

            if (currClassName == parsedClassName)
            {
                extended = true;
                break;
            }
        }
    }

    return extended;
}

STRING MgdFeatureService::FdoSchemaToXml(FdoFeatureSchemaCollection* schemas)
{
	STRING serializedXml;

    MG_FEATURE_SERVICE_TRY()
    CHECKARGUMENTNULL(schemas, L"MgdFeatureService::FdoSchemaToXml");

    FdoIoMemoryStreamP fmis = FdoIoMemoryStream::Create();
    CHECKARGUMENTNULL((FdoIoMemoryStream*)fmis, L"MgdFeatureService::FdoSchemaToXml");

    // Write to memory stream
    schemas->WriteXml(fmis);
    fmis->Reset(); // TODO: We should not be calling reset here. A defect in FDO should be fixed.

    FdoInt64 len = fmis->GetLength();
    FdoByte *bytes = new FdoByte[(size_t)len];
    CHECKARGUMENTNULL(bytes, L"MgdFeatureService::FdoSchemaToXml");

    fmis->Read(bytes, (FdoSize)len);

    Ptr<MgByteSource> byteSource = new MgByteSource((BYTE_ARRAY_IN)bytes, (INT32)len);
    byteSource->SetMimeType(MgMimeType::Xml);
    Ptr<MgByteReader> byteReader = byteSource->GetReader();

    string out = MgUtil::GetTextFromReader(byteReader);
    serializedXml = MgUtil::MultiByteToWideChar(out);

    delete [] bytes;

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureService::FdoSchemaToXml")

    return serializedXml;
}

STRING MgdFeatureService::SchemaToXml(MgFeatureSchemaCollection* schemas) 
{ 
    CHECKARGUMENTNULL(schemas, L"MgdFeatureService::SchemaToXml");
    STRING serializedXml;

    MG_FEATURE_SERVICE_TRY()

    FdoPtr<FdoFeatureSchemaCollection> fdoSchemaCol = MgFeatureUtil::GetFdoFeatureSchemaCollection(schemas);
    CHECKNULL(fdoSchemaCol, L"MgdFeatureService::SchemaToXml");

    FdoIoMemoryStreamP fmis = FdoIoMemoryStream::Create();
    CHECKNULL((FdoIoMemoryStream*)fmis, L"MgdFeatureService::SchemaToXml");

    // Write to memory stream
    fdoSchemaCol->WriteXml(fmis);
    fmis->Reset(); // TODO: We should not be calling reset here. A defect in FDO should be fixed.

    FdoInt64 len = fmis->GetLength();
    FdoByte *bytes = new FdoByte[(size_t)len];
    CHECKNULL(bytes, L"MgdFeatureService::SchemaToXml");

    fmis->Read(bytes, (FdoSize)len);

    Ptr<MgByteSource> byteSource = new MgByteSource((BYTE_ARRAY_IN)bytes, (INT32)len);
    byteSource->SetMimeType(MgMimeType::Xml);
    Ptr<MgByteReader> byteReader = byteSource->GetReader();

    string out = MgUtil::GetTextFromReader(byteReader);
    serializedXml = MgUtil::MultiByteToWideChar(out);

    delete [] bytes;

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureService::SchemaToXml")

    return serializedXml;
}

MgFeatureSchemaCollection* MgdFeatureService::XmlToSchema(CREFSTRING xml) 
{ 
	Ptr<MgFeatureSchemaCollection> mgSchemaCol;

    MG_FEATURE_SERVICE_TRY()
    mgSchemaCol = new MgFeatureSchemaCollection();

    string mbString = MgUtil::WideCharToMultiByte(xml);

    size_t len = mbString.length();

    FdoByte* gisBytes = (FdoByte*) mbString.c_str();

    FdoPtr<FdoIoMemoryStream> stream = FdoIoMemoryStream::Create();
    stream->Write(gisBytes, (FdoSize)len);

    FdoPtr<FdoFeatureSchemaCollection> fdoSchemaCol = FdoFeatureSchemaCollection::Create((FdoSchemaElement*)NULL);
    stream->Reset();
    fdoSchemaCol->ReadXml(stream);

    // Get schema count
    FdoInt32 cnt = fdoSchemaCol->GetCount();
    for (FdoInt32 i = 0; i < cnt; i++)
    {
        FdoPtr<FdoFeatureSchema> fdoSchema = fdoSchemaCol->GetItem(i);
        FdoStringP name = fdoSchema->GetName();
        CHECKNULL(name, L"MgdFeatureService::XmlToSchema");

        FdoStringP description = fdoSchema->GetDescription();

        STRING tmpName(name);
        STRING tmpDesc(description);

        Ptr<MgFeatureSchema> mgSchema = new MgFeatureSchema(tmpName, tmpDesc);
        Ptr<MgClassDefinitionCollection> classCol = mgSchema->GetClasses();

        // Get all classes for a schema
        FdoPtr<FdoClassCollection> fdoClassCol = fdoSchema->GetClasses();
        FdoInt32 classCount = fdoClassCol->GetCount();

        for (FdoInt32 j = 0; j < classCount; j++)
        {
            FdoPtr<FdoClassDefinition> fdoClassDef = fdoClassCol->GetItem(j);
            // TODO: Should we return qualified or non-qualified name
            FdoStringP qname = fdoClassDef->GetQualifiedName();
            FdoStringP name = fdoClassDef->GetName();

            if (name != NULL && qname != NULL)
            {
                Ptr<MgClassDefinition> classDefinition = MgFeatureUtil::GetMgClassDefinition(fdoClassDef, true);
                classCol->Add(classDefinition);
            }
        }
        mgSchemaCol->Add(mgSchema);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureService::XmlToSchema")

    return mgSchemaCol.Detach();
}

MgFeatureReader* MgdFeatureService::SelectFeatures(MgResourceIdentifier* resource,
                                                   CREFSTRING className,
                                                   MgFeatureQueryOptions* options) 
{ 
    Ptr<MgFeatureReader> ret;
    MG_FEATURE_SERVICE_TRY()
	ret = SelectFeatures(resource, className, options, L"");
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureService::SelectFeatures")
    return ret.Detach();
}

MgFeatureReader* MgdFeatureService::SelectFeatures(MgResourceIdentifier* resource,
                                                   CREFSTRING className,
                                                   MgFeatureQueryOptions* options,
                                                   CREFSTRING coordinateSystem) 
{ 
    Ptr<MgFeatureReader> ret;
    MG_FEATURE_SERVICE_TRY()
	ret = SelectFeaturesInternal(resource, className, options, coordinateSystem, false, false);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureService::SelectFeatures")
    return ret.Detach();
}

MgdScrollableFeatureReader* MgdFeatureService::SelectFeaturesExtended(MgResourceIdentifier* resource,
                                                                      CREFSTRING className,
                                                                      MgFeatureQueryOptions* options)
{
    Ptr<MgdScrollableFeatureReader> ret;
    MG_FEATURE_SERVICE_TRY()
    ret = static_cast<MgdScrollableFeatureReader*>(SelectFeaturesInternal(resource, className, options, L"", false, true));
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureService::SelectFeaturesExtended")
    return ret.Detach();
}

MgFeatureReader* MgdFeatureService::SelectFeaturesInternal(MgResourceIdentifier* resource,
														   CREFSTRING className,
														   MgFeatureQueryOptions* options,
														   CREFSTRING coordinateSystem,
														   bool withLock,
                                                           bool asScrollable)
{
	CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdFeatureService::SelectFeatures");
	//CHECKARGUMENTNULL(options, L"MgdFeatureService::SelectFeatures");
    if (className.empty())
        throw new MgInvalidArgumentException(L"MgdFeatureService::SelectFeatures", __LINE__, __WFILE__, NULL, L"", NULL);

    //Cannot both be true at the same time
    assert(!(withLock && asScrollable));

    Ptr<MgFeatureReader> reader;
    MG_FEATURE_SERVICE_TRY()
    
	Ptr<MgFeatureConnection> connWrap = new MgFeatureConnection(resource);
    {
        FdoPtr<FdoIConnection> conn = connWrap->GetConnection();

        FdoPtr<FdoISelect> select;
        if (asScrollable)
            select = (FdoIExtendedSelect*)conn->CreateCommand(FdoCommandType_ExtendedSelect);
        else
            select = (FdoISelect*)conn->CreateCommand(FdoCommandType_Select);
        
        if (NULL != (MgFeatureQueryOptions*)options)
        {
            Ptr<MgStringCollection> props = options->GetClassProperties();
            Ptr<MgStringPropertyCollection> computed = options->GetComputedProperties();
            Ptr<MgStringCollection> orderByProperties = options->GetOrderingProperties();
            INT32 orderOption = options->GetOrderOption();

            INT32 propCount = props->GetCount();
            INT32 compCount = computed->GetCount();
            INT32 orderCount = 0;
            if (NULL != (MgStringCollection*)orderByProperties)
            {
                orderCount = orderByProperties->GetCount();
            }

            FdoPtr<FdoIdentifierCollection> fdoProps = select->GetPropertyNames();
            if (propCount > 0)
            {
	            for (INT32 i = 0; i < propCount; i++)
	            {
		            FdoPtr<FdoIdentifier> name = FdoIdentifier::Create(props->GetItem(i).c_str());
		            fdoProps->Add(name);
	            }
            }
            if (compCount > 0)
            {
	            for (INT32 i = 0; i < compCount; i++)
	            {
		            Ptr<MgStringProperty> comp = computed->GetItem(i);
		            FdoPtr<FdoExpression> expr = FdoExpression::Parse(comp->GetValue().c_str());
		            FdoPtr<FdoComputedIdentifier> name= FdoComputedIdentifier::Create(comp->GetName().c_str(), expr);

		            fdoProps->Add(name);
	            }
            }
            FdoPtr<FdoIdentifierCollection> orderBy = select->GetOrdering();
            if (orderCount > 0)
            {
                for (INT32 i = 0; i < orderCount; i++)
	            {
		            FdoPtr<FdoIdentifier> name = FdoIdentifier::Create(orderByProperties->GetItem(i).c_str());
		            orderBy->Add(name);
	            }
                switch(orderOption)
                {
                case MgOrderingOption::Ascending:
                    select->SetOrderingOption(FdoOrderingOption_Ascending);
                    break;
                case MgOrderingOption::Descending:
                    select->SetOrderingOption(FdoOrderingOption_Descending);
                    break;
                }
            }

            FdoPtr<FdoFilter> regularFilter;
            FdoPtr<FdoFilter> spatialFilter;
            FdoPtr<FdoFilter> combineFilter;
            FdoBinaryLogicalOperations bOper = FdoBinaryLogicalOperations_And;

            // Build regular filter
            STRING filterText = options->GetFilter();
            if (!filterText.empty())
            {
                regularFilter = FdoFilter::Parse(filterText.c_str());
                #ifdef _DEBUG
                ACE_DEBUG((LM_ERROR, ACE_TEXT("FILTER(size=%d):\n%W\n\n"), filterText.length(), filterText.c_str()));
                #endif
            }

            // Build spatial filter
            bool isAnd = options->GetBinaryOperator();
            Ptr<MgGeometry> geom = options->GetGeometry();
            INT32 spatialOp = options->GetSpatialOperation();

            STRING geomProp = options->GetGeometryProperty();

            if (!isAnd) // Apply OR operator
            {
                bOper = FdoBinaryLogicalOperations_Or;
            }

            // Check whether we have valid geometric filter
            if (!geomProp.empty() && (geom != NULL))
            {
                MgAgfReaderWriter agfWriter;

                Ptr<MgByteReader> reader = agfWriter.Write(geom);
                Ptr<MgByteSink> sink = new MgByteSink(reader);
                Ptr<MgByte> bytes = sink->ToBuffer();

                FdoByte* gisBytes = bytes->Bytes();
                INT32 len = bytes->GetLength();

                FdoPtr<FdoByteArray> byteArray = FdoByteArray::Create(gisBytes, (FdoInt32)len);

                #ifdef _DEBUG
                // Get the spatial filter geometry text
                FdoPtr<FdoFgfGeometryFactory> geometryFactory = FdoFgfGeometryFactory::GetInstance();
                if(geometryFactory)
                {
                    FdoPtr<FdoIGeometry> geometry = geometryFactory->CreateGeometryFromFgf(byteArray);
                    STRING geomText = geometry->GetText();
                    ACE_DEBUG((LM_INFO, ACE_TEXT("(%t) SPATIAL FILTER:\n%W\n\n"), geomText.c_str()));
                }
                #endif

                FdoPtr<FdoGeometryValue> geomValue = FdoGeometryValue::Create(byteArray);
                if (geomValue != NULL)
                {
                    FdoSpatialOperations fdoSpatialOp = MgFeatureUtil::GetFdoSpatialOperation(spatialOp);
                    spatialFilter = FdoSpatialCondition::Create((FdoString*)geomProp.c_str(), fdoSpatialOp, (FdoExpression*)geomValue);
                }
            }

            // Determine which one to apply
            if ((regularFilter != NULL) && (spatialFilter != NULL))
            {
                // Both filter exists, combine them
                combineFilter = FdoFilter::Combine(regularFilter, bOper, spatialFilter);
            }
            else
            {
                if (regularFilter != NULL)
                {
                    // Only regular filter exists
                    combineFilter = FDO_SAFE_ADDREF((FdoFilter*)regularFilter);
                }
                else if (spatialFilter != NULL)
                {
                    // Only spatial filter exists
                    combineFilter = FDO_SAFE_ADDREF((FdoFilter*)spatialFilter);
                }
            }

            // Apply the filter
            if (combineFilter != NULL)
            {
                select->SetFilter(combineFilter);
            }

            if (asScrollable)
            {
                //TODO: Apply single property ordering option. Obviously then needs its own custom query options object
            }
        }
        // Check if a feature join is to be performed by inspecting the resource for join properties
        bool bFeatureJoinProperties = FindFeatureJoinProperties(resource, className);
        // Check if a feature join is only a calculation
        bool bFeatureCalculation = FindFeatureCalculation(resource, className);

        if (bFeatureJoinProperties)
        {
            FdoPtr<FdoFilter> fdoFilter = select->GetFilter();
            reader = SelectFeaturesJoined(resource, className, fdoFilter);
        }
        else
        {
            select->SetFeatureClassName(className.c_str());
            if (asScrollable)
            {
                FdoPtr<FdoIScrollableFeatureReader> scrollReader;
                FdoIExtendedSelect* extSelect = static_cast<FdoIExtendedSelect*>(select.p);
                scrollReader = extSelect->ExecuteScrollable();

                reader = new MgdScrollableFeatureReader(connWrap, scrollReader);
            }
            else
            {
                FdoPtr<FdoIFeatureReader> fdoReader;
                if (withLock)
	                fdoReader = select->ExecuteWithLock();
                else
	                fdoReader = select->Execute();
                
                //If coordinate system specified, set up a CS Transform to pass to the feature reader
                Ptr<MgCoordinateSystemTransform> xform;
                if (!coordinateSystem.empty())
                {
                    FdoPtr<FdoFeatureClass> clsDef = dynamic_cast<FdoFeatureClass*>(fdoReader->GetClassDefinition());
                    if (NULL != clsDef)
                    {
                        FdoPtr<FdoGeometricPropertyDefinition> geom = clsDef->GetGeometryProperty();
                        if (NULL != geom)
                        {
                            FdoString* scName = geom->GetSpatialContextAssociation();
                            FdoPtr<FdoIGetSpatialContexts> gsc = (FdoIGetSpatialContexts*)conn->CreateCommand(FdoCommandType_GetSpatialContexts);
                            FdoPtr<FdoISpatialContextReader> scReader = gsc->Execute();
                            while (scReader->ReadNext())
                            {
                                if (wcscmp(scReader->GetName(), scName) == 0)
                                {
                                    //TODO: Some FDO providers don't even use WKTs for spatial context. Capabilities
                                    //should tell us the correct way to construct our MgCoordinateSystem object

                                    STRING wkt = scReader->GetCoordinateSystemWkt();

                                    Ptr<MgCoordinateSystemFactory> fact = new MgCoordinateSystemFactory();

                                    Ptr<MgCoordinateSystem> source = fact->Create(wkt);
                                    Ptr<MgCoordinateSystem> target = fact->Create(coordinateSystem);

                                    xform = fact->GetTransform(source, target);
                                    break;
                                }
                            }
                        }
                    }
                }

                if (NULL == xform)
                {
                    reader = new MgdFeatureReader(connWrap, fdoReader);   
                }
                else
                {
                    //Wrap the feature reader in a projected feature reader
                    Ptr<MgFeatureReader> fr = new MgdFeatureReader(connWrap, fdoReader);
                    reader = new MgProjectedFeatureReader(fr, xform);
                }
            }
        }
    }

    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH_AND_THROW(resource, L"MgdFeatureService::SelectFeaturesInternal")

	return reader.Detach();
}

MgFeatureReader* MgdFeatureService::SelectFeaturesJoined(MgResourceIdentifier* featureSourceIdentifier, 
                                                         CREFSTRING extensionName, 
                                                         FdoFilter* filter)
{
    Ptr<MgdGwsFeatureReader> gwsFeatureReader;

    MG_FEATURE_SERVICE_TRY()

    FdoPtr<IGWSQueryDefinition> qd;
    FdoPtr<MgGwsConnectionPool> pool = MgGwsConnectionPool::Create();

    MgFeatureServiceCache* cacheManager = MgFeatureServiceCache::GetInstance();
    Ptr<MgFeatureSourceCacheItem> fsCacheItem = cacheManager->GetFeatureSource(featureSourceIdentifier);

    CHECKNULL(fsCacheItem.p, L"MgdFeatureService::JoinFeatures");
    MdfModel::FeatureSource* featureSource = fsCacheItem->Get();
    MdfModel::ExtensionCollection* extensions = featureSource->GetExtensions();
    CHECKNULL(extensions, L"MgdFeatureService::JoinFeatures");

    for (int i = 0; i < extensions->GetCount(); i++)
    {
        MdfModel::Extension* extension = extensions->GetAt(i);
        CHECKNULL(extension, L"MgdFeatureService::JoinFeatures");
        STRING name = (STRING)extension->GetName();

        STRING parsedSchemaName, parsedExtensionName;
        MgUtil::ParseQualifiedClassName(extensionName, parsedSchemaName, parsedExtensionName);

        if (parsedExtensionName != name)
        {
            continue;
        }
        else
        {
            // Establish connection to provider for primary feature source
            STRING primaryConnectionName;
            MgUtil::GenerateUuid(primaryConnectionName);
            Ptr<MgFeatureConnection> leftConn = new MgFeatureConnection(featureSourceIdentifier);
            
            STRING fsIdStr = featureSourceIdentifier->ToString();
            // See if we can use the FDO join optimization
            if (SupportsFdoJoins(leftConn, extension))
            {
#ifdef DEBUG_FDOJOIN
                STRING fsIdStr = featureSourceIdentifier->ToString();
                ACE_DEBUG((LM_INFO, ACE_TEXT("\nFeature Source (%W) supports FDO join optimization"), fsIdStr.c_str()));
#endif
                Ptr<MgFeatureReader> joinReader = SelectFdoJoin(leftConn, extension, filter);
                return joinReader.Detach();
            }
#ifdef DEBUG_FDOJOIN
            ACE_DEBUG((LM_INFO, ACE_TEXT("\nFeature Source (%W) does not support the FDO join optimization. Using GwsQueryEngine"), fsIdStr.c_str()));
#endif


            if ((NULL != leftConn.p) && ( leftConn->IsConnectionOpen() ))
            {
                pool->AddConnection(primaryConnectionName.c_str(), leftConn);
            }
            else
            {
                throw new MgConnectionFailedException(L"MgdFeatureService::JoinFeatures",
                    __LINE__, __WFILE__, NULL, L"", NULL);
            }

            // Retrieve the primary feature class
            STRING featureClass = (STRING)extension->GetFeatureClass();

            // Parse the qualifed classname
            STRING primaryFsSchema, primaryFsClassName;
            MgUtil::ParseQualifiedClassName(featureClass, primaryFsSchema, primaryFsClassName);

            // Create primary query definition
            FdoPtr<FdoIdentifierCollection> lsellist;
            FdoPtr<FdoFilter> lfilter;

            // in case we have calculations we must add all properties + computed properties to the selection list
            MdfModel::CalculatedPropertyCollection* calcProps = extension->GetCalculatedProperties();
            if (calcProps != NULL && calcProps->GetCount() != 0)
            {
                FdoPtr<FdoIdentifierCollection> idList = FdoIdentifierCollection::Create();
                for (int idx = 0; idx < calcProps->GetCount(); idx++)
                {
                    CalculatedProperty* calcProp = calcProps->GetAt(idx);
                    FdoPtr<FdoExpression> expressionCalc = FdoExpression::Parse(calcProp->GetExpression().c_str());
                    FdoPtr<FdoComputedIdentifier> idfCalc = FdoComputedIdentifier::Create(calcProp->GetName().c_str(), expressionCalc);
                    idList->Add(idfCalc);
                }
                lsellist = FdoIdentifierCollection::Create();
                for (int idx = 0; idx < calcProps->GetCount(); idx++)
                {
                    CalculatedProperty* calcProp = calcProps->GetAt(idx);
                    FdoPtr<FdoComputedIdentifier> idfCalc = static_cast<FdoComputedIdentifier*>(idList->GetItem(calcProp->GetName().c_str()));
                    FdoPtr<FdoExpression> expressionCalc = idfCalc->GetExpression();
                    FdoPtr<FdoExpression> expandedExpression = FdoExpressionEngineCopyFilter::Copy(expressionCalc, idList);

                    FdoPtr<FdoComputedIdentifier> newIdf = FdoComputedIdentifier::Create(calcProp->GetName().c_str(), expandedExpression);
                    lsellist->Add(newIdf);
                }
                FdoPtr<FdoIConnection> conn = leftConn->GetConnection();
                FdoPtr<FdoIDescribeSchema>  descSchema = (FdoIDescribeSchema *) conn->CreateCommand (FdoCommandType_DescribeSchema);

                STRING fullClassName = extension->GetFeatureClass();
                STRING schemaName, className;
                MgUtil::ParseQualifiedClassName(fullClassName, schemaName, className);

                if (!parsedSchemaName.empty())
                {
                    descSchema->SetSchemaName(parsedSchemaName.c_str());
                }

                if (!className.empty())
                {
                    FdoPtr<FdoStringCollection> classNames = FdoStringCollection::Create();

                    classNames->Add(className.c_str());
                    descSchema->SetClassNames(classNames.p);
                }

                FdoPtr<FdoFeatureSchemaCollection> schemas = (FdoFeatureSchemaCollection *) descSchema->Execute ();
                FdoPtr<FdoFeatureSchema> schema = (FdoFeatureSchema *)schemas->GetItem (parsedSchemaName.c_str());
                FdoPtr<FdoClassCollection> classes = schema->GetClasses();

                FdoPtr<FdoClassDefinition> activeClass = classes->GetItem(className.c_str());
                FdoPtr<FdoPropertyDefinitionCollection> properties = activeClass->GetProperties();
                for(int i = 0; i < properties->GetCount(); i++)
                {
                    FdoPtr<FdoPropertyDefinition> activeProperty = properties->GetItem(i);
                    FdoPtr<FdoIdentifier> idf = FdoIdentifier::Create(activeProperty->GetName());
                    lsellist->Add(idf);
                }
                FdoPtr<FdoReadOnlyPropertyDefinitionCollection> baseProps = activeClass->GetBaseProperties();
                if (baseProps != NULL)
                {
                    for(int i = 0; i < baseProps->GetCount(); i++)
                    {
                        FdoPtr<FdoPropertyDefinition>prop = baseProps->GetItem(i);
                        if( prop->GetIsSystem() )
                            continue;
                        lsellist->Add( FdoPtr<FdoIdentifier>(FdoIdentifier::Create( prop->GetName() ) ) );
                    }
                }
            }

            FdoPtr<IGWSQueryDefinition> lqd = IGWSFeatureQueryDefinition::Create(
                lsellist,
                GWSQualifiedName(primaryConnectionName.c_str(), primaryFsSchema.c_str(), primaryFsClassName.c_str()),
                lfilter);
            CHECKNULL(lqd, L"MgdFeatureService::JoinFeatures");
            qd = lqd;

            IGWSJoinQueryDefinition* jqd = NULL;

            MdfModel::AttributeRelateCollection* attributeRelates = extension->GetAttributeRelates();
            CHECKNULL(attributeRelates, L"MgdFeatureService::JoinFeatures");

            bool bForceOneToOne = true;
            Ptr<MgStringCollection> attributeNameDelimiters = new MgStringCollection();

            //Store all the secondary's name, and if the filter property doesn't contain secondary's name,
            //it applies only to the primary.
            Ptr<MgStringCollection> secondaryNames = new MgStringCollection();

            // For each join (attributeRelate) to a secondary source need to do the following
            for (int attributeRelateIndex = 0; attributeRelateIndex < attributeRelates->GetCount(); attributeRelateIndex++)
            {
                MdfModel::AttributeRelate* attributeRelate = attributeRelates->GetAt(attributeRelateIndex);
                CHECKNULL(attributeRelate, L"MgdFeatureService::JoinFeatures");

                // Get the secondary resource id
                STRING secondaryResourceId = (STRING)attributeRelate->GetResourceId();

                // Get the name for the join relationship
                STRING attributeRelateName = (STRING)attributeRelate->GetName();
                STRING secondaryConnectionName = attributeRelateName;

                if(!secondaryNames->Contains(attributeRelateName))
                    secondaryNames->Add(attributeRelateName);

                // Get the RelateType (join type).  Default is Left Outer join.
                MdfModel::AttributeRelate::RelateType relateType = attributeRelate->GetRelateType();

                // Get the ForceOneToOne field, which specifies if multiple matching secondary features
                // are retrieved via a 1-to-1 or 1-to-many relationship.  Default is 1-to-1 relationship.
                bool forceOneToOne = attributeRelate->GetForceOneToOne();
                // If there is at least one relation is defined as one-to-many, then the one-to-many result will apply to all join results.
                if (!forceOneToOne)
                {
                    bForceOneToOne = false;
                }

                // Get the AttributeNameDelimiter field, which specifies the delimiter between the JoinName (attribute relate name)
                // and the property name for an extended property.  Default delimiter is "" (blank).
                STRING attributeNameDelimiter = (STRING)attributeRelate->GetAttributeNameDelimiter();
                attributeNameDelimiters->Add(attributeNameDelimiter);

                // Establish connection to provider for secondary feature source
                Ptr<MgResourceIdentifier> secondaryFeatureSource = new MgResourceIdentifier(secondaryResourceId);

                if (NULL != secondaryFeatureSource)
                {
                    Ptr<MgFeatureConnection> fdoRightConn = new MgFeatureConnection(secondaryFeatureSource);
                    
                    if ((NULL != fdoRightConn.p) && ( fdoRightConn->IsConnectionOpen() ))
                    {
                        pool->AddConnection(secondaryConnectionName.c_str(), fdoRightConn);
                    }
                    else
                    {
                        throw new MgConnectionFailedException(L"MgdFeatureService::JoinFeatures",
                            __LINE__, __WFILE__, NULL, L"", NULL);
                    }
                }

                // Get the secondary featureClassName (qualified className)
                STRING secondaryClassName = (STRING)attributeRelate->GetAttributeClass();

                // Parse the qualified classname
                STRING secondaryFsSchema, secondaryFsClassName;
                MgUtil::ParseQualifiedClassName(secondaryClassName, secondaryFsSchema, secondaryFsClassName);

                // Create secondary query definition
                FdoPtr<FdoIdentifierCollection> rsellist;
                FdoPtr<FdoFilter> rfilter;

                FdoPtr<IGWSQueryDefinition> rqd  = IGWSFeatureQueryDefinition::Create(
                    rsellist,
                    GWSQualifiedName(secondaryConnectionName.c_str(), secondaryFsSchema.c_str(), secondaryFsClassName.c_str()),
                    rfilter);
                CHECKNULL(rqd, L"MgdFeatureService::JoinFeatures");

                // Get Join Attributes
                FdoPtr<FdoStringCollection> lattrs = FdoStringCollection::Create();
                FdoPtr<FdoStringCollection> rattrs = FdoStringCollection::Create();

                // Determine the number of RelateProperties (attributes)
                MdfModel::RelatePropertyCollection* relateProperties = attributeRelate->GetRelateProperties();
                CHECKNULL(relateProperties, L"MgdFeatureService::JoinFeatures");
                int nRelatePropertyCount = relateProperties->GetCount();

                // For each RelateProperty need to do the following
                for (int relatePropertyIndex = 0; relatePropertyIndex < nRelatePropertyCount; relatePropertyIndex++)
                {
                    MdfModel::RelateProperty* relateProperty = relateProperties->GetAt(relatePropertyIndex);
                    CHECKNULL(relateProperty, L"MgdFeatureService::JoinFeatures");

                    // Get the FeatureClassProperty (primary attribute)
                    STRING primaryAttribute = (STRING)relateProperty->GetFeatureClassProperty();

                    // Add to the primary attribute String collection
                    lattrs->Add(primaryAttribute.c_str());

                    // Get the AttributeClassProperty (secondary attribute)
                    STRING secondaryAttribute = (STRING)relateProperty->GetAttributeClassProperty();

                    // Add to the secondary attribute String collection
                    rattrs->Add(secondaryAttribute.c_str());
                }

                // Create the QueryDefinition
                if (NULL != rqd)
                {
                    FdoString* joinName = attributeRelateName.c_str();
                    FdoString* joinDelimiter = L".";
                    if (MdfModel::AttributeRelate::Inner == relateType)
                    {
                        jqd = IGWSEqualJoinQueryDefinition::Create(joinName, joinDelimiter, bForceOneToOne, lqd, rqd, lattrs, rattrs);
                    }
                    else  // if (RelateType::LeftOuter == relateType)
                    {
                        jqd = IGWSLeftJoinQueryDefinition::Create(joinName, joinDelimiter, bForceOneToOne, lqd, rqd, lattrs, rattrs);
                    }
                    lqd = jqd;
                }

            }  // Repeat for each secondary source
            qd = lqd;

            FdoPtr<IGWSQuery> query = IGWSQuery::Create(pool, qd, NULL);
            // This encapsulates our join query results
            FdoPtr<IGWSFeatureIterator> iter;
            // This copy is used to construct the Class Definition that the GWS Feature Reader returns
            FdoPtr<IGWSFeatureIterator> iterCopy;

            // Set batch size as it may be needed
            CGwsBatchSortedBlockJoinQueryResults::sm_nBatchSize = m_nJoinQueryBatchSize;

            // Prepare and Execute Query
            query->Prepare();

            // Search the filter to see if it contains the extension name
            // If the extension name is not found it means that the filter involves attribute(s) only from the primary
            if(NULL != filter)
            {
                FdoString* filterText = filter->ToString();
                if(NULL != filterText)
                {
                    bool match = false;
                    for(int i = 0; i < secondaryNames->GetCount(); i ++)
                    {
                        STRING secondaryName = secondaryNames->GetItem(i);
                        if(NULL != wcsstr(filterText, secondaryName.c_str()))
                        {
                            match = true;
                            break;
                        }
                    }
                    if(!match)
                    {
                        // Add the filter to the query because it only applies to the primary
                        query->SetFilter(filter);
                    }
                }
            }

            // Execute the query
            query->Execute(&iter, true);
            query->Execute(&iterCopy, true);

            FdoPtr<FdoStringCollection> fsNames = qd->FeatureSourceNames();

            gwsFeatureReader = new MgdGwsFeatureReader(pool, iter, iterCopy, parsedExtensionName, fsNames, bForceOneToOne, attributeNameDelimiters);
            gwsFeatureReader->SetFilter(filter);
            break;
        }
    }

    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH_AND_THROW(featureSourceIdentifier, L"MgdFeatureService::JoinFeatures")

    // Now that the reader has been created we will need to mark all of the connections it uses as OwnReader() because the GWS reader will be
    // taking ownership of the connections. We have to do it this late in the code in case an exception is thrown somewhere before this.
    // We want to avoid a deadlock of the connection :)
    gwsFeatureReader->OwnsConnections();

    return gwsFeatureReader.Detach();
}

bool MgdFeatureService::SupportsFdoJoins(MgFeatureConnection* conn, MdfModel::Extension* extension)
{
    bool bSupported = false;

    MG_FEATURE_SERVICE_TRY()

    //Get the easy checks out of the way
    if (!conn->SupportsJoins())
    {
#ifdef DEBUG_FDOJOIN
        ACE_DEBUG((LM_INFO, ACE_TEXT("\n[FDO Join Test]: Connection does not support FDO joins")));
#endif
        return false;
    }

    FdoJoinType jtypes = conn->GetJoinTypes();

    //Now ascertain if all participating feature classes originate from the same feature source
    Ptr<MgResourceIdentifier> fsId = conn->GetFeatureSource();
    MdfModel::AttributeRelateCollection* relates = extension->GetAttributeRelates();

    //We've yet to figure out chained joins
    if (relates->GetCount() > 1)
    {
#ifdef DEBUG_FDOJOIN
        ACE_DEBUG((LM_INFO, ACE_TEXT("\n[FDO Join Test]: Multiple joins not yet supported")));
#endif
        return false;
    }

    for (INT32 i = 0; i < relates->GetCount(); i++)
    {
        MdfModel::AttributeRelate* relate = relates->GetAt(i);
        const MdfModel::MdfString& featureSourceId = relate->GetResourceId();

        //Different feature sources
        if (fsId->ToString() != featureSourceId)
        {
#ifdef DEBUG_FDOJOIN
            ACE_DEBUG((LM_INFO, ACE_TEXT("\n[FDO Join Test]: Not joining to class from same feature source")));
#endif
            return false;
        }

        MdfModel::RelatePropertyCollection* relProps = relate->GetRelateProperties();

        //Check if the join type is supported. Given FDO exposes more join types than
        //the ones specified here, the chances are real good that we'll have a match
        MdfModel::AttributeRelate::RelateType rtype = relate->GetRelateType();
        switch(rtype)
        {
        case MdfModel::AttributeRelate::Inner:
            if ((jtypes & FdoJoinType_Inner) != FdoJoinType_Inner)
            {
#ifdef DEBUG_FDOJOIN
                ACE_DEBUG((LM_INFO, ACE_TEXT("\n[FDO Join Test]: Unsupported join type")));
#endif
                return false;
            }
            break;
        case MdfModel::AttributeRelate::LeftOuter:
            if ((jtypes & FdoJoinType_LeftOuter) != FdoJoinType_LeftOuter)
            {
#ifdef DEBUG_FDOJOIN
                ACE_DEBUG((LM_INFO, ACE_TEXT("\n[FDO Join Test]: Unsupported join type")));
#endif
                return false;
            }
            break;
        case MdfModel::AttributeRelate::RightOuter:
            if ((jtypes & FdoJoinType_RightOuter) != FdoJoinType_RightOuter)
            {
#ifdef DEBUG_FDOJOIN
                ACE_DEBUG((LM_INFO, ACE_TEXT("\n[FDO Join Test]: Unsupported join type")));
#endif
                return false;
            }
            break;
        default:
            {
#ifdef DEBUG_FDOJOIN
                ACE_DEBUG((LM_INFO, ACE_TEXT("\n[FDO Join Test]: Unrecognised join type")));
#endif
                return false;
            }
        }
    }

    //Still here? You pass the test
    bSupported = true;

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureService::SupportsFdoJoin")

    return bSupported;
}

MgFeatureReader* MgdFeatureService::SelectFdoJoin(MgFeatureConnection* conn, MdfModel::Extension* extension, FdoFilter* filter)
{
    Ptr<MgFeatureReader> ret;

    MG_FEATURE_SERVICE_TRY()

    FdoPtr<FdoIConnection> fdoConn = conn->GetConnection();
    FdoPtr<FdoISelect> selExt = static_cast<FdoISelect*>(fdoConn->CreateCommand(FdoCommandType_Select));

    const MdfModel::MdfString& primaryClassName = extension->GetFeatureClass();
    selExt->SetFeatureClassName(primaryClassName.c_str());

    if (NULL != filter)
        selExt->SetFilter(filter);

    STRING primarySchema;
    STRING primaryClass;
    MgUtil::ParseQualifiedClassName(primaryClassName, primarySchema, primaryClass);
    selExt->SetAlias(primaryClass.c_str());
    FdoPtr<FdoIdentifierCollection> propNames = selExt->GetPropertyNames();

    //These two maps will allow us to resolve an Extended Feature Class Property
    //to the form [Alias].[PropertyName]
    //
    //TODO: How does FDO escape property names with spaces? Double quotes doesn't work 
    //(as stupid and ridiculous as it is from a data modeling perspective)

    Ptr<MgStringCollection> idPropNames = new MgStringCollection();

    FdoPtr<FdoJoinCriteriaCollection> joinCriteria = selExt->GetJoinCriteria();
    
    //Include all primary class properties (data/geometry) in query
    Ptr<MgResourceIdentifier> fsId = conn->GetFeatureSource();
    Ptr<MgClassDefinition> primaryClassDef = GetClassDefinition(fsId, primarySchema, primaryClass);
    Ptr<MgPropertyDefinitionCollection> primaryClassProps = primaryClassDef->GetProperties();
    for (INT32 i = 0; i < primaryClassProps->GetCount(); i++)
    {
        Ptr<MgPropertyDefinition> propDef = primaryClassProps->GetItem(i);
        if (propDef->GetPropertyType() == MgFeaturePropertyType::DataProperty ||
            propDef->GetPropertyType() == MgFeaturePropertyType::GeometricProperty)
        {
            STRING pn = primaryClass + L"." + propDef->GetName();
            FdoString* expr = pn.c_str();
            FdoPtr<FdoIdentifier> propName = FdoIdentifier::Create(expr);
            //Include in query
            propNames->Add(propName);

#ifdef DEBUG_FDOJOIN
            ACE_DEBUG((LM_INFO, ACE_TEXT("\nAdd Property to join query: %W"), expr));
#endif
        }
    }
    Ptr<MgPropertyDefinitionCollection> idClassProps = primaryClassDef->GetIdentityProperties();
    for (INT32 i = 0; i < idClassProps->GetCount(); i++)
    {
        Ptr<MgPropertyDefinition> propDef = idClassProps->GetItem(i);
        idPropNames->Add(propDef->GetName());
    }

    //Process the joins
    MdfModel::AttributeRelateCollection* relates = extension->GetAttributeRelates();
    
    bool bForceOneToOne = false;
    //SupportsFdoJoin() has vetted that relates->GetCount() == 1
    //It's coded in this fashion to facilitate support for additional cases (eg. Chained joins)
    //in the future
    for (INT32 i = 0; i < relates->GetCount(); i++)
    {
        MdfModel::AttributeRelate* relate = relates->GetAt(i);
        const MdfModel::MdfString& prefix = relate->GetName();
        FdoPtr<FdoJoinCriteria> criteria;

        const MdfModel::MdfString& attrClassName = relate->GetAttributeClass();
        FdoPtr<FdoIdentifier> joinClass = FdoIdentifier::Create(attrClassName.c_str());

        FdoJoinType jtype = FdoJoinType_None;
        switch(relate->GetRelateType())
        {
        case MdfModel::AttributeRelate::Inner:
            jtype = FdoJoinType_Inner;
            break;
        case MdfModel::AttributeRelate::LeftOuter:
            jtype = FdoJoinType_LeftOuter;
            break;
        case MdfModel::AttributeRelate::RightOuter:
            jtype = FdoJoinType_RightOuter;
            break;
        }

        STRING joinSchemaName;
        STRING joinClassName;
        MgUtil::ParseQualifiedClassName(attrClassName, joinSchemaName, joinClassName);

        MdfModel::RelatePropertyCollection* relProps = relate->GetRelateProperties();

        if (relate->GetForceOneToOne())
        {
            bForceOneToOne = true;
        }

        //NOTE: Sadly, the extended class definition does not explicity specify the list of join properties
        //only the ones to join on. So grab the join class definition to compile this list
        Ptr<MgResourceIdentifier> joinResId = new MgResourceIdentifier(relate->GetResourceId());
        Ptr<MgClassDefinition> joinClassDef = GetClassDefinition(joinResId, joinSchemaName, joinClassName);

        //Compile mappings
        Ptr<MgPropertyDefinitionCollection> joinClassProps = joinClassDef->GetProperties();
        for (INT32 j = 0; j < joinClassProps->GetCount(); j++)
        {
            Ptr<MgPropertyDefinition> propDef = joinClassProps->GetItem(j);
            if (propDef->GetPropertyType() == MgFeaturePropertyType::DataProperty ||
                propDef->GetPropertyType() == MgFeaturePropertyType::GeometricProperty)
            {
                //[joinAlias].[propertyName] AS [readerPropName]
                STRING readerPropName = prefix + propDef->GetName();
                STRING exprText = joinClassName + L"." + propDef->GetName();
                FdoPtr<FdoExpression> expr = FdoExpression::Parse(exprText.c_str());
                FdoPtr<FdoComputedIdentifier> propName = FdoComputedIdentifier::Create(readerPropName.c_str(), expr);
                //Include in query
                propNames->Add(propName);

#ifdef DEBUG_FDOJOIN
                ACE_DEBUG((LM_INFO, ACE_TEXT("\nAdd Property to join query: %W AS %W"), exprText.c_str(), readerPropName.c_str()));
#endif
            }
        }

        STRING filterText;

        //Set filter for this join
        for (INT32 j = 0; j < relProps->GetCount(); j++)
        {
            MdfModel::RelateProperty* prop = relProps->GetAt(j);
            if (!filterText.empty())
            {
                filterText += L" AND ";
            }

            //[PrimaryClass].[PropertyName] = [JoinClass].[PropertyName]
            filterText += primaryClass;
            filterText += L".";
            filterText += prop->GetFeatureClassProperty();
            filterText += L" = ";
            filterText += joinClassName;
            filterText += L".";
            filterText += prop->GetAttributeClassProperty();
        }

#ifdef DEBUG_FDOJOIN
        ACE_DEBUG((LM_INFO, ACE_TEXT("\nAdd Join Filter: %W"), filterText.c_str()));
#endif
        FdoPtr<FdoFilter> filter = FdoFilter::Parse(filterText.c_str());
        if (prefix.empty())
            criteria = FdoJoinCriteria::Create(joinClass, jtype, filter);
        else
            criteria = FdoJoinCriteria::Create(joinClassName.c_str(), joinClass, jtype, filter);

        joinCriteria->Add(criteria);
    }

    FdoPtr<FdoIFeatureReader> fdoFr = selExt->Execute();
    if (bForceOneToOne) {
        FdoPtr<FdoStringCollection> propNames = MgFeatureUtil::MgToFdoStringCollection(idPropNames, false);
        FdoPtr<FdoIFeatureReader> forcedReader = new MgFdoForcedOneToOneFeatureReader(fdoFr, propNames);
        ret = new MgdFeatureReader(conn, forcedReader);
    } else {
        ret = new MgdFeatureReader(conn, fdoFr);
    }
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureService::SelectFdoJoin")

    return ret.Detach();
}

bool MgdFeatureService::FindFeatureJoinProperties(MgResourceIdentifier* featureSourceId, CREFSTRING extensionName)
{
    bool bJoinPropertiesExists = false;

    MgFeatureServiceCache* cache = MgFeatureServiceCache::GetInstance();
    Ptr<MgFeatureSourceCacheItem> fsCacheItem = cache->GetFeatureSource(featureSourceId);

    CHECKNULL(fsCacheItem.p, L"MgdFeatureService::FindFeatureJoinProperties");
    MdfModel::FeatureSource* featureSource = fsCacheItem->Get();
    MdfModel::ExtensionCollection* extensions = featureSource->GetExtensions();
    CHECKNULL(extensions, L"MgdFeatureService::FindFeatureJoinProperties");

    for (int i = 0; i < extensions->GetCount(); i++)
    {
        MdfModel::Extension* extension = extensions->GetAt(i);
        CHECKNULL(extension, L"MgdFeatureService::FindFeatureJoinProperties");
        STRING name = (STRING)extension->GetName();

        STRING parsedSchemaName, parsedExtensionName;
        MgUtil::ParseQualifiedClassName(extensionName, parsedSchemaName, parsedExtensionName);

        if (parsedExtensionName != name)
        {
            continue;
        }
        else
        {
            AttributeRelateCollection* relates = extension->GetAttributeRelates();
            bJoinPropertiesExists = (relates != NULL && relates->GetCount() != 0);
            break;
        }
    }

    return bJoinPropertiesExists;
}

bool MgdFeatureService::FindFeatureCalculation(MgResourceIdentifier* resourceId, CREFSTRING extensionName)
{
    bool bCalculationExists = false;

    MgFeatureServiceCache* cache = MgFeatureServiceCache::GetInstance();
    Ptr<MgFeatureSourceCacheItem> fsCacheItem = cache->GetFeatureSource(resourceId);

    CHECKNULL(fsCacheItem.p, L"MgdFeatureService::FindFeatureCalculation");
    MdfModel::FeatureSource* featureSource = fsCacheItem->Get();
    MdfModel::ExtensionCollection* extensions = featureSource->GetExtensions();
    CHECKNULL(extensions, L"MgdFeatureService::FindFeatureCalculation");

    for (int i = 0; i < extensions->GetCount(); i++)
    {
        MdfModel::Extension* extension = extensions->GetAt(i);
        CHECKNULL(extension, L"MgdFeatureService::FindFeatureCalculation");
        STRING name = (STRING)extension->GetName();

        STRING parsedSchemaName, parsedExtensionName;
        MgUtil::ParseQualifiedClassName(extensionName, parsedSchemaName, parsedExtensionName);

        if (parsedExtensionName != name)
        {
            continue;
        }
        else
        {
            CalculatedPropertyCollection* calcProps = extension->GetCalculatedProperties();
            // we don't have joins but we have calculations
            bCalculationExists = (calcProps != NULL && calcProps->GetCount() != 0);
            break;
        }
    }

    return bCalculationExists;
}

void MgdFeatureService::UpdateCommandOnCalculation(FdoIBaseSelect* command, MgResourceIdentifier* featureSourceId, CREFSTRING extensionName)
{
    MG_FEATURE_SERVICE_TRY()

    MgFeatureServiceCache* cache = MgFeatureServiceCache::GetInstance();
    Ptr<MgFeatureSourceCacheItem> fsCacheItem = cache->GetFeatureSource(featureSourceId);

    CHECKNULL(fsCacheItem.p, L"MgdFeatureService::UpdateCommandOnCalculation");
    MdfModel::FeatureSource* featureSource = fsCacheItem->Get();
    MdfModel::ExtensionCollection* extensions = featureSource->GetExtensions();
    CHECKNULL(extensions, L"MgdFeatureService::UpdateCommandOnCalculation");

    for (int i = 0; i < extensions->GetCount(); i++)
    {
        MdfModel::Extension* extension = extensions->GetAt(i);
        CHECKNULL(extension, L"MgdFeatureService::UpdateCommandOnCalculation");
        STRING name = (STRING)extension->GetName();

        STRING parsedSchemaName, parsedExtensionName;
        ParseQualifiedClassNameForCalculation(extension, extensionName, parsedSchemaName, parsedExtensionName);

        if (parsedExtensionName != name)
        {
            continue;
        }
        else
        {
            command->SetFeatureClassName(extension->GetFeatureClass().c_str());
            CalculatedPropertyCollection* calcProps = extension->GetCalculatedProperties();
            if (calcProps == NULL || calcProps->GetCount() == 0)
                break;
            FdoPtr<FdoIdentifierCollection> idList = FdoIdentifierCollection::Create();
            for (int idx = 0; idx < calcProps->GetCount(); idx++)
            {
                CalculatedProperty* calcProp = calcProps->GetAt(idx);
                FdoPtr<FdoExpression> expressionCalc = FdoExpression::Parse(calcProp->GetExpression().c_str());
                FdoPtr<FdoComputedIdentifier> idfCalc = FdoComputedIdentifier::Create(calcProp->GetName().c_str(), expressionCalc);
                idList->Add(idfCalc);
            }
            FdoPtr<FdoFilter> filter = command->GetFilter();
            if (filter != NULL)
            {
                FdoPtr<FdoFilter> newFilter = FdoExpressionEngineCopyFilter::Copy(filter, idList);
                command->SetFilter(newFilter);
            }

            FdoPtr<FdoIdentifierCollection> fic = command->GetPropertyNames();
            bool addAllProps = (fic->GetCount() == 0);
            for (int idx = 0; idx < calcProps->GetCount(); idx++)
            {
                CalculatedProperty* calcProp = calcProps->GetAt(idx);
                FdoPtr<FdoIdentifier> idf = fic->FindItem(calcProp->GetName().c_str());
                if (idf != NULL)
                {
                    // replace calculated properties provided as identifiers to computed identifiers
                    FdoPtr<FdoComputedIdentifier> idfCalc = static_cast<FdoComputedIdentifier*>(idList->GetItem(idf->GetName()));
                    FdoPtr<FdoExpression> expressionCalc = idfCalc->GetExpression();
                    FdoPtr<FdoExpression> expandedExpression = FdoExpressionEngineCopyFilter::Copy(expressionCalc, idList);

                    int idfIndex = fic->IndexOf(idf);
                    FdoPtr<FdoComputedIdentifier> newIdf = FdoComputedIdentifier::Create(idf->GetName(), expandedExpression);
                    fic->SetItem(idfIndex, newIdf);
                }
                else
                {
                    if (addAllProps)
                    {
                        FdoPtr<FdoComputedIdentifier> idfCalc = static_cast<FdoComputedIdentifier*>(idList->GetItem(calcProp->GetName().c_str()));
                        FdoPtr<FdoExpression> expressionCalc = idfCalc->GetExpression();
                        FdoPtr<FdoExpression> expandedExpression = FdoExpressionEngineCopyFilter::Copy(expressionCalc, idList);

                        FdoPtr<FdoComputedIdentifier> newIdf = FdoComputedIdentifier::Create(calcProp->GetName().c_str(), expandedExpression);
                        fic->Add(newIdf);
                    }
                }
                if (addAllProps)
                {
                    Ptr<MgFeatureConnection> conn = new MgFeatureConnection(featureSourceId);
                    if ((NULL != conn.p) && ( conn->IsConnectionOpen() ))
                    {
                        FdoPtr<FdoIConnection> fdoConn = conn->GetConnection();
                        // The reference to the FDO connection from the MgFeatureConnection object must be cleaned up before the parent object
                        // otherwise it leaves the FDO connection marked as still in use.
                        //FdoPtr<FdoIConnection> conn = fcConnection->GetConnection();
                        FdoPtr<FdoIDescribeSchema>  descSchema = (FdoIDescribeSchema *) fdoConn->CreateCommand (FdoCommandType_DescribeSchema);

                        STRING fullClassName = extension->GetFeatureClass();
                        STRING schemaName, className;
                        MgUtil::ParseQualifiedClassName(fullClassName, schemaName, className);

                        if (!parsedSchemaName.empty())
                        {
                            descSchema->SetSchemaName(parsedSchemaName.c_str());
                        }

                        if (!className.empty())
                        {
                            FdoPtr<FdoStringCollection> classNames = FdoStringCollection::Create();

                            classNames->Add(className.c_str());
                            descSchema->SetClassNames(classNames.p);
                        }

                        FdoPtr <FdoFeatureSchemaCollection> schemas = (FdoFeatureSchemaCollection *) descSchema->Execute ();
                        FdoPtr<FdoFeatureSchema> schema = (FdoFeatureSchema *)schemas->GetItem (parsedSchemaName.c_str());
                        FdoPtr<FdoClassCollection> classes = schema->GetClasses();

                        FdoPtr<FdoClassDefinition> activeClass = classes->GetItem(className.c_str());
                        FdoPtr<FdoPropertyDefinitionCollection> properties = activeClass->GetProperties();
                        for(int i = 0; i < properties->GetCount(); i++)
                        {
                            FdoPtr<FdoPropertyDefinition> activeProperty = properties->GetItem(i);
                            FdoPtr<FdoIdentifier> idf = fic->FindItem(activeProperty->GetName());
                            if (idf == NULL)
                            {
                                idf = FdoIdentifier::Create(activeProperty->GetName());
                                fic->Add(idf);
                            }
                        }
                        FdoPtr<FdoReadOnlyPropertyDefinitionCollection> baseProps = activeClass->GetBaseProperties();
                        if (baseProps != NULL)
                        {
                            for(int i = 0; i < baseProps->GetCount(); i++)
                            {
                                FdoPtr<FdoPropertyDefinition>prop = baseProps->GetItem(i);
                                if( prop->GetIsSystem() )
                                    continue;
                                fic->Add( FdoPtr<FdoIdentifier>(FdoIdentifier::Create( prop->GetName() ) ) );
                            }
                        }
                    }
                }
            }
            break;
        }
    }
    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH_AND_THROW(featureSourceId, L"MgdFeatureService::UpdateCommandOnCalculation")
}

void MgdFeatureService::UpdateCommandOnJoinCalculation(FdoIBaseSelect* command, MgResourceIdentifier* featureSourceId, CREFSTRING extensionName)
{
    MG_FEATURE_SERVICE_TRY()

    MgFeatureServiceCache* cache = MgFeatureServiceCache::GetInstance();
    Ptr<MgFeatureSourceCacheItem> fsCacheItem = cache->GetFeatureSource(featureSourceId);

    CHECKNULL(fsCacheItem.p, L"MgdFeatureService::UpdateCommandOnJoinCalculation");
    MdfModel::FeatureSource* featureSource = fsCacheItem->Get();
    MdfModel::ExtensionCollection* extensions = featureSource->GetExtensions();
    CHECKNULL(extensions, L"MgdFeatureService::UpdateCommandOnJoinCalculation");

    for (int i = 0; i < extensions->GetCount(); i++)
    {
        MdfModel::Extension* extension = extensions->GetAt(i);
        CHECKNULL(extension, L"MgdFeatureService::UpdateCommandOnJoinCalculation");
        STRING name = (STRING)extension->GetName();

        STRING parsedSchemaName, parsedExtensionName;
        ParseQualifiedClassNameForCalculation(extension, extensionName, parsedSchemaName, parsedExtensionName);

        if (parsedExtensionName != name)
        {
            continue;
        }
        else
        {
            CalculatedPropertyCollection* calcProps = extension->GetCalculatedProperties();
            if (calcProps == NULL || calcProps->GetCount() == 0)
                break;
            FdoPtr<FdoIdentifierCollection> idList = FdoIdentifierCollection::Create();
            for (int idx = 0; idx < calcProps->GetCount(); idx++)
            {
                CalculatedProperty* calcProp = calcProps->GetAt(idx);
                FdoPtr<FdoExpression> expressionCalc = FdoExpression::Parse(calcProp->GetExpression().c_str());
                FdoPtr<FdoComputedIdentifier> idfCalc = FdoComputedIdentifier::Create(calcProp->GetName().c_str(), expressionCalc);
                idList->Add(idfCalc);
            }
            FdoPtr<FdoFilter> filter = command->GetFilter();
            if (filter != NULL)
            {
                FdoPtr<FdoFilter> newFilter = FdoExpressionEngineCopyFilter::Copy(filter, idList);
                command->SetFilter(newFilter);
            }

            FdoPtr<FdoIdentifierCollection> fic = command->GetPropertyNames();
            if (fic->GetCount() != 0)
            {
                // replace calculated properties provided as identifiers to computed identifiers
                for (int idx = 0; idx < calcProps->GetCount(); idx++)
                {
                    CalculatedProperty* calcProp = calcProps->GetAt(idx);
                    FdoPtr<FdoIdentifier> idf = fic->FindItem(calcProp->GetName().c_str());
                    if (idf != NULL)
                    {
                        FdoPtr<FdoComputedIdentifier> idfCalc = static_cast<FdoComputedIdentifier*>(idList->GetItem(idf->GetName()));
                        FdoPtr<FdoExpression> expressionCalc = idfCalc->GetExpression();
                        FdoPtr<FdoExpression> expandedExpression = FdoExpressionEngineCopyFilter::Copy(expressionCalc, idList);

                        int idfIndex = fic->IndexOf(idf);
                        FdoPtr<FdoComputedIdentifier> newIdf = FdoComputedIdentifier::Create(idf->GetName(), expandedExpression);
                        fic->SetItem(idfIndex, newIdf);
                    }
                }
            }
            break;
        }
    }
    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH_AND_THROW(featureSourceId, L"MgdFeatureService::UpdateCommandOnJoinCalculation")
}

void MgdFeatureService::ParseQualifiedClassNameForCalculation(MdfModel::Extension* extension, CREFSTRING qualifiedClassName, STRING& schemaName, STRING& className)
{
    CHECKNULL(extension, L"MgdFeatureService::ParseQualifiedClassNameForCalculation");

    MgUtil::ParseQualifiedClassName(qualifiedClassName, schemaName, className);

    if (schemaName.empty())
    {
        STRING dummyStr;

        MgUtil::ParseQualifiedClassName(extension->GetFeatureClass(), schemaName, dummyStr);
    }
}

MgResourceIdentifier* MgdFeatureService::GetSecondaryResourceIdentifier(MgResourceIdentifier* primResId, CREFSTRING extensionName, CREFSTRING relationName)
{
    Ptr<MgResourceIdentifier> secResId;
    MgFeatureServiceCache* cache = MgFeatureServiceCache::GetInstance();
    Ptr<MgFeatureSourceCacheItem> fsCacheItem = cache->GetFeatureSource(primResId);

    CHECKNULL(fsCacheItem.p, L"MgdFeatureService::GetSecondaryResourceIdentifier");
    MdfModel::FeatureSource* featureSource = fsCacheItem->Get();
    MdfModel::ExtensionCollection* extensions = featureSource->GetExtensions();
    CHECKNULL(extensions, L"MgdFeatureService::GetSecondaryResourceIdentifier");

    for (int i = 0; i < extensions->GetCount(); i++)
    {
        MdfModel::Extension* extension = extensions->GetAt(i);
        CHECKNULL(extension, L"MgdFeatureService::GetSecondaryResourceIdentifier");

        // Get the extension name
        STRING name = (STRING)extension->GetName();

        STRING parsedSchemaName, parsedExtensionName;
        MgUtil::ParseQualifiedClassName(extensionName, parsedSchemaName, parsedExtensionName);

        if (parsedExtensionName != name)
        {
            continue;
        }
        else
        {
            // Determine the number of secondary sources (AttributeRelates)
            MdfModel::AttributeRelateCollection* attributeRelates = extension->GetAttributeRelates();
            CHECKNULL(attributeRelates, L"MgdFeatureService::GetSecondaryResourceIdentifier");
            int nAttributeRelateCount = attributeRelates->GetCount();

            // Find the specified relation name
            {
                for (int attributeRelateIndex = 0; attributeRelateIndex < nAttributeRelateCount; attributeRelateIndex++)
                {
                    MdfModel::AttributeRelate* attributeRelate = attributeRelates->GetAt(attributeRelateIndex);
                    CHECKNULL(attributeRelate, L"MgdFeatureService::GetSecondaryResourceIdentifier");

                    // Get the name for the join relationship
                    STRING attributeRelateName = (STRING)attributeRelate->GetName();

                    if (attributeRelateName != relationName)
                    {
                        continue;
                    }
                    else
                    {
                        // Get the secondary resource id
                        STRING secondaryResourceId = (STRING)attributeRelate->GetResourceId();
                        secResId = new MgResourceIdentifier(secondaryResourceId);
                        break;
                    }
                }
            }
        }
    }

    return secResId.Detach();
}

MgDataReader* MgdFeatureService::SelectAggregate(MgResourceIdentifier* resource,
                                                 CREFSTRING className,
                                                 MgFeatureAggregateOptions* options) 
{ 
	CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdFeatureService::SelectAggregate");
	CHECKARGUMENTNULL(options, L"MgdFeatureService::SelectAggregate");
    if (className.empty())
        throw new MgInvalidArgumentException(L"MgdFeatureService::SelectAggregate", __LINE__, __WFILE__, NULL, L"", NULL);

    Ptr<MgDataReader> reader;
    MG_FEATURE_SERVICE_TRY()

    Ptr<MgFeatureConnection> connWrap = new MgFeatureConnection(resource);
    {
        FdoPtr<FdoIConnection> conn = connWrap->GetConnection();

        FdoPtr<FdoISelectAggregates> select = (FdoISelectAggregates*)conn->CreateCommand(FdoCommandType_SelectAggregates);
        select->SetFeatureClassName(className.c_str());

        STRING filter = options->GetFilter();
        if (!filter.empty())
	        select->SetFilter(filter.c_str());

        Ptr<MgStringCollection> props = options->GetClassProperties();
        Ptr<MgStringPropertyCollection> computed = options->GetComputedProperties();

        INT32 propCount = props->GetCount();
        INT32 compCount = computed->GetCount();

        FdoPtr<FdoIdentifierCollection> fdoProps = select->GetPropertyNames();
        if (propCount > 0)
        {
	        for (INT32 i = 0; i < propCount; i++)
	        {
		        FdoPtr<FdoIdentifier> name = FdoIdentifier::Create(props->GetItem(i).c_str());
		        fdoProps->Add(name);
	        }
        }
        if (compCount > 0)
        {
	        for (INT32 i = 0; i < compCount; i++)
	        {
		        Ptr<MgStringProperty> comp = computed->GetItem(i);
		        FdoPtr<FdoExpression> expr = FdoExpression::Parse(comp->GetValue().c_str());
		        FdoPtr<FdoComputedIdentifier> name= FdoComputedIdentifier::Create(comp->GetName().c_str(), expr);

		        fdoProps->Add(name);
	        }
        }

        if (options->GetDistinct())
	        select->SetDistinct(true);

        STRING groupFilter = options->GetGroupFilter();
        if (!groupFilter.empty())
        {
	        FdoPtr<FdoFilter> grpFilter = FdoFilter::Parse(groupFilter.c_str());
	        select->SetGroupingFilter(grpFilter);
        }
        FdoPtr<FdoIdentifierCollection> fdoGroupBy = select->GetGrouping();
        Ptr<MgStringCollection> mgGroupBy = options->GetGroupingProperties();
        if (NULL != mgGroupBy && mgGroupBy->GetCount() > 0)
        {
	        for (INT32 i = 0; i < mgGroupBy->GetCount(); i++)
	        {
		        STRING name = mgGroupBy->GetItem(i);
		        FdoPtr<FdoIdentifier> fdoId = FdoIdentifier::Create(name.c_str());

		        fdoGroupBy->Add(fdoId);
	        }
        }

        FdoPtr<FdoIDataReader> fdoReader = select->Execute();
        reader = new MgdDataReader(connWrap, fdoReader);
    }

    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH_AND_THROW(resource, L"MgdFeatureService::SelectAggregate")

	return reader.Detach();
}

MgPropertyCollection* MgdFeatureService::UpdateFeatures(MgResourceIdentifier* resource,
                                                        MgFeatureCommandCollection* commands,
                                                        bool useTransaction) 
{ 
	CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdFeatureService::UpdateFeatures");
    CHECKARGUMENTNULL(commands, L"MgdFeatureService::UpdateFeatures");

    Ptr<MgPropertyCollection> result;

    MG_FEATURE_SERVICE_TRY()

    Ptr<MgTransaction> trans;
    if (useTransaction)
    {
        trans = BeginTransaction(resource);
    }

    result = UpdateFeatures(resource, commands, trans);
    if (useTransaction && NULL != trans)
    {
        trans->Commit();
    }

    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH_AND_THROW(resource, L"MgdFeatureService::UpdateFeatures")

    return result.Detach();
}

MgPropertyCollection* MgdFeatureService::UpdateFeatures(MgResourceIdentifier* resource,
                                                        MgFeatureCommandCollection* commands,
                                                        MgTransaction* transaction) 
{ 
	CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdFeatureService::UpdateFeatures");
    CHECKARGUMENTNULL(commands, L"MgdFeatureService::UpdateFeatures");
    CHECKARGUMENTNULL(transaction, L"MgdFeatureService::UpdateFeatures");

    Ptr<MgPropertyCollection> ret;

    MG_FEATURE_SERVICE_TRY()

    Ptr<MgFeatureConnection> connWrap;
	FdoPtr<FdoIConnection> conn;
    Ptr<MgdTransaction> mgTrans = dynamic_cast<MgdTransaction*>(transaction);
    if (NULL != mgTrans)
    {
        SAFE_ADDREF(mgTrans.p);
        Ptr<MgResourceIdentifier> origFeatureSource = mgTrans->GetFeatureSource();
        //Check that the transaction originates from the same feature source
        if (origFeatureSource->ToString() != resource->ToString())
            throw new MgInvalidArgumentException(L"MgdFeatureService::InsertFeatures", __LINE__, __WFILE__, NULL, L"", NULL);

        connWrap = mgTrans->GetConnection(); //Connection is already open
    }
    else
    {    
        connWrap = new MgFeatureConnection(resource);
    }

    conn = connWrap->GetConnection();

    ret = new MgPropertyCollection();

    INT32 cmdCount = commands->GetCount();
    if (cmdCount == 0)
    {
        MgStringCollection arguments;
        arguments.Add(L"2");
        arguments.Add(L"0");

        throw new MgInvalidArgumentException(L"MgdFeatureService::UpdateFeatures",
            __LINE__, __WFILE__, &arguments, L"MgCollectionEmpty", NULL);
    }

    FdoPtr<FdoICommandCapabilities> cmdCaps = conn->GetCommandCapabilities();
    FdoInt32 sCmdCount;
    FdoInt32* supportedCmds = cmdCaps->GetCommands(sCmdCount);

    for (INT32 i = 0; i < cmdCount; i++)
    {
        STRING sIndex;
        MgUtil::Int32ToString(i, sIndex);
        Ptr<MgFeatureCommand> fc = commands->GetItem(i);
        INT32 cmdType = fc->GetCommandType();
        switch(cmdType)
        {
        case MgFeatureCommandType::InsertFeatures:
            {
                bool supports = false;
                for (FdoInt32 j = 0; j < sCmdCount; j++)
                {
                    if (supportedCmds[i] == FdoCommandType_Insert)
                    {
                        supports = true;
                        break;
                    }
                }
                if (!supports)
                {
                    STRING message = MgFeatureUtil::GetMessage(L"MgCommandNotSupported");
                    MgStringCollection arguments;
                    arguments.Add(message);
                    throw new MgFeatureServiceException(L"MgdFeatureService::UpdateFeatures", __LINE__, __WFILE__, &arguments, L"", NULL);
                }

                Ptr<MgInsertFeatures> insert = dynamic_cast<MgInsertFeatures*>(fc.p);
                SAFE_ADDREF(insert.p);

                Ptr<MgPropertyCollection> properties = insert->GetPropertyValues();
                Ptr<MgFeatureReader> reader = InsertFeatures(resource, insert->GetFeatureClassName(), properties, transaction);

                Ptr<MgFeatureProperty> result = new MgFeatureProperty(sIndex, reader);
                ret->Add(result);
            }
            break;
        case MgFeatureCommandType::DeleteFeatures:
            {
                bool supports = false;
                for (FdoInt32 j = 0; j < sCmdCount; j++)
                {
                    if (supportedCmds[i] == FdoCommandType_Delete)
                    {
                        supports = true;
                        break;
                    }
                }
                if (!supports)
                {
                    STRING message = MgFeatureUtil::GetMessage(L"MgCommandNotSupported");
                    MgStringCollection arguments;
                    arguments.Add(message);
                    throw new MgFeatureServiceException(L"MgdFeatureService::UpdateFeatures", __LINE__, __WFILE__, &arguments, L"", NULL);
                }

                Ptr<MgDeleteFeatures> deleteCmd = dynamic_cast<MgDeleteFeatures*>(fc.p);
                SAFE_ADDREF(deleteCmd.p);

                int deleted = DeleteFeatures(resource, deleteCmd->GetFeatureClassName(), deleteCmd->GetFilterText(), transaction);

                Ptr<MgInt32Property> result = new MgInt32Property(sIndex, deleted);
                ret->Add(result);
            }
            break;
        case MgFeatureCommandType::UpdateFeatures:
            {
                bool supports = false;
                for (FdoInt32 j = 0; j < sCmdCount; j++)
                {
                    if (supportedCmds[i] == FdoCommandType_Update)
                    {
                        supports = true;
                        break;
                    }
                }
                if (!supports)
                {
                    STRING message = MgFeatureUtil::GetMessage(L"MgCommandNotSupported");
                    MgStringCollection arguments;
                    arguments.Add(message);
                    throw new MgFeatureServiceException(L"MgdFeatureService::UpdateFeatures", __LINE__, __WFILE__, &arguments, L"", NULL);
                }

                Ptr<MgUpdateFeatures> update = dynamic_cast<MgUpdateFeatures*>(fc.p);
                SAFE_ADDREF(update.p);

                Ptr<MgPropertyCollection> properties = update->GetPropertyValues();
                int updated = UpdateFeatures(resource, update->GetFeatureClassName(), properties, update->GetFilterText(), transaction);

                Ptr<MgInt32Property> result = new MgInt32Property(sIndex, updated);
                ret->Add(result);
            }
            break;
        default:
            STRING message = MgFeatureUtil::GetMessage(L"MgCommandNotSupported");
            MgStringCollection arguments;
            arguments.Add(message);
            throw new MgFeatureServiceException(L"MgdFeatureService::UpdateFeatures", __LINE__, __WFILE__, &arguments, L"", NULL);
        }
    }

    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH_AND_THROW(resource, L"MgdFeatureService::UpdateFeatures")

    return ret.Detach();
}

MgFeatureReader* MgdFeatureService::InsertFeatures(MgResourceIdentifier* resource, CREFSTRING className, MgPropertyCollection* propertyValues)
{
    Ptr<MgFeatureReader> ret;
    MG_FEATURE_SERVICE_TRY()
	ret = InsertFeatures(resource, className, propertyValues, NULL);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureService::InsertFeatures")
    return ret.Detach();
}

MgFeatureReader* MgdFeatureService::InsertFeatures(MgResourceIdentifier* resource, CREFSTRING className, MgPropertyCollection* propertyValues, MgTransaction* trans)
{
    CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdFeatureService::InsertFeatures");
	CHECKARGUMENTNULL(propertyValues, L"MgdFeatureService::InsertFeatures");
	if (className.empty())
		throw new MgNullArgumentException(L"MgdFeatureService::InsertFeatures", __LINE__, __WFILE__, NULL, L"", NULL);
	
	Ptr<MgFeatureReader> reader;

    MG_FEATURE_SERVICE_TRY()
	
    Ptr<MgFeatureConnection> connWrap;
	FdoPtr<FdoIConnection> conn;
    FdoPtr<FdoITransaction> fdoTrans;
    Ptr<MgdTransaction> mgTrans = dynamic_cast<MgdTransaction*>(trans);
    if (NULL != mgTrans)
    {
        SAFE_ADDREF(mgTrans.p);
        Ptr<MgResourceIdentifier> origFeatureSource = mgTrans->GetFeatureSource();
        //Check that the transaction originates from the same feature source
        if (origFeatureSource->ToString() != resource->ToString())
            throw new MgInvalidArgumentException(L"MgdFeatureService::InsertFeatures", __LINE__, __WFILE__, NULL, L"", NULL);

        connWrap = mgTrans->GetConnection(); //Connection is already open
        fdoTrans = mgTrans->GetFdoTransaction();
    }
    else
    {    
        connWrap = new MgFeatureConnection(resource);
    }

    conn = connWrap->GetConnection();
	FdoPtr<FdoIInsert> insert = (FdoIInsert*)conn->CreateCommand(FdoCommandType_Insert);
	
	insert->SetFeatureClassName(className.c_str());

	FdoPtr<FdoPropertyValueCollection> propVals = insert->GetPropertyValues();
	for (INT32 i = 0; i < propertyValues->GetCount(); i++)
	{
		Ptr<MgProperty> mgp = propertyValues->GetItem(i);
		FdoPtr<FdoPropertyValue> pv = MgFeatureUtil::MgPropertyToFdoProperty(mgp);

		propVals->Add(pv);
	}

    if (NULL != fdoTrans.p)
        insert->SetTransaction(fdoTrans);

	FdoPtr<FdoIFeatureReader> insertRes = insert->Execute();

	reader = new MgdFeatureReader(connWrap, insertRes);

    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH_AND_THROW(resource, L"MgdFeatureService::InsertFeatures")

	return reader.Detach();
}

MgPropertyCollection* MgdFeatureService::InsertFeatures(MgResourceIdentifier* resource, CREFSTRING className, MgBatchPropertyCollection* batchPropertyValues)
{
    Ptr<MgPropertyCollection> ret;
    MG_FEATURE_SERVICE_TRY()
    ret = InsertFeatures(resource, className, batchPropertyValues, NULL);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureService::InsertFeatures")
    return ret.Detach();
}

MgPropertyCollection* MgdFeatureService::InsertFeatures(MgResourceIdentifier* resource, CREFSTRING className, MgBatchPropertyCollection* batchPropertyValues, MgTransaction* trans)
{
    CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdFeatureService::InsertFeatures");
	CHECKARGUMENTNULL(batchPropertyValues, L"MgdFeatureService::InsertFeatures");
	if (className.empty())
		throw new MgNullArgumentException(L"MgdFeatureService::InsertFeatures", __LINE__, __WFILE__, NULL, L"", NULL);
	
    Ptr<MgPropertyCollection> ret;
    MG_FEATURE_SERVICE_TRY()
    ret = new MgPropertyCollection();

    Ptr<MgFeatureConnection> connWrap;
	FdoPtr<FdoIConnection> conn;
    FdoPtr<FdoITransaction> fdoTrans;
    Ptr<MgdTransaction> mgTrans = dynamic_cast<MgdTransaction*>(trans);
    if (NULL != mgTrans)
    {
        SAFE_ADDREF(mgTrans.p);
        Ptr<MgResourceIdentifier> origFeatureSource = mgTrans->GetFeatureSource();
        //Check that the transaction originates from the same feature source
        if (origFeatureSource->ToString() != resource->ToString())
            throw new MgInvalidArgumentException(L"MgdFeatureService::InsertFeatures", __LINE__, __WFILE__, NULL, L"", NULL);

        connWrap = mgTrans->GetConnection(); //Connection is already open
        fdoTrans = mgTrans->GetFdoTransaction();
    }
    else
    {    
        connWrap = new MgFeatureConnection(resource);
    }

    conn = connWrap->GetConnection();
	FdoPtr<FdoIInsert> insert = (FdoIInsert*)conn->CreateCommand(FdoCommandType_Insert);
	
	insert->SetFeatureClassName(className.c_str());

	FdoPtr<FdoPropertyValueCollection> propVals = insert->GetPropertyValues();
	
    if (NULL != fdoTrans.p)
        insert->SetTransaction(fdoTrans);

    //TODO: Support batch parameters, the main beneficiary of this API. Even then,
    //the value flipping approach employed here has performance benefits for certain
    //providers, like SQLite

    for (INT32 i = 0; i < batchPropertyValues->GetCount(); i++)
    {
        Ptr<MgPropertyCollection> propertyValues = batchPropertyValues->GetItem(i);

        //First feature, set up the FDO property value collection
        if (i == 0)
        {
            for (INT32 i = 0; i < propertyValues->GetCount(); i++)
	        {
		        Ptr<MgProperty> mgp = propertyValues->GetItem(i);
		        FdoPtr<FdoPropertyValue> pv = MgFeatureUtil::MgPropertyToFdoProperty(mgp);

		        propVals->Add(pv);
	        }
        }
        else //Feature after the first
        {
            //Set all to null
            for (INT32 i = 0; i < propVals->GetCount(); i++)
            {
                FdoPtr<FdoPropertyValue> fp = propVals->GetItem(i);
                FdoPtr<FdoValueExpression> expr = fp->GetValue();
                FdoDataValue* fdv = dynamic_cast<FdoDataValue*>(expr.p);
                FdoGeometryValue* fgv = dynamic_cast<FdoGeometryValue*>(expr.p);
                if (fdv)
                {
                    fdv->SetNull();
                }
                else if (fgv)
                {
                    fgv->SetNullValue();
                }
            }

            //Now set the appropriate values. MgFeatureUtil does the work
            for (INT32 i = 0; i < propertyValues->GetCount(); i++)
	        {
                Ptr<MgNullableProperty> mgp = (MgNullableProperty*)propertyValues->GetItem(i);
                if (!mgp->IsNull())
                {
                    FdoPtr<FdoPropertyValue> fp = propVals->GetItem(mgp->GetName().c_str());
                    MgFeatureUtil::UpdateFdoPropertyValue(fp, mgp);
                }
            }
        }

        STRING sIndex;
        MgUtil::Int32ToString(i, sIndex);

        //Insert and stash the result in the property collection
	    FdoPtr<FdoIFeatureReader> insertRes = insert->Execute();

	    Ptr<MgFeatureReader> fr = new MgdFeatureReader(connWrap, insertRes);
        Ptr<MgFeatureProperty> fp = new MgFeatureProperty(sIndex, fr);
        ret->Add(fp);
    }

    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH_AND_THROW(resource, L"MgdFeatureService::InsertFeatures")
    return ret.Detach();
}

int MgdFeatureService::UpdateFeatures(MgResourceIdentifier* resource, CREFSTRING className, MgPropertyCollection* propertyValues, CREFSTRING filter)
{
    int ret = 0;
    MG_FEATURE_SERVICE_TRY()
	ret = UpdateFeatures(resource, className, propertyValues, filter, NULL);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureService::UpdateFeatures")
    return ret;
}

int MgdFeatureService::UpdateFeatures(MgResourceIdentifier* resource, CREFSTRING className, MgPropertyCollection* propertyValues, CREFSTRING filter, MgTransaction* trans)
{
    CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdFeatureService::UpdateFeatures");
	CHECKARGUMENTNULL(propertyValues, L"MgdFeatureService::UpdateFeatures");
	if (className.empty())
		throw new MgNullArgumentException(L"MgdFeatureService::UpdateFeatures", __LINE__, __WFILE__, NULL, L"", NULL);

	int updated = 0;

    MG_FEATURE_SERVICE_TRY()

    Ptr<MgFeatureConnection> connWrap;
	FdoPtr<FdoIConnection> conn;
    FdoPtr<FdoITransaction> fdoTrans;
    Ptr<MgdTransaction> mgTrans = dynamic_cast<MgdTransaction*>(trans);
    if (NULL != mgTrans)
    {
        SAFE_ADDREF(mgTrans.p);
        Ptr<MgResourceIdentifier> origFeatureSource = mgTrans->GetFeatureSource();
        //Check that the transaction originates from the same feature source
        if (origFeatureSource->ToString() != resource->ToString())
            throw new MgInvalidArgumentException(L"MgdFeatureService::UpdateFeatures", __LINE__, __WFILE__, NULL, L"", NULL);

        connWrap = mgTrans->GetConnection(); //Connection is already open
        fdoTrans = mgTrans->GetFdoTransaction();
    }
    else
    {
        connWrap = new MgFeatureConnection(resource);
    }

    conn = connWrap->GetConnection();
	FdoPtr<FdoIUpdate> update = (FdoIUpdate*)conn->CreateCommand(FdoCommandType_Update);
	update->SetFeatureClassName(className.c_str());
	
	if (!filter.empty())
		update->SetFilter(filter.c_str());

    if (NULL != fdoTrans.p)
        update->SetTransaction(fdoTrans);

	FdoPtr<FdoPropertyValueCollection> propVals = update->GetPropertyValues();
	for (INT32 i = 0; i < propertyValues->GetCount(); i++)
	{
		Ptr<MgProperty> mgp = propertyValues->GetItem(i);
		FdoPtr<FdoPropertyValue> pv = MgFeatureUtil::MgPropertyToFdoProperty(mgp);

		propVals->Add(pv);
	}

	updated = update->Execute();

    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH_AND_THROW(resource, L"MgdFeatureService::UpdateFeatures")

	return updated;
}

int MgdFeatureService::DeleteFeatures(MgResourceIdentifier* resource, CREFSTRING className, CREFSTRING filter)
{
    int ret = 0;
    MG_FEATURE_SERVICE_TRY()
	ret = DeleteFeatures(resource, className, filter, NULL);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureService::DeleteFeatures")
    return ret;
}

int MgdFeatureService::DeleteFeatures(MgResourceIdentifier* resource, CREFSTRING className, CREFSTRING filter, MgTransaction* trans)
{
    CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdFeatureService::DeleteFeatures");
	if (className.empty())
		throw new MgNullArgumentException(L"MgdFeatureService::DeleteFeatures", __LINE__, __WFILE__, NULL, L"", NULL);

    int deleted = 0;

    MG_FEATURE_SERVICE_TRY()

    Ptr<MgFeatureConnection> connWrap;
	FdoPtr<FdoIConnection> conn;
    FdoPtr<FdoITransaction> fdoTrans;

    Ptr<MgdTransaction> mgTrans = dynamic_cast<MgdTransaction*>(trans);
    if (NULL != mgTrans)
    {
        SAFE_ADDREF(mgTrans.p);
        Ptr<MgResourceIdentifier> origFeatureSource = mgTrans->GetFeatureSource();
        //Check that the transaction originates from the same feature source
        if (origFeatureSource->ToString() != resource->ToString())
            throw new MgInvalidArgumentException(L"MgdFeatureService::DeleteFeatures", __LINE__, __WFILE__, NULL, L"", NULL);

        connWrap = mgTrans->GetConnection(); //Connection is already open
        fdoTrans = mgTrans->GetFdoTransaction();
    }
    else
    {
        connWrap = new MgFeatureConnection(resource);
    }

    conn = connWrap->GetConnection();
	FdoPtr<FdoIDelete> fdoDelete = (FdoIDelete*)conn->CreateCommand(FdoCommandType_Delete);
	fdoDelete->SetFeatureClassName(className.c_str());
    if (!filter.empty())
	    fdoDelete->SetFilter(filter.c_str());
    
    if (NULL != fdoTrans.p)
        fdoDelete->SetTransaction(fdoTrans);

	deleted = fdoDelete->Execute();

    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH_AND_THROW(resource, L"MgdFeatureService::DeleteFeatures")

	return deleted;
}

void MgdFeatureService::PurgeCache(MgResourceIdentifier* resource)
{
    MG_FEATURE_SERVICE_TRY()
    MgFeatureServiceCache* cache = MgFeatureServiceCache::GetInstance();
    cache->RemoveEntry(resource);
    MgFdoConnectionPool::PurgeCachedConnections(resource);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureService::PurgeCache")
}

MgFeatureReader* MgdFeatureService::GetLockedFeatures(MgResourceIdentifier* resource,
                                                      CREFSTRING className,
                                                      MgFeatureQueryOptions* options) 
{ 
    Ptr<MgFeatureReader> ret;
    MG_FEATURE_SERVICE_TRY()
	ret = SelectFeaturesInternal(resource, className, options, L"", true, false);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureService::GetLockedFeatures")
    return ret.Detach();
}

MgTransaction* MgdFeatureService::BeginTransaction(MgResourceIdentifier* resource) 
{ 
	CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdFeatureService::BeginTransaction");

    Ptr<MgTransaction> trans;

    MG_FEATURE_SERVICE_TRY()

    Ptr<MgFeatureConnection> conn = new MgFeatureConnection(resource);
    trans = new MgdTransaction(conn, resource);

    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH_AND_THROW(resource, L"MgdFeatureService::BeginTransaction")

    return trans.Detach();
}

MgSqlDataReader* MgdFeatureService::ExecuteSqlQuery(MgResourceIdentifier* resource,
                                         CREFSTRING sqlStatement) 
{ 
	CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdFeatureService::ExecuteSqlQuery");
    if (sqlStatement.empty())
        throw new MgInvalidArgumentException(L"MgdFeatureService::ExecuteSqlQuery", __LINE__, __WFILE__, NULL, L"", NULL);

    Ptr<MgSqlDataReader> reader;
    MG_FEATURE_SERVICE_TRY()

	Ptr<MgFeatureConnection> connWrap = new MgFeatureConnection(resource);
    FdoPtr<FdoIConnection> conn = connWrap->GetConnection();

	FdoPtr<FdoISQLCommand> sql = (FdoISQLCommand*)conn->CreateCommand(FdoCommandType_SQLCommand);
	sql->SetSQLStatement(sqlStatement.c_str());

	FdoPtr<FdoISQLDataReader> fdoReader = sql->ExecuteReader();
	reader = new MgdSqlDataReader(connWrap, fdoReader);

    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH_AND_THROW(resource, L"MgdFeatureService::ExecuteSqlQuery")

	return reader.Detach();
}

MgSqlDataReader* MgdFeatureService::ExecuteSqlQuery(MgResourceIdentifier* resource,
                                                    CREFSTRING sqlStatement,
                                                    MgParameterCollection* params,
                                                    MgTransaction* trans) 
{
    Ptr<MgSqlDataReader> ret;
    MG_FEATURE_SERVICE_TRY()
    ret = ExecuteSqlQuery(resource, sqlStatement, params, trans, m_nDataCacheSize);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureService::ExecuteSqlQuery")
    return ret.Detach();
}

MgSqlDataReader* MgdFeatureService::ExecuteSqlQuery(MgResourceIdentifier* resource,
                                                     CREFSTRING sqlStatement,
                                                     MgParameterCollection* params,
                                                     MgTransaction* trans,
                                                     INT32 fetchSize)
{
    Ptr<MgSqlDataReader> reader;
    MG_FEATURE_SERVICE_TRY()

	CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdFeatureService::ExecuteSqlQuery");
    if (sqlStatement.empty())
        throw new MgInvalidArgumentException(L"MgdFeatureService::ExecuteSqlQuery", __LINE__, __WFILE__, NULL, L"", NULL);

    CHECKARGUMENTNULL(params, L"MgdFeatureService::ExecuteSqlQuery");
    CHECKARGUMENTNULL(trans, L"MgdFeatureService::ExecuteSqlQuery");

    Ptr<MgFeatureConnection> connWrap;
	FdoPtr<FdoIConnection> conn;
    FdoPtr<FdoITransaction> fdoTrans;
    Ptr<MgdTransaction> mgTrans = dynamic_cast<MgdTransaction*>(trans);
    if (NULL != mgTrans)
    {
        SAFE_ADDREF(mgTrans.p);
        Ptr<MgResourceIdentifier> origFeatureSource = mgTrans->GetFeatureSource();
        //Check that the transaction originates from the same feature source
        if (origFeatureSource->ToString() != resource->ToString())
            throw new MgInvalidArgumentException(L"MgdFeatureService::InsertFeatures", __LINE__, __WFILE__, NULL, L"", NULL);

        connWrap = mgTrans->GetConnection(); //Connection is already open
        fdoTrans = mgTrans->GetFdoTransaction();
    }
    else
    {    
        connWrap = new MgFeatureConnection(resource);
    }

    conn = connWrap->GetConnection();

    // Create the SQL command
    FdoPtr<FdoISQLCommand> fdoCommand = (FdoISQLCommand*)conn->CreateCommand(FdoCommandType_SQLCommand);
    CHECKNULL((FdoISQLCommand*)fdoCommand, L"MgServerSqlCommand.ExecuteQuery");

    // Set SQL statement
    fdoCommand->SetSQLStatement((FdoString*)sqlStatement.c_str());

    // Set fetch size
    fdoCommand->SetFetchSize(fetchSize);

    // Set parameters
    FdoPtr<FdoParameterValueCollection> fdoParams = NULL;
    if (NULL != params && params->GetCount() > 0)
    {
        fdoParams = fdoCommand->GetParameterValues();
        MgFeatureUtil::FillFdoParameterCollection(params, fdoParams);
    }

    // Execute the command
    FdoPtr<FdoISQLDataReader> sqlReader = fdoCommand->ExecuteReader();
    CHECKNULL((FdoISQLDataReader*)sqlReader, L"MgServerSqlCommand.ExecuteQuery");

    // Update parameter whose direction is InputOutput, Output, or Return.
    if (NULL != params && params->GetCount() > 0)
        MgFeatureUtil::UpdateParameterCollection(fdoParams, params);

    reader = new MgdSqlDataReader(connWrap, sqlReader);

    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH_AND_THROW(resource, L"MgdFeatureService::ExecuteSqlQuery")
    return reader.Detach();
}

INT32 MgdFeatureService::ExecuteSqlNonQuery(MgResourceIdentifier* resource,
                                            CREFSTRING sqlNonSelectStatement,
                                            MgParameterCollection* params,
                                            MgTransaction* trans)
{ 
    INT32 ret = 0;
    MG_FEATURE_SERVICE_TRY()

    CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdFeatureService::ExecuteSqlNonQuery");
    if (sqlNonSelectStatement.empty())
        throw new MgInvalidArgumentException(L"MgdFeatureService::ExecuteSqlNonQuery", __LINE__, __WFILE__, NULL, L"", NULL);

    CHECKARGUMENTNULL(params, L"MgdFeatureService::ExecuteSqlQuery");
    CHECKARGUMENTNULL(trans, L"MgdFeatureService::ExecuteSqlQuery");

    Ptr<MgFeatureConnection> connWrap;
	FdoPtr<FdoIConnection> conn;
    FdoPtr<FdoITransaction> fdoTrans;
    Ptr<MgdTransaction> mgTrans = dynamic_cast<MgdTransaction*>(trans);
    if (NULL != mgTrans)
    {
        SAFE_ADDREF(mgTrans.p);
        Ptr<MgResourceIdentifier> origFeatureSource = mgTrans->GetFeatureSource();
        //Check that the transaction originates from the same feature source
        if (origFeatureSource->ToString() != resource->ToString())
            throw new MgInvalidArgumentException(L"MgdFeatureService::InsertFeatures", __LINE__, __WFILE__, NULL, L"", NULL);

        connWrap = mgTrans->GetConnection(); //Connection is already open
        fdoTrans = mgTrans->GetFdoTransaction();
    }
    else
    {    
        connWrap = new MgFeatureConnection(resource);
    }

    conn = connWrap->GetConnection();

    // Create the SQL command
    FdoPtr<FdoISQLCommand> fdoCommand = (FdoISQLCommand*)conn->CreateCommand(FdoCommandType_SQLCommand);
    CHECKNULL((FdoISQLCommand*)fdoCommand, L"MgServerSqlCommand.ExecuteQuery");

    // Set SQL statement
    fdoCommand->SetSQLStatement((FdoString*)sqlNonSelectStatement.c_str());

    // Set parameters
    FdoPtr<FdoParameterValueCollection> fdoParams = NULL;
    if (NULL != params && params->GetCount() > 0)
    {
        fdoParams = fdoCommand->GetParameterValues();
        MgFeatureUtil::FillFdoParameterCollection(params, fdoParams);
    }

    // Execute the command
    ret = fdoCommand->ExecuteNonQuery();

    // Update parameter whose direction is InputOutput, Output, or Return.
    if (NULL != params && params->GetCount() > 0)
        MgFeatureUtil::UpdateParameterCollection(fdoParams, params);

    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH_AND_THROW(resource, L"MgdFeatureService::ExecuteSqlNonQuery")
    return ret;
}

INT32 MgdFeatureService::ExecuteSqlNonQuery(MgResourceIdentifier* resource,
                                            CREFSTRING sqlNonSelectStatement) 
{ 
	CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdFeatureService::ExecuteSqlNonQuery");
    if (sqlNonSelectStatement.empty())
        throw new MgInvalidArgumentException(L"MgdFeatureService::ExecuteSqlNonQuery", __LINE__, __WFILE__, NULL, L"", NULL);

    INT32 ret = 0;

    MG_FEATURE_SERVICE_TRY()

    Ptr<MgFeatureConnection> connWrap = new MgFeatureConnection(resource);
	FdoPtr<FdoIConnection> conn = connWrap->GetConnection();

	FdoPtr<FdoISQLCommand> sql = (FdoISQLCommand*)conn->CreateCommand(FdoCommandType_SQLCommand);
	sql->SetSQLStatement(sqlNonSelectStatement.c_str());

    ret = sql->ExecuteNonQuery();

    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH_AND_THROW(resource, L"MgdFeatureService::ExecuteSqlNonQuery")

	return ret;
}

MgSpatialContextReader* MgdFeatureService::GetSpatialContexts(MgResourceIdentifier* resId, bool bActiveOnly) 
{
	CHECK_FEATURE_SOURCE_ARGUMENT(resId, L"MgdFeatureService::GetSpatialContexts");
	Ptr<MgSpatialContextReader> mgSpatialContextReader;

    MG_FEATURE_SERVICE_TRY()

    MgFeatureServiceCache* cache = MgFeatureServiceCache::GetInstance();
    mgSpatialContextReader = cache->GetSpatialContextReader(resId);

    if (NULL == mgSpatialContextReader.p)
    {
        // Connect to provider
        Ptr<MgFeatureConnection> msfc = new MgFeatureConnection(resId);

        // Connection must be open to retrieve a list of available contexts.
        if ((NULL != msfc.p) && ( msfc->IsConnectionOpen() ))
        {
            // The reference to the FDO connection from the MgFeatureConnection object must be cleaned up before the parent object
            // otherwise it leaves the FDO connection marked as still in use.
            FdoPtr<FdoIConnection> fdoConn = msfc->GetConnection();
            STRING providerName = msfc->GetProviderName();

            Ptr<MgSpatialContextCacheItem> cacheItem = cache->GetSpatialContextInfo(resId);
            MgSpatialContextInfo* spatialContextInfo = cacheItem->Get();

            // Check whether command is supported by provider
            if (!msfc->SupportsCommand((INT32)FdoCommandType_GetSpatialContexts))
            {
                // TODO: specify which argument and message, once we have the mechanism
                STRING message = MgFeatureUtil::GetMessage(L"MgCommandNotSupported");
                throw new MgInvalidOperationException(L"MgdFeatureService::GetSpatialContexts", __LINE__, __WFILE__, NULL, L"", NULL);
            }

            FdoPtr<FdoIGetSpatialContexts> fdoCommand = (FdoIGetSpatialContexts*)fdoConn->CreateCommand(FdoCommandType_GetSpatialContexts);
            CHECKNULL((FdoIGetSpatialContexts*)fdoCommand, L"MgdFeatureService::GetSpatialContexts");

            // Execute the command
            FdoPtr<FdoISpatialContextReader> spatialReader = fdoCommand->Execute();
            CHECKNULL((FdoISpatialContextReader*)spatialReader, L"MgdFeatureService::GetSpatialContexts");

            mgSpatialContextReader = new MgSpatialContextReader();
            while (spatialReader->ReadNext())
            {
                // Set providername for which spatial reader is executed
                mgSpatialContextReader->SetProviderName(providerName);

                Ptr<MgSpatialContextData> spatialData = GetSpatialContextData(spatialReader, spatialContextInfo);
                CHECKNULL((MgSpatialContextData*)spatialData, L"MgdFeatureService::GetSpatialContexts");

                // Add spatial data to the spatialcontext reader
                mgSpatialContextReader->AddSpatialData(spatialData);
            }

            cache->SetSpatialContextReader(resId, mgSpatialContextReader.p);
        }
        else
        {
            throw new MgConnectionFailedException(L"MgdFeatureService::GetSpatialContexts()", __LINE__, __WFILE__, NULL, L"", NULL);
        }
    }
    else
    {
        //cache->CheckPermission(resId, MgResourcePermission::ReadOnly);
    }

    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH_AND_THROW(resId, L"MgdFeatureService::GetSpatialContexts")

    return mgSpatialContextReader.Detach();
}

MgSpatialContextData* MgdFeatureService::GetSpatialContextData(FdoISpatialContextReader* spatialReader, MgSpatialContextInfo* spatialContextInfo)
{
    Ptr<MgSpatialContextData> spatialData;
    MG_FEATURE_SERVICE_TRY()

    spatialData = new MgSpatialContextData();

    // Name must exist
    FdoString* name = spatialReader->GetName();
    CHECKNULL((FdoString*)name, L"MgdFeatureService::GetSpatialContexts");
    spatialData->SetName(STRING(name));

    STRING coordSysName = L"";
    FdoString* csName = spatialReader->GetCoordinateSystem();

    Ptr<MgCoordinateSystemFactory> csFactory;
    // WKT for co-ordinate system
    FdoString* csWkt = NULL;
    STRING srsWkt = L"";

    bool haveValidCoordSys = false;
    if(NULL != csName && *csName != '\0')
    {
        coordSysName = STRING(csName);
    }
    else
    {
        csWkt = spatialReader->GetCoordinateSystemWkt();
        if (csWkt != NULL && *csWkt != '\0')
        {
            srsWkt = csWkt;
            try
            {
                csFactory = new MgCoordinateSystemFactory();
                coordSysName = csFactory->ConvertWktToCoordinateSystemCode(srsWkt);
                haveValidCoordSys = (coordSysName.size() != 0);
            }
            catch (MgException* e)
            {
                SAFE_RELEASE(e);
            }
            catch(...)
            {
            }
        }
    }

    bool spatialContextDefined = !coordSysName.empty();
    bool coordSysOverridden = false;

    // look for coordinate system override
    if (NULL != spatialContextInfo)
    {
        // Perform substitution of missing coordinate system with
        // the spatial context mapping defined in feature source document
        MgSpatialContextInfo::const_iterator iter = spatialContextInfo->find(name);

        if (spatialContextInfo->end() != iter)
        {
            csName = (iter->second).c_str();
            coordSysOverridden = true;
        }
    }

    if (csName != NULL && *csName != '\0')
    {
        spatialData->SetCoordinateSystem(STRING(csName));
    }

    // Desc for spatial context
    STRING desc = L"";

    // This flag is obsolete and will be deprecated.
    bool isActive = spatialReader->IsActive();

    if (coordSysOverridden)
    {
        srsWkt = csName;
        desc = MgFeatureUtil::GetMessage(L"MgCoordinateSystemOverridden");
    }
    else if (spatialContextDefined && !coordSysOverridden)
    {
        // avoid looking one more time after CS in case we have a valid one...
        if (!haveValidCoordSys)
        {
            csWkt = spatialReader->GetCoordinateSystemWkt();
            if(NULL != csWkt && *csWkt != '\0')
                srsWkt = csWkt;

            if (srsWkt.empty())
            {
                try
                {
                    if (csFactory == NULL)
                        csFactory = new MgCoordinateSystemFactory();

                    // Check if the spatial context coordinate system data represents an EPSG
                    // code. If this is the case the WKT data for the EPSG code has to be
                    // retrieved.
                    if (IsEpsgCodeRepresentation(csName))
                    {
                        // This is an EPSG code
                        FdoString* p = NULL;
                        if ((csName[0] == L'E') || (csName[0] == L'e'))
                            p = csName+5;
                        else
                            p = csName;

                        INT32 epsgCode = (INT32)wcstol(p, NULL, 10);

                        // Convert the EPSG numerical code to WKT
                        srsWkt = csFactory->ConvertEpsgCodeToWkt(epsgCode);
                    }
                    else
                    {
                        srsWkt = csFactory->ConvertCoordinateSystemCodeToWkt(STRING(csName));
                    }
                }
                catch (MgException* e)
                {
                    SAFE_RELEASE(e);
                }
                catch(...)
                {
                    // Just use the empty WKT.
                }
            }
        }
        FdoString* fdoDesc = spatialReader->GetDescription();
        if(NULL != fdoDesc)
            desc = STRING(fdoDesc);
    }

    // retrieve other values from spatialReader
    FdoSpatialContextExtentType extentType = spatialReader->GetExtentType();
    FdoPtr<FdoByteArray> byteArray = spatialReader->GetExtent();
    double xyTol = spatialReader->GetXYTolerance();
    double zTol = spatialReader->GetZTolerance();

    spatialData->SetCoordinateSystemWkt(srsWkt);
    spatialData->SetDescription(desc);
    spatialData->SetExtentType((INT32)extentType);

    if (byteArray.p != NULL)
    {
        INT32 size = (INT32)byteArray->GetCount();
        BYTE_ARRAY_IN bytes = (BYTE_ARRAY_IN)byteArray->GetData();
        Ptr<MgByte> extent = new MgByte(bytes, size);
        spatialData->SetExtent(extent);
    }

    // XY Tolerance
    spatialData->SetXYTolerance(xyTol);

    // Z Tolerance
    spatialData->SetZTolerance(zTol);

    // This flag is obsolete and will be deprecated.
    spatialData->SetActiveStatus(isActive);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureService::GetSpatialContextData")
    return spatialData.Detach();
}

bool MgdFeatureService::IsEpsgCodeRepresentation (FdoString *coordSysName)
{
    // If the given coordinate system name is NULL or not NULL but empty
    // return false as those cases do not represent an EPSG code.
    if ((coordSysName == NULL) || (coordSysName[0] == L'\0'))
        return false;

    // Check if the string contains a coded EPSG number ("EPSG:<num>"). If this
    // is the case return true;
    size_t coordSysNameLength  = wcslen(coordSysName);
    if (coordSysNameLength  > 5)
        if (((coordSysName[0] == L'E') || (coordSysName[0] == L'e')) &&
            ((coordSysName[1] == L'P') || (coordSysName[1] == L'p')) &&
            ((coordSysName[2] == L'S') || (coordSysName[2] == L's')) &&
            ((coordSysName[3] == L'G') || (coordSysName[3] == L'g')) &&
            (coordSysName[4] == L':')                                   )
            return true;

    // The string does not contain a coded EPSG number. Next, check if the
    // string represents a numeric value. If this is the case then the value
    // represents an EPSG code and the routine has to return true.
    for (size_t i = 0; i < coordSysNameLength; i++)
        if (!iswdigit(coordSysName[i]))
            return false;

    // This is an EPSG code. Indicate so.
    return true;
}


MgLongTransactionReader* MgdFeatureService::GetLongTransactions(MgResourceIdentifier* resource,
                                                                bool bActiveOnly) 
{ 
	CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdFeatureService::GetLongTransactions");
	Ptr<MgLongTransactionReader> reader;

    MG_FEATURE_SERVICE_TRY()

    Ptr<MgFeatureConnection> connWrap = new MgFeatureConnection(resource);
	FdoPtr<FdoIConnection> conn = connWrap->GetConnection();

	FdoPtr<FdoIGetLongTransactions> list = (FdoIGetLongTransactions*)conn->CreateCommand(FdoCommandType_GetLongTransactions);
	FdoPtr<FdoILongTransactionReader> ltReader = list->Execute();

	reader = new MgLongTransactionReader();

	while (ltReader->ReadNext())
	{
		Ptr<MgLongTransactionData> data = new MgLongTransactionData();
		data->SetActiveStatus(ltReader->IsActive());
		FdoDateTime fdt = ltReader->GetCreationDate();
		data->SetCreationDate(new MgDateTime(fdt.year, fdt.month, fdt.day, fdt.hour, fdt.minute, fdt.seconds, 0));
		data->SetDescription(ltReader->GetDescription());
		data->SetFrozenStatus(ltReader->IsFrozen());
		data->SetName(ltReader->GetName());
		data->SetOwner(ltReader->GetOwner());
	
		reader->AddLongTransactionData(data);
	}
	ltReader->Close();

    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH_AND_THROW(resource, L"MgdFeatureService::GetLongTransactions")

	return reader.Detach();
}

bool MgdFeatureService::SetLongTransaction(MgResourceIdentifier* resource,
                                CREFSTRING longTransactionName) 
{ 
	CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdFeatureService::SetLongTransaction");
    if (longTransactionName.empty())
        throw new MgNullArgumentException(L"MgdFeatureService::SetLongTransaction", __LINE__, __WFILE__, NULL, L"", NULL);

	bool ok = false;

    MG_FEATURE_SERVICE_TRY()

	Ptr<MgFeatureConnection> connWrap = new MgFeatureConnection(resource);
	FdoPtr<FdoIConnection> conn = connWrap->GetConnection();
	FdoPtr<FdoIActivateLongTransaction> activate = (FdoIActivateLongTransaction*)conn->CreateCommand(FdoCommandType_ActivateLongTransaction);
	activate->SetName(longTransactionName.c_str());
	activate->Execute();
	ok = true;

    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH_AND_THROW(resource, L"MgdFeatureService::SetLongTransaction")

	return ok;
}

bool MgdFeatureService::SupportsPartialSchemaDiscovery(FdoIConnection* conn)
{
	CHECKARGUMENTNULL(conn, L"MgdFeatureService::SupportsPartialSchemaDiscovery");

    bool getClassNames = false;
	bool getSchemaNames = false;

    MG_FEATURE_SERVICE_TRY()
	FdoPtr<FdoICommandCapabilities> caps = conn->GetCommandCapabilities();

	FdoInt32 cmdCount;
	FdoInt32* cmds = caps->GetCommands(cmdCount);
	for (FdoInt32 i = 0; i < cmdCount; i++)
	{
		if (cmds[i] == FdoCommandType_GetClassNames)
			getClassNames = true;
		else if (cmds[i] == FdoCommandType_GetSchemaNames)
			getSchemaNames = true;
	}
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureService::SupportsPartialSchemaDiscovery")

	return getClassNames && getSchemaNames;
}

MgStringCollection* MgdFeatureService::GetSchemas(MgResourceIdentifier* resource) 
{ 
	CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdFeatureService::GetSchemas");

    Ptr<MgStringCollection> schemaNames;
    MG_FEATURE_SERVICE_TRY()

    Ptr<MgFeatureConnection> connWrap = new MgFeatureConnection(resource);
	FdoPtr<FdoIConnection> conn = connWrap->GetConnection();

	if (SupportsPartialSchemaDiscovery(conn))
	{
		FdoPtr<FdoIGetSchemaNames> fetch = (FdoIGetSchemaNames*)conn->CreateCommand(FdoCommandType_GetSchemaNames);
		FdoPtr<FdoStringCollection> names = fetch->Execute();

		schemaNames = MgFeatureUtil::FdoToMgStringCollection(names, false);
	}
	else
	{
		Ptr<MgStringCollection> names = new MgStringCollection();
		FdoPtr<FdoIDescribeSchema> describe = (FdoIDescribeSchema*)conn->CreateCommand(FdoCommandType_DescribeSchema);
		FdoPtr<FdoFeatureSchemaCollection> schemas = describe->Execute();

		for (INT32 i = 0; i < schemas->GetCount(); i++)
		{
			FdoPtr<FdoFeatureSchema> schema = schemas->GetItem(i);
			STRING name = schema->GetName();
			names->Add(name);
		}

		schemaNames = names;
	}

    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH_AND_THROW(resource, L"MgdFeatureService::GetSchemas")

    return schemaNames.Detach();
}

MgStringCollection* MgdFeatureService::GetClasses(MgResourceIdentifier* resource, CREFSTRING schemaName) 
{ 
	CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdFeatureService::GetClasses");
	if (schemaName.empty())
		throw new MgInvalidArgumentException(L"MgdFeatureService::GetClasses", __LINE__, __WFILE__, NULL, L"", NULL);

    Ptr<MgStringCollection> classNames;
    MG_FEATURE_SERVICE_TRY()

    Ptr<MgFeatureConnection> connWrap = new MgFeatureConnection(resource);
	FdoPtr<FdoIConnection> conn = connWrap->GetConnection();

	if (SupportsPartialSchemaDiscovery(conn))
	{
		FdoPtr<FdoIGetClassNames> fetch = (FdoIGetClassNames*)conn->CreateCommand(FdoCommandType_GetClassNames);
		fetch->SetSchemaName(schemaName.c_str());
		FdoPtr<FdoStringCollection> names = fetch->Execute();

		classNames = MgFeatureUtil::FdoToMgStringCollection(names, false);
	}
	else
	{
		Ptr<MgStringCollection> names = new MgStringCollection();
		FdoPtr<FdoIDescribeSchema> describe = (FdoIDescribeSchema*)conn->CreateCommand(FdoCommandType_DescribeSchema);
		FdoPtr<FdoFeatureSchemaCollection> schemas = describe->Execute();

		for (INT32 i = 0; i < schemas->GetCount(); i++)
		{
			FdoPtr<FdoFeatureSchema> schema = schemas->GetItem(i);
			STRING name = schema->GetName();
			if (name != schemaName)
				continue;

			FdoPtr<FdoClassCollection> classes = schema->GetClasses();

			for (INT32 j = 0; j < classes->GetCount(); j++)
			{
				FdoPtr<FdoClassDefinition> cls = classes->GetItem(j);
				STRING clsName = cls->GetName();
				names->Add(clsName);
			}
		}

		classNames = names;
	}

    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH_AND_THROW(resource, L"MgdFeatureService::GetClasses")

    return classNames.Detach();
}

MgClassDefinition* MgdFeatureService::GetClassDefinition(MgResourceIdentifier* resource,
                                                         CREFSTRING schemaName,
                                                         CREFSTRING className) 
{ 
    Ptr<MgClassDefinition> ret;
    MG_FEATURE_SERVICE_TRY()
	ret = GetClassDefinition(resource, schemaName, className, true);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureService::GetClassDefinition")
    return ret.Detach();
}

//Though internal (and thus, normally wouldn't be implemented), it's needed by MgMappingUtil
MgClassDefinition* MgdFeatureService::GetClassDefinition(MgResourceIdentifier* resource,
                                              CREFSTRING schemaName,
                                              CREFSTRING className,
                                              bool serialize) 
{
    CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdFeatureService::GetClassDefinition");
	
	Ptr<MgClassDefinition> classDefinition;
    Ptr<MgClassDefinition> clone;

    MG_FEATURE_SERVICE_TRY()

    MgFeatureServiceCache* cache = MgFeatureServiceCache::GetInstance();
    if (className.empty())
    {
        throw new MgClassNotFoundException(
            L"MgdFeatureService::GetClassDefinition",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    classDefinition = cache->GetClassDefinition(resource, schemaName, className);

    if (NULL == classDefinition.p)
    {
        Ptr<MgStringCollection> classNames;

        // Since the FDO provider does not know about the join information,
        // use the full schema to retrieve the class definition if the feature source has joins.
        if (!CheckExtendedFeatureClass(resource, className))
        {
            classNames = new MgStringCollection();
            classNames->Add(className);
        }

        Ptr<MgFeatureSchemaCollection> schemas = DescribeSchema(
            resource, schemaName, classNames, serialize);

        // Get the class definition.
        classDefinition = GetClassDefinition(schemas.p, schemaName, className);

        if (NULL == classDefinition.p)
        {
            throw new MgClassNotFoundException(
                L"MgdFeatureService::GetClassDefinition",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }
        else
        {
            cache->SetClassDefinition(resource, schemaName, className, classDefinition.p);
        }
    }
    else
    {
        //m_cacheManager->CheckPermission(resource, MgResourcePermission::ReadOnly);
    }

    clone = MgFeatureUtil::CloneMgClassDefinition(classDefinition);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureService::GetClassDefinition")

    return clone.Detach();
}

MgClassDefinition* MgdFeatureService::GetClassDefinition(MgFeatureSchemaCollection* schemas, 
                                                         CREFSTRING schemaName, 
                                                         CREFSTRING className)
{
    CHECKNULL(schemas, L"MgdFeatureService::GetClassDefinition");

    Ptr<MgClassDefinition> classDef;

    MG_FEATURE_SERVICE_TRY()
    INT32 schemaCount = schemas->GetCount();

    for (INT32 i = 0; i < schemaCount; ++i)
    {
        Ptr<MgFeatureSchema> currSchema = schemas->GetItem(i);
        STRING currSchemaName = currSchema->GetName();
        bool schemaNameFound = false;

        if (schemaName.empty() || (schemaNameFound = (schemaName == currSchemaName)))
        {
            Ptr<MgClassDefinitionCollection> currClasses = currSchema->GetClasses();
            INT32 classCount = currClasses->GetCount();

            for (INT32 j = 0; j < classCount; ++j)
            {
                Ptr<MgClassDefinition> currClass = currClasses->GetItem(j);
                STRING currClassName = currClass->GetName();

                if (className == currClassName)
                {
                    classDef = currClass;
                    break;
                }
                else
                {
                    STRING parsedSchemaName, parsedClassName;
                    MgUtil::ParseQualifiedClassName(className, parsedSchemaName, parsedClassName);

                    if (parsedClassName == currClassName)
                    {
                        classDef = MgFeatureUtil::CloneMgClassDefinition(currClass);
                        break;
                    }
                }
            }
        }

        if (schemaNameFound || NULL != classDef.p)
        {
            break;
        }
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureService::GetClassDefinition")

    return classDef.Detach();
}

void MgdFeatureService::CreateFeatureSource(MgResourceIdentifier* resource, MgFeatureSourceParams* sourceParams) 
{ 
    CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdFeatureService::CreateFeatureSource");
    CHECKARGUMENTNULL(sourceParams, L"MgdFeatureService::CreateFeatureSource");

    MG_FEATURE_SERVICE_TRY()

    Ptr<MgCreateFeatureSource> creator = new MgCreateFeatureSource();
    creator->CreateFeatureSource(resource, sourceParams);

    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH_AND_THROW(resource, L"MgCreateFeatureSource.CreateFeatureSource")
}

MgByteReader* MgdFeatureService::DescribeWfsFeatureType(MgResourceIdentifier* featureSourceId,
                                                        MgStringCollection* featureClasses) 
{ 
	NOT_IMPLEMENTED(L"MgdFeatureService::DescribeWfsFeatureType");
}

MgByteReader* MgdFeatureService::DescribeWfsFeatureType(MgResourceIdentifier* featureSourceId,
                                                        MgStringCollection* featureClasses,
                                                        CREFSTRING namespacePrefix,
                                                        CREFSTRING namespaceUrl) 
{ 
	NOT_IMPLEMENTED(L"MgdFeatureService::DescribeWfsFeatureType");
}

MgByteReader* MgdFeatureService::GetWfsFeature(MgResourceIdentifier* featureSourceId,
                                               CREFSTRING featureClass,
                                               MgStringCollection* requiredProperties,
                                               CREFSTRING srs,
                                               CREFSTRING wfsFilter,
                                               INT32 maxFeatures) 
{ 
    Ptr<MgByteReader> ret;
    MG_FEATURE_SERVICE_TRY()
	ret = GetWfsFeature(featureSourceId, featureClass, requiredProperties, srs, wfsFilter, maxFeatures, L"1.0.0", L"text/xml; subtype=gml/2.1.2", L"", L"", L"");
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureService::GetWfsFeature")
    return ret;
}

MgByteReader* MgdFeatureService::GetWfsFeature(MgResourceIdentifier* featureSourceId,
                                               CREFSTRING featureClass,
                                               MgStringCollection* requiredProperties,
                                               CREFSTRING srs,
                                               CREFSTRING filter,
                                               INT32 maxFeatures,
                                               CREFSTRING wfsVersion,
                                               CREFSTRING outputFormat,
                                               CREFSTRING sortCriteria,
                                               CREFSTRING namespacePrefix,
                                               CREFSTRING namespaceUrl) 
{ 
	NOT_IMPLEMENTED(L"MgdFeatureService::GetWfsFeature");
}

MgByteReader* MgdFeatureService::EnumerateDataStores(CREFSTRING providerName, CREFSTRING partialConnString) 
{ 
    Ptr<MgByteReader> reader;
    MG_FEATURE_SERVICE_TRY()

    if (providerName.empty())
        throw new MgInvalidArgumentException(L"MgdFeatureService::EnumerateDataStores", __LINE__, __WFILE__, NULL, L"", NULL);
    
    Ptr<MgFeatureConnection> connWrap = new MgFeatureConnection(providerName, partialConnString);
	FdoPtr<FdoIConnection> conn = connWrap->GetConnection();

	FdoPtr<FdoIListDataStores> list = (FdoIListDataStores*)conn->CreateCommand(FdoCommandType_ListDataStores);
	list->SetIncludeNonFdoEnabledDatastores(true);
	
	FdoPtr<FdoIDataStoreReader> dsReader = list->Execute();

	STRING xml = L"<DataStoreList>\n";

	while (dsReader->ReadNext())
	{
		xml += L"\t<DataStore>\n";
		xml += L"\t\t<Name>";
		xml += dsReader->GetName();
		xml += L"</Name>\n";
		xml += L"\t\t<FdoEnabled>\n";
		xml += dsReader->GetIsFdoEnabled() ? L"true" : L"false";
		xml += L"</FdoEnabled>\n";
		xml += L"\t</DataStore>\n";
	}
	dsReader->Close();

	xml += L"</DataStoreList>";

	string utf8Text = MgUtil::WideCharToMultiByte(xml);
    Ptr<MgByteSource> byteSource = new MgByteSource((BYTE_ARRAY_IN) utf8Text.c_str(), (INT32)utf8Text.length());
    byteSource->SetMimeType(MgMimeType::Xml);
	reader = byteSource->GetReader();

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureService::GetSchemaMapping")

    return reader.Detach();
}

MgByteReader* MgdFeatureService::GetSchemaMapping(CREFSTRING providerName, CREFSTRING partialConnString) 
{	
    Ptr<MgByteReader> byteReader;

	MG_FEATURE_SERVICE_TRY()

    Ptr<MgFeatureConnection> msfc = new MgFeatureConnection(providerName, partialConnString);
    
    if ((NULL != msfc.p) && (( msfc->IsConnectionOpen() ) || ( msfc->IsConnectionPending() )))
    {
        // The reference to the FDO connection from the MgFeatureConnection object must be cleaned up before the parent object
        // otherwise it leaves the FDO connection marked as still in use.
        FdoPtr<FdoIConnection> fdoConnection;
        fdoConnection = msfc->GetConnection();

        // Create the memory stream
        FdoIoMemoryStreamP fmis = FdoIoMemoryStream::Create();
        CHECKNULL((FdoIoMemoryStream*)fmis, L"MgdFeatureService::GetSchemaMapping");

        FdoXmlWriterP writer = FdoXmlWriter::Create(fmis);

        FdoPtr<FdoXmlSpatialContextFlags> flags = FdoXmlSpatialContextFlags::Create();
        flags->SetIncludeDefault(true);

        // Serialize the spatial contexts
        FdoXmlSpatialContextSerializer::XmlSerialize(
            fdoConnection,
            FdoXmlSpatialContextWriterP(
                FdoXmlSpatialContextWriter::Create(writer)
            ),
            flags
        );

        // Get the schema
        FdoPtr<FdoIDescribeSchema> fdoDescribeSchemaCommand = (FdoIDescribeSchema*)fdoConnection->CreateCommand(FdoCommandType_DescribeSchema);
        CHECKNULL((FdoIDescribeSchema*)fdoDescribeSchemaCommand, L"MgdFeatureService::GetSchemaMapping");

        // Execute the command
        FdoPtr<FdoFeatureSchemaCollection> fdoFeatureSchemaCollection;
        fdoFeatureSchemaCollection = fdoDescribeSchemaCommand->Execute();
        CHECKNULL((FdoFeatureSchemaCollection*)fdoFeatureSchemaCollection, L"MgdFeatureService::GetSchemaMapping");

        // Write to memory stream
        fdoFeatureSchemaCollection->WriteXml(writer);

        // Get the schema mapping
        FdoPtr<FdoIDescribeSchemaMapping> fdoDescribeSchemaMappingCommand = (FdoIDescribeSchemaMapping*)fdoConnection->CreateCommand(FdoCommandType_DescribeSchemaMapping);
        CHECKNULL((FdoIDescribeSchemaMapping*)fdoDescribeSchemaMappingCommand, L"MgdFeatureService::GetSchemaMapping");

        fdoDescribeSchemaMappingCommand->SetIncludeDefaults(true);

        // Execute the command
        FdoPtr<FdoPhysicalSchemaMappingCollection> fdoPhysicalSchemaMappingCollection;
        fdoPhysicalSchemaMappingCollection = fdoDescribeSchemaMappingCommand->Execute();
        CHECKNULL((FdoPhysicalSchemaMappingCollection*)fdoPhysicalSchemaMappingCollection, L"MgdFeatureService::GetSchemaMapping");

        // Write to memory stream
        fdoPhysicalSchemaMappingCollection->WriteXml(writer);

        // Close the XML writer
        writer->Close();

        fmis->Reset(); // TODO: We should not be calling reset here. A defect in FDO should be fixed.

        FdoInt64 len = fmis->GetLength();
        FdoPtr<FdoByteArray> bytes = FdoByteArray::Create((FdoInt32)len);
        //FdoByte* bytes = new FdoByte[(size_t)len];
        FdoByte* data = bytes->GetData();
        CHECKNULL(data, L"MgdFeatureService::GetSchemaMapping");
        fmis->Read(data, (FdoSize)len);

        Ptr<MgByteSource> byteSource = new MgByteSource((BYTE_ARRAY_IN)data, (INT32)len);
        byteSource->SetMimeType(MgMimeType::Xml);
        byteReader = byteSource->GetReader();
    }
    else
    {
        throw new MgConnectionFailedException(L"MgdFeatureService::GetSchemaMapping()", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureService::GetSchemaMapping")

    return byteReader.Detach();
}

void MgdFeatureService::RegisterProvider(CREFSTRING providerLibraryPath)
{
    MG_FEATURE_SERVICE_TRY()

    FdoPtr<IProviderRegistry> provReg = FdoFeatureAccessManager::GetProviderRegistry();
    FdoPtr<IConnectionManager> connMgr = FdoFeatureAccessManager::GetConnectionManager();

    //As of FDO 3.6 this supports FDO dll paths
    FdoPtr<FdoIConnection> conn = connMgr->CreateConnection(providerLibraryPath.c_str());
    FdoPtr<FdoIConnectionInfo> connInfo = conn->GetConnectionInfo();

    //FDO connections already have this information!
    provReg->RegisterProvider(connInfo->GetProviderName(),
                              connInfo->GetProviderDisplayName(),
                              connInfo->GetProviderDescription(),
                              connInfo->GetProviderVersion(),
                              connInfo->GetFeatureDataObjectsVersion(),
                              providerLibraryPath.c_str(),
                              false);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureService::RegisterProvider")
}

void MgdFeatureService::UnregisterProvider(CREFSTRING providerName)
{
    MG_FEATURE_SERVICE_TRY()

    FdoPtr<IProviderRegistry> reg = FdoFeatureAccessManager::GetProviderRegistry();
    reg->UnregisterProvider(providerName.c_str());

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureService::UnregisterProvider")
}

//INTERNAL_API:
//These API's don't need to be implemented as we're not a server process sending data down the wire

//Unfortunately, this is needed by MgdMap, so we have to implement it
MgPropertyDefinitionCollection* MgdFeatureService::GetIdentityProperties(MgResourceIdentifier* resource,
                                                                         CREFSTRING schemaName,
                                                                         CREFSTRING className)

{
    CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdFeatureService::GetIdentityProperties");
    Ptr<MgPropertyDefinitionCollection> propDefs;

    MG_FEATURE_SERVICE_TRY()

    if (className.empty())
    {
        throw new MgClassNotFoundException(
            L"MgdFeatureService::GetIdentityProperties",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MgFeatureServiceCache* cache = MgFeatureServiceCache::GetInstance();
    propDefs = cache->GetClassIdentityProperties(resource, schemaName, className);
    if (NULL == propDefs)
    {
        Ptr<MgClassDefinition> clsDef = cache->GetClassDefinition(resource, schemaName, className);
        if (NULL == clsDef)
        {
            clsDef = GetClassDefinition(resource, schemaName, className);

            //Cache for future calls
            cache->SetClassDefinition(resource, schemaName, className, clsDef);
        }

        propDefs = clsDef->GetIdentityProperties();
        //Cache for future calls
        cache->SetClassIdentityProperties(resource, schemaName, className, propDefs);
    }
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureService::GetIdentityProperties")

    return propDefs.Detach();
}

//Unfortunately, this is needed by MgdMap, so we have to implement it
//TODO: Check if this requires immutability
MgClassDefinitionCollection* MgdFeatureService::GetIdentityProperties(MgResourceIdentifier* resource,
                                                                      CREFSTRING schemaName,
                                                                      MgStringCollection* classNames)
{
    CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdFeatureService::GetIdentityProperties");
    Ptr<MgClassDefinitionCollection> classDefs;

    MG_FEATURE_SERVICE_TRY()

    classDefs = new MgClassDefinitionCollection();
    if (NULL == classNames || classNames->GetCount() == 0)
    {
        throw new MgClassNotFoundException(
            L"MgdFeatureService::GetIdentityProperties",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MgFeatureServiceCache* cache = MgFeatureServiceCache::GetInstance();
    Ptr<MgStringCollection> uncachedClasses = new MgStringCollection();

    for (INT32 i = 0; i < classNames->GetCount(); i++)
    {
        STRING clsName = classNames->GetItem(i);
        Ptr<MgPropertyDefinitionCollection> propDefs = cache->GetClassIdentityProperties(resource, schemaName, clsName);
        if (NULL == propDefs)
        {
            uncachedClasses->Add(clsName);
        }
        else
        {
            Ptr<MgClassDefinition> clsCopy = new MgClassDefinition();
            clsCopy->SetName(clsName);
            Ptr<MgPropertyDefinitionCollection> idProps = clsCopy->GetIdentityProperties();
            for (INT32 k = 0; k < propDefs->GetCount(); k++)
            {
                Ptr<MgPropertyDefinition> idProp = propDefs->GetItem(k);
                idProps->Add(idProp);
            }
            classDefs->Add(clsCopy);
        }
    }

    if (uncachedClasses->GetCount() > 0)
    {
        Ptr<MgFeatureSchemaCollection> schemas = DescribeSchema(resource, schemaName, uncachedClasses);
        for (INT32 i = 0; i < schemas->GetCount(); i++)
        {
            Ptr<MgFeatureSchema> schema = schemas->GetItem(i);
            Ptr<MgClassDefinitionCollection> classes = schema->GetClasses();

            for (INT32 j = 0; j < classes->GetCount(); j++)
            {
                Ptr<MgClassDefinition> cls = classes->GetItem(j);

                //Cache it
                cache->SetClassDefinition(resource, schema->GetName(), cls->GetName(), cls);

                Ptr<MgClassDefinition> clsCopy = new MgClassDefinition();
                clsCopy->SetName(cls->GetName());
                Ptr<MgPropertyDefinitionCollection> idProps = clsCopy->GetIdentityProperties();
                Ptr<MgPropertyDefinitionCollection> propDefs = cls->GetIdentityProperties();

                //Cache it
                cache->SetClassIdentityProperties(resource, schema->GetName(), cls->GetName(), propDefs);

                for (INT32 k = 0; k < propDefs->GetCount(); k++)
                {
                    Ptr<MgPropertyDefinition> idProp = propDefs->GetItem(k);
                    idProps->Add(idProp);
                }

                classDefs->Add(clsCopy);
            }
        }
    }
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureService::GetIdentityProperties")

    return classDefs.Detach();
}

MgByteReader* MgdFeatureService::QueryCacheInfo()
{
    Ptr<MgByteReader> ret;

    MG_FEATURE_SERVICE_TRY()

    STRING content = L"<FdoConnectionPoolInfo>";

    std::vector<PoolCacheEntry*> entries;
    MgFdoConnectionPool::GetCacheInfo(entries);

    for (std::vector<PoolCacheEntry*>::iterator it = entries.begin(); it != entries.end(); it++)
    {
        content += L"<PoolCacheEntry>";
        {
            content += L"<ResourceId>";
            content += (*it)->ResourceId;
            content += L"</ResourceId>";

            content += L"<Provider>";
            content += (*it)->ProviderName;
            content += L"</Provider>";

            content += L"<OpenConnections>";
            STRING sOpen;
            MgUtil::Int32ToString((*it)->OpenCount, sOpen);
            content += sOpen;
            content += L"</OpenConnections>";

            content += L"<ClosedConnections>";
            STRING sClosed;
            MgUtil::Int32ToString((*it)->ClosedCount, sClosed);
            content += sClosed;
            content += L"</ClosedConnections>";
        }
        content += L"</PoolCacheEntry>";
    }

    content += L"</FdoConnectionPoolInfo>";

    for (std::vector<PoolCacheEntry*>::iterator it = entries.begin(); it != entries.end(); it++)
    {
        delete (*it);
    }

    entries.clear();

    string utf8Text = MgUtil::WideCharToMultiByte(content);
    Ptr<MgByteSource> byteSource = new MgByteSource((BYTE_ARRAY_IN) utf8Text.c_str(), (INT32)utf8Text.length());
    byteSource->SetMimeType(MgMimeType::Xml);
	ret = byteSource->GetReader();

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdFeatureService::QueryCacheInfo")

    return ret.Detach();
}

//NOTE: This is needed by MgRaster::GetStream(), but this is implemented in MgRasterHelper which we redirect the call to
//So it is not implemented here
MgByteReader* MgdFeatureService::GetRaster(CREFSTRING reader, INT32 xSize, INT32 ySize, STRING propName) 
{ 
    NOT_IMPLEMENTED(L"MgdFeatureService::GetRaster"); 
}

// Feature
MgBatchPropertyCollection* MgdFeatureService::GetFeatures(CREFSTRING featureReader) { NOT_IMPLEMENTED(L"MgdFeatureService::GetFeatures"); }
bool MgdFeatureService::CloseFeatureReader(CREFSTRING featureReader) { NOT_IMPLEMENTED(L"MgdFeatureService::CloseFeatureReader"); }

// SQL
MgBatchPropertyCollection* MgdFeatureService::GetSqlRows(CREFSTRING sqlReader) { NOT_IMPLEMENTED(L"MgdFeatureService::GetSqlRows"); }
bool MgdFeatureService::CloseSqlReader(CREFSTRING sqlReader) { NOT_IMPLEMENTED(L"MgdFeatureService::CloseSqlReader"); }

// Data
MgBatchPropertyCollection* MgdFeatureService::GetDataRows(CREFSTRING dataReader) { NOT_IMPLEMENTED(L"MgdFeatureService::GetDataRows"); }
bool MgdFeatureService::CloseDataReader(CREFSTRING dataReader) { NOT_IMPLEMENTED(L"MgdFeatureService::CloseDataReader"); }

STRING MgdFeatureService::GetFdoCacheInfo() { NOT_IMPLEMENTED(L"MgdFeatureService::GetFdoCacheInfo"); }

// Commit the transaction specified by the transaction id.
bool MgdFeatureService::CommitTransaction(CREFSTRING transactionId) { NOT_IMPLEMENTED(L"MgdFeatureService::CommitTransaction"); }

// Rollback the transaction specified by the transaction id.
bool MgdFeatureService::RollbackTransaction(CREFSTRING transactionId) { NOT_IMPLEMENTED(L"MgdFeatureService::RollbackTransaction"); }

//Add save point
STRING MgdFeatureService::AddSavePoint(CREFSTRING transactionId, CREFSTRING suggestName) { NOT_IMPLEMENTED(L"MgdFeatureService::AddSavePoint"); }

//Roll back to a save point
bool MgdFeatureService::RollbackSavePoint(CREFSTRING transactionId, CREFSTRING savePointName) { NOT_IMPLEMENTED(L"MgdFeatureService::RollbackSavePoint"); }

//Release a save point
bool MgdFeatureService::ReleaseSavePoint(CREFSTRING transactionId, CREFSTRING savePointName) { NOT_IMPLEMENTED(L"MgdFeatureService::ReleaseSavePoint"); }
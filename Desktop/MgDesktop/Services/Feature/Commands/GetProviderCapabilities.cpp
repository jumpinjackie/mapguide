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

#include "Services/Feature/FeatureDefs.h"
#include "GetProviderCapabilities.h"
#include "Services/Feature/FeatureConnection.h"
#include "Services/Feature/FeatureUtil.h"
#include "Services/Feature/FdoConnectionUtil.h"

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

bool MgdGetProviderCapabilities::m_isInitialized = MgdGetProviderCapabilities::Initialize();


MgdGetProviderCapabilities::MgdGetProviderCapabilities(CREFSTRING providerName, CREFSTRING connectionString)
{
    if (providerName.empty())
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(MgResources::BlankArgument);

        throw new MgInvalidArgumentException(L"MgdGetProviderCapabilities.MgdGetProviderCapabilities",
            __LINE__, __WFILE__, &arguments, L"MgStringEmpty", NULL);
    }

    FdoPtr<IConnectionManager> connManager = FdoFeatureAccessManager::GetConnectionManager();
    CHECKNULL(connManager, L"MgdGetProviderCapabilities.MgdGetProviderCapabilities");

    // Remove the version from the provider name
    FdoPtr<FdoProviderNameTokens> tokens = FdoProviderNameTokens::Create(providerName.c_str());
    FdoStringsP strTokens = tokens->GetNameTokens();

    FdoPtr<FdoStringElement> companyName = strTokens->GetItem(0);
    FdoPtr<FdoStringElement> prvName = strTokens->GetItem(1);

    STRING providerNoVersion;
    providerNoVersion = companyName->GetString();
    providerNoVersion += L".";
    providerNoVersion += prvName->GetString();

    FdoPtr<FdoIConnection> fdoConn = connManager->CreateConnection(providerNoVersion.c_str());

    // Note: WFS doesn't return the proper capabilities if a connection is not opened to the actual server using a connection string.

    // Check if a connection string was specified
    if(!connectionString.empty())
    {
        fdoConn->SetConnectionString(connectionString.c_str());
        fdoConn->Open();
    }

    CHECKNULL(fdoConn, L"MgdGetProviderCapabilities.MgdGetProviderCapabilities");

    m_xmlUtil = new MgXmlUtil();
    CHECKNULL(m_xmlUtil, L"MgdGetProviderCapabilities.MgdGetProviderCapabilities");

    m_xmlCap = NULL;

    // no more risk of exceptions, so we can now assign these
    m_fdoConn = fdoConn.Detach();
    m_providerName = providerNoVersion;
}

MgdGetProviderCapabilities::~MgdGetProviderCapabilities()
{
    // Check if the connection needs to be closed
    if(m_fdoConn->GetConnectionState() == FdoConnectionState_Open)
    {
        MgdFdoConnectionUtil::CloseConnection(m_fdoConn);
    }

    m_fdoConn = NULL;

    delete m_xmlUtil;
    delete m_xmlCap;
}


MgByteReader* MgdGetProviderCapabilities::GetProviderCapabilities()
{
    CHECKNULL(m_xmlUtil, L"MgdGetProviderCapabilities.GetProviderCapabilities");
    Ptr<MgByteReader> byteReader;

    MG_FEATURE_SERVICE_TRY()

    // Generate the capability document
    CreateCapabilitiesDocument();

    if (m_xmlCap != NULL)
    {
        byteReader = m_xmlCap->ToReader();
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdGetProviderCapabilities.GetProviderCapabilities")

    return byteReader.Detach();
}

void MgdGetProviderCapabilities::CreateCapabilitiesDocument()
{
    // Root node element created
    // this XML follows the FdoProviderCapabilities-1.0.0.xsd schema
    m_xmlCap = new MgXmlUtil("FeatureProviderCapabilities");
    CHECKNULL(m_xmlCap, L"MgdGetProviderCapabilities::CreateCapabilitiesDocument");
    DOMElement* root = m_xmlCap->GetRootNode();

    // Provide name element and its attribute
    DOMElement* providerNode = m_xmlCap->AddChildNode(root, "Provider");
    m_xmlCap->SetAttribute(providerNode, "Name", m_providerName.c_str());

    // Connection capabilities
    CreateConnectionCapabilities();

    CreateSchemaCapabilities();
    CreateCommandCapabilities();
    CreateFilterCapabilities();
    //if (m_version == VERSION_SUPPORTED(1,0))
    //    CreateExpressionCapabilities();
    //else
        CreateExpressionCapabilities2();
    CreateRasterCapabilities();
    CreateTopologyCapabilities();
    CreateGeometryCapabilities();
}



void MgdGetProviderCapabilities::CreateConnectionCapabilities()
{
    CHECKNULL(m_xmlCap, L"MgdGetProviderCapabilities::CreateConnectionCapabilities");
    CHECKNULL(m_fdoConn, L"MgdGetProviderCapabilities::CreateConnectionCapabilities");

    DOMElement* root = m_xmlCap->GetRootNode();
    CHECKNULL(root, L"MgdGetProviderCapabilities::CreateConnectionCapabilities");

    DOMElement* connNode = m_xmlCap->AddChildNode(root, "Connection");
    CHECKNULL(connNode, L"MgdGetProviderCapabilities::CreateConnectionCapabilities");

    FdoPtr<FdoIConnectionCapabilities> ficc = m_fdoConn->GetConnectionCapabilities();
    CHECKNULL((FdoIConnectionCapabilities*)ficc, L"MgdGetProviderCapabilities::CreateConnectionCapabilities");

    // Thread
    FdoThreadCapability ftc = ficc->GetThreadCapability();
    string str = s_FdoThreadCapability[ftc];
    m_xmlCap->AddTextNode(connNode, "ThreadCapability", str.c_str());

    // Spatial Context
    FdoInt32 cnt;
    FdoSpatialContextExtentType* fscet = ficc->GetSpatialContextTypes(cnt);
    if (cnt > 0 && fscet != NULL)
    {
        DOMElement* scNode = m_xmlCap->AddChildNode(connNode, "SpatialContextExtent");
        CHECKNULL(scNode, L"MgdGetProviderCapabilities::CreateConnectionCapabilities");

        for (FdoInt32 i = 0; i < cnt; i++)
        {
            string scStr = s_FdoSpatialContextExtentType[fscet[i]];
            m_xmlCap->AddTextNode(scNode, "Type", scStr.c_str());
        }
    }

    // Locking
    bool supportsLocking = ficc->SupportsLocking();
    m_xmlCap->AddTextNode(connNode, "SupportsLocking", supportsLocking);

    bool supportsTimeout = ficc->SupportsTimeout();
    m_xmlCap->AddTextNode(connNode, "SupportsTimeout", supportsTimeout);

    bool supportsTransactions = ficc->SupportsTransactions();
    m_xmlCap->AddTextNode(connNode, "SupportsTransactions", supportsTransactions);

    bool supportsSavePoint = ficc->SupportsSavePoint();
    m_xmlCap->AddTextNode(connNode, "SupportsSavePoint", supportsSavePoint);

    bool supportsLongTransactions = ficc->SupportsLongTransactions();
    m_xmlCap->AddTextNode(connNode, "SupportsLongTransactions", supportsLongTransactions);

    bool supportsSQL = ficc->SupportsSQL();
    m_xmlCap->AddTextNode(connNode, "SupportsSQL", supportsSQL);

    bool supportsConfiguration = ficc->SupportsConfiguration();
    m_xmlCap->AddTextNode(connNode, "SupportsConfiguration", supportsConfiguration);
}


void MgdGetProviderCapabilities::CreateSchemaCapabilities()
{
    CHECKNULL(m_xmlCap, L"MgdGetProviderCapabilities::CreateSchemaCapabilities");
    CHECKNULL(m_fdoConn, L"MgdGetProviderCapabilities::CreateSchemaCapabilities");

    FdoPtr<FdoISchemaCapabilities> fsc = m_fdoConn->GetSchemaCapabilities();
    CHECKNULL((FdoISchemaCapabilities*)fsc, L"MgdGetProviderCapabilities::CreateSchemaCapabilities");

    DOMElement* root = m_xmlCap->GetRootNode();
    CHECKNULL(root, L"MgdGetProviderCapabilities::CreateSchemaCapabilities");

    DOMElement* schemaNode = m_xmlCap->AddChildNode(root, "Schema");
    CHECKNULL(schemaNode, L"MgdGetProviderCapabilities::CreateSchemaCapabilities");

    // Add all class types
    FdoInt32 cnt = 0;
    FdoClassType* fct = fsc->GetClassTypes(cnt);
    if (cnt > 0 && fct != NULL)
    {
        DOMElement* classNode = m_xmlCap->AddChildNode(schemaNode, "Class");
        CHECKNULL(classNode, L"MgdGetProviderCapabilities::CreateSchemaCapabilities");

        for (FdoInt32 i=0; i < cnt; i++)
        {
            string scStr = s_FdoClassType[fct[i]];
            m_xmlCap->AddTextNode(classNode, "Type", scStr.c_str());
        }
    }

    // Add all data types
    cnt = 0;
    FdoDataType* fdt = fsc->GetDataTypes(cnt);
    if (cnt > 0 && fdt != NULL)
    {
        DOMElement* dataNode = m_xmlCap->AddChildNode(schemaNode, "Data");
        CHECKNULL(dataNode, L"MgdGetProviderCapabilities::CreateSchemaCapabilities");

        for (FdoInt32 i=0; i < cnt; i++)
        {
            string dtStr = s_FdoDataType[fdt[i]];
            m_xmlCap->AddTextNode(dataNode, "Type", dtStr.c_str());
        }
    }

    // Supports Inheritance
    bool supportsInheritance        = fsc->SupportsInheritance();
    m_xmlCap->AddTextNode(schemaNode, "SupportsInheritance", supportsInheritance);

    // Supports Multiple Schemas
    bool supportsMultipleSchemas    = fsc->SupportsMultipleSchemas();
    m_xmlCap->AddTextNode(schemaNode, "SupportsMultipleSchemas", supportsMultipleSchemas);

    bool supportsObjectProperties   = fsc->SupportsObjectProperties();
    m_xmlCap->AddTextNode(schemaNode, "SupportsObjectProperties", supportsObjectProperties);

    bool supportsAssociationProperties = fsc->SupportsAssociationProperties();
    m_xmlCap->AddTextNode(schemaNode, "SupportsAssociationProperties", supportsAssociationProperties);

    bool supportsSchemaOverrides =  fsc->SupportsSchemaOverrides();
    m_xmlCap->AddTextNode(schemaNode, "SupportsSchemaOverrides", supportsSchemaOverrides);

    bool supportsNetworkModel  = fsc->SupportsNetworkModel();
    m_xmlCap->AddTextNode(schemaNode, "SupportsNetworkModel", supportsNetworkModel);

    bool supportsAutoIdGeneration  = fsc->SupportsAutoIdGeneration();
    m_xmlCap->AddTextNode(schemaNode, "SupportsAutoIdGeneration", supportsAutoIdGeneration);

    bool supportsDataStoreScopeUniqueIdGeneration  = fsc->SupportsDataStoreScopeUniqueIdGeneration();
    m_xmlCap->AddTextNode(schemaNode, "SupportsDataStoreScopeUniqueIdGeneration", supportsDataStoreScopeUniqueIdGeneration);

    FdoDataType* sagt = fsc->GetSupportedAutoGeneratedTypes(cnt);
    if (cnt > 0 && sagt != NULL)
    {
        DOMElement* sagtNode = m_xmlCap->AddChildNode(schemaNode, "SupportedAutoGeneratedTypes");
        CHECKNULL(sagtNode, L"MgdGetProviderCapabilities::CreateSchemaCapabilities");

        for (FdoInt32 i=0; i < cnt; i++)
        {
            string sagtStr = s_FdoDataType[sagt[i]];
            m_xmlCap->AddTextNode(sagtNode, "Type", sagtStr.c_str());
        }
    }

    bool supportsSchemaModification  = fsc->SupportsSchemaModification();
    m_xmlCap->AddTextNode(schemaNode, "SupportsSchemaModification", supportsSchemaModification);
}

void MgdGetProviderCapabilities::CreateCommandCapabilities()
{
    CHECKNULL(m_xmlCap, L"MgdGetProviderCapabilities::CreateCommandCapabilities");
    CHECKNULL(m_fdoConn, L"MgdGetProviderCapabilities::CreateCommandCapabilities");

    FdoPtr<FdoICommandCapabilities> fcc = m_fdoConn->GetCommandCapabilities();
    CHECKNULL((FdoICommandCapabilities*)fcc, L"MgdGetProviderCapabilities::CreateCommandCapabilities");

    DOMElement* root = m_xmlCap->GetRootNode();
    CHECKNULL(root, L"MgdGetProviderCapabilities::CreateCommandCapabilities");

    DOMElement* cmdNode = m_xmlCap->AddChildNode(root, "Command");
    CHECKNULL(cmdNode, L"MgdGetProviderCapabilities::CreateCommandCapabilities");

    // Add all command types
    FdoInt32 cnt = 0;
    FdoInt32* fcmd = fcc->GetCommands(cnt);
    if (cnt > 0 && fcmd != NULL)
    {
        DOMElement* scNode = m_xmlCap->AddChildNode(cmdNode, "SupportedCommands");
        CHECKNULL(scNode, L"MgdGetProviderCapabilities::CreateCommandCapabilities");

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
                        //if (m_version == VERSION_SUPPORTED(1,0))
                        break;
                        // these enumerates were added for version 2
                        // fall thru to write this command
                    default:
                        m_xmlCap->AddTextNode(scNode, "Name", cmdStr.c_str());
                        break;
                }
            }
        }
    }

    // Supports Parameters
    bool supportsParameters = fcc->SupportsParameters();
    m_xmlCap->AddTextNode(cmdNode, "SupportsParameters", supportsParameters);

    // Supports Timeout
    bool supportsTimeout = fcc->SupportsTimeout();
    m_xmlCap->AddTextNode(cmdNode, "SupportsTimeout", supportsTimeout);

    // Supports SupportsSelectExpressions
    bool supportsSelectExpressions = fcc->SupportsSelectExpressions();
    m_xmlCap->AddTextNode(cmdNode, "SupportsSelectExpressions", supportsSelectExpressions);

    // Supports SupportsSelectFunctions
    bool supportsSelectFunctions = fcc->SupportsSelectFunctions();
    m_xmlCap->AddTextNode(cmdNode, "SupportsSelectFunctions", supportsSelectFunctions);

    // Supports SupportsSelectDistinct
    bool supportsSelectDistinct = fcc->SupportsSelectDistinct();
    m_xmlCap->AddTextNode(cmdNode, "SupportsSelectDistinct", supportsSelectDistinct);

    // Supports SupportsSelectOrdering
    bool supportsSelectOrdering = fcc->SupportsSelectOrdering();
    m_xmlCap->AddTextNode(cmdNode, "SupportsSelectOrdering", supportsSelectOrdering);

    // Supports SupportsSelectGrouping
    bool supportsSelectGrouping = fcc->SupportsSelectGrouping();
    m_xmlCap->AddTextNode(cmdNode, "SupportsSelectGrouping", supportsSelectGrouping);
}

void MgdGetProviderCapabilities::CreateFilterCapabilities()
{
    CHECKNULL(m_xmlCap, L"MgdGetProviderCapabilities::CreateFilterCapabilities");
    CHECKNULL(m_fdoConn, L"MgdGetProviderCapabilities::CreateFilterCapabilities");

    FdoPtr<FdoIFilterCapabilities> ffc = m_fdoConn->GetFilterCapabilities();
    CHECKNULL((FdoIFilterCapabilities*)ffc, L"MgdGetProviderCapabilities::CreateFilterCapabilities");

    DOMElement* root = m_xmlCap->GetRootNode();
    CHECKNULL(root, L"MgdGetProviderCapabilities::CreateFilterCapabilities");

    DOMElement* filterNode = m_xmlCap->AddChildNode(root, "Filter");
    CHECKNULL(filterNode, L"MgdGetProviderCapabilities::CreateFilterCapabilities");

    // Add all condition types
    FdoInt32 cnt = 0;
    FdoConditionType* fct = ffc->GetConditionTypes(cnt);
    if (cnt > 0 && fct != NULL)
    {
        DOMElement* condNode = m_xmlCap->AddChildNode(filterNode, "Condition");
        CHECKNULL(condNode, L"MgdGetProviderCapabilities::CreateFilterCapabilities");

        for (FdoInt32 i=0; i < cnt; i++)
        {
            string condStr = s_FdoConditionType[fct[i]];
            m_xmlCap->AddTextNode(condNode, "Type", condStr.c_str());
        }
    }

    // All spatial operations
    cnt = 0;
    FdoSpatialOperations* fso = ffc->GetSpatialOperations(cnt);
    if (cnt > 0 && fso != NULL)
    {
        DOMElement* fsoNode = m_xmlCap->AddChildNode(filterNode, "Spatial");
        CHECKNULL(fsoNode, L"MgdGetProviderCapabilities::CreateFilterCapabilities");

        for (FdoInt32 i=0; i < cnt; i++)
        {
            string operStr = s_FdoSpatialOperations[fso[i]];
            m_xmlCap->AddTextNode(fsoNode, "Operation", operStr.c_str());
        }
    }

    // All distance operations
    cnt = 0;
    FdoDistanceOperations* fdo = ffc->GetDistanceOperations(cnt);
    if (cnt > 0 && fdo != NULL)
    {
        DOMElement* distNode = m_xmlCap->AddChildNode(filterNode, "Distance");
        CHECKNULL(distNode, L"MgdGetProviderCapabilities::CreateFilterCapabilities");

        for (FdoInt32 i=0; i < cnt; i++)
        {
            string fdoStr = s_FdoDistanceOperations[fdo[i]];
            m_xmlCap->AddTextNode(distNode, "Operation", fdoStr.c_str());
        }
    }

    // supports Geodesic Distance
    bool supportsGeodesicDistance = ffc->SupportsGeodesicDistance();
    m_xmlCap->AddTextNode(filterNode, "SupportsGeodesicDistance", supportsGeodesicDistance);

    // supports NonLiteral Geometric Operations
    bool supportsNonLiteralGeometricOperations = ffc->SupportsNonLiteralGeometricOperations();
    m_xmlCap->AddTextNode(filterNode, "SupportsNonLiteralGeometricOperations", supportsNonLiteralGeometricOperations);
}

void MgdGetProviderCapabilities::CreateExpressionCapabilities()
{
    CHECKNULL(m_xmlCap, L"MgdGetProviderCapabilities::CreateExpressionCapabilities");
    CHECKNULL(m_fdoConn, L"MgdGetProviderCapabilities::CreateExpressionCapabilities");

    FdoPtr<FdoIExpressionCapabilities> fec = m_fdoConn->GetExpressionCapabilities();
    CHECKNULL((FdoIExpressionCapabilities*)fec, L"MgdGetProviderCapabilities::CreateExpressionCapabilities");

    DOMElement* root = m_xmlCap->GetRootNode();
    CHECKNULL(root, L"MgdGetProviderCapabilities::CreateExpressionCapabilities");

    DOMElement* expressionNode = m_xmlCap->AddChildNode(root, "Expression");
    CHECKNULL(expressionNode, L"MgdGetProviderCapabilities::CreateExpressionCapabilities");

    // Add all expression types
    FdoInt32 cnt = 0;
    FdoExpressionType* fet = fec->GetExpressionTypes(cnt);
    if (cnt > 0 && fet != NULL)
    {
        DOMElement* typeNode = m_xmlCap->AddChildNode(expressionNode, "Type");
        CHECKNULL(typeNode, L"MgdGetProviderCapabilities::CreateExpressionCapabilities");

        for (FdoInt32 i=0; i < cnt; i++)
        {
            string typeStr = s_FdoExpressionType[fet[i]];
            m_xmlCap->AddTextNode(typeNode, "Name", typeStr.c_str());
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
            DOMElement* funcDefColNode = m_xmlCap->AddChildNode(expressionNode, "FunctionDefinitionList");
            CHECKNULL(funcDefColNode, L"MgdGetProviderCapabilities::CreateExpressionCapabilities");

            for (FdoInt32 i=0; i < funcCnt; i++)
            {
                // Add function definition element
                FdoPtr<FdoFunctionDefinition> ffd = ffdc->GetItem(i);
                CHECKNULL((FdoFunctionDefinition*)ffd, L"MgdGetProviderCapabilities::CreateExpressionCapabilities");

                DOMElement* funcDefNode = m_xmlCap->AddChildNode(funcDefColNode, "FunctionDefinition");
                CHECKNULL(funcDefNode, L"MgdGetProviderCapabilities::CreateExpressionCapabilities");

                const char* strName = MgUtil::WideCharToMultiByte(ffd->GetName());
                const char* strDesc = MgUtil::WideCharToMultiByte(ffd->GetDescription());

                FdoDataType eDataType = ffd->GetReturnType();
                string strDataType = s_FdoDataType[eDataType];

                m_xmlCap->AddTextNode(funcDefNode, "Name", strName);
                m_xmlCap->AddTextNode(funcDefNode, "Description", strDesc);
                m_xmlCap->AddTextNode(funcDefNode, "ReturnType",  strDataType.c_str());

                delete[] strName;
                delete[] strDesc;

                // Add argument of each functions if there are any
                FdoPtr<FdoReadOnlyArgumentDefinitionCollection> argCol = ffd->GetArguments();
                if (NULL != (FdoReadOnlyArgumentDefinitionCollection*)argCol)
                {
                    FdoInt32 argCnt = argCol->GetCount();
                    if (argCnt > 0)
                    {
                        // Add ArgumentDefinitionCollection if there are arguments
                        DOMElement* argDefColNode = m_xmlCap->AddChildNode(funcDefNode, "ArgumentDefinitionList");
                        CHECKNULL(argDefColNode, L"MgdGetProviderCapabilities::CreateExpressionCapabilities");

                        for (FdoInt32 j=0; j < argCnt; j++)
                        {
                            // Add ArgumentDefinition for each argument
                            FdoPtr<FdoArgumentDefinition> fad = argCol->GetItem(j);
                            CHECKNULL((FdoArgumentDefinition*)fad, L"MgdGetProviderCapabilities::CreateExpressionCapabilities");

                            DOMElement* argDefNode = m_xmlCap->AddChildNode(argDefColNode, "ArgumentDefinition");
                            CHECKNULL(argDefNode, L"MgdGetProviderCapabilities::CreateExpressionCapabilities");

                            const char* strArgName = MgUtil::WideCharToMultiByte(fad->GetName());
                            const char* strArgDesc = MgUtil::WideCharToMultiByte(fad->GetDescription());

                            FdoDataType eArgDataType = fad->GetDataType();
                            string strArgDataType = s_FdoDataType[eArgDataType];

                            m_xmlCap->AddTextNode(argDefNode, "Name", strArgName);
                            m_xmlCap->AddTextNode(argDefNode, "Description", strArgDesc);
                            m_xmlCap->AddTextNode(argDefNode, "ReturnType",  strArgDataType.c_str());

                            delete[] strArgName;
                            delete[] strArgDesc;
                        }
                    }
                }
            }
        }
    }
}


void MgdGetProviderCapabilities::CreateExpressionCapabilities2()
{
    CHECKNULL(m_xmlCap, L"MgdGetProviderCapabilities::CreateExpressionCapabilities");
    CHECKNULL(m_fdoConn, L"MgdGetProviderCapabilities::CreateExpressionCapabilities");

    FdoPtr<FdoIExpressionCapabilities> fec = m_fdoConn->GetExpressionCapabilities();
    CHECKNULL((FdoIExpressionCapabilities*)fec, L"MgdGetProviderCapabilities::CreateExpressionCapabilities");

    DOMElement* root = m_xmlCap->GetRootNode();
    CHECKNULL(root, L"MgdGetProviderCapabilities::CreateExpressionCapabilities");

    DOMElement* expressionNode = m_xmlCap->AddChildNode(root, "Expression");
    CHECKNULL(expressionNode, L"MgdGetProviderCapabilities::CreateExpressionCapabilities");

    // Add all expression types
    FdoInt32 cnt = 0;
    FdoExpressionType* fet = fec->GetExpressionTypes(cnt);
    if (cnt > 0 && fet != NULL)
    {
        DOMElement* typeNode = m_xmlCap->AddChildNode(expressionNode, "Type");
        CHECKNULL(typeNode, L"MgdGetProviderCapabilities::CreateExpressionCapabilities");

        for (FdoInt32 i=0; i < cnt; i++)
        {
            string typeStr = s_FdoExpressionType[fet[i]];
            m_xmlCap->AddTextNode(typeNode, "Name", typeStr.c_str());
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
            DOMElement* funcDefColNode = m_xmlCap->AddChildNode(expressionNode, "FunctionDefinitionList");
            CHECKNULL(funcDefColNode, L"MgdGetProviderCapabilities::CreateExpressionCapabilities");

            for (FdoInt32 i=0; i < funcCnt; i++)
            {
                // Add function definition element
                FdoPtr<FdoFunctionDefinition> ffd = ffdc->GetItem(i);
                CHECKNULL((FdoFunctionDefinition*)ffd, L"MgdGetProviderCapabilities::CreateExpressionCapabilities");

                DOMElement* funcDefNode = m_xmlCap->AddChildNode(funcDefColNode, "FunctionDefinition");
                CHECKNULL(funcDefNode, L"MgdGetProviderCapabilities::CreateExpressionCapabilities");

                const char* strName = MgUtil::WideCharToMultiByte(ffd->GetName());
                const char* strDesc = MgUtil::WideCharToMultiByte(ffd->GetDescription());

                FdoFunctionCategoryType eFdoFunctionCategoryType = ffd->GetFunctionCategoryType();
                string strFunctionCategoryType = s_FdoFunctionCategoryType[eFdoFunctionCategoryType];

                m_xmlCap->AddTextNode(funcDefNode, "Name", strName);
                m_xmlCap->AddTextNode(funcDefNode, "Description", strDesc);
                m_xmlCap->AddTextNode(funcDefNode, "CategoryType",  strFunctionCategoryType.c_str());
                m_xmlCap->AddTextNode(funcDefNode, "IsAggregate",  ffd->IsAggregate());
                m_xmlCap->AddTextNode(funcDefNode, "IsSupportsVariableArgumentsList",  ffd->SupportsVariableArgumentsList());

                delete[] strName;
                delete[] strDesc;

                // Add argument of each functions if there are any
                FdoPtr<FdoReadOnlySignatureDefinitionCollection> signatures = ffd->GetSignatures();
                if (NULL != (FdoReadOnlySignatureDefinitionCollection*)signatures)
                {
                    FdoInt32 signaturesCnt = signatures->GetCount();
                    if (signaturesCnt > 0)
                    {

                        DOMElement* signDefColNode = m_xmlCap->AddChildNode(funcDefNode, "SignatureDefinitionCollection");
                        CHECKNULL(signDefColNode, L"MgdGetProviderCapabilities::CreateExpressionCapabilities");
                        for (FdoInt32 j=0; j < signaturesCnt; j++)
                        {

                            // Add SignatureDefinition for each signature
                            FdoPtr<FdoSignatureDefinition> fsd = signatures->GetItem(j);
                            CHECKNULL((FdoSignatureDefinition*)fsd, L"MgdGetProviderCapabilities::CreateExpressionCapabilities");

                            DOMElement* signDefNode = m_xmlCap->AddChildNode(signDefColNode, "SignatureDefinition");
                            CHECKNULL(signDefNode, L"MgdGetProviderCapabilities::CreateExpressionCapabilities");

                            FdoPropertyType eSignPropertyDataType = fsd->GetReturnPropertyType();
                            string strSignPropertyType = s_FdoPropertyTypeAsString[eSignPropertyDataType];
                            string strSignDataType;
                            if (eSignPropertyDataType == FdoPropertyType_DataProperty)
                            {
                                FdoDataType eSignDataType = fsd->GetReturnType();
                                strSignDataType = s_FdoDataType[eSignDataType];
                            }

                            m_xmlCap->AddTextNode(signDefNode, "PropertyType", strSignPropertyType.c_str());
                            if (eSignPropertyDataType == FdoPropertyType_DataProperty)
                                m_xmlCap->AddTextNode(signDefNode, "DataType",  strSignDataType.c_str());

                            DOMElement* argDefColNode = m_xmlCap->AddChildNode(signDefNode, "ArgumentDefinitionList");
                            CHECKNULL(argDefColNode, L"MgdGetProviderCapabilities::CreateExpressionCapabilities");

                            FdoPtr<FdoReadOnlyArgumentDefinitionCollection> fads = fsd->GetArguments();
                            if (NULL != (FdoReadOnlyArgumentDefinitionCollection *) fads)
                            {
                                FdoInt32 argCnt = fads->GetCount();
                                if (argCnt > 0)
                                {
                                    for (int k=0; k<argCnt; k++)
                                    {
                                        FdoPtr<FdoArgumentDefinition> fad = fads->GetItem(k);
                                        CHECKNULL((FdoArgumentDefinition*)fad, L"MgdGetProviderCapabilities::CreateExpressionCapabilities");

                                        DOMElement* argDefNode = m_xmlCap->AddChildNode(argDefColNode, "ArgumentDefinition");
                                        CHECKNULL(argDefNode, L"MgdGetProviderCapabilities::CreateExpressionCapabilities");

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

                                        m_xmlCap->AddTextNode(argDefNode, "Name", strArgName);
                                        m_xmlCap->AddTextNode(argDefNode, "Description", strArgDesc);
                                        m_xmlCap->AddTextNode(argDefNode, "PropertyType", strArgPropertyType.c_str());
                                        if (eArgPropertyDataType == FdoPropertyType_DataProperty)
                                            m_xmlCap->AddTextNode(argDefNode, "DataType",  strArgDataType.c_str());

                                        delete[] strArgName;
                                        delete[] strArgDesc;

                                        FdoPtr<FdoPropertyValueConstraintList> fpvc = fad->GetArgumentValueList();
                                        if (NULL != (FdoPropertyValueConstraintList *) fpvc)
                                        {
                                            if (fpvc->GetConstraintType() == FdoPropertyValueConstraintType_List)
                                            {
                                                DOMElement* propValueConstListNode = m_xmlCap->AddChildNode(argDefNode, "PropertyValueConstraintList");
                                                CHECKNULL(propValueConstListNode, L"MgdGetProviderCapabilities::CreateExpressionCapabilities");

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
                                                            CHECKNULL((FdoDataValue*)dv, L"MgdGetProviderCapabilities::CreateExpressionCapabilities");
                                                            FdoDataType dataType = dv->GetDataType();
                                                            // FdoDataType_String is the only supported type
                                                            if (dataType == FdoDataType_String)
                                                            {
                                                                FdoStringValue *stringValue = (FdoStringValue *) dv.p;
                                                                FdoString* xmlValue = stringValue->GetString();
                                                                const char* strDataValue = MgUtil::WideCharToMultiByte(stringValue->GetString());
                                                                m_xmlCap->AddTextNode(propValueConstListNode, "Value", xmlValue);
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

void MgdGetProviderCapabilities::CreateRasterCapabilities()
{
    CHECKNULL(m_xmlCap, L"MgdGetProviderCapabilities::CreateRasterCapabilities");
    CHECKNULL(m_fdoConn, L"MgdGetProviderCapabilities::CreateRasterCapabilities");

    FdoPtr<FdoIRasterCapabilities> frc = m_fdoConn->GetRasterCapabilities();
    CHECKNULL((FdoIRasterCapabilities*)frc, L"MgdGetProviderCapabilities::CreateRasterCapabilities");

    DOMElement* root = m_xmlCap->GetRootNode();
    CHECKNULL(root, L"MgdGetProviderCapabilities::CreateRasterCapabilities");

    DOMElement* rasterNode = m_xmlCap->AddChildNode(root, "Raster");
    CHECKNULL(rasterNode, L"MgdGetProviderCapabilities::CreateRasterCapabilities");

    // Supports Raster
    bool supportsRaster = frc->SupportsRaster();
    m_xmlCap->AddTextNode(rasterNode, "SupportsRaster", supportsRaster);

    // Supports Stitching
    bool supportsStitching = frc->SupportsStitching();
    m_xmlCap->AddTextNode(rasterNode, "SupportsStitching", supportsStitching);

    bool supportsSubsampling = frc->SupportsSubsampling();
    m_xmlCap->AddTextNode(rasterNode, "SupportsSubsampling", supportsSubsampling);

}

void MgdGetProviderCapabilities::CreateTopologyCapabilities()
{
    CHECKNULL(m_xmlCap, L"MgdGetProviderCapabilities::CreateTopologyCapabilities");
    CHECKNULL(m_fdoConn, L"MgdGetProviderCapabilities::CreateTopologyCapabilities");

    MG_FEATURE_SERVICE_TRY()

    FdoPtr<FdoITopologyCapabilities> frc = m_fdoConn->GetTopologyCapabilities();

    // Provider has no topology capabilities
    if (NULL == (FdoITopologyCapabilities*)frc)
    {
        return;
    }

    DOMElement* root = m_xmlCap->GetRootNode();
    CHECKNULL(root, L"MgdGetProviderCapabilities::CreateTopologyCapabilities");

    DOMElement* topologyNode = m_xmlCap->AddChildNode(root, "Topology");
    CHECKNULL(topologyNode, L"MgdGetProviderCapabilities::CreateTopologyCapabilities");

    // Supports Topology
    bool supportsTopology = frc->SupportsTopology();
    m_xmlCap->AddTextNode(topologyNode, "SupportsTopology", supportsTopology);

    // Supports Stitching
    bool supportsTopologicalHierarchy = frc->SupportsTopologicalHierarchy();
    m_xmlCap->AddTextNode(topologyNode, "SupportsTopologicalHierarchy", supportsTopologicalHierarchy);

    bool breaksCurveCrossingsAutomatically = frc->BreaksCurveCrossingsAutomatically();
    m_xmlCap->AddTextNode(topologyNode, "BreaksCurveCrossingsAutomatically", breaksCurveCrossingsAutomatically);

    bool activatesTopologyByArea = frc->ActivatesTopologyByArea();
    m_xmlCap->AddTextNode(topologyNode, "ActivatesTopologyByArea", activatesTopologyByArea);

    bool constrainsFeatureMovements = frc->ConstrainsFeatureMovements();
    m_xmlCap->AddTextNode(topologyNode, "ConstrainsFeatureMovements", constrainsFeatureMovements);

    // TODO: Change this to CATCH_AND_THROW when SimpleDB stops throwing exception of not implemented
    MG_FEATURE_SERVICE_CATCH(L"MgdGetProviderCapabilities.CreateTopologyCapabilities")

}

void MgdGetProviderCapabilities::CreateGeometryCapabilities()
{
    CHECKNULL(m_xmlCap, L"MgdGetProviderCapabilities.CreateGeometryCapabilities");
    CHECKNULL(m_fdoConn, L"MgdGetProviderCapabilities.CreateGeometryCapabilities");

    MG_FEATURE_SERVICE_TRY()

    FdoPtr<FdoIGeometryCapabilities> fgc = m_fdoConn->GetGeometryCapabilities();

    // Provider has no geometric capabilities
    if (NULL == (FdoIGeometryCapabilities*)fgc)
    {
        return;
    }

    DOMElement* root = m_xmlCap->GetRootNode();
    CHECKNULL(root, L"MgdGetProviderCapabilities.CreateGeometryCapabilities");

    DOMElement* geometryNode = m_xmlCap->AddChildNode(root, "Geometry");
    CHECKNULL(geometryNode, L"MgdGetProviderCapabilities.CreateGeometryCapabilities");

    FdoInt32 cnt = 0;
    FdoGeometryType* geomType = fgc->GetGeometryTypes(cnt);
    if (cnt > 0 && geomType != NULL)
    {
        DOMElement* geometryTypeNode = m_xmlCap->AddChildNode(geometryNode, "Types");
        CHECKNULL(geometryTypeNode, L"MgdGetProviderCapabilities.CreateGeometryCapabilities");

        for (FdoInt32 i=0; i < cnt; i++)
        {
            string geomTypeStr = s_FdoGeometryType[geomType[i]];
            m_xmlCap->AddTextNode(geometryTypeNode, "Type", geomTypeStr.c_str());
        }
    }

    FdoGeometryComponentType* geomCompType = fgc->GetGeometryComponentTypes(cnt);
    if (cnt > 0 && geomCompType != NULL)
    {
        DOMElement* geometryCompNode = m_xmlCap->AddChildNode(geometryNode, "Components");
        CHECKNULL(geometryCompNode, L"MgdGetProviderCapabilities.CreateGeometryCapabilities");

        for (FdoInt32 i=0; i < cnt; i++)
        {
            string geomCompStr = s_FdoGeometryComponentType[geomCompType[i]];
            m_xmlCap->AddTextNode(geometryCompNode, "Type", geomCompStr.c_str());
        }
    }

    FdoInt32 dim = fgc->GetDimensionalities();

    char buff[8]; buff[0] = 0;
    sprintf(buff, "%d", dim);

    m_xmlCap->AddTextNode(geometryNode, "Dimensionality", &buff[0]);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdGetProviderCapabilities.CreateGeometryCapabilities")
}

bool MgdGetProviderCapabilities::IsConnectionOpen()
{
    CHECKNULL(m_fdoConn, L"MgdGetProviderCapabilities.IsConnectionOpen");

    FdoConnectionState state = m_fdoConn->GetConnectionState();
    if (state != FdoConnectionState_Open)
        return false;

    return true;
}

bool MgdGetProviderCapabilities::Initialize()
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

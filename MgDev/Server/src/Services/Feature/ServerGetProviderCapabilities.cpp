//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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

#include "ServerFeatureServiceDefs.h"
#include "ServerGetProviderCapabilities.h"
#include "ServerFeatureConnection.h"
#include "ServerFeatureUtil.h"

static std::map<FdoThreadCapability, std::string>          s_FdoThreadCapability;
static std::map<FdoSpatialContextExtentType, std::string>  s_FdoSpatialContextExtentType;
static std::map<FdoClassType, std::string>                 s_FdoClassType;
static std::map<FdoDataType,  std::string>                 s_FdoDataType;
static std::map<FdoCommandType,  std::string>              s_FdoCommandType;
static std::map<FdoConditionType,  std::string>            s_FdoConditionType;
static std::map<FdoSpatialOperations,  std::string>        s_FdoSpatialOperations;
static std::map<FdoDistanceOperations,  std::string>       s_FdoDistanceOperations;
static std::map<FdoExpressionType,  std::string>           s_FdoExpressionType;
static std::map<FdoGeometryType,  std::string>             s_FdoGeometryType;
static std::map<FdoGeometryComponentType,  std::string>    s_FdoGeometryComponentType;

bool MgServerGetProviderCapabilities::m_isInitialized = MgServerGetProviderCapabilities::Initialize();


MgServerGetProviderCapabilities::MgServerGetProviderCapabilities(CREFSTRING providerName)
{
    if (providerName.empty())
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(MgResources::BlankArgument);

        throw new MgInvalidArgumentException(L"MgServerGetProviderCapabilities.MgServerGetProviderCapabilities",
            __LINE__, __WFILE__, &arguments, L"MgStringEmpty", NULL);
    }

    FdoPtr<IConnectionManager> connManager = FdoFeatureAccessManager::GetConnectionManager();
    CHECKNULL(connManager, L"MgServerGetProviderCapabilities.MgServerGetProviderCapabilities");

    // TODO: Should this connection be cached?
    // use a smart pointer until the end in case there's an exception
    FdoPtr<FdoIConnection> fdoConn = connManager->CreateConnection(providerName.c_str());
    CHECKNULL(fdoConn, L"MgServerGetProviderCapabilities.MgServerGetProviderCapabilities");

    m_xmlUtil = new MgXmlUtil();
    CHECKNULL(m_xmlUtil, L"MgServerGetProviderCapabilities.MgServerGetProviderCapabilities");

    m_xmlCap = NULL;

    // no more risk of exceptions, so we can now assign these
    m_fdoConn = fdoConn.Detach();
    m_providerName = providerName;
}

MgServerGetProviderCapabilities::~MgServerGetProviderCapabilities()
{
    FDO_SAFE_RELEASE(m_fdoConn);

    if (NULL != m_xmlUtil)
        delete m_xmlUtil;

    if (NULL != m_xmlCap)
        delete m_xmlCap;
}


MgByteReader* MgServerGetProviderCapabilities::GetProviderCapabilities()
{
    CHECKNULL(m_xmlUtil, L"MgServerGetProviderCapabilities.GetProviderCapabilities");
    Ptr<MgByteReader> byteReader;

    MG_FEATURE_SERVICE_TRY()

    // Generate the capability document
    CreateCapabilitiesDocument();

    if (m_xmlCap != NULL)
    {
        byteReader = m_xmlCap->ToReader();
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerGetProviderCapabilities.GetProviderCapabilities")

    return byteReader.Detach();
}

void MgServerGetProviderCapabilities::CreateCapabilitiesDocument()
{
    // Root node element created
    // this XML follows the FdoProviderCapabilities-1.0.0.xsd schema
    m_xmlCap = new MgXmlUtil("FeatureProviderCapabilities");
    CHECKNULL(m_xmlCap, L"MgServerGetProviderCapabilities::CreateCapabilitiesDocument");
    DOMElement* root = m_xmlCap->GetRootNode();

    // Provide name element and its attribute
    DOMElement* providerNode = m_xmlCap->AddChildNode(root, "Provider");
    m_xmlCap->SetAttribute(providerNode, "Name", m_providerName.c_str());

    // Connection capabilities
    CreateConnectionCapabilities();

    CreateSchemaCapabilities();
    CreateCommandCapabilities();
    CreateFilterCapabilities();
    CreateExpressionCapabilities();
    CreateRasterCapabilities();
    CreateTopologyCapabilities();
    CreateGeometryCapabilities();
}


void MgServerGetProviderCapabilities::CreateConnectionCapabilities()
{
    CHECKNULL(m_xmlCap, L"MgServerGetProviderCapabilities::CreateConnectionCapabilities");
    CHECKNULL(m_fdoConn, L"MgServerGetProviderCapabilities::CreateConnectionCapabilities");

    DOMElement* root = m_xmlCap->GetRootNode();
    CHECKNULL(root, L"MgServerGetProviderCapabilities::CreateConnectionCapabilities");

    DOMElement* connNode = m_xmlCap->AddChildNode(root, "Connection");
    CHECKNULL(connNode, L"MgServerGetProviderCapabilities::CreateConnectionCapabilities");

    FdoPtr<FdoIConnectionCapabilities> ficc = m_fdoConn->GetConnectionCapabilities();
    CHECKNULL((FdoIConnectionCapabilities*)ficc, L"MgServerGetProviderCapabilities::CreateConnectionCapabilities");

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
        CHECKNULL(scNode, L"MgServerGetProviderCapabilities::CreateConnectionCapabilities");

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

    bool supportsLongTransactions = ficc->SupportsLongTransactions();
    m_xmlCap->AddTextNode(connNode, "SupportsLongTransactions", supportsLongTransactions);

    bool supportsSQL = ficc->SupportsSQL();
    m_xmlCap->AddTextNode(connNode, "SupportsSQL", supportsSQL);

    bool supportsConfiguration = ficc->SupportsConfiguration();
    m_xmlCap->AddTextNode(connNode, "SupportsConfiguration", supportsConfiguration);
}


void MgServerGetProviderCapabilities::CreateSchemaCapabilities()
{
    CHECKNULL(m_xmlCap, L"MgServerGetProviderCapabilities::CreateSchemaCapabilities");
    CHECKNULL(m_fdoConn, L"MgServerGetProviderCapabilities::CreateSchemaCapabilities");

    FdoPtr<FdoISchemaCapabilities> fsc = m_fdoConn->GetSchemaCapabilities();
    CHECKNULL((FdoISchemaCapabilities*)fsc, L"MgServerGetProviderCapabilities::CreateSchemaCapabilities");

    DOMElement* root = m_xmlCap->GetRootNode();
    CHECKNULL(root, L"MgServerGetProviderCapabilities::CreateSchemaCapabilities");

    DOMElement* schemaNode = m_xmlCap->AddChildNode(root, "Schema");
    CHECKNULL(schemaNode, L"MgServerGetProviderCapabilities::CreateSchemaCapabilities");

    // Add all class types
    FdoInt32 cnt = 0;
    FdoClassType* fct = fsc->GetClassTypes(cnt);
    if (cnt > 0 && fct != NULL)
    {
        DOMElement* classNode = m_xmlCap->AddChildNode(schemaNode, "Class");
        CHECKNULL(classNode, L"MgServerGetProviderCapabilities::CreateSchemaCapabilities");

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
        CHECKNULL(dataNode, L"MgServerGetProviderCapabilities::CreateSchemaCapabilities");

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
        CHECKNULL(sagtNode, L"MgServerGetProviderCapabilities::CreateSchemaCapabilities");

        for (FdoInt32 i=0; i < cnt; i++)
        {
            string sagtStr = s_FdoDataType[sagt[i]];
            m_xmlCap->AddTextNode(sagtNode, "Type", sagtStr.c_str());
        }
    }

    bool supportsSchemaModification  = fsc->SupportsSchemaModification();
    m_xmlCap->AddTextNode(schemaNode, "SupportsSchemaModification", supportsSchemaModification);
}

void MgServerGetProviderCapabilities::CreateCommandCapabilities()
{
    CHECKNULL(m_xmlCap, L"MgServerGetProviderCapabilities::CreateCommandCapabilities");
    CHECKNULL(m_fdoConn, L"MgServerGetProviderCapabilities::CreateCommandCapabilities");

    FdoPtr<FdoICommandCapabilities> fcc = m_fdoConn->GetCommandCapabilities();
    CHECKNULL((FdoICommandCapabilities*)fcc, L"MgServerGetProviderCapabilities::CreateCommandCapabilities");

    DOMElement* root = m_xmlCap->GetRootNode();
    CHECKNULL(root, L"MgServerGetProviderCapabilities::CreateCommandCapabilities");

    DOMElement* cmdNode = m_xmlCap->AddChildNode(root, "Command");
    CHECKNULL(cmdNode, L"MgServerGetProviderCapabilities::CreateCommandCapabilities");

    // Add all command types
    FdoInt32 cnt = 0;
    FdoInt32* fcmd = fcc->GetCommands(cnt);
    if (cnt > 0 && fcmd != NULL)
    {
        DOMElement* scNode = m_xmlCap->AddChildNode(cmdNode, "SupportedCommands");
        CHECKNULL(scNode, L"MgServerGetProviderCapabilities::CreateCommandCapabilities");

        for (FdoInt32 i=0; i < cnt; i++)
        {
            string cmdStr = s_FdoCommandType[(FdoCommandType)fcmd[i]];
            if (!cmdStr.empty())
            {
                m_xmlCap->AddTextNode(scNode, "Name", cmdStr.c_str());
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

    // Supports SupportsSelectDistinct
    bool supportsSelectOrdering = fcc->SupportsSelectOrdering();
    m_xmlCap->AddTextNode(cmdNode, "SupportsSelectOrdering", supportsSelectOrdering);

    // Supports SupportsSelectDistinct
    bool supportsSelectGrouping = fcc->SupportsSelectGrouping();
    m_xmlCap->AddTextNode(cmdNode, "SupportsSelectGrouping", supportsSelectGrouping);
}

void MgServerGetProviderCapabilities::CreateFilterCapabilities()
{
    CHECKNULL(m_xmlCap, L"MgServerGetProviderCapabilities::CreateFilterCapabilities");
    CHECKNULL(m_fdoConn, L"MgServerGetProviderCapabilities::CreateFilterCapabilities");

    FdoPtr<FdoIFilterCapabilities> ffc = m_fdoConn->GetFilterCapabilities();
    CHECKNULL((FdoIFilterCapabilities*)ffc, L"MgServerGetProviderCapabilities::CreateFilterCapabilities");

    DOMElement* root = m_xmlCap->GetRootNode();
    CHECKNULL(root, L"MgServerGetProviderCapabilities::CreateFilterCapabilities");

    DOMElement* filterNode = m_xmlCap->AddChildNode(root, "Filter");
    CHECKNULL(filterNode, L"MgServerGetProviderCapabilities::CreateFilterCapabilities");

    // Add all condition types
    FdoInt32 cnt = 0;
    FdoConditionType* fct = ffc->GetConditionTypes(cnt);
    if (cnt > 0 && fct != NULL)
    {
        DOMElement* condNode = m_xmlCap->AddChildNode(filterNode, "Condition");
        CHECKNULL(condNode, L"MgServerGetProviderCapabilities::CreateFilterCapabilities");

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
        CHECKNULL(fsoNode, L"MgServerGetProviderCapabilities::CreateFilterCapabilities");

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
        CHECKNULL(distNode, L"MgServerGetProviderCapabilities::CreateFilterCapabilities");

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

void MgServerGetProviderCapabilities::CreateExpressionCapabilities()
{
    CHECKNULL(m_xmlCap, L"MgServerGetProviderCapabilities::CreateExpressionCapabilities");
    CHECKNULL(m_fdoConn, L"MgServerGetProviderCapabilities::CreateExpressionCapabilities");

    FdoPtr<FdoIExpressionCapabilities> fec = m_fdoConn->GetExpressionCapabilities();
    CHECKNULL((FdoIExpressionCapabilities*)fec, L"MgServerGetProviderCapabilities::CreateExpressionCapabilities");

    DOMElement* root = m_xmlCap->GetRootNode();
    CHECKNULL(root, L"MgServerGetProviderCapabilities::CreateExpressionCapabilities");

    DOMElement* expressionNode = m_xmlCap->AddChildNode(root, "Expression");
    CHECKNULL(expressionNode, L"MgServerGetProviderCapabilities::CreateExpressionCapabilities");

    // Add all expression types
    FdoInt32 cnt = 0;
    FdoExpressionType* fet = fec->GetExpressionTypes(cnt);
    if (cnt > 0 && fet != NULL)
    {
        DOMElement* typeNode = m_xmlCap->AddChildNode(expressionNode, "Type");
        CHECKNULL(typeNode, L"MgServerGetProviderCapabilities::CreateExpressionCapabilities");

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
            DOMElement* funcDefColNode = m_xmlCap->AddChildNode(expressionNode, "FunctionDefinitionCollection");
            CHECKNULL(funcDefColNode, L"MgServerGetProviderCapabilities::CreateExpressionCapabilities");

            for (FdoInt32 i=0; i < funcCnt; i++)
            {
                // Add function definition element
                FdoPtr<FdoFunctionDefinition> ffd = ffdc->GetItem(i);
                CHECKNULL((FdoFunctionDefinition*)ffd, L"MgServerGetProviderCapabilities::CreateExpressionCapabilities");

                DOMElement* funcDefNode = m_xmlCap->AddChildNode(funcDefColNode, "FunctionDefinition");
                CHECKNULL(funcDefNode, L"MgServerGetProviderCapabilities::CreateExpressionCapabilities");

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
                        DOMElement* argDefColNode = m_xmlCap->AddChildNode(funcDefNode, "ArgumentDefinitionCollection");
                        CHECKNULL(argDefColNode, L"MgServerGetProviderCapabilities::CreateExpressionCapabilities");

                        for (FdoInt32 j=0; j < argCnt; j++)
                        {
                            // Add ArgumentDefinition for each argument
                            FdoPtr<FdoArgumentDefinition> fad = argCol->GetItem(j);
                            CHECKNULL((FdoArgumentDefinition*)fad, L"MgServerGetProviderCapabilities::CreateExpressionCapabilities");

                            DOMElement* argDefNode = m_xmlCap->AddChildNode(argDefColNode, "ArgumentDefinition");
                            CHECKNULL(argDefNode, L"MgServerGetProviderCapabilities::CreateExpressionCapabilities");

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

void MgServerGetProviderCapabilities::CreateRasterCapabilities()
{
    CHECKNULL(m_xmlCap, L"MgServerGetProviderCapabilities::CreateRasterCapabilities");
    CHECKNULL(m_fdoConn, L"MgServerGetProviderCapabilities::CreateRasterCapabilities");

    FdoPtr<FdoIRasterCapabilities> frc = m_fdoConn->GetRasterCapabilities();
    CHECKNULL((FdoIRasterCapabilities*)frc, L"MgServerGetProviderCapabilities::CreateRasterCapabilities");

    DOMElement* root = m_xmlCap->GetRootNode();
    CHECKNULL(root, L"MgServerGetProviderCapabilities::CreateRasterCapabilities");

    DOMElement* rasterNode = m_xmlCap->AddChildNode(root, "Raster");
    CHECKNULL(rasterNode, L"MgServerGetProviderCapabilities::CreateRasterCapabilities");

    // Supports Raster
    bool supportsRaster = frc->SupportsRaster();
    m_xmlCap->AddTextNode(rasterNode, "SupportsRaster", supportsRaster);

    // Supports Stitching
    bool supportsStitching = frc->SupportsStitching();
    m_xmlCap->AddTextNode(rasterNode, "SupportsStitching", supportsStitching);

    bool supportsSubsampling = frc->SupportsSubsampling();
    m_xmlCap->AddTextNode(rasterNode, "SupportsSubsampling", supportsSubsampling);

}

void MgServerGetProviderCapabilities::CreateTopologyCapabilities()
{
    CHECKNULL(m_xmlCap, L"MgServerGetProviderCapabilities::CreateTopologyCapabilities");
    CHECKNULL(m_fdoConn, L"MgServerGetProviderCapabilities::CreateTopologyCapabilities");

    MG_FEATURE_SERVICE_TRY()

    FdoPtr<FdoITopologyCapabilities> frc = m_fdoConn->GetTopologyCapabilities();

    // Provider has no topology capabilities
    if (NULL == (FdoITopologyCapabilities*)frc)
    {
        return;
    }

    DOMElement* root = m_xmlCap->GetRootNode();
    CHECKNULL(root, L"MgServerGetProviderCapabilities::CreateTopologyCapabilities");

    DOMElement* topologyNode = m_xmlCap->AddChildNode(root, "Topology");
    CHECKNULL(topologyNode, L"MgServerGetProviderCapabilities::CreateTopologyCapabilities");

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
    MG_FEATURE_SERVICE_CATCH(L"MgServerGetProviderCapabilities.CreateTopologyCapabilities")

}

void MgServerGetProviderCapabilities::CreateGeometryCapabilities()
{
    CHECKNULL(m_xmlCap, L"MgServerGetProviderCapabilities.CreateGeometryCapabilities");
    CHECKNULL(m_fdoConn, L"MgServerGetProviderCapabilities.CreateGeometryCapabilities");

    MG_FEATURE_SERVICE_TRY()

    FdoPtr<FdoIGeometryCapabilities> fgc = m_fdoConn->GetGeometryCapabilities();

    // Provider has no geometric capabilities
    if (NULL == (FdoIGeometryCapabilities*)fgc)
    {
        return;
    }

    DOMElement* root = m_xmlCap->GetRootNode();
    CHECKNULL(root, L"MgServerGetProviderCapabilities.CreateGeometryCapabilities");

    DOMElement* geometryNode = m_xmlCap->AddChildNode(root, "Geometry");
    CHECKNULL(geometryNode, L"MgServerGetProviderCapabilities.CreateGeometryCapabilities");

    FdoInt32 cnt = 0;
    FdoGeometryType* geomType = fgc->GetGeometryTypes(cnt);
    if (cnt > 0 && geomType != NULL)
    {
        DOMElement* geometryTypeNode = m_xmlCap->AddChildNode(geometryNode, "Types");
        CHECKNULL(geometryTypeNode, L"MgServerGetProviderCapabilities.CreateGeometryCapabilities");

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
        CHECKNULL(geometryCompNode, L"MgServerGetProviderCapabilities.CreateGeometryCapabilities");

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

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerGetProviderCapabilities.CreateGeometryCapabilities")
}

bool MgServerGetProviderCapabilities::IsConnectionOpen()
{
    FdoConnectionState state = m_fdoConn->GetConnectionState();
    if (state != FdoConnectionState_Open)
        return false;

    return true;
}

bool MgServerGetProviderCapabilities::Initialize()
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
    s_FdoCommandType[FdoCommandType_FirstProviderCommand]               = "FirstProviderCommand";
    s_FdoCommandType[FdoCommandType_SelectAggregates]                   = "SelectAggregates";

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

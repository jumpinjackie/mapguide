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

#include "Foundation.h"

#include "XmlJsonConvert.h"

#include <string>
#include <map>
#include <vector>
#include <set>
using namespace std;

static map<string, INT32> s_elementPathTypeMap;
static set<string> s_multiElementPaths;

bool MgXmlJsonConvert::m_isInitialized = MgXmlJsonConvert::Initialize();

MgXmlJsonConvert::MgXmlJsonConvert()
{
}

MgXmlJsonConvert::~MgXmlJsonConvert()
{
}

void MgXmlJsonConvert::ToJson(Ptr<MgByteReader> &byteReader, bool bClean)
{
    string xmlDoc;
    byteReader->ToStringUtf8(xmlDoc);

    string jsonDoc;
    this->ToJson(xmlDoc, jsonDoc, bClean);
    Ptr<MgByteSource> byteSource = new MgByteSource(
        (unsigned char*)jsonDoc.c_str(), (INT32)jsonDoc.length());
    byteSource->SetMimeType(MgMimeType::Json);
    byteReader.Attach(byteSource->GetReader());
}

void MgXmlJsonConvert::ToJson(const string &xmlString, string &jsonString, bool bClean)
{
    // Parse into DOM
    m_xmlUtil.ParseString(xmlString.c_str());
    DOMElement *root = m_xmlUtil.GetRootNode();
    string nodeName = MgUtil::WideCharToMultiByte(X2W(root->getNodeName()));
    m_jsonDoc.BeginObject(nodeName);
    {
        XmlToJsonNode((DOMNode *)root, bClean);
    }
    m_jsonDoc.EndObject();

    m_jsonDoc.Print(jsonString);
}

//  XmlToJSONnode:  Output an XmlElement, possibly as part of a higher array
void MgXmlJsonConvert::XmlToJsonNode(DOMNode *node, bool bClean)
{
    // Build a sorted list of key-value pairs
    //  where   key is case-sensitive nodeName
    //          value is an ArrayList of string or XmlElement
    //  so that we know whether the nodeName is an array or not.

    NameNodeList childNameNodeList;
    string nodeName;

    //  Add in all node attributes

    if (node->hasAttributes())
    {
        DOMNamedNodeMap *attributes = node->getAttributes();
        XMLSize_t attributeCount = attributes->getLength();
        DOMNode *attribute;
        string nodeValue;
        for (XMLSize_t i = 0; i < attributeCount; i++)
        {
            attribute = attributes->item(i);
            MgUtil::WideCharToMultiByte(X2W(attribute->getNodeName()), nodeName);
            STRING wNodeValue = X2W(attribute->getNodeValue());
            MgUtil::WideCharToMultiByte(wNodeValue, nodeValue);

            if (bClean)
            {
                int type;
                if (GetAttributeType(attribute, node, type))
                {
                    switch (type)
                    {
                    case XML_DATA_TYPE_BOOLEAN:
                        {
                            bool b = MgUtil::StringToBoolean(wNodeValue);
                            m_jsonDoc.AddAttribute(nodeName, b);
                        }
                        break;
                    case XML_DATA_TYPE_NUM_INT:
                        {
                            INT32 i = MgUtil::StringToInt32(wNodeValue);
                            m_jsonDoc.AddAttribute(nodeName, i);
                        }
                        break;
                    case XML_DATA_TYPE_NUM_DOUBLE:
                        {
                            double d = MgUtil::StringToDouble(wNodeValue);
                            m_jsonDoc.AddAttribute(nodeName, d);
                        }
                        break;
                    default:
                        m_jsonDoc.AddAttribute(nodeName, nodeValue);
                        break;
                    }
                }
                else //Type assumed to be string
                {
                    m_jsonDoc.AddAttribute(nodeName, nodeValue);
                }
            }
            else
            {
                // Write the attribute as a single array element
                m_jsonDoc.BeginArray(1, "@" + nodeName);
                m_jsonDoc.SetArrayValue(0, nodeValue);
                m_jsonDoc.EndArray();
            }
        }
    }

    //  Add in all nodes

    int childIndex = 0;
    DOMNode *childNode = node->getFirstChild();
    while (childNode != NULL)
    {
        short childNodeType = childNode->getNodeType();
        bool isValidNode = false;

        MgUtil::WideCharToMultiByte(X2W(childNode->getNodeName()), nodeName);

        if (childNodeType == DOMNode::TEXT_NODE)
        {
            string textContent;
            MgUtil::WideCharToMultiByte(X2W(childNode->getTextContent()), textContent);
            isValidNode = this->ValidateTextContent(textContent);
        }
        else if (childNodeType == DOMNode::ELEMENT_NODE)
        {
            isValidNode = true;
        }

        if (isValidNode)
        {
            vector<DOMNode *> *childNodeList = childNameNodeList[nodeName];
            vector<DOMNode *> *nodeList;
            if (childNodeList == NULL)
            {
                nodeList = new vector<DOMNode *>();
                childNameNodeList[nodeName] = nodeList;
            }
            else
            {
                nodeList = childNodeList;
            }
            nodeList->push_back(childNode);
        }

        childNode = childNode->getNextSibling();
    }

    for (NameNodeList::iterator iter = childNameNodeList.begin(); iter != childNameNodeList.end(); iter++)
    {
        vector<DOMNode *> *nodeList = iter->second;
        nodeName = iter->first;
        if (nodeList->size() == 1)
        {
            childNode = (*nodeList)[0];
            if (bClean)
            {
                bool bMultiple = false;
                IsMultiple(childNode, bMultiple);

                //If this node is known to house more than one element, then we must write
                //a JSON array, even if there's only one element to write
                if (bMultiple)
                {
                    m_jsonDoc.BeginArray(1, nodeName);
                    this->ProcessArrayNode(0, childNode, bClean);
                    m_jsonDoc.EndArray();
                }
                else
                {
                    this->ProcessObjectNode(childNode, bClean);
                }
            }
            else
            {
                this->ProcessObjectNode(childNode, bClean);
            }
        }
        else
        {
            int nodeListLength = (int)nodeList->size();
            m_jsonDoc.BeginArray(nodeListLength, nodeName);
            {
                for (int i = 0; i < nodeListLength; i++)
                {
                    childNode = (*nodeList)[i];
                    this->ProcessArrayNode(i, childNode, bClean);
                }
            }
            m_jsonDoc.EndArray();
        }
        delete nodeList;
    }
}

bool MgXmlJsonConvert::ValidateTextContent(const string &textContent)
{
    bool isValidText = false;
    const char *str = textContent.c_str();
    for (int i = 0; i < (int)textContent.length(); i++)
    {
        if (str[i] != '\t' && str[i] != '\n' && str[i] != ' ')
        {
            isValidText = true;
        }
    }
    return isValidText;
}

void MgXmlJsonConvert::ProcessObjectNode(DOMNode *node, bool bClean)
{
    short nodeType = node->getNodeType();
    string nodeName;
    MgUtil::WideCharToMultiByte(X2W(node->getNodeName()), nodeName);
    if (nodeType == DOMNode::TEXT_NODE)
    {
        STRING wTextContent = X2W(node->getTextContent());
        string textContent = MgUtil::WideCharToMultiByte(wTextContent);

        if (bClean)
        {
            int type;
            if (GetElementType(node, type))
            {
                switch (type)
                {
                case XML_DATA_TYPE_BOOLEAN:
                    {
                        bool b = MgUtil::StringToBoolean(wTextContent);
                        m_jsonDoc.Add(nodeName, b);
                    }
                    break;
                case XML_DATA_TYPE_NUM_INT:
                    {
                        INT32 i = MgUtil::StringToInt32(wTextContent);
                        m_jsonDoc.Add(nodeName, i);
                    }
                    break;
                case XML_DATA_TYPE_NUM_DOUBLE:
                    {
                        double d = MgUtil::StringToDouble(wTextContent);
                        m_jsonDoc.Add(nodeName, d);
                    }
                    break;
                default:
                    m_jsonDoc.Add(nodeName, textContent);
                    break;
                }
            }
            else //Type assumed to be string
            {
                m_jsonDoc.Add(nodeName, textContent);
            }
        }
        else
        {
            // Write the text as a single array element
            m_jsonDoc.BeginArray(1, nodeName);
            m_jsonDoc.SetArrayValue(0, textContent);
            m_jsonDoc.EndArray();
        }
        return;
    }
    else if (nodeType == DOMNode::ELEMENT_NODE)
    {
        if (!node->hasAttributes())
        {
            // Convert  <aa></aa> into "aa":null
            //          <aa>xx</aa> into "aa":"xx"
            DOMNodeList *childNodes = node->getChildNodes();
            XMLSize_t childNodeCount = childNodes->getLength();
            if (childNodeCount == 0)
            {
                m_jsonDoc.AddEmptyObject(nodeName);
                return;
            }
            else if (childNodeCount == 1)
            {
                DOMNode *childNode = childNodes->item(0);
                if (childNode->getNodeType() == DOMNode::TEXT_NODE)
                {
                    STRING wTextContent = X2W(childNode->getTextContent());
                    string textContent = MgUtil::WideCharToMultiByte(wTextContent);
                    if (this->ValidateTextContent(textContent))
                    {
                        if (bClean)
                        {
                            int type;
                            if (GetElementType(node, type))
                            {
                                switch (type)
                                {
                                case XML_DATA_TYPE_BOOLEAN:
                                    {
                                        bool b = MgUtil::StringToBoolean(wTextContent);
                                        m_jsonDoc.Add(nodeName, b);
                                    }
                                    break;
                                case XML_DATA_TYPE_NUM_INT:
                                    {
                                        INT32 i = MgUtil::StringToInt32(wTextContent);
                                        m_jsonDoc.Add(nodeName, i);
                                    }
                                    break;
                                case XML_DATA_TYPE_NUM_DOUBLE:
                                    {
                                        double d = MgUtil::StringToDouble(wTextContent);
                                        m_jsonDoc.Add(nodeName, d);
                                    }
                                    break;
                                default:
                                    m_jsonDoc.Add(nodeName, textContent);
                                    break;
                                }
                            }
                            else //Type assumed to be string
                            {
                                m_jsonDoc.Add(nodeName, textContent);
                            }
                        }
                        else
                        {
                            // Write the text as a single array element
                            m_jsonDoc.BeginArray(1, nodeName);
                            m_jsonDoc.SetArrayValue(0, textContent);
                            m_jsonDoc.EndArray();
                        }
                    }
                    return;
                }
            }
        }
    }

    if (bClean)
    {
        m_jsonDoc.BeginObject(nodeName);
        this->XmlToJsonNode(node, bClean);
        m_jsonDoc.EndObject();
    }
    else
    {
        m_jsonDoc.BeginArray(1, nodeName);
        m_jsonDoc.BeginArrayObject(0);
        {
            this->XmlToJsonNode(node, bClean);
        }
        m_jsonDoc.EndArrayObject();
        m_jsonDoc.EndArray();
    }
}

void MgXmlJsonConvert::ProcessArrayNode(int index, DOMNode *node, bool bClean)
{
    short nodeType = node->getNodeType();
    string nodeName;
    MgUtil::WideCharToMultiByte(X2W(node->getNodeName()), nodeName);

    if (nodeType == DOMNode::TEXT_NODE)
    {
        string textContent;
        MgUtil::WideCharToMultiByte(X2W(node->getTextContent()), textContent);
        m_jsonDoc.Add(nodeName, textContent);
        return;
    }
    else if (nodeType == DOMNode::ELEMENT_NODE)
    {
        if (!node->hasAttributes())
        {
            // Convert  <aa></aa> into "aa":null
            //          <aa>xx</aa> into "aa":"xx"
            DOMNodeList *childNodes = node->getChildNodes();
            XMLSize_t childNodeCount = childNodes->getLength();
            if (childNodeCount == 0)
            {
                // Write the node as a zero array element
                m_jsonDoc.BeginArrayObject(index);
                m_jsonDoc.EndArrayObject();
                return;
            }
            else if (childNodeCount == 1)
            {
                DOMNode *childNode = childNodes->item(0);
                if (childNode->getNodeType() == DOMNode::TEXT_NODE)
                {
                    string textContent;
                    STRING wTextContent = X2W(node->getTextContent());
                    MgUtil::WideCharToMultiByte(wTextContent, textContent);
                    if (this->ValidateTextContent(textContent))
                    {
                        if (bClean)
                        {
                            int type;
                            if (GetElementType(node, type))
                            {
                                switch (type)
                                {
                                case XML_DATA_TYPE_BOOLEAN:
                                    {
                                        bool b = MgUtil::StringToBoolean(wTextContent);
                                        m_jsonDoc.SetArrayValue(index, b);
                                    }
                                    break;
                                case XML_DATA_TYPE_NUM_INT:
                                    {
                                        INT32 i = MgUtil::StringToInt32(wTextContent);
                                        m_jsonDoc.SetArrayValue(index, i);
                                    }
                                    break;
                                case XML_DATA_TYPE_NUM_DOUBLE:
                                    {
                                        double d = MgUtil::StringToDouble(wTextContent);
                                        m_jsonDoc.SetArrayValue(index, d);
                                    }
                                    break;
                                default:
                                    m_jsonDoc.SetArrayValue(index, textContent);
                                    break;
                                }
                            }
                            else //Type assumed to be string
                            {
                                m_jsonDoc.SetArrayValue(index, textContent);
                            }
                        }
                        else
                        {
                            m_jsonDoc.SetArrayValue(index, textContent);
                        }
                    }
                    return;
                }
            }
        }
    }

    // Write the node as a single array element
    m_jsonDoc.BeginArrayObject(index);
    {
        this->XmlToJsonNode(node, bClean);
    }
    m_jsonDoc.EndArrayObject();
}

bool MgXmlJsonConvert::GetAttributePath(DOMNode * attribute, DOMNode * parent, string & path, const XMLCh * suffix)
{
    if (NULL != attribute)
    {
        STRING elPath = L"/@";
        elPath += X2W(attribute->getNodeName());
        if (NULL != suffix)
        {
            STRING ws;
            ws = X2W(suffix);
            elPath += ws;
        }
        DOMNode* currentNode = parent;
        while (NULL != currentNode)
        {
            DOMNode::NodeType nt = currentNode->getNodeType();
            if (nt == DOMNode::ELEMENT_NODE)
            {
                STRING pathPart = L"/";
                pathPart += X2W(currentNode->getNodeName());
                elPath = pathPart + elPath;
                currentNode = currentNode->getParentNode();
                if (NULL == currentNode)
                    break;
            }
            else
            {
                break;
            }
        }
        path = MgUtil::WideCharToMultiByte(elPath);
        return true;
    }
    return false;
}

bool MgXmlJsonConvert::GetElementPath(DOMNode * node, string & path, const XMLCh * suffix)
{
    if (NULL != node)
    {
        STRING elPath = L"/";
        if (node->getNodeType() == DOMNode::ATTRIBUTE_NODE)
        {
            elPath += L"@";
        }
        elPath += X2W(node->getNodeName());
        if (NULL != suffix)
        {
            STRING ws;
            ws = X2W(suffix);
            elPath += ws;
        }
        DOMNode* currentNode = node->getParentNode();
        while (NULL != currentNode)
        {
            DOMNode::NodeType nt = currentNode->getNodeType();
            if (nt == DOMNode::ELEMENT_NODE)
            {
                STRING pathPart = L"/";
                pathPart += X2W(currentNode->getNodeName());
                elPath = pathPart + elPath;
                currentNode = currentNode->getParentNode();
                if (NULL == currentNode)
                    break;
            }
            else
            {
                break;
            }
        }
        path = MgUtil::WideCharToMultiByte(elPath);
        return true;
    }
    return false;
}

bool MgXmlJsonConvert::GetAttributeType(DOMNode * attribute, DOMNode * parent, int & type, const XMLCh * suffix)
{
    string path;
    if (GetAttributePath(attribute, parent, path, suffix))
    {
        if (s_elementPathTypeMap.find(path) != s_elementPathTypeMap.end())
        {
            type = s_elementPathTypeMap[path];
            return true;
        }
    }
    return false;
}


bool MgXmlJsonConvert::GetElementType(DOMNode * node, int & type, const XMLCh * suffix)
{
    string path;
    if (GetElementPath(node, path, suffix))
    {
        if (s_elementPathTypeMap.find(path) != s_elementPathTypeMap.end())
        {
            type = s_elementPathTypeMap[path];
            return true;
        }
    }
    return false;
}

bool MgXmlJsonConvert::IsMultiple(DOMNode * node, bool & isMultiple, const XMLCh * suffix)
{
    string path;
    if (GetElementPath(node, path, suffix))
    {
        isMultiple = (s_multiElementPaths.find(path) != s_multiElementPaths.end());
        return true;
    }
    return false;
}

bool MgXmlJsonConvert::Initialize()
{
    //ApplicationDefinition-1.0.0.xsd
    s_elementPathTypeMap["/ApplicationDefinition/MapSet/MapGroup/InitialView/CenterX"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/ApplicationDefinition/MapSet/MapGroup/InitialView/CenterY"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/ApplicationDefinition/MapSet/MapGroup/InitialView/Scale"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/ApplicationDefinition/WidgetSet/Widget/Disabled"] = XML_DATA_TYPE_BOOLEAN;
    //ApplicationDefinitionInfo-1.0.0.xsd
    s_elementPathTypeMap["/ApplicationDefinitionWidgetInfoSet/WidgetInfo/StandardUi"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/ApplicationDefinitionWidgetInfoSet/WidgetInfo/Parameter/IsMandatory"] = XML_DATA_TYPE_BOOLEAN;
    //DataStoreList-1.0.0.xsd
    s_elementPathTypeMap["/DataStoreList/DataStore/FdoEnabled"] = XML_DATA_TYPE_BOOLEAN;
    //DrawingSource-1.0.0.xsd
    s_elementPathTypeMap["/DrawingSource/Sheet/Extent/MinX"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/DrawingSource/Sheet/Extent/MinY"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/DrawingSource/Sheet/Extent/MaxX"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/DrawingSource/Sheet/Extent/MaxY"] = XML_DATA_TYPE_NUM_DOUBLE;
    //FdoLongTransactionList-1.0.0.xsd
    s_elementPathTypeMap["/FdoLongTransactionList/LongTransaction/@IsActive"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/FdoLongTransactionList/LongTransaction/@IsFrozen"] = XML_DATA_TYPE_BOOLEAN;
    //FdoSpatialContextList-1.0.0.xsd
    s_elementPathTypeMap["/FdoSpatialContextList/SpatialContext/@IsActive"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/FdoSpatialContextList/SpatialContext/XYTolerance"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/FdoSpatialContextList/SpatialContext/ZTolerance"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/FdoSpatialContextList/SpatialContext/Extent/LowerLeftCoordinate/X"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/FdoSpatialContextList/SpatialContext/Extent/LowerLeftCoordinate/Y"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/FdoSpatialContextList/SpatialContext/Extent/UpperRightCoordinate/X"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/FdoSpatialContextList/SpatialContext/Extent/UpperRightCoordinate/Y"] = XML_DATA_TYPE_NUM_DOUBLE;
    //FdoProviderCapabilities-1.1.0.xsd
    s_elementPathTypeMap["/FeatureProviderCapabilities/Geometry/Dimensionality"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/FeatureProviderCapabilities/Connection/SupportsLocking"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/FeatureProviderCapabilities/Connection/SupportsTimeout"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/FeatureProviderCapabilities/Connection/SupportsTransactions"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/FeatureProviderCapabilities/Connection/SupportsLongTransactions"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/FeatureProviderCapabilities/Connection/SupportsSQL"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/FeatureProviderCapabilities/Connection/SupportsConfiguration"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/FeatureProviderCapabilities/Connection/SupportsSavePoint"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/FeatureProviderCapabilities/Schema/SupportsInheritance"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/FeatureProviderCapabilities/Schema/SupportsMultipleSchemas"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/FeatureProviderCapabilities/Schema/SupportsObjectProperties"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/FeatureProviderCapabilities/Schema/SupportsAssociationProperties"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/FeatureProviderCapabilities/Schema/SupportsSchemaOverrides"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/FeatureProviderCapabilities/Schema/SupportsNetworkModel"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/FeatureProviderCapabilities/Schema/SupportsAutoIdGeneration"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/FeatureProviderCapabilities/Schema/SupportsDataStoreScopeUniqueIdGeneration"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/FeatureProviderCapabilities/Schema/SupportsSchemaModification"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/FeatureProviderCapabilities/Command/SupportsParameters"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/FeatureProviderCapabilities/Command/SupportsTimeout"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/FeatureProviderCapabilities/Command/SupportsSelectExpressions"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/FeatureProviderCapabilities/Command/SupportsSelectFunctions"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/FeatureProviderCapabilities/Command/SupportsSelectDistinct"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/FeatureProviderCapabilities/Command/SupportsSelectOrdering"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/FeatureProviderCapabilities/Command/SupportsSelectGrouping"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/FeatureProviderCapabilities/Filter/SupportsGeodesicDistance"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/FeatureProviderCapabilities/Filter/SupportsNonLiteralGeometricOperations"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/FeatureProviderCapabilities/Raster/SupportsRaster"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/FeatureProviderCapabilities/Raster/SupportsStitching"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/FeatureProviderCapabilities/Raster/SupportsSubsampling"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/FeatureProviderCapabilities/Topology/SupportsTopology"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/FeatureProviderCapabilities/Topology/SupportsTopologicalHierarchy"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/FeatureProviderCapabilities/Topology/BreaksCurveCrossingsAutomatically"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/FeatureProviderCapabilities/Topology/ActivatesTopologyByArea"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/FeatureProviderCapabilities/Topology/ConstrainsFeatureMovements"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/FeatureProviderCapabilities/Expression/FunctionDefinitionList/FunctionDefinition/IsAggregate"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/FeatureProviderCapabilities/Expression/FunctionDefinitionList/FunctionDefinition/IsSupportsVariableArgumentsList"] = XML_DATA_TYPE_BOOLEAN;
    //FeatureProviderRegistry-1.0.0.xsd
    s_elementPathTypeMap["/FeatureProviderRegistry/FeatureProvider/ConnectionProperties/ConnectionProperty/@Required"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/FeatureProviderRegistry/FeatureProvider/ConnectionProperties/ConnectionProperty/@Protected"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/FeatureProviderRegistry/FeatureProvider/ConnectionProperties/ConnectionProperty/@Enumerable"] = XML_DATA_TYPE_BOOLEAN;
    //FeatureSource-1.0.0.xsd
    s_elementPathTypeMap["/FeatureSource/Extension/AttributeRelate/ForceOneToOne"] = XML_DATA_TYPE_BOOLEAN;
    //LayerDefinition-2.4.0.xsd
    s_elementPathTypeMap["/LayerDefinition/DrawingLayerDefinition/Opacity"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/LayerDefinition/DrawingLayerDefinition/MinScale"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/LayerDefinition/DrawingLayerDefinition/MaxScale"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/LayerDefinition/VectorLayerDefinition/Opacity"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/LayerDefinition/VectorLayerDefinition/VectorScaleRange/MinScale"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/LayerDefinition/VectorLayerDefinition/VectorScaleRange/MaxScale"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/LayerDefinition/VectorLayerDefinition/VectorScaleRange/AreaTypeStyle/AreaRule/Label/AdvancedPlacement/ScaleLimit"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/LayerDefinition/VectorLayerDefinition/VectorScaleRange/AreaTypeStyle/ShowInLegend"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/LayerDefinition/VectorLayerDefinition/VectorScaleRange/LineTypeStyle/LineRule/Label/AdvancedPlacement/ScaleLimit"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/LayerDefinition/VectorLayerDefinition/VectorScaleRange/LineTypeStyle/ShowInLegend"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/LayerDefinition/VectorLayerDefinition/VectorScaleRange/PointTypeStyle/PointRule/Label/AdvancedPlacement/ScaleLimit"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/LayerDefinition/VectorLayerDefinition/VectorScaleRange/PointTypeStyle/DisplayAsText"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/LayerDefinition/VectorLayerDefinition/VectorScaleRange/PointTypeStyle/AllowOverpost"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/LayerDefinition/VectorLayerDefinition/VectorScaleRange/PointTypeStyle/ShowInLegend"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/LayerDefinition/VectorLayerDefinition/VectorScaleRange/PointTypeStyle/PointRule/Label/MaintainAspect"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/LayerDefinition/VectorLayerDefinition/VectorScaleRange/PointTypeStyle/PointRule/PointSymbolization2D/Mark/MaintainAspect"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/LayerDefinition/VectorLayerDefinition/VectorScaleRange/PointTypeStyle/PointRule/PointSymbolization2D/Image/MaintainAspect"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/LayerDefinition/VectorLayerDefinition/VectorScaleRange/PointTypeStyle/PointRule/PointSymbolization2D/Font/MaintainAspect"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/LayerDefinition/VectorLayerDefinition/VectorScaleRange/PointTypeStyle/PointRule/PointSymbolization2D/Font/Bold"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/LayerDefinition/VectorLayerDefinition/VectorScaleRange/PointTypeStyle/PointRule/PointSymbolization2D/Font/Italic"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/LayerDefinition/VectorLayerDefinition/VectorScaleRange/PointTypeStyle/PointRule/PointSymbolization2D/Font/Underlined"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/LayerDefinition/VectorLayerDefinition/VectorScaleRange/PointTypeStyle/PointRule/PointSymbolization2D/W2D/MaintainAspect"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/LayerDefinition/VectorLayerDefinition/VectorScaleRange/PointTypeStyle/PointRule/PointSymbolization2D/Block/MaintainAspect"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/LayerDefinition/VectorLayerDefinition/VectorScaleRange/CompositeTypeStyle/ShowInLegend"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/LayerDefinition/GridLayerDefinition/Opacity"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/LayerDefinition/GridLayerDefinition/GridScaleRange/ColorStyle/ColorRule/Label/AdvancedPlacement/ScaleLimit"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/LayerDefinition/GridLayerDefinition/GridScaleRange/ColorStyle/ColorRule/Color/Bands/RedBand/LowBand"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/LayerDefinition/GridLayerDefinition/GridScaleRange/ColorStyle/ColorRule/Color/Bands/RedBand/HighBand"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/LayerDefinition/GridLayerDefinition/GridScaleRange/ColorStyle/ColorRule/Color/Bands/RedBand/LowChannel"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/LayerDefinition/GridLayerDefinition/GridScaleRange/ColorStyle/ColorRule/Color/Bands/RedBand/HighChannel"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/LayerDefinition/GridLayerDefinition/GridScaleRange/ColorStyle/ColorRule/Color/Bands/GreenBand/LowBand"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/LayerDefinition/GridLayerDefinition/GridScaleRange/ColorStyle/ColorRule/Color/Bands/GreenBand/HighBand"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/LayerDefinition/GridLayerDefinition/GridScaleRange/ColorStyle/ColorRule/Color/Bands/GreenBand/LowChannel"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/LayerDefinition/GridLayerDefinition/GridScaleRange/ColorStyle/ColorRule/Color/Bands/GreenBand/HighChannel"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/LayerDefinition/GridLayerDefinition/GridScaleRange/ColorStyle/ColorRule/Color/Bands/BlueBand/LowBand"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/LayerDefinition/GridLayerDefinition/GridScaleRange/ColorStyle/ColorRule/Color/Bands/BlueBand/HighBand"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/LayerDefinition/GridLayerDefinition/GridScaleRange/ColorStyle/ColorRule/Color/Bands/BlueBand/LowChannel"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/LayerDefinition/GridLayerDefinition/GridScaleRange/ColorStyle/ColorRule/Color/Bands/BlueBand/HighChannel"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/LayerDefinition/GridLayerDefinition/GridScaleRange/ColorStyle/ColorRule/HillShade/Azimuth"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/LayerDefinition/GridLayerDefinition/GridScaleRange/ColorStyle/ColorRule/HillShade/Altitude"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/LayerDefinition/GridLayerDefinition/GridScaleRange/ColorStyle/ColorRule/HillShade/ScaleFactor"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/LayerDefinition/GridLayerDefinition/GridScaleRange/ColorStyle/ColorRule/BrightnessFactor"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/LayerDefinition/GridLayerDefinition/GridScaleRange/ColorStyle/ColorRule/ContrastFactor"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/LayerDefinition/GridLayerDefinition/GridScaleRange/SurfaceStyle/ZeroValue"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/LayerDefinition/GridLayerDefinition/GridScaleRange/SurfaceStyle/ScaleFactor"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/LayerDefinition/GridLayerDefinition/GridScaleRange/MinScale"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/LayerDefinition/GridLayerDefinition/GridScaleRange/MaxScale"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/LayerDefinition/GridLayerDefinition/GridScaleRange/RebuildFactor"] = XML_DATA_TYPE_NUM_DOUBLE;
    //LoadProcedure-2.2.0.xsd
    s_elementPathTypeMap["/LoadProcedure/RasterLoadProcedure/GeoReferenceOverride/LocationX"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/LoadProcedure/RasterLoadProcedure/GeoReferenceOverride/LocationY"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/LoadProcedure/RasterLoadProcedure/GeoReferenceOverride/ScaleX"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/LoadProcedure/RasterLoadProcedure/GeoReferenceOverride/ScaleY"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/LoadProcedure/RasterLoadProcedure/SubsampleFactor"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/LoadProcedure/RasterLoadProcedure/GenerateSpatialDataSources"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/LoadProcedure/RasterLoadProcedure/GenerateLayers"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/LoadProcedure/RasterLoadProcedure/GenerateMaps"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/LoadProcedure/RasterLoadProcedure/GenerateSymbolLibraries"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/LoadProcedure/DwgLoadProcedure/Generalization"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/LoadProcedure/DwgLoadProcedure/ClosedPolylinesToPolygons"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/LoadProcedure/DwgLoadProcedure/GenerateSpatialDataSources"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/LoadProcedure/DwgLoadProcedure/GenerateLayers"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/LoadProcedure/DwgLoadProcedure/GenerateMaps"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/LoadProcedure/DwgLoadProcedure/GenerateSymbolLibraries"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/LoadProcedure/DwgLoadProcedure/LayerComponents/LayerComponent/Selected"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/LoadProcedure/SdfLoadProcedure/Generalization"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/LoadProcedure/SdfLoadProcedure/GenerateSpatialDataSources"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/LoadProcedure/SdfLoadProcedure/GenerateLayers"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/LoadProcedure/SdfLoadProcedure/GenerateMaps"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/LoadProcedure/SdfLoadProcedure/GenerateSymbolLibraries"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/LoadProcedure/SQLiteLoadProcedure/Generalization"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/LoadProcedure/SQLiteLoadProcedure/GenerateSpatialDataSources"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/LoadProcedure/SQLiteLoadProcedure/GenerateLayers"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/LoadProcedure/SQLiteLoadProcedure/GenerateMaps"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/LoadProcedure/SQLiteLoadProcedure/GenerateSymbolLibraries"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/LoadProcedure/ShpLoadProcedure/Generalization"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/LoadProcedure/ShpLoadProcedure/GenerateSpatialDataSources"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/LoadProcedure/ShpLoadProcedure/GenerateLayers"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/LoadProcedure/ShpLoadProcedure/GenerateMaps"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/LoadProcedure/ShpLoadProcedure/GenerateSymbolLibraries"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/LoadProcedure/ShpLoadProcedure/ConvertToSdf"] = XML_DATA_TYPE_BOOLEAN;
    //MapDefinition-2.4.0.xsd
    s_elementPathTypeMap["/MapDefinition/Extents/MinX"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/MapDefinition/Extents/MinY"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/MapDefinition/Extents/MaxX"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/MapDefinition/Extents/MaxY"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/MapDefinition/BaseMapDefinition/FiniteDisplayScale"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/MapDefinition/MapLayer/Selectable"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/MapDefinition/MapLayer/ShowInLegend"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/MapDefinition/MapLayer/ExpandInLegend"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/MapDefinition/MapLayer/Visible"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/MapDefinition/MapLayerGroup/ShowInLegend"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/MapDefinition/MapLayerGroup/ExpandInLegend"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/MapDefinition/MapLayerGroup/Visible"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/MapDefinition/BaseMapDefinition/BaseMapLayerGroup/ShowInLegend"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/MapDefinition/BaseMapDefinition/BaseMapLayerGroup/ExpandInLegend"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/MapDefinition/BaseMapDefinition/BaseMapLayerGroup/Visible"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/MapDefinition/BaseMapDefinition/BaseMapLayerGroup/BaseMapLayer/ShowInLegend"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/MapDefinition/BaseMapDefinition/BaseMapLayerGroup/BaseMapLayer/ExpandInLegend"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/MapDefinition/BaseMapDefinition/BaseMapLayerGroup/BaseMapLayer/Visible"] = XML_DATA_TYPE_BOOLEAN;
    //PrintLayout-1.0.0.xsd
    s_elementPathTypeMap["/PrintLayout/PageProperties/BackgroundColor/Red"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/PrintLayout/PageProperties/BackgroundColor/Blue"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/PrintLayout/PageProperties/BackgroundColor/Green"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/PrintLayout/LayoutProperties/ShowTitle"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/PrintLayout/LayoutProperties/ShowLegend"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/PrintLayout/LayoutProperties/ShowScaleBar"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/PrintLayout/LayoutProperties/ShowNorthArrow"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/PrintLayout/LayoutProperties/ShowURL"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/PrintLayout/LayoutProperties/ShowDateTime"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/PrintLayout/LayoutProperties/ShowCustomLogos"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/PrintLayout/LayoutProperties/ShowCustomText"] = XML_DATA_TYPE_BOOLEAN;
    //ProfileResult-2.4.0.xsd
    s_elementPathTypeMap["/ProfileResult/ProfileRenderMap/LayerCount"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/ProfileResult/ProfileRenderMap/Scale"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/ProfileResult/ProfileRenderMap/RenderTime"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/ProfileResult/ProfileRenderMap/CreateImageTime"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/ProfileResult/ProfileRenderMap/Extents/MinX"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/ProfileResult/ProfileRenderMap/Extents/MinY"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/ProfileResult/ProfileRenderMap/Extents/MaxX"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/ProfileResult/ProfileRenderMap/Extents/MaxY"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/ProfileResult/ProfileRenderDynamicOverlay/LayerCount"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/ProfileResult/ProfileRenderDynamicOverlay/Scale"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/ProfileResult/ProfileRenderDynamicOverlay/RenderTime"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/ProfileResult/ProfileRenderDynamicOverlay/CreateImageTime"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/ProfileResult/ProfileRenderLayer/RenderTime"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/ProfileResult/ProfileRenderLayer/ScaleRange/MinScale"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/ProfileResult/ProfileRenderLayer/ScaleRange/MaxScale"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/ProfileResult/ProfileRenderLayers/RenderTime"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/ProfileResult/ProfileRenderSelection/RenderTime"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/ProfileResult/ProfileRenderWatermark/RenderTime"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/ProfileResult/ProfileRenderWatermarks/RenderTime"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/ProfileResult/ProfileRenderLabels/RenderTime"] = XML_DATA_TYPE_NUM_DOUBLE;
    //ResourceList-1.0.0.xsd
    s_elementPathTypeMap["/ResourceList/ResourceFolder/Depth"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/ResourceList/ResourceFolder/NumberOfFolders"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/ResourceList/ResourceFolder/NumberOfDocuments"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/ResourceList/ResourceDocument/Depth"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/ResourceList/ResourceFolder/ResourceFolderHeader/Security/Inherited"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/ResourceList/ResourceDocument/ResourceDocumentHeader/Security/Inherited"] = XML_DATA_TYPE_BOOLEAN;
    //RuntimeMap-2.6.0.xsd
    s_elementPathTypeMap["/RuntimeMap/DisplayDpi"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/RuntimeMap/Group/Type"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/RuntimeMap/Group/DisplayInLegend"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/RuntimeMap/Group/ExpandInLegend"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/RuntimeMap/Group/Visible"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/RuntimeMap/Group/ActuallyVisible"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/RuntimeMap/Layer/Type"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/RuntimeMap/Layer/ScaleRange/MinScale"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/RuntimeMap/Layer/ScaleRange/MaxScale"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/RuntimeMap/Layer/ScaleRange/FeatureStyle/Type"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/RuntimeMap/Layer/Selectable"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/RuntimeMap/Layer/DisplayInLegend"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/RuntimeMap/Layer/ExpandInLegend"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/RuntimeMap/Layer/Visible"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/RuntimeMap/Layer/ActuallyVisible"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/RuntimeMap/FiniteDisplayScale"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/RuntimeMap/CoordinateSystem/MetersPerUnit"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/RuntimeMap/Extents/LowerLeftCoordinate/X"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/RuntimeMap/Extents/LowerLeftCoordinate/Y"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/RuntimeMap/Extents/UpperRightCoordinate/X"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/RuntimeMap/Extents/UpperRightCoordinate/Y"] = XML_DATA_TYPE_NUM_DOUBLE;
    //RuntimeMap-3.0.0.xsd
    s_elementPathTypeMap["/RuntimeMap/TileWidth"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/RuntimeMap/TileHeight"] = XML_DATA_TYPE_NUM_INT;
    //SiteInformation-1.0.0.xsd
    s_elementPathTypeMap["/SiteInformation/SiteServer/OperatingSystem/AvailablePhysicalMemory"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/SiteInformation/SiteServer/OperatingSystem/TotalPhysicalMemory"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/SiteInformation/SiteServer/OperatingSystem/AvailableVirtualMemory"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/SiteInformation/SiteServer/OperatingSystem/TotalVirtualMemory"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/SiteInformation/Statistics/AdminOperationsQueueCount"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/SiteInformation/Statistics/ClientOperationsQueueCount"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/SiteInformation/Statistics/SiteOperationsQueueCount"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/SiteInformation/Statistics/AverageOperationTime"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/SiteInformation/Statistics/CpuUtilization"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/SiteInformation/Statistics/TotalOperationTime"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/SiteInformation/Statistics/ActiveConnections"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/SiteInformation/Statistics/TotalConnections"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/SiteInformation/Statistics/TotalOperationsProcessed"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/SiteInformation/Statistics/TotalOperationsReceived"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/SiteInformation/Statistics/Uptime"] = XML_DATA_TYPE_NUM_INT;
    //SiteInformation-2.2.0.xsd
    s_elementPathTypeMap["/SiteInformation/Server/OperatingSystem/AvailablePhysicalMemory"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/SiteInformation/Server/OperatingSystem/TotalPhysicalMemory"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/SiteInformation/Server/OperatingSystem/AvailableVirtualMemory"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/SiteInformation/Server/OperatingSystem/TotalVirtualMemory"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/SiteInformation/Server/Statistics/AdminOperationsQueueCount"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/SiteInformation/Server/Statistics/ClientOperationsQueueCount"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/SiteInformation/Server/Statistics/SiteOperationsQueueCount"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/SiteInformation/Server/Statistics/AverageOperationTime"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/SiteInformation/Server/Statistics/CpuUtilization"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/SiteInformation/Server/Statistics/WorkingSet"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/SiteInformation/Server/Statistics/VirtualMemory"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/SiteInformation/Server/Statistics/TotalOperationTime"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/SiteInformation/Server/Statistics/ActiveConnections"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/SiteInformation/Server/Statistics/TotalConnections"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/SiteInformation/Server/Statistics/TotalOperationsProcessed"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/SiteInformation/Server/Statistics/TotalOperationsReceived"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/SiteInformation/Server/Statistics/Uptime"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/SiteInformation/Server/Statistics/CacheSize"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/SiteInformation/Server/Statistics/CacheDroppedEntries"] = XML_DATA_TYPE_NUM_INT;
    //UnmanagedDataList-1.0.0.xsd
    s_elementPathTypeMap["/UnmanagedDataList/UnmanagedDataFolder/NumberOfFolders"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/UnmanagedDataList/UnmanagedDataFolder/NumberOfFiles"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/UnmanagedDataList/UnmanagedDataFile/Size"] = XML_DATA_TYPE_NUM_INT;
    //WebLayout-2.6.0.xsd
    s_elementPathTypeMap["/WebLayout/PointSelectionBuffer"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/WebLayout/InformationPane/Width"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/WebLayout/InformationPane/LegendVisible"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/WebLayout/InformationPane/PropertiesVisible"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/WebLayout/TaskPane/Width"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/WebLayout/CommandSet/Command/MatchLimit"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/WebLayout/CommandSet/Command/DisableIfSelectionEmpty"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/WebLayout/Map/InitialView/CenterX"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/WebLayout/Map/InitialView/CenterY"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/WebLayout/Map/InitialView/Scale"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/WebLayout/EnablePingServer"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/WebLayout/ToolBar/Visible"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/WebLayout/InformationPane/Visible"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/WebLayout/ContextMenu/Visible"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/WebLayout/TaskPane/Visible"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/WebLayout/TaskPane/TaskBar/Visible"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/WebLayout/StatusBar/Visible"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/WebLayout/ZoomControl/Visible"] = XML_DATA_TYPE_BOOLEAN;
    //WatermarkDefinition-2.4.0.xsd
    s_elementPathTypeMap["/WatermarkDefinition/Position/XYPosition/XPosition/Offset"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/WatermarkDefinition/Position/XYPosition/YPosition/Offset"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/WatermarkDefinition/Position/TilePosition/TileWidth"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/WatermarkDefinition/Position/TilePosition/TileHeight"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/WatermarkDefinition/Appearance/Transparency"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/WatermarkDefinition/Appearance/Rotation"] = XML_DATA_TYPE_NUM_DOUBLE;
    //TileSetDefinition-3.0.0.xsd
    s_elementPathTypeMap["/TileSetDefinition/Extents/MinX"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/TileSetDefinition/Extents/MinY"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/TileSetDefinition/Extents/MaxX"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/TileSetDefinition/Extents/MaxY"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/TileSetDefinition/BaseMapLayerGroup/Visible"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/TileSetDefinition/BaseMapLayerGroup/ShowInLegend"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/TileSetDefinition/BaseMapLayerGroup/ExpandInLegend"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/TileSetDefinition/BaseMapLayerGroup/BaseMapLayer/Selectable"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/TileSetDefinition/BaseMapLayerGroup/BaseMapLayer/ShowInLegend"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/TileSetDefinition/BaseMapLayerGroup/BaseMapLayer/ExpandInLegend"] = XML_DATA_TYPE_BOOLEAN;
    //TileProviderList-3.0.0.xsd
    s_elementPathTypeMap["/TileProviderList/TileProvider/ConnectionProperties/ConnectionProperty/@Required"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/TileProviderList/TileProvider/ConnectionProperties/ConnectionProperty/@Protected"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/TileProviderList/TileProvider/ConnectionProperties/ConnectionProperty/@Enumerable"] = XML_DATA_TYPE_BOOLEAN;
    //FeatureSchemaCollection-3.3.0.xsd
    s_elementPathTypeMap["/FeatureSchemaCollection/FeatureSchema/Classes/ClassDefinition/IsAbstract"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/FeatureSchemaCollection/FeatureSchema/Classes/ClassDefinition/IsComputed"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/FeatureSchemaCollection/FeatureSchema/Classes/ClassDefinition/Properties/Property/PropertyType"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/FeatureSchemaCollection/FeatureSchema/Classes/ClassDefinition/Properties/Property/DataType"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/FeatureSchemaCollection/FeatureSchema/Classes/ClassDefinition/Properties/Property/Length"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/FeatureSchemaCollection/FeatureSchema/Classes/ClassDefinition/Properties/Property/Nullable"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/FeatureSchemaCollection/FeatureSchema/Classes/ClassDefinition/Properties/Property/ReadOnly"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/FeatureSchemaCollection/FeatureSchema/Classes/ClassDefinition/Properties/Property/IsAutoGenerated"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/FeatureSchemaCollection/FeatureSchema/Classes/ClassDefinition/Properties/Property/IsIdentity"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/FeatureSchemaCollection/FeatureSchema/Classes/ClassDefinition/Properties/Property/Precision"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/FeatureSchemaCollection/FeatureSchema/Classes/ClassDefinition/Properties/Property/Scale"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/FeatureSchemaCollection/FeatureSchema/Classes/ClassDefinition/Properties/Property/GeometryTypes"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/FeatureSchemaCollection/FeatureSchema/Classes/ClassDefinition/Properties/Property/SpecificGeometryTypes/Type"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/FeatureSchemaCollection/FeatureSchema/Classes/ClassDefinition/Properties/Property/HasElevation"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/FeatureSchemaCollection/FeatureSchema/Classes/ClassDefinition/Properties/Property/HasMeasure"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/FeatureSchemaCollection/FeatureSchema/Classes/ClassDefinition/Properties/Property/DefaultImageXSize"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/FeatureSchemaCollection/FeatureSchema/Classes/ClassDefinition/Properties/Property/DefaultImageYSize"] = XML_DATA_TYPE_NUM_INT;
    //ClassDefinition-3.3.0.xsd
    s_elementPathTypeMap["/ClassDefinition/IsAbstract"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/ClassDefinition/IsComputed"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/ClassDefinition/Properties/Property/PropertyType"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/ClassDefinition/Properties/Property/DataType"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/ClassDefinition/Properties/Property/Length"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/ClassDefinition/Properties/Property/Nullable"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/ClassDefinition/Properties/Property/ReadOnly"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/ClassDefinition/Properties/Property/IsAutoGenerated"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/ClassDefinition/Properties/Property/IsIdentity"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/ClassDefinition/Properties/Property/Precision"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/ClassDefinition/Properties/Property/Scale"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/ClassDefinition/Properties/Property/GeometryTypes"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/ClassDefinition/Properties/Property/SpecificGeometryTypes/Type"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/ClassDefinition/Properties/Property/HasElevation"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/ClassDefinition/Properties/Property/HasMeasure"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/ClassDefinition/Properties/Property/DefaultImageXSize"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/ClassDefinition/Properties/Property/DefaultImageYSize"] = XML_DATA_TYPE_NUM_INT;
    //UnitOfMeasure-3.3.0.xsd
    s_elementPathTypeMap["/UnitOfMeasure/Value"] = XML_DATA_TYPE_NUM_DOUBLE;
    //GeometryInfo-3.3.0.xsd
    s_elementPathTypeMap["/GeometryInfo/Area"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/GeometryInfo/Dimension"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/GeometryInfo/Length"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/GeometryInfo/IsClosed"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/GeometryInfo/IsEmpty"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/GeometryInfo/IsSimple"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/GeometryInfo/IsValid"] = XML_DATA_TYPE_BOOLEAN;
    s_elementPathTypeMap["/GeometryInfo/Envelope/LowerLeft/X"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/GeometryInfo/Envelope/LowerLeft/Y"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/GeometryInfo/Envelope/UpperRight/X"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/GeometryInfo/Envelope/UpperRight/Y"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/GeometryInfo/Centroid/X"] = XML_DATA_TYPE_NUM_DOUBLE;
    s_elementPathTypeMap["/GeometryInfo/Centroid/Y"] = XML_DATA_TYPE_NUM_DOUBLE;
    //Miscellaneous MapGuide response types that don't have a formal schema
    s_elementPathTypeMap["/SessionTimeout/Value"] = XML_DATA_TYPE_NUM_INT;
    s_elementPathTypeMap["/FeatureInformation/SelectedFeatures/SelectedLayer/LayerMetadata/Property/Type"] = XML_DATA_TYPE_NUM_INT;

    // ====================================================================================================================
    // Element paths below basically correspond to XML Schema elements that have minOccurs="1|0" and maxOccurs="unbounded"
    // ====================================================================================================================

    //FeatureSource-1.0.0.xsd
    s_multiElementPaths.insert("/FeatureSource/Parameter");
    s_multiElementPaths.insert("/FeatureSource/SupplementalSpatialContextInfo");
    s_multiElementPaths.insert("/FeatureSource/Extension");
    s_multiElementPaths.insert("/FeatureSource/Extension/CalculatedProperty");
    s_multiElementPaths.insert("/FeatureSource/Extension/AttributeRelate");
    s_multiElementPaths.insert("/FeatureSource/Extension/AttributeRelate/RelateProperty");
    //DrawingSource-1.0.0.xsd
    s_multiElementPaths.insert("/DrawingSource/Sheet");
    //LayerDefinition-2.4.0.xsd (schema has been additive, so this includes older versions as well)
    s_multiElementPaths.insert("/LayerDefinition/VectorLayerDefinition/PropertyMapping");
    s_multiElementPaths.insert("/LayerDefinition/VectorLayerDefinition/VectorScaleRange");
    s_multiElementPaths.insert("/LayerDefinition/VectorLayerDefinition/VectorScaleRange/AreaTypeStyle/AreaRule");
    s_multiElementPaths.insert("/LayerDefinition/VectorLayerDefinition/VectorScaleRange/LineTypeStyle/LineRule");
    s_multiElementPaths.insert("/LayerDefinition/VectorLayerDefinition/VectorScaleRange/LineTypeStyle/LineRule/LineSymbolization2D");
    s_multiElementPaths.insert("/LayerDefinition/VectorLayerDefinition/VectorScaleRange/PointTypeStyle/PointRule");
    s_multiElementPaths.insert("/LayerDefinition/VectorLayerDefinition/VectorScaleRange/CompositeTypeStyle/CompositeRule");
    s_multiElementPaths.insert("/LayerDefinition/VectorLayerDefinition/VectorScaleRange/CompositeTypeStyle/CompositeRule/CompositeSymbolization/SymbolInstance");
    s_multiElementPaths.insert("/LayerDefinition/VectorLayerDefinition/VectorScaleRange/CompositeTypeStyle/CompositeRule/CompositeSymbolization/SymbolInstance/ParameterOverrides/Override");
    s_multiElementPaths.insert("/LayerDefinition/GridLayerDefinition/GridScaleRange");
    s_multiElementPaths.insert("/LayerDefinition/GridLayerDefinition/GridScaleRange/ColorStyle/ColorRule");
    //SymbolDefinition-2.4.0.xsd (schema has been additive, so this includes older versions as well)
    s_multiElementPaths.insert("/SimpleSymbolDefinition/ParameterDefinition/Parameter");
    s_multiElementPaths.insert("/CompoundSymbolDefinition/SimpleSymbol");
    s_multiElementPaths.insert("/SimpleSymbolDefinition/Graphics/Path");
    s_multiElementPaths.insert("/SimpleSymbolDefinition/Graphics/Image");
    s_multiElementPaths.insert("/SimpleSymbolDefinition/Graphics/Text");
    //MapDefinition-2.4.0.xsd (schema has been additive, so this includes older versions as well)
    s_multiElementPaths.insert("/MapDefinition/MapLayer");
    s_multiElementPaths.insert("/MapDefinition/MapLayerGroup");
    s_multiElementPaths.insert("/MapDefinition/BaseMapDefinition/BaseLayerGroup");
    s_multiElementPaths.insert("/MapDefinition/BaseMapDefinition/BaseLayerGroup/BaseMapLayer");
    s_multiElementPaths.insert("/MapDefinition/Watermarks/Watermark");
    //WebLayout-2.6.0.xsd (schema has been additive, so this includes older versions as well)
    s_multiElementPaths.insert("/WebLayout/ToolBar/Button");
    s_multiElementPaths.insert("/WebLayout/ToolBar/Button/SubItem");
    s_multiElementPaths.insert("/WebLayout/ContextMenu/MenuItem");
    s_multiElementPaths.insert("/WebLayout/ContextMenu/MenuItem/SubItem");
    s_multiElementPaths.insert("/WebLayout/TaskPane/TaskBar/MenuButton");
    s_multiElementPaths.insert("/WebLayout/CommandSet/Command");
    //LoadProcedure-2.2.0.xsd (schema has been additive, so this includes older versions as well)
    s_multiElementPaths.insert("/LoadProcedure/SdfLoadProcedure/SourceFile");
    s_multiElementPaths.insert("/LoadProcedure/SdfLoadProcedure/ResourceId");
    s_multiElementPaths.insert("/LoadProcedure/DwfLoadProcedure/SourceFile");
    s_multiElementPaths.insert("/LoadProcedure/DwfLoadProcedure/ResourceId");
    s_multiElementPaths.insert("/LoadProcedure/ShpLoadProcedure/SourceFile");
    s_multiElementPaths.insert("/LoadProcedure/ShpLoadProcedure/ResourceId");
    s_multiElementPaths.insert("/LoadProcedure/DwgLoadProcedure/SourceFile");
    s_multiElementPaths.insert("/LoadProcedure/DwgLoadProcedure/ResourceId");
    s_multiElementPaths.insert("/LoadProcedure/DwgLoadProcedure/FileComponents/FileComponent");
    s_multiElementPaths.insert("/LoadProcedure/DwgLoadProcedure/LayerComponents/LayerComponent");
    s_multiElementPaths.insert("/LoadProcedure/RasterLoadProcedure/SourceFile");
    s_multiElementPaths.insert("/LoadProcedure/RasterLoadProcedure/ResourceId");
    s_multiElementPaths.insert("/LoadProcedure/RasterLoadProcedure/GeoReferenceOverride");
    s_multiElementPaths.insert("/LoadProcedure/SQLiteLoadProcedure/SourceFile");
    s_multiElementPaths.insert("/LoadProcedure/SQLiteLoadProcedure/ResourceId");
    //PrintLayout-1.0.0.xsd
    s_multiElementPaths.insert("/PrintLayout/CustomLogos/Logo");
    s_multiElementPaths.insert("/PrintLayout/CustomText/Text");
    //ApplicationDefinition-1.0.0.xsd
    s_multiElementPaths.insert("/ApplicationDefinition/WidgetSet");
    s_multiElementPaths.insert("/ApplicationDefinition/WidgetSet/Container");
    s_multiElementPaths.insert("/ApplicationDefinition/WidgetSet/Container/Item");
    s_multiElementPaths.insert("/ApplicationDefinition/WidgetSet/Container/Item/Item");
    s_multiElementPaths.insert("/ApplicationDefinition/MapSet/MapGroup");
    s_multiElementPaths.insert("/ApplicationDefinition/MapSet/MapGroup/Map");
    //ApplicationDefinitionInfo-1.0.0.xsd
    s_multiElementPaths.insert("/ApplicationDefinitionWidgetInfoSet/WidgetInfo");
    s_multiElementPaths.insert("/ApplicationDefinitionWidgetInfoSet/WidgetInfo/ContainableBy");
    s_multiElementPaths.insert("/ApplicationDefinitionWidgetInfoSet/WidgetInfo/Parameter");
    s_multiElementPaths.insert("/ApplicationDefinitionWidgetInfoSet/WidgetInfo/Parameter/AllowedValue");
    s_multiElementPaths.insert("/ApplicationDefinitionContainerInfoSet/ContainerInfo");
    s_multiElementPaths.insert("/ApplicationDefinitionTemplateInfoSet/TemplateInfo");
    s_multiElementPaths.insert("/ApplicationDefinitionTemplateInfoSet/TemplateInfo/Panel");
    //BatchPropertyCollection-1.0.0.xsd
    s_multiElementPaths.insert("/BatchPropertyCollection/PropertyCollection");
    s_multiElementPaths.insert("/BatchPropertyCollection/PropertyCollection/Property");
    //ClassDefinition-3.3.0.xsd
    s_multiElementPaths.insert("/ClassDefinition/Properties/Property");
    s_multiElementPaths.insert("/ClassDefinition/Properties/Property/SpecificGeometryTypes/Type");
    //DataStoreList-1.0.0.xsd
    s_multiElementPaths.insert("/DataStoreList/DataStore");
    //DrawingSectionList-1.0.0.xsd
    s_multiElementPaths.insert("/DrawingSectionList/Section");
    //DrawingSectionResourceList-1.0.0.xsd
    s_multiElementPaths.insert("/DrawingSectionResourceList/SectionResource");
    //FdoLongTransactionList-1.0.0.xsd
    s_multiElementPaths.insert("/FdoLongTransactionList/LongTransaction");
    //FdoProviderCapabilities-1.0.0.xsd
    s_multiElementPaths.insert("/FeatureProviderCapabilities/Connection/SpatialContextExtent/Type");
    s_multiElementPaths.insert("/FeatureProviderCapabilities/Schema/Class/Type");
    s_multiElementPaths.insert("/FeatureProviderCapabilities/Schema/Data/Type");
    s_multiElementPaths.insert("/FeatureProviderCapabilities/Command/SupportedCommands/Type");
    s_multiElementPaths.insert("/FeatureProviderCapabilities/Filter/Condition/Type");
    s_multiElementPaths.insert("/FeatureProviderCapabilities/Filter/Spatial/Operation");
    s_multiElementPaths.insert("/FeatureProviderCapabilities/Filter/Distance/Operation");
    s_multiElementPaths.insert("/FeatureProviderCapabilities/Expression/Type/Name");
    s_multiElementPaths.insert("/FeatureProviderCapabilities/Expression/FunctionDefinitionList/FunctionDefinition");
    s_multiElementPaths.insert("/FeatureProviderCapabilities/Expression/FunctionDefinitionList/FunctionDefinition/ArgumentDefinitionList/ArgumentDefinition");
    s_multiElementPaths.insert("/FeatureProviderCapabilities/Geometry/Types/Type");
    s_multiElementPaths.insert("/FeatureProviderCapabilities/Geometry/Components/Type");
    //FdoProviderCapabilities-1.1.0.xsd
    s_multiElementPaths.insert("/FeatureProviderCapabilities/Schema/SupportedAutoGeneratedTypes");
    s_multiElementPaths.insert("/FeatureProviderCapabilities/Expression/FunctionDefinitionList/FunctionDefinition/SignatureDefinitionCollection/SignatureDefinition");
    s_multiElementPaths.insert("/FeatureProviderCapabilities/Expression/FunctionDefinitionList/FunctionDefinition/SignatureDefinitionCollection/SignatureDefinition/ArgumentDefinitionList/ArgumentDefinition");
    s_multiElementPaths.insert("/FeatureProviderCapabilities/Expression/FunctionDefinitionList/FunctionDefinition/SignatureDefinitionCollection/SignatureDefinition/ArgumentDefinitionList/ArgumentDefinition/PropertyValueConstraintList/Value");
    //FdoSpatialContextList-1.0.0.xsd
    s_multiElementPaths.insert("/FdoSpatialContextList/SpatialContext");
    //FeatureProviderRegistry-1.0.0.xsd
    s_multiElementPaths.insert("/FeatureProviderRegistry/FeatureProvider");
    s_multiElementPaths.insert("/FeatureProviderRegistry/FeatureProvider/ConnectionProperties/ConnectionProperty");
    s_multiElementPaths.insert("/FeatureProviderRegistry/FeatureProvider/ConnectionProperties/ConnectionProperty/Value");
    //FeatureSchemaCollection-3.3.0.xsd
    s_multiElementPaths.insert("/FeatureSchemaCollection/FeatureSchema");
    s_multiElementPaths.insert("/FeatureSchemaCollection/FeatureSchema/Classes/ClassDefinition");
    s_multiElementPaths.insert("/FeatureSchemaCollection/FeatureSchema/Classes/ClassDefinition/Properties/Property");
    s_multiElementPaths.insert("/FeatureSchemaCollection/FeatureSchema/Classes/ClassDefinition/Properties/Property/SpecificGeometryTypes/Type");
    //Group-1.0.0.xsd
    s_multiElementPaths.insert("/Group/Users/User");
    //GroupList-1.0.0.xsd
    s_multiElementPaths.insert("/GroupList/Group");
    //ProfileResult-2.4.0.xsd
    s_multiElementPaths.insert("/ProfileResult/ProfileRenderMap/ProfileRenderLayers/ProfileRenderLayer");
    s_multiElementPaths.insert("/ProfileResult/ProfileRenderMap/ProfileRenderSelection/ProfileSelectedRenderLayer");
    s_multiElementPaths.insert("/ProfileResult/ProfileRenderMap/ProfileRenderWatermarks/ProfileRenderWatermark");
    //RepositoryList-1.0.0.xsd
    s_multiElementPaths.insert("/RepositoryList/Repository");
    //ResourceDataList-1.0.0.xsd
    s_multiElementPaths.insert("/ResourceDataList/ResourceData");
    //ResourceDocumentHeader-1.0.0.xsd
    s_multiElementPaths.insert("/ResourceDocumentHeader/Metadata/Simple/Property");
    //ResourceList-1.0.0.xsd
    s_multiElementPaths.insert("/ResourceList/ResourceFolder");
    s_multiElementPaths.insert("/ResourceList/ResourceFolder/ResourceFolderHeader/Security/Users/User"); //ResourceSecurity-1.0.0.xsd
    s_multiElementPaths.insert("/ResourceList/ResourceFolder/ResourceFolderHeader/Security/Groups/Group"); //ResourceSecurity-1.0.0.xsd
    s_multiElementPaths.insert("/ResourceList/ResourceDocument");
    s_multiElementPaths.insert("/ResourceList/ResourceDocument/ResourceDocumentHeader/Security/Users/User"); //ResourceSecurity-1.0.0.xsd
    s_multiElementPaths.insert("/ResourceList/ResourceDocument/ResourceDocumentHeader/Security/Groups/Group"); //ResourceSecurity-1.0.0.xsd
    s_multiElementPaths.insert("/ResourceList/ResourceDocument/ResourceDocumentHeader/Metadata/Simple/Property");
    //ResourcePackageManifest-1.0.0.xsd
    s_multiElementPaths.insert("/ResourcePackageManifest/Operations/Operation");
    s_multiElementPaths.insert("/ResourcePackageManifest/Operations/Operation/Parameters/Parameter");
    //ResourceReferenceList-1.0.0.xsd
    s_multiElementPaths.insert("/ResourceReferenceList/ResourceId");
    //RuntimeMap-2.6.0.xsd
    s_multiElementPaths.insert("/RuntimeMap/Group");
    s_multiElementPaths.insert("/RuntimeMap/Layer");
    s_multiElementPaths.insert("/RuntimeMap/Layer/ScaleRange");
    s_multiElementPaths.insert("/RuntimeMap/Layer/ScaleRange/FeatureStyle");
    s_multiElementPaths.insert("/RuntimeMap/Layer/ScaleRange/FeatureStyle/Rule");
    s_multiElementPaths.insert("/RuntimeMap/FiniteDisplayScale");
    //SelectAggregate-1.0.0.xsd
    s_multiElementPaths.insert("/PropertySet/PropertyDefinitions/PropertyDefinition");
    s_multiElementPaths.insert("/PropertySet/Properties/PropertyCollection");
    s_multiElementPaths.insert("/PropertySet/Properties/PropertyCollection/Property");
    //ServerList-1.0.0.xsd
    s_multiElementPaths.insert("/ServerList/Server");
    //SqlSelect-1.0.0.xsd
    s_multiElementPaths.insert("/RowSet/ColumnDefinitions");
    s_multiElementPaths.insert("/RowSet/ColumnDefinitions/Column");
    s_multiElementPaths.insert("/RowSet/Rows/Row");
    s_multiElementPaths.insert("/RowSet/Rows/Row/Column");
    //StringCollection-1.0.0.xsd
    s_multiElementPaths.insert("/StringCollection/Item");
    //TileSetDefinition-3.0.0.xsd
    s_multiElementPaths.insert("/TileSetDefinition/TileStoreParameters/Parameter");
    s_multiElementPaths.insert("/TileSetDefinition/BaseMapLayerGroup");
    s_multiElementPaths.insert("/TileSetDefinition/BaseMapLayerGroup/BaseMapLayer");
    //TileProviderList-1.0.0.xsd
    s_multiElementPaths.insert("/TileProviderList/TileProvider");
    s_multiElementPaths.insert("/TileProviderList/TileProvider/ConnectionProperties/ConnectionProperty");
    s_multiElementPaths.insert("/TileProviderList/TileProvider/ConnectionProperties/ConnectionProperty/Value");
    //UnmanagedDataList-1.0.0.xsd
    s_multiElementPaths.insert("/UnmanagedDataList/UnmanagedDataFolder");
    s_multiElementPaths.insert("/UnmanagedDataList/UnmanagedDataFile");
    //UserList-1.0.0.xsd
    s_multiElementPaths.insert("/UserList/User");
    s_multiElementPaths.insert("/UserList/Group");
    //Miscellaneous MapGuide response types that don't have a formal schema
    s_multiElementPaths.insert("/FeatureInformation/FeatureSet/Layer");
    s_multiElementPaths.insert("/FeatureInformation/FeatureSet/Layer/Class/ID");
    s_multiElementPaths.insert("/FeatureInformation/SelectedFeatures/SelectedLayer");
    s_multiElementPaths.insert("/FeatureInformation/SelectedFeatures/SelectedLayer/LayerMetadata/Property");
    s_multiElementPaths.insert("/FeatureInformation/SelectedFeatures/SelectedLayer/Feature");
    s_multiElementPaths.insert("/FeatureInformation/SelectedFeatures/SelectedLayer/Feature/Property");

    return true;
}
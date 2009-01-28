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

#include "Foundation.h"

#include "XmlJsonConvert.h"

#include <string>
#include <map>
#include <vector>
using namespace std;

MgXmlJsonConvert::MgXmlJsonConvert()
{
}

MgXmlJsonConvert::~MgXmlJsonConvert()
{
}

void MgXmlJsonConvert::ToJson(Ptr<MgByteReader> &byteReader)
{
    string xmlDoc;
    byteReader->ToStringUtf8(xmlDoc);

    string jsonDoc;
    this->ToJson(xmlDoc, jsonDoc);
    Ptr<MgByteSource> byteSource = new MgByteSource(
        (unsigned char*)jsonDoc.c_str(), (INT32)jsonDoc.length());
    byteSource->SetMimeType(MgMimeType::Json);
    byteReader.Attach(byteSource->GetReader());
}

void MgXmlJsonConvert::ToJson(const string &xmlString, string &jsonString)
{
    // Parse into DOM
    m_xmlUtil.ParseString(xmlString.c_str());
    DOMElement *root = m_xmlUtil.GetRootNode();
    string nodeName = MgUtil::WideCharToMultiByte(X2W(root->getNodeName()));
    m_jsonDoc.BeginObject(nodeName);
    {
        XmlToJsonNode((DOMNode *)root);
    }
    m_jsonDoc.EndObject();

    m_jsonDoc.Print(jsonString);
}

//  XmlToJSONnode:  Output an XmlElement, possibly as part of a higher array
void MgXmlJsonConvert::XmlToJsonNode(DOMNode *node)
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
        int attributeCount = attributes->getLength();
        DOMNode *attribute;
        string nodeValue;
        for (int i = 0; i < attributeCount; i++)
        {
            attribute = attributes->item(i);
            MgUtil::WideCharToMultiByte(X2W(attribute->getNodeName()), nodeName);
            MgUtil::WideCharToMultiByte(X2W(attribute->getNodeValue()), nodeValue);

            // Write the attribute as a single array element
            m_jsonDoc.BeginArray(1, "@" + nodeName);
            m_jsonDoc.SetArrayValue(0, nodeValue);
            m_jsonDoc.EndArray();
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
            this->ProcessObjectNode(childNode);
        }
        else
        {
            int nodeListLength = (int)nodeList->size();
            m_jsonDoc.BeginArray(nodeListLength, nodeName);
            {
                for (int i = 0; i < nodeListLength; i++)
                {
                    childNode = (*nodeList)[i];
                    this->ProcessArrayNode(i, childNode);
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

void MgXmlJsonConvert::ProcessObjectNode(DOMNode *node)
{
    short nodeType = node->getNodeType();
    string nodeName;
    MgUtil::WideCharToMultiByte(X2W(node->getNodeName()), nodeName);
    if (nodeType == DOMNode::TEXT_NODE)
    {
        string textContent;
        MgUtil::WideCharToMultiByte(X2W(node->getTextContent()), textContent);

        // Write the text as a single array element
        m_jsonDoc.BeginArray(1, nodeName);
        m_jsonDoc.SetArrayValue(0, textContent);
        m_jsonDoc.EndArray();
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
                    string textContent;
                    MgUtil::WideCharToMultiByte(X2W(childNode->getTextContent()), textContent);
                    if (this->ValidateTextContent(textContent))
                    {
                        // Write the text as a single array element
                        m_jsonDoc.BeginArray(1, nodeName);
                        m_jsonDoc.SetArrayValue(0, textContent);
                        m_jsonDoc.EndArray();
                    }
                    return;
                }
            }
        }
    }

    m_jsonDoc.BeginArray(1, nodeName);
    m_jsonDoc.BeginArrayObject(0);
    {
        this->XmlToJsonNode(node);
    }
    m_jsonDoc.EndArrayObject();
    m_jsonDoc.EndArray();
}

void MgXmlJsonConvert::ProcessArrayNode(int index, DOMNode *node)
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
                m_jsonDoc.SetArrayValue(index, NULL);
                return;
            }
            else if (childNodeCount == 1)
            {
                DOMNode *childNode = childNodes->item(0);
                if (childNode->getNodeType() == DOMNode::TEXT_NODE)
                {
                    string textContent;
                    MgUtil::WideCharToMultiByte(X2W(childNode->getTextContent()), textContent);
                    if (this->ValidateTextContent(textContent))
                    {
                        m_jsonDoc.SetArrayValue(index, textContent);
                    }
                    return;
                }
            }
        }
    }

    // Write the node as a single array element
    m_jsonDoc.BeginArrayObject(index);
    {
        this->XmlToJsonNode(node);
    }
    m_jsonDoc.EndArrayObject();
}

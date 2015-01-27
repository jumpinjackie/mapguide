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
#include "XmlDefs.h"
#include "XmlUtil.h"
#include "UnicodeString.h"

#include <assert.h>

static const char* sgBufId = "MgXmlUtil.Class";


// Dummy class used to automate initialization/uninitialization of Xerces.
class CInitXerces
{
public:
    CInitXerces()
    {
        XMLPlatformUtils::Initialize();
    }
    ~CInitXerces()
    {
        XMLPlatformUtils::Terminate();
    }
};

static CInitXerces s_InitXerces;


MgXmlUtil::MgXmlUtil() :
    m_parser(NULL),
    m_bis(NULL),
    m_doc(NULL),
    m_ownDomDoc(false)
{
}

MgXmlUtil::MgXmlUtil(DOMDocument* document, bool ownDomDoc) :
    m_parser(NULL),
    m_bis(NULL),
    m_doc(document),
    m_ownDomDoc(ownDomDoc)
{
    CHECKNULL(m_doc, L"MgXmlUtil.MgXmlUtil");
}

MgXmlUtil::MgXmlUtil(const string& xmlContent) :
    m_parser(NULL),
    m_bis(NULL),
    m_doc(NULL),
    m_ownDomDoc(false)
{
    ParseString(xmlContent.c_str());
}

MgXmlUtil::MgXmlUtil(const char* rootElementName) :
    m_parser(NULL),
    m_bis(NULL),
    m_doc(NULL),
    m_ownDomDoc(true)
{
    CHECKARGUMENTNULL(rootElementName, L"MgXmlUtil.MgXmlUtil");

    DOMImplementation* impl =
        DOMImplementationRegistry::getDOMImplementation(X("Core"));

    if (NULL != impl)
    {
        m_doc = impl->createDocument(0, X(rootElementName), 0);
    }
}

MgXmlUtil::~MgXmlUtil()
{
    delete m_parser;
    m_parser = NULL;

    delete m_bis;
    m_bis = NULL;

    if (NULL != m_doc)
    {
        if (m_ownDomDoc)
            m_doc->release();

        m_doc = NULL;
    }

    m_ownDomDoc = false;
}

void MgXmlUtil::ParseString(const char* xmlContent)
{
    CHECKARGUMENTNULL(xmlContent, L"MgXmlUtil.MgXmlUtil");
    assert(NULL == m_parser && NULL == m_bis);
    assert(NULL == m_doc && !m_ownDomDoc);

    MG_XML_TRY()

    xstring xStr;
    UnicodeString::UTF8toUTF16(xmlContent, xStr);
    size_t nChars = xStr.length();
    m_parser = new XercesDOMParser();
    m_bis = new MemBufInputSource((const XMLByte*)xStr.c_str(),
        (unsigned int)nChars*sizeof(XMLCh), sgBufId, false);
    m_bis->setEncoding(X("UTF-16"));
    m_parser->parse(*m_bis);
    m_doc = m_parser->getDocument();
    m_ownDomDoc = false;

    MG_XML_CATCH_AND_THROW(L"MgXmlUtil.ParseString")
}

DOMElement* MgXmlUtil::AddChildNode(DOMElement* parentNode, const char* elementName)
{
    CHECKNULL(m_doc,       L"MgXmlUtil.AddChildNode");
    CHECKARGUMENTNULL(parentNode,  L"MgXmlUtil.AddChildNode");
    CHECKARGUMENTNULL(elementName, L"MgXmlUtil.AddChildNode");

    // Add Feature Provider element
    DOMElement* childNode = m_doc->createElement(X(elementName));
    if (NULL != childNode)
        parentNode->appendChild(childNode);

    return childNode;
}

void MgXmlUtil::AddTextNode(DOMElement* parentNode, const char* elementName, bool value)
{
    if (value) // true
        AddTextNode(parentNode,elementName, "true");
    else
        AddTextNode(parentNode,elementName, "false");
}


void MgXmlUtil::AddTextNode(DOMElement* parentNode, const char* elementName, const char* elementText)
{
    CHECKNULL(m_doc,       L"MgXmlUtil.AddTextNode");
    CHECKARGUMENTNULL(parentNode, L"MgXmlUtil.AddTextNode");
    CHECKARGUMENTNULL(elementName, L"MgXmlUtil.AddTextNode");

    if (NULL != elementText)
    {
        DOMElement* elemName = m_doc->createElement(X(elementName));
        if (NULL != elemName)
            parentNode->appendChild(elemName);

        DOMText* textNode = m_doc->createTextNode(X(elementText));
        if (NULL != textNode)
            elemName->appendChild(textNode);
    }
}

void MgXmlUtil::AddTextNode(DOMElement* parentNode, const char* elementName, const wchar_t* elementText)
{
    CHECKNULL(m_doc,       L"MgXmlUtil.AddTextNode");
    CHECKARGUMENTNULL(parentNode, L"MgXmlUtil.AddTextNode");
    CHECKARGUMENTNULL(elementName, L"MgXmlUtil.AddTextNode");
    CHECKARGUMENTNULL(elementText, L"MgXmlUtil.AddTextNode");

    DOMElement* elemName = m_doc->createElement(X(elementName));
    if (NULL != elemName)
        parentNode->appendChild(elemName);

    char* mbStr = MgUtil::WideCharToMultiByte(elementText);
    CHECKNULL(mbStr,  L"MgXmlUtil.AddTextNode");

    DOMText* textNode = m_doc->createTextNode(X(mbStr));
    if (NULL != textNode)
        elemName->appendChild(textNode);

    delete[] mbStr;
}

void MgXmlUtil::UpdateTextNode(DOMNode* parentNode, const char* elementText)
{
    CHECKNULL(m_doc,       L"MgXmlUtil.UpdateTextNode");
    CHECKARGUMENTNULL(parentNode, L"MgXmlUtil.UpdateTextNode");
    CHECKARGUMENTNULL(elementText, L"MgXmlUtil.UpdateTextNode");

    DOMNode* child = parentNode->getFirstChild();

    if (NULL == child)
    {
        DOMText* textNode = m_doc->createTextNode(X(elementText));
        parentNode->appendChild(textNode);
    }
    else
    {
        short type = child->getNodeType();

        if ( DOMNode::TEXT_NODE == type )
        {
            DOMText* textNode = m_doc->createTextNode(X(elementText));
            parentNode->replaceChild(textNode, child);
        }
    }
}

void MgXmlUtil::UpdateTextNode(DOMNode* parentNode, const wchar_t* elementText)
{
    CHECKNULL(m_doc,       L"MgXmlUtil.UpdateTextNode");
    CHECKARGUMENTNULL(parentNode, L"MgXmlUtil.UpdateTextNode");
    CHECKARGUMENTNULL(elementText, L"MgXmlUtil.UpdateTextNode");

    DOMNode* child = parentNode->getFirstChild();

    if (NULL == child)
    {
        DOMText* textNode = m_doc->createTextNode(W2X(elementText));
        parentNode->appendChild(textNode);
    }
    else
    {
        short type = child->getNodeType();

        if ( DOMNode::TEXT_NODE == type )
        {
            DOMText* textNode = m_doc->createTextNode(W2X(elementText));
            parentNode->replaceChild(textNode, child);
        }
    }
}

DOMNode* MgXmlUtil::GetNodeWithTextData(DOMDocument* doc, const char* elementName,
    const wchar_t* elementText)
{
    CHECKARGUMENTNULL(doc, L"MgXmlUtil::GetNodeWithTextData");
    CHECKARGUMENTNULL(elementName, L"MgXmlUtil::GetNodeWithTextData");

    DOMNode* ret = NULL;

    DOMNodeList* list = doc->getElementsByTagName( X(elementName) );

    XMLSize_t len = list->getLength();

    for (XMLSize_t i = 0; i < len; i++)
    {
        DOMNode* node = list->item(i);
        DOMNode* child = node->getFirstChild(); //  we expect the text is the first node

        if ( DOMNode::TEXT_NODE == child->getNodeType() )
        {
            XMLCh const* tdata = ((DOMText*) child)->getData();
            if ( XMLString::equals( tdata, W2X(elementText) ) )
            {
                ret = node;
                break;      //  found it; break out of loop
            }
        }
    }

    return ret;
}

DOMDocument* MgXmlUtil::GetDocument() const
{
    return m_doc;
}

DOMElement* MgXmlUtil::GetRootNode() const
{
    CHECKNULL(m_doc, L"MgXmlUtil.GetRootNode");
    return m_doc->getDocumentElement();
}

DOMNodeList* MgXmlUtil::GetNodeList(const char* elementName) const
{
    CHECKNULL(m_doc, L"MgXmlUtil.GetNodeList");
    return m_doc->getElementsByTagName(X(elementName));
}

///----------------------------------------------------------------------------
/// <summary>
/// Returns a DOMNodeList of all descendant elements with a given tag name,
/// in the order in which they would be encountered in a preorder traversal of
/// the DOMElement or DOMDocument tree.
/// </summary>
///
/// <param name="parentNode">
/// Parent node containing a list of nodes of descendant elements.
/// </param>
/// <param name="elementName">
/// Name of descendant elements.
/// </param>
/// <param name="required">
/// Flag specifying whether or not the node list is required (or optional).
/// </param>
///
/// <exceptions>
/// MgXmlParserException
/// MgNullArgumentException
/// MgInvalidArgumentException
/// </exceptions>
///----------------------------------------------------------------------------

DOMNodeList* MgXmlUtil::GetNodeList(DOMNode* parentNode,
    const char* elementName) const
{
    DOMNodeList* nodeList = NULL;

    MG_XML_TRY()

    CHECKARGUMENTNULL(parentNode, L"MgXmlUtil.GetNodeList");
    CHECKARGUMENTNULL(elementName, L"MgXmlUtil.GetNodeList");
    if (0 == ::strlen(elementName))
    {
        throw new MgNullArgumentException(L"MgXmlUtil.GetNodeList",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    short nodeType = parentNode->getNodeType();

    if (DOMNode::ELEMENT_NODE == nodeType)
    {
        nodeList = (reinterpret_cast<DOMElement*>(parentNode))->
            getElementsByTagName(X(elementName));
    }
    else if (DOMNode::DOCUMENT_NODE == nodeType)
    {
        nodeList = (reinterpret_cast<DOMDocument*>(parentNode))->
            getElementsByTagName(X(elementName));
    }
    else
    {
        STRING buffer;
        MgUtil::Int32ToString((INT32)nodeType, buffer);

        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(buffer);

        throw new MgInvalidArgumentException(L"MgXmlUtil.GetNodeList",
            __LINE__, __WFILE__, &arguments, L"MgInvalidNode", NULL);
    }

    MG_XML_CATCH_AND_THROW(L"MgXmlUtil.GetNodeList")

    return nodeList;
}

void MgXmlUtil::SetAttribute(DOMElement* node, const char* attributeName, const wchar_t* value)
{
    CHECKARGUMENTNULL(node, L"MgXmlUtil.SetAttribute");

    char* mbStr = MgUtil::WideCharToMultiByte(value);
    CHECKNULL(mbStr,  L"MgXmlUtil.SetAttribute");

    // set attribute
    node->setAttribute(X(attributeName), X(mbStr));

    delete[] mbStr;
}

///----------------------------------------------------------------------------
/// <summary>
/// Gets a node (as a string) of the specified element.
/// Assuming that the document contains only one unique node of the specified
/// element.
/// </summary>
///
/// <param name="elementName">
/// Name of the element whose node is to be retrieved.
/// </param>
/// <param name="document">
/// Document containing the specified element.
/// </param>
/// <param name="node">
/// Node returned as a string.
/// </param>
/// <param name="required">
/// Flag specifying whether or not the element node is required (or optional).
/// </param>
///
/// <exceptions>
/// MgXmlParserException
/// MgNullArgumentException
/// MgInvalidArgumentException
/// </exceptions>
///----------------------------------------------------------------------------

void MgXmlUtil::GetElementNode(const char* elementName, const string& document,
    string& node, bool required)
{
    MG_XML_TRY()

    CHECKARGUMENTNULL(elementName, L"MgXmlUtil.GetElementNode");
    if (0 == ::strlen(elementName))
    {
        throw new MgNullArgumentException(L"MgXmlUtil.GetElementNode",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    string beginTag = "<";
    beginTag += elementName;
    beginTag += ">";

    string endTag = "</";
    endTag += elementName;
    endTag += ">";

    size_t pos1 = document.find(beginTag);
    size_t pos2 = document.find(endTag);

    if (string::npos != pos1 && string::npos != pos2 && pos2 > pos1)
    {
        node = document.substr(pos1, pos2 - pos1 + endTag.length());
    }
    else if (required)
    {
        throw new MgXmlParserException(L"MgXmlUtil.GetElementNode",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }
    else
    {
        node.clear();
    }

    MG_XML_CATCH_AND_THROW(L"MgXmlUtil.GetElementNode")
}

///----------------------------------------------------------------------------
/// <summary>
/// Gets a node of the specified element.
/// </summary>
///
/// <param name="parentNode">
/// Parent node containing the specified element.
/// </param>
/// <param name="elementName">
/// Name of the element whose node is to be retrieved.
/// </param>
/// <param name="required">
/// Flag specifying whether or not the element node is required (or optional).
/// </param>
///
/// <exceptions>
/// MgXmlParserException
/// MgNullArgumentException
/// MgInvalidArgumentException
/// </exceptions>
///----------------------------------------------------------------------------

DOMElement* MgXmlUtil::GetElementNode(DOMNode* parentNode,
    const char* elementName, bool required)
{
    DOMElement* elementNode = NULL;

    MG_XML_TRY()

    DOMNodeList* nodeList = GetNodeList(parentNode, elementName);

    if (NULL != nodeList && nodeList->getLength() > 0)
    {
        DOMNode* item = nodeList->item(0);
        short nodeType = item->getNodeType();

        if (DOMNode::ELEMENT_NODE == nodeType)
        {
            elementNode = reinterpret_cast<DOMElement*>(item);
        }
    }

    if (NULL == elementNode && required)
    {
        throw new MgXmlParserException(L"MgXmlUtil.GetElementNode",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_XML_CATCH_AND_THROW(L"MgXmlUtil.GetElementNode")

    return elementNode;
}

///----------------------------------------------------------------------------
/// <summary>
/// Gets the value of the specified element.
/// </summary>
///
/// <param name="parentNode">
/// Parent node containing the specified element.
/// </param>
/// <param name="elementName">
/// Name of the element whose value is to be retrieved.
/// </param>
/// <param name="elementValue">
/// Value (as a string) of the specified element.
/// </param>
/// <param name="required">
/// Flag specifying whether or not the element node is required (or optional).
/// </param>
///
/// <exceptions>
/// MgXmlParserException
/// MgNullArgumentException
/// MgInvalidArgumentException
/// </exceptions>
///----------------------------------------------------------------------------

void MgXmlUtil::GetElementValue(DOMNode* parentNode,
    const char* elementName, wstring& elementValue, bool required)
{
    MG_XML_TRY()

    DOMNode* textNode = NULL;
    DOMElement* elementNode = GetElementNode(parentNode, elementName, required);

    if (NULL != elementNode)
    {
        textNode = elementNode->getFirstChild();
    }

    if (NULL == textNode)
    {
        if (required)
        {
            throw new MgXmlParserException(L"MgXmlUtil.GetElementValue",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }

        elementValue = L"";
    }
    else
    {
        assert(DOMNode::TEXT_NODE == textNode->getNodeType());
        elementValue = X2W(textNode->getNodeValue());
    }

    MG_XML_CATCH_AND_THROW(L"MgXmlUtil.GetElementValue")
}

///----------------------------------------------------------------------------
/// <summary>
/// Gets the value of the specified element.
/// </summary>
///
/// <param name="parentNode">
/// Parent node containing the specified element.
/// </param>
/// <param name="elementName">
/// Name of the element whose value is to be retrieved.
/// </param>
/// <param name="elementValue">
/// Value (as a boolean) of the specified element.
/// </param>
/// <param name="required">
/// Flag specifying whether or not the element node is required (or optional).
/// </param>
///
/// <exceptions>
/// MgXmlParserException
/// MgNullArgumentException
/// MgInvalidArgumentException
/// </exceptions>
///----------------------------------------------------------------------------

void MgXmlUtil::GetElementValue(DOMNode* parentNode,
    const char* elementName, bool& elementValue, bool required)
{
    MG_XML_TRY()

    wstring strValue;

    GetElementValue(parentNode, elementName, strValue, required);

    if (string::npos != strValue.find(L"true"))
    {
        elementValue = true;
    }
    else if (string::npos != strValue.find(L"false"))
    {
        elementValue = false;
    }
    else
    {
        throw new MgXmlParserException(L"MgXmlUtil.GetElementValue",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_XML_CATCH_AND_THROW(L"MgXmlUtil.GetElementValue")
}

///----------------------------------------------------------------------------
/// <summary>
/// Sets the value of the specified element.
/// </summary>
///
/// <param name="parentNode">
/// Parent node containing the specified element.
/// </param>
/// <param name="elementName">
/// Name of the element whose value is to be updated.
/// </param>
/// <param name="elementValue">
/// Value of the specified element.
/// </param>
///
/// <returns>
/// true if update is needed because the specified element value is different
/// from the existing one, and false otherwise.
/// </returns>
///
/// <exceptions>
/// MgXmlParserException
/// MgNullArgumentException
/// MgInvalidArgumentException
/// </exceptions>
///----------------------------------------------------------------------------

bool MgXmlUtil::SetElementValue(DOMNode* parentNode,
    const char* elementName, const wstring& elementValue)
{
    bool changed = false;

    MG_XML_TRY()

    DOMNode* textNode = NULL;
    DOMElement* elementNode = GetElementNode(parentNode, elementName, true);

    if (NULL != elementNode)
    {
        textNode = elementNode->getFirstChild();
    }

    if (NULL == textNode)
    {
        throw new MgXmlParserException(L"MgXmlUtil.SetElementValue",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }
    else if (elementValue != X2W(textNode->getNodeValue()))
    {
        textNode->setNodeValue(W2X(elementValue.c_str()));
        changed = true;
    }

    assert(DOMNode::TEXT_NODE == textNode->getNodeType());

    MG_XML_CATCH_AND_THROW(L"MgXmlUtil.SetElementValue")

    return changed;
}

DOMNode* MgXmlUtil::GetFirstChild(DOMNode* parent)
{
    return parent->getFirstChild();
}

DOMNode* MgXmlUtil::GetNextSibling(DOMNode* sibling)
{
    return sibling->getNextSibling();
}

wstring MgXmlUtil::GetTagName(DOMElement* element)
{
    const XMLCh* name = element->getTagName();
    wstring str;
    str = X2W(name);
    return str;
}

wstring MgXmlUtil::GetNodeValue(DOMNode* node)
{
    const XMLCh* name = node->getNodeValue();
    wstring str;
    str = X2W(name);
    return str;
}

short MgXmlUtil::GetNodeType(DOMNode* node)
{
    return node->getNodeType();
}

wstring MgXmlUtil::GetAttribute(DOMElement* element, const char* name)
{
    const XMLCh* strval = element->getAttribute(X(name));
    if(strval == NULL)
        return wstring(L"");
    wstring str;
    str = X2W(strval);
    return str;
}

void MgXmlUtil::GetTextFromElement(DOMElement* element, REFSTRING str)
{
    MG_XML_TRY()

    DOMNode* child = MgXmlUtil::GetFirstChild(element);
    while(0 != child)
    {
        if(MgXmlUtil::GetNodeType(child) == DOMNode::TEXT_NODE)
        {
            str = MgUtil::Trim(MgXmlUtil::GetNodeValue(child), L" \t\n");
            return;
        }
        child = MgXmlUtil::GetNextSibling(child);
    }

    MG_XML_CATCH_AND_THROW(L"MgXmlUtil.GetStringFromElement")

    str = L"";
}

///----------------------------------------------------------------------------
/// <summary>
/// Returns the DOM document as bytes.
/// </summary>
///----------------------------------------------------------------------------

MgByte* MgXmlUtil::ToBytes()
{
    Ptr<MgByte> bytes;
    DOMLSSerializer* theSerializer = NULL;
    DOMLSOutput* theOutputDesc = NULL;
    DOMConfiguration* theDC = NULL;
    XMLFormatTarget* memTarget = NULL;

    MG_XML_TRY()

    bytes = new MgByte();

    // get a serializer, an instance of DOMWriter
    XMLCh tempStr[100];
    XMLString::transcode("LS", tempStr, 99);

    DOMImplementation *impl  = DOMImplementationRegistry::getDOMImplementation(tempStr);
    CHECKNULL(impl, L"MgXmlUtil.ToBytes");

    theSerializer = ((DOMImplementationLS*)impl)->createLSSerializer();
    theOutputDesc = ((DOMImplementationLS*)impl)->createLSOutput();
    CHECKNULL(theSerializer, L"MgXmlUtil.ToBytes");
    CHECKNULL(theOutputDesc, L"MgXmlUtil.ToBytes");
    theDC = theSerializer->getDomConfig();

    // set user specified output encoding
    XMLCh encodeStr[100];
    XMLString::transcode("UTF-8", encodeStr, 99);
    theOutputDesc->setEncoding(encodeStr);

    // set feature if the serializer supports the feature/mode
    if (theDC->canSetParameter(XMLUni::fgDOMWRTSplitCdataSections, gSplitCdataSections))
        theDC->setParameter(XMLUni::fgDOMWRTSplitCdataSections, gSplitCdataSections);

    if (theDC->canSetParameter(XMLUni::fgDOMWRTDiscardDefaultContent, gDiscardDefaultContent))
        theDC->setParameter(XMLUni::fgDOMWRTDiscardDefaultContent, gDiscardDefaultContent);

    if (theDC->canSetParameter(XMLUni::fgDOMWRTFormatPrettyPrint, gFormatPrettyPrint))
        theDC->setParameter(XMLUni::fgDOMWRTFormatPrettyPrint, gFormatPrettyPrint);

    if (theDC->canSetParameter(XMLUni::fgDOMWRTBOM, gWriteBOM))
        theDC->setParameter(XMLUni::fgDOMWRTBOM, gWriteBOM);

    memTarget = new MemBufFormatTarget();
    theOutputDesc->setByteStream(memTarget);

    theSerializer->write(m_doc, theOutputDesc);

    INT32 bytesLen = (INT32)((MemBufFormatTarget*)memTarget)->getLen();
    BYTE_ARRAY_IN rawBytes = (BYTE_ARRAY_IN)((MemBufFormatTarget*)memTarget)->getRawBuffer();
    bytes->Append(rawBytes, bytesLen);

    MG_XML_CATCH(L"MgXmlUtil.ToBytes")

    delete theSerializer;
    delete memTarget;

    MG_XML_THROW()

    return bytes.Detach();
}

///----------------------------------------------------------------------------
/// <summary>
/// Returns the DOM document as a byte reader.
/// </summary>
///----------------------------------------------------------------------------

MgByteReader* MgXmlUtil::ToReader()
{
    Ptr<MgByteReader> byteReader;
    Ptr<MgByte> bytes = ToBytes();
    Ptr<MgByteSource> byteSource = new MgByteSource(bytes);

    byteSource->SetMimeType(MgMimeType::Xml);
    byteReader = byteSource->GetReader();

    return byteReader.Detach();
}

///----------------------------------------------------------------------------
/// <summary>
/// Returns the DOM document as a UTF-8 string.
/// </summary>
///----------------------------------------------------------------------------

void MgXmlUtil::ToStringUtf8(string& str)
{
    Ptr<MgByteReader> byteReader = ToReader();

    byteReader->ToStringUtf8(str);
}

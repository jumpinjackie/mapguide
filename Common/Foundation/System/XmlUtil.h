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

#ifndef MGXML_UTIL_
#define MGXML_UTIL_

/// \cond INTERNAL
class MG_FOUNDATION_API MgXmlUtil
{
    DECLARE_CLASSNAME(MgXmlUtil)

    /// Constructors/Destructor

    public:

        MgXmlUtil();
        explicit MgXmlUtil(XERCES_CPP_NAMESPACE::DOMDocument* document, bool ownDomDoc);
        explicit MgXmlUtil(const string& xmlContent);
        explicit MgXmlUtil(const char* rootElementName);
        virtual ~MgXmlUtil();

    private:

    /// Unimplemented Methods

        MgXmlUtil(const MgXmlUtil& util);
        MgXmlUtil& operator=(const MgXmlUtil& util);

/// Methods

    public:

        static void GetElementNode(const char* elementName,
            const string& document, string& node, bool required = true);

        static XERCES_CPP_NAMESPACE::DOMNode* GetFirstChild(XERCES_CPP_NAMESPACE::DOMNode* parent);
        static XERCES_CPP_NAMESPACE::DOMNode* GetNextSibling(XERCES_CPP_NAMESPACE::DOMNode* sibling);
        static wstring GetTagName(XERCES_CPP_NAMESPACE::DOMElement* element);
        static wstring GetNodeValue(XERCES_CPP_NAMESPACE::DOMNode* node);
        static short GetNodeType(XERCES_CPP_NAMESPACE::DOMNode* node);
        static wstring GetAttribute(XERCES_CPP_NAMESPACE::DOMElement* element, const char* name);
        static void GetTextFromElement(XERCES_CPP_NAMESPACE::DOMElement* element, REFSTRING str);

    public:

        void ParseString(const char* xmlContent);

        XERCES_CPP_NAMESPACE::DOMDocument* GetDocument() const;
        XERCES_CPP_NAMESPACE::DOMElement* GetRootNode() const;

        XERCES_CPP_NAMESPACE::DOMNodeList* GetNodeList(const char* elementName) const;
        XERCES_CPP_NAMESPACE::DOMNodeList* GetNodeList(XERCES_CPP_NAMESPACE::DOMNode* parentNode,
            const char* elementName) const;
        XERCES_CPP_NAMESPACE::DOMNode* GetNodeWithTextData(XERCES_CPP_NAMESPACE::DOMDocument* doc, const char* elementName,
            const wchar_t* elementText);

        XERCES_CPP_NAMESPACE::DOMElement* AddChildNode(XERCES_CPP_NAMESPACE::DOMElement* parentNode, const char* elementName);

        void AddTextNode(XERCES_CPP_NAMESPACE::DOMElement* parentNode, const char* elementName, const char* elementText);
        void AddTextNode(XERCES_CPP_NAMESPACE::DOMElement* parentNode, const char* elementName, bool value);
        void AddTextNode(XERCES_CPP_NAMESPACE::DOMElement* parentNode, const char* elementName, const wchar_t* elementText);

        void UpdateTextNode(XERCES_CPP_NAMESPACE::DOMNode* parentNode, const char* elementText);
        void UpdateTextNode(XERCES_CPP_NAMESPACE::DOMNode* parentNode, const wchar_t* elementText);

        void SetAttribute(XERCES_CPP_NAMESPACE::DOMElement* node, const char* attributeName, const wchar_t* value);

        XERCES_CPP_NAMESPACE::DOMElement* GetElementNode(XERCES_CPP_NAMESPACE::DOMNode* parentNode,
            const char* elementName, bool required = true);

        void GetElementValue(XERCES_CPP_NAMESPACE::DOMNode* parentNode, const char* elementName,
            bool& elementValue, bool required = true);
        void GetElementValue(XERCES_CPP_NAMESPACE::DOMNode* parentNode, const char* elementName,
            wstring& elementValue, bool required = true);
        bool SetElementValue(XERCES_CPP_NAMESPACE::DOMNode* parentNode, const char* elementName,
            const wstring& elementValue);

        void ToStringUtf8(string& str);
        MgByteReader* ToReader();

    protected:

        MgByte* ToBytes();

/// Data Members

    private:

        XercesDOMParser* m_parser;
        MemBufInputSource* m_bis;
        XERCES_CPP_NAMESPACE::DOMDocument* m_doc;
        bool m_ownDomDoc;
};
/// \endcond

#endif // MG_XML_UTIL

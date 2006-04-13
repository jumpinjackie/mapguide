//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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
class MG_SERVICE_API MgXmlUtil
{
    DECLARE_CLASSNAME(MgXmlUtil)

    /// Constructors/Destructor

    public:

        MgXmlUtil();
        explicit MgXmlUtil(DOMDocument* document, bool ownDomDoc);
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

        static DOMNode* GetFirstChild(DOMNode* parent);
        static DOMNode* GetNextSibling(DOMNode* sibling);
        static wstring GetTagName(DOMElement* element);
        static wstring GetNodeValue(DOMNode* node);
        static short GetNodeType(DOMNode* node);
        static wstring GetAttribute(DOMElement* element, const char* name);
        static void GetTextFromElement(DOMElement* element, REFSTRING str);

    public:

        void ParseString(const char* xmlContent);

        DOMDocument* GetDocument() const;
        DOMElement* GetRootNode() const;

        DOMNodeList* GetNodeList(const char* elementName) const;
        DOMNodeList* GetNodeList(DOMNode* parentNode,
            const char* elementName) const;
        DOMNode* GetNodeWithTextData(DOMDocument* doc, const char* elementName,
            const wchar_t* elementText);

        DOMElement* AddChildNode(DOMElement* parentNode, const char* elementName);

        void AddTextNode(DOMElement* parentNode, const char* elementName, const char* elementText);
        void AddTextNode(DOMElement* parentNode, const char* elementName, bool value);
        void AddTextNode(DOMElement* parentNode, const char* elementName, const wchar_t* elementText);

        void UpdateTextNode(DOMNode* parentNode, const char* elementText);
        void UpdateTextNode(DOMNode* parentNode, const wchar_t* elementText);

        void SetAttribute(DOMElement* node, const char* attributeName, const wchar_t* value);

        DOMElement* GetElementNode(DOMNode* parentNode,
            const char* elementName, bool required = true);

        void GetElementValue(DOMNode* parentNode, const char* elementName,
            bool& elementValue, bool required = true);
        void GetElementValue(DOMNode* parentNode, const char* elementName,
            wstring& elementValue, bool required = true);
        bool SetElementValue(DOMNode* parentNode, const char* elementName,
            const wstring& elementValue);

        void ToStringUtf8(string& str);
        MgByteReader* ToReader();

    protected:

        MgByte* ToBytes();

/// Data Members

    private:

        XercesDOMParser* m_parser;
        MemBufInputSource* m_bis;
        DOMDocument* m_doc;
        bool m_ownDomDoc;
};
/// \endcond 

#endif // MG_XML_UTIL

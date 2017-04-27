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

#ifndef MGXML_JSON_CONVERT_
#define MGXML_JSON_CONVERT_

#include "System/XmlDefs.h"
#include "System/XmlUtil.h"
#include "JsonDoc.h"

#define XML_DATA_TYPE_NUM_INT 1
#define XML_DATA_TYPE_NUM_DOUBLE 2
#define XML_DATA_TYPE_BOOLEAN 3
#define XML_DATA_TYPE_STRING 4

/// \cond INTERNAL
class MgXmlJsonConvert
{
    typedef map<string, vector<DOMNode *> *> NameNodeList;

    DECLARE_CLASSNAME(MgXmlJsonConvert)

    /// Constructors/Destructor
public:
    MgXmlJsonConvert();
    virtual ~MgXmlJsonConvert();

    /// Methods
public:
    void ToJson(Ptr<MgByteReader> &byteReader, bool bClean = false);
    void ToJson(const string &xmlString, string &jsonString, bool bClean = false);

private:
    void XmlToJsonNode(DOMNode *node, bool bClean);
    bool ValidateTextContent(const string &textContent);
    void ProcessObjectNode(DOMNode *node, bool bClean);
    void ProcessArrayNode(int index, DOMNode *node, bool bClean);

    static bool GetAttributePath(DOMNode* attribute, DOMNode* parent, string& path, const XMLCh* suffix = NULL);
    static bool GetAttributeType(DOMNode* attribute, DOMNode* parent, int& type, const XMLCh* suffix = NULL);

    static bool GetElementPath(DOMNode* node, string& path, const XMLCh* suffix = NULL);
    static bool GetElementType(DOMNode* node, int& type, const XMLCh* suffix = NULL);
    static bool IsMultiple(DOMNode* node, bool& isMultiple, const XMLCh* suffix = NULL);
    /// Data Members
private:
    MgXmlUtil m_xmlUtil;
    MgJsonDoc m_jsonDoc;

private:
    static bool Initialize();
private:
    static bool m_isInitialized;
};
/// \endcond

#endif // MGXML_JSON_CONVERT_

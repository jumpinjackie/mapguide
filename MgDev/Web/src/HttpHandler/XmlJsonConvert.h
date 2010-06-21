//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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
    void ToJson(Ptr<MgByteReader> &byteReader);
    void ToJson(const string &xmlString, string &jsonString);

private:
    void XmlToJsonNode(DOMNode *node);
    bool ValidateTextContent(const string &textContent);
    void ProcessObjectNode(DOMNode *node);
    void ProcessArrayNode(int index, DOMNode *node);

    /// Data Members
private:
    MgXmlUtil m_xmlUtil;
    MgJsonDoc m_jsonDoc;
};
/// \endcond

#endif // MGXML_JSON_CONVERT_

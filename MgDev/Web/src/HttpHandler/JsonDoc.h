//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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

#ifndef MGJSON_DOC_
#define MGJSON_DOC_

#include <json/json.h>
using namespace Json;

struct MgJsonNode
{
    MgJsonNode() {}
    MgJsonNode(const StaticString &name, Json::Value element)
    {
        this->Name = name;
        this->Element = element;
        this->isUsingStaticString = true;
    }
    MgJsonNode(const string &name, Json::Value element)
    {
        this->strName = name;
        this->Element = element;
        this->isUsingStaticString = false;
    }
    MgJsonNode(Json::Value element, int index)
    {
        this->Element = element;
        this->Index = index;
    }
    MgJsonNode(Json::Value element)
    {
        this->Element = element;
    }

    string strName;
    StaticString Name;
    bool isUsingStaticString;
    Json::Value Element;
    int Index;
};

/// \cond INTERNAL
class MgJsonDoc
{
    DECLARE_CLASSNAME(MgJsonDoc)

    /// Constructors/Destructor
public:
    MgJsonDoc();
    virtual ~MgJsonDoc();

    /// Methods
public:

    void BeginObject(const char *name);
    void BeginObject(const string &name);
    void EndObject();
    void AddEmptyObject(const char *name);
    void AddEmptyObject(const string &name);
    void Add(const char *name, const char *value);
    void Add(const char *name, const string &value);
    void Add(const string &name, const char *value);
    void Add(const string &name, const string &value);
    void Add(const char *name, bool value);
    void Add(const string &name, bool value);
    void AddAttribute(const char *name, const char *value);
    void AddAttribute(const char *name, const string &value);
    void AddAttribute(const string &name, const char *value);
    void AddAttribute(const string &name, const string &value);
    void AddAttribute(const char *name, bool value);
    void AddAttribute(const string &name, bool value);

    void BeginArray(const char *name);
    void BeginArray(int size, const char *name);
    void BeginArray(int size, const string &name);
    void EndArray();
    void SetArrayValue(int index, const char *value);
    void SetArrayValue(int index, const string &value);

    void AppendArrayValue(const char *value);
    void AppendArrayValue(const string &value);
    void BeginAppendArrayObject();
    void EndAppendArrayObject();

    void BeginArrayObject(int index);
    void EndArrayObject();

    void Print(string &result);

    /// Data Members
private:

    stack<MgJsonNode *> m_tree;
    MgJsonNode *m_root;
    
    // We use the FastWriter class here to produce very compact output 
    // with minimal linebreaks and spaces. StyledWriter could be used instead
    // if a more structured format was desired.
    FastWriter m_writer;
};
/// \endcond

#endif // MGJSON_DOC_

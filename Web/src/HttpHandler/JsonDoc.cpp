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
#include "JsonDoc.h"

#include <string>
#include <json/json.h>

using namespace Json;
using namespace std;

MgJsonDoc::MgJsonDoc()
{
    m_root = new MgJsonNode();
    m_tree.push(m_root);
}

MgJsonDoc::~MgJsonDoc()
{
    m_tree.pop();
    delete m_root;
}

void MgJsonDoc::BeginObject(const char *name)
{
    MgJsonNode *node = new MgJsonNode(StaticString(name), Value(Json::objectValue));
    m_tree.push(node);
}

void MgJsonDoc::BeginObject(const string &name)
{
    MgJsonNode *node = new MgJsonNode(name, Value(Json::objectValue));
    m_tree.push(node);
}

void MgJsonDoc::EndObject()
{
    MgJsonNode *childNode = m_tree.top();
    m_tree.pop();
    MgJsonNode *parentNode = m_tree.top();
    if (childNode->isUsingStaticString)
    {
        parentNode->Element[childNode->Name] = childNode->Element;
    }
    else
    {
        parentNode->Element[childNode->strName] = childNode->Element;
    }
    delete childNode;
}

void MgJsonDoc::AddEmptyObject(const char *name)
{
    MgJsonNode *parentNode = m_tree.top();
    parentNode->Element[StaticString(name)] = Value(Json::nullValue);
}

void MgJsonDoc::AddEmptyObject(const string &name)
{
    MgJsonNode *parentNode = m_tree.top();
    parentNode->Element[name] = Value(Json::nullValue);
}

void MgJsonDoc::Add(const char *name, const char *value)
{
    MgJsonNode *node = m_tree.top();
    node->Element[StaticString(name)] = StaticString(value);
}

void MgJsonDoc::Add(const char *name, const string &value)
{
    MgJsonNode *node = m_tree.top();
    node->Element[StaticString(name)] = value;
}

void MgJsonDoc::Add(const string &name, const char *value)
{
    MgJsonNode *node = m_tree.top();
    node->Element[name] = StaticString(value);
}

void MgJsonDoc::Add(const string &name, const string &value)
{
    MgJsonNode *node = m_tree.top();
    node->Element[name] = value;
}

void MgJsonDoc::Add(const char *name, bool value)
{
    MgJsonNode *node = m_tree.top();
    node->Element[StaticString(name)] = value;
}

void MgJsonDoc::Add(const string &name, bool value)
{
    MgJsonNode *node = m_tree.top();
    node->Element[name] = value;
}

void MgJsonDoc::AddAttribute(const char *name, const char *value)
{
    this->Add("@" + string(name), value);
}

void MgJsonDoc::AddAttribute(const char *name, const string &value)
{
    this->Add("@" + string(name), value);
}

void MgJsonDoc::AddAttribute(const string &name, const char *value)
{
    this->Add("@" + name, value);
}

void MgJsonDoc::AddAttribute(const string &name, const string &value)
{
    this->Add("@" + name, value);
}

void MgJsonDoc::AddAttribute(const char *name, bool value)
{
    this->Add("@" + string(name), value);
}

void MgJsonDoc::AddAttribute(const string &name, bool value)
{
    this->Add("@" + name, value);
}

void MgJsonDoc::BeginArray(const char *name)
{
    Value element(Json::arrayValue);
    MgJsonNode *node = new MgJsonNode(StaticString(name), element);
    m_tree.push(node);
}

void MgJsonDoc::BeginArray(int size, const char *name)
{
    Value element(Json::arrayValue);
    element.resize(size);
    MgJsonNode *node = new MgJsonNode(StaticString(name), element);
    m_tree.push(node);
}

void MgJsonDoc::BeginArray(int size, const string &name)
{
    Value element(Json::arrayValue);
    element.resize(size);
    MgJsonNode *node = new MgJsonNode(name, element);
    m_tree.push(node);
}

void MgJsonDoc::EndArray()
{
    MgJsonNode *childNode = m_tree.top();
    m_tree.pop();
    MgJsonNode *parentNode = m_tree.top();
    if (childNode->isUsingStaticString)
    {
        parentNode->Element[childNode->Name] = childNode->Element;
    }
    else
    {
        parentNode->Element[childNode->strName] = childNode->Element;
    }
    delete childNode;
}

void MgJsonDoc::SetArrayValue(int index, const char *value)
{
    MgJsonNode *node = m_tree.top();
    node->Element[index] = StaticString(value);
}

void MgJsonDoc::SetArrayValue(int index, const string &value)
{
    MgJsonNode *node = m_tree.top();
    node->Element[index] = value;
}

void MgJsonDoc::AppendArrayValue(const char *value)
{
    MgJsonNode *node = m_tree.top();
    (node->Element).append(Json::Value(StaticString(value)));
}

void MgJsonDoc::AppendArrayValue(const string &value)
{
    MgJsonNode *node = m_tree.top();
    (node->Element).append(Json::Value(value));
}

void MgJsonDoc::BeginAppendArrayObject()
{
    MgJsonNode *node = new MgJsonNode(Value(Json::objectValue));
    m_tree.push(node);
}

void MgJsonDoc::EndAppendArrayObject()
{
    MgJsonNode *childNode = m_tree.top();
    m_tree.pop();
    MgJsonNode *parentNode = m_tree.top();
    (parentNode->Element).append(childNode->Element);
    delete childNode;
}

void MgJsonDoc::BeginArrayObject(int index)
{
    MgJsonNode *node = new MgJsonNode(Value(Json::objectValue), index);
    m_tree.push(node);
}

void MgJsonDoc::EndArrayObject()
{
    MgJsonNode *childNode = m_tree.top();
    m_tree.pop();
    MgJsonNode *parentNode = m_tree.top();
    parentNode->Element[childNode->Index] = childNode->Element;
    delete childNode;
}

void MgJsonDoc::Print(string &result)
{
    result = m_writer.write(m_root->Element);
}

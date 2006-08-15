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

#include "stdafx.h"
#include "IONameStringPair.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

IONameStringPair::IONameStringPair()
{
    this->_nameStringPair = NULL;
    this->layer = NULL;
}

IONameStringPair::IONameStringPair(VectorLayerDefinition *layer)
{
    this->_nameStringPair = NULL;
    this->layer = layer;
}

IONameStringPair::~IONameStringPair()
{
}

void IONameStringPair::StartElement(const wchar_t *name, HandlerStack *handlerStack)
{
    m_currElemName = name;
    if (m_currElemName == L"PropertyMapping") // NOXLATE
    {
        m_startElemName = name;
        this->_nameStringPair = new NameStringPair(L"", L"");
    }
}

void IONameStringPair::ElementChars(const wchar_t *ch)
{
    if (m_currElemName == L"Name") // NOXLATE
        (this->_nameStringPair)->SetName(ch);
    else if (m_currElemName == L"Value") // NOXLATE
        (this->_nameStringPair)->SetValue(ch);
}

void IONameStringPair::EndElement(const wchar_t *name, HandlerStack *handlerStack)
{
    if (m_startElemName == name)
    {
        this->layer->GetPropertyMappings()->Adopt(this->_nameStringPair);
        handlerStack->pop();
        this->layer = NULL;
        this->_nameStringPair = NULL;
        m_startElemName = L"";
        delete this;
    }
}

void IONameStringPair::Write(MdfStream &fd, NameStringPair *nameStringPair)
{
    //Property: Name
    fd << tab() << "<Name>"; // NOXLATE
    fd << EncodeString(nameStringPair->GetName());
    fd << "</Name>" << std::endl; // NOXLATE

    //Property: Value
    fd << tab() << "<Value>"; // NOXLATE
    fd << EncodeString(nameStringPair->GetValue());
    fd << "</Value>" << std::endl; // NOXLATE
}

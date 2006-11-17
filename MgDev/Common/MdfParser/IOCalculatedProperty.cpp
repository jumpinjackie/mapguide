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
#include "IOCalculatedProperty.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

IOCalculatedProperty::IOCalculatedProperty()
    : m_pCalculatedProperty(NULL), m_pExtension(NULL)
{
}

IOCalculatedProperty::IOCalculatedProperty(Extension *pExtension)
    : m_pCalculatedProperty(NULL), m_pExtension(pExtension)
{
}

IOCalculatedProperty::~IOCalculatedProperty()
{
}

void IOCalculatedProperty::StartElement(const wchar_t *name, HandlerStack *handlerStack)
{
    m_currElemName = name;
    if (m_currElemName == L"CalculatedProperty") // NOXLATE
    {
        m_startElemName = name;
        m_pCalculatedProperty = new CalculatedProperty();
    }
}

void IOCalculatedProperty::ElementChars(const wchar_t *ch)
{
    if (m_currElemName == L"Name") // NOXLATE
        (this->m_pCalculatedProperty)->SetName(ch);
    else if (m_currElemName == L"Expression") // NOXLATE
        (this->m_pCalculatedProperty)->SetExpression(ch);
}

void IOCalculatedProperty::EndElement(const wchar_t *name, HandlerStack *handlerStack)
{
    if (m_startElemName == name)
    {
        m_pExtension->GetCalculatedProperties()->Adopt(m_pCalculatedProperty);
        handlerStack->pop();
        this->m_pCalculatedProperty = NULL;
        m_startElemName = L"";
        delete this;
    }
}

void IOCalculatedProperty::Write(MdfStream &fd,  CalculatedProperty *pCalculatedProperty)
{
    fd << tab() << "<CalculatedProperty>" << std::endl; // NOXLATE
    inctab();

    // Property: Name
    fd << tab() << "<Name>"; // NOXLATE
    fd << EncodeString(pCalculatedProperty->GetName());
    fd << "</Name>" << std::endl; // NOXLATE
    
    // Property: Expression
    fd << tab() << "<Expression>";  // NOXLATE
    fd << EncodeString(pCalculatedProperty->GetExpression());
    fd << "</Expression>" << std::endl; // NOXLATE

    dectab();
    fd << tab() << "</CalculatedProperty>" << std::endl; // NOXLATE
}

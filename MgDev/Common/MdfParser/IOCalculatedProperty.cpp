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

#include "stdafx.h"
#include "IOCalculatedProperty.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, CalculatedProperty);
ELEM_MAP_ENTRY(2, Name);
ELEM_MAP_ENTRY(3, Expression);
ELEM_MAP_ENTRY(4, ExtendedData1);


IOCalculatedProperty::IOCalculatedProperty(Version& version) : SAX2ElementHandler(version)
{
    this->m_calculatedProperty = NULL;
    this->m_extension = NULL;
}


IOCalculatedProperty::IOCalculatedProperty(Extension* extension, Version& version) : SAX2ElementHandler(version)
{
    this->m_calculatedProperty = NULL;
    this->m_extension = extension;
}


IOCalculatedProperty::~IOCalculatedProperty()
{
}


void IOCalculatedProperty::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    this->m_currElemId = _ElementIdFromName(name);

    switch (this->m_currElemId)
    {
    case eCalculatedProperty:
        this->m_startElemName = name;
        this->m_calculatedProperty = new CalculatedProperty();
        break;

    case eExtendedData1:
        this->m_procExtData = true;
        break;

    case eUnknown:
        ParseUnknownXml(name, handlerStack);
        break;
    }
}


void IOCalculatedProperty::ElementChars(const wchar_t* ch)
{
    switch (this->m_currElemId)
    {
    case eName:
        this->m_calculatedProperty->SetName(ch);
        break;

    case eExpression:
        this->m_calculatedProperty->SetExpression(ch);
        break;
    }
}


void IOCalculatedProperty::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_calculatedProperty->SetUnknownXml(this->m_unknownXml);

        this->m_extension->GetCalculatedProperties()->Adopt(this->m_calculatedProperty);
        this->m_calculatedProperty = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
    else if (eExtendedData1 == _ElementIdFromName(name))
    {
        this->m_procExtData = false;
    }
}


void IOCalculatedProperty::Write(MdfStream& fd, CalculatedProperty* calculatedProperty, Version* version, MgTab& tab)
{
    fd << tab.tab() << startStr(sCalculatedProperty) << std::endl;
    tab.inctab();

    // Property: Name
    fd << tab.tab() << startStr(sName);
    fd << EncodeString(calculatedProperty->GetName());
    fd << endStr(sName) << std::endl;

    // Property: Expression
    fd << tab.tab() << startStr(sExpression);
    fd << EncodeString(calculatedProperty->GetExpression());
    fd << endStr(sExpression) << std::endl;

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, calculatedProperty->GetUnknownXml(), version, tab);

    tab.dectab();
    fd << tab.tab() << endStr(sCalculatedProperty) << std::endl;
}

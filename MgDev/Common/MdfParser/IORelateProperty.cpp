//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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
#include "IORelateProperty.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, RelateProperty);
ELEM_MAP_ENTRY(2, FeatureClassProperty);
ELEM_MAP_ENTRY(3, AttributeClassProperty);


IORelateProperty::IORelateProperty()
{
    this->m_relateProperty = NULL;
    this->m_attributeRelate = NULL;
}


IORelateProperty::IORelateProperty(AttributeRelate* attributeRelate)
{
    this->m_relateProperty = NULL;
    this->m_attributeRelate = attributeRelate;
}


IORelateProperty::~IORelateProperty()
{
}


void IORelateProperty::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    this->m_currElemId = _ElementIdFromName(name);

    switch (this->m_currElemId)
    {
    case eRelateProperty:
        this->m_startElemName = name;
        this->m_relateProperty = new RelateProperty();
        break;

    case eUnknown:
        ParseUnknownXml(name, handlerStack);
        break;

    default:
        break;
    }
}


void IORelateProperty::ElementChars(const wchar_t* ch)
{
    if (this->m_currElemName == L"FeatureClassProperty") // NOXLATE
    {
        // parse input string to separate primary AttributeRelate
        // prefix and a property name
        MdfString primaryAttributeRelateName;
        MdfString propertyName;
        RelateProperty::ParseDelimitedClassName (ch, primaryAttributeRelateName, propertyName);
        this->m_relateProperty->SetFeatureClassProperty(propertyName, primaryAttributeRelateName);
    }
    else if (this->m_currElemName == L"AttributeClassProperty") // NOXLATE
        this->m_relateProperty->SetAttributeClassProperty(ch);
}


void IORelateProperty::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_relateProperty->SetUnknownXml(this->m_unknownXml);

        this->m_attributeRelate->GetRelateProperties()->Adopt(this->m_relateProperty);
        this->m_relateProperty = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
}


void IORelateProperty::Write(MdfStream& fd, RelateProperty* relateProperty, Version* version)
{
    fd << tab() << "<RelateProperty>" << std::endl; // NOXLATE
    inctab();

    // Property: FeatureClassProperty
    fd << tab() << "<FeatureClassProperty>"; // NOXLATE
    // use false parameter to get the complete property name with the prefix
    fd << EncodeString(relateProperty->GetFeatureClassProperty(false));
    fd << "</FeatureClassProperty>" << std::endl; // NOXLATE

    // Property: AttributeClassProperty
    fd << tab() << "<AttributeClassProperty>"; // NOXLATE
    fd << EncodeString(relateProperty->GetAttributeClassProperty());
    fd << "</AttributeClassProperty>" << std::endl; // NOXLATE

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, relateProperty->GetUnknownXml(), version);

    dectab();
    fd << tab() << "</RelateProperty>" << std::endl; // NOXLATE
}

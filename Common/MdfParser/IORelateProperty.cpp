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
#include "IORelateProperty.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

IORelateProperty::IORelateProperty()
    : m_pRelateProperty(NULL), m_pAttributeRelate(NULL)
{
}

IORelateProperty::IORelateProperty(AttributeRelate *pAttributeRelate)
    : m_pRelateProperty(NULL), m_pAttributeRelate(pAttributeRelate)
{
}

IORelateProperty::~IORelateProperty()
{
}

void IORelateProperty::StartElement(const wchar_t *name, HandlerStack *handlerStack)
{
    m_currElemName = name;
    if (m_currElemName == L"RelateProperty") // NOXLATE
    {
        m_startElemName = name;
        m_pRelateProperty = new RelateProperty();
    }
}

void IORelateProperty::ElementChars(const wchar_t *ch)
{
    if (m_currElemName == L"FeatureClassProperty") // NOXLATE
    {
        // parse input string to separate primary AttributeRelate
        // prefix and a property name
        MdfString primaryAttributeRelateName;
        MdfString propertyName;
        RelateProperty::ParseDelimitedClassName (ch, primaryAttributeRelateName, propertyName);
        (this->m_pRelateProperty)->SetFeatureClassProperty(propertyName, primaryAttributeRelateName);
    }
    else if (m_currElemName == L"AttributeClassProperty") // NOXLATE
        (this->m_pRelateProperty)->SetAttributeClassProperty(ch);
}

void IORelateProperty::EndElement(const wchar_t *name, HandlerStack *handlerStack)
{
    if (m_startElemName == name)
    {
        m_pAttributeRelate->GetRelateProperties()->Adopt(m_pRelateProperty);
        handlerStack->pop();
        this->m_pRelateProperty = NULL;
        m_startElemName = L"";
        delete this;
    }
}

void IORelateProperty::Write(MdfStream &fd,  RelateProperty *pRelateProperty)
{
    fd << tab() << "<RelateProperty>" << std::endl; // NOXLATE
    inctab();

    // Property: FeatureClassProperty
    fd << tab() << "<FeatureClassProperty>"; // NOXLATE
    // use false parameter to get the complete property name with the prefix
    fd << EncodeString(pRelateProperty->GetFeatureClassProperty(false));
    fd << "</FeatureClassProperty>" << std::endl; // NOXLATE
    
    // Property: AttributeClassProperty
    fd << tab() << "<AttributeClassProperty>";  // NOXLATE
    fd << EncodeString(pRelateProperty->GetAttributeClassProperty());
    fd << "</AttributeClassProperty>" << std::endl; // NOXLATE

    dectab();
    fd << tab() << "</RelateProperty>" << std::endl; // NOXLATE
}


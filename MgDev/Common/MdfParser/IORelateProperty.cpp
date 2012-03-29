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
#include "IORelateProperty.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, RelateProperty);
ELEM_MAP_ENTRY(2, FeatureClassProperty);
ELEM_MAP_ENTRY(3, AttributeClassProperty);
ELEM_MAP_ENTRY(4, ExtendedData1);


IORelateProperty::IORelateProperty(Version& version) : SAX2ElementHandler(version)
{
    this->m_relateProperty = NULL;
    this->m_attributeRelate = NULL;
}


IORelateProperty::IORelateProperty(AttributeRelate* attributeRelate, Version& version) : SAX2ElementHandler(version)
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

    case eExtendedData1:
        this->m_procExtData = true;
        break;

    case eUnknown:
        ParseUnknownXml(name, handlerStack);
        break;
    }
}


void IORelateProperty::ElementChars(const wchar_t* ch)
{
    switch (this->m_currElemId)
    {
    case eFeatureClassProperty:
        {
            // parse input string to separate primary AttributeRelate
            // prefix and a property name
            MdfString primaryAttributeRelateName;
            MdfString propertyName;
            RelateProperty::ParseDelimitedClassName(ch, primaryAttributeRelateName, propertyName);
            this->m_relateProperty->SetFeatureClassProperty(propertyName, primaryAttributeRelateName);
        }
        break;

    case eAttributeClassProperty:
        this->m_relateProperty->SetAttributeClassProperty(ch);
        break;
    }
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
    else if (eExtendedData1 == _ElementIdFromName(name))
    {
        this->m_procExtData = false;
    }
}


void IORelateProperty::Write(MdfStream& fd, RelateProperty* relateProperty, Version* version, MgTab& tab)
{
    fd << tab.tab() << startStr(sRelateProperty) << std::endl;
    tab.inctab();

    // Property: FeatureClassProperty
    fd << tab.tab() << startStr(sFeatureClassProperty);
    // use false parameter to get the complete property name with the prefix
    fd << EncodeString(relateProperty->GetFeatureClassProperty(false));
    fd << endStr(sFeatureClassProperty) << std::endl;

    // Property: AttributeClassProperty
    fd << tab.tab() << startStr(sAttributeClassProperty);
    fd << EncodeString(relateProperty->GetAttributeClassProperty());
    fd << endStr(sAttributeClassProperty) << std::endl;

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, relateProperty->GetUnknownXml(), version, tab);

    tab.dectab();
    fd << tab.tab() << endStr(sRelateProperty) << std::endl;
}

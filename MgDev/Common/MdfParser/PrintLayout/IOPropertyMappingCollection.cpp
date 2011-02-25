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

#include "../stdafx.h"
#include "../IOUnknown.h"
#include "IOPropertyMappingCollection.h"

CREATE_ELEMENT_MAP;
// Start Elements
ELEM_MAP_ENTRY(1, PropertyMappings);
// Local Elements
ELEM_MAP_ENTRY(2, PropertyMapping);
ELEM_MAP_ENTRY(3, TargetProperty);
ELEM_MAP_ENTRY(4, SourceProperty);
ELEM_MAP_ENTRY(5, SourceUnits);
ELEM_MAP_ENTRY(6, ExtendedData1);

IOPropertyMappingCollection::IOPropertyMappingCollection(PropertyMappingCollection* propMappings, Version& version) :
    SAX2ElementHandler(version),
    m_propMappings(propMappings)
{
    // The parser will update all the data of the object pointed by the following assigned pointer.
    _ASSERT(NULL != m_propMappings);
}

IOPropertyMappingCollection::~IOPropertyMappingCollection()
{
}

void IOPropertyMappingCollection::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    m_currElemName = name;
    m_currElemId = _ElementIdFromName(name);

    switch (m_currElemId)
    {
    case ePropertyMappings:
        m_startElemName = name;
        break;

    case ePropertyMapping:
        m_currMapping.reset(new PropertyMapping());
        break;

    case eExtendedData1:
        m_procExtData = true;
        break;

    case eUnknown:
        ParseUnknownXml(name, handlerStack);
    }
}

void IOPropertyMappingCollection::ElementChars(const wchar_t* ch)
{
    switch (m_currElemId)
    {
    case eTargetProperty:
        m_currMapping->SetTargetProperty(ch);
        break;

    case eSourceProperty:
        m_currMapping->SetSourceProperty(ch);
        break;

    case eSourceUnits:
        m_currMapping->SetSourceUnits(ch);
        break;
    }
}

void IOPropertyMappingCollection::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (m_startElemName == name)
    {
        m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
    else if (ePropertyMapping == _ElementIdFromName(name))
    {
        m_currMapping->SetUnknownXml(m_unknownXml);
        m_propMappings->Adopt(m_currMapping.release());
    }
    else if (eExtendedData1 == _ElementIdFromName(name))
    {
        m_procExtData = false;
    }
}

void IOPropertyMappingCollection::Write(MdfStream& fd, PropertyMappingCollection* propMappings, Version* version, const std::string& name, MgTab& tab)
{
    _ASSERT(NULL != propMappings);

    fd << tab.tab() << startStr(name) << std::endl;
    tab.inctab();

    for (int i = 0; i < propMappings->GetCount(); ++i)
    {
       fd << tab.tab() << startStr(sPropertyMapping) << std::endl;
       tab.inctab();

        PropertyMapping* propMapping = dynamic_cast<PropertyMapping*>(propMappings->GetAt(i));
        _ASSERT(NULL != propMapping);

        // Property: TargetProperty
        fd << tab.tab() << startStr(sTargetProperty);
        fd << EncodeString(propMapping->GetTargetProperty());
        fd << endStr(sTargetProperty) << std::endl;

        // Property: SourceProperty
        fd << tab.tab() << startStr(sSourceProperty);
        fd << EncodeString(propMapping->GetSourceProperty());
        fd << endStr(sSourceProperty) << std::endl;

        // Property: SourceUnits
        fd << tab.tab() << startStr(sSourceUnits);
        fd << EncodeString(propMapping->GetSourceUnits());
        fd << endStr(sSourceUnits) << std::endl;

        // Write any unknown XML / extended data
        IOUnknown::Write(fd, propMapping->GetUnknownXml(), version, tab);

        tab.dectab();
        fd << tab.tab() << endStr(sPropertyMapping) << std::endl;
    }

    tab.dectab();
    fd << tab.tab() << endStr(name) << std::endl;
}

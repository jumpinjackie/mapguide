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

#include "stdafx.h"
#include "IOPropertyMappingCollection.h"
#include "IOUnknown.h"

CREATE_ELEMENT_MAP;
// Start Elements
ELEM_MAP_ENTRY(1, PropertyMapping);
// Local Elements
ELEM_MAP_ENTRY(2, TargetProperty);
ELEM_MAP_ENTRY(3, SourceProperty);
ELEM_MAP_ENTRY(4, SourceUnits);
ELEM_MAP_ENTRY(5, ExtendedData1);

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
    case ePropertyMapping:
        m_startElemName = name;
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
        m_currMapping->SetUnknownXml(m_unknownXml);
        m_propMappings->Adopt(m_currMapping.release());

        m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
    else if (eExtendedData1 == _ElementIdFromName(name))
    {
        m_procExtData = false;
    }
}

void IOPropertyMappingCollection::Write(MdfStream& fd, PropertyMappingCollection* propMappings, Version* version, const std::string& name)
{
    _ASSERT(NULL != propMappings);

    fd << tab() << startStr(name) << std::endl;
    inctab();

    for (int i = 0; i < propMappings->GetCount(); ++i)
    {
       fd << tab() << startStr(sPropertyMapping) << std::endl;
       inctab();

        PropertyMapping* propMapping = dynamic_cast<PropertyMapping*>(propMappings->GetAt(i));
        _ASSERT(NULL != propMapping);

        // Property: TargetProperty
        fd << tab() << startStr(sTargetProperty);
        fd << EncodeString(propMapping->GetTargetProperty());
        fd << endStr(sTargetProperty) << std::endl;

        // Property: SourceProperty
        fd << tab() << startStr(sSourceProperty);
        fd << EncodeString(propMapping->GetSourceProperty());
        fd << endStr(sSourceProperty) << std::endl;

        // Property: SourceUnits
        fd << tab() << startStr(sSourceUnits);
        fd << EncodeString(propMapping->GetSourceUnits());
        fd << endStr(sSourceUnits) << std::endl;

        // Write any unknown XML / extended data
        IOUnknown::Write(fd, propMapping->GetUnknownXml(), version);

        dectab();
        fd << tab() << endStr(sPropertyMapping) << std::endl;
    }

    dectab();
    fd << tab() << endStr(name) << std::endl;
}

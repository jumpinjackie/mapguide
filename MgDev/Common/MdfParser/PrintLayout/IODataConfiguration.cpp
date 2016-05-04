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
#include "IODataConfiguration.h"
#include "IOPropertyMappingCollection.h"

CREATE_ELEMENT_MAP;
// Start Elements
ELEM_MAP_ENTRY(1, Data);
// Local Elements
ELEM_MAP_ENTRY(2, ResourceId);
ELEM_MAP_ENTRY(3, FeatureClass);
ELEM_MAP_ENTRY(4, Geometry);
ELEM_MAP_ENTRY(5, Filter);
ELEM_MAP_ENTRY(6, PropertyMappings);
ELEM_MAP_ENTRY(7, ExtendedData1);

IODataConfiguration::IODataConfiguration(DataConfiguration* dataConf, Version& version) :
    SAX2ElementHandler(version),
    m_dataConf(dataConf)
{
    // The parser will update all the data of the object pointed by the following assigned pointer.
    _ASSERT(NULL != m_dataConf);
}

IODataConfiguration::~IODataConfiguration()
{
}

void IODataConfiguration::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    m_currElemName = name;
    m_currElemId = _ElementIdFromName(name);

    switch (m_currElemId)
    {
    case eData:
        m_startElemName = name;
        break;

    case ePropertyMappings:
        {
            IOPropertyMappingCollection* IO = new IOPropertyMappingCollection(m_dataConf->GetPropertyMappings(), m_version);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;

    case eExtendedData1:
        m_procExtData = true;
        break;

    case eUnknown:
        ParseUnknownXml(name, handlerStack);
        break;
    }
}

void IODataConfiguration::ElementChars(const wchar_t* ch)
{
    switch (m_currElemId)
    {
    case eResourceId:
        m_dataConf->SetResourceId(ch);
        break;

    case eFeatureClass:
        m_dataConf->SetFeatureClass(ch);
        break;

    case eGeometry:
        m_dataConf->SetGeometry(ch);
        break;

    case eFilter:
        m_dataConf->SetFilter(ch);
        break;
    }
}

void IODataConfiguration::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (m_startElemName == name)
    {
        m_dataConf->SetUnknownXml(m_unknownXml);

        m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
    else if (eExtendedData1 == _ElementIdFromName(name))
    {
        m_procExtData = false;
    }
}

void IODataConfiguration::Write(MdfStream& fd, DataConfiguration* dataConf, Version* version, MgTab& tab)
{
    _ASSERT(NULL != dataConf);

    fd << tab.tab() << startStr(sData) << std::endl;
    tab.inctab();

    // Property: ResourceId
    fd << tab.tab() << startStr(sResourceId);
    fd << EncodeString(dataConf->GetResourceId());
    fd << endStr(sResourceId) << std::endl;

    // Property: FeatureClass
    fd << tab.tab() << startStr(sFeatureClass);
    fd << EncodeString(dataConf->GetFeatureClass());
    fd << endStr(sFeatureClass) << std::endl;

    // Property: Geometry
    fd << tab.tab() << startStr(sGeometry);
    fd << EncodeString(dataConf->GetGeometry());
    fd << endStr(sGeometry) << std::endl;

    // Property: Filter
    fd << tab.tab() << startStr(sFilter);
    fd << EncodeString(dataConf->GetFilter());
    fd << endStr(sFilter) << std::endl;

    // Property: PropertyMappings
    IOPropertyMappingCollection::Write(fd, dataConf->GetPropertyMappings(), version, sPropertyMappings, tab);

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, dataConf->GetUnknownXml(), version, tab);

    tab.dectab();
    fd << tab.tab() << endStr(sData) << std::endl;
}

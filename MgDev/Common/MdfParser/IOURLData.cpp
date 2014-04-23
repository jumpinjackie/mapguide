//
//  Copyright (C) 2011 by Autodesk, Inc.
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
#include "IOURLData.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, UrlData);
ELEM_MAP_ENTRY(2, Content);
ELEM_MAP_ENTRY(3, ContentOverride);
ELEM_MAP_ENTRY(4, Description);
ELEM_MAP_ENTRY(5, DescriptionOverride);
ELEM_MAP_ENTRY(6, ExtendedData1);


IOURLData::IOURLData(Version& version) : SAX2ElementHandler(version)
{
    this->m_urlData = NULL;
    this->m_vectorLayerDef = NULL;
}


IOURLData::IOURLData(VectorLayerDefinition* vectorLayerDefinition, Version& version) : SAX2ElementHandler(version)
{
    this->m_urlData = NULL;
    this->m_vectorLayerDef = vectorLayerDefinition;
}


IOURLData::~IOURLData()
{
}


void IOURLData::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    this->m_currElemId = _ElementIdFromName(name);

    switch (this->m_currElemId)
    {
    case eUrlData:
        this->m_startElemName = name;
        if(m_urlData == NULL)
        {
            this->m_urlData = new URLData();
        }
        break;

    case eExtendedData1:
        this->m_procExtData = true;
        break;

    case eUnknown:
        ParseUnknownXml(name, handlerStack);
        break;
    }
}


void IOURLData::ElementChars(const wchar_t* ch)
{
    switch (this->m_currElemId)
    {
    case eContent:
        this->m_urlData->SetUrlContent(ch);
        break;

    case eContentOverride:
        this->m_urlData->SetUrlContentOverride(ch);
        break;

    case eDescription:
        this->m_urlData->SetUrlDescription(ch);
        break;

    case eDescriptionOverride:
        this->m_urlData->SetUrlDescriptionOverrride(ch);
        break;
    }
}


void IOURLData::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_urlData->SetUnknownXml(this->m_unknownXml);

        this->m_vectorLayerDef->AdoptUrlData(this->m_urlData);
        this->m_vectorLayerDef = NULL;
        this->m_urlData = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
    else if (eExtendedData1 == _ElementIdFromName(name))
    {
        this->m_procExtData = false;
    }
}


void IOURLData::Write(MdfStream& fd, URLData* urlData, Version* version, MgTab& tab)
{
    fd << tab.tab() << startStr(sUrlData) << std::endl;
    tab.inctab();

    // Property: Content
    if(!urlData->GetUrlContent().empty())
    {
        fd << tab.tab() << startStr(sContent);
        fd << EncodeString(urlData->GetUrlContent());
        fd << endStr(sContent) << std::endl;
    }

    // Property: Description
    if(!urlData->GetUrlDescription().empty())
    {
        fd << tab.tab() << startStr(sDescription);
        fd << EncodeString(urlData->GetUrlDescription());
        fd << endStr(sDescription) << std::endl;
    }

    // Property: Content Override
    if(!urlData->GetUrlContentOverride().empty())
    {
        fd << tab.tab() << startStr(sContentOverride);
        fd << EncodeString(urlData->GetUrlContentOverride());
        fd << endStr(sContentOverride) << std::endl;
    }

    // Property: Description Override
    if(!urlData->GetUrlDescriptionOverrride().empty())
    {
        fd << tab.tab() << startStr(sDescriptionOverride);
        fd << EncodeString(urlData->GetUrlDescriptionOverrride());
        fd << endStr(sDescriptionOverride) << std::endl;
    }

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, urlData->GetUnknownXml(), version, tab);

    tab.dectab();
    fd << tab.tab() << endStr(sUrlData) << std::endl;
}

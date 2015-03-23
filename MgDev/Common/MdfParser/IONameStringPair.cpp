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
#include "IONameStringPair.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, Name);
ELEM_MAP_ENTRY(2, Value);
ELEM_MAP_ENTRY(3, ExtendedData1);


IONameStringPair::IONameStringPair(Version& version) : SAX2ElementHandler(version)
{
    this->m_featureSource = NULL;
    this->m_nameStringPair = NULL;
    this->m_layer = NULL;
    this->m_overrides = NULL;
    this->m_tileStoreParams = NULL;
}


IONameStringPair::IONameStringPair(VectorLayerDefinition* layer, Version& version) : SAX2ElementHandler(version)
{
    this->m_featureSource = NULL;
    this->m_nameStringPair = NULL;
    this->m_layer = layer;
    this->m_overrides = NULL;
    this->m_tileStoreParams = NULL;
}


IONameStringPair::IONameStringPair(FeatureSource* featureSource, Version& version) : SAX2ElementHandler(version)
{
    this->m_featureSource = featureSource;
    this->m_nameStringPair = NULL;
    this->m_layer = NULL;
    this->m_overrides = NULL;
    this->m_tileStoreParams = NULL;
}

IONameStringPair::IONameStringPair(TileStoreParameters* params, Version& version) : SAX2ElementHandler(version)
{
    this->m_featureSource = NULL;
    this->m_nameStringPair = NULL;
    this->m_layer = NULL;
    this->m_overrides = NULL;
    this->m_tileStoreParams = params;
}


IONameStringPair::~IONameStringPair()
{
}


void IONameStringPair::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    this->m_currElemId = _ElementIdFromName(name);

    if (this->m_layer)
    {
        if (this->m_currElemName == L"PropertyMapping") // NOXLATE
        {
            this->m_startElemName = name;
            this->m_nameStringPair = new NameStringPair();
        }
        else if (this->m_currElemId == eExtendedData1)
        {
            this->m_procExtData = true;
        }
        else if (this->m_currElemId == eUnknown)
        {
            ParseUnknownXml(name, handlerStack);
        }
    }
    else if (this->m_featureSource || this->m_tileStoreParams)
    {
        if (this->m_currElemName == L"Parameter") // NOXLATE
        {
            this->m_startElemName = name;
            this->m_nameStringPair = new NameStringPair();
        }
        else if (this->m_currElemId == eExtendedData1)
        {
            this->m_procExtData = true;
        }
        else if (this->m_currElemId == eUnknown)
        {
            ParseUnknownXml(name, handlerStack);
        }
    }
}


void IONameStringPair::ElementChars(const wchar_t* ch)
{
    switch (this->m_currElemId)
    {
    case eName:
        this->m_nameStringPair->SetName(ch);
        break;

    case eValue:
        this->m_nameStringPair->SetValue(ch);
        break;
    }
}


void IONameStringPair::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_nameStringPair->SetUnknownXml(this->m_unknownXml);

        if (this->m_layer)
            this->m_layer->GetPropertyMappings()->Adopt(this->m_nameStringPair);
        else if (this->m_featureSource)
            this->m_featureSource->GetParameters()->Adopt(this->m_nameStringPair);
        else if (this->m_tileStoreParams)
            this->m_tileStoreParams->GetParameters()->Adopt(this->m_nameStringPair);

        this->m_layer = NULL;
        this->m_featureSource = NULL;
        this->m_nameStringPair = NULL;
        this->m_tileStoreParams = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
    else if (eExtendedData1 == _ElementIdFromName(name))
    {
        this->m_procExtData = false;
    }
}


void IONameStringPair::Write(MdfStream& fd, std::string name, NameStringPair* nameStringPair, Version* version, MgTab& tab)
{
    fd << tab.tab() << startStr(name) << std::endl;
    tab.inctab();

    // Property: Name
    fd << tab.tab() << startStr(sName);
    fd << EncodeString(nameStringPair->GetName());
    fd << endStr(sName) << std::endl;

    // Property: Value
    fd << tab.tab() << startStr(sValue);
    fd << EncodeString(nameStringPair->GetValue());
    fd << endStr(sValue) << std::endl;

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, nameStringPair->GetUnknownXml(), version, tab);

    tab.dectab();
    fd << tab.tab() << endStr(name) << std::endl;
}

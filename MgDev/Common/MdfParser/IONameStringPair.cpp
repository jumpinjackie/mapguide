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
#include "IONameStringPair.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;


IONameStringPair::IONameStringPair()
{
    this->m_featureSource = NULL;
    this->m_nameStringPair = NULL;
    this->m_layer = NULL;
    this->m_overrides = NULL;
}


IONameStringPair::IONameStringPair(VectorLayerDefinition* layer)
{
    this->m_featureSource = NULL;
    this->m_nameStringPair = NULL;
    this->m_layer = layer;
    this->m_overrides = NULL;
}


IONameStringPair::IONameStringPair(FeatureSource* featureSource)
{
    this->m_featureSource = featureSource;
    this->m_nameStringPair = NULL;
    this->m_layer = NULL;
    this->m_overrides = NULL;
}


IONameStringPair::~IONameStringPair()
{
}


void IONameStringPair::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    if (NULL != this->m_layer)
    {
        if (this->m_currElemName == L"PropertyMapping") // NOXLATE
        {
            this->m_startElemName = name;
            this->m_nameStringPair = new NameStringPair(L"", L"");
        }
        else if (this->m_currElemName == L"ExtendedData1") // NOXLATE
        {
            ParseUnknownXml(name, handlerStack);
        }
    }
    else if (NULL != this->m_featureSource)
    {
        if (this->m_currElemName == L"Parameter") // NOXLATE
        {
            this->m_startElemName = name;
            this->m_nameStringPair = new NameStringPair(L"", L"");
        }
        else if (this->m_currElemName == L"ExtendedData1") // NOXLATE
        {
            ParseUnknownXml(name, handlerStack);
        }
    }
}


void IONameStringPair::ElementChars(const wchar_t* ch)
{
    if (this->m_currElemName == L"Name") // NOXLATE
        this->m_nameStringPair->SetName(ch);
    else if (this->m_currElemName == L"Value") // NOXLATE
        this->m_nameStringPair->SetValue(ch);
}


void IONameStringPair::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_nameStringPair->SetUnknownXml(this->m_unknownXml);

        if (NULL != this->m_layer)
            this->m_layer->GetPropertyMappings()->Adopt(this->m_nameStringPair);
        else if (NULL != this->m_featureSource)
            this->m_featureSource->GetParameters()->Adopt(this->m_nameStringPair);

        this->m_layer = NULL;
        this->m_featureSource = NULL;
        this->m_nameStringPair = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
}


void IONameStringPair::Write(MdfStream& fd, NameStringPair* nameStringPair, Version* version)
{
    //Property: Name
    fd << tab() << "<Name>"; // NOXLATE
    fd << EncodeString(nameStringPair->GetName());
    fd << "</Name>" << std::endl; // NOXLATE

    //Property: Value
    fd << tab() << "<Value>"; // NOXLATE
    fd << EncodeString(nameStringPair->GetValue());
    fd << "</Value>" << std::endl; // NOXLATE

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, nameStringPair->GetUnknownXml(), version);
}

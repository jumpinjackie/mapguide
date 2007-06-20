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
: _nameStringPair(NULL), layer(NULL), featureSource(NULL)
{
    this->_nameStringPair = NULL;
    this->layer = NULL;
}


IONameStringPair::IONameStringPair(VectorLayerDefinition* pLayer)
: _nameStringPair(NULL), layer(pLayer), featureSource(NULL), overrides(NULL)
{
}


IONameStringPair::IONameStringPair(FeatureSource* pFeatureSource)
: _nameStringPair(NULL), layer(NULL), featureSource(pFeatureSource), overrides(NULL)
{
}


IONameStringPair::~IONameStringPair()
{
}


void IONameStringPair::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    m_currElemName = name;
    if (NULL != layer)
    {
        if (m_currElemName == L"PropertyMapping") // NOXLATE
        {
            m_startElemName = name;
            this->_nameStringPair = new NameStringPair(L"", L"");
        }
        else if (m_currElemName == L"ExtendedData1") // NOXLATE
        {
            ParseUnknownXml(name, handlerStack);
        }
    }
    else if (NULL != featureSource)
    {
        if (m_currElemName == L"Parameter") // NOXLATE
        {
            m_startElemName = name;
            this->_nameStringPair = new NameStringPair(L"", L"");
        }
        else if (m_currElemName == L"ExtendedData1") // NOXLATE
        {
            ParseUnknownXml(name, handlerStack);
        }
    }
}


void IONameStringPair::ElementChars(const wchar_t* ch)
{
    if (m_currElemName == L"Name") // NOXLATE
        this->_nameStringPair->SetName(ch);
    else if (m_currElemName == L"Value") // NOXLATE
        this->_nameStringPair->SetValue(ch);
}


void IONameStringPair::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (m_startElemName == name)
    {
        this->_nameStringPair->SetUnknownXml(UnknownXml());

        if (NULL != this->layer)
            this->layer->GetPropertyMappings()->Adopt(this->_nameStringPair);
        else if (NULL != this->featureSource)
            this->featureSource->GetParameters()->Adopt(this->_nameStringPair);

        handlerStack->pop();
        this->layer = NULL;
        this->_nameStringPair = NULL;
        this->featureSource = NULL;
        m_startElemName = L"";
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

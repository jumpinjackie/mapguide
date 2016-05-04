//
//  Copyright (C) 2007-2011 by Autodesk, Inc.
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
#include "IOParameterCollection.h"
#include "IOParameter.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;


IOParameterCollection::IOParameterCollection(ParameterCollection* parameterCollection, Version& version) : SAX2ElementHandler(version)
{
    this->m_parameterCollection = parameterCollection;
}


void IOParameterCollection::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    if (this->m_currElemName == L"ParameterDefinition") // NOXLATE
    {
        this->m_startElemName = name;
    }
    else if (this->m_currElemName == L"Parameter") // NOXLATE
    {
        IOParameter* IO = new IOParameter(this->m_parameterCollection, this->m_version);
        handlerStack->push(IO);
        IO->StartElement(name, handlerStack);
    }
    else if (this->m_currElemName == L"ExtendedData1") // NOXLATE
    {
        this->m_procExtData = true;
    }
    else
    {
        ParseUnknownXml(name, handlerStack);
    }
}


void IOParameterCollection::ElementChars(const wchar_t* ch)
{
}


void IOParameterCollection::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_parameterCollection->SetUnknownXml(this->m_unknownXml);

        this->m_parameterCollection = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
    else if (::wcscmp(name, L"ExtendedData1") == 0) // NOXLATE
    {
        this->m_procExtData = false;
    }
}


void IOParameterCollection::Write(MdfStream& fd, ParameterCollection* parameterCollection, Version* version, MgTab& tab)
{
    // always write the element, even if it has 0 elements
    int numElements = parameterCollection->GetCount();

    fd << tab.tab() << "<ParameterDefinition>" << std::endl; // NOXLATE
    tab.inctab();

    for (int i=0; i<numElements; ++i)
    {
        Parameter* parameter = parameterCollection->GetAt(i);
        IOParameter::Write(fd, parameter, version, tab);
    }

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, parameterCollection->GetUnknownXml(), version, tab);

    tab.dectab();
    fd << tab.tab() << "</ParameterDefinition>" << std::endl; // NOXLATE
}

//
//  Copyright (C) 2007 by Autodesk, Inc.
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
#include "IOOverrideCollection.h"
#include "IOOverride.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;


IOOverrideCollection::IOOverrideCollection(OverrideCollection* overrideCollection)
{
    this->_overrideCollection = overrideCollection;
}


void IOOverrideCollection::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    m_currElemName = name;
    if (m_currElemName == L"ParameterOverrides") // NOXLATE
    {
        m_startElemName = name;
    }
    else if (m_currElemName == L"Override") // NOXLATE
    {
        IOOverride* IO = new IOOverride(this->_overrideCollection);
        handlerStack->push(IO);
        IO->StartElement(name, handlerStack);
    }
    else if (m_currElemName == L"ExtendedData1") // NOXLATE
    {
        ParseUnknownXml(name, handlerStack);
    }
}


void IOOverrideCollection::ElementChars(const wchar_t* ch)
{
}


void IOOverrideCollection::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (m_startElemName == name)
    {
        this->_overrideCollection->SetUnknownXml(UnknownXml());

        this->_overrideCollection = NULL;
        m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
}


void IOOverrideCollection::Write(MdfStream& fd, OverrideCollection* overrideCollection, Version* version)
{
    // always write the element, even if it has 0 elements
    int numElements = overrideCollection->GetCount();

    fd << tab() << "<ParameterOverrides>" << std::endl; // NOXLATE
    inctab();

    for (int i=0; i<numElements; ++i)
    {
        Override* pOverride = overrideCollection->GetAt(i);
        IOOverride::Write(fd, pOverride, version);
    }

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, overrideCollection->GetUnknownXml(), version);

    dectab();
    fd << tab() << "</ParameterOverrides>" << std::endl; // NOXLATE
}

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
#include "IOParameterCollection.h"
#include "IOParameter.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;


IOParameterCollection::IOParameterCollection(ParameterCollection* parameterCollection)
{
    this->_parameterCollection = parameterCollection;
}


void IOParameterCollection::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    m_currElemName = name;
    if (m_currElemName == L"ParameterDefinition") // NOXLATE
    {
        m_startElemName = name;
    }
    else if (m_currElemName == L"Parameter") // NOXLATE
    {
        IOParameter* IO = new IOParameter(this->_parameterCollection);
        handlerStack->push(IO);
        IO->StartElement(name, handlerStack);
    }
    else if (m_currElemName == L"ExtendedData1") // NOXLATE
    {
        ParseUnknownXml(name, handlerStack);
    }
}


void IOParameterCollection::ElementChars(const wchar_t* ch)
{
}


void IOParameterCollection::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (m_startElemName == name)
    {
        this->_parameterCollection->SetUnknownXml(UnknownXml());

        this->_parameterCollection = NULL;
        m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
}


void IOParameterCollection::Write(MdfStream& fd, ParameterCollection* parameterCollection, Version* version)
{
    // always write the element, even if it has 0 elements
    int numElements = parameterCollection->GetCount();

    fd << tab() << "<ParameterDefinition>" << std::endl; // NOXLATE
    inctab();

    for (int i=0; i<numElements; ++i)
    {
        Parameter* parameter = parameterCollection->GetAt(i);
        IOParameter::Write(fd, parameter, version);
    }

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, parameterCollection->GetUnknownXml(), version);

    dectab();
    fd << tab() << "</ParameterDefinition>" << std::endl; // NOXLATE
}

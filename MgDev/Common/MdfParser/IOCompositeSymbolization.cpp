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
#include "IOCompositeSymbolization.h"
#include "IOSymbolInstance.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;


IOCompositeSymbolization::IOCompositeSymbolization(CompositeRule* compositeRule)
{
    this->m_compositeRule = compositeRule;
    this->m_compositeSymbolization = NULL;
}


IOCompositeSymbolization::~IOCompositeSymbolization()
{
}


void IOCompositeSymbolization::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    if (this->m_currElemName == L"CompositeSymbolization") // NOXLATE
    {
        this->m_startElemName = name;
        this->m_compositeSymbolization = new CompositeSymbolization();
    }
    else if (this->m_currElemName == L"SymbolInstance") // NOXLATE
    {
        IOSymbolInstance* IO = new IOSymbolInstance(this->m_compositeSymbolization->GetSymbolCollection());
        handlerStack->push(IO);
        IO->StartElement(name, handlerStack);
    }
    else if (this->m_currElemName == L"ExtendedData1") // NOXLATE
    {
        ParseUnknownXml(name, handlerStack);
    }
}


void IOCompositeSymbolization::ElementChars(const wchar_t* ch)
{
}


void IOCompositeSymbolization::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_compositeSymbolization->SetUnknownXml(this->m_unknownXml);

        this->m_compositeRule->AdoptSymbolization(this->m_compositeSymbolization);
        this->m_compositeRule = NULL;
        this->m_compositeSymbolization = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
}


void IOCompositeSymbolization::Write(MdfStream& fd, CompositeSymbolization* compositeSymbolization, Version* version)
{
    fd << tab() << "<CompositeSymbolization>" << std::endl; // NOXLATE
    inctab();

    SymbolInstanceCollection* instanceCollection = compositeSymbolization->GetSymbolCollection();
    int numInstances = instanceCollection->GetCount();
    for (int i=0; i<numInstances; ++i)
    {
        SymbolInstance* instance = instanceCollection->GetAt(i);
        IOSymbolInstance::Write(fd, instance, version);
    }

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, compositeSymbolization->GetUnknownXml(), version);

    dectab();
    fd << tab() << "</CompositeSymbolization>" << std::endl; // NOXLATE
}

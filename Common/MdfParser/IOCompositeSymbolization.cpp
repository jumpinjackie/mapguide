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

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

IOCompositeSymbolization::IOCompositeSymbolization(CompositeRule* compositeRule)
{
    this->_compositeRule = compositeRule;
    this->_compositeSymbolization = NULL;
}

IOCompositeSymbolization::~IOCompositeSymbolization()
{
}

void IOCompositeSymbolization::StartElement(const wchar_t *name, HandlerStack *handlerStack)
{
    m_currElemName = name;
    if (m_currElemName == L"CompositeSymbolization") // NOXLATE
    {
        m_startElemName = name;
        this->_compositeSymbolization = new CompositeSymbolization();
    }
    else if (m_currElemName == L"SymbolInstance") // NOXLATE
    {
        IOSymbolInstance* IO = new IOSymbolInstance(this->_compositeSymbolization->GetSymbolCollection());
        handlerStack->push(IO);
        IO->StartElement(name, handlerStack);
    }
}

void IOCompositeSymbolization::ElementChars(const wchar_t *ch)
{
}

void IOCompositeSymbolization::EndElement(const wchar_t *name, HandlerStack *handlerStack)
{
    if (m_startElemName == name)
    {
        this->_compositeRule->AdoptSymbolization(this->_compositeSymbolization);
        this->_compositeRule = NULL;
        this->_compositeSymbolization = NULL;
        m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
}

void IOCompositeSymbolization::Write(MdfStream &fd, CompositeSymbolization* compositeSymbolization, Version* version)
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

    dectab();
    fd << tab() << "</CompositeSymbolization>" << std::endl; // NOXLATE
}

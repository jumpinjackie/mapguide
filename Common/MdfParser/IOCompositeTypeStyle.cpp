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
#include "IOCompositeTypeStyle.h"
#include "IOCompositeRule.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;


IOCompositeTypeStyle::IOCompositeTypeStyle(VectorScaleRange* vectorScaleRange)
{
    this->_vectorScaleRange = vectorScaleRange;
    this->_compositeTypeStyle = NULL;
}


IOCompositeTypeStyle::~IOCompositeTypeStyle()
{
}


void IOCompositeTypeStyle::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    m_currElemName = name;
    if (m_currElemName == L"CompositeTypeStyle") // NOXLATE
    {
        m_startElemName = name;
        this->_compositeTypeStyle = new CompositeTypeStyle();
    }
    else if (m_currElemName == L"CompositeRule") // NOXLATE
    {
        IOCompositeRule* IO = new IOCompositeRule(this->_compositeTypeStyle);
        handlerStack->push(IO);
        IO->StartElement(name, handlerStack);
    }
    else if (m_currElemName == L"ExtendedData1") // NOXLATE
    {
        ParseUnknownXml(name, handlerStack);
    }
}


void IOCompositeTypeStyle::ElementChars(const wchar_t* ch)
{
}


void IOCompositeTypeStyle::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (m_startElemName == name)
    {
        if (!UnknownXml().empty())
            this->_compositeTypeStyle->SetUnknownXml(UnknownXml());

        this->_vectorScaleRange->GetFeatureTypeStyles()->Adopt(this->_compositeTypeStyle);
        this->_vectorScaleRange = NULL;
        this->_compositeTypeStyle = NULL;
        m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
}


void IOCompositeTypeStyle::Write(MdfStream& fd, CompositeTypeStyle* compositeTypeStyle, Version* version)
{
    // the schema currently requires at least one rule
    RuleCollection* ruleCollection = compositeTypeStyle->GetRules();
    int numElements = ruleCollection->GetCount();
    _ASSERT(numElements > 0);

    fd << tab() << "<CompositeTypeStyle>" << std::endl; // NOXLATE
    inctab();

    for (int i=0; i<numElements; ++i)
    {
        CompositeRule* compositeRule = dynamic_cast<CompositeRule*>(ruleCollection->GetAt(i));
        if (compositeRule)
            IOCompositeRule::Write(fd, compositeRule, version);
    }

    // write any previously found unknown XML
    if (!compositeTypeStyle->GetUnknownXml().empty())
        fd << tab() << toCString(compositeTypeStyle->GetUnknownXml()) << std::endl;

    dectab();
    fd << tab() << "</CompositeTypeStyle>" << std::endl; // NOXLATE
}
